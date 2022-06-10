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

#include <mux/mux_rule.h>
#include <core/base/binary_rom.h>
#include <core/tools/tool_crc32.h>
#include <core/tools/tool_bufstream.h>
#include <stdio.h>

#define DEFAULT_RULE_BLOCK_SIZE		16*1024
#define RULE_CHPROP_OFFSET			0x100
#define RULE_PROGPROP_OFFSET		32

extern vatek_result rule_config_header(Pmux_rule_block pblock, uint8_t* psection);

vatek_result muxrule_load_raw(Pth_mempool pmempool, uint8_t* praw, int32_t len, Pmux_rule_block* prule)
{
	vatek_result nres = muxrule_check_valid(praw);
	if (is_vatek_success(nres))
	{
		Pmux_rule_block newblock = (Pmux_rule_block)th_mempool_malloc(pmempool, sizeof(mux_rule_block));
		vatek_result nres = vatek_memfail;
		if (newblock)
		{
			memset(newblock, 0, sizeof(mux_rule_block));
			newblock->block_len = len;
			newblock->rawblock = praw;

			nres = rule_config_header(newblock, newblock->rawblock);
			if (is_vatek_success(nres))*prule = newblock;
		}
	}
	return nres;
}

vatek_result muxrule_load_file(Pth_mempool pmempool, const char* filename, Pmux_rule_block* prule)
{
	#ifdef __OR1K__
		vatek_result nres = vatek_unsupport;
	#else
		vatek_result nres = vatek_memfail;
		FILE* frule = fopen(filename, "rb");
		if (frule != NULL)
		{
			int32_t len = sizeof(mux_rule_block) + BINARY_SECTION_SIZE;
			uint8_t* praw = th_mempool_malloc(pmempool, len);
			if (praw)
			{
				Pmux_rule_block pblock = (Pmux_rule_block)&praw[0];
				pblock->rawblock = &praw[sizeof(mux_rule_block)];
				if (fread(pblock->rawblock, BINARY_SECTION_SIZE, 1, frule) > 0)
				{
					nres = muxrule_check_valid(&pblock->rawblock[0]);
					if(is_vatek_success(nres))
						nres = rule_config_header(pblock, pblock->rawblock);
					if (is_vatek_success(nres))*prule = pblock;
				}
			}
		}
	#endif
	return nres;
}

vatek_result muxrule_check_valid(uint8_t* praw)
{
	bufstream bs = { 0,praw, };
	vatek_result nres = vatek_success;
	if (strcmp((char*)getbuf_ptr(&bs), MUX_RULE_HDR_TAG) != 0)
	{
		if (strcmp((char*)getbuf_ptr(&bs), MUX_RULE_HDR_TAG_V1) != 0)
			nres = vatek_format;
	}
	setbuf_offset(&bs, 8);

	if (is_vatek_success(nres))
	{
		uint8_t* ptr;
		uint32_t rulelen = getbuf_uint32_t(&bs);
		uint32_t rulecrc = getbuf_uint32_t(&bs);

		ptr = setbuf_offset(&bs, MUX_RULE_HDR_LEN);
		if (tool_crc32(ptr, rulelen) != rulecrc)nres = vatek_crcfail;
		else nres = (vatek_result)rulelen;
	}
	return nres;
}

mux_spec_mode muxrule_get_spec(Pmux_rule_block prule)
{
	return prule->spec;
}

mux_country_code muxrule_get_country(Pmux_rule_block prule)
{
	return prule->country;
}

vatek_result muxrule_get_channel_rawbuf(Pspec_channel pch, uint8_t* psur)
{
	Pui_prop_item puiprops = NULL;
	vatek_result nres = muxspec_get_channel_uiprops(pch->spec, &puiprops);
	if (is_vatek_success(nres))
	{
		nres = muxrule_get_uiprops(puiprops, psur, (uint8_t*)&pch->_spec);
	}
		
	return nres;
}

vatek_result muxrule_get_program_rawbuf(Pspec_program pprog,uint8_t* psur)
{
	Pui_prop_item puiprops = NULL;
	vatek_result nres = muxspec_get_program_uiprops(pprog->spec, &puiprops);
	if (is_vatek_success(nres))
		nres = muxrule_get_uiprops(puiprops, psur, (uint8_t*)&pprog->_spec);
	return nres;
}

vatek_result muxrule_set_channel_rawbuf(Pspec_channel pch, uint8_t* pdest)
{
	Pui_prop_item puiprops = NULL;
	vatek_result nres = muxspec_get_channel_uiprops(pch->spec, &puiprops);
	if (is_vatek_success(nres))
	{
		nres = muxrule_set_uiprops(puiprops, pdest, (uint8_t*)&pch->_spec);
	}
		
	return nres;
}

vatek_result muxrule_set_program_rawbuf(Pspec_program pprog, uint8_t* pdest)
{
	Pui_prop_item puiprops = NULL;
	vatek_result nres = muxspec_get_program_uiprops(pprog->spec, &puiprops);
	if (is_vatek_success(nres))
		nres = muxrule_set_uiprops(puiprops, pdest, (uint8_t*)&pprog->_spec);
	return nres;
}

vatek_result muxrule_get_channel_param(Pmux_rule_block prule, Pspec_channel pch)
{
	vatek_result nres = vatek_badparam;
	if (prule->spec == pch->spec)
	{
		uint8_t* ptrprop = &prule->rawblock[prule->channel_pos + RULE_CHPROP_OFFSET];
		Pui_prop_item puiprops = NULL;
		nres = muxspec_get_channel_uiprops(prule->spec, &puiprops);
		if (is_vatek_success(nres))
			nres = muxrule_get_uiprops(puiprops, ptrprop, (uint8_t*)&pch->_spec);
	}
	return nres;
}

vatek_result muxrule_get_program_param(Pmux_rule_block prule, Pspec_program prog)
{
	vatek_result nres = vatek_badparam;
	if (prule->spec == prog->spec)
	{
		uint8_t* ptrprop = &prule->rawblock[prule->program_pos + RULE_PROGPROP_OFFSET];
		Pui_prop_item puiprops = NULL;
		nres = muxspec_get_program_uiprops(prule->spec, &puiprops);
		if (is_vatek_success(nres))
			nres = muxrule_get_uiprops(puiprops, ptrprop, (uint8_t*)&prog->_spec);
	}
	return nres;
}

vatek_result muxrule_set_channel_param(Pmux_rule_block prule, Pspec_channel pch)
{
	vatek_result nres = vatek_badparam;
	if (prule->spec == pch->spec)
	{
		uint8_t* ptrprop = &prule->rawblock[prule->channel_pos + RULE_CHPROP_OFFSET];
		Pui_prop_item puiprops = NULL;
		nres = muxspec_get_channel_uiprops(prule->spec, &puiprops);
		if (is_vatek_success(nres))
			nres = muxrule_set_uiprops(puiprops, ptrprop, (uint8_t*)&pch->_spec);
	}
	return nres;
}

vatek_result muxrule_set_program_param(Pmux_rule_block prule, Pspec_program prog)
{
	vatek_result nres = vatek_badparam;
	if (prule->spec == prog->spec)
	{
		uint8_t* ptrprop = &prule->rawblock[prule->program_pos + RULE_PROGPROP_OFFSET];
		Pui_prop_item puiprops = NULL;
		nres = muxspec_get_program_uiprops(prule->spec, &puiprops);
		if (is_vatek_success(nres))
			nres = muxrule_set_uiprops(puiprops, ptrprop, (uint8_t*)&prog->_spec);
	}
	return nres;
}

vatek_result muxrule_copy_channel(Pmux_rule_block prule, uint8_t* pbuf, int32_t len)
{
	int32_t maxlen = (prule->channel_len - RULE_CHPROP_OFFSET);
	vatek_result nres = vatek_badparam;

	if (pbuf)
	{
		if (maxlen == len)
		{
			uint8_t* ptr = &prule->rawblock[prule->channel_pos + RULE_CHPROP_OFFSET];
			memcpy(pbuf, ptr, len);
			nres = vatek_success;
		}
	}
	else nres = (vatek_result)maxlen;
	return nres;
}

vatek_result muxrule_copy_program(Pmux_rule_block prule, uint8_t* pbuf, int32_t len)
{
	int32_t maxlen = (prule->program_len - RULE_PROGPROP_OFFSET);
	vatek_result nres = vatek_badparam;
	if (pbuf)
	{
		if (maxlen == len)
		{
			uint8_t* ptr = &prule->rawblock[prule->program_pos + RULE_PROGPROP_OFFSET];
			memcpy(pbuf, ptr, len);
			nres = vatek_success;
		}
	}
	else nres = (vatek_result)maxlen;
	return nres;
}

uint32_t section_get_uint(uint8_t* psection, int32_t offset)
{
	uint32_t val = psection[offset++] << 24;
	val |= psection[offset++] << 16;
	val |= psection[offset++] << 8;
	val |= psection[offset++];
	return val;
}

vatek_result rule_config_header(Pmux_rule_block pblock, uint8_t* psection)
{
	pblock->channel_pos = section_get_uint(psection, 16);
	pblock->channel_len = section_get_uint(psection, 20);
	pblock->program_pos = section_get_uint(psection, 24);
	pblock->program_len = section_get_uint(psection, 28);
	pblock->rule_runction = section_get_uint(psection, 32);
	pblock->spec = section_get_uint(psection, 36);
	pblock->country = section_get_uint(psection, 40);
	pblock->block_len = pblock->program_pos + pblock->program_len;
	return vatek_success;
}

vatek_result muxrule_malloc_uiprops(Pth_mempool pmempool, Pmux_ui_props puiprops, uint8_t** pbufprop)
{
	uint8_t* pnewbuf = th_mempool_malloc(pmempool, puiprops->size);
	vatek_result nres = vatek_memfail;
	if (pnewbuf)
	{
		int32_t i = 0;
		nres = vatek_success;
		memcpy(pnewbuf, puiprops->def_val, puiprops->size);
		while (puiprops->props[i].name)
		{
			uint8_t* pdefptr = &puiprops->def_val[puiprops->props[i].offset];
			uint8_t* pdestptr = &pnewbuf[puiprops->props[i].offset];
			if (IS_UIPROP_STRBUF(puiprops->props[i].type))
			{
				Pmux_string pstrdef = *(Pmux_string*)pdefptr;
				Pmux_string pstrdest = *(Pmux_string*)pdestptr;

				pstrdest->text = th_mempool_malloc(pmempool, pstrdest->maxlen);
				nres = vatek_memfail;
				if (pstrdest->text)
				{
					memcpy(&pstrdest->text[0], &pstrdef->text[0], pstrdef->maxlen);
					nres = vatek_success;
				}
			}
			if (!is_vatek_success(nres))break;
			i++;
		}
		if (is_vatek_success(nres))*pbufprop = pnewbuf;
	}
	return nres;
}

extern uint32_t raw_getuint(uint8_t** ptr);
extern void raw_getbuf(uint8_t** ptr, uint8_t* pdest, int32_t len);
extern void raw_getstr(uint8_t** ptr, uint8_t* pdest, int32_t len, int32_t maxlen);

vatek_result muxrule_get_uiprops(Pui_prop_item puiprops, uint8_t* prulebuf, uint8_t* pdest)
{
	int32_t i = 0;
	uint8_t* ptrprop = prulebuf;
	vatek_result nres = vatek_success;

	while (puiprops[i].name)
	{
		uint8_t* pval = &pdest[puiprops[i].offset];
		if (IS_UIPROP_STRBUF(puiprops[i].type))
		{
			Pmux_string pstring = *(Pmux_string*)pval;
			memset(&pstring->text[0], 0, pstring->maxlen);
			uint32_t len = raw_getuint(&ptrprop);
			if (len > pstring->maxlen)nres = vatek_format;
			else raw_getstr(&ptrprop, pstring->text, len, pstring->maxlen);
		}
		else if (IS_UIPROP_LANG(puiprops[i].type))
		{
			raw_getbuf(&ptrprop, pval, 4);
		}
		else
		{
			uint32_t val = raw_getuint(&ptrprop);
			int32_t len = puiprops[i].type & 0xF;
			if (len == 1)*((uint8_t*)pval) = (uint8_t)val;
			else if (len == 2)*((uint16_t*)pval) = (uint16_t)val;
			else if (len == 4)*((uint32_t*)pval) = (uint32_t)val;
			else nres = vatek_format;
		}
		if (!is_vatek_success(nres))break;
		i++;
	}
	return nres;
}

extern void raw_setuint(uint8_t** ptr, uint32_t val);
extern void raw_setbuf(uint8_t** ptr, uint8_t* psur, int32_t len);
extern void raw_setstr(uint8_t** ptr, uint8_t* psur, int32_t len, int32_t maxlen);

vatek_result muxrule_set_uiprops(Pui_prop_item puiprops, uint8_t* prulebuf, uint8_t* psur)
{
	int32_t i = 0;
	uint8_t* ptrprop = prulebuf;
	vatek_result nres = vatek_success;

	while (puiprops[i].name)
	{
		uint8_t* pval = &psur[puiprops[i].offset];
		if (IS_UIPROP_STRBUF(puiprops[i].type))
		{
			Pmux_string pstring = *(Pmux_string*)pval;
			raw_setuint(&ptrprop, pstring->len);
			raw_setstr(&ptrprop, pstring->text, pstring->len, pstring->maxlen);
		}
		else if (IS_UIPROP_LANG(puiprops[i].type))raw_setbuf(&ptrprop, pval, 4);
		else
		{
			int32_t len = puiprops[i].type & 0xF;
			if (len == 1)raw_setuint(&ptrprop, *(uint8_t*)pval);
			else if (len == 2)raw_setuint(&ptrprop, *(uint16_t*)pval);
			else if (len == 4)raw_setuint(&ptrprop, *(uint32_t*)pval);
			else nres = vatek_format;
		}
		if (!is_vatek_success(nres))break;
		i++;
	}
	return nres;
}

uint32_t raw_getuint(uint8_t** ptr)
{
	uint8_t* pval = *ptr;
	uint32_t val = (pval[0] << 24) | (pval[1] << 16) | (pval[2] << 8) | pval[3];
	*ptr += 4;
	return val;
}

void raw_getbuf(uint8_t** ptr, uint8_t* pdest, int32_t len)
{
	memcpy(pdest, *ptr, len);
	len = _V_ALIGN_WORD(len);
	*ptr += len;
}

void raw_getstr(uint8_t** ptr, uint8_t* pdest, int32_t len, int32_t maxlen)
{
	memcpy(pdest, *ptr, len);
	maxlen = _V_ALIGN_WORD(maxlen);
	*ptr += maxlen;
}

void raw_setuint(uint8_t** ptr, uint32_t val)
{
	uint8_t* pdest = *ptr;
	pdest[0] = (uint8_t)(val >> 24);
	pdest[1] = (uint8_t)(val >> 16);
	pdest[2] = (uint8_t)(val >> 8);
	pdest[3] = (uint8_t)(val);
	*ptr += 4;
}

void raw_setbuf(uint8_t** ptr, uint8_t* psur, int32_t len)
{
	memcpy(*ptr, psur, len);
	len = _V_ALIGN_WORD(len);
	*ptr += len;
}

void raw_setstr(uint8_t** ptr, uint8_t* psur, int32_t len, int32_t maxlen)
{
	memcpy(*ptr, psur, len);
	maxlen = _V_ALIGN_WORD(maxlen);
	*ptr += maxlen;
}
