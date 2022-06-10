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

#ifndef QTV_MUX_H
#define QTV_MUX_H

#include <core/qtv_device.h>
#include <core/qtv_property.h>
#include <service/service_broadcast.h>

class QTV_DECL qtvMuxItem : public QObject
{
	Q_OBJECT
public:
	virtual qtv_muxitem_mode _mode() = 0;
};

class QTV_DECL qtvMuxStream : public qtvMuxItem
{
	Q_OBJECT
public:
	qtv_muxitem_mode _mode() override { return qmuxitem_stream; }
	virtual QString& _name() = 0;
	virtual uint16_t _pid() = 0;
	virtual mux_stream_type _stream_type() = 0;
	virtual Pmux_stream _raw_stream() = 0;
	virtual int32_t _esinfo_len() = 0;
	virtual uint8_t* _esinfo_buffer() = 0;
};

class QTV_DECL qtvMuxProgram : public qtvMuxItem
{
	Q_OBJECT
public:
	qtv_muxitem_mode _mode() override { return qmuxitem_program; }
	virtual QString& _name() = 0;
	virtual uint16_t _pcr_pid() = 0;
	virtual uint16_t _pmt_pid() = 0;
	virtual const QList<qtvMuxStream*> _streams() = 0;
	virtual qtvPropertyCollection* _properties() = 0;
};

class QTV_DECL qtvMuxChannel : public qtvMuxItem
{
	Q_OBJECT
public:
	qtv_muxitem_mode _mode() override { return qmuxitem_channel; }
	virtual QString& _name() = 0;
	virtual qtvPropertyCollection* _properties() = 0;
	virtual const QList<qtvMuxProgram*> _programs() = 0;
};

class QTV_DECL qtvMuxRawTable : public qtvMuxItem
{
	Q_OBJECT
public:
	qtv_muxitem_mode _mode() override { return qmuxitem_rawtable; }
	virtual const uint8_t* _packet_buffer() = 0;
	virtual const int32_t _packet_nums() = 0;
	virtual const int32_t _interval_ms() = 0;
};

class QTV_DECL qtvMuxServiceBase : public QObject
{
	Q_OBJECT
public:
	virtual qtv_muxservice_mode _mux_mode() = 0;
	virtual vatek_result applyDevice(qtvDevice* device) = 0;
};

class QTV_DECL qtvMuxServiceDefault : public qtvMuxServiceBase
{
	Q_OBJECT
public:
	qtv_muxservice_mode _mux_mode() override { return qmuxservice_default; }
	virtual qtvMuxChannel* _channel() = 0;

	static vatek_result createBroadcastMux(mux_spec_mode spec, 
										   mux_country_code country,
										   uint16_t pcrpid,
										   Pencoder_param penc,
										   qtvMuxServiceDefault** muxhandle);
};

class QTV_DECL qtvMuxServiceChannel : public qtvMuxServiceDefault
{
	Q_OBJECT
public:
	virtual vatek_result resetChannel(mux_spec_mode mode, mux_country_code country) = 0;
	virtual vatek_result createProgram(uint16_t pcrpid, uint16_t pmtpid) = 0;
	virtual vatek_result pushStream(uint16_t pid, Pmedia_video video,uint8_t* esinfo,int32_t eslen) = 0;
	virtual vatek_result pushStream(uint16_t pid, Pmedia_audio audio, uint8_t* esinfo, int32_t eslen) = 0;
	virtual vatek_result pushStream(uint16_t pid, uint8_t streamtype, uint8_t* esinfo, int32_t eslen) = 0;
	virtual vatek_result commitProgram() = 0;
	virtual vatek_result finishChannel() = 0;

	static vatek_result createChannel(qtvMuxServiceChannel** muxchannel);
};

class QTV_DECL qtvMuxServiceRawTable : public qtvMuxServiceBase
{
	Q_OBJECT
public:
	qtv_muxservice_mode _mux_mode() override { return qmuxservice_rawtable; }
	virtual vatek_result insertTable(int32_t intervalms, int32_t nums, uint8_t* buffer) = 0;
	virtual const QList<qtvMuxRawTable*> _tables() = 0;
};

#endif
