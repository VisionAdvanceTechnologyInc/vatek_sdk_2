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

#ifndef _MUX_HANDLE_
#define _MUX_HANDLE_

#include <mux/mux_define.h>
#include <mux/mux_epg.h>
#include <mux/mux_spec.h>
#include <mux/mux_rule.h>
#include <core/base/stream_encoder.h>

typedef void* hmux_core;
typedef void* hmux_broadcast;
typedef void* hmux_channel;
typedef void* hmux_rawtable;
typedef void* hmux_program;
typedef void* hmux_stream;

typedef void* mux_position;

#ifdef __cplusplus
extern "C" {
#endif

	HAL_API vatek_result mux_handle_create(hmux_core* hmux);
	HAL_API void mux_handle_reset(hmux_core hmux);
	HAL_API void mux_handle_free(hmux_core hmux);

	HAL_API vatek_result mux_handle_set_hal(hmux_core hmux, hvatek_chip hchip);
	HAL_API vatek_result mux_handle_get_hal_broadcast(hmux_core hmux, hvatek_chip hchip,uint16_t pcrpid,Pencoder_param penc);
	HAL_API vatek_result mux_handle_get_hal_channel(hmux_core hmux, hvatek_chip hchip);

	HAL_API vatek_result mux_open_channel_rule(hmux_core hmux,const char* filename,hmux_channel* hchannel);
	HAL_API vatek_result mux_open_channel_rule_raw(hmux_core hmux, Pmux_rule_block pblock, hmux_channel* hchannel);
	HAL_API vatek_result mux_open_channel_default(hmux_core hmux, mux_spec_mode spec, mux_country_code country,hmux_channel* hchannel);

	HAL_API vatek_result mux_open_raw(hmux_core hmux,hmux_rawtable* hraw);
	HAL_API vatek_result muxraw_get_handle(hmux_core hmux, hmux_rawtable* hraw);
	HAL_API vatek_result muxraw_create_table(hmux_rawtable hraw, int32_t packetnums, int32_t intervalms,Ppsi_table_raw* ptable);
	HAL_API	vatek_result muxraw_push_table(hmux_rawtable hraw, Ppsi_table_raw ptable);
	HAL_API vatek_result muxraw_get_table(hmux_rawtable hraw, Ppsi_table_raw* ptables);

	HAL_API vatek_result mux_open_broadcast_rule(hmux_core hmux,uint16_t pcrpid,Pencoder_param penc,const char* filename,hmux_broadcast* hbc);
	HAL_API vatek_result mux_open_broadcast_rule_raw(hmux_core hmux, uint16_t pcrpid, Pencoder_param penc, Pmux_rule_block prule, hmux_broadcast* hbc);
	HAL_API vatek_result mux_open_broadcast_default(hmux_core hmux, uint16_t pcrpid, Pencoder_param penc, mux_spec_mode spec, mux_country_code country,hmux_broadcast* hbc);
	HAL_API vatek_result muxbroadcast_get_handle(hmux_core hmux, hmux_broadcast* hbc);
	HAL_API vatek_result muxbroadcast_get_channel(hmux_broadcast hbc,Pspec_channel* pch);
	HAL_API vatek_result muxbroadcast_get_program(hmux_broadcast hbc,Pspec_program* pprogram);
	HAL_API vatek_result muxbroadcast_get_program_handle(hmux_broadcast hbc, hmux_program* hprog);

	HAL_API vatek_result muxbroadcast_add_stream(hmux_broadcast hbc, uint16_t pid, uint8_t streamtype, uint8_t* pesinfo, int32_t eslen);
	HAL_API vatek_result muxbroadcast_add_epg(hmux_broadcast hbc,Pmux_epg_param pepg);

	/* hmux_chnnel tools */
	HAL_API vatek_result mux_handle_get_channel(hmux_core hmux, hmux_channel* hch);
	HAL_API vatek_result mux_handle_get_rule(hmux_core hmux, Pmux_rule_block* prule);
	HAL_API uint8_t* mux_handle_malloc_buffer(hmux_core hmux, int32_t len);
	HAL_API mux_psi_mode mux_handle_get_mode(hmux_core hmux);
	HAL_API mux_spec_mode mux_handle_get_spec(hmux_core hmux);
	HAL_API mux_country_code mux_handle_get_country(hmux_core hmux);
	HAL_API Pth_mempool mux_handle_get_mempool(hmux_core hmux);
	HAL_API vatek_result mux_handle_malloc_epgparam(hmux_core hmux, mux_epg_mode mode, Pmux_epg_param* pepg);

	HAL_API vatek_result muxchannel_get_param(hmux_channel hch, Pspec_channel* pchannel);
	HAL_API vatek_result muxchannel_replace_param(hmux_channel hch, Pspec_channel pchannel);
	HAL_API vatek_result muxchannel_add_program(hmux_channel hch, uint16_t pmtpid, uint16_t pcrpid, hmux_program* hprog);
	HAL_API hmux_program muxchannel_get_first_program(hmux_channel hch);
	HAL_API hmux_program muxchannel_get_next_program(hmux_program hprog);
	HAL_API vatek_result muxchannel_get_program_nums(hmux_channel hch);

	HAL_API vatek_result muxprogram_malloc_epg(hmux_program hprog, mux_epg_mode mode, Pmux_epg_param* pepg);
	HAL_API vatek_result muxprogram_set_epg(hmux_program hprog, Pmux_epg_param pepg);
	HAL_API vatek_result muxprogram_add_video(hmux_program hprog, uint16_t pid, Pmedia_video pvideo, hmux_stream* hstream);
	HAL_API vatek_result muxprogram_add_audio(hmux_program hprog, uint16_t pid, Pmedia_audio paudio, hmux_stream* hstream);
	HAL_API vatek_result muxprogram_add_stream(hmux_program hprog, uint16_t pid, uint8_t streamtype, hmux_stream* hstream);

	HAL_API Pspec_program muxprogram_get_param(hmux_program hprog);
	HAL_API vatek_result muxprogram_replace_param(hmux_program hprog, Pspec_program pprogram);
	HAL_API Pmux_program muxprogram_get(hmux_program hprog);
	HAL_API Pmux_epg_param muxprogram_get_epg(hmux_program hprog);

	HAL_API hmux_stream muxprogram_get_first_stream(hmux_program hprog);
	HAL_API vatek_result muxprogram_get_stream_nums(hmux_program hprog);
	HAL_API hmux_stream muxprogram_get_next_stream(hmux_stream hstream);

	HAL_API Pmux_stream muxstream_get(hmux_stream hstream);
	HAL_API void muxstream_set_es_info(hmux_stream hstream, uint8_t* pesinfo, int32_t len);
	HAL_API void muxstream_get_es_info(hmux_stream hstream, uint8_t** pesinfo, int32_t* len);
	HAL_API void muxstream_set_private(hmux_stream hstream, void* pdata);
	HAL_API void* muxstream_get_private(hmux_stream hstream);

	HAL_API uint8_t muxtool_get_video_streamtype(video_codec vcodec);
	HAL_API uint8_t muxtool_get_audio_streamtype(audio_codec acodec);
	
#ifdef __cplusplus
}
#endif

#endif
