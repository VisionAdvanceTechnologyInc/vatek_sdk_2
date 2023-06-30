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

#include "./internal/qinternal_broadcast.h"
#include "./internal/qinternal_core.h"
#include "./internal/source/qsource_base.h"
#include "./internal/source/qsource_bridge.h"

class broadcast_handle : public qi_servicehandle
{
public:
	broadcast_handle(qtvChartDataCaptureService* chartdata,
					hvatek_broadcast hbc,
					hvatek_chip hchip,
					Pbroadcast_param param,
					qtvSourceBroadcast* source,
					r2_param r2param);
	~broadcast_handle();

protected:
	hvatek_broadcast m_hbc;
	qtvSourceBroadcast* m_source;
	Pbroadcast_param m_param;
	qtvChartDataCaptureService* m_chartdata;
	Pbroadcast_info m_info;
	uint32_t m_freqkhz;
	r2_param m_r2param;

	vatek_result startService() override;
	vatek_result pollingService() override;
	void updateServiceTick() override;
	void stopService() override;
	Pbroadcast_info getServiceInfo() override;

private:
	vatek_result startBroadcast();
	void stopBroadcast();
};

/* qi_service_broadcast */

qi_service_broadcast::qi_service_broadcast(qtvDevice* device, hvatek_broadcast hbc) :
	qtvServiceBroadcast(device),
	m_output(device),
	m_hbc(hbc)
{
	broadcast_param_reset(device->_chip_info()->chip_module, &m_param);
}

qi_service_broadcast::~qi_service_broadcast()
{
	stop();
	vatek_broadcast_close(m_hbc);
	m_hbc = NULL;
}

vatek_result qi_service_broadcast::setLocalDefault(qtvDTVProperties* locale)
{
	vatek_result nres = vatek_badstatus;
	if (m_status == qstatus_idle)
	{
		m_output.resetParam(locale->_param()->modulation,locale->_param()->bw_sb, locale->_param()->frequency);
		nres = vatek_success;
	}
	return nres;
}

vatek_result qi_service_broadcast::startBroadcast(qtvSourceBase* source, qtvMuxServiceBase* muxservice)
{
	vatek_result nres = vatek_badstatus;
	if (_status() == qstatus_idle)
	{
		qtvSourceBroadcast* bcsource = qobject_cast<qtvSourceBroadcast*>(source);
		nres = vatek_unsupport;
		if (bcsource && bcsource->_status() == qstatus_lock)
		{
			nres = setParam(bcsource);
			if (is_vatek_success(nres))
				nres = muxservice->applyDevice(m_device);

			if (is_vatek_success(nres))
			{
				changedStatus(qstatus_ready);
				nres = startHandle((qtvServiceHandle*)new broadcast_handle(m_chartdata, m_hbc, m_device->_handle(), &m_param, bcsource, m_output._param()->r2param));
			}
		}
	}
	return nres;
}

vatek_result qi_service_broadcast::pollingRunning(Pbroadcast_info* pinfo)
{
	vatek_result nres = vatek_badstatus;
	if (_status() == qstatus_running)
	{
		Pbroadcast_info info = NULL;
		nres = m_handle->polling(&info);
		if (is_vatek_success(nres))
		{
			nres = vatek_badstatus;
			if (is_bcstatus_running(info->status))nres = vatek_success;
		}
		*pinfo = info;
	}
	else VWAR("qi_service_broadcast polling not running : %d", _status());
	return nres;
}

vatek_result qi_service_broadcast::stop()
{
	vatek_result nres = vatek_badstatus;
	if (_status() == qstatus_running)
	{
		stopHandle();
		changedStatus(qstatus_idle);
		nres = vatek_success;
	}
	return nres;
}

vatek_result qi_service_broadcast::setParam(qtvSourceBroadcast* psource)
{
	vatek_result nres = vatek_badparam;
	Pqtv_source_param param = psource->_param();
	if (param->source == stream_source_encoder)
	{
		memcpy(&m_param.enc, &param->stream.encoder, sizeof(encoder_param));
		memcpy(&m_param.mod, &m_output._param()->modulation, sizeof(modulator_param));
		//memcpy(&m_param.mux.bitrate, &param->stream.mux.bitrate, sizeof(uint32_t));
		nres = vatek_success;
	}
	return nres;
}

/* qi_broadcast_sources */

qi_broadcast_sources::qi_broadcast_sources()
{
	m_sources.append(new qencoder_source_factory());
	m_sources.append(new qbridge_encoder_factory());
}

qi_broadcast_sources::~qi_broadcast_sources()
{
	while (m_sources.count())
	{
		delete m_sources.first();
		m_sources.removeFirst();
	}
}

qi_broadcast_sources& qi_broadcast_sources::getInstance()
{
	static qi_broadcast_sources oneInstance;
	return oneInstance;
}

/* qtvServiceBroadcast */

QList<qtvSourceFactory*>& qtvServiceBroadcast::getSupportedSources()
{
	return qi_broadcast_sources::getInstance()._sources();
}

/* broadcast_handle */

broadcast_handle::broadcast_handle(qtvChartDataCaptureService* chartdata, 
								hvatek_broadcast hbc, 
								hvatek_chip hchip, 
								Pbroadcast_param param, 
								qtvSourceBroadcast* source, 
								r2_param r2param) :
	qi_servicehandle(hchip),
	m_hbc(hbc),
	m_param(param),
	m_chartdata(chartdata),
	m_r2param(r2param),
	m_source(source),
	m_info(NULL)
{

}

broadcast_handle::~broadcast_handle()
{

}

vatek_result broadcast_handle::startService()
{
	vatek_result nres = startBroadcast();
	if (is_vatek_success(nres))
	{
		nres = m_source->start();
		if (!is_vatek_success(nres))stopBroadcast();
	}
	return nres;
}

vatek_result broadcast_handle::pollingService()
{
	vatek_result nres = vatek_broadcast_polling(m_hbc, &m_info);
	if (is_vatek_success(nres))
	{
		nres = m_source->polling();
	}
	return nres;
}

void broadcast_handle::updateServiceTick()
{
	Pbroadcast_info pinfo = vatek_broadcast_get_info(m_hbc);
	m_chartdata->pushBroadcast(pinfo);
}

void broadcast_handle::stopService()
{
	m_source->stop();
	stopBroadcast();
}

Pbroadcast_info broadcast_handle::getServiceInfo()
{
	return m_info;
}

vatek_result broadcast_handle::startBroadcast()
{
	vatek_result nres = vatek_broadcast_start(m_hbc, m_param,NULL, m_r2param);
	if (is_vatek_success(nres))
	{
		nres = vatek_broadcast_polling(m_hbc, &m_info);
		if (is_vatek_success(nres))
		{
			if (m_info->status == bcstatus_wait_source ||
				m_info->status == bcstatus_broadcast)
			{

			}
			else nres = vatek_badstatus;
		}
		if (!is_vatek_success(nres))stopBroadcast();
	}
	return nres;
}

void broadcast_handle::stopBroadcast()
{
	vatek_result nres = vatek_broadcast_stop(m_hbc);
	if (!is_vatek_success(nres))
		VWAR("broadcast stop fail : %d", nres);
}
