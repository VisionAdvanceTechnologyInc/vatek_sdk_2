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

#ifndef QINTERNAL_PROPERTIES_H
#define QINTERNAL_PROPERTIES_H

#include <core/qtv_device.h>
#include <core/qtv_property.h>
#include <core/qtv_mux.h>
#include <core/qtv_media.h>
#include <core/ui/ui_props_api.h>

typedef enum _qi_properties_mode
{
	qproperties_normal,
	qproperties_source,
	qproperties_source_filter,
	qproperties_bridge,
}qi_properties_mode;

class qi_property : public qtvProperty
{
public:

	static qtv_property_type getPropertyType(const Pui_prop_item uiprop);

	explicit qi_property(const Pui_prop_item uiprop, uint8_t* raw);
	~qi_property();

	virtual bool _is_refresh() { return false; }
	/* qtvProperty */
	qtv_property_type _type() override { return m_type; }
	QString _name() override { return QString(m_uiprop->name); }
	QString _descriptor() override { return QString(m_uiprop->desc); }
	const QVariant& _value() override;
	QString _display_value() override;
	const Pui_prop_item _ui_prop() { return m_uiprop; }

	bool checkSelectValid(const Pui_prop_val val) override;
	vatek_result setValue(const QVariant& value) override;
	virtual bool checkValueValid(const QVariant& value) { return true; }

	/* qi_property */
	virtual QWidget* createTableEditor(QWidget* parent);
	virtual vatek_result setValueEditor(QWidget* editor);

protected:
	const Pui_prop_item m_uiprop;
	uint8_t* m_raw;
	char m_szdisplay[_UIPROP_STRBUF_LEN_MAX];
	qtv_property_type m_type;
	QVariant m_value;

	virtual void refreshValue();
};

class qi_property_keyvalue : public qi_property
{
public:
	explicit qi_property_keyvalue(qtvPropertyHandler* handler, const Pui_prop_item uiprop, uint8_t* raw);

	bool _is_refresh() override { return true; }
	vatek_result setValue(const QVariant& value) override;

protected:
	qtvPropertyHandler* m_hadnler;
};

class qi_property_video_codec : public qi_property
{
public:
	explicit qi_property_video_codec(qtvPropertyHandler* handler, const Pui_prop_item uiprop, uint8_t* raw, qtvDevice* device);

	bool _is_refresh() override { return true; }
	vatek_result setValue(const QVariant& value) override;
	bool checkSelectValid(const Pui_prop_val val) override;

protected:
	qtvPropertyHandler* m_hadnler;
	qtvDevice* m_device;
};

#if 0

class qi_property_string : public qi_property
{
public:
	explicit qi_property_string(qtvMuxItem* mux, const Pui_prop_item uiprop, uint8_t* raw);

	QString _display_value() override;
	vatek_result setValue(const QVariant& value) override;
	QWidget* createTableEditor(QWidget* parent) override;
	vatek_result setValueEditor(QWidget* editor) override;
	
protected:
	qtvMuxItem* m_muxitem;

	void refreshValue() override;
};

#endif

class qi_property_string_base : public qi_property
{
public:
	explicit qi_property_string_base(const Pui_prop_item uiprop, uint8_t* raw);

	QString _display_value() override;
	vatek_result setValue(const QVariant& value) override;
	QWidget* createTableEditor(QWidget* parent) override;
	vatek_result setValueEditor(QWidget* editor) override;

protected:
	qtvMuxItem* m_muxitem;


	virtual vatek_result getSpecString(mux_spec_mode* spec, uint8_t** rawprop) = 0;
	virtual vatek_result setSpecString(const char* szvalue) = 0;
	virtual vatek_result getSpecStringDefine(Pmux_ui_string* pmstring) = 0;
	void refreshValue() override;
};

class qi_property_string_channel : public qi_property_string_base
{
public:
	explicit qi_property_string_channel(hmux_channel hch, const Pui_prop_item uiprop, uint8_t* raw);

protected:
	hmux_channel m_hch;
	vatek_result getSpecString(mux_spec_mode* spec, uint8_t** rawprop) override;
	vatek_result setSpecString(const char* szvalue) override;
	vatek_result getSpecStringDefine(Pmux_ui_string* pmstring) override;
}; 

class qi_property_string_program : public qi_property_string_base
{
public:
	explicit qi_property_string_program(hmux_program hprogram, const Pui_prop_item uiprop, uint8_t* raw);

protected:
	hmux_program m_hprogram;
	vatek_result getSpecString(mux_spec_mode* spec, uint8_t** rawprop) override;
	vatek_result setSpecString(const char* szvalue) override;
	vatek_result getSpecStringDefine(Pmux_ui_string* pmstring) override;
};

class qi_property_path : public qi_property
{
public:
	explicit qi_property_path(qtvPropertyHandler* handler,const Pui_prop_item uiprop,qtv_property_pathmode mode, uint8_t* raw);

	vatek_result setValue(const QVariant& value) override;
	QWidget* createTableEditor(QWidget* parent) override;
	vatek_result setValueEditor(QWidget* editor) override;

protected:
	qtv_property_pathmode m_mode;
	qtvPropertyHandler* m_handler;
};

class qi_properties_base : public qtvPropertyCollection, public qtvPropertyHandler
{
public:
	qi_properties_base();
	~qi_properties_base();

	/* qi_property_handler */
	void raisePropertiesChanged() override;
	void raisePropertyEdited(qtvProperty* prop) override;
};

class qi_properties_simple : public qi_properties_base
{
public:
	explicit qi_properties_simple(const Pui_prop_item props, uint8_t* raw, const Pui_prop_item advprops = nullptr, uint8_t* advraw = nullptr);

	void resetProperties() override;
	vatek_result setAdvance(bool isenable) override;

protected:
	const Pui_prop_item m_uiprops;
	uint8_t* m_rawbuf;
	const Pui_prop_item m_uipropsadv;
	uint8_t* m_rawbufadv;

	void refreshProperties() override;
};

class qi_source_properties : public qi_properties_base
{
public:
	explicit qi_source_properties(qtvDevice* device, stream_source source, qi_properties_mode mode = qproperties_normal);
	Pqtv_source_param _param() { return &m_param; }

	/* qtvPropertiesCollection */
	void resetProperties() override;

protected:
	qtvDevice* m_device;
	qtv_source_param m_param;
	qi_properties_mode m_mode;
	int32_t m_inited;

	/* qi_properties_base */
	void refreshProperties() override;
	void insertProperty(const Pui_prop_item uiprop, uint8_t* raw) override;
};

class qi_output_properties : public qi_properties_base
{
public:
	explicit qi_output_properties(qtvDevice* device);
	Pqtv_output_param _param() { return&m_param; }

	void resetParam(modulator_type type, uint32_t bw_sb, uint32_t frequency);

	/* qtvPropertiesCollection */
	void resetProperties() override;

protected:
	qtvDevice* m_device;
	qtv_output_param m_param;

	/* qi_properties_base */
	void refreshProperties() override;
	void insertProperty(const Pui_prop_item uiprop, uint8_t* raw) override;
};

class qi_modulation_properties : public qi_properties_base
{
public:
	explicit qi_modulation_properties(Pmodulator_param pmod);

	void resetProperties() override;

protected:
	Pmodulator_param m_param;
	void refreshProperties() override;
	void insertProperty(const Pui_prop_item uiprop, uint8_t* raw) override;
};

#endif
