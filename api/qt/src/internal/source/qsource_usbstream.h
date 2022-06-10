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

#ifndef QSOURCE_USBSTREAM_H
#define QSOURCE_USBSTREAM_H

#include <core/qtv_source.h>
#include "../qinternal_properties.h"
#include <cross/cross_stream.h>

#define QSOURCE_USBSTREAM_NAME	"USB-Stream"

typedef struct _qusbstream_param
{
	qtv_usbstream_param usbstream;
	qtv_source_param source;
}qusbstream_param,*Pqusbstream_param;

class qusbstream_properties : public qi_properties_base
{
public:
	explicit qusbstream_properties();
	~qusbstream_properties();

	void resetProperties() override;
	Pqusbstream_param _param() { return &m_param; }

	static vatek_result checkUSBStreamValid(qusbstream_properties* props);
protected:
	qusbstream_param m_param;

	void insertProperty(const Pui_prop_item uiprop, uint8_t* raw) override;
	void refreshProperties() override;
	bool checkPropertiesValid() override;
};

class qusbstream_source_factory : public qtvSourceFactory
{
public:
	explicit qusbstream_source_factory();
	~qusbstream_source_factory();

	QString& _name() override { return m_name; }
	qtv_source_mode _mode() override { return qsource_usbstream; }

	bool checkSupported(qtvServiceBase* sevice) override;
	vatek_result createProperties(qtvServiceBase* service, qtvPropertyCollection** properties) override;
	vatek_result checkProperties(qtvPropertyCollection* properties) override;
	vatek_result createSource(qtvServiceBase* service, qtvPropertyCollection* properties, qtvSourceBase** source) override;

protected:
	QString m_name;
};

class qusbstream_source : public qtvSourceUSBStream
{
public:
	explicit qusbstream_source(qtvServiceBase* service, qtvSourceFactory* factory, qusbstream_properties* properties);
	~qusbstream_source();

	/* qtvSourceBase */
	qtv_status _status() override { return m_status; }
	qtvPropertyCollection* _properites() override { return m_properties; }
	Pqtv_source_param _param() override { return &m_properties->_param()->source; }
	const qtvSourceFactory* _factory() { return m_factory; }

	/* qtvSourceBase */
	vatek_result open() override;
	vatek_result start() override;
	vatek_result polling() override;
	void stop() override;
	void close() override;

	/* qtvSourceTransform */
	stream_mode _stream_mode() override { return stream_remux; }
	bool _enable_enum_context()override { return (_usbstream_param()->filter == qfilter_enum); }
	vatek_result getStreamPackets(uint8_t** slice) override;

	/* qtvSourceTransform */
	Pqtv_usbstream_param _usbstream_param() override { return &m_properties->_param()->usbstream; }

protected:
	qtv_status m_status;
	qtvSourceFactory* m_factory;
	qtvServiceBase* m_service;
	qusbstream_properties* m_properties;
	hcross_stream m_hstream;
};


#endif
