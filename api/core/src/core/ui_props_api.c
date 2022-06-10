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

#include <core/ui/ui_props_api.h>

static const char* _str_unknown = "_unknown";

extern uint32_t ui_prop_get_uint32(const Pui_prop_item pprop, uint8_t* pblock);
extern void ui_prop_set_uint32(const Pui_prop_item pprop, uint8_t* pblock, uint32_t val);
extern vatek_result check_value_select(const Pui_prop_item pprops, uint32_t val);
extern vatek_result check_value_checkbox(const Pui_prop_item pprops, uint32_t val);
extern vatek_result ui_prop_check(const Pui_prop_item pprop, uint8_t* pblock);
extern const char* ui_prop_get_select(const Pui_prop_item pprop, uint32_t val);

vatek_result ui_props_set_uint32(const Pui_prop_item pprop, uint8_t* pblock, uint32_t val)
{
	if (IS_UIPROP_STRBUF(pprop->type) || IS_UIPROP_LANG(pprop->type))return vatek_badparam;
	else
	{
		ui_prop_set_uint32(pprop, pblock, val);
		return vatek_success;
	}
}

vatek_result ui_props_get_uint32(const Pui_prop_item pprop, uint8_t* pblock, uint32_t* val)
{
	if (IS_UIPROP_STRBUF(pprop->type) || IS_UIPROP_LANG(pprop->type))return vatek_badparam;
	else
	{
		*val = ui_prop_get_uint32(pprop, pblock);
		return vatek_success;
	}
}

vatek_result ui_props_set_strbuf(const Pui_prop_item props, uint8_t* pblock, uint8_t* val)
{
	uint8_t* ptr = &pblock[props->offset];
	vatek_result nres = ui_props_get_strbuf_len(props);
	if (is_vatek_success(nres))memcpy(ptr, val, nres);
	return nres;
}

vatek_result ui_props_get_strbuf(const Pui_prop_item props, uint8_t* pblock, uint8_t** val)
{
	char* ptr = (char*)&pblock[props->offset];
	if (IS_UIPROP_STRBUF(props->type) || IS_UIPROP_LANG(props->type))*val = (uint8_t*)ptr;
	else return vatek_badparam;
	return vatek_success;
}

vatek_result ui_props_get_strbuf_len(const Pui_prop_item props)
{
	if (IS_UIPROP_STRBUF(props->type))return (vatek_result)(props->type & _UIPROP_STRBUF_LEN_MASK);
	else if (IS_UIPROP_LANG(props->type))return (vatek_result)4;
	return vatek_badparam;
}

const Pui_prop_item ui_props_check_values(const Pui_prop_item pprops, uint8_t* pblock)
{
	int32_t pos = 0;
	vatek_result nres = vatek_badparam;
	while (pprops[pos].type != prop_type_null)
	{
		nres = ui_prop_check(pprops, pblock);
		if (!is_vatek_success(nres))break;
		pos++;
	}
	if (!is_vatek_success(nres))return &pprops[pos];
	return NULL;
}

void ui_props_get_value_str(const Pui_prop_item pprop, uint8_t* pblock, char* szval, int32_t len)
{
	uint8_t* pvalptr = &pblock[pprop->offset];
	if (IS_UIPROP_STRBUF(pprop->type))
	{
		if (IS_UIPROP_PATH(pprop->type))
		{
			int32_t len = (int32_t)strlen((char*)pvalptr);
			if (len == 0)strncpy(szval, "empty", _UIPROP_STRBUF_LEN_MAX);
			else strncpy(szval, (char*)pvalptr, _UIPROP_STRBUF_LEN_MAX);
		}
		else strncpy(szval, "string", len);
	}
	else if (IS_UIPROP_LANG(pprop->type))strncpy(szval, (char*)pvalptr, len);
	else
	{
		uint32_t val = ui_prop_get_uint32(pprop, pblock);
		if (pprop->values)
		{
			Pui_prop_item perr = ui_props_check_values(pprop, pblock);
			if (perr)strncpy(szval, "unknown", len);
			else strncpy(szval, ui_prop_get_select(pprop, val), len);
		}

		if (IS_UIPROP_CHECKBOS(pprop->type) || IS_UIPROP_HEX(pprop->type))
		{
			int32_t len = pprop->type & 0xF;
			if (len == 1)sprintf(szval, "0x%02x", val);
			else if (len == 2)sprintf(szval, "0x%04x", val);
			else if (len == 4)sprintf(szval, "0x%08x", val);
			else sprintf(szval, "%x", val);
		}else if(!pprop->values)sprintf(szval, "%d", val);
	}
}

const char* ui_props_get_type_str(ui_prop_type type, const Pui_prop_val pvals)
{
	if (IS_UIPROP_STRBUF(type))
	{
		if (IS_UIPROP_PATH(type))return "path";
		else return "string";
	}
	else if (IS_UIPROP_LANG(type))return "lang";
	else
	{
		if (pvals)
		{
			if (IS_UIPROP_CHECKBOS(type))return "flags";
			else return "select";
		}
		else
		{
			int32_t len = type & 0xF;
			if (len == 1)return "uint8";
			else if (len == 2)return "uint16";
			else return "uint32";
		}
	}
}

uint32_t ui_prop_get_uint32(const Pui_prop_item pprop, uint8_t* pblock)
{
	int32_t len = pprop->type & 0xF;
	uint8_t* pval = &pblock[pprop->offset];
	uint32_t val = 0;
	if (len == 1)val = *(uint8_t*)pval;
	else if (len == 2)val = *(uint16_t*)pval;
	else if (len == 4)val = *(uint32_t*)pval;
	else VERR("unknown uint len : %s - %d", pprop->name, pprop->type);
	/* halmap == null used with mux api*/
	if(pprop->halmap != NULL)return HAL_VAL(pprop->halmap, val);
	return val;
}

void ui_prop_set_uint32(const Pui_prop_item pprop, uint8_t* pblock, uint32_t val)
{
	int32_t len = pprop->type & 0xF;
	uint8_t* pval = &pblock[pprop->offset];

	/* halmap == null used with mux api*/
	if(pprop->halmap != NULL)
		val = HAL_VAL(pprop->halmap, val);
	if (len == 1)*pval = (uint8_t)val;
	else if (len == 2)*((uint16_t*)pval) = (uint16_t)val;
	else if (len == 4)*((uint32_t*)pval) = (uint32_t)val;
	else VERR("unknown uint len : %s - %d", pprop->name, pprop->type);
}

const char* ui_prop_get_select(const Pui_prop_item pprop, uint32_t val)
{
	int32_t pos = 0;
	while (pprop->values[pos].name != NULL)
	{
		if (pprop->values[pos].val == val)
			return pprop->values[pos].name;
		pos++;
	}
	return _str_unknown;
}

vatek_result ui_prop_check(const Pui_prop_item pprop, uint8_t* pblock)
{
	vatek_result nres = vatek_success;
	if (pprop->values != NULL)
	{
		uint32_t val = ui_prop_get_uint32(pprop, pblock);
		if (IS_UIPROP_CHECKBOS(pprop->type))nres = check_value_checkbox(pprop, val);
		else nres = check_value_select(pprop, val);
	}
	return nres;
}

vatek_result check_value_select(const Pui_prop_item pprops, uint32_t val)
{
	int32_t pos = 0;
	vatek_result nres = vatek_badparam;
	while (pprops->values[pos].name != NULL)
	{
		if (pprops->values[pos].val == val)nres = vatek_success;
		if (is_vatek_success(nres))break;
		pos++;
	}

	return nres;
}

vatek_result check_value_checkbox(const Pui_prop_item pprops, uint32_t val)
{
	int32_t pos = 0;
	uint32_t tags = 0xFFFFFFFF;
	while (pprops->values[pos].name != NULL)
	{
		tags &= ~(pprops->values[pos].val);
		pos++;
	}

	if ((val & tags) != 0)return vatek_badparam;
	return vatek_success;
}

vatek_result ui_props_read_hal(hvatek_chip hchip,const Pui_prop_item pprop, uint8_t* pblock)
{
	int32_t pos = 0;
	vatek_result nres = vatek_success;

	while (pprop[pos].type != prop_type_null)
	{
		if (!IS_UIPROP_STRBUF(pprop[pos].type))
		{
			Pui_hal_param phal = pprop[pos].halmap;
			if (IS_UIPROP_LANG(pprop[pos].type))
				nres = vatek_chip_read_buffer(hchip, phal->addr, &pblock[pprop[pos].offset], 1);
			else
			{
				uint32_t val = 0;
				nres = vatek_chip_read_memory(hchip, phal->addr, &val);
				if (is_vatek_success(nres))
					ui_prop_set_uint32(&pprop[pos], pblock, val);
			}

			if (!is_vatek_success(nres))break;
			pos++;
		}
		else pos++;
	}

	if (!is_vatek_success(nres))VWAR("chip read fail : %s - %d", pprop[pos].name, nres);
	return nres;
}

vatek_result ui_props_write_hal(hvatek_chip hchip,const  Pui_prop_item pprop, uint8_t* pblock)
{
	int32_t pos = 0;
	vatek_result nres = vatek_success;

	while (pprop[pos].type != prop_type_null)
	{
		if (!IS_UIPROP_STRBUF(pprop[pos].type))
		{
			Pui_hal_param phal = pprop[pos].halmap;
			if (IS_UIPROP_LANG(pprop[pos].type))
				nres = vatek_chip_write_buffer(hchip, phal->addr, &pblock[pprop[pos].offset], 1);
			else nres = vatek_chip_write_memory(hchip, phal->addr, ui_prop_get_uint32(&pprop[pos], pblock));
			if (!is_vatek_success(nres))break;
		}
		pos++;
	}

	if (!is_vatek_success(nres))VWAR("chip write memory fail : %d - %d", pprop[pos].name, nres);
	return nres;
}

const Pui_prop_item ui_props_get_param(const Pui_prop_item pprops, const char* name)
{
	int32_t i = 0;
	while (pprops[i].type != prop_type_null)
	{
		if (strcmp(pprops[i].name, name) == 0)
			return &pprops[i];
		i++;
	}
	return NULL;
}

vatek_result ui_props_is_same_param(const Pui_prop_item props, const char* name,const  Pui_prop_item prop)
{
	Pui_prop_item param = ui_props_get_param(props, name);
	vatek_result nres = vatek_badparam;
	if (param)
	{
		if (strcmp(param->desc, prop->desc) == 0)
			nres = vatek_success;
	}
	return nres;
}
const char* ui_enum_val2str(const Pui_prop_val vals, uint32_t val)
{
	int32_t i = 0;
	while (vals[i].name)
	{
		if (vals[i].val == val)
			return vals[i].name;
		i++;
	}
	return "_unknwon[bad enum]";
}

vatek_result ui_enum_str2val(const Pui_prop_val vals, const char* str, uint32_t* val)
{
	int32_t i = 0;
	while (vals[i].name)
	{
		if (strcmp(str, vals[i].name) == 0)
		{
			*val = vals[i].val;
			return vatek_success;
		}
		i++;
	}
	return vatek_badparam;
}

void ui_props_printf(const char* fmt_n_t_v, const char* fmt_sv,const Pui_prop_item puiprops, uint8_t* prawbuf)
{
	int32_t i = 0;
	char sztmpp[64];
	while (puiprops[i].name)
	{
		const Pui_prop_item ptrprop = &puiprops[i];
		const char* sztype = ui_props_get_type_str(puiprops[i].type,puiprops[i].values);
		memset(&sztmpp[0],0,64);
		ui_props_get_value_str(ptrprop, prawbuf, &sztmpp[0], 64);
		printf(fmt_n_t_v, ptrprop->name,sztype, &sztmpp[0]);

		if (puiprops[i].values && fmt_sv)
		{
			int32_t j = 0;
			while (puiprops[i].values[j].name)
			{
				printf(fmt_sv,"", puiprops[i].values[j].name, puiprops[i].values[j].val);
				j++;
			}
		}
		i++;
	}
}
