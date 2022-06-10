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

#include <mux/mux_define.h>



#if 0

uint32_t mux_clock_get_eclipse(Pmux_clock_tick pclk0, Pmux_clock_tick pclk1)
{
	if(pclk1->_90KHz < pclk0->_90KHz)
	{
		uint32_t tick = (((uint64_t)0x1FFFFFFFF) - pclk0->_90KHz) + pclk1->_90KHz;
		tick *= 300;
		tick += pclk1->_27MHz + (300 - pclk0->_27MHz);
		return (uint32_t)tick;
	}else if(pclk0->_90KHz == pclk1->_90KHz)
	{
		int32_t tick = pclk1->_27MHz - pclk0->_27MHz;
		if(tick < 0)return 0;
		return tick;
	}else
	{
		uint32_t tick = ((pclk1->_90KHz - pclk0->_90KHz) * 300);
		tick +=  (int32_t)(pclk1->_27MHz - pclk0->_27MHz);
		return tick;
	}
}

#endif
