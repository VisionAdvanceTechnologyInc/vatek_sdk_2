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

#include <core/tools/tool_helpful.h>

extern void bs_next(Pth_bitstream pbs);
extern void bs_write(Pth_bitstream pbs);

Pth_bitstream th_bs_create_pool(Pth_mempool pmempool, uint8_t* pbuf, int32_t len)
{
	Pth_bitstream pbs = (Pth_bitstream)th_mempool_malloc(pmempool, sizeof(th_bitstream));
	th_bs_config(pbs, pbuf, len);
	return pbs;
}

void th_bs_config(Pth_bitstream pbs, uint8_t* pbuf, int32_t len)
{
	memset(pbs, 0, sizeof(th_bitstream));
	pbs->pbuf = pbuf;
	pbs->size = len;
	pbs->pos = 0;
	pbs->bitrecv = 0;
	pbs->value = 0;
}

const uint8_t bit_mask[9] = { 0x00,0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0xFF, };

uint32_t th_bs_read(Pth_bitstream pbs, int32_t bits)
{
	uint32_t rval = 0;
	int32_t reach = 0;
	if (bits > 32)VWAR("read bad bits - %d", bits);

	while (bits)
	{
		bs_next(pbs);
		reach = pbs->bitrecv;
		if (bits < reach)reach = bits;
		rval <<= reach;
		rval |= (pbs->value >> (8 - reach));
		pbs->value <<= reach;
		pbs->bitrecv -= reach;
		bits -= reach;
	}
	return rval;
}

void th_bs_write(Pth_bitstream pbs, uint32_t val, int32_t bits)
{
	int32_t reach = 0;
	if (bits > 32)VWAR("write bad bits - %d", bits);

	while (bits)
	{
		bs_next(pbs);
		reach = bits;
		if (reach > pbs->bitrecv)reach = pbs->bitrecv;
		pbs->value <<= reach;
		pbs->value |= val >> (32 - reach);
		val <<= reach;
		pbs->bitrecv -= reach;
		bits -= reach;
		bs_write(pbs);
	}
}

inline void bs_next(Pth_bitstream pbs)
{
	if (!pbs->bitrecv)
	{
		pbs->value = pbs->pbuf[pbs->pos++];
		pbs->bitrecv = 8;
		if (pbs->pos > pbs->size)VWAR("bs read buffer overflow");
	}
}

inline void bs_write(Pth_bitstream pbs)
{
	if (!pbs->bitrecv)
	{
		if(pbs->pos)pbs->pbuf[pbs->pos - 1] = pbs->value;
	}
}