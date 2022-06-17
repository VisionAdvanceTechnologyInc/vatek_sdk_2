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

#ifndef QTV_SOURCE_H
#define QTV_SOURCE_H

#include <core/qtv_property.h>
#include <core/qtv_mux.h>

class qtvSourceFactory;
class qtvServiceBase;

class QTV_DECL qtvSourceBase : public QObject
{
	Q_OBJECT
public:

	virtual qtv_source_mode _source_mode() = 0;
	virtual qtv_status _status() = 0;
	virtual qtvPropertyCollection* _properites() = 0;
	virtual Pqtv_source_param _param() = 0;
	virtual const qtvSourceFactory* _factory() = 0;

	virtual vatek_result open() = 0;
	virtual vatek_result start() = 0;
	virtual vatek_result polling() = 0;
	virtual void stop() = 0;
	virtual void close() = 0;
};

class QTV_DECL qtvSourceFactory : public QObject
{
	Q_OBJECT
public:
	virtual QString& _name() = 0;
	virtual qtv_source_mode _mode() = 0;

	virtual bool checkSupported(qtvServiceBase* sevice) = 0;
	virtual vatek_result createProperties(qtvServiceBase* service,qtvPropertyCollection** properties) = 0;
	virtual vatek_result checkProperties(qtvPropertyCollection* properties) = 0;
	virtual vatek_result createSource(qtvServiceBase* service, qtvPropertyCollection* properties, qtvSourceBase** source) = 0;
};

class QTV_DECL qtvSourceBroadcast : public qtvSourceBase
{
	Q_OBJECT
public:

};

class QTV_DECL qtvSourceTransform : public qtvSourceBase
{
	Q_OBJECT
public:
	virtual stream_mode _stream_mode() = 0;
	virtual bool _enable_enum_context() = 0;
};

class QTV_DECL qtvSourceUSBStream : public qtvSourceTransform
{
	Q_OBJECT
public:
	qtv_source_mode _source_mode() override { return qsource_usbstream; }
	virtual Pqtv_usbstream_param _usbstream_param() = 0;
	virtual vatek_result getStreamPackets(uint8_t** slice) = 0;
};

class QTV_DECL qtvSourceBridgeAV : public qtvSourceBroadcast
{
	Q_OBJECT
public:
	qtv_source_mode _source_mode() override { return qsource_bridge_av; }
};

class QTV_DECL qtvSourceBridgeDemod : public qtvSourceTransform
{
	Q_OBJECT
public:
	qtv_source_mode _source_mode() override { return qsource_bridge_demod; }
};

#endif
