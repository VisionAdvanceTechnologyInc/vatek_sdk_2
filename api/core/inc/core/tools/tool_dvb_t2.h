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

#ifndef _TOOL_DVB_T2_
#define _TOOL_DVB_T2_

#include <core/tools/tool_ofdm.h>

#define DVB_T2_SUPERFRAME_NUMS          2
#define DVB_T2_PLP_NUMS                 1
#define DVB_T2_AUX_NUMS                 0

/* dvb_t2 implement with fixed l1 fixed len (1 _plp, no aux and _fef)*/
#define L1_POST_CONFIG_LEN              (35 + 35 + (89 * DVB_T2_PLP_NUMS) + 32 + (DVB_T2_AUX_NUMS * 32)) 
#define L1_POST_DYNAMIC_LEN             (71 + (48 * DVB_T2_PLP_NUMS) + 8 + (DVB_T2_AUX_NUMS*48))
#define L1_POST_EXTENSION_LEN           0
#define L1_POST_INFO_SIZE               (L1_POST_CONFIG_LEN + L1_POST_DYNAMIC_LEN + L1_POST_EXTENSION_LEN)

/* L1 */
#define KBCH_1_2        7032
#define KSIG_POST       (L1_POST_INFO_SIZE + 32) /* L1_POST_INFO_SIZE + 32 (crc32)*/
#define NBCH_PARITY     168

#define N_punc_temp		((6 * (KBCH_1_2 - KSIG_POST)) / 5)
#define N_post_temp		(KSIG_POST + NBCH_PARITY + 9000 - N_punc_temp)

typedef struct _dvb_t2_cell
{
    uint16_t bch;
    uint16_t cells;
    uint8_t etamod;
    uint8_t n_p2;
    uint16_t c_p2;
    uint16_t c_data;
    uint16_t n_fc;
    uint16_t c_fc;
    uint16_t d_l1;
    uint16_t n_post;
    uint16_t recv;
}dvb_t2_cell,*Pdvb_t2_cell;

typedef struct _dvb_t2_frame
{
    ofdm_frame ofdm;
    dvb_t2_cell cell;
}dvb_t2_frame,* Pdvb_t2_frame;


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    HAL_API vatek_result tool_dvb_t2_frame_reset(Pmodulator_param pmod, Pdvb_t2_frame pframe);
	HAL_API uint32_t tool_dvb_t2_get_bitrate(Pmodulator_param pmod);
	HAL_API vatek_result tool_dvb_t2_check_param(Pmodulator_param pmod);

    HAL_API void tool_dvb_t2_get_cell(Pmodulator_param pmod, Pdvb_t2_cell pcell);

    /* get t2 max symbol numbers */
    HAL_API vatek_result tool_dvb_t2_get_symbol_nums_max(Pmodulator_param pmod,Pdvb_t2_cell pcell);
    HAL_API vatek_result tool_dvb_t2_get_symbol_nums(Pmodulator_param pmod, Pdvb_t2_cell pcell);
    HAL_API vatek_result tool_dvb_t2_get_fecblock_nums(Pdvb_t2_cell pcell, int32_t symbolnums);
    HAL_API int32_t tool_dvb_t2_get_L1_size(Pdvb_t2_cell pcell);

    HAL_API int32_t tool_dvb_t2_get_n_punc(Pdvb_t2_cell pcell);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
