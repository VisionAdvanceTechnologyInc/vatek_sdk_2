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

#include "./internal/qinternal_transform.h"
#include "./internal/qinternal_core.h"
#include "./internal/qinternal_mux.h"

#include "./internal/source/qsource_base.h"
#include "./internal/source/qsource_bridge.h"
#include "./internal/source/qsource_usbstream.h"
#include <qDebug>
/* qi_transform_handle */

class transform_handle : public qi_servicehandle
{
public:
	transform_handle(qtvChartDataCaptureService* chartdata,
		hvatek_transform htr,
		hvatek_chip hchip,
		Pqi_transform_param param,
		qtvSourceTransform* source);
	~transform_handle();

protected:
	hvatek_chip m_hchip;
	hvatek_transform m_htr;
	modulator_param m_mod;
	qtvSourceTransform* m_source;
	Pqi_transform_param m_param;
	qtvChartDataCaptureService* m_chartdata;
	Ptransform_info m_info;

	vatek_result startService() override;
	vatek_result pollingService() override;
	void updateServiceTick() override;
	void stopService() override;
	Pbroadcast_info getServiceInfo() override;

private:
	vatek_result startTransform();
	void stopTransform();
};

/* qi_service_transform */

qi_service_transform::qi_service_transform(qtvDevice* device, hvatek_transform htr) :
	qtvServiceTransform(device),
	m_output(device),
	m_htr(htr),
	m_enumcontext(NULL)
{
	resetParam();
}

qi_service_transform::~qi_service_transform()
{
	stop();
	vatek_transform_close(m_htr);
	resetEnumContext();
}

Pbroadcast_info qi_service_transform::_broadcast_detail()
{
	Ptransform_info ptrinfo = vatek_transform_get_info(m_htr);
	return &ptrinfo->info;
}


vatek_result qi_service_transform::setLocalDefault(qtvDTVProperties* locale)
{
	vatek_result nres = vatek_badstatus;
	if (m_status == qstatus_idle)
	{
		m_output.resetParam(locale->_param()->modulation,locale->_param()->bw_sb, locale->_param()->frequency);
		nres = vatek_success;
	}
	return nres;
}

vatek_result qi_service_transform::startEnumContext(qtvSourceBase* source, int32_t timeout)
{
	vatek_result nres = vatek_badstatus;
	if (_status() == qstatus_idle)
	{
		qtvSourceTransform* trsource = qobject_cast<qtvSourceTransform*>(source);
		nres = vatek_unsupport;
		if(trsource && trsource->_status() == qstatus_lock)
		{
			resetParam();
			resetEnumContext();
			m_param.mode = trmode_enum;
			nres = setParamSource(source);
			if (is_vatek_success(nres))
			{
				Ptransform_enum penum = &m_param.param.trenum;
				penum->timeout = timeout;
				changedStatus(qstatus_ready);
				nres = startHandle(new transform_handle(m_chartdata,m_htr,m_device->_handle(),&m_param, trsource));
			}
			if (!is_vatek_success(nres))resetParam();
		}
	}
	return nres;
}

qtvMuxChannel* qi_service_transform::getEnumContext()
{
	return m_enumcontext;
}

void qi_service_transform::resetEnumContext()
{
	if (m_enumcontext)delete m_enumcontext;
	m_enumcontext = NULL;
}

vatek_result qi_service_transform::configFilter(Ptransform_broadcast pbc, qtvMuxServiceBase* muxhandle)
{
	qtvMuxServiceChannel* muxch = qobject_cast<qtvMuxServiceChannel*>(muxhandle);
	vatek_result nres = vatek_success;
	if (muxch)
	{
		QList<uint16_t> pids;
		qtvMuxChannel* pch = muxch->_channel();
		int32_t i = 0;
		int32_t j = 0;

		for (i = 0; i < pch->_programs().count(); i++)
		{
			qtvMuxProgram* pprog = pch->_programs().at(i);
			if (!pids.contains(pprog->_pcr_pid()))
				pids.append(pprog->_pcr_pid());

			for (j = 0; j < pprog->_streams().count(); j++)
			{
				qtvMuxStream* pstream = pprog->_streams().at(j);
				if (!pids.contains(pstream->_pid()))
					pids.append(pstream->_pid());
			}
		}
		pbc->filters.filter_nums = pids.count();
		for (i = 0; i < pids.count(); i++)
		{
			pbc->filters.filters[i].orange_pid = pids[i];
			pbc->filters.filters[i].replace_pid = PID_FILTER_NOT_REPLACE;
		}
		nres = muxhandle->applyDevice(m_device);
	}
	else pbc->filters.filter_nums = 0;
	return nres;
}

vatek_result qi_service_transform::startBroadcast(qtvSourceBase* source, qtvMuxServiceBase* muxservice)
{
	vatek_result nres = vatek_badstatus;
	if (_status() == qstatus_idle)
	{
		qtvSourceTransform* trsource = qobject_cast<qtvSourceTransform*>(source);
		nres = vatek_unsupport;
		if (trsource && trsource->_status() == qstatus_lock)
		{
			resetParam();
			m_param.mode = trmode_broadcast;
			nres = setParamSource(source);
			if (is_vatek_success(nres))
			{
				Ptransform_broadcast pbc = &m_param.param.trbc;
				memcpy(&pbc->modulator, &m_output._param()->modulation, sizeof(modulator_param));
				m_param.r2param.freqkhz = m_output._param()->r2param.freqkhz;
				m_param.r2param.mode = m_output._param()->r2param.mode;

				nres = configFilter(pbc, muxservice);
				if (is_vatek_success(nres))
				{
					changedStatus(qstatus_ready);
					nres = startHandle(new transform_handle(m_chartdata,m_htr, m_device->_handle(), &m_param, trsource));
				}
			}
			if (!is_vatek_success(nres))resetParam();
		}
	}
	return nres;
}

vatek_result qi_service_transform::pollingRunning(Pbroadcast_info* pinfo)
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
			else if (info->status == bcstatus_finish)nres = vatek_success;
		}
		*pinfo = info;
	}
	else VWAR("qi_service_transform polling not running : %d", _status());
	return nres;
}

vatek_result qi_service_transform::stop()
{
	vatek_result nres = vatek_badstatus;
	if (_status() == qstatus_running)
	{
		stopHandle();
		if (m_param.mode == trmode_enum)
		{
			hmux_channel hchannel = NULL;
			nres = vatek_transform_enum_get_context(m_htr, &hchannel);
			if (is_vatek_success(nres))
			{
				m_enumcontext = new qi_muxchannel(hchannel);
			}
		}
		resetParam();
		changedStatus(qstatus_idle);
		nres = vatek_success;
	}
	return nres;
}

void qi_service_transform::resetParam()
{
	memset(&m_param, 0, sizeof(qi_transform_param));
}

vatek_result qi_service_transform::setParamSource(qtvSourceBase* source)
{
	Pqtv_source_param psource = source->_param();
	vatek_result nres = vatek_badparam;
	if (m_param.mode == trmode_enum)
	{
		Ptransform_enum penum = &m_param.param.trenum;
		nres = vatek_success;
		if (psource->source == stream_source_tsin)
			memcpy(&penum->stream, &psource->stream, sizeof(tsin_param));
		else if (psource->source == stream_source_usb)
			memcpy(&penum->stream, &psource->stream, sizeof(usb_param));
		else nres = vatek_unsupport;
		if (is_vatek_success(nres))penum->source = psource->source;
	}
	else if (m_param.mode == trmode_enum)
	{
		Ptransform_capture pcapture = &m_param.param.trcapture;
		nres = vatek_success;
		if (psource->source == stream_source_tsin)
			memcpy(&pcapture->stream, &psource->stream, sizeof(tsin_param));
		else if (psource->source == stream_source_usb)
			memcpy(&pcapture->stream, &psource->stream, sizeof(usb_param));
		else nres = vatek_unsupport;
		if (is_vatek_success(nres))pcapture->source = psource->source;
	}
	else if (m_param.mode == trmode_broadcast)
	{
		Ptransform_broadcast pbc = &m_param.param.trbc;
		nres = vatek_success;
		if (psource->source == stream_source_tsin)
			memcpy(&pbc->stream, &psource->stream, sizeof(tsin_param));
		else if (psource->source == stream_source_usb)
			memcpy(&pbc->stream, &psource->stream, sizeof(usb_param));
		else nres = vatek_unsupport;
		if (is_vatek_success(nres))pbc->source = psource->source;
	}
	return nres;
}

/* qi_transform_sources */

qi_transform_sources::qi_transform_sources()
{
	m_sources.append(new qtsin_source_factory());
	m_sources.append(new qusbstream_source_factory());
}

qi_transform_sources::~qi_transform_sources()
{
	while (m_sources.count())
	{
		delete m_sources.first();
		m_sources.removeFirst();
	}
}

qi_transform_sources& qi_transform_sources::getInstance()
{
	static qi_transform_sources oneInstance;
	return oneInstance;
}

/* qtvServiceTransform */

QList<qtvSourceFactory*>& qtvServiceTransform::getSupportedSources()
{
	return qi_transform_sources::getInstance()._sources();
}

/* transform_handle */

transform_handle::transform_handle(qtvChartDataCaptureService* chartdata, hvatek_transform htr, hvatek_chip hchip, Pqi_transform_param param, qtvSourceTransform* source) :
	m_hchip(hchip),
	m_htr(htr),
	m_chartdata(chartdata),
	m_param(param),
	m_source(source),
	m_info(NULL),
	qi_servicehandle(hchip)
{
	modulator_param_reset(modulator_dvb_t, &m_mod);
}

transform_handle::~transform_handle()
{

}

vatek_result transform_handle::startService()
{
	vatek_result nres = startTransform();
	if (is_vatek_success(nres))
	{
		nres = m_source->start();
		if (!is_vatek_success(nres))stopTransform();
	}
	return nres;
}

vatek_result transform_handle::pollingService()
{
	vatek_result nres = vatek_transform_polling(m_htr, &m_info);
	if(is_vatek_success(nres))
		nres = m_source->polling();
	if (is_vatek_success(nres))
	{
		if (m_source->_source_mode() == qsource_usbstream)
		{
			uint32_t pktnums = 0;
			uint8_t* pslice = NULL;
			qtvSourceUSBStream* usbstream = qobject_cast<qtvSourceUSBStream*>(m_source);
			nres = vatek_transform_get_packets(m_htr, &pktnums);

			if (is_vatek_success(nres))
			{
				if (pktnums < CHIP_STREAM_SLICE_PACKET_NUMS)
				{
					if (pktnums)
						nres = vatek_transform_commit_packets(m_htr);
					cross_os_sleep(1);
				}
				else
				{
					int32_t slicenums = 0;
					while (pktnums >= CHIP_STREAM_SLICE_PACKET_NUMS)
					{
						nres = usbstream->getStreamPackets(&pslice);
						if (nres > vatek_success)
						{
							nres = vatek_device_stream_write(m_hchip, pslice, CHIP_STREAM_SLICE_LEN);
							if (is_vatek_success(nres))
								pktnums -= CHIP_STREAM_SLICE_PACKET_NUMS;
							slicenums++;
						}
						else break;
						if (!is_vatek_success(nres))break;
					}
					if (is_vatek_success(nres))
						nres = vatek_transform_commit_packets(m_htr);
				}
			}
		}
	}
	return nres;
}

void transform_handle::updateServiceTick()
{
	m_info = vatek_transform_get_info(m_htr);
	m_chartdata->pushBroadcast(&m_info->info);
}

void transform_handle::stopService()
{
	m_source->stop();
	stopTransform();
}

Pbroadcast_info transform_handle::getServiceInfo()
{
	if(m_info)return &m_info->info;
	return NULL;
}

vatek_result transform_handle::startTransform()
{
	vatek_result nres = vatek_unsupport;
	Pmodulator_param pmod = NULL;
	if (m_param->mode == trmode_enum)
	{
		nres = vatek_transform_start_enum(m_htr, &m_param->param.trenum);
		pmod = &m_mod;
	}
	else if (m_param->mode == trmode_broadcast)
	{
		uint32_t bitrate = modulator_param_get_bitrate(&m_param->param.trbc.modulator);
		qDebug() << bitrate;
		nres = vatek_transform_start_broadcast(m_htr, &m_param->param.trbc, m_param->r2param);
		pmod = &m_param->param.trbc.modulator;
	}

	if (is_vatek_success(nres))
	{
		nres = vatek_transform_polling(m_htr, &m_info);
		if (is_vatek_success(nres))
		{
			broadcast_status bcstatus = m_info->info.status;
			if (bcstatus == bcstatus_broadcast || bcstatus == bcstatus_wait_source || bcstatus == bcstatus_finish)
			{
				if (m_source->_source_mode() == qsource_usbstream)
					nres = vatek_device_stream_start(m_hchip, pmod , m_param->param.trbc.stream.usb.mode);
			}
			else nres = vatek_badstatus;
		}
		if (!is_vatek_success(nres))vatek_transform_stop(m_htr);
	}
	return nres;
}

void transform_handle::stopTransform()
{
	vatek_result nres = vatek_success;
	if (m_source->_source_mode() == qsource_usbstream)
		nres = vatek_device_stream_stop(m_hchip);
	if (!is_vatek_success(nres))VWAR("vatek_device_stream_stop fail : %d", nres);
	nres = vatek_transform_stop(m_htr);
	if (!is_vatek_success(nres))VWAR("vatek_transform_stop fail : %d", nres);
	m_info = NULL;
}
