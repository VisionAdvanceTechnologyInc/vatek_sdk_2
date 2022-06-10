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

#include "./internal/mux_usbstream.h"

#define USTREAM_EN_PCR_ALIGN_SLICE		1
#define USBSTREAM_SLICE_PACKET_NUMS (CHIP_STREAM_SLICE_LEN/CHIP_TS_PACKET_LEN)

extern void usbmuxer_handler(Pcross_thread_param param);
extern vatek_result usbmuxer_set_tick(Pusbmux_param pparam, Pumuxer_tick ptick);

#if USTREAM_EN_PCR_ALIGN_SLICE
	extern void usbmuxer_append_slice(Phandle_muxer pmuxer, Pusbstream_slice pslice);
#else
	extern void usbmuxer_append_slice(Pumuxer_tick ptick);
	extern void usbstream_check_pcr(Phandle_muxer pmuxer, Pusbstream_slice pslice);
#endif

vatek_result usbstream_muxer_create(Pth_mempool pmem, Pusbmux_param param, int32_t nums, Phandle_muxer* pmuxer)
{
	Phandle_muxer newmuxer = NULL;
	hcross_mutex hlock = NULL;
	vatek_result nres = cross_os_create_mutex(&hlock);;

	if (is_vatek_success(nres))
	{
		newmuxer = (Phandle_muxer)th_mempool_malloc(pmem, sizeof(handle_muxer));
		nres = vatek_memfail;
		if (newmuxer)nres = vatek_success;
	}

	if (is_vatek_success(nres))
	{
		newmuxer->stream_nums = nums;
		newmuxer->streams = (Pumuxer_stream)th_mempool_malloc(pmem, sizeof(umuxer_stream) * nums);
		if (newmuxer->streams)
		{
			if (is_vatek_success(nres))
			{
				Pumuxer_tick ptick = &newmuxer->streamtick;
				nres = usbmuxer_set_tick(param, ptick);
				if (is_vatek_success(nres))
				{
					//uint32_t pcrns = ptick->pcr_packetnums * ptick->packet_ns;
					//newmuxer->pcrpacket = muxpacket_pcr_create(pmem, param->pcr_pid, pcrns);
					newmuxer->pcrpacket = muxpacket_pcr_create(pmem, param->pcr_pid);
					newmuxer->nullpacket = muxpacket_null_create(pmem);
					newmuxer->stream_pktnums = USBSTREAM_SLICE_PACKET_NUMS / newmuxer->stream_nums;
					newmuxer->latencyms = param->latency_ms;
					newmuxer->hlock = hlock;
				}
			}
		}
	}
	if (is_vatek_success(nres))*pmuxer = newmuxer;
	else
	{
		if (hlock)cross_os_free_mutex(hlock);
	}
	return nres;
}

vatek_result usbstream_muxer_close(Phandle_muxer pmuxer)
{
	vatek_result nres = vatek_badstatus;
	if (pmuxer->status == umux_status_idle)
	{
		cross_os_free_mutex(pmuxer->hlock);
	}
	return nres;
}

vatek_result usbstream_muxer_start(Phandle_muxer pmuxer, Pusbmux_source psource,hvatek_usbstream hustream)
{
	vatek_result nres = vatek_badstatus;
	if (!pmuxer->hthread)
	{
		nres = psource->start(psource->humsource);
		if (is_vatek_success(nres))
		{
			pmuxer->hustream = hustream;
			pmuxer->source = psource;
			pmuxer->status = umux_status_running;
			pmuxer->hthread = cross_os_create_thread(usbmuxer_handler, pmuxer);
			if (!pmuxer->hthread)
			{
				psource->stop(psource->humsource);
				pmuxer->status = umux_status_idle;
				VWAR("usbstream_muxer_start create thread fail");
			}
		}else VWAR("mux_source start fail : %d",nres);
	}
	return nres;
}

void usbstream_muxer_stop(Phandle_muxer pmuxer)
{
	if (pmuxer->status == umux_status_running)
	{
		pmuxer->status = umux_status_stoping;
		while (pmuxer->status == umux_status_stoping)
			cross_os_sleep(10);
	}

	if (pmuxer->hthread)
	{
		cross_os_free_thread(pmuxer->hthread);
		pmuxer->hthread = NULL;
	}
	pmuxer->status = umux_status_idle;
}

void usbstream_muxer_query_clean(Phandle_muxer pmuxer)
{
	int32_t i = 0;
	Pusbmux_source psource = pmuxer->source;
	cross_os_lock_mutex(pmuxer->hlock);

	for (i = 0; i < pmuxer->stream_nums;i++)
	{
		Pumuxer_stream pums = &pmuxer->streams[i];
		while (pums->nptrr != pums->nptrw)
		{
			Pmux_pes_frame pesframe = pums->frames[pums->nptrr];
			psource->freeframe(psource->humsource, pums->umstream, pesframe);
			pums->nptrr = (pums->nptrr + 1) & HUSBPES_FRAME_MASK;
		}
	}

	cross_os_release_mutex(pmuxer->hlock);
}

int32_t usbstream_packet_commit(Phandle_muxer pmuxer, Pusbstream_slice pslice)
{
	pslice->packet_pos++;
	pslice->ptrbuf += TS_PACKET_LEN;
#if !USTREAM_EN_PCR_ALIGN_SLICE
	pmuxer->streamtick.pcr_pos++;
	if (pslice->packet_len > pslice->packet_pos)
	{
		usbstream_check_pcr(pmuxer, pslice);
	}
#endif
	return (pslice->packet_pos < pslice->packet_len);
}

vatek_result usbstream_muxer_stream(Phandle_muxer pmuxer,Pumuxer_stream pumstream, Pusbstream_slice pslice)
{
	vatek_result nres = vatek_success;
	int32_t npackets = 0;
	while (npackets < pmuxer->stream_pktnums)
	{
		nres = muxpacket_pes_get_next(pumstream->pespacket, pslice->ptrbuf);
		if (is_vatek_success(nres))
		{
			int32_t recv = (vatek_result)nres;
			npackets++;

			if (!usbstream_packet_commit(pmuxer, pslice))break;
			if (nres != vatek_success)break;
			if (!recv)break;
		}else break;
	}

	if (is_vatek_success(nres))nres = (vatek_result)npackets;
	else VWAR("usbstream_muxer_stream fail : %d",nres);
	return nres;
}

vatek_result usbstream_muxer_mux(Pusbstream_slice pslice, Phandle_muxer pmuxer)
{
	vatek_result nres = vatek_success;
	int32_t i = 0;
	int32_t packetnums = 0;
	for (i = 0; i < pmuxer->stream_nums; i++)
	{
		Pumuxer_stream pums = &pmuxer->streams[i];
		nres = muxpacket_pes_check(pums->pespacket);
		if (nres > vatek_success)
		{
			if (pums->pespacket->tick.ms <= pmuxer->streamtick.tick.ms)
			{
				nres = usbstream_muxer_stream(pmuxer,pums,pslice);
				if (is_vatek_success(nres))packetnums += nres;
			}
		}
		if (!is_vatek_success(nres))break;
		if (pslice->packet_len == pslice->packet_pos)break;
	}

	if (is_vatek_success(nres))nres = (vatek_result)packetnums;
	else VWAR("usbstream_muxer_mux fail : %d",nres);
	return nres;
}

void usbmuxer_handler(Pcross_thread_param param)
{
	Phandle_muxer pmuxer = (Phandle_muxer)param->userparam;
	Pusbmux_source psource = pmuxer->source;
	vatek_result nres = vatek_success;

	while (pmuxer->status == umux_status_running)
	{
		uint32_t muxpackets = 0;
		Pusbstream_slice pslice = NULL;
		nres = vatek_ustream_async_get_buffer(pmuxer->hustream, &pslice);
		if (nres > vatek_success)
		{
			Pumuxer_tick ptick = &pmuxer->streamtick;
#if USTREAM_EN_PCR_ALIGN_SLICE
			usbmuxer_append_slice(pmuxer,pslice);
			psource->prepare(psource->humsource, ptick->tick.ms);
#else
			usbmuxer_append_slice(ptick);
			psource->prepare(psource->humsource, ptick->tick.ms);
			usbstream_check_pcr(pmuxer, pslice);
#endif

			while (pslice->packet_pos < pslice->packet_len)
			{
				nres = usbstream_muxer_mux(pslice, pmuxer);
				if (is_vatek_success(nres))
				{
					muxpackets += nres;
					if (nres == vatek_success)break;	/* no more packets*/
				}
				else VWAR("usbstream_muxer_mux fail : %d",nres);
				if (!is_vatek_success(nres))break;
				if (pslice->packet_len == pslice->packet_pos)break;
			}

			if (is_vatek_success(nres))
			{
				while (pslice->packet_pos < pslice->packet_len)
				{
					memcpy(pslice->ptrbuf, muxpacket_null_get_next(pmuxer->nullpacket), TS_PACKET_LEN);
					usbstream_packet_commit(pmuxer, pslice);
				}
				nres = vatek_ustream_async_commit_buffer(pmuxer->hustream, pslice);
			}
		}
		else if (nres == vatek_success)cross_os_sleep(1);
		if (!is_vatek_success(nres))break;
	}
	param->result = nres;
	usbstream_muxer_query_clean(pmuxer);
	psource->stop(psource->humsource);
	pmuxer->status = umux_status_stoped;
	
}

extern Pmux_pes_frame pesosurce_get(void* param);
extern vatek_result pessource_commit(void* param, Pmux_pes_frame pframe);

vatek_result usbstream_muxer_set_stream(Pth_mempool pmem, Phandle_muxer pmuxer, int32_t idx, hmux_stream hstream, Pusbmux_stream pstream)
{
	vatek_result nres = vatek_badparam;
	if (idx < pmuxer->stream_nums)
	{
		Pumuxer_stream pumstream = &pmuxer->streams[idx];
		pumstream->umstream = pstream;
		pumstream->pessource.param = pumstream;
		pumstream->pessource.get = pesosurce_get;
		pumstream->pessource.commit = pessource_commit;
		pumstream->muxer = pmuxer;
		pumstream->pespacket = muxpacket_pes_create(pmem, muxstream_get(hstream), &pumstream->pessource,pmuxer->latencyms);
		if(pumstream->pespacket)nres = vatek_success;
		else nres = vatek_memfail;
	}
	return nres;
}

vatek_result usbstream_muxer_put_frame(Phandle_muxer pmuxer, int32_t idx, Pmux_pes_frame pframe)
{
	Pumuxer_stream pumstream = &pmuxer->streams[idx];
	vatek_result nres = vatek_badstatus;
	if (pmuxer->status == umux_status_running)
	{
		int32_t nextidx = pumstream->nptrw + 1;
		nres = vatek_memfail;
		if (nextidx == pumstream->nptrr)VWAR("umuxer_stream pes_frame query overflow - [%d]",(uint32_t)(pframe->pts/90));
		else
		{
			cross_os_lock_mutex(pmuxer->hlock);
			pumstream->frames[pumstream->nptrw] = pframe;
			pumstream->nptrw = (pumstream->nptrw + 1) & HUSBPES_FRAME_MASK;
			pumstream->queryms = (uint32_t)(pframe->pts / 90);
			cross_os_release_mutex(pmuxer->hlock);
			nres = vatek_success;
		}
	}
	return nres;
}

Pmux_pes_frame pesosurce_get(void* param)
{
	Pumuxer_stream pumstream = (Pumuxer_stream)param;
	Pmux_pes_frame ptrframe = NULL;
	cross_os_lock_mutex(pumstream->muxer->hlock);
	if (pumstream->nptrw != pumstream->nptrr)
		ptrframe = pumstream->frames[pumstream->nptrr];
	cross_os_release_mutex(pumstream->muxer->hlock);
	return ptrframe;
}

vatek_result pessource_commit(void* param, Pmux_pes_frame pframe)
{
	Pumuxer_stream pumstream = (Pumuxer_stream)param;
	Pusbmux_source psource = pumstream->muxer->source;
	cross_os_lock_mutex(pumstream->muxer->hlock);
	pumstream->nptrr = (pumstream->nptrr + 1) & HUSBPES_FRAME_MASK;
	cross_os_release_mutex(pumstream->muxer->hlock);
	return psource->freeframe(psource->humsource, pumstream->umstream, pframe);
}

#define BITRATE_MB		1000000
#define TIMETICK_NS		1000000000
#define TIMETICK_US		1000000

vatek_result usbmuxer_set_tick(Pusbmux_param pparam, Pumuxer_tick ptick)
{
	uint32_t bitrate = pparam->bitrate;
	vatek_result nres = vatek_badparam;
	if (bitrate == 0)
	{
		bitrate = modulator_param_get_bitrate(&pparam->modulator);
		bitrate = ((bitrate / BITRATE_MB) - 1) * BITRATE_MB;
	}

	if (bitrate > BITRATE_MB)
	{
		uint32_t packetnums = (bitrate / (TS_PACKET_LEN * 8));
		uint32_t packetns = TIMETICK_NS / packetnums;
		ptick->pcr_packetnums = (pparam->interval_ms * TIMETICK_US)/ packetns;
#if USTREAM_EN_PCR_ALIGN_SLICE
		ptick->pcr_packetnums = (ptick->pcr_packetnums / USBSTREAM_SLICE_PACKET_NUMS) * USBSTREAM_SLICE_PACKET_NUMS;
#endif
		ptick->packet_ns = packetns;
		ptick->slice_ns = USBSTREAM_SLICE_PACKET_NUMS * packetns + packetns; /* append a little mux speed */
		nres = vatek_success;
	}
	else VWAR("bad bitrate : %d", nres);
	return nres;
}

#if USTREAM_EN_PCR_ALIGN_SLICE

void usbmuxer_append_slice(Phandle_muxer pmuxer, Pusbstream_slice pslice)
{
	Pumuxer_tick ptick = &pmuxer->streamtick;
	ptick->pcr_pos += USBSTREAM_SLICE_PACKET_NUMS;

	if (ptick->pcr_pos >= ptick->pcr_packetnums)
	{
		Ppcr_packet pcr = pmuxer->pcrpacket;
		memcpy(pslice->ptrbuf, muxpacket_pcr_get_next_time(pcr,&ptick->tick), TS_PACKET_LEN);
		pslice->packet_pos++;
		pslice->ptrbuf += TS_PACKET_LEN;
		ptick->pcr_pos = 0;
	}
	ptick->tick.ns += ptick->slice_ns;
	ptick->tick.ms += ptick->tick.ns / TIMETICK_US;
	ptick->tick.ns %= TIMETICK_US;
}
#else

void usbstream_check_pcr(Phandle_muxer pmuxer, Pusbstream_slice pslice)
{
	Pumuxer_tick ptick = &pmuxer->streamtick;
	if (ptick->pcr_pos >= ptick->pcr_packetnums)
	{
		memcpy(pslice->ptrbuf, muxpacket_pcr_get_next(pmuxer->pcrpacket), TS_PACKET_LEN);
		pslice->packet_pos++;
		pslice->ptrbuf += TS_PACKET_LEN;
		ptick->pcr_pos = 0;
	}
}


void usbmuxer_append_slice(Pumuxer_tick ptick)
{
	ptick->tick_ns += ptick->slice_ns;
	ptick->tick_ms += ptick->tick_ns / TIMETICK_US;
	ptick->tick_ns %= TIMETICK_US;
}
#endif

const char* usbmux_get_eventname()
{
	static int32_t idx = 0;
	static char sztag[32];
	sprintf(&sztag[0], "umux_%d", idx++);
	return &sztag[0];
}
