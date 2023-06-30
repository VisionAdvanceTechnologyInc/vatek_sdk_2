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

#include <core/base/output_rfmixer.h>
#include <vatek_sdk_device.h>
#include <core/ui/ui_props/ui_props_output.h>

vatek_result rfmixer_check_support(hvatek_chip hchip)
{
    return rfmixer_r2_check_support(hchip);
}

vatek_result rfmixer_start(hvatek_chip hchip,uint32_t cntl,uint32_t freqkhz)
{
    vatek_result nres = writehal(HALREG_RF_COM_FREQ,freqkhz);
    if (is_vatek_success(nres))
    {
        nres = chip_send_command(hchip,RFMIXER_CMD_START, cntl,HALREG_SYS_ERRCODE);
    }
    return nres;
}

vatek_result rfmixer_stop(hvatek_chip hchip,uint32_t cntl)
{
    return chip_send_command(hchip,RFMIXER_CMD_STOP, cntl,HALREG_SYS_ERRCODE);
}

rfmixer_status rfmixer_get_status(hvatek_chip hchip)
{
    uint32_t val = 0;
    vatek_result nres = readhal(HALREG_RF_COM_STATUS,&val);
    rfmixer_status status = rfmixer_fail;
    if(is_vatek_success(nres))
    {
        if(val == RF_STATUS_ACTIVE)status = rfmixer_powered;
        else if(val == RF_STATUS_IDLE)status = rfmixer_idle;
    }
    return status;
}

vatek_result rfmixer_set_status(hvatek_chip hchip,rfmixer_status status)
{
    vatek_result nres = vatek_badparam;
    if(status == rfmixer_powered)nres = writehal(HALREG_RF_COM_STATUS,RF_STATUS_ACTIVE);
    else if(status == rfmixer_idle)nres = writehal(HALREG_RF_COM_STATUS,RF_STATUS_IDLE);
    else if(status == rfmixer_fail)nres = writehal(HALREG_RF_COM_STATUS,RF_STATUS_FAIL);
    return nres;
}

vatek_result rfmixer_r2_check_support(hvatek_chip hchip)
{
	uint32_t val = 0;
	vatek_result nres = readhal(HALREG_PERIPHERAL_EN,&val);
	if(is_vatek_success(nres))
	{
		if(!(val & PERIPHERAL_FINTEKR2))
			nres = vatek_unsupport;
	}
	return nres;
}

#if 0

vatek_result rfmixer_r2_get_default(hvatek_chip hchip,Pr2_default_rule pr2def)
{
    uint32_t val = 0;
    vatek_result nres = readhal(HALREG_R2_DEF_TAG,&val);
    if(is_vatek_success(nres))
    {
        if(val == R2_EN_DEF_TAG)
            nres = ui_props_read_hal(hchip,_ui_struct(r2_tune_default),(uint8_t*)pr2def);
        else nres = vatek_format;
    }
    return nres;
}

vatek_result rfmixer_r2_set_default(hvatek_chip hchip,Pr2_default_rule pr2def)
{
    vatek_result nres = ui_props_write_hal(hchip,_ui_struct(r2_tune_default),(uint8_t*)pr2def);
    if(is_vatek_success(nres))
        nres = writehal(HALREG_R2_DEF_TAG, R2_EN_DEF_TAG);
    return nres;
}

#endif

vatek_result rfmixer_r2_get_param(hvatek_chip hchip,Pr2_param pr2)
{
    uint32_t val = 0;
    vatek_result nres = readhal(HALREG_R2_EN_CNTL,&val);
    if(is_vatek_success(nres))
    {
        nres = vatek_badparam;
        if((val & 0xFFFF0000) == R2_EN_TAG_BASE)
        {
            nres = vatek_success;
            if(val == R2_EN_PATH_0)pr2->mode = r2_cntl_path_0;
            else if(val == R2_EN_PATH_1)pr2->mode = r2_cntl_path_1;
            else if(val == R2_EN_PARAM)pr2->mode = r2_cntl_param;
            else nres = vatek_badparam;
        }

        if(is_vatek_success(nres))
            nres = readhal(HALREG_RF_COM_FREQ,&pr2->freqkhz);
        

        nres = readhal(HALREG_R2_FLAGS,&pr2->r2_flags);
        if (is_vatek_success(nres))
        {
            nres = ui_props_read_hal(hchip, _ui_struct(r2_tune_rule), (uint8_t*)&pr2->rule);
            if (is_vatek_success(nres))
                nres = ui_props_read_hal(hchip, _ui_struct(r2_tune), (uint8_t*)&pr2->rule.tune);
        }    

    }
    return nres;
}

vatek_result rfmixer_r2_set_param(hvatek_chip hchip,Pr2_param pr2)
{
    vatek_result nres = vatek_badparam;
    if(pr2->mode == r2_cntl_path_0)nres = writehal(HALREG_R2_EN_CNTL,R2_EN_PATH_0);
    else if(pr2->mode == r2_cntl_path_1)nres = writehal(HALREG_R2_EN_CNTL,R2_EN_PATH_1);
    else if(pr2->mode == r2_cntl_param)nres = writehal(HALREG_R2_EN_CNTL,R2_EN_PARAM);

    if(is_vatek_success(nres))
    {
        nres = writehal(HALREG_RF_COM_FREQ,pr2->freqkhz);
        if(is_vatek_success(nres) && pr2->mode == r2_cntl_param)
        {
            nres = writehal(HALREG_R2_FLAGS,pr2->r2_flags);
            if (is_vatek_success(nres))
            {
                nres = ui_props_write_hal(hchip, _ui_struct(r2_tune_rule), (uint8_t*)&pr2->rule);
                if (is_vatek_success(nres))
                    nres = ui_props_write_hal(hchip, _ui_struct(r2_tune), (uint8_t*)&pr2->rule.tune);
            }
        }
    }
    return nres;
}

vatek_result rfmixer_r2_adjust_pagain(hvatek_chip hchip, int8_t gain) {
    uint32_t nvalue = EXT_R2_GAIN_EN_TAG | (EXT_R2_GAIN_MASK & gain);
    return writehal(HALREG_EXT_R2_GAIN, nvalue);
}

vatek_result rfmixer_r2_start(hvatek_chip hchip,uint32_t cntl,Pr2_param pr2param)
{
    vatek_result nres = rfmixer_r2_set_param(hchip,pr2param);
    if(is_vatek_success(nres))
        nres = rfmixer_start(hchip,cntl,pr2param->freqkhz);
    return nres;
}

vatek_result rfmixer_r2_stop(hvatek_chip hchip,uint32_t cntl)
{
    return rfmixer_stop(hchip,cntl);
}

void rfmixer_r2_reset_tune_table(Pr2_tune_handle pr2tune)
{
	typedef struct _r2fixed_item
	{
		uint32_t frequency;
		uint16_t gpio;
		uint16_t gain;
	}r2fixed_item;

	static const r2fixed_item r2_fixedtable[] =
	{
		{79000	,0x21,0x3033,},
		{135000	,0x2a,0x3033,},
		{255000	,0x33,0x3033,},
		{435000	,0x1E,0x3043,},
		{495000	,0x15,0x3043,},
		{598000	,0x15,0x3053,},
		{646000	,0x15,0x3063,},
		{695000	,0x15,0x3073,},
		{750000	,0x15,0x3083,},
		{808000	,0x15,0x3083,},
		{900000	,0x0C,0x3093,},
		{950000	,0x0C,0x30A3,},
	};

    #define R2_TABLE_LEN	(sizeof(r2_fixedtable)/sizeof(r2fixed_item))
    int32_t i = 0;
    memset(pr2tune, 0, sizeof(r2_tune_handle));
	
    pr2tune->items_num = R2_TABLE_LEN;
    pr2tune->r2_flags_0 = 0x00000001;
    pr2tune->r2_flags_1 = 0x00000000;
    pr2tune->tag = R2_TUNE_HDR_TAG;

	for (i = 0; i < R2_TABLE_LEN; i++)
	{
		Pr2_tune_item pitem = &pr2tune->rules[i];
		pitem->freq = r2_fixedtable[i].frequency;
		pitem->tag = R2_TUNE_ITEM_TAG;
        memset(&pitem->rule[0], 0, sizeof(r2_tune_rule));
		pitem->rule[0].gpiocntl = r2_fixedtable[i].gpio;
		pitem->rule[0].pagain = r2_fixedtable[i].gain;
        memset(&pitem->rule[1], 0, sizeof(r2_tune_rule));
		pitem->rule[1].gpiocntl = r2_fixedtable[i].gpio;
		pitem->rule[1].pagain = r2_fixedtable[i].gain;
	}
}

vatek_result rfmixer_r2_add_tune_table(Pr2_tune_handle pr2tune, uint32_t freq, Pr2_tune_item* pitem)
{
    int32_t index = 0;
    if (pr2tune->items_num >= R2_TUNE_NUMS)return vatek_badstatus;
    else if (pr2tune->items_num == 0) {
        index = pr2tune->items_num;
    }
    else if (pr2tune->rules[pr2tune->items_num - 1].freq < freq) {
        index = pr2tune->items_num;
    }
    else
    {
        int32_t i = 0;
        for (i = 0; i < pr2tune->items_num; i++)
        {
            if (pr2tune->rules[i].freq == freq)
            {
                *pitem = &pr2tune->rules[i];
                return vatek_success;
            }
            else if (pr2tune->rules[i].freq < freq && pr2tune->rules[i + 1].freq > freq)
            {
                uint8_t* psource = (uint8_t*)&pr2tune->rules[i + 1];
                uint8_t* pdest = (uint8_t*)&pr2tune->rules[i + 2];
                int32_t nlen = (pr2tune->items_num - (i + 1)) * sizeof(r2_tune_item);
                index = i + 1;
                memcpy(pdest, psource, nlen);
                memset(psource, 0, sizeof(r2_tune_item));
                break;
            }
            else if (freq < pr2tune->rules[i].freq)
            {
                uint8_t* psource = (uint8_t*)&pr2tune->rules[i];
                uint8_t* pdest = (uint8_t*)&pr2tune->rules[i + 1];
                int32_t nlen = (pr2tune->items_num - (i)) * sizeof(r2_tune_item);
                index = i;
                memcpy(pdest, psource, nlen);
                memset(psource, 0, sizeof(r2_tune_item));
                break;
            }
        }
    }

	pr2tune->rules[index].freq = freq;
	pr2tune->rules[index].tag = R2_TUNE_ITEM_TAG;
    pr2tune->items_num++;
	*pitem = &pr2tune->rules[index];

    return (vatek_result)1;
}

vatek_result rfmixer_r2_del_tune_table(Pr2_tune_handle pr2tune, uint32_t freq)
{
    if (!pr2tune->items_num)return vatek_badstatus;
    else if (pr2tune->items_num == 1)memset(&pr2tune->rules[0], 0, sizeof(r2_tune_item));
    else if (pr2tune->rules[pr2tune->items_num - 1].freq == freq)memset(&pr2tune->rules[pr2tune->items_num - 1], 0, sizeof(r2_tune_item));
    else
    {
		int32_t index = -1;
		int32_t i = 0;

		for (i = 0; i < pr2tune->items_num - 1; i++)
		{
			if (pr2tune->rules[i].freq == freq)
			{
				index = i;
				break;
			}
		}

		if (index == -1)return vatek_badparam;
		else
		{
			int32_t lne = sizeof(r2_tune_item) * (pr2tune->items_num - (index));
			memcpy(&pr2tune->rules[index], &pr2tune->rules[index + 1], lne);
		}
    }
    pr2tune->items_num--;
    return vatek_success;
}

uint16_t rfmixer_r2_int2tune(int32_t val, int32_t idx)
{
	uint16_t res = 0;
	if (val < 0)res |= r2_value_ranges[idx][R2_VALIDX_SIGN];
	val = abs(val);
	res |= val & r2_value_ranges[idx][R2_VALIDX_MASK];
	return res;
}

int32_t rfmixer_r2_tune2int(uint16_t val, int32_t idx)
{
	int32_t res = val & r2_value_ranges[idx][R2_VALIDX_MASK];
	if ((val & r2_value_ranges[idx][R2_VALIDX_SIGN]) != 0)
		res = -res;
	return res;
}