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

#include <core/storage/storage_section.h>
#include <core/tools/tool_bufstream.h>
#include <core/tools/tool_crc32.h>

#include <core/ui/ui_props/ui_props_chip.h>
#include <core/ui/ui_props/ui_props_encoder.h>
#include <core/ui/ui_props/ui_props_modulator.h>

/* import default params */
#include "storage_default.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern vatek_result section_put_start(uint32_t tag, Pbufstream pstream, int32_t len);
extern vatek_result section_put_end(Pbufstream pstream);

extern vatek_result section_get_start(Pbufstream pstream,uint32_t tag,uint32_t* len);

extern vatek_result section_put_ui_props(Pbufstream pstream,const Pui_prop_item puiprops,uint8_t* praw);
extern vatek_result section_get_ui_props(Pbufstream pstream,const Pui_prop_item puiprops,uint8_t* praw);

extern vatek_result section_put_mpeg2(Pbufstream pstream,Pmpeg2_param pmpeg2);
extern vatek_result section_get_mpeg2(Pbufstream pstream,Pmpeg2_param pmpeg2);

extern vatek_result section_put_h264(Pbufstream pstream,Ph264_param ph264);
extern vatek_result section_get_h264(Pbufstream pstream,Ph264_param ph264);

extern vatek_result section_put_chip_config(Pbufstream pstream,Pchip_config pcfg);
extern vatek_result section_get_chip_config(Pbufstream pstream,Pchip_config pcfg);

extern vatek_result section_put_modulation_config(Pbufstream pstream, Pmodulation_config pmod);
extern vatek_result section_get_modulation_config(Pbufstream pstream, Pmodulation_config pmod);

extern vatek_result section_put_modulator(Pbufstream pstream,Pmodulator_param pmod);
extern vatek_result section_get_modulator(Pbufstream pstream,Pmodulator_param pmod);

vatek_result storage_section_check_tag(uint8_t* psection,uint32_t* tag,uint32_t* sectionnums)
{
    bufstream s = {0,psection};
    uint32_t val = getbuf_uint32_t(&s);
    vatek_result nres = vatek_badparam;
    if(is_section_valid(val))
    {
        if(tag)*tag = val;
        if(sectionnums)*sectionnums = getbuf_uint32_t(&s);
        nres = vatek_success;
		
    }
    return nres;
}

vatek_result storage_section_check_loader(uint8_t* psection)
{
	bufstream s = { 4,psection };
	vatek_result nres = vatek_format;
	if (getbuf_uint32_t(&s) == LOADER_MAGIC_V2)
		nres = vatek_success;
	return nres;
}

vatek_result storage_section_get_loader(uint8_t* psection,Ploader_header plhdr)
{
    bufstream s = {0,psection};
    vatek_result nres = vatek_format;
    
    getbuf_buf(&s,&plhdr->bin_size[0],4);
    if(getbuf_uint32_t(&s) == LOADER_MAGIC_V2)
    {
        plhdr->magic_tag = LOADER_MAGIC_V2;
        plhdr->major_version = getbuf_uint32_t(&s);
        plhdr->minor_version = getbuf_uint32_t(&s);
        plhdr->chip_arch = (binary_chip_arch)getbuf_uint32_t(&s);
        nres = vatek_success;
    }
    return nres;
}

vatek_result storage_section_check_app(uint8_t* psection)
{
	bufstream s = { 4,psection };
	vatek_result nres = vatek_unsupport;
	if (getbuf_uint32_t(&s) == APP_MAGIC_V2)
		nres = vatek_success;
	return nres;
}

vatek_result storage_section_get_app(uint8_t* psection,Papp_header papphdr)
{
    bufstream s = {0,psection};
	vatek_result nres = vatek_unsupport;
	papphdr->bin_size = getbuf_uint32_t(&s);
	papphdr->magic_tag = getbuf_uint32_t(&s);
    if(papphdr->magic_tag == APP_MAGIC_V2)
    {
        papphdr->major_version = getbuf_uint32_t(&s);
		papphdr->minor_version = getbuf_uint32_t(&s);
		papphdr->chip_arch = (binary_chip_arch)getbuf_uint32_t(&s);
		papphdr->service_mode = (hal_service_mode)getbuf_uint32_t(&s);	/* different 1.1 and 1.2 (no service mode) */
		papphdr->crc32 = getbuf_uint32_t(&s);
		nres = vatek_success;
    }
    return nres;
}

vatek_result  storage_section_set_loader(uint8_t* psection, Ploader_header plhdr)
{
	if (plhdr->magic_tag == LOADER_MAGIC_V2)
	{
		bufstream s = { 0,psection };
		putbuf_buf(&s, &plhdr->bin_size[0], 4);
		putbuf_uint32_t(&s, plhdr->magic_tag);
		putbuf_uint32_t(&s, plhdr->major_version);
		putbuf_uint32_t(&s, plhdr->minor_version);
		putbuf_uint32_t(&s, (uint32_t)plhdr->chip_arch);
		return vatek_success;
	}
	return vatek_format;
}

vatek_result storage_section_set_app(uint8_t* psection, Papp_header papphdr)
{
	if (papphdr->magic_tag == APP_MAGIC_V2)
	{
		bufstream s = { 0,psection };
		putbuf_uint32_t(&s, papphdr->bin_size);
		putbuf_uint32_t(&s, papphdr->magic_tag);
		putbuf_uint32_t(&s, papphdr->major_version);
		putbuf_uint32_t(&s, papphdr->minor_version);
		putbuf_uint32_t(&s, papphdr->chip_arch);
		putbuf_uint32_t(&s, papphdr->service_mode);
		putbuf_uint32_t(&s, papphdr->crc32);
		return vatek_success;
	}
	return vatek_format;
}

void storage_broadcast_reset(Pstorage_broadcast pbc)
{
	memset(pbc, 0, sizeof(storage_broadcast));
	modulator_param_reset(modulator_dvb_t, &pbc->modulator);
	audio_param_reset(&pbc->audio);
	mpeg2_param_reset(resolution_480i, &pbc->mpeg2[MEDIA_IDX_VIDEO_SD]);
	mpeg2_param_reset(resolution_720p, &pbc->mpeg2[MEDIA_IDX_VIDEO_HD]);
	mpeg2_param_reset(resolution_1080p, &pbc->mpeg2[MEDIA_IDX_VIDEO_FULLHD]);
	h264_param_reset(resolution_480i, &pbc->h264[MEDIA_IDX_VIDEO_SD]);
	h264_param_reset(resolution_720p, &pbc->h264[MEDIA_IDX_VIDEO_HD]);
	h264_param_reset(resolution_1080p, &pbc->h264[MEDIA_IDX_VIDEO_FULLHD]);
}

void audio_param_reset(Paudio_param paudio)
{
	memcpy(paudio, &default_audio_param, sizeof(audio_param));
}

void mpeg2_param_reset(video_resolution resolution, Pmpeg2_param pmpeg2)
{
	if (resolution < resolution_720p)
		memcpy(pmpeg2, &default_mpeg2_codec_sd, sizeof(mpeg2_param));
	else if (resolution == resolution_720p)
		memcpy(pmpeg2, &default_mpeg2_codec_hd, sizeof(mpeg2_param));
	else memcpy(pmpeg2, &default_mpeg2_codec_fullhd, sizeof(mpeg2_param));
}

void h264_param_reset(video_resolution resolution, Ph264_param ph264)
{
	memcpy(ph264, &default_h264_codec_param, sizeof(h264_param));
}

vatek_result storage_section_put_resource(uint8_t* psection, int32_t size, Pbin_resource_section pbin, uint8_t* praw)
{
	bufstream s = { 0,psection };
	uint8_t pos = 0;
	vatek_result nres = section_put_start(BIN_SECTION_RESOURCE, &s, size);
	if (is_vatek_success(nres))
	{
		int32_t i = 0;
		putbuf_uint32_t(&s, pbin->width);
		putbuf_uint32_t(&s, pbin->height);
		putbuf_uint32_t(&s, pbin->index);
		putbuf_uint32_t(&s, pbin->bkcolor);

		for (i = 0; i < RESOURCE_COLOR_LEN; i++)
			putbuf_uint32_t(&s, pbin->table[i]);
		putbuf_uint32_t(&s, 0);

		i = BINARY_SECTION_SIZE - s.pos;
		putbuf_buf(&s, praw, i);
		nres = (vatek_result)i;
		pos = nres;
	}

	if (is_vatek_success(nres))
		nres = section_put_end(&s);
	return pos;
}

vatek_result storage_section_get_resource(uint8_t* psection, Pbin_resource_section pbin)
{
	bufstream s = { 0,psection };
	vatek_result nres = section_get_start(&s, BIN_SECTION_RESOURCE, NULL);

	if (is_vatek_success(nres))
	{
		memset(pbin, 0, sizeof(bin_resource_section));
		int32_t i = 0;
		pbin->width = getbuf_uint32_t(&s);
		pbin->height = getbuf_uint32_t(&s);
		pbin->index = getbuf_uint32_t(&s);
		pbin->bkcolor = getbuf_uint32_t(&s);

		for (i = 0; i < RESOURCE_COLOR_LEN; i++) 
			pbin->table[i] = getbuf_uint32_t(&s);
		getbuf_uint32_t(&s);
		nres = (vatek_result)s.pos;
	}
	return nres;
}

vatek_result storage_broadcast_set(Pstorage_broadcast pbc,uint8_t* psection)
{
    bufstream s = {0,psection};
    vatek_result nres = section_put_start(BIN_SECTION_BROADCAST,&s,sizeof(storage_broadcast));
    if(is_vatek_success(nres))
    {
        nres = section_put_modulator(&s,&pbc->modulator);
        if(is_vatek_success(nres))
            nres = section_put_ui_props(&s,_ui_struct(audio_param),(uint8_t*)&pbc->audio);
        if(is_vatek_success(nres))
        {
            int32_t i = 0;
            for(i = 0;i < MEDIA_IDX_VIDEO_NUMS;i++)
            {
                nres = section_put_mpeg2(&s,&pbc->mpeg2[i]);
                if(!is_vatek_success(nres))break;
            }

            if(is_vatek_success(nres))
            {
                for(i = 0;i < MEDIA_IDX_VIDEO_NUMS;i++)
                {
                    nres = section_put_h264(&s,&pbc->h264[i]);
                    if(!is_vatek_success(nres))break;
                }      
            }
        }
    }
    if(is_vatek_success(nres))
        nres = section_put_end(&s);
    return nres;
}

vatek_result storage_broadcast_get(Pstorage_broadcast pbc,uint8_t* psection)
{
    bufstream s = {0,psection};
    vatek_result nres = section_get_start(&s,BIN_SECTION_BROADCAST,NULL);
    if(is_vatek_success(nres))
    {
        memset(pbc,0,sizeof(storage_broadcast));
        nres = section_get_modulator(&s,&pbc->modulator);
        if(is_vatek_success(nres))
            nres = section_get_ui_props(&s,_ui_struct(audio_param),(uint8_t*)&pbc->audio);
        if(is_vatek_success(nres))
        {
            int32_t i = 0;
            for(i = 0;i < MEDIA_IDX_VIDEO_NUMS;i++)
            {
                nres = section_get_mpeg2(&s,&pbc->mpeg2[i]);
                if(!is_vatek_success(nres))break;
            }

            if(is_vatek_success(nres))
            {
                for(i = 0;i < MEDIA_IDX_VIDEO_NUMS;i++)
                {
                    nres = section_get_h264(&s,&pbc->h264[i]);
                    if(!is_vatek_success(nres))break;
                }      
            }
        }
    }
    return nres;
}

void storage_transform_reset(Pstorage_transform ptr)
{
	modulator_param_reset(modulator_dvb_t, &ptr->modulator);
}

vatek_result storage_transform_set(Pstorage_transform ptr,uint8_t* psection)
{
    bufstream s = { 0,psection, };
	vatek_result nres = section_put_start(BIN_SECTION_TRANSFORM,&s,sizeof(storage_transform));
    if(is_vatek_success(nres))
    {
        nres = section_put_modulator(&s,&ptr->modulator);
        if(is_vatek_success(nres))
            nres = section_put_end(&s);  
    }
    return nres;
}

vatek_result storage_transform_get(Pstorage_transform ptr,uint8_t* psection)
{
    bufstream s = { 0,psection, };
    vatek_result nres = section_get_start(&s,BIN_SECTION_TRANSFORM,NULL);
    if(is_vatek_success(nres))
    {
        nres = section_get_modulator(&s,&ptr->modulator);
    }
    return nres;
}

void storage_r2tune_reset(Pr2_tune_handle pr2)
{
	rfmixer_r2_reset_tune_table(pr2);
}

vatek_result storage_r2tune_set(Pr2_tune_handle pr2,uint8_t* psection)
{
	bufstream s = { 0,psection, };
	vatek_result nres = section_put_start(BIN_SECTION_R2TUNE,&s,sizeof(r2_tune_handle));

	if (is_vatek_success(nres))
	{
		uint32_t i = 0;

		putbuf_uint32_t(&s, pr2->tag);
		putbuf_uint32_t(&s, pr2->items_num);
		putbuf_uint32_t(&s, pr2->r2_flags_0);
		putbuf_uint32_t(&s, pr2->r2_flags_1);

		for (i = 0; i < pr2->items_num; i++)
		{
			Pr2_tune_item prule = &pr2->rules[i];
			uint32_t val = 0;
			putbuf_uint32_t(&s, prule->tag);
			putbuf_uint32_t(&s, prule->freq);

			val = (prule->rule[0].tune.ioffset << 16) | (prule->rule[0].tune.qoffset);
			putbuf_uint32_t(&s,val);
			val = (prule->rule[0].tune.imgoffset << 16) | (prule->rule[0].tune.phaseoffset);
			putbuf_uint32_t(&s,val);
			val = (prule->rule[0].pagain << 16) | (prule->rule[0].gpiocntl);
			putbuf_uint32_t(&s, val);

			val = (prule->rule[1].tune.ioffset << 16) | (prule->rule[1].tune.qoffset);
			putbuf_uint32_t(&s,val);
			val = (prule->rule[1].tune.imgoffset << 16) | (prule->rule[1].tune.phaseoffset);
			putbuf_uint32_t(&s,val);
			val = (prule->rule[1].pagain << 16) | (prule->rule[1].gpiocntl);
			putbuf_uint32_t(&s, val);
		}
		nres = section_put_end(&s);
	}
	return nres;
}

vatek_result storage_r2tune_get(Pr2_tune_handle pr2,uint8_t* psection)
{
	bufstream s = { 0,psection, };
	vatek_result nres = section_get_start(&s,BIN_SECTION_R2TUNE, NULL);
	if (is_vatek_success(nres))
	{
		uint32_t i = 0;
        memset(pr2,0,sizeof(r2_tune_handle));
		pr2->tag = getbuf_uint32_t(&s);
		if (pr2->tag == R2_TUNE_HDR_TAG)
		{
			nres = vatek_badparam;
			pr2->items_num = getbuf_uint32_t(&s);
			pr2->r2_flags_0 = getbuf_uint32_t(&s);
			pr2->r2_flags_1 = getbuf_uint32_t(&s);
			nres = vatek_success;
			for (i = 0; i < pr2->items_num; i++)
			{
				Pr2_tune_item prule = &pr2->rules[i];
				prule->tag = getbuf_uint32_t(&s);
				if (prule->tag != R2_TUNE_ITEM_TAG)nres = vatek_badparam;
				else
				{
					uint32_t val = 0;
					prule->freq = getbuf_uint32_t(&s);
					val = getbuf_uint32_t(&s);
					prule->rule[0].tune.ioffset = (uint16_t)(val >> 16);
					prule->rule[0].tune.qoffset = (uint16_t)(val);
					val = getbuf_uint32_t(&s);
					prule->rule[0].tune.imgoffset = (uint16_t)(val >> 16);
					prule->rule[0].tune.phaseoffset = (uint16_t)(val);
					val = getbuf_uint32_t(&s);
					prule->rule[0].pagain = val >> 16;
					prule->rule[0].gpiocntl = val & 0xFFFF;

					val = getbuf_uint32_t(&s);
					prule->rule[1].tune.ioffset = (uint16_t)(val >> 16);
					prule->rule[1].tune.qoffset = (uint16_t)(val);
					val = getbuf_uint32_t(&s);
					prule->rule[1].tune.imgoffset = (uint16_t)(val >> 16);
					prule->rule[1].tune.phaseoffset = (uint16_t)(val);
					val = getbuf_uint32_t(&s);
					prule->rule[1].pagain = val >> 16;
					prule->rule[1].gpiocntl = val & 0xFFFF;
				}
				if (!is_vatek_success(nres))break;
			}
		}
	}
	return nres;
}

void storage_chip_config_reset(Pstorage_chip_config pcfg)
{
	const char* filename = "modulation.mudul";
	const char* str = &default_chip_config;
	char buffer[sizeof(storage_chip_config)];

	FILE* output_file = fopen(filename, "rb+");
	if (!output_file) {
		memcpy(pcfg, str, sizeof(storage_chip_config));
	}
	else {
		fread(buffer, sizeof(storage_chip_config), 1, output_file);
		memcpy(pcfg, buffer, sizeof(storage_chip_config));
		fclose(output_file);
	}
} 

vatek_result storage_chip_config_set(Pstorage_chip_config pcfg, uint8_t* psection)
{
	bufstream s = { 0,psection};
	vatek_result nres = section_put_start(BIN_SECTION_CONFIG, &s, sizeof(storage_chip_config));
	if (is_vatek_success(nres))
	{
		nres = section_put_chip_config(&s, &pcfg->_chip.config);
		if (is_vatek_success(nres))
		{
			putbuf_buf(&s, (uint8_t*)&pcfg->srrc[0][0], CONFIG_MOD_SRRC_LEN * CONFIG_MODSRRC_NUMS);
			nres = section_put_modulation_config(&s, &pcfg->modulation[CONFIG_CHIPIDX_B2]);
			if (is_vatek_success(nres))
				nres = section_put_modulation_config(&s, &pcfg->modulation[CONFIG_CHIPIDX_B2_plus]);
			if(is_vatek_success(nres))
				nres = section_put_modulation_config(&s, &pcfg->modulation[CONFIG_CHIPIDX_B3]);
			if(is_vatek_success(nres))
				nres = section_put_modulation_config(&s, &pcfg->modulation[CONFIG_CHIPIDX_A3]);
		}

		if (is_vatek_success(nres))
			nres = section_put_end(&s);
	}
	return nres;
}

vatek_result storage_chip_config_get(Pstorage_chip_config pcfg, uint8_t* psection)
{
	bufstream s = { 0,psection, };
	vatek_result nres = section_get_start(&s,BIN_SECTION_CONFIG,NULL);
	if (is_vatek_success(nres))
	{
		nres = section_get_chip_config(&s, &pcfg->_chip.config);
		if (is_vatek_success(nres))
		{
			getbuf_buf(&s, (uint8_t*)&pcfg->srrc[0][0], CONFIG_MOD_SRRC_LEN * CONFIG_MODSRRC_NUMS);
			nres = section_get_modulation_config(&s, &pcfg->modulation[CONFIG_CHIPIDX_B2]);
			if (is_vatek_success(nres))
				nres = section_get_modulation_config(&s, &pcfg->modulation[CONFIG_CHIPIDX_B2_plus]);
			if (is_vatek_success(nres))
				nres = section_get_modulation_config(&s, &pcfg->modulation[CONFIG_CHIPIDX_B3]);
			if (is_vatek_success(nres))
				nres = section_get_modulation_config(&s, &pcfg->modulation[CONFIG_CHIPIDX_A3]);
		}
	}
	return nres;
}

vatek_result section_put_start(uint32_t tag, Pbufstream pstream, int32_t len)
{
    int32_t sectionlen = BINARY_SECTION_LEN(len);
    memset(pstream->buf,0xFFFFFFFF,sizeof(BINARY_SECTION_SIZE));
	putbuf_uint32_t(pstream, tag);
	putbuf_uint32_t(pstream, (uint32_t)sectionlen);
	putbuf_uint32_t(pstream, 0);
    return vatek_success;
}

vatek_result section_get_start(Pbufstream pstream,uint32_t tag,uint32_t* len)
{
	vatek_result nres = vatek_badparam;
	uint32_t val = getbuf_uint32_t(pstream);
	if (val == tag)
	{
		uint32_t scrc = 0;
        if(len)*len = getbuf_uint32_t(pstream);
		else val = getbuf_uint32_t(pstream);
		val = getbuf_uint32_t(pstream);
		pstream->pos = 8;
		putbuf_uint32_t(pstream, 0);
		scrc = tool_crc32(pstream->buf, BINARY_SECTION_SIZE);
		if (scrc != val)
		{
			VWAR("bad section crc fail : [%08x:%08x:%08x]",tag,val,scrc);
			nres = vatek_crcfail;
		}
		else nres = vatek_success;
	}else VWAR("bad section tag unmatch : [%08x:%08x]",tag,val);
	return nres;
}

vatek_result section_put_end(Pbufstream pstream)
{
	pstream->pos = 8;
	putbuf_uint32_t(pstream, tool_crc32(pstream->buf, BINARY_SECTION_SIZE));
	return vatek_success;
}

vatek_result section_put_ui_props(Pbufstream pstream,const Pui_prop_item puiprops,uint8_t* praw)
{
    int32_t i = 0;
    vatek_result nres = vatek_success;
	while (puiprops[i].type != prop_type_null)
	{
		uint32_t val = 0;
		nres = ui_props_get_uint32(&puiprops[i], praw, &val);
		if (!is_vatek_success(nres))break;
		putbuf_uint32_t(pstream, val);
		i++;
	}
    return nres;
}

vatek_result section_get_ui_props(Pbufstream pstream,const Pui_prop_item puiprops,uint8_t* praw)
{
    int32_t i = 0;
    vatek_result nres = vatek_success;
	while (puiprops[i].type != prop_type_null)
	{
		uint32_t val = getbuf_uint32_t(pstream);
		nres = ui_props_set_uint32(&puiprops[i], praw, val);
		if (!is_vatek_success(nres))break;
		i++;
	}
    return nres;
}

vatek_result section_put_modulator(Pbufstream pstream,Pmodulator_param pmod)
{
    vatek_result nres = section_put_ui_props(pstream,_ui_struct(modulator_param),(uint8_t*)pmod);
    if(is_vatek_success(nres))
    {
        nres = section_put_ui_props(pstream,modulator_param_get_ui_props(pmod->type),(uint8_t*)&pmod->mod.raw_byte);
    }
    return nres;
}

vatek_result section_get_modulator(Pbufstream pstream,Pmodulator_param pmod)
{
    vatek_result nres = section_get_ui_props(pstream,_ui_struct(modulator_param),(uint8_t*)pmod);
    if(is_vatek_success(nres))
    {
        nres = section_get_ui_props(pstream, modulator_param_get_ui_props(pmod->type),(uint8_t*)&pmod->mod.raw_byte);
    }
    return nres;
}

vatek_result section_put_mpeg2(Pbufstream pstream,Pmpeg2_param pmpeg2)
{
    uint8_t* pbufmatrix = (uint8_t*)&pmpeg2->intra_quant_matrix[0][0];
    vatek_result nres = section_put_ui_props(pstream,_ui_struct(mpeg2_param),(uint8_t*)pmpeg2);
    if(is_vatek_success(nres))
        putbuf_buf(pstream,pbufmatrix, MPEG2_QUANT_LEN);
	return nres;
}

vatek_result section_get_mpeg2(Pbufstream pstream,Pmpeg2_param pmpeg2)
{
    uint8_t* pbufmatrix = (uint8_t*)&pmpeg2->intra_quant_matrix[0][0];
    vatek_result nres = section_get_ui_props(pstream,_ui_struct(mpeg2_param),(uint8_t*)pmpeg2);
    if(is_vatek_success(nres))
        getbuf_buf(pstream,pbufmatrix, MPEG2_QUANT_LEN);
	return nres;
}

vatek_result section_put_h264(Pbufstream pstream,Ph264_param ph264)
{
    uint8_t* pbufmatrix = (uint8_t*)&ph264->intra_4x4;
    vatek_result nres = section_put_ui_props(pstream,_ui_struct(h264_param),(uint8_t*)ph264);
    if(is_vatek_success(nres))
        putbuf_buf(pstream,pbufmatrix, H264_QUANT_LEN);
	return nres;
}

vatek_result section_get_h264(Pbufstream pstream,Ph264_param ph264)
{
    uint8_t* pbufmatrix = (uint8_t*)&ph264->intra_4x4;
    vatek_result nres = section_get_ui_props(pstream,_ui_struct(h264_param),(uint8_t*)ph264);
    if(is_vatek_success(nres))
        getbuf_buf(pstream,pbufmatrix, H264_QUANT_LEN);
	return nres;
}

vatek_result section_put_chip_config(Pbufstream pstream,Pchip_config pcfg)
{
	int32_t flen = CONFIG_CHIP_BASE_LEN - sizeof(chip_config);
	vatek_result nres = vatek_success;
	if (flen < 0 || ((flen % 4) != 0) || pcfg->tag != CHIP_CFGV1_TAG)nres = vatek_badparam;
	else
	{
		flen >>= 2;
		putbuf_uint32_t(pstream, pcfg->tag);
		putbuf_uint32_t(pstream, pcfg->functions);
		putbuf_uint32_t(pstream, pcfg->status_led);
		putbuf_uint32_t(pstream, pcfg->usb_vid);
		putbuf_uint32_t(pstream, pcfg->usb_pid);
		putbuf_buf(pstream, (uint8_t*)&pcfg->usb_str[0], CHIP_USBSTR_LEN);
		putbuf_uint32_t(pstream, pcfg->vendor_functions);

		while (flen)
		{
			putbuf_uint32_t(pstream, 0xFFFFFFFF);
			flen--;
		}
	}
	return nres;
}

vatek_result section_get_chip_config(Pbufstream pstream,Pchip_config pcfg)
{
	vatek_result nres = vatek_badparam;
	int32_t stpos = pstream->pos;
	memset(pcfg, 0, sizeof(chip_config));
	pcfg->tag = getbuf_uint32_t(pstream);
	if (pcfg->tag == CHIP_CFGV1_TAG)
	{
		pcfg->functions = getbuf_uint32_t(pstream);
		pcfg->status_led = getbuf_uint32_t(pstream);
		pcfg->usb_vid = getbuf_uint32_t(pstream);
		pcfg->usb_pid = getbuf_uint32_t(pstream);
		getbuf_buf(pstream, (uint8_t*)&pcfg->usb_str[0], CHIP_USBSTR_LEN);
		pcfg->vendor_functions = getbuf_uint32_t(pstream);
		pstream->pos = CONFIG_CHIP_BASE_LEN + stpos;
		nres = vatek_success;
	}
	return nres;
}

vatek_result section_put_modulation_config(Pbufstream pstream, Pmodulation_config pmod)
{
	int32_t len = sizeof(modulation_config);
	vatek_result nres = vatek_badparam;
	if (len % 4 == 0)
	{
		uint32_t* pwptr = (uint32_t*)pmod;
		len >>= 2;

		while (len)
		{
			putbuf_uint32_t(pstream, *pwptr);
			pwptr++;
			len--;
		}
		nres = vatek_success;
	}
	return nres;
}

vatek_result section_get_modulation_config(Pbufstream pstream, Pmodulation_config pmod)
{
	int32_t len = sizeof(modulation_config);
	vatek_result nres = vatek_badparam;
	if (len % 4 == 0)
	{
		uint32_t* pwptr = (uint32_t*)pmod;
		len >>= 2;

		while (len)
		{
			*pwptr = getbuf_uint32_t(pstream);
			pwptr++;
			len--;
		}
		nres = vatek_success;
	}
	return nres;
}
