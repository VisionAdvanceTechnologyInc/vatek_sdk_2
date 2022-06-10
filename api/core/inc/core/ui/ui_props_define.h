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

#ifndef _UI_PROPS_DEFINE_
#define _UI_PROPS_DEFINE_

#include <core/vatek_base.h>
#include <core/hal/halreg_define.h>

typedef struct _ui_hal_param
{
    uint8_t len;
    uint8_t recv;
    uint16_t addr;
}ui_hal_param,*Pui_hal_param;

const static uint32_t hal_len_mask[] =
{
	0x00000001,0x00000003,0x00000007,0x0000000F,
	0x0000001F,0x0000003F,0x0000007F,0x000000FF,
	0x000001FF,0x000003FF,0x000007FF,0x00000FFF,
	0x00001FFF,0x00003FFF,0x00007FFF,0x0000FFFF,
	0x0001FFFF,0x0003FFFF,0x0007FFFF,0x000FFFFF,
	0x001FFFFF,0x003FFFFF,0x007FFFFF,0x00FFFFFF,
	0x01FFFFFF,0x03FFFFFF,0x07FFFFFF,0x0FFFFFFF,
	0x1FFFFFFF,0x3FFFFFFF,0x7FFFFFFF,0xFFFFFFFF,
};

const static ui_hal_param _halreg_NULLHAL 				= {0,0, };

#define HAL_VAL(param,val)  							(val & (hal_len_mask[(param->len -1) & 0x1F]))

#define HALREG_S(tag,len)								const static ui_hal_param _halreg_##tag = {len,0,HALREG_##tag};
#define HALREG(tag)										((Pui_hal_param)&_halreg_##tag)

HALREG_S(EMPTY_HAL,32)

#define PROPTYPE_HEX_TYPE       0x0100
#define PROPTYPE_FLAG_TYPE      0x0200
#define PROPTYPE_STRBUF			0x8000
#define PROPTYPE_LANG			0x1000

typedef enum _ui_prop_type
{
	prop_type_null		= -1,
	prop_type_uint8		= 1,
	prop_type_uint16	= 2,
	prop_type_uint32	= 4,
	prop_type_hex8		= PROPTYPE_HEX_TYPE | 1,
	prop_type_hex16		= PROPTYPE_HEX_TYPE | 2,
	prop_type_hex32		= PROPTYPE_HEX_TYPE | 4,
	prop_type_cb8		= PROPTYPE_FLAG_TYPE | 1,
	prop_type_cb16		= PROPTYPE_FLAG_TYPE | 2,
	prop_type_cb32		= PROPTYPE_FLAG_TYPE | 4,
	prop_type_cblang	= PROPTYPE_LANG,
	prop_type_strbuf	= PROPTYPE_STRBUF,
	prop_type_strpath	= 0x1000 | PROPTYPE_STRBUF,
}ui_prop_type;

#define IS_UIPROP_HEX(type)         					((type & PROPTYPE_HEX_TYPE) == PROPTYPE_HEX_TYPE)
#define IS_UIPROP_CHECKBOS(type)    					((type & PROPTYPE_FLAG_TYPE) == PROPTYPE_FLAG_TYPE)

#define IS_UIPROP_LANG(type)							((type & PROPTYPE_LANG) == PROPTYPE_LANG)
#define IS_UIPROP_STRBUF(type)							((type & PROPTYPE_STRBUF) == PROPTYPE_STRBUF)

#define IS_UIPROP_PATH(type)							((type & prop_type_strpath) == prop_type_strpath)

#define _UIPROP_VAL_NULL								0xFFFFFFFF
#define _UIPROP_STRBUF_LEN_MASK							0xFFF
#define _UIPROP_STRBUF_LEN_MAX							0x100

struct _ui_prop_item;

typedef struct _ui_prop_val
{
    const char* name;
    uint32_t val;
}ui_prop_val, *Pui_prop_val;

typedef struct _ui_prop_range
{
	int32_t minvalue;
	int32_t maxvalue;
}ui_prop_range,*Pui_prop_range;

typedef struct _ui_prop_item
{
    const char* name;
    const char* desc;
    ui_prop_type type;
    const Pui_prop_val values;
    const Pui_hal_param halmap;
    int32_t offset;
	const Pui_prop_range range;
}ui_prop_item, *Pui_prop_item;

#define _val(val,name)									{#name,(uint32_t)val,},
#define _val_end										{NULL,_UIPROP_VAL_NULL},

#define _prop_str(type,name,desc,len)					{#name,desc,(ui_prop_type)(prop_type_strbuf | (len & _UIPROP_STRBUF_LEN_MASK)),NULL,0,offsetof(type,name),NULL,},
#define _prop_path(type,name,desc,len)					{#name,desc,(ui_prop_type)(prop_type_strpath |(len & _UIPROP_STRBUF_LEN_MASK)),NULL,0,offsetof(type,name),NULL,},

#define _prop_end										{NULL,NULL,prop_type_null,NULL,NULL,0,NULL,},

#define _prop_u32(type,name,desc,hal)					{#name,desc,prop_type_uint32,NULL,HALREG(hal),offsetof(type,name),NULL,},
#define _prop_u16(type,name,desc,hal)					{#name,desc,prop_type_uint16,NULL,HALREG(hal),offsetof(type,name),NULL,},
#define _prop_u8(type,name,desc,hal)					{#name,desc,prop_type_uint8,NULL,HALREG(hal),offsetof(type,name),NULL,},
#define _prop_h32(type,name,desc,hal)					{#name,desc,prop_type_hex32,NULL,HALREG(hal),offsetof(type,name),NULL,},
#define _prop_h16(type,name,desc,hal)					{#name,desc,prop_type_hex16,NULL,HALREG(hal),offsetof(type,name),NULL,},
#define _prop_h8(type,name,desc,hal)					{#name,desc,prop_type_hex8,NULL,HALREG(hal),offsetof(type,name),NULL,},


#define _ui_name(type,name,tag)							type##_##name##_##tag


#define _ui_enum_start(type)							const static ui_prop_val type##_enum[] = {
#define _ui_enum_end									_val_end};
#define _ui_enum(type) 									((const Pui_prop_val)&type##_enum[0])

#define _enum_raw(type,enum,name,desc,hal,uitype)		{#name,desc,uitype,_ui_enum(enum),HALREG(hal),offsetof(type,name),},

#define _ui_flags_start(type,name)						const static ui_prop_val _ui_name(type,name,flags)[] = {
#define _ui_flags_end									_val_end};
#define _ui_flags(type,name) 							((const Pui_prop_val)&_ui_name(type,name,flags)[0])

#define _flag_raw(type,name,desc,hal,vals)				{#name,desc,prop_type_cb32,vals,HALREG(hal),offsetof(type,name),},

#define _ui_struct_start(type)							const static ui_prop_item type##_struct[] = {

#define _prop_flag(type,name,desc,hal)					_flag_raw(type,name,desc,hal,_ui_flags(type,name))
#define _prop_flag_diff(type,name,flags,desc,hal) 		_flag_raw(type,name,desc,hal,_ui_flags(type,flags))

#define _prop_enum(type,enum,name,desc,hal) 			_enum_raw(type,enum,name,desc,hal,(ui_prop_type)sizeof(enum))
#define _prop_enum_dif(type,enum,dtype,name,desc,hal) 	_enum_raw(type,enum,name,desc,hal,(ui_prop_type)sizeof(dtype))

#define _ui_struct_end									_prop_end};

#define _ui_struct(type)								((const Pui_prop_item)&type##_struct[0])

#endif
