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

#include <mux/mux_spec.h>
#include <mux/ui/spec/ui_mux_spec_arib.h>
#include <mux/ui/spec/ui_mux_spec_dvb.h>
#include <mux/ui/spec/ui_mux_spec_iso.h>
#include <mux/ui/spec/ui_mux_spec_psip.h>

typedef struct _muxspec_props
{
	mux_spec_mode mode;
	Pmux_ui_string strings;
	Pui_prop_item chprops;
	Pui_prop_item progprops;
	uint8_t* chbuf;
	int32_t chlen;
	uint8_t* progbuf;
	int32_t proglen;
}muxspec_props, *Pmuxspec_props;

#define _defch(spec)	default_##spec##_channel
#define _defprog(spec)	default_##spec##_program

#define _defspec(spec)	{spec, \
						 _mux_get_string(spec), \
						 _mux_ui_get_struct(spec##_channel),\
						 _mux_ui_get_struct(spec##_program),\
						 (uint8_t*)&_defch(spec),\
						 sizeof(_defch(spec)),\
						 (uint8_t*)&_defprog(spec),\
						 sizeof(_defprog(spec)),\
						}

static const muxspec_props defspec_props[] =
{
	_defspec(mux_spec_iso),
	_defspec(mux_spec_dvb),
	_defspec(mux_spec_arib),
	_defspec(mux_spec_abnt),
	_defspec(mux_spec_psip),
};

#define MUXSPEC_PROPS_NUMS	(sizeof(defspec_props)/sizeof(muxspec_props))

extern Pmuxspec_props muxspec_get_props(mux_spec_mode spec);
extern vatek_result muxspec_copy_props(Pth_mempool pmem,
									   Pui_prop_item props, 
									   uint8_t* pdest, 
									   uint8_t* psource, 
									   int32_t len);

extern void muxspec_printf_uiprops(mux_spec_mode spec,const char* fmt,Pui_prop_item props, uint8_t* pbuf);

vatek_result muxspec_check_support(mux_spec_mode spec, mux_country_code country)
{
	Pmuxspec_props pprops = muxspec_get_props(spec);
	if (pprops)return vatek_success;
	return vatek_unsupport;
}

vatek_result muxspec_get_channel_uiprops(mux_spec_mode spec, Pui_prop_item* pprops)
{
	vatek_result nres = vatek_unsupport;
	Pmuxspec_props pmprops = muxspec_get_props(spec);
	if (pmprops)
	{
		*pprops = pmprops->chprops;
		nres = vatek_success;
	}
	return nres;
}

vatek_result muxspec_get_program_uiprops(mux_spec_mode spec, Pui_prop_item* pprops)
{
	vatek_result nres = vatek_unsupport;
	Pmuxspec_props pmprops = muxspec_get_props(spec);
	if (pmprops)
	{
		*pprops = pmprops->progprops;
		nres = vatek_success;
	}
	return nres;
}

vatek_result muxspec_get_spec_string(mux_spec_mode spec, Pui_prop_item puiprop, Pmux_ui_string* pstrings)
{
	Pmuxspec_props puispec = muxspec_get_props(spec);
	vatek_result nres = vatek_unsupport;
	if (puispec)
	{
		int32_t idx = puiprop->type & 0xFF;
		Pmux_ui_string pmstring = &puispec->strings[idx];
		nres = vatek_success;
		if (strcmp(pmstring->name, puiprop->name) == 0)
			*pstrings = pmstring;
		else nres = vatek_unsupport;
	}

	if(!is_vatek_success(nres))
		VWAR("spec string not found : [%s:%s]",ui_enum_get_str(mux_spec_mode,spec),puiprop->name);
	return nres;
}

vatek_result muxspec_set_string_ascii(Pmux_string pstring, const char* szeng)
{
	vatek_result nres = vatek_badparam;
	int32_t slen = (int32_t)strlen(szeng);
	if (slen)
	{
		memset(&pstring->text[0], 0, pstring->maxlen);
		if (slen > (int32_t)pstring->maxlen)slen = pstring->maxlen;
		strncpy((char*)&pstring->text[0], szeng, slen);
		pstring->len = slen;
		nres = vatek_success;
	}
	return nres;
}

vatek_result muxspec_set_string_jiseng(Pmux_string pstring, const char* szeng)
{
	vatek_result nres = vatek_badparam;
	int32_t slen = (int32_t)strlen(szeng);
	if (slen)
	{
		int32_t maxlen = pstring->maxlen - 2;
		uint8_t* ptrs = &pstring->text[2];
		pstring->text[0] = 0x1B;
		pstring->text[1] = 0x7E;
		if (slen > maxlen)slen = maxlen;

		for (maxlen = 0; maxlen < slen; maxlen++)
		{
			*ptrs = _AC(szeng[maxlen]);
			ptrs++;
		}
		pstring->len = slen + 2;
		nres = vatek_success;
	}
	return nres;
}

vatek_result muxspec_set_string_utf16(Pmux_string pstring, const char* szeng)
{
	vatek_result nres = vatek_badparam;
	int32_t slen = (int32_t)strlen(szeng);
	if (slen)
	{
		int32_t ulen = pstring->maxlen >> 1;
		uint8_t* ptrs = &pstring->text[1];
		if (slen > ulen)slen = ulen;
		for (ulen = 0; ulen < slen; ulen++)
		{
			*ptrs = szeng[ulen];
			ptrs += 2;
		}
		pstring->len = slen << 1;
		nres = vatek_success;
	}
	return nres;
}

vatek_result muxspec_set_string_psip_mutil(Pmux_string pstring, const char* szeng)
{
	vatek_result nres = vatek_badparam;
	int32_t slen = (int32_t)strlen(szeng);
	if (slen)
	{
		const uint8_t psip_end[] = { 0x01, 'e', 'n', 'g', 0x01, 0x00, 0x00, 0x04, };
		int32_t alen = pstring->maxlen - 8;
		if (slen > alen)slen = alen;

		memcpy(&pstring->text[0], &psip_end[0], 7);
		pstring->text[7] = slen;
		strncpy((char*)&pstring->text[8], szeng, slen);
		pstring->len = slen + 8;
		nres = vatek_success;
	}
	return nres;
}

vatek_result muxspec_set_spec_string(Pmux_ui_string pmuxstr, Pmux_string pstring, const char* szval)
{
	vatek_result nres = vatek_unsupport;
	if (pmuxstr->coding == _MUX_STRING_TYPE_ASCII)
		nres = muxspec_set_string_ascii(pstring, szval);
	else if (pmuxstr->coding == _MUX_STRING_TYPE_JIS_ENG)
		nres = muxspec_set_string_jiseng(pstring, szval);
	else if (pmuxstr->coding == _MUX_STRING_UTF16)
		nres = muxspec_set_string_utf16(pstring, szval);
	else if (pmuxstr->coding == _MUX_STRING_PSIP_MUTIL)
		nres = muxspec_set_string_psip_mutil(pstring, szval);
	return nres;
}

vatek_result muxspec_set_uiprops_spec_string(mux_spec_mode spec, Pui_prop_item props, const char* propname, uint8_t* praw, const char* value)
{
	int32_t i = 0;
	vatek_result nres = vatek_badparam;
	while (props[i].name)
	{
		if (IS_UIPROP_STRBUF(props[i].type))
		{
			if (strcmp(props[i].name, propname) == 0)
			{
				Pmux_string pstring = *(Pmux_string*)&praw[props[i].offset];
				Pmux_ui_string pmstr = NULL;
				nres = muxspec_get_spec_string(spec, &props[i], &pmstr);
				if (is_vatek_success(nres))nres = muxspec_set_spec_string(pmstr, pstring, value);
				break;
			}
		}
		i++;
	}
	return nres;
}

vatek_result muxspec_set_channel_spec_string(mux_spec_mode spec, const char* propname,uint8_t* praw, const char* value)
{
	Pui_prop_item puiprops = NULL;
	vatek_result nres = muxspec_get_channel_uiprops(spec, &puiprops);
	if (is_vatek_success(nres))nres = muxspec_set_uiprops_spec_string(spec, puiprops, propname, praw, value);
	return nres;
}

vatek_result muxspec_set_program_spec_string(mux_spec_mode spec, const char* propname, uint8_t* praw, const char* value)
{
	Pui_prop_item puiprops = NULL;
	vatek_result nres = muxspec_get_program_uiprops(spec, &puiprops);
	if (is_vatek_success(nres))
		nres = muxspec_set_uiprops_spec_string(spec, puiprops, propname, praw, value);
	return nres;
}

vatek_result muxspec_get_string_ascii(Pmux_string pstring, char* pbuf, int32_t len)
{
	memset(pbuf, 0, len);
	if (len > (int32_t)pstring->maxlen)len = pstring->maxlen;
	strncpy(pbuf, (char*)pstring->text, len);
	return vatek_success;
}

vatek_result muxspec_get_string_jiseng(Pmux_string pstring, char* pbuf, int32_t len)
{
	int32_t i = 2;
	vatek_result nres = vatek_format;
	if (pstring->text[0] == 0x1B || pstring->text[1] == 0x7E)
	{
		for (; i < (int32_t)pstring->len; i++)
		{
			if (!pstring->text[i])break;
			else pbuf[i - 2] = _AC_ASCII(pstring->text[i]);
		}
		nres = vatek_success;
	}
	return nres;
}

vatek_result muxspec_get_string_utf16(Pmux_string pstring, char* pbuf, int32_t len)
{
	int32_t i = 1;
	int32_t pos = 0;
	vatek_result nres = vatek_success;

	while (i < (int32_t)pstring->len)
	{
		if (!pstring->text[i])break;
		else pbuf[pos] = pstring->text[i];
		i += 2;
		pos++;
	}
	return nres;
}

vatek_result muxspec_get_string_psip_mutil(Pmux_string pstring, char* pbuf, int32_t len)
{
	vatek_result nres = vatek_format;
	if (pstring->text[0] == 0x01 && 
		pstring->text[1] == 'e' && 
		pstring->text[2] == 'n' && 
		pstring->text[3] == 'g')
	{
		int32_t i = 0;
		int32_t len = pstring->text[7];
		while (i < len)
		{
			pbuf[i] = pstring->text[8 + i];
			i++;
		}
		nres = vatek_success;
	}
	return nres;
}

vatek_result muxspec_get_spec_string_display(Pmux_ui_string puistring, Pmux_string pvstring, char* pbuf, int32_t len)
{
	vatek_result nres = vatek_success;
	if (puistring->coding == _MUX_STRING_TYPE_ASCII)
		nres = muxspec_get_string_ascii(pvstring, pbuf, len);
	else if (puistring->coding == _MUX_STRING_TYPE_JIS_ENG)
		nres = muxspec_get_string_jiseng(pvstring, pbuf, len);
	else if (puistring->coding == _MUX_STRING_UTF16)
		nres = muxspec_get_string_utf16(pvstring, pbuf, len);
	else if (puistring->coding == _MUX_STRING_PSIP_MUTIL)
		nres = muxspec_get_string_psip_mutil(pvstring, pbuf, len);
	else nres = vatek_unsupport;
	return nres;
}

extern vatek_result muxspec_get_props_len(mux_spec_mode spec,Pui_prop_item puiprops);

vatek_result muxspec_get_channel_len(mux_spec_mode spec)
{
	Pui_prop_item puiprops = NULL;
	vatek_result nres = muxspec_get_channel_uiprops(spec,&puiprops);
	if (is_vatek_success(nres))
		nres = muxspec_get_props_len(spec, puiprops);
	return nres;
}

vatek_result muxspec_get_program_len(mux_spec_mode spec)
{
	Pui_prop_item puiprops = NULL;
	vatek_result nres = muxspec_get_program_uiprops(spec, &puiprops);
	if (is_vatek_success(nres))
		nres = muxspec_get_props_len(spec, puiprops);
	return nres;
}

vatek_result muxspec_get_props_len(mux_spec_mode spec, Pui_prop_item puiprops)
{
	vatek_result nres = vatek_success;
	int32_t i = 0;
	int32_t len = 0;
	while (puiprops[i].name)
	{
		if (IS_UIPROP_STRBUF(puiprops[i].type))
		{
			Pmux_ui_string pstr = NULL;
			nres = muxspec_get_spec_string(spec, &puiprops[i], &pstr);
			if (is_vatek_success(nres))
				len += 4 + _V_ALIGN_WORD(pstr->maxlen);
		}
		else len += 4;
		if (!is_vatek_success(nres))break;
		i++;
	}
	if (is_vatek_success(nres))nres = (vatek_result)len;
	return nres;
}

Pmuxspec_props muxspec_get_props(mux_spec_mode spec)
{
	int32_t i = 0;
	if (mux_spec_is_dvb(spec))spec = mux_spec_dvb;
	for (i = 0; i < MUXSPEC_PROPS_NUMS; i++)
	{
		if (defspec_props[i].mode == spec)
			return (Pmuxspec_props)&defspec_props[i];
	}
	return NULL;
}

vatek_result muxspec_malloc_channel(Pth_mempool pmem,mux_spec_mode spec,mux_country_code country, Pspec_channel* pchannel)
{
	vatek_result nres = vatek_unsupport;
	Pmuxspec_props pmprops = muxspec_get_props(spec);
	if (pmprops)
	{
		Pspec_channel newch = (Pspec_channel)th_mempool_malloc(pmem,sizeof(spec_channel));
		nres = vatek_memfail;
		if (newch)
		{
			newch->spec = spec;
			newch->country = country;
			nres = muxspec_copy_props(pmem,pmprops->chprops, (uint8_t*)&newch->_spec, pmprops->chbuf, pmprops->chlen);
			if(is_vatek_success(nres))*pchannel = newch;
		}
	}
	return nres;
}

vatek_result muxspec_malloc_program(Pth_mempool pmem, mux_spec_mode spec, Pspec_program* pprogram)
{
	vatek_result nres = vatek_unsupport;
	Pmuxspec_props pmprops = muxspec_get_props(spec);
	if (pmprops)
	{
		Pspec_program newprog = (Pspec_program)th_mempool_malloc(pmem,sizeof(spec_program));
		nres = vatek_memfail;
		if (newprog)
		{
			newprog->spec = spec;
			nres = muxspec_copy_props(pmem,pmprops->progprops, (uint8_t*)&newprog->_spec, pmprops->progbuf, pmprops->proglen);
			if(is_vatek_success(nres))*pprogram = newprog;
		}
	}
	return nres;
}

vatek_result muxspec_copy_props(Pth_mempool pmem,Pui_prop_item props, uint8_t* pdest, uint8_t* psource, int32_t len)
{
	vatek_result nres = vatek_success;
	int32_t i = 0;
	memcpy(pdest, psource, len);

	while (props[i].name)
	{
		if (IS_UIPROP_STRBUF(props[i].type))
		{
			Pmux_string pstring = *(Pmux_string*)&psource[props[i].offset];
			uint8_t* newbuf = (uint8_t*)th_mempool_malloc(pmem,sizeof(mux_string) + pstring->maxlen);
			*((uint8_t**)&pdest[props[i].offset]) = NULL;
			nres = vatek_memfail;
			if (newbuf)
			{
				Pmux_string newstr = (Pmux_string)newbuf;
				newstr->len = pstring->len;
				newstr->maxlen = pstring->maxlen;
				newstr->text = &newbuf[sizeof(mux_string)];
				memcpy(&newstr->text[0], &pstring->text[0], pstring->len);
				*((Pmux_string*)&pdest[props[i].offset]) = newstr;
				nres = vatek_success;
			}
		}

		if (!is_vatek_success(nres))break;
		i++;
	}
	return nres;
}

vatek_result muxspec_set_program_default(uint32_t tag, Pspec_program pspec)
{
	#define TAGNAME "VATV-%02d"
	char sztmp[16];
	vatek_result nres = vatek_success;
	sprintf(&sztmp[0], TAGNAME, tag);

	tag += 1;

	if (mux_spec_is_dvb(pspec->spec))
	{
		Pmux_spec_dvb_program dvbprog = &pspec->_spec.dvb;
		dvbprog->channel_no = tag;
		dvbprog->program_number = tag;
		nres = muxspec_set_string_ascii(dvbprog->service_name, &sztmp[0]);
	}
	else if (mux_spec_is_arib(pspec->spec))
	{
		Pmux_spec_arib_program parib = &pspec->_spec.arib;
		parib->service_no = tag;
		nres = muxspec_set_string_jiseng(parib->service_name, &sztmp[0]);
		if (is_vatek_success(nres))
			nres = muxspec_set_string_jiseng(parib->ts_name, &sztmp[0]);
	}
	else if (mux_spec_is_arib(pspec->spec))
	{
		Pmux_spec_abnt_program pabnt = &pspec->_spec.abnt;
		pabnt->service_no = tag;
		nres = muxspec_set_string_jiseng(pabnt->service_name, &sztmp[0]);
		if (is_vatek_success(nres))
			nres = muxspec_set_string_jiseng(pabnt->ts_name, &sztmp[0]);
	}
	else if (mux_spec_is_psip(pspec->spec))
	{
		Pmux_spec_psip_program ppsip = &pspec->_spec.psip;
		ppsip->channel_minor = tag;
		ppsip->program_number = tag;
		ppsip->source_id = tag;
		nres = muxspec_set_string_psip_mutil(ppsip->long_name, &sztmp[0]);
	}
	else if (mux_spec_is_iso(pspec->spec))
	{
		Pmux_spec_iso_program piso = &pspec->_spec.iso;
		piso->program_number = tag;
	}
	else nres = vatek_unsupport;
	return nres;
}

void muxspec_printf_uiprops(mux_spec_mode spec,const char* fmt, Pui_prop_item props, uint8_t* pbuf)
{
	int32_t i = 0;
	char szval[32];
	while (props[i].name)
	{
		if (IS_UIPROP_STRBUF(props[i].type))
		{
			Pmux_string pmstr = *(Pmux_string*)&pbuf[props[i].offset];
			Pmux_ui_string puistr = NULL;
			memset(&szval[0], 0, 32);
			if(is_vatek_success(muxspec_get_spec_string(spec, &props[i], &puistr)))
				muxspec_get_spec_string_display(puistr, pmstr, &szval[0], 32);
			else strncpy(&szval[0], "_unknown", 32);
		}
		else ui_props_get_value_str(&props[i], pbuf, &szval[0], 32);
		printf(fmt, props[i].name, &szval[0]);
		i++;
	}
}

vatek_result muxspec_printf_channel(const char* fmt,Pspec_channel pch)
{
	Pui_prop_item puiprops = NULL;
	vatek_result nres = muxspec_get_channel_uiprops(pch->spec, &puiprops);
	if (is_vatek_success(nres))
		muxspec_printf_uiprops(pch->spec,fmt, puiprops, (uint8_t*)&pch->_spec);
	return nres;
}

vatek_result muxspec_printf_program(const char* fmt,Pspec_program program)
{
	Pui_prop_item puiprops = NULL;
	vatek_result nres = muxspec_get_program_uiprops(program->spec, &puiprops);
	if (is_vatek_success(nres))
		muxspec_printf_uiprops(program->spec,fmt, puiprops, (uint8_t*)&program->_spec);
	return nres;
}

