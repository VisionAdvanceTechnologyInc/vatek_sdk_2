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

#include "./internal/qinternal_properties.h"

#include <core/ui/ui_props/ui_props_modulator.h>
#include <core/ui/ui_props/ui_props_stream.h>
#include <core/ui/ui_props/ui_props_chip.h>

#include <ui/base/qtv_ui_define.h>


/* qi_properties_base */

qi_properties_base::qi_properties_base() :
	qtvPropertyCollection()
{

}

qi_properties_base::~qi_properties_base()
{

}

void qi_properties_base::raisePropertiesChanged()
{
	resetProperties();
	refreshProperties();
	emit notifyPropertiesChanged();
}

void qi_properties_base::raisePropertyEdited(qtvProperty* prop)
{
	setValid(checkPropertiesValid());
}

/* qi_output_properties */

qi_output_properties::qi_output_properties(qtvDevice* device) :
	m_device(device)
{
	m_param.r2param.freqkhz = 473000;
	m_param.r2param.mode = r2_cntl_path_0;
	m_param.modulation.type = modulator_dvb_t;
	resetProperties();
	refreshProperties();
}

void qi_output_properties::resetParam(modulator_type type,uint32_t bw_sb, uint32_t frequency)
{
	int32_t freqmod = frequency % 1000;
	m_param.r2param.freqkhz = frequency - freqmod;
	modulator_param_reset(type, &m_param.modulation);
	m_param.modulation.type = type;
	m_param.modulation.bandwidth_symbolrate = bw_sb;

	if (freqmod)
	{
		m_param.modulation.ifmode = ifmode_iqoffset;
		m_param.modulation.iffreq_offset = freqmod;
	}
	refreshProperties();
}

void qi_output_properties::resetProperties()
{
	modulator_type type = m_param.modulation.type;
	memset(&m_param, 0, sizeof(qtv_output_param));
	modulator_param_reset(type, &m_param.modulation);
	if (type == modulator_dtmb) 
		m_param.r2param.freqkhz = 474000;
	else 
		m_param.r2param.freqkhz = 473000;

	m_param.r2param.mode = r2_cntl_path_0;
}

void qi_output_properties::refreshProperties()
{
	clearProperties();
	insertProperties(_ui_struct(qtv_output_param), (uint8_t*)&m_param);
	insertProperties(_ui_struct(modulator_param), (uint8_t*)&m_param.modulation);
	insertProperties(modulator_param_get_ui_props(m_param.modulation.type), (uint8_t*)&m_param.modulation.mod.raw_byte);
}

void qi_output_properties::insertProperty(const Pui_prop_item uiprop, uint8_t* raw)
{
	if (ui_props_is_struct_param(modulator_param, type, uiprop))
		qi_properties_base::insertProperty(new qi_property_keyvalue(this, uiprop, raw));
	else qi_properties_base::insertProperty(uiprop, raw);
}

/* qi_modulation_properties */

qi_modulation_properties::qi_modulation_properties(Pmodulator_param pmod) :
	m_param(pmod)
{
	refreshProperties();
}

void qi_modulation_properties::resetProperties()
{
	modulator_param_reset(m_param->type, m_param);
}

void qi_modulation_properties::refreshProperties()
{
	clearProperties();
	insertProperties(_ui_struct(modulator_param), (uint8_t*)m_param);
	insertProperties(modulator_param_get_ui_props(m_param->type), (uint8_t*)&m_param->mod.raw_byte);
}

void qi_modulation_properties::insertProperty(const Pui_prop_item uiprop, uint8_t* raw)
{
	if (ui_props_is_struct_param(modulator_param, type, uiprop))
		qi_properties_base::insertProperty(new qi_property_keyvalue(this, uiprop, raw));
	else qi_properties_base::insertProperty(uiprop, raw);
}

/* qi_source_properties */

qi_source_properties::qi_source_properties(qtvDevice* device, stream_source source, qi_properties_mode mode) :
	m_device(device),
	m_mode(mode),
	m_inited(0)
{
	m_param.source = source;
	resetProperties();
	refreshProperties();
}

void qi_source_properties::resetProperties()
{
	stream_source source = m_param.source;
	int32_t isreset = 0;
	if (!m_inited)
	{
		isreset = 1;
		m_inited = 1;
	}else if(source != stream_source_encoder)isreset = 1;

	if(isreset)
	{
		memset(&m_param, 0, sizeof(qtv_source_param));
		stream_source_reset(m_device->_chip_info()->chip_module, source, (uint8_t*)&m_param.stream);
		m_param.filter = qfilter_disable;
		m_param.source = source;
	}
	else
	{
		encoder_mode mode = m_param.stream.encoder.mode;
		stream_source_reset(m_device->_chip_info()->chip_module, source, (uint8_t*)&m_param.stream);
		m_param.stream.encoder.mode = mode;
	}
}

void qi_source_properties::refreshProperties()
{
	clearProperties();
	
	insertProperties(stream_source_get_ui_props(m_param.source), (uint8_t*)&m_param.stream);
	if (m_param.source == stream_source_encoder)
	{
		Pencoder_param penc = &m_param.stream.encoder;

		if (m_mode != qproperties_bridge)
		{
			if(penc->mode == encoder_source_vi_0)
				insertProperties(_ui_struct(vi_param), (uint8_t*)&penc->viparam);
			insertProperties(_ui_struct(media_video), (uint8_t*)&penc->video);
			insertProperties(_ui_struct(media_audio), (uint8_t*)&penc->audio);
		}
		insertProperties(_ui_struct(quality_param), (uint8_t*)&penc->quality);
	}

	if (m_mode == qproperties_normal)
		insertProperties(_ui_struct(qtv_source_param), (uint8_t*)&m_param);
	if (m_mode == qproperties_source_filter)
		insertProperties(_ui_struct(qtv_source_param_filter_only), (uint8_t*)&m_param);
}

void qi_source_properties::insertProperty(const Pui_prop_item uiprop, uint8_t* raw)
{
	if (ui_props_is_struct_param(qtv_source_param, source, uiprop))
		qi_properties_base::insertProperty(new qi_property_keyvalue(this, uiprop, raw));
	else if (ui_props_is_struct_param(encoder_param, mode, uiprop))
		qi_properties_base::insertProperty(new qi_property_keyvalue(this, uiprop, raw));
	else qi_properties_base::insertProperty(uiprop, raw);
}

/* qi_properties_simple */

qi_properties_simple::qi_properties_simple(const Pui_prop_item props, uint8_t* raw, const Pui_prop_item advprops, uint8_t* advraw) :
	m_uiprops(props),
	m_rawbuf(raw),
	m_uipropsadv(advprops),
	m_rawbufadv(advraw)
{
	refreshProperties();
}

void qi_properties_simple::resetProperties()
{

}

void qi_properties_simple::refreshProperties()
{
	clearProperties();
	insertProperties(m_uiprops, m_rawbuf);
	if (m_isadvance)
		insertProperties(m_uipropsadv, m_rawbufadv);
}

vatek_result qi_properties_simple::setAdvance(bool isenable)
{
	vatek_result nres = vatek_unsupport;
	if (m_rawbufadv && m_uipropsadv)
	{
		m_isadvance = isenable;
		refreshProperties();
		nres = vatek_success;
	}
	return nres;
}

/* qtvPropertyCollection */

qtvPropertyCollection::qtvPropertyCollection() :
	m_valid(true),
	m_isadvance(false)
{

}

qtvPropertyCollection::~qtvPropertyCollection()
{
	clearProperties();
}


void qtvPropertyCollection::clearProperties()
{
	while (m_properties.count())
	{
		qtvProperty* prop = m_properties.first();
		delete prop;
		m_properties.removeFirst();
	}
}

void qtvPropertyCollection::insertProperty(qtvProperty* prop)
{
	m_properties.insert(m_properties.count(), prop);
}

void qtvPropertyCollection::insertProperty(const Pui_prop_item uiprop, uint8_t* raw)
{
	m_properties.insert(m_properties.count(), new qi_property(uiprop, raw));
}

void qtvPropertyCollection::insertProperties(const Pui_prop_item uiprops, uint8_t* raw)
{
	int32_t i = 0;
	while (uiprops[i].name)
	{
		insertProperty(&uiprops[i], raw);
		i++;
	}
}

void qtvPropertyCollection::setValid(bool bvalid)
{
	if (m_valid != bvalid)
	{
		m_valid = bvalid;
		emit notifyValidChanged(m_valid);
	}
}

qtvPropertyCollection* qtvPropertyCollection::createChipInformation(Pchip_info pinfo)
{
	return new qi_properties_simple(_ui_struct(chip_info), (uint8_t*)pinfo);
}

qtvPropertyCollection* qtvPropertyCollection::createProperties(const Pui_prop_item props, uint8_t* raw, const Pui_prop_item advprops, uint8_t* advraw)
{
	return new qi_properties_simple(props, raw, advprops, advraw);
}
