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

#ifndef _MUX_RULE_
#define _MUX_RULE_

#include <mux/mux_spec.h>

/* deprecated only used with before v1.2 not supported muxpsi_api*/
#define MUX_RULE_HDR_TAG				"mazo-r0"
#define MUX_RULE_HDR_TAG_V1				"mazo-r1"
#define MUX_RULE_HDR_LEN				128

#define RULE_EN_TOT						0x00000001
#define RULE_EN_EIT						0x00000002

#define RULE_EPG_LEN					512
#define RULE_EPG_TAG					0xF8F40007

typedef struct _mux_rule_block{
	uint32_t rule_runction;
	uint8_t* rawblock;
	uint32_t block_len;		
	uint32_t channel_pos;
	uint32_t channel_len;
	uint32_t program_pos;
	uint32_t program_len;
	mux_spec_mode spec;
	mux_country_code country;
}mux_rule_block;

typedef mux_rule_block* Pmux_rule_block;

#define is_mux_rule_en_tot(rule)	((rule->rule_runction & RULE_EN_TOT) == RULE_EN_TOT)
#define is_mux_rule_en_eit(rule)	((rule->rule_runction & RULE_EN_EIT) == RULE_EN_EIT)

#ifdef __cplusplus
extern "C" {
#endif

	HAL_API vatek_result muxrule_load_file(Pth_mempool pmempool,const char* filename, Pmux_rule_block* prule);
	HAL_API vatek_result muxrule_load_raw(Pth_mempool pmempool, uint8_t* praw, int32_t len, Pmux_rule_block* prule);

	HAL_API vatek_result muxrule_check_valid(uint8_t* praw);

	HAL_API mux_spec_mode muxrule_get_spec(Pmux_rule_block prule);
	HAL_API mux_country_code muxrule_get_country(Pmux_rule_block prule);

	HAL_API vatek_result muxrule_malloc_uiprops(Pth_mempool pmempool, Pmux_ui_props puiprops, uint8_t** pbufprop);
	HAL_API vatek_result muxrule_get_uiprops(Pui_prop_item puiprops, uint8_t* prulebuf, uint8_t* pdest);
	HAL_API vatek_result muxrule_set_uiprops(Pui_prop_item puiprops, uint8_t* prulebuf, uint8_t* psur);

	HAL_API vatek_result muxrule_copy_channel(Pmux_rule_block prule,uint8_t* pbuf,int32_t len);
	HAL_API vatek_result muxrule_copy_program(Pmux_rule_block prule, uint8_t* pbuf, int32_t len);
	HAL_API vatek_result muxrule_get_channel_param(Pmux_rule_block prule, Pspec_channel pch);
	HAL_API vatek_result muxrule_get_program_param(Pmux_rule_block prule, Pspec_program prog);
	HAL_API vatek_result muxrule_set_channel_param(Pmux_rule_block prule, Pspec_channel pch);
	HAL_API vatek_result muxrule_set_program_param(Pmux_rule_block prule, Pspec_program prog);

	HAL_API vatek_result muxrule_set_channel_rawbuf(Pspec_channel pch, uint8_t* pdest);
	HAL_API vatek_result muxrule_set_program_rawbuf(Pspec_program pprog, uint8_t* pdest);
	HAL_API vatek_result muxrule_get_channel_rawbuf(Pspec_channel pch, uint8_t* psur);
	HAL_API vatek_result muxrule_get_program_rawbuf(Pspec_program pprog,uint8_t* psur);

#ifdef __cplusplus
}
#endif

#endif
