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

#ifndef _TOOL_OUTPUT_
#define _TOOL_OUTPUT_

#include <core/tools/tool_ofdm.h>
#include <core/tools/tool_helpful.h>

#define OUTCLK_DEFAULT_PCRINTERVAL_MS		40

typedef void* houtput_clock;

#ifdef __cplusplus
extern "C" {
#endif

	HAL_API vatek_result tool_output_clock_create(Pmodulator_param pmod, houtput_clock* houtclk);
	HAL_API vatek_result tool_output_clock_create_mempool(Pmodulator_param pmod,Pth_mempool pmem, houtput_clock* houtclk);
	HAL_API vatek_result tool_output_clock_config(houtput_clock houtclk, Pmodulator_param pmod);
	HAL_API void tool_output_clock_set_tick(houtput_clock houtclk,Pmux_clock_tick pclk);

	HAL_API void tool_output_clock_adjust_tick(houtput_clock houtclk,int32_t tick27mhz);

	HAL_API Pmux_clock_tick tool_output_clock_append(houtput_clock houtclk);
	HAL_API vatek_result tool_output_clock_get_pcrpacket(houtput_clock houtclk, int32_t* intervalms);
	HAL_API Pmux_clock_tick tool_output_clock_get_tick(houtput_clock houtclk);

	HAL_API vatek_result tool_output_clock_reset(houtput_clock houtclk);
	HAL_API void tool_output_clock_free(houtput_clock houtclk);

#ifdef __cplusplus
}
#endif

#endif
