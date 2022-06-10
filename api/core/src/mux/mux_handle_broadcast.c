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

#include <mux/mux_handle.h>
#include "internal/mux_core.h"

extern vatek_result broadcast_add_context(uint16_t pcrpid, Pencoder_param penc, hmux_channel hch);

vatek_result mux_open_broadcast_rule(hmux_core hmux, uint16_t pcrpid, Pencoder_param penc, const char* filename, hmux_broadcast* hbc)
{
    hmux_channel hch = NULL;
    vatek_result nres = mux_open_channel_rule(hmux, filename, &hch);
    if (is_vatek_success(nres))
    {
		Phandle_mux pmux = (Phandle_mux)hmux;
		pmux->hmode = hmux_mode_broadcast;
		nres = broadcast_add_context(pcrpid, penc, hch);
        if (is_vatek_success(nres))*hbc = pmux;
    }
    return nres;
}

vatek_result mux_open_broadcast_rule_raw(hmux_core hmux, uint16_t pcrpid, Pencoder_param penc, Pmux_rule_block prule, hmux_broadcast* hbc)
{
	hmux_channel hch = NULL;
	vatek_result nres = mux_open_channel_rule_raw(hmux, prule, &hch);
	if (is_vatek_success(nres))
	{
		Phandle_mux pmux = (Phandle_mux)hmux;
		pmux->hmode = hmux_mode_broadcast;
		nres = broadcast_add_context(pcrpid, penc, hch);
		if (is_vatek_success(nres))*hbc = pmux;
	}
	return nres;
}

vatek_result mux_open_broadcast_default(hmux_core hmux, uint16_t pcrpid, Pencoder_param penc, mux_spec_mode spec, mux_country_code country, hmux_broadcast* hbc)
{
    hmux_channel hch = NULL;
    vatek_result nres = mux_open_channel_default(hmux, spec, country, &hch);
    if (is_vatek_success(nres))
    {
        Phandle_mux pmux = (Phandle_mux)hmux;
        pmux->hmode = hmux_mode_broadcast;
        nres = broadcast_add_context(pcrpid, penc, hch);
        if (is_vatek_success(nres))
        {
            if (hbc)*hbc = pmux;
        }
    }
    return nres;
}

vatek_result muxbroadcast_get_handle(hmux_core hmux, hmux_broadcast* hbc)
{
    vatek_result nres = vatek_badstatus;
    Phandle_mux pmux = (Phandle_mux)hmux;
    if (pmux->hmode == hmux_mode_broadcast)
    {
        *hbc = pmux;
        nres = vatek_success;
    }
    return nres;
}

vatek_result muxbroadcast_get_channel(hmux_broadcast hbc, Pspec_channel* pch)
{
    hmux_channel hch = NULL;
    vatek_result nres = mux_handle_get_channel((hmux_core)hbc, &hch);
    if (is_vatek_success(nres))
    {
        nres = muxchannel_get_param(hch,pch);
    }
    return nres;
}

vatek_result muxbroadcast_get_program(hmux_broadcast hbc, Pspec_program* pprogram)
{
	hmux_channel hch = NULL;
	vatek_result nres = mux_handle_get_channel((hmux_core)hbc, &hch);
	if (is_vatek_success(nres))
	{
        hmux_program hprog = muxchannel_get_first_program(hch);
        if (hprog)*pprogram = muxprogram_get_param(hprog);
	}
	return nres;
}

vatek_result muxbroadcast_get_program_handle(hmux_broadcast hbc, hmux_program* hprog)
{
	hmux_channel hch = NULL;
	vatek_result nres = mux_handle_get_channel((hmux_core)hbc, &hch);
	if (is_vatek_success(nres))*hprog = muxchannel_get_first_program(hch);
    return nres;
}

vatek_result muxbroadcast_add_stream(hmux_broadcast hbc, uint16_t pid, uint8_t streamtype, uint8_t* pesinfo, int32_t eslen)
{
	hmux_channel hch = NULL;
	vatek_result nres = mux_handle_get_channel((hmux_core)hbc, &hch);
    if (is_vatek_success(nres))
    {
        hmux_program hprog = muxchannel_get_first_program(hch);
        nres = vatek_badstatus;
        if (hprog)
        {
            hmux_stream hstream = NULL;
            nres = muxprogram_add_stream(hprog, pid, streamtype, &hstream);
            if (is_vatek_success(nres))muxstream_set_es_info(hstream, pesinfo, eslen);
        }
    }
    return nres;
}

vatek_result muxbroadcast_add_epg(hmux_broadcast hbc, Pmux_epg_param pepg)
{
	hmux_channel hch = NULL;
	vatek_result nres = mux_handle_get_channel((hmux_core)hbc, &hch);
	if (is_vatek_success(nres))
	{
		hmux_program hprog = muxchannel_get_first_program(hch);
        if (hprog)nres = muxprogram_set_epg(hprog, pepg);
	}
	return nres;
}

vatek_result broadcast_add_context(uint16_t pcrpid, Pencoder_param penc, hmux_channel hch)
{
	hmux_program hprog = NULL;
	vatek_result nres = muxchannel_add_program(hch, penc->pmt_pid, pcrpid, &hprog);
	if (is_vatek_success(nres))
	{
		hmux_stream hstream = NULL;
		nres = muxprogram_add_video(hprog, penc->video_pid, &penc->video, &hstream);
		if (is_vatek_success(nres))
			nres = muxprogram_add_audio(hprog, penc->audio_pid, &penc->audio, &hstream);
	}
    return nres;
}
