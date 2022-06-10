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

#include <core/tools/tool_8vsb.h>

/**
 *
	Standard MPEG transport packet = 187 bytes
	Add 20 bytes of Reed Solomon parity code = 207 bytes
	207 bytes X 8 = 1656 bits
	Trellis coding – For every 2 bits of data a third bit is added for error
	correction
	1656 bits X 3/2 = 2484 bits
	1 8VSB symbol = 3 bits
	2484 bits / 3 = 828 symbols
	1 8VSB data segment = 828 symbols + 4 symbols of segment sync =
	832 symbols per segment


	2 data fields = 1 data frame
	313 segments = 1 data field
	626 segments = 1 data frame
	First segment in each data field carries field sync
	information and the 511 symbol training signal used by the receiver
	equalizer
	10,766,080 symbols(sec) / 832 symbols(segment) =
	12940 segments/ sec
	12940 segments/sec // 626 segments/frame = 20.67 frames / sec

 */

#if 0
	#define VSBFRAME_FRAME_SB		(VSB_SEGMENT_SB_NUMS * VSB_FIELD_SEGMENT_NUMS * VSB_FRAME_FIELD_NUMS)
	#define VSBFRAME_27MHZ_TICK		(1306648) /* 1,306,648.6728875 */
	#define VSBPACKET_27MHZ_TICK	(2094)
#endif

#define VSBFRAME_SYMBOLRATE			((VSB_SEGMENT_SB_NUMS * VSB_FIELD_SEGMENT_NUMS) * VSB_FRAME_FIELD_NUMS)
#define VSBATSC_BITRATE				19392550

vatek_result tool_8vsb_vsbframe_reset(Pmodulator_param pmod, Pofdm_frame pframe)
{
	vatek_result nres = tool_atsc_check_param(pmod);
	if (is_vatek_success(nres))
	{
		pframe->frametick = (uint32_t)(((uint64_t)VSBFRAME_SYMBOLRATE * 27000000)/ATSC_8VSB_SYMBOLRATE);
		pframe->frametick += 1;
		pframe->packetnums = (VSB_FIELD_SEGMENT_NUMS * 2) - VSB_FRAME_FIELD_NUMS;
		pframe->framerecv = 0;
	}
	return nres;
}

#if 0

vatek_result tool_8vsb_get_vsbframe_reset(Pmodulator_param pmod, Pvsb_dataframe pvsb)
{
	vatek_result nres = tool_atsc_check_param(pmod);
	if (is_vatek_success(nres))
	{
		pvsb->symbolrate = ATSC_8VSB_SYMBOLRATE;
		pvsb->vsb_framepkts = VSB_FRAME_PACKET_NUMS;
		pvsb->vsb_packettick = VSBPACKET_27MHZ_TICK;
		nres = vatek_success;
	}
	return nres;
}

uint32_t tool_8vsb_get_vsbframe_tick(Pvsb_dataframe pvsb, int32_t pktpos)
{
	return pktpos * VSBPACKET_27MHZ_TICK;
}

void tool_8vsb_append_vsbframe(Pvsb_dataframe pvsb, Pmux_clock_tick pclk)
{
	mux_clock_append(pclk, VSBFRAME_27MHZ_TICK);
}

#endif

uint32_t tool_atsc_get_bitrate(Pmodulator_param pmod)
{
	if(pmod->type == modulator_atsc)return VSBATSC_BITRATE;
	return 0;
}

vatek_result tool_atsc_check_param(Pmodulator_param pmod)
{
	if (pmod->type == modulator_atsc)return vatek_success;
	return vatek_badparam;
}