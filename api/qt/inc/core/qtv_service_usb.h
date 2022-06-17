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

#ifndef QTV_SERVICE_USB_H
#define QTV_SERVICE_USB_H

#include <core/qtv_media.h>
#include <core/qtv_service.h>
#include <vatek_sdk_usbstream.h>
#include <vatek_sdk_usbmux.h>

typedef struct _qtv_usbmuxer_param
{
	uint16_t pcr_pid;
	uint16_t pcr_interval;
	uint16_t latency_ms;
	uint16_t recv;
}qtv_usbmuxer_param,*Pqtv_usbmuxer_param;

class QTV_DECL qtvServiceUSBStream : public qtvServiceBase
{
	Q_OBJECT
public:
	static vatek_result createServiceUSBStream(qtvDevice* device);
};

class qtvUSBMuxerProgram;
class qtvUSBMuxerChannel;

class QTV_DECL qtvUSBMuxerStream : public qtvMuxStream
{
	Q_OBJECT
public:
	virtual qtvCodec* _codec() = 0;
	virtual qtvUSBMuxerProgram* _program() = 0;
	virtual qtvPropertyCollection* _properties() = 0;
};

class QTV_DECL qtvUSBMuxerProgram : public qtvMuxProgram
{
	Q_OBJECT
public:
	virtual qtvUSBMuxerChannel* _channel() = 0;

	virtual void clear() = 0;
	virtual vatek_result createStream(uint16_t pid, qtvCodec* codec, qtvUSBMuxerStream** stream) = 0;
	virtual vatek_result getStream(uint16_t pid, qtvUSBMuxerStream** stream) = 0;
	virtual vatek_result commitStream(qtvUSBMuxerStream* stream) = 0;
};

class QTV_DECL qtvUSBMuxerChannel : public qtvMuxChannel
{
	Q_OBJECT
public:
	virtual void clear() = 0;
	virtual vatek_result createProgram(uint16_t pmtpid, qtvUSBMuxerProgram** program) = 0;
	virtual vatek_result getProgram(uint16_t pmtpid, qtvUSBMuxerProgram** program) = 0;
	virtual vatek_result commitProgram(qtvUSBMuxerProgram* program) = 0;
};

typedef void* hqtv_program;

class QTV_DECL qtvUSBMuxSource : public QObject
{
	Q_OBJECT
public:
	virtual Pusbmux_source getHandle() = 0;
	virtual vatek_result startProgram(qtvUSBMuxerProgram* program, Pusbmux_program uprogram, hqtv_program* hprogram) = 0;
	virtual vatek_result pushStream(hqtv_program hprogram, qtvUSBMuxerStream* stream, Pusbmux_stream ustream) = 0;
	virtual void stopProgram(hqtv_program hprogram) = 0;
};

class QTV_DECL qtvServiceUSBMuxer : public qtvServiceBase
{
	Q_OBJECT
public:
	static vatek_result createService(qtvDevice* device, qtvServiceUSBMuxer** service);
	~qtvServiceUSBMuxer() {}

	virtual Pqtv_usbmuxer_param _mux_param() = 0;
	virtual qtvPropertyCollection* _mux_properties() = 0;

	virtual void resetChannel() = 0;
	virtual vatek_result createChannel(mux_spec_mode spec, mux_country_code country, qtvUSBMuxerChannel** channel) = 0;
	virtual vatek_result getChannel(qtvUSBMuxerChannel** channel) = 0;
	virtual vatek_result commitChannel(qtvUSBMuxerChannel* channel) = 0;

	virtual void resetOutput(uint32_t freqkhz, uint32_t bw_sb, modulator_type type) = 0;

	virtual vatek_result startBroadcast(qtvUSBMuxSource* source) = 0;
	virtual vatek_result sendFrame(Pusbmux_stream hstream,Pmux_pes_frame pframe) = 0;
	virtual vatek_result stopBroadcast() = 0;

protected:
	explicit qtvServiceUSBMuxer(qtv_service_mode mode, qtvDevice* device) : qtvServiceBase(mode, device) {}
};

#endif
