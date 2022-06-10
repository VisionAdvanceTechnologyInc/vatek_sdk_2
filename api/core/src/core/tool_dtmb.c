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

#include <core/tools/tool_dtmb.h>

#define DTMB_6MHZ	(uint32_t)(5.67 * 1000000)
#define DTMB_8MHZ	(uint32_t)(7.56 * 1000000)

#define DTMB_FRAME_CRBASE		3780
#define DTMB_FRAME_CRDATA		3744
#define DTMB_CODE_BASE			7488

const uint16_t dtmb_coderate[] = { 3008,4512,6016, };
const uint16_t dtmb_framehdr[] = { 420,945,595, };
const uint16_t dtmb_framelen[] = { 225,216,200, };
const uint8_t dtmb_fftbits[] = { 2,4,6,2,5, };

#define _crframe(p)		((DTMB_FRAME_CRBASE + dtmb_framehdr[p->framesync]) * dtmb_framelen[p->framesync])
#define _crdata(p)		(((DTMB_FRAME_CRDATA * dtmb_coderate[p->coderate]) / DTMB_CODE_BASE) * dtmb_framelen[p->framesync])

vatek_result tool_dtmb_superframe_reset(Pmodulator_param pmod, Pofdm_frame pframe)
{
	vatek_result nres = tool_dtmb_check_param(pmod);
	if (is_vatek_success(nres))
	{
		Pdtmb_param pdtmb = &pmod->mod.dtmb;
		uint32_t symbolrate = DTMB_6MHZ;
		uint32_t bitrate = _crdata(pdtmb) * dtmb_fftbits[pdtmb->constellation];
		if (pmod->bandwidth_symbolrate == 8)
			symbolrate = DTMB_8MHZ;
		pframe->frametick = (uint32_t)(((uint64_t)_crframe(pdtmb) * 27000000) / symbolrate);
		pframe->packetnums = bitrate / (TS_PACKET_LEN * 8);
		pframe->framerecv = bitrate % (TS_PACKET_LEN * 8);
	}
	return nres;
}

uint32_t tool_dtmb_get_bitrate(Pmodulator_param pmod)
{
	vatek_result nres = tool_dtmb_check_param(pmod);
	if (is_vatek_success(nres))
	{
		Pdtmb_param pdtmb = &pmod->mod.dtmb;
		uint32_t sb = DTMB_6MHZ;
		if (pmod->bandwidth_symbolrate == 8)sb = DTMB_8MHZ;
		int32_t pktlen = (_crdata(pdtmb) * dtmb_fftbits[pdtmb->constellation]);
		return (uint32_t)(((uint64_t)pktlen  * sb) / _crframe(pdtmb));
	}
	return nres;
}

vatek_result tool_dtmb_check_param(Pmodulator_param pmod)
{
	vatek_result nres = vatek_badparam;
	if (pmod->type == modulator_dtmb)
	{
		Pdtmb_param pdtmb = &pmod->mod.dtmb;
		if (is_dtmb_vaild_bw(pmod->bandwidth_symbolrate) &&
			is_constellation_dtmb(pdtmb->constellation) &&
			is_dtmb_carrier_mode(pdtmb->carriermode) &&
			is_dtmb_code_rate(pdtmb->coderate) &&
			is_dtmb_frame_sync(pdtmb->framesync) &&
			is_dtmb_ti_mode(pdtmb->timeinterleaved))nres = vatek_success;

		if (is_vatek_success(nres))
		{
			if (pdtmb->constellation == dtmb_qam4_nr || pdtmb->constellation == dtmb_qam32)
			{
				if (pdtmb->coderate != dtmb_code_rate_0_8)nres = vatek_badparam;
			}
		}
	}
	return nres;
}
