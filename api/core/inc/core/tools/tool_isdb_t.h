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

#ifndef _TOOL_ISDB_T_
#define _TOOL_ISDB_T_

#include <core/tools/tool_ofdm.h>

#define ISDB_T_6MHZ_SYMBOL		8126984 /* 512/63 */
#define ISDB_T_TSP_MAXNUMS		(3276 >> 2)

typedef struct _isdb_t_bts_frame
{
	ofdm_frame ofdm;
	int32_t frameus;
	uint32_t frame_sb;
	uint16_t tsp_maps[ISDB_T_TSP_MAXNUMS];
}isdb_t_bts_frame;

typedef isdb_t_bts_frame* Pisdb_t_bts_frame;

#ifdef __cplusplus
extern "C" {
#endif

	HAL_API vatek_result tool_isdb_t_bts_frame_reset(Pmodulator_param pmod, Pisdb_t_bts_frame pbts);
	HAL_API vatek_result tool_isdb_t_bts_frame_get_tick(Pisdb_t_bts_frame pbts, int32_t pktpos);

	HAL_API uint32_t tool_isdb_t_get_bitrate(Pmodulator_param pmod);
	HAL_API vatek_result tool_isdb_t_check_param(Pmodulator_param pmod);

#ifdef __cplusplus
}
#endif

#endif
