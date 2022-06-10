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

#ifndef QINTERNAL_USBMUXER_H
#define QINTERNAL_USBMUXER_H

#include <core/qtv_service_usb.h>
#include <cross/cross_os_api.h>
#include "qinternal_mux.h"
#include "qinternal_properties.h"

typedef struct _qusbmuxer_program_param
{
	uint16_t pmt_pid;
	uint16_t recv;
}qusbmuxer_program_param,*Pqusbmuxer_program_param;

typedef struct _qusbmuxer_stream_param
{
	uint16_t pid;
	uint16_t recv;
}qusbmuxer_stream_param,*Pqusbmuxer_stream_param;

class qi_usbmuxer_channel : public qtvUSBMuxerChannel
{
public:

	explicit qi_usbmuxer_channel(hmux_channel hch,uint16_t pcrpid);
	~qi_usbmuxer_channel();

	hmux_channel _handle() { return m_hchannel; }

	/* qtvMuxChannel */
	qtv_muxitem_mode _mode() override { return qmuxitem_channel; }
	QString& _name() override { return m_name; }
	qtvPropertyCollection* _properties() override { return m_properties; }
	const QList<qtvMuxProgram*> _programs() override { return m_programs; }

	/* qtvUSBMuxerChannel */
	void clear() override;
	vatek_result createProgram(uint16_t pmtpid, qtvUSBMuxerProgram** program) override;
	vatek_result getProgram(uint16_t pmtpid, qtvUSBMuxerProgram** program) override;
	vatek_result commitProgram(qtvUSBMuxerProgram* program) override;

protected:
	hmux_channel m_hchannel;
	uint16_t m_pcrpid;
	qtvUSBMuxerProgram* m_ptrprogram;
	QString m_name;
	qi_properties_channal* m_properties;
	QList<qtvMuxProgram*> m_programs;
};

class qi_usbmuxer_program : public qtvUSBMuxerProgram
{
public:

	explicit qi_usbmuxer_program(hmux_program hprogram,qtvUSBMuxerChannel* channel);
	~qi_usbmuxer_program();

	hmux_program _handle() { return m_hprogram; }

	/* qtvMuxProgram */
	qtv_muxitem_mode _mode() override { return qmuxitem_program; }
	QString& _name() override { return m_name; }
	uint16_t _pcr_pid() override { return muxprogram_get(m_hprogram)->pcr_pid; }
	uint16_t _pmt_pid() override { return  muxprogram_get(m_hprogram)->pmt_pid; }
	const QList<qtvMuxStream*> _streams() { return m_streams; }
	qtvPropertyCollection* _properties() { return m_properties; }

	/* qtvUSBMuxerProgram */
	qtvUSBMuxerChannel* _channel() override { return m_channel; }

	void clear() override;
	vatek_result createStream(uint16_t pid, qtvCodec* codec, qtvUSBMuxerStream** stream) override;
	vatek_result getStream(uint16_t pid, qtvUSBMuxerStream** stream) override;
	vatek_result commitStream(qtvUSBMuxerStream* stream) override;

	/* qtvUSBMuxerProgram */
	Pqusbmuxer_program_param _param() { return &m_param; }

protected:
	hmux_program m_hprogram;
	qtvUSBMuxerChannel* m_channel;
	qtvUSBMuxerStream* m_ptrstream;
	qusbmuxer_program_param m_param;

	QString m_name;
	QList<qtvMuxStream*> m_streams;
	qi_properties_program* m_properties;
};

class qi_usbmuxer_stream : public qtvUSBMuxerStream
{
public:
	explicit qi_usbmuxer_stream(hmux_stream hstream,qtvUSBMuxerProgram* program, qtvCodec* codec);
	~qi_usbmuxer_stream();

	/* qtvMuxStream */
	qtv_muxitem_mode _mode() override { return qmuxitem_stream; }
	QString& _name() override { return m_name; }
	uint16_t _pid() override { return m_rawstream->stream_pid; }
	int32_t _esinfo_len() override { return m_rawstream->es_info_len; }
	uint8_t* _esinfo_buffer() override { return m_rawstream->es_info_buf; }
	mux_stream_type _stream_type() override { return m_rawstream->type; }
	Pmux_stream _raw_stream() override { return m_rawstream; }

	/* qtvUSBMuxerStream */
	qtvCodec* _codec() override { return m_codec; }
	qtvUSBMuxerProgram* _program() override { return m_program; }
	qtvPropertyCollection* _properties() override { return m_properties; }

	static vatek_result createStream(uint16_t pid,qi_usbmuxer_program* program, qtvCodec* codec, qtvUSBMuxerStream** stream);

	/* qtvUSBMuxerStream */
	Pqusbmuxer_stream_param _param() { return &m_param; }

protected:
	hmux_stream m_hstream;
	qusbmuxer_stream_param m_param;
	qtvCodec* m_codec;
	qtvUSBMuxerProgram* m_program;
	QString m_name;
	Pmux_stream m_rawstream;
	qtvPropertyCollection* m_properties;
};

class qi_usbmuxer_service : public qtvServiceUSBMuxer
{
public:
	explicit qi_usbmuxer_service(qtvDevice* device, hmux_core hmux);
	~qi_usbmuxer_service();

	/* qtvServiceBase */
	qtvPropertyCollection* _output_properties() override { return &m_output; }
	Pqtv_output_param _output_param() override { return m_output._param(); }
	Pbroadcast_info _broadcast_detail() override { return vatek_usbmux_get_info(m_husbmux); }

	/* qtvServiceUSBMuxer */

	Pqtv_usbmuxer_param _mux_param() override { return &m_muxparam; }
	qtvPropertyCollection* _mux_properties() override { return &m_muxproperties; }

	void resetChannel() override;
	vatek_result createChannel(mux_spec_mode spec, mux_country_code country, qtvUSBMuxerChannel** channel) override;
	vatek_result getChannel(qtvUSBMuxerChannel** channel) override;
	vatek_result commitChannel(qtvUSBMuxerChannel* channel) override;

	void resetOutput(uint32_t freqkhz, uint32_t bw_sb, modulator_type type) override;

	vatek_result startBroadcast(qtvUSBMuxSource* source) override;
	vatek_result sendFrame(Pusbmux_stream hstream, Pmux_pes_frame pframe) override;
	vatek_result stopBroadcast() override;

protected:
	hmux_core m_hmux;
	qi_output_properties m_output;
	qi_usbmuxer_channel* m_channel;
	qtv_usbmuxer_param m_muxparam;
	qi_properties_simple m_muxproperties;
	hvatek_usbmux m_husbmux;
	void resetMuxProperties();

	Pusbmux_param getUSBMuxParam();
private:
	usbmux_param m_usbmuxparam;
};

class qusbmuxer_program_properties : public qi_properties_program
{
public: 
	explicit qusbmuxer_program_properties(qi_usbmuxer_program* uprogram);

protected:
	qi_usbmuxer_program* m_uprogram;

	void refreshProperties() override;
};

_ui_struct_start(qusbmuxer_program_param)
	_prop_h16(qusbmuxer_program_param,pmt_pid,"program pmt pid",NULLHAL)
_ui_struct_end

_ui_struct_start(qusbmuxer_stream_param)
	_prop_h16(qusbmuxer_stream_param,pid,"stream pid",NULLHAL)
_ui_struct_end

#endif
