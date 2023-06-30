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
#include <mux/mux_rule.h>
#include "internal/mux_core.h"

vatek_result mux_handle_create(hmux_core* hmux)
{
	Pth_mempool pmpool = th_mempool_create(MUX_CORE_MEMPOOL_SIZE);
	vatek_result nres = vatek_memfail;
	if (pmpool)
	{
		Phandle_mux phmux = (Phandle_mux)malloc(sizeof(handle_mux));
		if (phmux)
		{
			memset(phmux, 0, sizeof(handle_mux));
			phmux->mempool = pmpool;
			mux_handle_reset((hmux_core)phmux);
			*hmux = phmux;
			nres = vatek_success;
		}
		if (!is_vatek_success(nres))th_mempool_free(pmpool);
	}
	return nres;
}

void mux_handle_reset(hmux_core hmux)
{
	Phandle_mux pmux = (Phandle_mux)hmux;
	mux_reset(pmux);
}

void mux_handle_free(hmux_core hmux)
{
	Phandle_mux pmux = (Phandle_mux)hmux;
	mux_handle_reset(hmux);
	th_mempool_free(pmux->mempool);
	free(pmux);	
}

mux_spec_mode mux_handle_get_spec(hmux_core hmux)
{
	Phandle_mux pmux = (Phandle_mux)hmux;
	if (pmux->hmode == hmux_mode_channel ||
		pmux->hmode == hmux_mode_broadcast)
	{
		return pmux->_h.channel->spec;
	}
	return mux_spec_undefine;
}

mux_country_code mux_handle_get_country(hmux_core hmux)
{
	Phandle_mux pmux = (Phandle_mux)hmux;
	if (pmux->hmode == hmux_mode_channel ||
		pmux->hmode == hmux_mode_broadcast)
	{
		return pmux->_h.channel->country;
	}
	return mux_country_undefined;
}

mux_psi_mode mux_handle_get_mode(hmux_core hmux)
{
	Phandle_mux pmux = (Phandle_mux)hmux;
	if(pmux->_h.handle)
		return pmux->mode;
	return mux_psi_undefine;
}

vatek_result mux_handle_get_channel(hmux_core hmux, hmux_channel* hch)
{
	Phandle_mux pmux = (Phandle_mux)hmux;
	if (pmux->hmode == hmux_mode_channel ||
		pmux->hmode == hmux_mode_broadcast)
	{
		*hch = pmux;
		return vatek_success;
	}
	return vatek_unsupport;
}

vatek_result mux_handle_get_rule(hmux_core hmux, Pmux_rule_block* prule)
{
	Phandle_mux pmux = (Phandle_mux)hmux;
	if (pmux->mode == mux_psi_specrule)
	{
		*prule = pmux->rule;
		return vatek_success;
	}
	return vatek_unsupport;
}

vatek_result mux_handle_malloc_epgparam(hmux_core hmux, mux_epg_mode mode, Pmux_epg_param* pepg)
{
	Phandle_mux pmux = (Phandle_mux)hmux;
	return muxepg_create(pmux->mempool, mode, pepg);
}

uint8_t muxtool_get_video_streamtype(video_codec vcodec)
{
	if (vcodec == encvideo_mpeg2)
		return STREAMTYPE_ISO13818_2_VIDEO;
	else if (vcodec == encvideo_h264)
		return STREAMTYPE_ISO14496_10_VIDEO;
	else if (vcodec == encvideo_hevc)
		return STREAMTYPE_ISO23008_2_VIDEO;
	VWAR("bad video codec unknown stream type : %d", vcodec);
	return STREAMTYPE_ISO13818_2_VIDEO;
}

#if 0
vatek_result mux_malloc_rule_block(hmux_core hmux, int32_t blocklen, Pmux_rule_block* prule)
{
	Phandle_mux pmux = (Phandle_mux)hmux;
	int32_t len = sizeof(mux_rule_block) + blocklen;
	uint8_t* pbuf = th_mempool_malloc(pmux->mempool, len);
	vatek_result nres = vatek_memfail;
	if (pbuf)
	{
		Pmux_rule_block newblock = (Pmux_rule_block)&pbuf[0];
		memset(pbuf, 0, len);
		newblock->rawblock = &pbuf[sizeof(mux_rule_block)];
		newblock->block_len = blocklen;
		*prule = newblock;
		nres = vatek_success;
	}
	return nres;
}
#endif

uint8_t* mux_handle_malloc_buffer(hmux_core hmux, int32_t len)
{
	Phandle_mux pmux = (Phandle_mux)hmux;
	return th_mempool_malloc(pmux->mempool, len);
}

Pth_mempool mux_handle_get_mempool(hmux_core hmux)
{
	Phandle_mux pmux = (Phandle_mux)hmux;
	return pmux->mempool;
}

uint8_t muxtool_get_audio_streamtype(audio_codec acodec)
{
	if (acodec == encaudio_mp2)
		return STREAMTYPE_ISO11172_AUDIO;
	else if (acodec == encaudio_aac_lc_adts)
		return STREAMTYPE_ISO13818_7_ADTS;
	else if (acodec == encaudio_aac_lc_latm)
		return STREAMTYPE_ISO144962_3_LATM;
	else if (acodec == encaudio_ac3)
		return STREAMTYPE_PES_PRIVATE_DATA;
	VWAR("bad audio codec unknown stream type : %d", acodec);
	return STREAMTYPE_ISO11172_AUDIO;
}

int32_t mux_check_idle(Phandle_mux pmux)
{
	return (pmux->_h.handle ? 0 : 1);
}

void mux_reset(Phandle_mux pmux)
{
	th_mempool_reset(pmux->mempool);
	pmux->rule = NULL;
	pmux->_h.handle = NULL;
	pmux->hmode = hmux_mode_null;
	pmux->mode = mux_psi_undefine;
}

vatek_result mux_malloc_psiparam_buffer_rule(Pth_mempool pmem,Pmux_rule_block prule,Ppsiparam_buffer pbuffer)
{
	vatek_result nres = vatek_memfail;
	pbuffer->ch_len =  muxrule_copy_channel(prule, NULL, 0);
	pbuffer->ch_buf = th_mempool_malloc(pmem, pbuffer->ch_len);
	if(pbuffer->ch_buf)
	{
		pbuffer->prog_len = muxrule_copy_program(prule, NULL, 0);
		pbuffer->prog_buf = th_mempool_malloc(pmem,pbuffer->prog_len);
		if(pbuffer->prog_buf)nres = vatek_success;
	}
	return nres;
}

vatek_result mux_malloc_psiparam_buffer_default(Pth_mempool pmem,Pspec_channel pch,Ppsiparam_buffer pbuffer)
{
	vatek_result nres = muxspec_get_channel_len(pch->spec);
	if(is_vatek_success(nres))
	{
		pbuffer->ch_len = (int32_t)nres;
		pbuffer->ch_buf = th_mempool_malloc(pmem,pbuffer->ch_len);
	}

	if(is_vatek_success(nres))
	{
		nres = muxspec_get_program_len(pch->spec);
		if(is_vatek_success(nres))
		{
			pbuffer->prog_len = (int32_t)nres;
			pbuffer->prog_buf = th_mempool_malloc(pmem,pbuffer->prog_len);
		}
	}
	return nres;
}
