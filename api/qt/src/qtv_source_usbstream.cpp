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

#include "./internal/source/qsource_usbstream.h"
#include <core/qtv_service.h>
#include <ui/base/qtv_ui_define.h>
#include <stdio.h>

/* qi_usbstream_properties */

qusbstream_properties::qusbstream_properties()
{
	memset(&m_param, 0, sizeof(qusbstream_param));
	m_param.usbstream.mode = qpathmode_file_ts;
	m_param.source.source = stream_source_usb;
	m_param.source.stream.usb.mode = stream_remux;
	m_param.source.stream.usb.pcrmode = pcr_disable;
	m_param.source.stream.usb.usb_flags = USB_EN_ASYNCBUFFER;
	resetProperties();
	refreshProperties();
}

qusbstream_properties::~qusbstream_properties()
{

}

void qusbstream_properties::resetProperties()
{
	m_param.usbstream.filter = qfilter_disable;
	if (m_param.usbstream.mode != qpathmode_null)
	{
		memset(&m_param.usbstream.url[0], 0, _UIPROP_STRBUF_LEN_MAX);
		setValid(false);
	}
	else setValid(true);
}

void qusbstream_properties::refreshProperties()
{
	clearProperties();
	m_param.usbstream.filter = qfilter_disable;
	if (m_param.usbstream.mode == qpathmode_null)
	{
		insertProperties(_ui_struct(qtv_usbstream_param_test), (uint8_t*)&m_param.usbstream);
		setValid(true);
	}
	else if (m_param.usbstream.mode == qpathmode_passthrough)
	{
		insertProperties(_ui_struct(qtv_usbstream_passthrough), (uint8_t*)&m_param.usbstream);
		setValid(false);
	}
	else
	{
		insertProperties(_ui_struct(qtv_usbstream_param), (uint8_t*)&m_param.usbstream);
		insertProperties(_ui_struct(qtv_usbstream_pcronly),(uint8_t*)&m_param.source.stream.usb);
		setValid(false);
	}
}

void qusbstream_properties::insertProperty(const Pui_prop_item uiprop, uint8_t* raw)
{
	if (ui_props_is_struct_param(qtv_usbstream_param, mode, uiprop) ||
		ui_props_is_struct_param(qtv_usbstream_param_test, mode, uiprop))
		qi_properties_base::insertProperty(new qi_property_keyvalue(this, uiprop, raw));
	else if (ui_props_is_struct_param(qtv_usbstream_param, url, uiprop))
		qi_properties_base::insertProperty(new qi_property_path(this,uiprop,m_param.usbstream.mode, raw));
	else qi_properties_base::insertProperty(uiprop, raw);
}

bool qusbstream_properties::checkPropertiesValid()
{
	vatek_result nres = checkUSBStreamValid(this);
	if (is_vatek_success(nres))return true;
	return false;
}

vatek_result qusbstream_properties::checkUSBStreamValid(qusbstream_properties* props)
{
	Pqtv_usbstream_param param = &props->_param()->usbstream;
	vatek_result nres = vatek_badparam;
	if (param->mode == qpathmode_null)
	{
		props->_param()->source.stream.usb.mode = stream_passthrogh;
		nres = vatek_success;
	}
	else
	{
		if (strlen(&param->url[0]))
		{
			if (param->mode == qpathmode_file_ts || param->mode == qpathmode_passthrough)
			{
				FILE* ffile = fopen(&param->url[0], "rb");
				if (ffile)
				{
					nres = vatek_success;
					fclose(ffile);
				}
			}
			else if (param->mode == qpathmode_url_udp)
			{
				if (strstr(&param->url[0], "udp://"))nres = vatek_success;
			}

			if (is_vatek_success(nres))
			{
				if (param->mode == qpathmode_passthrough)
				{
					props->_param()->source.stream.usb.mode = stream_passthrogh;
					props->_param()->source.stream.usb.pcrmode = pcr_disable;
				}
				else props->_param()->source.stream.usb.mode = stream_remux;
			}
		}
	}
	return nres;
}

/* qi_usbstream_factory */

qusbstream_source_factory::qusbstream_source_factory() :
	m_name(QSOURCE_USBSTREAM_NAME)
{

}

qusbstream_source_factory::~qusbstream_source_factory()
{

}

bool qusbstream_source_factory::checkSupported(qtvServiceBase* sevice)
{
	return true;
}

vatek_result qusbstream_source_factory::createProperties(qtvServiceBase* service, qtvPropertyCollection** properties)
{
	vatek_result nres = vatek_unsupport;
	if (service->_mode() == qservice_transfrom && 
		service->_device()->_chip_bus() == DEVICE_BUS_USB)
	{
		*properties = new qusbstream_properties();
		nres = vatek_success;
	}
	return nres;
}

vatek_result qusbstream_source_factory::checkProperties(qtvPropertyCollection* properties)
{
	vatek_result nres = vatek_badparam;
	qusbstream_properties* qusb = static_cast<qusbstream_properties*>(properties);
	if (qusb)nres = qusbstream_properties::checkUSBStreamValid(qusb);
	return nres;
}

vatek_result qusbstream_source_factory::createSource(qtvServiceBase* service, qtvPropertyCollection* properties, qtvSourceBase** source)
{
	vatek_result nres = vatek_unsupport;
	qusbstream_properties* qprops = static_cast<qusbstream_properties*>(properties);
	if (qprops)
	{
		*source = new qusbstream_source(service, this, qprops);
		nres = vatek_success;
	}
	return nres;
}

/* qusbstream_source */

qusbstream_source::qusbstream_source(qtvServiceBase* service, qtvSourceFactory* factory, qusbstream_properties* properties) :
	m_status(qstatus_idle),
	m_factory(factory),
	m_service(service),
	m_properties(properties),
	m_hstream(NULL)
{

}

qusbstream_source::~qusbstream_source()
{
	close();
	delete m_properties;
}

vatek_result qusbstream_source::open()
{
	vatek_result nres = vatek_badstatus;
	if (m_status == qstatus_idle)
	{
		nres = m_factory->checkProperties(m_properties);
		if (is_vatek_success(nres))m_status = qstatus_lock;
	}
	return nres;
}

vatek_result qusbstream_source::start()
{
	vatek_result nres = vatek_badstatus;
	if (m_status == qstatus_lock)
	{
		Pqtv_usbstream_param param = &m_properties->_param()->usbstream;
		if (param->mode == qpathmode_url_udp)
		{
			nres = cross_stream_open_udp(&param->url[0], &m_hstream);
		}
		else if (param->mode == qpathmode_file_ts || param->mode == qpathmode_passthrough)
			nres = cross_stream_open_file(&param->url[0], &m_hstream);
		else if (param->mode == qpathmode_null)
		{
			uint32_t bitrate = modulator_param_get_bitrate(&m_service->_output_param()->modulation);
			//nres = cross_stream_open_test(bitrate, &m_hstream);
			nres = cross_stream_open_mux(&m_service->_output_param()->modulation, &m_hstream);
		}

		if (is_vatek_success(nres))
		{
			nres = cross_stream_start(m_hstream);
			if (!is_vatek_success(nres))
			{
				cross_stream_close(m_hstream);
				m_hstream = NULL;
			}
		}
		if (is_vatek_success(nres))m_status = qstatus_running;
	}
	return nres;
}

vatek_result qusbstream_source::polling()
{
	vatek_result nres = vatek_badstatus;
	if (m_status == qstatus_running)
	{
		nres = vatek_success;
	}
	return nres;
}

void qusbstream_source::stop()
{
	if (m_status == qstatus_running)
	{
		cross_stream_stop(m_hstream);
		cross_stream_close(m_hstream);
		m_status = qstatus_lock;
	}
	m_hstream = NULL;
}

void qusbstream_source::close()
{
	stop();
	m_status = qstatus_idle;
}

vatek_result qusbstream_source::getStreamPackets(uint8_t** slice)
{
	vatek_result nres = vatek_badstatus;
	if (m_status == qstatus_running)
	{
		nres = cross_stream_get_slice(m_hstream, slice);
	}
	return nres;
}