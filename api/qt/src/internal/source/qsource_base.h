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

#ifndef QSOURCE_BASE_H
#define QSOURCE_BASE_H

#include <core/qtv_source.h>
#include "../qinternal_properties.h"

#define QSOURCE_TSIN_NAME		"tsin general"
#define QSOURCE_ENCODER_NAME	"encoder general"

class qtsin_source_factory : public qtvSourceFactory
{
public:

	explicit qtsin_source_factory();
	~qtsin_source_factory();

	QString& _name() override { return m_name; }
	qtv_source_mode _mode() { return qsource_tsin; }

	bool checkSupported(qtvServiceBase* sevice) override;
	vatek_result createProperties(qtvServiceBase* service, qtvPropertyCollection** properties) override;
	vatek_result checkProperties(qtvPropertyCollection* properties) override;
	vatek_result createSource(qtvServiceBase* service, qtvPropertyCollection* properties, qtvSourceBase** source) override;

protected:
	QString m_name;
};

class qtsin_source : public qtvSourceTransform
{
public:
	explicit qtsin_source(qtvServiceBase* service, qi_source_properties* properties, qtvSourceFactory* factory);
	~qtsin_source();

	/* qtvSourceBase */
	qtv_source_mode _source_mode() override { return qsource_tsin; }
	qtv_status _status() override { return m_status; }
	qtvPropertyCollection* _properites() override { return m_properties; }
	Pqtv_source_param _param() override { return m_properties->_param(); }
	const qtvSourceFactory* _factory() override { return m_factory; }

	vatek_result open() override;
	vatek_result start() override;
	vatek_result polling() override;
	void stop() override;
	void close() override;

	/* qtvSourceTransform */
	stream_mode _stream_mode() override { return _param()->stream.tsin.streammode; }
	bool _enable_enum_context() override { return (_param()->filter == qfilter_enum); }

protected:
	qtv_status m_status;
	qtvServiceBase* m_service;
	qi_source_properties* m_properties;
	qtvSourceFactory* m_factory;
};

class qencoder_source_factory : public qtvSourceFactory
{
public:

	qencoder_source_factory();
	~qencoder_source_factory();

	QString& _name() override { return m_name; }
	qtv_source_mode _mode() { return qsource_encoder; }

	bool checkSupported(qtvServiceBase* sevice) override;
	vatek_result createProperties(qtvServiceBase* service, qtvPropertyCollection** properties) override;
	vatek_result checkProperties(qtvPropertyCollection* properties) override;
	vatek_result createSource(qtvServiceBase* service, qtvPropertyCollection* properties, qtvSourceBase** source) override;

protected:
	QString m_name;
};

class qencoder_source : public qtvSourceBroadcast
{
public:

	explicit qencoder_source(qtvServiceBase* service, qi_source_properties* properties, qtvSourceFactory* factory);
	~qencoder_source();

	/* qtvSourceBase */
	qtv_source_mode _source_mode() override { return qsource_encoder; }
	qtv_status _status() override { return m_status; }
	qtvPropertyCollection* _properites() override { return m_properties; }
	Pqtv_source_param _param() override { return m_properties->_param(); }
	const qtvSourceFactory* _factory() override { return m_factory; }

	vatek_result open() override;
	vatek_result start() override;
	vatek_result polling() override;
	void stop() override;
	void close() override;

protected:
	qtv_status m_status;
	qtvServiceBase* m_service;
	qi_source_properties* m_properties;
	qtvSourceFactory* m_factory;
};

#endif
