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

#include <mux/mux_handle.h>
#include "internal/mux_core.h"

extern vatek_result channel_malloc_psispec(Phandle_mux pmux,Phandle_channel pch);
extern void program_stream_put(Ppsispec_program pprog, Ppsispec_stream pstream);
extern void program_stream_set(Ppsispec_stream pstream, mux_stream_type mode, Phandle_mux pmux, uint16_t pid, uint8_t streamtype);

vatek_result mux_open_channel_rule(hmux_core hmux,const char* filename,hmux_channel* hchannel)
{
    vatek_result nres = vatek_badstatus;
    Phandle_mux pmux = (Phandle_mux)hmux;
    if(mux_check_idle(pmux))
    {
		Pmux_rule_block prule = NULL;
		nres = muxrule_load_file(pmux->mempool,filename, &prule);
        if(is_vatek_success(nres))
        {
            Phandle_channel newch = (Phandle_channel)th_mempool_malloc(pmux->mempool,sizeof(handle_channel));
            nres = vatek_memfail;
            if(newch)
            {
                pmux->rule = prule;
                pmux->mode = mux_psi_specrule;
                pmux->hmode = hmux_mode_channel;
                newch->country = muxrule_get_country(prule);
                newch->spec = muxrule_get_spec(prule);
                nres = channel_malloc_psispec(pmux,newch);
                if(is_vatek_success(nres))
                {
                    pmux->_h.channel = newch;
                    *hchannel = pmux;
                }
            }
        }
        if(!is_vatek_success(nres))mux_reset(pmux);
    }
    return nres;
}

vatek_result mux_open_channel_rule_raw(hmux_core hmux, Pmux_rule_block pblock, hmux_channel* hchannel)
{
	vatek_result nres = vatek_badstatus;
	Phandle_mux pmux = (Phandle_mux)hmux;
	if (mux_check_idle(pmux))
	{
		Phandle_channel newch = (Phandle_channel)th_mempool_malloc(pmux->mempool, sizeof(handle_channel));
		nres = vatek_memfail;
		if (newch)
		{
			pmux->rule = pblock;
			pmux->mode = mux_psi_specrule;
			pmux->hmode = hmux_mode_channel;
			newch->country = muxrule_get_country(pblock);
			newch->spec = muxrule_get_spec(pblock);
			nres = channel_malloc_psispec(pmux, newch);
			if (is_vatek_success(nres))
			{
				pmux->_h.channel = newch;
				*hchannel = pmux;
			}
		}
		if (!is_vatek_success(nres))mux_reset(pmux);
	}
	return nres;
}

vatek_result mux_open_channel_default(hmux_core hmux, mux_spec_mode spec, mux_country_code country,hmux_channel* hchannel)
{
    vatek_result nres = vatek_badstatus;
    Phandle_mux pmux = (Phandle_mux)hmux;
    if(mux_check_idle(pmux))
    {
        nres = muxspec_check_support(spec,country);
        if(is_vatek_success(nres))
        {
            Phandle_channel newch = (Phandle_channel)th_mempool_malloc(pmux->mempool,sizeof(handle_channel));
            nres = vatek_memfail;
            if(newch)
            {
                pmux->rule = NULL;
                pmux->mode = mux_psi_specdefault;
                pmux->hmode = hmux_mode_channel;
                newch->country = country;
                newch->spec = spec;
                nres = channel_malloc_psispec(pmux,newch);
                if(is_vatek_success(nres))
                {
                    pmux->_h.channel = newch;
                    *hchannel = pmux;
                }
            }            
        }
        if(!is_vatek_success(nres))mux_reset(pmux);
    }
    return nres;
}

vatek_result muxchannel_get_program_nums(hmux_channel hch)
{
	Phandle_mux pmux = (Phandle_mux)hch;
	int32_t counts = 0;
	Pmux_program ptrprog = pmux->_h.channel->context.programs;
	while (ptrprog)
	{
		counts++;
		ptrprog = ptrprog->next;
	}
	return (vatek_result)counts;
}

vatek_result muxchannel_get_param(hmux_channel hch, Pspec_channel* pchannel)
{
    Phandle_mux pmux = (Phandle_mux)hch;
    vatek_result nres = vatek_badstatus;
    if(pmux->_h.handle)
    {
        Phandle_channel pch = pmux->_h.channel;
        *pchannel = pch->context.specchannel;
        nres = vatek_success;
    }
    return nres;
}

vatek_result muxchannel_replace_param(hmux_channel hch, Pspec_channel pchannel)
{
    Phandle_mux pmux = (Phandle_mux)hch;
    vatek_result nres = vatek_badstatus;
    if(pmux->_h.handle)
    {
        Phandle_channel pch = pmux->_h.channel;
		Pspec_channel pchprop = pch->context.specchannel;
		if (pchprop->spec == pchannel->spec && pchprop->country == pchannel->country)
		{
			pch->context.specchannel = pchannel;
			nres = vatek_success;
		}
		else nres = vatek_badparam;
    }
    return nres;
}

vatek_result muxchannel_add_program(hmux_channel hch, uint16_t pmtpid, uint16_t pcrpid, hmux_program* hprog)
{
	Phandle_mux pmux = (Phandle_mux)hch;
    vatek_result nres = vatek_badstatus;

    if(pmux->_h.handle)
    {
        Phandle_channel pch = pmux->_h.channel;
        Ppsispec_program newprog = (Ppsispec_program)th_mempool_malloc(pmux->mempool, sizeof(psispec_program));
		nres = vatek_memfail;

		if (newprog)
		{
			newprog->hpsi = pmux;
			newprog->mprogram.pmt_pid = pmtpid;
			newprog->mprogram.pcr_pid = pcrpid;

			nres = muxspec_malloc_program(pmux->mempool, pch->spec, &newprog->specprogram);
			if (is_vatek_success(nres) && pmux->mode == mux_psi_specrule)
				nres = muxrule_get_program_param(pmux->rule, newprog->specprogram);
			else nres = muxspec_set_program_default(muxchannel_get_program_nums(hch), newprog->specprogram);

			if (is_vatek_success(nres))
			{
				Ppsispec_handle pspec = &pch->context;
				if (!pspec->programs)
					pspec->programs = &newprog->mprogram;
				else pspec->lastprogram->next = &newprog->mprogram;
				pspec->lastprogram = &newprog->mprogram;
				*hprog = newprog;
			}
		}
    }
    return nres;
}

hmux_program muxchannel_get_first_program(hmux_channel hch)
{
	Phandle_mux pmux = (Phandle_mux)hch;
    if(pmux->_h.handle)
    {
		Phandle_channel pch = pmux->_h.channel;
		return pch->context.programs;
    }
    return NULL;	
}

hmux_program muxchannel_get_next_program(hmux_program hprog)
{
	Ppsispec_program ptrprogram = (Ppsispec_program)hprog;
	return ptrprogram->mprogram.next;
}

Pmux_program muxprogram_get(hmux_program hprog)
{
	Ppsispec_program pprog = (Ppsispec_program)hprog;
	return &pprog->mprogram;
}

vatek_result muxprogram_get_stream_nums(hmux_program hprog)
{
	Ppsispec_program pprog = (Ppsispec_program)hprog;
	int32_t count = 0;
	Pmux_stream ptrstream = pprog->mprogram.streams;
	while (ptrstream)
	{
		count++;
		ptrstream = ptrstream->next;
	}
	return (vatek_result)count;
}

vatek_result muxprogram_malloc_epg(hmux_program hprog, mux_epg_mode mode, Pmux_epg_param* pepg)
{
	vatek_result nres = vatek_unsupport;
	Ppsispec_program pprog = (Ppsispec_program)hprog;
    Phandle_mux pmux = pprog->hpsi;
	if (pmux->mode == mux_psi_specrule &&
		pmux->rule->rule_runction & RULE_EN_EIT)
	{
		if (!pprog->pepg)
		{
			Pmux_epg_param pnewepg = NULL;
			nres = muxepg_create(pmux->mempool, mode, &pnewepg);
		}
		if (is_vatek_success(nres))*pepg = pprog->pepg;
	}
	return nres;
}

vatek_result muxprogram_set_epg(hmux_program hprog, Pmux_epg_param pepg)
{
	vatek_result nres = vatek_unsupport;
	Ppsispec_program pprog = (Ppsispec_program)hprog;
	//Phandle_mux pmux = pprog->hpsi;
	//if (pmux->mode == mux_psi_specrule &&
	//	pmux->rule->rule_runction & RULE_EN_EIT)
	{
		pprog->pepg = pepg;
		nres = vatek_success;
	}
	return nres;
}

Pmux_epg_param muxprogram_get_epg(hmux_program hprog)
{
	Ppsispec_program pprog = (Ppsispec_program)hprog;
	return pprog->pepg;
}

Pspec_program muxprogram_get_param(hmux_program hprog)
{
	Ppsispec_program pprog = (Ppsispec_program)hprog;
	return pprog->specprogram;
}

vatek_result muxprogram_replace_param(hmux_program hprog, Pspec_program pprogram)
{
	Ppsispec_program pprog = (Ppsispec_program)hprog;
	Pspec_program pspec = pprog->specprogram;
	vatek_result nres = vatek_badparam;
	if (pspec->spec == pprogram->spec)
	{
		pprog->specprogram = pprogram;
		nres = vatek_success;
	}
	return nres;
}

vatek_result muxprogram_add_video(hmux_program hprog, uint16_t pid, Pmedia_video pvideo, hmux_stream* hstream)
{
	Ppsispec_program pprog = (Ppsispec_program)hprog;
	Phandle_mux pmux = pprog->hpsi;
	vatek_result nres = vatek_memfail;
	Ppsispec_stream pstream = (Ppsispec_stream)th_mempool_malloc(pmux->mempool, sizeof(psispec_stream));
	if (pstream)
	{
		program_stream_set(pstream, mux_stream_video, pmux, pid, muxtool_get_video_streamtype(pvideo->vcodec));
		memcpy(&pstream->mstream._stream_info.video, pvideo, sizeof(media_video));
		program_stream_put(pprog, pstream);
		if (hstream)*hstream = pstream;
		nres = vatek_success;
	}
	return nres;
}

vatek_result muxprogram_add_audio(hmux_program hprog, uint16_t pid, Pmedia_audio paudio, hmux_stream* hstream)
{
	Ppsispec_program pprog = (Ppsispec_program)hprog;
	Phandle_mux pmux = pprog->hpsi;
	vatek_result nres = vatek_memfail;
	Ppsispec_stream pstream = (Ppsispec_stream)th_mempool_malloc(pmux->mempool, sizeof(psispec_stream));
	if (pstream)
	{
		uint8_t streamtype = muxtool_get_audio_streamtype(paudio->acodec);
		if(paudio->acodec == encaudio_ac3 && pprog->specprogram->spec == mux_spec_psip)
			streamtype = STREAMTYPE_ATSC_A53_AC3;
		program_stream_set(pstream, mux_stream_audio, pmux, pid, streamtype);
		memcpy(&pstream->mstream._stream_info.video, paudio, sizeof(media_audio));
		program_stream_put(pprog, pstream);
		if (hstream)*hstream = pstream;
		nres = vatek_success;
	}
	return nres;
}

vatek_result muxprogram_add_stream(hmux_program hprog, uint16_t pid, uint8_t streamtype, hmux_stream* hstream)
{
	Ppsispec_program pprog = (Ppsispec_program)hprog;
	Phandle_mux pmux = pprog->hpsi;
	vatek_result nres = vatek_memfail;
	Ppsispec_stream pstream = (Ppsispec_stream)th_mempool_malloc(pmux->mempool, sizeof(psispec_stream));
	if (pstream)
	{
		program_stream_set(pstream, mux_stream_data, pmux, pid, streamtype);
		program_stream_put(pprog, pstream);
		if (hstream)*hstream = pstream;
		nres = vatek_success;
	}
	return nres;
}

hmux_stream muxprogram_get_first_stream(hmux_program hprog)
{
	Ppsispec_program pprog = (Ppsispec_program)hprog;
	return pprog->mprogram.streams;
}

hmux_stream muxprogram_get_next_stream(hmux_stream hstream)
{
	Ppsispec_stream ptrstream = (Ppsispec_stream)hstream;
	return ptrstream->mstream.next;
}

Pmux_stream muxstream_get(hmux_stream hstream)
{
	Ppsispec_stream pstream = (Ppsispec_stream)hstream;
	return &pstream->mstream;
}

void muxstream_set_es_info(hmux_stream hstream, uint8_t* pesinfo, int32_t len)
{
	Ppsispec_stream pstream = (Ppsispec_stream)hstream;
    Phandle_mux pmux = pstream->hpsi;
	if (!pstream->mstream.es_info_buf)
		pstream->mstream.es_info_buf = th_mempool_malloc(pmux->mempool, PES_STREAM_ES_INFO_LEN);

	if (pstream->mstream.es_info_buf)
	{
		memset(pstream->mstream.es_info_buf, 0, PES_STREAM_ES_INFO_LEN);
		memcpy(pstream->mstream.es_info_buf, pesinfo, len);
		pstream->mstream.es_info_len = len;
	}
}

void muxstream_get_es_info(hmux_stream hstream, uint8_t** pesinfo, int32_t* len)
{
	Ppsispec_stream pstream = (Ppsispec_stream)hstream;
	if (pstream->mstream.es_info_buf)
	{
		*pesinfo = pstream->mstream.es_info_buf;
		*len = PES_STREAM_ES_INFO_LEN;
	}
	else
	{
		*pesinfo = NULL;
		*len = 0;
	}
}

void muxstream_set_private(hmux_stream hstream, void* pdata)
{
	Ppsispec_stream pstream = (Ppsispec_stream)hstream;
	pstream->private_data = pdata;
}

void* muxstream_get_private(hmux_stream hstream)
{
	Ppsispec_stream pstream = (Ppsispec_stream)hstream;
	return pstream->private_data;
}

vatek_result channel_malloc_psispec(Phandle_mux pmux,Phandle_channel pch)
{
	Pspec_channel pchpro = NULL;
	vatek_result nres = muxspec_malloc_channel(pmux->mempool, pch->spec, pch->country, &pchpro);
	if (is_vatek_success(nres))
	{
		Ppsispec_handle pspec = &pch->context;
		if (pmux->mode == mux_psi_specrule)
		{
			nres = muxrule_get_channel_param(pmux->rule, pchpro);
			if(is_vatek_success(nres))
				nres = mux_malloc_psiparam_buffer_rule(pmux->mempool,pmux->rule,&pch->buffer);
		}
		else if(pmux->mode == mux_psi_specdefault)
			nres = mux_malloc_psiparam_buffer_default(pmux->mempool,pchpro,&pch->buffer);
			
		if (is_vatek_success(nres))
			pspec->specchannel = pchpro;
	}
	return nres;
}

void program_stream_set(Ppsispec_stream pstream, mux_stream_type mode, Phandle_mux pmux, uint16_t pid, uint8_t streamtype)
{
	pstream->hpsi = pmux;
	pstream->mstream.stream_pid = pid;
	pstream->mstream.type = mode;
	pstream->mstream.stream_type = streamtype;
}

void program_stream_put(Ppsispec_program pprog, Ppsispec_stream pstream)
{
	Pmux_stream pmstream = &pstream->mstream;
	if (!pprog->mprogram.streams)
		pprog->mprogram.streams = pmstream;
	else pprog->laststream->next = pmstream;
	pprog->laststream = pmstream;
}
