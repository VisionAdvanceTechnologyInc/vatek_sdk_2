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

#ifndef _MUX_PACKET_
#define _MUX_PACKET_

#include <mux/mux_define.h>

/* ts_packet */

typedef struct _mux_packet {
	uint16_t pid;
	uint16_t packetlen;
	uint32_t ncontinue;
	uint8_t* packetraw;
}mux_packet;

typedef mux_packet* Pmux_packet;

typedef struct _null_packet{
	mux_packet base;
}null_packet;

typedef null_packet* Pnull_packet;

typedef struct _pcr_packet{
	mux_packet base;
	//mux_time_tick time;
	//uint32_t tickns;
}pcr_packet;

typedef pcr_packet* Ppcr_packet;

/* pes_stream */

/**
 * @ingroup mux_props
 * send pes frame buffer to usb mux api
 * 
 */
typedef struct _mux_pes_frame{
	int64_t pts;
	int64_t dts;
	uint8_t* ptrhdr;
	int32_t lenhdr;
	int32_t lenbuf;
	uint8_t* ptrbuf;
	void* private_data;
}mux_pes_frame;

typedef mux_pes_frame* Pmux_pes_frame;

typedef enum _pes_mode
{
	pes_null,
	pes_pts,
	pes_pts_dts
}pes_mode;

typedef struct _pes_section
{
	int32_t len;
	uint8_t* packet;
}pes_section, *Ppes_section;

typedef Pmux_pes_frame(*fppes_get)(void* param);
typedef vatek_result(*fppes_commit)(void* param, Pmux_pes_frame pframe);

typedef struct _pes_source
{
	void* param;
	fppes_get get;
	fppes_commit commit;
}pes_source, *Ppes_source;

typedef struct _pes_packet{
	pes_mode mode;
	Ppes_source source;
	Pmux_stream stream;
	uint32_t ncontinue;
	mux_time_tick tick;
	int32_t peslen;
	int32_t pespos;
	uint32_t latencytick;
	Pmux_pes_frame pframe;
	pes_section section[3];
}pes_packet;

typedef pes_packet* Ppes_packet;

#define PKTHDR_TEI					0x80	/* Transport Error _Indicator		*/
#define PKTHDR_PSTART				0x40	/* Payload Unit Start Indicator		*/
#define PKTHDR_HPRIORITY			0x20	/* Transport Priority				*/

#define PKTTAG_TSI_RECV				0x40	/* Transport Scrambling control - recv	*/
#define PKTTAG_TSI_ODD				0x80
#define PKTTAG_TSI_EVEN				0xC0

#define PKTTAG_PLAYLOAD_ONLY		0x10
#define PKTTAG_ADAP_ONLY			0x20
#define PKTTAG_BOTH					0x30

#ifdef __cplusplus
extern "C" {
#endif

	HAL_API void muxpacket_put_hdr(uint8_t* pktbuf, uint16_t pid, uint8_t hdr, uint8_t tag);

	HAL_API Pnull_packet muxpacket_null_create(Pth_mempool pmem);
	HAL_API uint8_t* muxpacket_null_get_next(Pnull_packet pnull);

	HAL_API Ppcr_packet muxpacket_pcr_create(Pth_mempool pmem, uint16_t pid);
	HAL_API uint8_t* muxpacket_pcr_get_next_time(Ppcr_packet ppcr, Pmux_time_tick ptime);
	HAL_API uint8_t* muxpacket_pcr_get_next_tick(Ppcr_packet ppcr, Pmux_clock_tick pclk);

	HAL_API Ppes_packet muxpacket_pes_create(Pth_mempool pmem, Pmux_stream pstream, Ppes_source psource,uint32_t latency);
	HAL_API vatek_result muxpacket_pes_check(Ppes_packet ppes);
	HAL_API vatek_result muxpacket_pes_get_next(Ppes_packet ppes, uint8_t* packet);

#ifdef __cplusplus
}
#endif

#endif
