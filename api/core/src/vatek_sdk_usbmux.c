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

#include <vatek_sdk_usbmux.h>
#include <vatek_sdk_usbstream.h>
#include <mux/mux_handle.h>
#include <mux/mux_spec.h>
#include "device/internal/mux_usbstream.h"

#define HUSBMUX_MEMPOOL_SIZE		1024*1024
#define HUSBMUX_TAG_PROGRAM			0xFF880001
#define HUSBMUX_TAG_STREAM			0xFF880002

struct _handle_usbmux;

typedef struct _handle_umstream
{
	usbmux_stream stream;
	uint16_t index;
	uint16_t streamtype;
	uint32_t tag;
	hmux_stream hstream;
	struct _handle_umstream* next;
	struct _handle_usbmux* husb;
}handle_umstream, *Phandle_umstream;

typedef struct _handle_umprogram
{
	usbmux_program program;
	uint32_t tag;
	hmux_program hprogram;
	Phandle_umstream streams;
	Phandle_umstream laststream;
	struct _handle_umprogram* next;
	struct _handle_usbmux* husb;
}handle_umprogram, *Phandle_umprogram;

typedef struct _handle_usbmux
{
	usbmux_status umstatus;
	usbmux_param uparam;
	Pth_mempool hmempool;
	hvatek_chip hchip;
	hvatek_usbstream husbstream;
	hmux_core hmux;
	hmux_channel hchannel;
	Phandle_muxer pmuxer;
	Phandle_umprogram programs;
	Phandle_umprogram lastprogram;
}handle_usbmux, *Phandle_usbmux;

#define usbmux_is_running(h)	(h->umstatus == umux_status_running || h->umstatus == umux_status_prepare)

/* start and stop usb_stream core */
extern vatek_result usbmux_start_stream(Phandle_usbmux pumux);
extern void usbmux_stop_stream(Phandle_usbmux pumux);

/* create _muxer and mux_handle */
extern int32_t usbmux_get_streams_nums(Phandle_usbmux pumux);
extern vatek_result usbmux_set_stream(Phandle_umprogram phprog, Phandle_umstream phstream);
extern vatek_result usbmux_set_channel(Phandle_usbmux pumux, Phandle_muxer pmuxer);

vatek_result vatek_usbmux_create(hvatek_chip hchip, Pusbmux_param puparam, hvatek_usbmux* husbmux)
{
	hvatek_usbstream hstream = NULL;
	vatek_result nres = vatek_usbstream_open(hchip, &hstream);
	if (is_vatek_success(nres))
	{
		Pth_mempool pmempool = th_mempool_create(HUSBMUX_MEMPOOL_SIZE);
		nres = vatek_memfail;
		if (pmempool)
		{
			Phandle_usbmux pmusb = (Phandle_usbmux)malloc(sizeof(handle_usbmux));
			if (pmusb)
			{
				memset(pmusb, 0, sizeof(handle_usbmux));
				pmusb->hchip = hchip;
				pmusb->hmempool = pmempool;
				nres = mux_handle_create(&pmusb->hmux);
				if (is_vatek_success(nres))
				{
					memcpy(&pmusb->uparam, puparam, sizeof(usbmux_param));
					pmusb->umstatus = umux_status_idle;
					pmusb->husbstream = hstream;
					*husbmux = pmusb;
				}
				if (!is_vatek_success(nres))free(pmusb);
			}
			if (!is_vatek_success(nres))th_mempool_free(pmempool);
		}
		if (!is_vatek_success(nres))vatek_usbstream_close(hstream);
	}
	return nres;
}

vatek_result vatek_usbmux_open_rule(hvatek_usbmux husbmux, const char* filerule)
{
	Phandle_usbmux pmusb = (Phandle_usbmux)husbmux;
	vatek_result nres = vatek_badstatus;
	if (pmusb->umstatus == umux_status_idle)
	{
		vatek_usbmux_close(husbmux);
		nres = mux_open_channel_rule(pmusb->hmux, filerule, &pmusb->hchannel);
	}
	return nres;
}

vatek_result vatek_usbmux_open_spec(hvatek_usbmux husbmux, mux_spec_mode spec, mux_country_code country)
{
	Phandle_usbmux pmusb = (Phandle_usbmux)husbmux;
	vatek_result nres = vatek_badstatus;
	if (pmusb->umstatus == umux_status_idle)
	{
		vatek_usbmux_close(husbmux);
		nres = mux_open_channel_default(pmusb->hmux, spec, country, &pmusb->hchannel);
	}
	return nres;
}

void vatek_usbmux_close(hvatek_usbmux husbmux)
{
	Phandle_usbmux pmusb = (Phandle_usbmux)husbmux;
	vatek_result nres = vatek_badstatus;
	if (pmusb->umstatus == umux_status_idle)
	{
		if (pmusb->hchannel)
			mux_handle_reset(pmusb->hmux);
	}
	pmusb->hchannel = NULL;
}

vatek_result vatek_usbmux_get_param(hvatek_usbmux husbmux, Pspec_channel* pchparam)
{
	Phandle_usbmux pmusb = (Phandle_usbmux)husbmux;
	vatek_result nres = vatek_badstatus;
	if (pmusb->umstatus == umux_status_idle && pmusb->hchannel)
		nres = muxchannel_get_param(pmusb->hchannel,pchparam);
	return nres;
}

Pbroadcast_info vatek_usbmux_get_info(hvatek_usbmux husbmux)
{
	Phandle_usbmux pmusb = (Phandle_usbmux)husbmux;
	return vatek_usbstream_get_info(pmusb->husbstream);
}

vatek_result vatek_usbmux_replace_param(hvatek_usbmux husbmux, Pspec_channel pchparam)
{
	Phandle_usbmux pmusb = (Phandle_usbmux)husbmux;
	vatek_result nres = vatek_badstatus;
	if (pmusb->umstatus == umux_status_idle && pmusb->hchannel)
		nres = muxchannel_replace_param(pmusb->hchannel, pchparam);
	return nres;
}

mux_spec_mode vatek_usbmux_get_spec(hvatek_usbmux husbmux)
{
	Pspec_channel pchparam = NULL;
	vatek_result nres = vatek_usbmux_get_param(husbmux, &pchparam);
	if (is_vatek_success(nres))
		return pchparam->spec;
	return mux_spec_undefine;
}

mux_country_code vatek_usbmux_get_country(hvatek_usbmux husbmux)
{
	Pspec_channel pchparam = NULL;
	vatek_result nres = vatek_usbmux_get_param(husbmux, &pchparam);
	if (is_vatek_success(nres))
		return pchparam->country;
	return mux_country_undefined;
}

vatek_result vatek_usbmux_reset(hvatek_usbmux husbmux)
{
	Phandle_usbmux pmusb = (Phandle_usbmux)husbmux;
	vatek_result nres = vatek_badstatus;
	if (pmusb->umstatus == umux_status_idle)
	{
		if (pmusb->pmuxer)usbstream_muxer_close(pmusb->pmuxer);
		th_mempool_reset(pmusb->hmempool);
		pmusb->pmuxer = NULL;
		pmusb->programs = NULL;
		pmusb->lastprogram = NULL;
		nres = vatek_success;
	}
	return nres;
}

vatek_result vatek_usbmux_add_program(hvatek_usbmux husbmux, uint16_t pmtpid, Pusbmux_program* program)
{
	Phandle_usbmux pmusb = (Phandle_usbmux)husbmux;
	vatek_result nres = vatek_badstatus;
	if (pmusb->umstatus == umux_status_idle && pmusb->hchannel)
	{
		Phandle_umprogram newprog = (Phandle_umprogram)th_mempool_malloc(pmusb->hmempool, sizeof(handle_umprogram));
		nres = vatek_memfail;
		if (newprog)
		{
			hmux_program hprogram = NULL;
			nres = muxchannel_add_program(pmusb->hchannel, pmtpid, pmusb->uparam.pcr_pid, &hprogram);
			if (is_vatek_success(nres))
			{
				Pusbmux_program puprogram = &newprog->program;
				puprogram->pmtpid = pmtpid;
				newprog->tag = HUSBMUX_TAG_PROGRAM;
				newprog->husb = pmusb;
				newprog->hprogram = hprogram;

				if (!pmusb->programs)pmusb->programs = newprog;
				else pmusb->lastprogram->next = newprog;
				pmusb->lastprogram = newprog;
				if (program)*program = puprogram;
				nres = vatek_success;
			}
		}
	}
	return nres;
}

vatek_result vatek_usbmux_program_get_param(Pusbmux_program program, Pspec_program* pprogparam)
{
	Phandle_umprogram phprogram = (Phandle_umprogram)program;
	vatek_result nres = vatek_badparam;
	if (phprogram->tag == HUSBMUX_TAG_PROGRAM)
	{
		*pprogparam = muxprogram_get_param(phprogram->hprogram);
		nres = vatek_success;
	}
	return nres;
}

vatek_result vatek_usbmux_program_replace_param(Pusbmux_program program, Pspec_program pprogram)
{
	Phandle_umprogram phprogram = (Phandle_umprogram)program;
	vatek_result nres = vatek_badparam;
	if (phprogram->tag == HUSBMUX_TAG_PROGRAM)
	{
		nres = muxprogram_replace_param(phprogram->hprogram, pprogram);
	}
	return nres;
}

#define _video_stream_type(v)	muxtool_get_video_streamtype(v->vcodec)
#define _audio_stream_type(a)	muxtool_get_audio_streamtype(a->acodec)

vatek_result vatek_usbmux_add_video_stream(Pusbmux_program program, uint16_t pid, Pmedia_video pvideo, Pusbmux_stream* pstream)
{
	vatek_result nres = vatek_usbmux_add_stream(program, pid, _video_stream_type(pvideo), pstream);
	if (is_vatek_success(nres))
	{
		Phandle_umstream phstream = (Phandle_umstream)*pstream;
		Pusbmux_stream pustream = (Pusbmux_stream)*pstream;
		pustream->type = mux_stream_video;
		memcpy(&pustream->_stream_info.video, pvideo, sizeof(media_video));
		nres = vatek_success;
	}
	return nres;
}

vatek_result vatek_usbmux_add_audio_stream(Pusbmux_program program, uint16_t pid, Pmedia_audio paudio, Pusbmux_stream* pstream)
{
	vatek_result nres = vatek_usbmux_add_stream(program, pid, _audio_stream_type(paudio), pstream);
	if (is_vatek_success(nres))
	{
		Phandle_umstream phstream = (Phandle_umstream)*pstream;
		Pusbmux_stream pustream = (Pusbmux_stream)*pstream;
		pustream->type = mux_stream_audio;
		memcpy(&pustream->_stream_info.audio, paudio, sizeof(media_audio));
		nres = vatek_success;
	}
	return nres;
}

vatek_result vatek_usbmux_add_stream(Pusbmux_program program, uint16_t pid, uint8_t streamtype, Pusbmux_stream* pstream)
{
	Phandle_umprogram phprog = (Phandle_umprogram)program;
	vatek_result nres = vatek_badparam;
	if (phprog->tag == HUSBMUX_TAG_PROGRAM)
	{
		Phandle_usbmux pmusb = (Phandle_usbmux)phprog->husb;
		Phandle_umstream newstream = (Phandle_umstream)th_mempool_malloc(pmusb->hmempool, sizeof(handle_umstream));
		nres = vatek_memfail;
		if (newstream)
		{
			Pusbmux_stream pmstream = &newstream->stream;
			pmstream->pid = pid;
			pmstream->type = mux_stream_data;
			newstream->husb = phprog->husb;
			newstream->streamtype = streamtype;
			newstream->tag = HUSBMUX_TAG_STREAM;

			if (!phprog->streams)
				phprog->streams = newstream;
			else phprog->laststream->next = newstream;
			phprog->laststream = newstream;

			if (pstream)*pstream = pmstream;
			nres = vatek_success;
		}
	}
	return nres;
}

vatek_result vatek_usbmux_stream_set_esinfo(Pusbmux_stream pstream, uint8_t* pesbuf, int32_t neslen)
{
	Phandle_umstream phstream = (Phandle_umstream)pstream;
	vatek_result nres = vatek_badparam;
	if (phstream->tag == HUSBMUX_TAG_STREAM && neslen <= PES_STREAM_ES_INFO_LEN)
	{
		nres = vatek_memfail;
		if(!pstream->es_info_buf)
			pstream->es_info_buf = th_mempool_malloc(phstream->husb->hmempool,PES_STREAM_ES_INFO_LEN);

		if(pstream->es_info_buf)
		{
			memset(pstream->es_info_buf,0,PES_STREAM_ES_INFO_LEN);
			memcpy(pstream->es_info_buf,pesbuf,neslen);
			pstream->es_info_len = neslen;
			nres = vatek_success;
		}
	}
	return nres;
}

void vatek_usbmux_stream_set_private(Pusbmux_stream pstream, void* param)
{
	pstream->private_data = param;
}

void* vatek_usbmux_stream_get_private(Pusbmux_stream pstream)
{
	return pstream->private_data;
}

vatek_result vatek_usbmux_finish(hvatek_usbmux husbmux)
{
	Phandle_usbmux pumux = (Phandle_usbmux)husbmux;
	vatek_result nres = vatek_badstatus;
	if (pumux->umstatus == umux_status_idle && !pumux->pmuxer)
	{
		int32_t nums = usbmux_get_streams_nums(pumux);
		nres = vatek_badparam;
		if (nums > 0)
		{
			nres = usbstream_muxer_create(pumux->hmempool, &pumux->uparam, nums, &pumux->pmuxer);
			if (is_vatek_success(nres))
				nres = usbmux_set_channel(pumux, pumux->pmuxer);
		}
		if (is_vatek_success(nres))
			pumux->umstatus = umux_status_ready;
	}
	return nres;
}

vatek_result vatek_usbmux_start(hvatek_usbmux husbmux, Pusbmux_source psource)
{
	Phandle_usbmux pumux = (Phandle_usbmux)husbmux;
	vatek_result nres = vatek_badstatus;
	if (pumux->umstatus == umux_status_ready)
	{
		nres = mux_handle_set_hal(pumux->hmux, pumux->hchip);
		if(is_vatek_success(nres))
			nres = usbmux_start_stream(pumux);

		if (is_vatek_success(nres))
		{
			nres = usbstream_muxer_start(pumux->pmuxer, psource, pumux->husbstream);
			if (!is_vatek_success(nres))
				usbmux_stop_stream(pumux);
			else pumux->umstatus = umux_status_running;
		}
	}
	return nres;
}

void vatek_usbmux_stop(hvatek_usbmux husbmux)
{
	Phandle_usbmux pumux = (Phandle_usbmux)husbmux;
	if (pumux->umstatus != umux_status_idle)
	{
		usbstream_muxer_stop(pumux->pmuxer);
		usbmux_stop_stream(pumux);
	}
	pumux->umstatus = umux_status_idle;
}

void vatek_usbmux_free(hvatek_usbmux husbmux)
{
	Phandle_usbmux pmusb = (Phandle_usbmux)husbmux;
	if (pmusb->umstatus != umux_status_idle)
		vatek_usbmux_stop(husbmux);
	vatek_usbmux_reset(pmusb);
	vatek_usbstream_close(pmusb->husbstream);
	mux_handle_free(pmusb->hmux);
	th_mempool_free(pmusb->hmempool);
	free(pmusb);
}

usbmux_status vatek_usbmux_get_status(hvatek_usbmux husbmux, Ptransform_info* pinfo)
{
	Phandle_usbmux pmusb = (Phandle_usbmux)husbmux;
	if (usbmux_is_running(pmusb))
	{
		Ptransform_info psinfo = NULL;
		usbstream_status ustatus = vatek_usbstream_get_status(pmusb->husbstream,&psinfo);
		if (ustatus == usbstream_status_running)
		{
			if (pinfo)*pinfo = psinfo;
			return umux_status_running;
		}
	}
	return pmusb->umstatus;
}

vatek_result vatek_usbmux_push_frame(hvatek_usbmux husbmux, Pusbmux_stream pstream, Pmux_pes_frame pframe)
{
	vatek_result nres = vatek_badstatus;
	Phandle_usbmux pmusb = (Phandle_usbmux)husbmux;
	if (usbmux_is_running(pmusb))
	{
		Phandle_umstream phstream = (Phandle_umstream)pstream;
		Phandle_muxer phmuxer = (Phandle_muxer)phstream->husb->pmuxer;
		if (phstream->tag == HUSBMUX_TAG_STREAM)
		{
			nres = usbstream_muxer_put_frame(phmuxer,phstream->index, pframe);
		}
	}
	return nres;
}

vatek_result usbmux_start_stream(Phandle_usbmux pumux)
{
	vatek_result nres = vatek_memfail;
	usbstream_param ustream;
	ustream.r2param.freqkhz = pumux->uparam.r2param.freqkhz;
	ustream.r2param.mode = pumux->uparam.r2param.mode;
	ustream.mode = ustream_mode_async;
	ustream.remux = ustream_remux_pcr;
	ustream.pcradjust = pcr_adjust;
	memcpy(&ustream.modulator, &pumux->uparam.modulator, sizeof(modulator_param));
	ustream.async.bitrate = pumux->uparam.bitrate;
	ustream.async.prepare_ms = pumux->uparam.latency_ms;
	nres = vatek_usbstream_start(pumux->husbstream, &ustream);
	if (!is_vatek_success(nres))
		vatek_usbstream_stop(pumux->husbstream);
	return nres;
}

void usbmux_stop_stream(Phandle_usbmux pumux)
{
	if (pumux->husbstream)
		vatek_usbstream_stop(pumux->husbstream);
}

vatek_result usbmux_set_channel(Phandle_usbmux pumux, Phandle_muxer pmuxer)
{
	vatek_result nres = vatek_badparam;
	Phandle_umprogram ptrprog = pumux->programs;
	int32_t idx = 0;
	while (ptrprog)
	{
		Phandle_umstream ptrstream = ptrprog->streams;
		while (ptrstream)
		{
			nres = usbmux_set_stream(ptrprog, ptrstream);
			if (!is_vatek_success(nres))break;
			else
			{
				ptrstream->index = idx;
				nres = usbstream_muxer_set_stream(pumux->hmempool,pmuxer, idx,ptrstream->hstream, (Pusbmux_stream)&ptrstream->stream);
			}
			if (!is_vatek_success(nres))break;
			ptrstream = ptrstream->next;
			idx++;
		}
		if (!is_vatek_success(nres))break;
		ptrprog = ptrprog->next;
	}
	return nres;
}

vatek_result usbmux_set_stream(Phandle_umprogram phprog,Phandle_umstream phstream)
{
	Pusbmux_stream pustream = &phstream->stream;
	vatek_result nres = vatek_badparam;
	if (pustream->type == mux_stream_video)
	{
		nres = muxprogram_add_video(phprog->hprogram,
			pustream->pid,
			&pustream->_stream_info.video,
			&phstream->hstream);
	}
	else if (pustream->type == mux_stream_audio)
	{
		nres = muxprogram_add_audio(phprog->hprogram,
			pustream->pid,
			&pustream->_stream_info.audio,
			&phstream->hstream);
	}
	else
	{
		nres = muxprogram_add_stream(phprog->hprogram,
			pustream->pid,
			(uint8_t)phstream->streamtype,
			&phstream->hstream);
	}

	if (is_vatek_success(nres))
	{
		if (pustream->es_info_buf)
			muxstream_set_es_info(phstream->hstream,pustream->es_info_buf,pustream->es_info_len);
	}
	return nres;
}

int32_t usbmux_get_streams_nums(Phandle_usbmux pumux)
{
	int32_t nums = 0;
	Phandle_umprogram program = pumux->programs;
	while (program)
	{
		Phandle_umstream pstream = program->streams;
		while (pstream)
		{
			nums++;
			pstream = pstream->next;
		}
		program = program->next;
	}
	return nums;
}

