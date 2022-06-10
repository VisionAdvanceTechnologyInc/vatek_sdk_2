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

#ifndef _BRIDGE_BASE_
#define _BRIDGE_BASE_

#include <core/vatek_base.h>

#define BBRIDGE_TAG_MASK						0xFFF00000		/* bridge driver valid tag						*/	
#define BBRIDGE_TAG_TYPE_MASK					0xFFFFF000		/* bridge driver type [source|_demod|_rfmixer]	*/
#define BBRIDGE_TAG_DRI_MASK					0xFFFFFF00		/* bridge driver id								*/
#define BBRIDGE_TAG_IDX_MASK					0x000000FF		/* bridge driver attach device index			*/

#define BBRIDGE_TAG_BASE						0x7FA00000

#define BBRIDGE_TYPE_BSOURCE					(BBRIDGE_TAG_BASE | 0x1000)
#define BBRIDGE_TYPE_BDEMOD						(BBRIDGE_TAG_BASE | 0x2000)
#define BBRIDGE_TYPE_BRFMIXER					(BBRIDGE_TAG_BASE | 0x3000)

#define BBRIDGE_DRI_TO_BIT(dri)					(1 << ((dri & 0xF00) >> 8))

#define BBRIDGE_ENUM_MAX						0x7FFFFFFF		

#define BRIDGE_SET_CMD(ip,cmd)					(BOARDBRIDGE_CMD_TAG | ip | (cmd & 0xFFFF))

#define BPARAM_TYPE_UINT						1		/* fixed used uint32_t	*/
#define BPARAM_TYPE_ENUM						2
#define BPARAM_TYPE_STR							3		/* fixed len 16 bytes	*/
#define BPARAM_TYPE_RAW							4
#define BPARAM_TYPE_NULL						0xFF
#define BPARAM_STR_LEN							8

typedef struct _struct_bridge_param
{
	uint8_t type;
	uint8_t typelen;
	uint8_t s_offset;
	uint8_t b_offset;
}struct_bridge_param,*Pstruct_bridge_param;

typedef enum _bridge_status
{
	bstatus_invalid = -2,						/* video not supported or format not valid	*/
	bstatus_hw_fail = -1,						/* hdmi_phy detect video fail				*/
	bstatus_idle = 0,							/* can not found video						*/
	bstatus_active = 1,							/* valid video found						*/
    bstatus_active_protect = 2,					/* valid video found and protected (HDCP)	*/
}bridge_status;

#define BTAG_NUMS	(sizeof(bridge_tags)/sizeof(bridge_tag))

#define is_bstatus_fail(b)						(b < bstatus_idle)
#define is_bstatus_active(b)					(b >= bstatus_active && b <= bstatus_active_protect)

#define _bparam(pos,type,dtype,stype,pname)		{type,(uint8_t)sizeof(dtype),(uint8_t)offsetof(stype,pname),(pos << 2),},
#define _bparam_uint(pos,stype,pname)			_bparam(pos,BPARAM_TYPE_UINT,uint32_t,stype,pname)
#define _bparam_enum(pos,stype,etype,pname)		_bparam(pos,BPARAM_TYPE_ENUM,etype,stype,pname)
#define _bparam_str(pos,stype,pname)			{BPARAM_TYPE_STR,BPARAM_STR_LEN,(uint8_t)offsetof(stype,pname[0]),(pos << 2),},
#define _bparam_raw(pos,stype,praw,len)			{BPARAM_TYPE_RAW,len,(uint8_t)offsetof(stype,praw[0]),(pos << 2),},
#define _bparam_null							{BPARAM_TYPE_NULL,0,0,0,},

#define _bparam_start(stype)					static const struct_bridge_param bparam_##stype[] = {
#define _bparam_end								_bparam_null};

#define _bparam_struct_get(stype)				((const Pstruct_bridge_param)&bparam_##stype[0])

#endif
