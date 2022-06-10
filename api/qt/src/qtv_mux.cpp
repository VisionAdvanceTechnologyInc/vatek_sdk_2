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

#include "./internal/qinternal_mux.h"

/* qi_properties_channal */

qi_properties_channal::qi_properties_channal(qtvMuxChannel* ch, hmux_channel hch) :
	m_param(NULL),
	m_channel(ch),
	m_hchannel(hch)
{
	muxchannel_get_param(hch, &m_param);
	refreshProperties();
}

void qi_properties_channal::resetProperties()
{

}

void qi_properties_channal::refreshProperties()
{
	Pui_prop_item props = NULL;
	vatek_result nres = muxspec_get_channel_uiprops(m_param->spec, &props);
	clearProperties();
	if (is_vatek_success(nres))
	{
		insertProperties(props, (uint8_t*)&m_param->_spec);
	}
	else VWAR("qi_properties_channal get channel ui_props fail : %d", nres);
}

void qi_properties_channal::insertProperty(const Pui_prop_item uiprop, uint8_t* raw)
{
	if (IS_UIPROP_STRBUF(uiprop->type))
		qi_properties_base::insertProperty(new qi_property_string_channel(m_hchannel, uiprop, raw));
	else if (IS_UIPROP_LANG(uiprop->type))
	{

	}
	else qi_properties_base::insertProperty(uiprop, raw);
}

/* qi_properties_program */

qi_properties_program::qi_properties_program(qtvMuxProgram* program, hmux_program hprogram) :
	m_param(muxprogram_get_param(hprogram)),
	m_program(program),
	m_hprogram(hprogram)
{
	refreshProperties();
}

void qi_properties_program::resetProperties()
{

}

void qi_properties_program::refreshProperties()
{
	Pui_prop_item props = NULL;
	vatek_result nres = muxspec_get_program_uiprops(m_param->spec,&props);
	clearProperties();
	if (is_vatek_success(nres))
		insertProperties(props, (uint8_t*)&m_param->_spec);
	else VWAR("qi_properties_channal get program ui_props fail : %d", nres);
}

void qi_properties_program::insertProperty(const Pui_prop_item uiprop, uint8_t* raw)
{
	if (IS_UIPROP_STRBUF(uiprop->type))
	{
		qi_properties_base::insertProperty(new qi_property_string_program(m_hprogram, uiprop, raw));
	}
	else if (IS_UIPROP_LANG(uiprop->type))
	{

	}
	else qi_properties_base::insertProperty(uiprop, raw);
}

/* qi_muxstream */

qi_muxstream::qi_muxstream(hmux_stream hstream) :
	m_hstream(hstream),
	m_stream(muxstream_get(hstream)),
	m_esbuf(NULL),
	m_eslen(0)
{
	m_name = QString::asprintf("stream [%04x:%02x]", _raw_stream()->stream_pid, _raw_stream()->stream_type);
	muxstream_get_es_info(hstream, &m_esbuf, &m_eslen);
}

/* qi_muxprogram */

qi_muxprogram::qi_muxprogram(hmux_program hprogram) :
	m_hprogram(hprogram),
	m_program(muxprogram_get(hprogram)),
	m_properties(new qi_properties_program(this,hprogram))
{
	refreshItem();
	m_name = QString::asprintf("program [%04x:%04x]", _pcr_pid(), _pmt_pid());
}

qi_muxprogram::~qi_muxprogram()
{
	while (m_streams.count())
	{
		qtvMuxStream* pstream = m_streams.first();
		delete pstream;
		m_streams.removeFirst();
	}
}

void qi_muxprogram::refreshItem()
{
	hmux_stream hstream = muxprogram_get_first_stream(m_program);
	while (hstream)
	{
		m_streams.append(new qi_muxstream(hstream));
		hstream = muxprogram_get_next_stream(hstream);
	}
}

/* qi_muxchannel */

qi_muxchannel::qi_muxchannel(hmux_channel hchannel) :
	m_hchannel(hchannel),
	m_properties(new qi_properties_channal(this,hchannel))
{
	refreshItem();
	m_name = QString("channel");
}

qi_muxchannel::~qi_muxchannel()
{
	while (m_programs.count())
	{
		qtvMuxProgram* pprogram = m_programs.first();
		delete pprogram;
		m_programs.removeFirst();
	}
	delete m_properties;
}

void qi_muxchannel::refreshItem()
{
	hmux_program hprog = muxchannel_get_first_program(m_hchannel);
	while (hprog)
	{
		m_programs.append(new qi_muxprogram(hprog));
		hprog = muxchannel_get_next_program(hprog);
	}
}

/* qi_muxdefault_service */

qi_muxdefault_service::qi_muxdefault_service(hmux_core hmux, hmux_channel hch) :
	m_hmux(hmux),
	m_hchannel(hch),
	m_channel(new qi_muxchannel(hch))
{

}

qi_muxdefault_service::~qi_muxdefault_service()
{
	delete m_channel;
}

vatek_result qi_muxdefault_service::applyDevice(qtvDevice* device)
{
	vatek_result nres = vatek_badstatus;
	if (device->_chip_info()->status == chip_status_waitcmd)
	{
		nres = mux_handle_set_hal(m_hmux, device->_handle());
	}
	return nres;
}

/* qi_muxdefault_channel */

qi_muxdefault_channel::qi_muxdefault_channel(hmux_core hmux) :
	m_hmux(hmux),
	m_hchannel(NULL),
	m_hprogram(NULL),
	m_channel(NULL)
{

}

qi_muxdefault_channel::~qi_muxdefault_channel()
{
	resetContext();
	mux_handle_free(m_hmux);
}

vatek_result qi_muxdefault_channel::resetChannel(mux_spec_mode mode, mux_country_code country)
{
	vatek_result nres = vatek_success;
	resetContext();
	nres = mux_open_channel_default(m_hmux, mode, country, &m_hchannel);
	return nres;
}

vatek_result qi_muxdefault_channel::createProgram(uint16_t pcrpid, uint16_t pmtpid)
{
	vatek_result nres = vatek_badstatus;
	if (m_hchannel && !m_hprogram)
	{
		nres = muxchannel_add_program(m_hchannel, pmtpid, pcrpid, &m_hprogram);
	}
	return nres;
}

vatek_result qi_muxdefault_channel::pushStream(uint16_t pid, Pmedia_video video, uint8_t* esinfo, int32_t eslen)
{
	vatek_result nres = vatek_badstatus;
	if (m_hprogram)
	{
		hmux_stream hstream = NULL;
		nres = muxprogram_add_video(m_hprogram, pid, video, &hstream);
		if (is_vatek_success(nres))
		{
			if (esinfo && eslen)muxstream_set_es_info(hstream, esinfo, eslen);
		}
	}
	return nres;
}

vatek_result qi_muxdefault_channel::pushStream(uint16_t pid, Pmedia_audio audio, uint8_t* esinfo, int32_t eslen)
{
	vatek_result nres = vatek_badstatus;
	if (m_hprogram)
	{
		hmux_stream hstream = NULL;
		nres = muxprogram_add_audio(m_hprogram, pid, audio, &hstream);
		if (is_vatek_success(nres))
		{
			if (esinfo && eslen)muxstream_set_es_info(hstream, esinfo, eslen);
		}
	}
	return nres;
}

vatek_result qi_muxdefault_channel::pushStream(uint16_t pid, uint8_t streamtype, uint8_t* esinfo, int32_t eslen)
{
	vatek_result nres = vatek_badstatus;
	if (m_hprogram)
	{
		hmux_stream hstream = NULL;
		nres = muxprogram_add_stream(m_hprogram, pid, streamtype, &hstream);
		if (is_vatek_success(nres))
		{
			if (esinfo && eslen)muxstream_set_es_info(hstream, esinfo, eslen);
		}
	}
	return nres;
}

vatek_result qi_muxdefault_channel::commitProgram()
{
	vatek_result nres = vatek_badstatus;
	if (m_hprogram)
	{
		nres = muxprogram_get_stream_nums(m_hprogram);
		if (nres == vatek_success)nres = vatek_badparam;
		if (is_vatek_success(nres))m_hprogram = NULL;
	}
	return nres;
}

vatek_result qi_muxdefault_channel::finishChannel()
{
	vatek_result nres = vatek_badstatus;
	if (m_hchannel && !m_hprogram)
	{
		nres = muxchannel_get_program_nums(m_hchannel);
		if (nres == vatek_success)nres = vatek_badparam;

		if (is_vatek_success(nres))
		{
			m_channel = new qi_muxchannel(m_hchannel);
		}
	}
	return nres;
}

void qi_muxdefault_channel::resetContext()
{
	if (m_channel)delete m_channel;
	mux_handle_reset(m_hmux);
	m_channel = NULL;
	m_hchannel = NULL;
	m_hprogram = NULL;
}

vatek_result qi_muxdefault_channel::applyDevice(qtvDevice* device)
{
	vatek_result nres = vatek_badstatus;
	if (device->_chip_info()->status == chip_status_waitcmd)
	{
		nres = mux_handle_set_hal(m_hmux, device->_handle());
	}
	return nres;
}

/* qi_muxrawtable_service */

qi_muxrawtable_service::qi_muxrawtable_service(hmux_rawtable hraw) :
	m_hrawtable(hraw)
{

}

qi_muxrawtable_service::~qi_muxrawtable_service()
{
	while (m_rawtables.count())
	{
		qtvMuxRawTable* praw = m_rawtables.first();
		delete praw;
		m_rawtables.removeFirst();
	}
}

vatek_result qi_muxrawtable_service::insertTable(int32_t intervalms, int32_t nums, uint8_t* buffer)
{
	Ppsi_table_raw prawtable = NULL;
	vatek_result nres = muxraw_create_table(m_hrawtable, nums, intervalms, &prawtable);
	if (is_vatek_success(nres))
	{
		prawtable->interval_ms = intervalms;
		prawtable->packet_nums = nums;
		memcpy(&prawtable->packet_buf[0], buffer, nums * TS_PACKET_LEN);
		nres = muxraw_push_table(m_hrawtable, prawtable);
		if (is_vatek_success(nres))m_rawtables.append(new qi_muxrawtable(prawtable));
	}
	return nres;
}

/* qtvMuxServiceDefault */

vatek_result qtvMuxServiceDefault::createBroadcastMux(mux_spec_mode spec,
													  mux_country_code country,
													  uint16_t pcrpid,
													  Pencoder_param penc,
													  qtvMuxServiceDefault** muxhandle)
{
	hmux_core hmux = NULL;
	vatek_result nres = mux_handle_create(&hmux);
	if (is_vatek_success(nres))
	{
		hmux_broadcast hbc = NULL;
		nres = mux_open_broadcast_default(hmux, pcrpid, penc, spec, country, &hbc);
		if (is_vatek_success(nres))
		{
			hmux_channel hch = NULL;
			nres = mux_handle_get_channel(hmux, &hch);

			if (is_vatek_success(nres))
				*muxhandle = new qi_muxdefault_service(hmux, hch);
		}
		if (!is_vatek_success(nres))mux_handle_free(hmux);
	}
	return nres;
}

/* qtvMuxServiceChannel */

vatek_result qtvMuxServiceChannel::createChannel(qtvMuxServiceChannel** muxchannel)
{
	hmux_core hmux = NULL;
	vatek_result nres = mux_handle_create(&hmux);
	if (is_vatek_success(nres))
	{
		*muxchannel = new qi_muxdefault_channel(hmux);
	}
	return nres;
}
