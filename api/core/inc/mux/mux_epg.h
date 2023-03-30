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

#ifndef _MUX_EPG_
#define _MUX_EPG_

#include <mux/mux_time.h>

/* first section started at current hour (default start at 00:00:00)*/
#define EPG_FLAG_START_HOUR				0x00000001	
#define EPG_TITLE_MAX_LEN				256
#define EPG_DESC_MAX_LEN				64
#define EPG_CONTENT_MAX_LEN				512

typedef enum _mux_epg_mode{
	epg_mode_null = -1,
	epg_mode_dvb = 0,
	epg_mode_arib_0 = 1,
	epg_mode_arib_1 = 2,
	epg_mode_simple = 3,
}mux_epg_mode;

#define  is_current_mux_epg_mode(mode)	(mode >= epg_mode_dvb && mode <= epg_mode_arib_1)

typedef enum _mux_content_type{
	mux_content_undefine = 0,
	mux_content_news = 1,
	mux_content_sport = 2,
	mux_content_movie = 3,
	mux_content_drama = 4,
	mux_content_music = 5,
	mux_content_tabloidshow = 6,
	mux_content_varietyshow = 7,
	mux_content_animation = 8,
	mux_content_documentary = 9,
	mux_content_performance = 10,
	mux_content_education = 11,
	mux_content_welfare = 12,
	mux_content_others = 0xFF,
}mux_content_type;

typedef enum _mux_parental_rating{
	rating_disable = 0,
	rating_general = 1,
	rating_guidance = 2,
	rating_restricted = 3,
}mux_parental_rating;

typedef struct _mux_epg_section{
	struct _mux_epg_section* next;
	uint8_t duration;
	uint8_t recv[3];
	mux_content_type type;
	mux_parental_rating parental_rating;
	mux_string title;
	mux_string content;
}mux_epg_section;

typedef mux_epg_section* Pmux_epg_section;

typedef struct _mux_epg_param{
	mux_epg_mode mode;
	mux_time start_time;
	uint16_t event_id;
	uint16_t recv;
	Pmux_epg_section sections;
}mux_epg_param;

typedef mux_epg_param* Pmux_epg_param;

#ifdef __cplusplus
extern "C" {
#endif

	HAL_API vatek_result muxepg_create(Pth_mempool pmem, mux_epg_mode mode, Pmux_epg_param* pepg);
	HAL_API vatek_result muxepg_check(Pmux_epg_param pepg);

#ifdef __cplusplus
}
#endif

#endif
