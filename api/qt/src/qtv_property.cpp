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

#include "./internal/qinternal_core.h"
#include "./internal/qinternal_ui.h"
#include "./internal/qinternal_mux.h"
#include <core/ui/ui_props_api.h>

/* qi_property */

qi_property::qi_property(const Pui_prop_item uiprop, uint8_t* raw) :
	m_uiprop(uiprop),
	m_raw(raw),
	m_type(getPropertyType(uiprop))
{
	refreshValue();
}

qi_property::~qi_property()
{

}

qtv_property_type qi_property::getPropertyType(const Pui_prop_item uiprop)
{
	if (uiprop->values)
	{
		if (IS_UIPROP_CHECKBOS(uiprop->type))return qproperty_flag;
		else return qproperty_select;
	}
	else
	{
		if (IS_UIPROP_PATH(uiprop->type))return qproperty_path;
		else if (IS_UIPROP_STRBUF(uiprop->type))return qproperty_string;
		else
		{
			int32_t len = uiprop->type & 0xF;
			qtv_property_type type = qproperty_null;
			if (IS_UIPROP_HEX(uiprop->type))
			{
				if (len == 1)return qproperty_h8;
				else if (len == 2)return qproperty_h16;
				else if (len == 4)return qproperty_h32;
			}
			else
			{
				if (len == 1)return qproperty_u8;
				else if (len == 2)return qproperty_u16;
				else if (len == 4)return qproperty_u32;
			}
			return type;
		}
	}
}

const QVariant& qi_property::_value()
{
	return m_value;
}

QString qi_property::_display_value()
{
	ui_props_get_value_str(m_uiprop, m_raw, &m_szdisplay[0], _UIPROP_STRBUF_LEN_MAX);
	return QString(&m_szdisplay[0]);
}

vatek_result qi_property::setValue(const QVariant& value)
{
	vatek_result nres = vatek_unsupport;
	if (_type() != qproperty_null &&
		_type() != qproperty_string &&
		_type() != qproperty_path)
	{
		if (checkValueValid(value))
		{
			nres = ui_props_set_uint32(m_uiprop, m_raw, value.toUInt());
			if (is_vatek_success(nres))m_value = value;
		}
		else nres = vatek_badparam;
	}
	return nres;
}

bool qi_property::checkSelectValid(const Pui_prop_val val)
{
	return true;
}

QWidget* qi_property::createTableEditor(QWidget* parent)
{
	if (_type() == qproperty_select || _type() == qproperty_flag)
		return new qui_editor_combobox(this, parent);
	else if (_type() == qproperty_path || _type() == qproperty_null || _type() == qproperty_string)
		return new qui_editor_unsupport(parent);
	else return new qui_editor_uintedit(this, parent);
}

vatek_result qi_property::setValueEditor(QWidget* editor)
{
	vatek_result nres = vatek_unsupport;
	if (_type() == qproperty_select || _type() == qproperty_flag)
	{
		qui_editor_combobox* combobox = qobject_cast<qui_editor_combobox*>(editor);
		if (combobox)
			nres = setValue(combobox->getValue());
	}
	else if (_type() == qproperty_path || _type() == qproperty_null || _type() == qproperty_string)
	{

	}
	else
	{
		qui_editor_uintedit* uintedit = qobject_cast<qui_editor_uintedit*>(editor);
		if (uintedit)
			nres = setValue((uint32_t)uintedit->value());
	}

	if (!is_vatek_success(nres))
		VWAR("setValueEditor fail : %d",nres);
	return nres;
}

void qi_property::refreshValue()
{
	if (_type() == qproperty_null)m_value.setValue(nullptr);
	else if (_type() == qproperty_string)m_value.setValue(QString("unsupported"));
	else if (_type() == qproperty_path)m_value.setValue(QString("unsupported"));
	else
	{
		uint32_t val = 0;
		vatek_result nres = ui_props_get_uint32(m_uiprop, m_raw, &val);
		if (is_vatek_success(nres))m_value.setValue(val);
		else m_value.setValue(0);

		if (!is_vatek_success(nres))VWAR("ui_property get uint32 fail : %d", nres);
	}
}

/* qi_property_keyvalue */

qi_property_keyvalue::qi_property_keyvalue(qtvPropertyHandler* handler, const Pui_prop_item uiprop, uint8_t* raw) :
	qi_property(uiprop, raw),
	m_hadnler(handler)
{

}

vatek_result qi_property_keyvalue::setValue(const QVariant& value)
{
	vatek_result nres = vatek_badparam;
	if (checkValueValid(value))
	{
		if (m_value.compare(value) == 0)nres = vatek_success;
		else
		{
			nres = qi_property::setValue(value);
			if (is_vatek_success(nres))
				m_hadnler->raisePropertiesChanged();
		}
	}
	return nres;
}

/* qi_property_string */

qi_property_string_base::qi_property_string_base(const Pui_prop_item uiprop, uint8_t* raw) :
	qi_property(uiprop, raw)
{

}

QString qi_property_string_base::_display_value()
{
	vatek_result nres = vatek_unsupport;
	Pmux_string pstring = *(Pmux_string*)&m_raw[m_uiprop->offset];
	Pmux_ui_string pmstr = NULL;
	nres = getSpecStringDefine(&pmstr);
	if (is_vatek_success(nres))
	{
		memset(&m_szdisplay[0], 0, _UIPROP_STRBUF_LEN_MAX);
		nres = muxspec_get_spec_string_display(pmstr, pstring, &m_szdisplay[0], _UIPROP_STRBUF_LEN_MAX);
	}

	if (is_vatek_success(nres))return &m_szdisplay[0];
	else
	{
		VWAR("qi_property_string get fail : %d", nres);
		return "_fail_string";
	}
}

vatek_result qi_property_string_base::setValue(const QVariant& value)
{
	vatek_result nres = vatek_badparam;
	if (value.type() == QVariant::String)
	{

		QString strvalue = value.toString();
		QByteArray qArr = strvalue.toLatin1();
		const char* cStr2 = qArr.data();
		nres = setSpecString(cStr2);
	}

	if (!is_vatek_success(nres))VWAR("qi_property_string setValue fail : %d", nres);
	return nres;
}

QWidget* qi_property_string_base::createTableEditor(QWidget* parent)
{
	return new qui_editor_string(this, parent);
}

vatek_result qi_property_string_base::setValueEditor(QWidget* editor)
{
	qui_editor_string* streditor = qobject_cast<qui_editor_string*>(editor);
	vatek_result nres = vatek_unsupport;
	if (streditor)
		nres = setValue(streditor->text());

	if (!is_vatek_success(nres))VWAR("qi_property_string set value edit fail : %d", nres);
	return nres;
}

void qi_property_string_base::refreshValue()
{
	vatek_result nres = vatek_unsupport;
	memset(&m_szdisplay[0], 0, _UIPROP_STRBUF_LEN_MAX);
	if (m_muxitem)
	{
		mux_spec_mode spec = mux_spec_undefine;
		uint8_t* praw = NULL;
		nres = getSpecString(&spec, &praw);
		if (is_vatek_success(nres))
		{
			Pmux_ui_string puistr = NULL;
			nres = muxspec_get_spec_string(spec, _ui_prop(), &puistr);
			if (is_vatek_success(nres))
			{
				Pmux_string pstr = *(Pmux_string*)&praw[_ui_prop()->offset];
				nres = muxspec_get_spec_string_display(puistr, pstr, &m_szdisplay[0], _UIPROP_STRBUF_LEN_MAX);
			}
		}
	}
	if (!is_vatek_success(nres))VWAR("qi_property_string refreshValue fail : %d", nres);
}

/* qi_property_string_channel */

qi_property_string_channel::qi_property_string_channel(hmux_channel hch, const Pui_prop_item uiprop, uint8_t* raw) :
	m_hch(hch),
	qi_property_string_base(uiprop, raw)
{

}

vatek_result qi_property_string_channel::getSpecString(mux_spec_mode* spec, uint8_t** rawprop)
{
	Pspec_channel param = NULL;
	vatek_result nres = muxchannel_get_param(m_hch, &param);
	if (is_vatek_success(nres))
	{
		*spec = param->spec;
		*rawprop = (uint8_t*)&param->_spec;
	}
	return nres;
}

vatek_result qi_property_string_channel::setSpecString(const char* szvalue)
{
	Pspec_channel param = NULL;
	vatek_result nres = muxchannel_get_param(m_hch, &param);
	if (is_vatek_success(nres))
		nres = muxspec_set_channel_spec_string(param->spec, _ui_prop()->name, (uint8_t*)&param->_spec, szvalue);
	return nres;
}


vatek_result qi_property_string_channel::getSpecStringDefine(Pmux_ui_string* pmstring)
{
	Pspec_channel param = NULL;
	vatek_result nres = muxchannel_get_param(m_hch, &param);
	if (is_vatek_success(nres))
	{
		nres = muxspec_get_spec_string(param->spec, m_uiprop, pmstring);
	}
	return nres;
}

/* qi_property_string_program */

qi_property_string_program::qi_property_string_program(hmux_program hprogram, const Pui_prop_item uiprop, uint8_t* raw) :
	m_hprogram(hprogram),
	qi_property_string_base(uiprop, raw)
{

}

vatek_result qi_property_string_program::getSpecString(mux_spec_mode* spec, uint8_t** rawprop)
{
	Pspec_program param = muxprogram_get_param(m_hprogram);
	*spec = param->spec;
	*rawprop = (uint8_t*)&param->_spec;
	return vatek_success;
}

vatek_result qi_property_string_program::setSpecString(const char* szvalue)
{
	Pspec_program param = muxprogram_get_param(m_hprogram);
	vatek_result nres = muxspec_set_program_spec_string(param->spec, _ui_prop()->name, (uint8_t*)&param->_spec, szvalue);
	return nres;
}

vatek_result qi_property_string_program::getSpecStringDefine(Pmux_ui_string* pmstring)
{
	Pspec_program param = muxprogram_get_param(m_hprogram);
	vatek_result nres = muxspec_get_spec_string(param->spec, m_uiprop, pmstring);
	return nres;
}

/* qi_property_path */

qi_property_path::qi_property_path(qtvPropertyHandler* handler, const Pui_prop_item uiprop, qtv_property_pathmode mode, uint8_t* raw) :
	m_mode(mode),
	m_handler(handler),
	qi_property(uiprop, raw)
{

}

vatek_result qi_property_path::setValue(const QVariant& value)
{
	vatek_result nres = vatek_badparam;
	if (value.type() == QVariant::String)
	{
		QString szval = qvariant_cast<QString>(value);
		memset(&m_szdisplay[0], 0, _UIPROP_STRBUF_LEN_MAX);
		strncpy(&m_szdisplay[0], szval.toLocal8Bit().data(), _UIPROP_STRBUF_LEN_MAX);
		nres = ui_props_set_strbuf(m_uiprop, m_raw, (uint8_t*)&m_szdisplay[0]);
		if (is_vatek_success(nres))m_handler->raisePropertyEdited(this);
	}
	return nres;
}

QWidget* qi_property_path::createTableEditor(QWidget* parent)
{
	return new qui_editor_path(this,m_mode, parent);
}

vatek_result qi_property_path::setValueEditor(QWidget* editor)
{
	qui_editor_path* path = qobject_cast<qui_editor_path*>(editor);
	QString szvalue = path->getResult();
	vatek_result nres = vatek_badparam;
	if (!szvalue.isEmpty())nres = setValue(szvalue);
	return nres;
}

/* qi_property_video_codec */

qi_property_video_codec::qi_property_video_codec(qtvPropertyHandler* handler, const Pui_prop_item uiprop, uint8_t* raw, qtvDevice* device) :
	qi_property(uiprop, raw),
	m_hadnler(handler),
	m_device(device)
{

}

vatek_result qi_property_video_codec::setValue(const QVariant& value)
{
	vatek_result nres = vatek_badparam;
	if (checkValueValid(value))
	{
		if (m_value.compare(value) == 0)nres = vatek_success;
		else
		{
			nres = qi_property::setValue(value);
			if (is_vatek_success(nres))
				m_hadnler->raisePropertiesChanged();
		}
	}
	return nres;
}

bool qi_property_video_codec::checkSelectValid(const Pui_prop_val val)
{
	if (m_device)
	{
		vatek_ic_module icchip = m_device->_chip_info()->chip_module;
		if (val->val == encvideo_mpeg2)
		{
			if (icchip != ic_module_b3_lite)return true;
		}
		else if (val->val == encvideo_h264)
		{
			if (icchip == ic_module_b3_plus || icchip == ic_module_b3_lite)
				return true;
		}
	}
	return false;
}

