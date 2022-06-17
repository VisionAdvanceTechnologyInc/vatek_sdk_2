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

#ifndef QINTERNAL_MUX_H
#define QINTERNAL_MUX_H

#include <QWidget>
#include <core/qtv_mux.h>
#include <mux/mux_handle.h>
#include "qinternal_core.h"

class qi_properties_channal : public qi_properties_base
{
public:
	explicit qi_properties_channal(qtvMuxChannel* ch,hmux_channel hch);

	void resetProperties() override;

protected:
	Pspec_channel m_param;
	qtvMuxChannel* m_channel;
	hmux_channel m_hchannel;

	void refreshProperties() override;
	void insertProperty(const Pui_prop_item uiprop, uint8_t* raw) override;
};

class qi_properties_program : public qi_properties_base
{
public:
	qi_properties_program(qtvMuxProgram* program,hmux_program hprogram);

	void resetProperties() override;

protected:
	Pspec_program m_param;
	hmux_program m_hprogram;
	qtvMuxProgram* m_program;

	void refreshProperties() override;
	void insertProperty(const Pui_prop_item uiprop, uint8_t* raw) override;
};

class qi_muxstream : public qtvMuxStream
{
public:
	explicit qi_muxstream(hmux_stream hstream);
	
	hmux_stream _handle() { return m_hstream; }
	QString& _name() override { return m_name; }
	uint16_t _pid() override { return m_stream->stream_pid; }
	mux_stream_type _stream_type() override{ return m_stream->type; }
	Pmux_stream _raw_stream() override { return m_stream; }
	int32_t _esinfo_len() override { return m_eslen; }
	uint8_t* _esinfo_buffer() override { return m_esbuf; }

protected:
	QString m_name;
	hmux_stream m_hstream;
	Pmux_stream m_stream;
	int32_t m_eslen;
	uint8_t* m_esbuf;
};

class qi_muxprogram : public qtvMuxProgram
{
public:
	explicit qi_muxprogram(hmux_program hprogram);
	~qi_muxprogram();

	hmux_program _handle() { return m_hprogram; }
	QString& _name() override { return m_name; }
	uint16_t _pcr_pid() override { return m_program->pcr_pid; }
	uint16_t _pmt_pid() override { return m_program->pmt_pid; }
	const QList<qtvMuxStream*> _streams() override { return m_streams; }
	qtvPropertyCollection* _properties() override { return m_properties; }

protected:
	QString m_name;
	hmux_program m_hprogram;
	Pmux_program m_program;
	qtvPropertyCollection* m_properties;
	QList<qtvMuxStream*> m_streams;

	void refreshItem();
};

class qi_muxchannel : public qtvMuxChannel
{
public:
	explicit qi_muxchannel(hmux_channel hchannel);
	~qi_muxchannel();

	hmux_channel _handle() { return m_hchannel; }

	QString& _name() override { return m_name; }
	qtvPropertyCollection* _properties() override { return m_properties; }
	const QList<qtvMuxProgram*> _programs() override { return m_programs; }

protected:
	QString m_name;
	hmux_channel m_hchannel;
	qtvPropertyCollection* m_properties;
	QList<qtvMuxProgram*> m_programs;

	void refreshItem();
};

class qi_muxdefault_service : public qtvMuxServiceDefault
{
public:
	explicit qi_muxdefault_service(hmux_core hmux, hmux_channel hch);
	~qi_muxdefault_service();
	qtvMuxChannel* _channel() override { return m_channel; }
	vatek_result applyDevice(qtvDevice* device) override;

protected:
	hmux_core m_hmux;
	hmux_channel m_hchannel;
	qtvMuxChannel* m_channel;
};

class qi_muxdefault_channel : public qtvMuxServiceChannel
{
public:
	explicit qi_muxdefault_channel(hmux_core hmux);
	~qi_muxdefault_channel();

	/* qtvMuxServiceDefault */
	qtvMuxChannel* _channel() override { return m_channel; }
	vatek_result applyDevice(qtvDevice* device) override;
	
	/* qtvMuxServiceChannel */
	vatek_result resetChannel(mux_spec_mode mode, mux_country_code country) override;
	vatek_result createProgram(uint16_t pcrpid, uint16_t pmtpid) override;
	vatek_result pushStream(uint16_t pid, Pmedia_video video, uint8_t* esinfo, int32_t eslen) override;
	vatek_result pushStream(uint16_t pid, Pmedia_audio audio, uint8_t* esinfo, int32_t eslen) override;
	vatek_result pushStream(uint16_t pid, uint8_t streamtype, uint8_t* esinfo, int32_t eslen) override;
	vatek_result commitProgram() override;
	vatek_result finishChannel() override;


protected:
	hmux_core m_hmux;
	hmux_channel m_hchannel;
	hmux_program m_hprogram;
	qtvMuxChannel* m_channel;

	void resetContext();
};

class qi_muxrawtable : public qtvMuxRawTable
{
public:
	explicit qi_muxrawtable(Ppsi_table_raw table) : m_rawtable(table){}
	const uint8_t* _packet_buffer() override { return m_rawtable->packet_buf; }
	const int32_t _packet_nums() override { return m_rawtable->packet_nums; }
	const int32_t _interval_ms() override { return m_rawtable->interval_ms; }

protected:
	Ppsi_table_raw m_rawtable;
};

class qi_muxrawtable_service : public qtvMuxServiceRawTable
{
public:
	explicit qi_muxrawtable_service(hmux_rawtable hraw);
	~qi_muxrawtable_service();

	vatek_result insertTable(int32_t intervalms, int32_t nums, uint8_t* buffer) override;
	const QList<qtvMuxRawTable*> _tables() override { return m_rawtables; }

protected:
	hmux_rawtable m_hrawtable;
	QList<qtvMuxRawTable*> m_rawtables;
};

#endif
