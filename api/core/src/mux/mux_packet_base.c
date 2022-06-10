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

#include <mux/mux_packet.h>
#include "internal/mux_core.h"

extern Pmux_packet packet_malloc(Pth_mempool pmem,int32_t sle,int32_t packetnum);

Pnull_packet muxpacket_null_create(Pth_mempool pmem)
{
	Pnull_packet pnull = (Pnull_packet)packet_malloc(pmem, sizeof(null_packet), 1);
	if (pnull)
	{
		muxpacket_put_hdr(pnull->base.packetraw, TS_PID_NULL, 0, PKTTAG_PLAYLOAD_ONLY);
		return pnull;
	}
	return NULL;
}

uint8_t* muxpacket_null_get_next(Pnull_packet pnull)
{
	return pnull->base.packetraw;
}

#if 0

Ppcr_packet muxpacket_pcr_create(Pth_mempool pmem, uint16_t pid, uint32_t intervalns)
{
	Ppcr_packet ppcr = (Ppcr_packet)packet_malloc(pmem, sizeof(pcr_packet), 1);
	if (ppcr)
	{
		muxpacket_put_hdr(ppcr->base.packetraw, pid, 0x40, PKTTAG_ADAP_ONLY);
		ppcr->base.pid = pid;
		//ppcr->tickns = intervalns;
		ppcr->base.packetraw[5] = 0x10;	/* adaptation _pcr en*/
		return ppcr;
	}
	return NULL;
}


uint8_t* muxpacket_pcr_get_next(Ppcr_packet ppcr)
{
#if 0
	mux_clock_tick mtick;
	uint32_t hi90k;
	uint8_t* m_packet = ppcr->base.packetraw;

	mtick._90KHz = (uint64_t)ppcr->time.ms * 90;
	mtick._27MHz = (uint32_t)((27 * ppcr->time.ns) / 1000);
	mtick._90KHz += mtick._27MHz / 300;
	mtick._27MHz = mtick._27MHz % 300;

	hi90k = (uint32_t)(mtick._90KHz >> 1);


	m_packet[6] = hi90k >> 24;
	m_packet[7] = hi90k >> 16;
	m_packet[8] = hi90k >> 8;
	m_packet[9] = hi90k;

	hi90k = mtick._90KHz & 0x01;
	m_packet[10] = 0x7E | (hi90k << 7) | ((mtick._27MHz >> 8) & 1);
	m_packet[11] = mtick._27MHz;

	ppcr->time.ns += ppcr->tickns;
	ppcr->time.ms += ppcr->time.ns / 1000000;
	ppcr->time.ns = ppcr->time.ns % 1000000;
#endif

	int64_t hi90k = ppcr->time.ms * 90;
	int32_t low27k = ppcr->time.ns / 37;
	uint8_t* ptr = &ppcr->base.packetraw[6];
	hi90k += low27k / 300;
	low27k = low27k % 300;
	*ptr++ = (uint8_t)(hi90k >> 25);
	*ptr++ = (uint8_t)(hi90k >> 17);
	*ptr++ = (uint8_t)(hi90k >> 9);
	*ptr++ = (uint8_t)(hi90k >> 1);
	*ptr++ = (uint8_t)(hi90k << 7 | low27k >> 8 | 0x7e);
	*ptr++ = (uint8_t)low27k;
	return &ppcr->base.packetraw[0];
}


#endif

Ppcr_packet muxpacket_pcr_create(Pth_mempool pmem, uint16_t pid)
{
	Ppcr_packet ppcr = (Ppcr_packet)packet_malloc(pmem, sizeof(pcr_packet), 1);
	if (ppcr)
	{
		muxpacket_put_hdr(ppcr->base.packetraw, pid, 0x40, PKTTAG_ADAP_ONLY);
		ppcr->base.pid = pid;
		ppcr->base.packetraw[5] = 0x10;	/* adaptation _pcr en*/
		return ppcr;
	}
	return NULL;
}

uint8_t* muxpacket_pcr_get_next_time(Ppcr_packet ppcr, Pmux_time_tick ptime)
{
	mux_clock_tick clk;
	clk._90KHz = ptime->ms * 90;
	clk._27MHz = ptime->ns / 37;
	clk._90KHz += clk._27MHz / 300;
	clk._27MHz = clk._27MHz % 300;
	return muxpacket_pcr_get_next_tick(ppcr, &clk);
}

uint8_t* muxpacket_pcr_get_next_tick(Ppcr_packet ppcr, Pmux_clock_tick pclk)
{
	uint8_t* ptr = &ppcr->base.packetraw[6];
	*ptr++ = (uint8_t)(pclk->_90KHz >> 25);
	*ptr++ = (uint8_t)(pclk->_90KHz >> 17);
	*ptr++ = (uint8_t)(pclk->_90KHz >> 9);
	*ptr++ = (uint8_t)(pclk->_90KHz >> 1);
	*ptr++ = (uint8_t)((pclk->_90KHz << 7) | (pclk->_27MHz >> 8) | 0x7e);
	*ptr++ = (uint8_t)pclk->_27MHz;
	return &ppcr->base.packetraw[0];
}

Pmux_packet packet_malloc(Pth_mempool pmem, int32_t sle, int32_t packetnum)
{
	int32_t len = sle + (TS_PACKET_LEN * packetnum);
	uint8_t* praw = (uint8_t*)th_mempool_malloc(pmem,len);
	if (praw)
	{
		Pmux_packet pkt = (Pmux_packet)&praw[0];
		pkt->packetlen = packetnum;
		pkt->packetraw = &praw[sle];
		memset(pkt->packetraw, 0xFF, packetnum * TS_PACKET_LEN);
		return pkt;
	}
	return NULL;
}

void muxpacket_put_hdr(uint8_t* pktbuf, uint16_t pid, uint8_t hdr, uint8_t tag)
{
	pktbuf[0] = TS_PACKET_SYNC_TAG;
	pktbuf[1] = (uint8_t)(hdr | ((pid >> 8) & 0x1F));
	pktbuf[2] = (uint8_t)(pid & 0xFF);
	pktbuf[3] = tag;
}