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

#ifndef _BRIDGE_DEMODE_
#define _BRIDGE_DEMODE_

#include <bridge/bridge_base.h>

typedef enum _bdemod_driver
{
	bdemod_avl68xx = (BBRIDGE_TYPE_BDEMOD | 0x100),
}bdemod_driver;


#define BDEMOD_AVL68XX_BIT			BBRIDGE_DRI_TO_BIT(bdemod_avl68xx)

#define BRIDGE_IP_DEMOD_TAG        	0x00F80000
#define IP_DEMOD_SET_CMD(cmd)		BRIDGE_SET_CMD(BRIDGE_IP_DEMOD_TAG,cmd)
#define IS_BRIDGE_DEMOD_CMD(cmd)	((cmd & 0x00FF0000) == BRIDGE_IP_DEMOD_TAG)

/* bridge _demod command define */

#define BDEMOD_CMD_GETINFO			IP_DEMOD_SET_CMD(0x100)
#define BDEMOD_CMD_STATUS			IP_DEMOD_SET_CMD(0x101)
#define BDEMOD_CMD_SET_MODE			IP_DEMOD_SET_CMD(0x102)
#define BDEMOD_CMD_GET_MODE			IP_DEMOD_SET_CMD(0x103)
#define BDEMOD_CMD_START			IP_DEMOD_SET_CMD(0x104)
#define BDEMOD_CMD_STOP				IP_DEMOD_SET_CMD(0x105)

typedef struct _bdemod_info
{
	uint32_t support_mode;
	uint32_t enable_mode;
}bdemod_info,*Pbdemod_info;

#define BDEMOD_EN_TS_SERIAL			0x00000001

typedef struct _bdemod_mode_param
{
	uint32_t mod_mode;							/* MOD_XXXX_BIT defined in base/mod_define.h */
	uint32_t ts_clock;
	uint32_t ts_enable;
}bdemod_mode_param,*Pbdemod_mode_param;

typedef struct _bdemod_op_param
{
	uint32_t bandwidth_or_symbolrate;
	uint32_t freq_khz;
}bdemod_op_param,*Pbdemod_op_param;

_bparam_start(bdemod_info)
	_bparam_uint(0,bdemod_info, support_mode)
	_bparam_uint(1,bdemod_info, enable_mode)
_bparam_end

_bparam_start(bdemod_mode_param)
	_bparam_uint(0, bdemod_mode_param, mod_mode)
	_bparam_uint(1, bdemod_mode_param, ts_clock)
	_bparam_uint(2, bdemod_mode_param, ts_enable)
_bparam_end

_bparam_start(bdemod_op_param)
	_bparam_uint(0, bdemod_op_param, bandwidth_or_symbolrate)
	_bparam_uint(1, bdemod_op_param, freq_khz)
_bparam_end

#endif
