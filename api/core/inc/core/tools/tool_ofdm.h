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

#ifndef _TOOL_OFDM_
#define _TOOL_OFDM_

#include <core/base/output_modulator.h>
#include <mux/mux_define.h>
#include <core/ui/ui_props_define.h>

typedef struct _ofdm_frame
{
	uint32_t frametick;
	uint32_t packetnums;
	uint32_t framerecv;
}ofdm_frame;

typedef ofdm_frame* Pofdm_frame;

typedef struct _ofdm_position
{
	int32_t packetnums;
	int32_t packetptr;
	int32_t pkttick;
	uint32_t bitrecv;
}ofdm_position;

typedef ofdm_position* Pofdm_position;

typedef vatek_result(*fpofdm_check_param)(Pmodulator_param pmod);
typedef uint32_t(*fpofdm_get_bitrate)(Pmodulator_param pmod);

typedef struct _ofdm_modulation
{
	int32_t defaultsize;
	const uint8_t* defaultparam;
	const Pui_prop_item uiprops;
	fpofdm_get_bitrate getbitrate;
	fpofdm_check_param checkparam;
}ofdm_modulation;

typedef ofdm_modulation* Pofdm_modulation;

const static uint8_t GI_RATE[][2] = { {32,33,},{16,17,},{8,9,},{4,5,}, };
const static uint8_t CR_RATE[][2] = { {1,2,},{2,3,},{3,4,},{5,6,},{7,8,}, };

#ifdef __cplusplus
extern "C" {
#endif

	HAL_API vatek_result tool_ofdm_get_modulation(modulator_type type,Pofdm_modulation* pofdm);

#ifdef __cplusplus
}
#endif

#endif
