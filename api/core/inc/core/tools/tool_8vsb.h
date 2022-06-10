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

#ifndef _TOOL_8VSB_
#define _TOOL_8VSB_

#include <core/tools/tool_ofdm.h>

#define ATSC_8VSB_SYMBOLRATE	10762238	
#define VSB_SEGMENT_SB_NUMS		832
#define VSB_FIELD_SEGMENT_NUMS	313
#define VSB_FRAME_FIELD_NUMS	2

#define VSB_FIELD_PACKET_NUMS	(VSB_FIELD_SEGMENT_NUMS - 1)
#define VSB_FRAME_PACKET_NUMS	(VSB_FIELD_PACKET_NUMS * VSB_FRAME_FIELD_NUMS)

#ifdef __cplusplus
extern "C" {
#endif

	HAL_API vatek_result tool_8vsb_vsbframe_reset(Pmodulator_param pmod, Pofdm_frame pframe);
	HAL_API uint32_t tool_atsc_get_bitrate(Pmodulator_param pmod);
	HAL_API vatek_result tool_atsc_check_param(Pmodulator_param pmod);

#ifdef __cplusplus
}
#endif

#endif
