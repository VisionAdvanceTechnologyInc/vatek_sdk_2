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

#include <core/tools/tool_j83abc.h>

#define J83B_Q64_BITRATE		26970355
#define J83B_Q256_BITRATE		38810705

#define J83_FRAME_PACKETNUMS	60

extern uint32_t j83ac_get_bitrate(uint32_t sb, int32_t bitsb);

vatek_result tool_j83_j83frame_reset(Pmodulator_param pmod, Pofdm_frame pframe)
{
	vatek_result nres = tool_j83_check_param(pmod);
	if (is_vatek_success(nres))
	{
		uint32_t bitrate = tool_j83_get_bitrate(pmod);
		pframe->packetnums = J83_FRAME_PACKETNUMS;
		pframe->frametick = ((uint32_t)(((uint64_t)27000000 * (J83_FRAME_PACKETNUMS * 188 * 8)) / bitrate)) + 1;
		pframe->framerecv = 0;
	}
	return nres;
}

uint32_t tool_j83_get_bitrate(Pmodulator_param pmod)
{
	if (pmod->type == modulator_j83a)
	{
		Pj83a_param pj83a = &pmod->mod.j83a;
		if (is_j83a_valid_sb(pmod->bandwidth_symbolrate))
		{
			if (pj83a->constellation >= j83a_qam16 && pj83a->constellation <= j83a_qam256)
				return j83ac_get_bitrate(pmod->bandwidth_symbolrate, pj83a->constellation + 4);
		}
	}
	else if (pmod->type == modulator_j83b)
	{
		Pj83b_param pj83b = &pmod->mod.j83b;
		if (pj83b->constellation == j83b_qam64)return J83B_Q64_BITRATE;
		else if (pj83b->constellation == j83b_qam256)return J83B_Q256_BITRATE;
	}
	else if (pmod->type == modulator_j83c)
	{
		Pj83c_param pj83c = &pmod->mod.j83c;
		if (pj83c->constellation == j83c_qam64)return j83ac_get_bitrate(J83C_SYMBOL_RATE, 6);
		else if (pj83c->constellation == j83c_qam256)return j83ac_get_bitrate(J83C_SYMBOL_RATE, 8);
	}
	return 0;
}

vatek_result tool_j83_check_param(Pmodulator_param pmod)
{
	uint32_t bitrate = tool_j83_get_bitrate(pmod);
	if (bitrate)return vatek_success;
	return vatek_badparam;
}

uint32_t j83ac_get_bitrate(uint32_t sb, int32_t bitsb)
{
	return (((uint64_t)(sb * 1000) * bitsb) * 188) / 204;
}
