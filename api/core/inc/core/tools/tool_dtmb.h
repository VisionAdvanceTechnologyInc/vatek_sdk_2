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

#ifndef _TOOL_DTMB_
#define _TOOL_DTMB_

#include <core/tools/tool_ofdm.h>

#if 0

typedef struct _dtmb_superframe
{
	uint32_t symbolrate;
	uint32_t sframe_pktnums;
	uint32_t sframe_tick;
	uint32_t sframe_pkttick;
}dtmb_superframe;

typedef dtmb_superframe* Pdtmb_superframe;

#endif

#ifdef __cplusplus
extern "C" {
#endif

	HAL_API vatek_result tool_dtmb_superframe_reset(Pmodulator_param pmod, Pofdm_frame pframe);

#if 0
	HAL_API vatek_result tool_dtmb_superframe_reset(Pmodulator_param pmod, Pdtmb_superframe psframe);
	HAL_API vatek_result tool_dtmb_superframe_get_tick(Pdtmb_superframe psframe, int32_t pktpos);
	HAL_API void tool_dtmb_superframe_append_frame(Pdtmb_superframe psframe, Pmux_clock_tick pclk);
#endif

	HAL_API uint32_t tool_dtmb_get_bitrate(Pmodulator_param pmod);
	HAL_API vatek_result tool_dtmb_check_param(Pmodulator_param pmod);

#ifdef __cplusplus
}
#endif

#endif
