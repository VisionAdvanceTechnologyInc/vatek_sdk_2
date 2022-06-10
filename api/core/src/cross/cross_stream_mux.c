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

#include "./internal/stream_handle.h"
#include <core/tools/tool_output.h>
#include <mux/mux_packet.h>
#include <core/tools/tool_crc32.h>

#define CSTREAM_MUX_MEMPOOL_SIZE	8192
#define CSTREAM_MUX_PCRINTERVAL		40

#define CSTREAM_MUX_EN_OUTPUT		0
#define CSTREAM_MUX_OUTPUT_NUMS		256

static uint8_t fake_suffing_packet[2][188] =
{
	{
		0x47 ,0x40 ,0x00 ,0x10 ,0x00 ,0x00 ,0x80 ,0x0D ,0x01 ,0x11 ,0xDF ,0x00 ,0x00 ,0x01 ,0x03 ,0xE3,
		0x00 ,0x19 ,0x3F ,0xB7 ,0xCD ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF,
		0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF,
		0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF,
		0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF,
		0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF,
		0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF,
		0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF,
		0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF,
		0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF,
		0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF,
		0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF,
	},
	{
		0x47 ,0x43 ,0x00 ,0x10 ,0x00 ,0x02 ,0xB0 ,0x5A ,0x01 ,0x03 ,0xC3 ,0x00 ,0x00 ,0xE1 ,0x01 ,0xF0,
		0x00 ,0x02 ,0xE3 ,0x01 ,0xF0 ,0x05 ,0x02 ,0x03 ,0x9A ,0x48 ,0x5F ,0x03 ,0xE3 ,0x11 ,0xF0 ,0x09,
		0x0A ,0x04 ,0x63 ,0x7A ,0x65 ,0x00 ,0x52 ,0x01 ,0x02 ,0x03 ,0xE3 ,0x13 ,0xF0 ,0x11 ,0x0A ,0x04,
		0x63 ,0x7A ,0x65 ,0x03 ,0x52 ,0x01 ,0x07 ,0x7F ,0x06 ,0x06 ,0x05 ,0x63 ,0x7A ,0x65 ,0x03 ,0x06,
		0xE3 ,0x21 ,0xF0 ,0x1A ,0x45 ,0x11 ,0x01 ,0x0F ,0xE7 ,0xE8 ,0xE9 ,0xEA ,0xEC ,0xED ,0xEE ,0xEF,
		0xC7 ,0xC8 ,0xC9 ,0xCA ,0xCC ,0xCD ,0xCE ,0x56 ,0x05 ,0x63 ,0x7A ,0x65 ,0x09 ,0x00 ,0x58 ,0x5E,
		0xC6 ,0xF0 ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF, 0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF,
		0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF,
		0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF,
		0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF,
		0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF,
		0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF,
	},
};

typedef struct _cstream_mux
{
	cstream_handler handle;
	uint32_t bitrate;
	uint32_t pcrpkts;
	int32_t ptrpkts;
	th_mempool mempool;
	Pnull_packet pktnull;
	Ppcr_packet pktpcr;
	houtput_clock houtclk;
	uint8_t slicebuf[CHIP_STREAM_SLICE_LEN];
#if CSTREAM_MUX_EN_OUTPUT
	int32_t outcount;
	FILE* outfile;
#endif
}cstream_mux,*Pcstream_mux;

extern vatek_result cstream_mux_start(hcstream hstream);
extern vatek_result cstream_mux_get_slice(hcstream hstream, uint8_t** pslice);
extern uint32_t cstream_mux_get_bitrate(hcstream hstream);
extern void cstream_mux_stop(hcstream hstream);
extern void cstream_mux_close(hcstream hstream);

vatek_result cross_stream_open_mux(Pmodulator_param pmod, hcross_stream* hcstream)
{
	vatek_result nres = vatek_memfail;
	Pcstream_mux pmux = (Pcstream_mux)malloc(sizeof(cstream_mux) + CSTREAM_MUX_MEMPOOL_SIZE);
	if (pmux)
	{
		uint32_t crcpmt = tool_crc32(&fake_suffing_packet[1][5], 0x5A - 1);
		memset(pmux, 0, sizeof(cstream_mux) + CSTREAM_MUX_MEMPOOL_SIZE);
		th_mempool_config_raw(&pmux->mempool, &((uint8_t*)pmux)[sizeof(cstream_mux)], CSTREAM_MUX_MEMPOOL_SIZE);

		fake_suffing_packet[1][94] = (uint8_t)(crcpmt >> 24);
		fake_suffing_packet[1][95] = (uint8_t)(crcpmt >> 16);
		fake_suffing_packet[1][96] = (uint8_t)(crcpmt >> 8);
		fake_suffing_packet[1][97] = (uint8_t)(crcpmt);
		

		pmux->pktnull = muxpacket_null_create(&pmux->mempool);
		pmux->pktpcr = muxpacket_pcr_create(&pmux->mempool, 0x101);
		nres = tool_output_clock_create_mempool(pmod,&pmux->mempool, &pmux->houtclk);
		if (is_vatek_success(nres))
		{
			mux_clock_tick clkstart;
			clkstart._90KHz = ((uint64_t)0x1FFFFFFFF) - (10000 * 90);
			clkstart._27MHz = 0;
			tool_output_clock_set_tick(pmux->houtclk, &clkstart);
			pmux->pcrpkts = 0;
			pmux->ptrpkts = 0;
			pmux->bitrate = modulator_param_get_bitrate(pmod);
			if (pmux->bitrate == 0)nres = vatek_badparam;
			else nres = vatek_success;
		}

		if(is_vatek_success(nres))
		{
			pmux->handle.hstream = pmux;
			pmux->handle.get_bitrate = cstream_mux_get_bitrate;
			pmux->handle.get_slice = cstream_mux_get_slice;
			pmux->handle.close = cstream_mux_close;
			pmux->handle.start = cstream_mux_start;
			pmux->handle.stop = cstream_mux_stop;
#if CSTREAM_MUX_EN_OUTPUT
			pmux->outfile = fopen("test.ts", "wb+");
			pmux->outcount = 0;
#endif
			*hcstream = pmux;
		}
	}
	return nres;
}

vatek_result cstream_mux_start(hcstream hstream)
{
	Pcstream_mux pmux = (Pcstream_mux)hstream;
	int32_t pcrms = 0;
	vatek_result nres = tool_output_clock_get_pcrpacket(pmux->houtclk,&pcrms);
	if (is_vatek_success(nres))
	{
		pmux->pcrpkts = nres;
		pmux->ptrpkts = 0;
		nres = vatek_success;
	}
	return nres;
}

vatek_result cstream_mux_get_slice(hcstream hstream, uint8_t** pslice)
{
	Pcstream_mux pmux = (Pcstream_mux)hstream;
	int32_t i = 0;
	uint8_t* ptrbuf = &pmux->slicebuf[0];
	*pslice = &pmux->slicebuf[0];

	for (i = 0; i < CHIP_STREAM_SLICE_PACKET_NUMS; i++)
	{
		if (pmux->ptrpkts == 0)
		{
			Pmux_clock_tick tclk = tool_output_clock_get_tick(pmux->houtclk);
			memcpy(ptrbuf, muxpacket_pcr_get_next_tick(pmux->pktpcr, tclk), TS_PACKET_LEN);
		}
		else
		{
			uint8_t* ptr = &fake_suffing_packet[pmux->ptrpkts % 2][0];
			ptr[3] = (ptr[3] & 0xF0) | ((ptr[3] & 0x0F) + 1);
			memcpy(ptrbuf, &ptr[0], TS_PACKET_LEN);
		}
		tool_output_clock_append(pmux->houtclk);
		ptrbuf += TS_PACKET_LEN;
		pmux->ptrpkts++;
		if (pmux->pcrpkts == pmux->ptrpkts)
			pmux->ptrpkts = 0;
	}

#if CSTREAM_MUX_EN_OUTPUT
	if (pmux->outfile)
	{
		fwrite(&pmux->slicebuf[0], CHIP_STREAM_SLICE_LEN, 1, pmux->outfile);
		pmux->outcount++;
		if (pmux->outcount == CSTREAM_MUX_OUTPUT_NUMS)
		{
			fclose(pmux->outfile);
			pmux->outfile = NULL;
		}
	}
#endif
	return (vatek_result)1;
}

uint32_t cstream_mux_get_bitrate(hcstream hstream)
{
	Pcstream_mux pmux = (Pcstream_mux)hstream;
	return pmux->bitrate;
}

void cstream_mux_stop(hcstream hstream)
{
	Pcstream_mux pmux = (Pcstream_mux)hstream;
	tool_output_clock_reset(pmux->houtclk);
	pmux->ptrpkts = 0;
}

void cstream_mux_close(hcstream hstream)
{
	Pcstream_mux pmux = (Pcstream_mux)hstream;
#if CSTREAM_MUX_EN_OUTPUT
	if (pmux->outfile)fclose(pmux->outfile);
#endif
	tool_output_clock_free(pmux->houtclk);
	free(pmux);
}
