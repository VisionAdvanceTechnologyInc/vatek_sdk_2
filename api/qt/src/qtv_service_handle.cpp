//----------------------------------------------------------------------------
//
// Vision Advance Technology - Software Development Kit
// Copyright (c) 2014-2022, Vision Advance Technology Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//

#include "internal/qinternal_core.h"

qi_servicehandle::qi_servicehandle(hvatek_chip hchip) :
	m_hchip(hchip),
	m_hthread(NULL),
	m_hlock(NULL),
	m_calibrationcount(0),
	m_updatetick(0),
	m_status(qstatus_idle),
	m_encalibration(0)
{
	Pchip_info pinfo = vatek_device_get_info(hchip);
	if (!is_vatek_success(cross_os_create_mutex(&m_hlock)))
		VERR("create mutex fail");

	if (pinfo->peripheral_en & PERIPHERAL_CALIBRATION)
	{
		m_encalibration |= QCALIBRATION_EN;
		if (pinfo->peripheral_en & PERIPHERAL_FINTEKR2)
			m_encalibration |= QCALIBRATION_R2;
	}
	memset(&m_calibration, 0, sizeof(calibration_param));
}

qi_servicehandle::~qi_servicehandle()
{
	stop();
	cross_os_free_mutex(m_hlock);
}

vatek_result qi_servicehandle::start()
{
	vatek_result nres = vatek_badstatus;
	if (m_status == qstatus_idle)
	{
		nres = vatek_device_calibration_load(m_hchip, &m_calibration);
		if(is_vatek_success(nres))
			nres = startService();
		if (is_vatek_success(nres))
		{
			nres = vatek_hwfail;
			m_status = qstatus_running;
			m_updatetick = 0;
			m_calibrationcount = 0;
			m_hthread = cross_os_create_thread(qi_servicehandle::service_handler, this);
			if (!m_hthread)m_status = qstatus_idle;
			else nres = vatek_success;
			if (!is_vatek_success(nres))stopService();
		}
	}
	return nres;
}

uint32_t qi_servicehandle::calibratiobCheck()
{
	return m_encalibration;
}

vatek_result qi_servicehandle::calibrationSet(Pcalibration_param pcalibration)
{
	vatek_result nres = vatek_badstatus;
	if (m_status == qstatus_running)
	{
		if (!m_encalibration)nres = vatek_unsupport;
		else
		{
			internalLock();
			m_calibrationcount++;
			memcpy(&m_calibration, pcalibration, sizeof(calibration_param));
			internalUnlock();
			nres = vatek_success;
		}
	}
	return nres;
}

vatek_result qi_servicehandle::calibrationGet(Pcalibration_param pcalibration)
{
	vatek_result nres = vatek_unsupport;
	uint32_t val = 0;

	if (m_encalibration)
	{
		memcpy(pcalibration, &m_calibration, sizeof(calibration_param));
		
		if (m_calibration.r2_power == 0) {
			nres = vatek_chip_read_memory(m_hchip, HALREG_EXT_R2_GAIN, &val); // read PA_Gain
			pcalibration->r2_power = val;
		}

		nres = vatek_success;
	}
	return nres;
}

vatek_result qi_servicehandle::calibrationSave()
{
	vatek_result nres = vatek_badstatus;
	if (!m_encalibration)nres = vatek_unsupport;
	else
	{
		if (m_calibrationcount)
		{
			internalLock();
			nres = vatek_device_calibration_save(m_hchip, &m_calibration);
			internalUnlock();
			m_calibrationcount = 0;
		}
		else nres = vatek_success;
	}
	return nres;
}

vatek_result qi_servicehandle::polling(Pbroadcast_info* pinfo)
{
	vatek_result nres = vatek_badstatus;
	if (m_status != qstatus_idle)
	{
		if (pinfo)*pinfo = getServiceInfo();
		nres = vatek_success;
	}
	return nres;
}

void qi_servicehandle::stop()
{
	if (m_status != qstatus_idle)
	{
		m_status = qstatus_stop;
		while (m_status == qstatus_stop)
			cross_os_sleep(1);
		stopService();
		cross_os_free_thread(m_hthread);
	}
	m_status = qstatus_idle;
}

void qi_servicehandle::internalLock()
{
	cross_os_lock_mutex(m_hlock);
}

void qi_servicehandle::internalUnlock()
{
	cross_os_release_mutex(m_hlock);
}

void qi_servicehandle::service_handler(Pcross_thread_param param)
{
	qi_servicehandle* core = (qi_servicehandle*)param->userparam;
	vatek_result nres = vatek_success;
	broadcast_status bcstatus = bcstatus_idle;
	Pbroadcast_info pinfo = core->getServiceInfo();
	uint32_t ncalibration = 0;

	while (core->m_status == qstatus_running)
	{
		if (is_vatek_success(nres))
		{
			nres = core->pollingService();
			if (is_vatek_success(nres))
			{
				uint32_t eclipse = cross_os_get_tick_ms() - core->m_updatetick;
				if (eclipse > 1000)
				{
					core->updateServiceTick();
					core->m_updatetick = cross_os_get_tick_ms();

					if (ncalibration != core->m_calibrationcount)
					{
						nres = vatek_device_calibration_apply(core->m_hchip, &core->m_calibration);
						if (is_vatek_success(nres))
							ncalibration = core->m_calibrationcount;
					}
				}
			}
			if (!is_vatek_success(nres))VWAR("service_handler fail : %d", nres);
		}
		else
		{
			if (pinfo->status != bcstatus_fail_unknown)
				pinfo->status = bcstatus_fail_unknown;
			cross_os_sleep(1);
		}
	}
	core->m_status = qstatus_idle;
	param->result = nres;
}

