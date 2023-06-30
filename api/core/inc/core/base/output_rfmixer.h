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

#ifndef _OUTPUT_RFMIXER_
#define _OUTPUT_RFMIXER_

#include <core/vatek_base.h>
#include <core/hal/halreg_rfmixer.h>

typedef enum _rfmixer_status{
	rfmixer_unsupport = -2,
	rfmixer_fail = -1,
	rfmixer_idle = 0,
	rfmixer_powered = 1,
}rfmixer_status;

#define R2_TUNE_HDR_TAG				0xFF001934
#define R2_TUNE_ITEM_TAG			0x47001934
#define R2_TUNE_STORAGE_TYPE		0xF1F10122
#define R2_TUNE_NUMS				0x40

#define R2_RANGE_I          0
#define R2_RANGE_Q          1
#define R2_RANGE_IMG        2
#define R2_RANGE_PHASE      3
#define R2_RANGE_PAGAIN     4
#define R2_RANGE_POWER      5
#define R2_RANGE_RC         6
#define R2_RANGE_NUMS       7

#define R2_VALIDX_MIN       0
#define R2_VALIDX_MAX       1
#define R2_VALIDX_SIGN      2
#define R2_VALIDX_MASK      3
#define R2_VALIDX_NUMS		4


static const int32_t r2_value_ranges[R2_RANGE_NUMS][R2_VALIDX_NUMS] =
{
	{ -127,127,0x80,0x7F, },
	{ -127,127,0x80,0x7F, },
	{ -31,31,0x20,0x1F, },
	{ -15,15,0x10,0xF, },
	{ 0,3,0,0x3 },
	{ 0,0xFFFF,0,0xF},
	{ 0,3,0,0x3 },
};

typedef struct _r2_tune
{
	uint16_t ioffset;
	uint16_t qoffset;
	uint16_t imgoffset;
	uint16_t phaseoffset;
}r2_tune,*Pr2_tune;

typedef struct _r2_tune_rule
{
	r2_tune tune;
	uint16_t pagain;
	uint16_t gpiocntl;
}r2_tune_rule;

typedef r2_tune_rule* Pr2_tune_rule;

typedef r2_tune r2_default_rule;
typedef r2_default_rule* Pr2_default_rule;

typedef struct _r2_tune_item{
	uint32_t tag;
	uint32_t freq;
	r2_tune_rule rule[2];
}r2_tune_item;

typedef r2_tune_item* Pr2_tune_item;

#define R2_FLAG_SET_IQSWAP(flag,a)		flag = ((flag & 0xFFFFFF00) | (a & 0xFF))
#define R2_FLAG_GET_IQSWAP(a)			(a & 0xFF)
#define R2_FLAG_EN_CALIBRATION(flag)	((flag & R2FLAG_EN_CALIBRATION) != 0)

typedef struct _r2_tune_handle{
	uint32_t tag;
	uint32_t items_num;
	uint32_t r2_flags_0;
	uint32_t r2_flags_1;
	r2_tune_item rules[R2_TUNE_NUMS];
}r2_tune_handle;

typedef r2_tune_handle* Pr2_tune_handle;

typedef enum _r2_cntl_mode
{
	r2_cntl_unknown = -1,
	r2_cntl_path_0 = 0,
	r2_cntl_path_1 = 1,
	r2_cntl_param = 2,
}r2_cntl_mode;

#define is_current_r2_cntl_mode(cntl)	(cntl >= r2_cntl_path_0 && cntl <= r2_cntl_param)
#define is_current_r2_param(cntl)		(cntl == r2_cntl_param)

typedef struct _r2_param{
	r2_cntl_mode mode;
	uint32_t r2_flags;
	uint32_t freqkhz;
	r2_tune_rule rule;
}r2_param;

typedef r2_param* Pr2_param;


#ifdef __cplusplus
extern "C" {
#endif

	HAL_API vatek_result rfmixer_check_support(hvatek_chip hchip);
	HAL_API vatek_result rfmixer_start(hvatek_chip hchip,uint32_t cntl,uint32_t freqkhz);
	HAL_API vatek_result rfmixer_stop(hvatek_chip hchip,uint32_t cntl);

	HAL_API rfmixer_status rfmixer_get_status(hvatek_chip hchip);
	HAL_API vatek_result rfmixer_set_status(hvatek_chip hchip,rfmixer_status status);

	HAL_API vatek_result rfmixer_r2_check_support(hvatek_chip hchip);

	HAL_API void rfmixer_r2_reset_tune_table(Pr2_tune_handle pr2tune);
	HAL_API vatek_result rfmixer_r2_add_tune_table(Pr2_tune_handle pr2tune, uint32_t freq, Pr2_tune_item* pitem);
	HAL_API vatek_result rfmixer_r2_del_tune_table(Pr2_tune_handle pr2tune, uint32_t freq);

	HAL_API vatek_result rfmixer_r2_get_default(hvatek_chip hchip,Pr2_default_rule pr2def);
	HAL_API vatek_result rfmixer_r2_set_default(hvatek_chip hchip,Pr2_default_rule pr2def);

	HAL_API vatek_result rfmixer_r2_get_param(hvatek_chip hchip,Pr2_param pr2);
	HAL_API vatek_result rfmixer_r2_set_param(hvatek_chip hchip,Pr2_param pr2);

	HAL_API vatek_result rfmixer_r2_start(hvatek_chip hchip,uint32_t cntl,Pr2_param pr2param);
	HAL_API vatek_result rfmixer_r2_stop(hvatek_chip hchip,uint32_t cntl);
	HAL_API vatek_result rfmixer_r2_adjust_pagain(hvatek_chip hchip, int8_t gain);

	HAL_API uint16_t rfmixer_r2_int2tune(int32_t val, int32_t idx);
	HAL_API int32_t rfmixer_r2_tune2int(uint16_t val, int32_t idx);

#ifdef __cplusplus
}
#endif

#endif
