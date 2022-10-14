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

#ifndef _BRIDGE_DEVICE_
#define _BRIDGE_DEVICE_

#include <bridge/bridge_base.h>
#include <bridge/bridge_source.h>
#include <bridge/bridge_demod.h>
#include <bridge/bridge_rfmixer.h>

#define BRIDGE_IP_DEVICE_TAG						0x00F00000
#define IP_DEVICE_SET_CMD(cmd)						BRIDGE_SET_CMD(BRIDGE_IP_DEVICE_TAG,cmd)
#define IS_BRIDGE_DEVICE_CMD(cmd)					((cmd & 0x00FF0000) == BRIDGE_IP_DEVICE_TAG)

#define BDEVICE_CMD_GET_INFO						IP_DEVICE_SET_CMD(0x800)

#define BDEVICE_CMD_BFLASH_START					IP_DEVICE_SET_CMD(0x201)
#define BDEVICE_CMD_BFLASH_POS						IP_DEVICE_SET_CMD(0x202)
#define BDEVICE_CMD_BFLASH_END						IP_DEVICE_SET_CMD(0x203)

#define BFLASH_CNTL_WRITE							0x0F0F0500
#define BFLASH_CNTL_READ							0x0F0F0501
#define BFLASH_CNTL_ERASE							0x0F0F0502

typedef struct _bdevice_info
{
	uint32_t bversion;								/* bridge fw version					*/
	uint32_t source_enable;							/* enabled source driver				*/
	uint32_t demod_enable;							/* enabled _demod driver				*/
	uint32_t rfmixer_enable;						/* enabled rf_mixer driver				*/
	uint32_t source_nums;							/* how many valid source devices		*/
}bdevice_info,*Pbdevice_info;

typedef struct _bflash_command
{
	uint32_t bcmd;
	uint32_t nsection;
}bflash_command,*Pbflash_command;

#define BSTORAGE_FILE_EXTENSION				".bstorage"

#define BSTORAGE_SECTION_LEN				4096

#define BRESOURCE_TAG_MASK					0xFFFFFF00
#define BRESOURCE_TAG_BASE					0x08FF5500
#define BRESOURCE_TAG_HEADER				BRESOURCE_TAG_BASE

#define BRESOURCE_TAG_DEMODFW				(BRESOURCE_TAG_BASE | 0x10)
#define BRESOURCE_TAG_DEMOD_DVB_T			(BRESOURCE_TAG_DEMODFW | MOD_DVB_T)
#define BRESOURCE_TAG_DEMOD_J83A			(BRESOURCE_TAG_DEMODFW | MOD_J83A)
#define BRESOURCE_TAG_DEMOD_DVB_S			(BRESOURCE_TAG_DEMODFW | MOD_DVB_S)
#define BRESOURCE_TAG_DEMOD_ISDB_T			(BRESOURCE_TAG_DEMODFW | MOD_ISDB_T)
#define BRESOURCE_TAG_END					(BRESOURCE_TAG_BASE | 0xFF)

#define BFLASH_HDR_TAG						"va_bridge_01"
#define BFLASH_TAG_LEN						32
#define BFLASH_HDR_VER						0x08700122
#define BFLASH_CONFIG_START					256
#define BFLASH_CONFIG_OFFSET				64

typedef struct _bstorage_header
{
	uint8_t tag[BFLASH_TAG_LEN];
	uint32_t version;
}bstorage_header,*Pbstorage_header;


#define STORAGE_CFG_OFFSET				64

typedef struct _bstorage_config
{
	uint32_t bridge_tag;
	union
	{
		uint8_t raw[BFLASH_CONFIG_OFFSET - 4];
		ep9555e_param ep9555e;
		h1_param h1;
		adv718x_param adv718x;
		ep9351_param ep9351;
	}cfg;
}bstorage_config, *Pbstorage_config;

typedef struct _bresource_section
{
	uint32_t tag;
	uint32_t section_addr;
	uint32_t section_len;
	uint32_t binary_size;
}bresource_section,*Pbresource_section;

typedef struct _bridge_tag
{
	uint32_t tag;
	const char* name;
}bridge_tag, * Pbridge_tag;

static const bridge_tag bridge_tags[] =
{
	{bsource_ep9555e	,"ep9555e",},
	{bsource_ep9351		,"ep9351",},
	{bsource_adv718x	,"adv718x",},
	{bsource_h1			,"h1",},
	{bdemod_avl68xx		,"avl68xx",},
	{brfmixer_fintek_r2	,"fintek_r2",},
	{brfmixer_adrf6755	,"adrf6755",},
};

/*
	|	section 0 [header + driver_param [each 128 bytes] ]
*/

#define BRESOURCE_FIRST_SECTIONLEN		(BSTORAGE_SECTION_LEN - sizeof(bresource_section))

_bparam_start(bflash_command)
	_bparam_uint(0, bflash_command, bcmd)
	_bparam_uint(1, bflash_command, nsection)
_bparam_end

_bparam_start(bdevice_info)
	_bparam_uint(0, bdevice_info, bversion)
	_bparam_uint(1, bdevice_info, source_enable)
	_bparam_uint(2, bdevice_info, demod_enable)
	_bparam_uint(3, bdevice_info, rfmixer_enable)
	_bparam_uint(4, bdevice_info, source_nums)
_bparam_end

#endif
