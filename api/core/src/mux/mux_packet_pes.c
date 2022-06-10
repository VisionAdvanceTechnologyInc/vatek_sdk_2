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

#define PES_EN_DATACHK						0

#define PES_START_SECTION_LEN_POS			8
#define PES_START_PESHEADER_LEN_POS			12
#define PES_START_PTSDTS_POS				13
#define PES_END_ADAPTATION_LEN_POS			4

#define PES_FLAG_DTS						0x00000001
#define PES_FLAG_PTS						0x00000002
#define PES_FLAG_PTS_DTS					(PES_FLAG_DTS | PES_FLAG_PTS)

extern vatek_result create_section_0(Pth_mempool pmem, Ppes_packet ppes);
extern vatek_result create_section_1(Pth_mempool pmem, Ppes_packet ppes);
extern vatek_result create_section_2(Pth_mempool pmem, Ppes_packet ppes);

extern uint8_t get_pes_pts_dts_flag(pes_mode mode);
extern uint8_t get_pes_header_len(pes_mode mode);
extern void update_pes_start(Ppes_packet ppes, int32_t len, uint64_t pts, uint64_t dts);

extern void get_pes_frame(Ppes_packet ppes);
extern void get_pes_buffer(Ppes_packet ppes, uint8_t* pbuf, int32_t len);
extern void commit_pes_frame(Ppes_packet ppes);

Ppes_packet muxpacket_pes_create(Pth_mempool pmem, Pmux_stream pstream, Ppes_source psource, uint32_t latency)
{
	vatek_result nres = vatek_success;
	pes_mode mode = pes_pts_dts;
	if (pstream->type == mux_stream_audio)mode = pes_pts;
	else if(pstream->type != mux_stream_video)nres = vatek_badparam;

	if (is_vatek_success(nres))
	{
		Ppes_packet newpes = (Ppes_packet)th_mempool_malloc(pmem, sizeof(pes_packet));
		nres = vatek_memfail;
		if (newpes)
		{
			newpes->mode = mode;
			newpes->stream = pstream;
			newpes->source = psource;
			newpes->latencytick = latency * 90;
			nres = create_section_0(pmem, newpes);
			if (is_vatek_success(nres))
				nres = create_section_1(pmem, newpes);
			if (is_vatek_success(nres))
				nres = create_section_2(pmem, newpes);
			if (is_vatek_success(nres))return newpes;
		}
	}

	VERR("create packet_pes fail : %d", nres);
	return NULL;
}

vatek_result muxpacket_pes_check(Ppes_packet ppes)
{
	if (!ppes->pframe)get_pes_frame(ppes);
	if (ppes->pframe)return (vatek_result)1;
	else return vatek_success;
}

vatek_result muxpacket_pes_get_next(Ppes_packet ppes,uint8_t* packet)
{
	vatek_result nres = vatek_badstatus;
	if (ppes->pframe)
	{
		#define section_buf(i)		&ppes->section[i].packet[0]
		if (ppes->peslen)
		{
			int32_t len = ppes->peslen - ppes->pespos;
			if (len <= 0)VERR("pes_stream underflow");
			nres = vatek_success;

			if (ppes->pespos == 0)
			{
				int32_t hlen = ppes->section[0].len;
				int32_t rlen = TS_PACKET_LEN - hlen;
				int32_t sectionlen;

				if (len >= rlen)
				{
					memcpy(&packet[0], section_buf(0), hlen);
					get_pes_buffer(ppes,&packet[hlen], rlen);
					sectionlen = packet[PES_START_PESHEADER_LEN_POS] + (ppes->peslen) + 3;
				}
				else
				{
					int32_t plen = rlen - len;
					hlen += plen;
					memcpy(&packet[0], section_buf(0), hlen);
					packet[PES_START_PESHEADER_LEN_POS] += plen;
					get_pes_buffer(ppes,&packet[hlen], len);
					sectionlen = 184 - 6;
				}

				if (ppes->stream->type != mux_stream_video)
				{
					packet[PES_START_SECTION_LEN_POS] = (uint8_t)(sectionlen >> 8);
					packet[PES_START_SECTION_LEN_POS + 1] = (uint8_t)sectionlen;
				}
			}
			else if (len >= (TS_PACKET_LEN - 4))
			{
				memcpy(&packet[0], section_buf(1), 4);
				get_pes_buffer(ppes,&packet[4], TS_PACKET_LEN - 4);
			}
			else
			{
				int32_t plen = (TS_PACKET_LEN - len);
				memcpy(&packet[0], section_buf(2), plen);
				get_pes_buffer(ppes,&packet[plen], len);
				packet[4] = plen - 5;
			}
			packet[3] = (packet[3] & 0xF0) | (ppes->ncontinue & 0xF);
			ppes->ncontinue++;
			nres = (vatek_result)(ppes->peslen - ppes->pespos);
			if (nres == 0)commit_pes_frame(ppes);
		}
	}

	if(!is_vatek_success(nres))VWAR("muxpacket_pes_get_next fail : %d",nres);
	return nres;
}

void get_pes_buffer(Ppes_packet ppes,uint8_t* pbuf, int32_t len)
{
	while (len)
	{
		int32_t elen = len;
		uint8_t* praw;
		if (ppes->pespos < ppes->pframe->lenhdr)
		{
			int32_t hlen = ppes->pframe->lenhdr - ppes->pespos;
			if (elen > hlen)elen = hlen;
			praw = &ppes->pframe->ptrhdr[ppes->pespos];
		}
		else
		{
			int32_t rlen = ppes->peslen - ppes->pespos;
			praw = &ppes->pframe->ptrbuf[ppes->pespos - ppes->pframe->lenhdr];
			if (elen > rlen)VERR("pes_stream overflow");
		}

		memcpy(pbuf, praw, elen);
		len -= elen;
		pbuf += elen;
		ppes->pespos += elen;
	}
}

void get_pes_frame(Ppes_packet ppes)
{
	#define TICKNS	(1000000000/90000)
	
	ppes->pframe = ppes->source->get(ppes->source->param);
	if (ppes->pframe)
	{
		ppes->peslen = ppes->pframe->lenhdr + ppes->pframe->lenbuf;
		ppes->pespos = 0;
		ppes->tick.ms = (uint32_t)(ppes->pframe->pts / 90);
		ppes->tick.ns = (uint32_t)((ppes->pframe->pts % 90) * TICKNS);

#if PES_EN_DATACHK
		static uint8_t tag = 33;
		if (ppes->pframe->lenhdr > 3)
		{
			ppes->pframe->ptrhdr[0] = 0xA0;
			ppes->pframe->ptrhdr[1] = ppes->pframe->lenhdr;
			ppes->pframe->ptrhdr[ppes->pframe->lenhdr - 1] = 0xAF;
			memset(&ppes->pframe->ptrhdr[2], tag++, ppes->pframe->lenhdr - 3);
			if (tag > 0x7F)tag = 33;
		}
		ppes->pframe->ptrbuf[0] = 0xB0;
		ppes->pframe->ptrbuf[1] = (uint8_t)(ppes->pframe->lenbuf >> 24);
		ppes->pframe->ptrbuf[2] = (uint8_t)(ppes->pframe->lenbuf >> 16);
		ppes->pframe->ptrbuf[3] = (uint8_t)(ppes->pframe->lenbuf >> 8);
		ppes->pframe->ptrbuf[4] = (uint8_t)(ppes->pframe->lenbuf);
		ppes->pframe->ptrbuf[ppes->pframe->lenbuf - 1] = 0xBF;

		memset(&ppes->pframe->ptrbuf[5], tag++, ppes->pframe->lenbuf - 6);
		if (tag > 0x7F)tag = 33;
#endif
		update_pes_start(ppes, ppes->peslen, ppes->pframe->pts + ppes->latencytick, ppes->pframe->dts + ppes->latencytick);
	}
}

void update_pes_start(Ppes_packet ppes,int32_t len, uint64_t pts, uint64_t dts)
{
	Ppes_section psection = &ppes->section[0];
	uint8_t* packet = psection->packet;
	uint8_t flag = get_pes_pts_dts_flag(ppes->mode);
	packet[PES_START_PTSDTS_POS] = (uint8_t)((flag << 4) | 0x01 | (((pts >> 30) & 0x7) << 1));
	packet[PES_START_PTSDTS_POS + 1] = (uint8_t)(pts >> 22);
	packet[PES_START_PTSDTS_POS + 2] = (uint8_t)(((pts >> 15) << 1) | 0x01);
	packet[PES_START_PTSDTS_POS + 3] = (uint8_t)(pts >> 7);
	packet[PES_START_PTSDTS_POS + 4] = (uint8_t)((pts << 1) | 0x01);

	if (flag & PES_FLAG_DTS)
	{
		packet[PES_START_PTSDTS_POS + 5] = (uint8_t)(0x11 | (((dts >> 30) & 0x7) << 1));
		packet[PES_START_PTSDTS_POS + 6] = (uint8_t)(dts >> 22);
		packet[PES_START_PTSDTS_POS + 7] = (uint8_t)(((dts >> 15) << 1) | 0x01);
		packet[PES_START_PTSDTS_POS + 8] = (uint8_t)(dts >> 7);
		packet[PES_START_PTSDTS_POS + 9] = (uint8_t)((dts << 1) | 0x01);
	}

	packet[PES_START_PESHEADER_LEN_POS] = get_pes_header_len(ppes->mode);

#if 0
	if (ppes->stream->type != mux_stream_video)
	{
		len &= 0xFFFF;
		len += 8; /* include pes_header */
		packet[PES_START_SECTION_LEN_POS] = (uint8_t)(len >> 8);
		packet[PES_START_SECTION_LEN_POS + 1] = (uint8_t)len;
	}
#endif
}

void commit_pes_frame(Ppes_packet ppes)
{
	vatek_result nres = ppes->source->commit(ppes->source->param,ppes->pframe);
	if (!is_vatek_success(nres))VERR("commit pes_frame fail - [%d:%04x]", nres, ppes->stream->stream_pid);
	ppes->pframe = NULL;
	ppes->pespos = 0;
	ppes->peslen = 0;
}

vatek_result create_section_0(Pth_mempool pmem, Ppes_packet ppes)
{
	Ppes_section psection = &ppes->section[0];
	vatek_result nres = vatek_memfail;
	psection->packet = th_mempool_malloc(pmem, TS_PACKET_LEN);
	if (psection->packet)
	{
		uint8_t* packet = psection->packet;
		int32_t pos = 0;
		nres = vatek_success;
		memset(packet, 0xFF, TS_PACKET_LEN);
		packet[pos++] = TS_PACKET_SYNC_TAG;
		packet[pos++] = 0x40 | ((ppes->stream->stream_pid >> 8) & 0x1F);
		packet[pos++] = ppes->stream->stream_pid & 0xFF;
		packet[pos++] = 0x10;
		packet[pos++] = 0x00;
		packet[pos++] = 0x00;
		packet[pos++] = 0x01;

		if (ppes->stream->type == mux_stream_video)
			packet[pos++] = STREAMID_VIDEO_STREAM_BASE;
		else if (ppes->stream->type == mux_stream_audio)
		{
			audio_codec acodec = ppes->stream->_stream_info.audio.acodec;
			if (acodec == encaudio_ac3)
				packet[pos++] = STREAMID_PRIVATE_STREAM_1;
			else packet[pos++] = STREAMID_AUDIO_STREAM_BASE;
		}			
		else nres = vatek_unsupport;

		if (is_vatek_success(nres))
		{
			packet[pos++] = 0x00; /* section len */
			packet[pos++] = 0x00;
			packet[pos++] = 0x85; //packet[pos++] = 0x80;  Original & Data alignment indicator
			//packet[pos++] = 0x80; //packet[pos++] = 0x80;  Original & Data alignment indicator
			packet[pos++] = get_pes_pts_dts_flag(ppes->mode) << 6; /* pts dts flag*/
			packet[pos++] = get_pes_header_len(ppes->mode);
			psection->len = pos + packet[pos - 1];
		}
	}
	return nres;
}

vatek_result create_section_1(Pth_mempool pmem, Ppes_packet ppes)
{
	Ppes_section psection = &ppes->section[1];
	vatek_result nres = vatek_memfail;
	psection->packet = th_mempool_malloc(pmem, TS_PACKET_LEN);
	if (psection->packet)
	{
		int32_t pos = 0;
		uint8_t* packet = psection->packet;
		memset(packet, 0xFF, TS_PACKET_LEN);
		packet[pos++] = TS_PACKET_SYNC_TAG;
		packet[pos++] = 0x00 | ((ppes->stream->stream_pid >> 8) & 0x1F);
		packet[pos++] = ppes->stream->stream_pid & 0xFF;
		packet[pos++] = 0x10;
		psection->len = pos;
		nres = vatek_success;
	}
	return nres;
}

vatek_result create_section_2(Pth_mempool pmem, Ppes_packet ppes)
{
	Ppes_section psection = &ppes->section[2];
	vatek_result nres = vatek_memfail;
	psection->packet = th_mempool_malloc(pmem, TS_PACKET_LEN);
	if (psection->packet)
	{
		int32_t pos = 0;
		uint8_t* packet = psection->packet;
		memset(packet, 0xFF, TS_PACKET_LEN);
		packet[pos++] = TS_PACKET_SYNC_TAG;
		packet[pos++] = 0x00 | ((ppes->stream->stream_pid >> 8) & 0x1F);
		packet[pos++] = ppes->stream->stream_pid & 0xFF;
		packet[pos++] = 0x30;
		packet[pos++] = 0x00;
		packet[pos++] = 0x00;
		psection->len = pos;
		nres = vatek_success;
	}
	return nres;
}

uint8_t get_pes_pts_dts_flag(pes_mode mode)
{
	if (mode == pes_pts)return PES_FLAG_PTS;
	else if (mode == pes_pts_dts)return PES_FLAG_PTS_DTS;
	else VERR("unknown pes mode : %d", mode);
	return 0;
}

uint8_t get_pes_header_len(pes_mode mode)
{
	if (mode == pes_pts)return 5;
	else if (mode == pes_pts_dts)return 10;
	return 0;
}