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

#include <core/base/chip_define.h>
//#include <cross/cross_os_api.h>

chip_status chip_status_get(hvatek_chip hchip,uint32_t* errcode)
{
    uint32_t val = 0;
	vatek_result nres = readhal(HALREG_SYS_STATUS_0,&val);
	chip_status chipstatus = chip_status_fail_hw;
	if (is_vatek_success(nres))
	{
		int32_t i = 0;
        val &= SYS_STATUS_TAG | 0xFF;

		for (i = 0; i < STATUS_TAGS_NUMS; i++)
		{
			if (val == chip_status_tags[i])
			{
				chipstatus = (chip_status)(i + chip_status_last);
                if (!is_chip_status_valid(chipstatus))
                {
                    if(errcode)nres = readhal(HALREG_SYS_ERRCODE, errcode);
                }
                    
				break;
			}
		}
	}

    if(!is_vatek_success(nres))
    {
        if(errcode)*errcode = SERVICE_HW_FAIL;
        chipstatus = chip_status_fail_hw;
    }
	return chipstatus;
}

vatek_result chip_status_set(hvatek_chip hchip,chip_status status,uint32_t errcode)
{
	vatek_result nres = vatek_badparam;
	static chip_status prev_status = chip_status_unknown;
	static uint32_t status_ticks = 0;

	if (is_current_chip_status(status))
	{
		uint32_t value = chip_status_tags[status - chip_status_last];
		nres = vatek_success;
		if (prev_status != status)
        {
            status_ticks = 0;
            prev_status = status;
        }
		else if (status == chip_status_running)value |= ((status_ticks++) & 0xFFFF) << 0;
        else if(status == chip_status_unknown)nres = vatek_success;

		if(is_vatek_success(nres))
        {
            nres = writehal(HALREG_SYS_STATUS_0, value);
            if(is_vatek_success(nres))
                nres = writehal(HALREG_SYS_ERRCODE,errcode);
        }
	}
	return nres;
}

vatek_result chip_status_check(hvatek_chip hchip, chip_status status)
{
    vatek_result nres = vatek_success;
    chip_status cstatus = chip_status_get(hchip,NULL);
	if (!is_chip_status_valid(cstatus))nres = vatek_hwfail;
	else if (cstatus != status)nres = vatek_badstatus;
	return nres;
}

vatek_result chip_info_reset(Pchip_info pinfo, hal_service_mode service, vatek_ic_module icmodule)
{
    typedef struct _ic_info
	{
		vatek_ic_module chip;
		uint32_t input;
		uint32_t output;
	}ic_info;

	const ic_info ic_tables[] =
	{
		{ic_module_a1       ,A1_EN_IN       ,A1_EN_OUT,},
		{ic_module_a3       ,A3_EN_IN       ,A3_EN_OUT,},
		{ic_module_b1       ,B1_EN_IN       ,B1_EN_OUT,},
		{ic_module_b2       ,B2_EN_IN       ,B2_EN_OUT,},
		{ic_module_b3_lite  ,B3_EN_IN       ,B3_EN_OUT,},
		{ic_module_b3_plus  ,B3PLUS_EN_IN   ,B3PLUS_EN_OUT,},
		{ic_module_b2_plus  ,B2_EN_IN       ,B2_EN_OUT,},
		{ic_module_e1       ,B2_EN_IN       ,E1_EN_OUT,},
	};

	#define IC_NUMS (sizeof(ic_tables)/sizeof(ic_info))

	int32_t i = 0;
    int32_t found = 0;

	memset(pinfo, 0, sizeof(chip_info));
	pinfo->chip_module = ic_module_unknown;
	pinfo->status = chip_status_badstatus;

	for (i = 0; i < IC_NUMS; i++)
	{
		if (icmodule == ic_tables[i].chip)
		{
			pinfo->chip_module = icmodule;
			pinfo->input_support = ic_tables[i].input;
			pinfo->output_support = ic_tables[i].output;
            found = 1;
			break;
		}
	}

    if (found)
    {
		pinfo->hal_service = service;
		pinfo->status = chip_status_waitcmd;
		return vatek_success;
    }
    return vatek_badparam;
}

vatek_result chip_info_get(hvatek_chip hchip,Pchip_info pinfo)
{
    uint32_t errcode = 0;
	chip_status status = chip_status_get(hchip,&errcode);
    vatek_result nres = vatek_success;

    memset(pinfo, 0, sizeof(chip_info));
    pinfo->status = status;
    pinfo->errcode = errcode;

    if(is_current_chip_status(status))
    {
        uint32_t val = 0;
        nres = readhal(HALREG_CHIP_ID, &val);
        if (is_vatek_success(nres))
        {
            pinfo->chip_module = ic_module_unknown;
            if (val == HAL_CHIPID_A1)pinfo->chip_module = ic_module_a1;
            else if (val == HAL_CHIPID_A3)pinfo->chip_module = ic_module_a3;
            else if (val == HAL_CHIPID_B1)pinfo->chip_module = ic_module_b1;
            else if (val == HAL_CHIPID_B2)pinfo->chip_module = ic_module_b2;
            else if (val == HAL_CHIPID_B3)pinfo->chip_module = ic_module_b3_lite;
            else if(val == HAL_CHIPID_B3_PLUS)pinfo->chip_module = ic_module_b3_plus;
            else if(val == HAL_CHIPID_B2_PLUS)pinfo->chip_module = ic_module_b2_plus;
            else if(val == HAL_CHIPID_E1)pinfo->chip_module = ic_module_e1;
            else nres = vatek_hwfail;

            if(is_vatek_success(nres))
            {
                nres = readhal(HALREG_SERVICE_MODE, &val);
                if (is_vatek_success(nres))
                {
                    pinfo->hal_service = service_unknown;
                    if (val == SERVICE_TAG_RESCUE)pinfo->hal_service = service_rescue;
                    else if (val == SERVICE_TAG_BROADCAST)pinfo->hal_service = service_broadcast;
                    else if (val == SERVICE_TAG_TRANSFORM)pinfo->hal_service = service_transform;
                    else nres = vatek_hwfail;
                }
            }

            if(is_vatek_success(nres))
            {
                nres = readhal(HALREG_FW_VER, &pinfo->version);
                if (is_vatek_success(nres))readhal(HALREG_PERIPHERAL_EN, &pinfo->peripheral_en);
                if (is_vatek_success(nres))readhal(HALREG_INPUT_SUPPORT, &pinfo->input_support);
                if (is_vatek_success(nres))readhal(HALREG_OUTPUT_SUPPORT, &pinfo->output_support);
            }
        }
    }

    if(!is_vatek_success(nres))
        pinfo->status = chip_status_fail_hw;
	return nres;
}

vatek_result chip_info_set(hvatek_chip hchip,Pchip_info pinfo)
{
	vatek_result nres = chip_status_set(hchip, pinfo->status,pinfo->errcode);
	if (is_vatek_success(nres))
	{
		uint32_t val = 0;
		if (pinfo->chip_module == ic_module_a1)val = HAL_CHIPID_A1;
		else if (pinfo->chip_module == ic_module_a3)val = HAL_CHIPID_A3;
		else if (pinfo->chip_module == ic_module_b1)val = HAL_CHIPID_B1;
		else if (pinfo->chip_module == ic_module_b2)val = HAL_CHIPID_B2;
		else if (pinfo->chip_module == ic_module_b3_lite)val = HAL_CHIPID_B3;
		else if (pinfo->chip_module == ic_module_b3_plus)val = HAL_CHIPID_B3_PLUS;
		else if (pinfo->chip_module == ic_module_b2_plus)val = HAL_CHIPID_B2_PLUS;
		else if (pinfo->chip_module == ic_module_e1)val = HAL_CHIPID_E1;
		else nres = vatek_badparam;

        if(is_vatek_success(nres))
        {
            nres = writehal(HALREG_CHIP_ID, val);
            if (is_vatek_success(nres))
            {
                val = 0;
                if (pinfo->hal_service == service_rescue)val = SERVICE_TAG_RESCUE;
                else if (pinfo->hal_service == service_broadcast)val = SERVICE_TAG_BROADCAST;
                else if (pinfo->hal_service == service_transform)val = SERVICE_TAG_TRANSFORM;
                else nres = vatek_badparam;
            }
        }

        if(is_vatek_success(nres))
        {
            nres = writehal(HALREG_SERVICE_MODE, val);
            if (is_vatek_success(nres))nres = writehal(HALREG_FW_VER, pinfo->version);
            if (is_vatek_success(nres))nres = writehal(HALREG_PERIPHERAL_EN, pinfo->peripheral_en);
            if (is_vatek_success(nres))nres = writehal(HALREG_INPUT_SUPPORT, pinfo->input_support);
            if (is_vatek_success(nres))nres = writehal(HALREG_OUTPUT_SUPPORT, pinfo->output_support);
        }
	}
	return nres;
}

vatek_result chip_raise_command(hvatek_chip hchip,uint32_t cmdaddr,uint32_t cmd,int32_t iswait)
{
    uint32_t val = 0;
    vatek_result nres = readhal(cmdaddr,&val);
    if(is_vatek_success(nres))
    {
        if(val != 0)nres = vatek_badstatus;
        else
        {
            nres = writehal(cmdaddr,cmd);
            if (iswait)
            {
                uint32_t tick = vatek_get_tick_ms();
                int32_t is_timeout = 1;
                while ((vatek_get_tick_ms() - tick) < CHIP_RAISE_COMMAND_TIMEOUT)
                {
                    vatek_sleep_ms(500);
                    nres = readhal(cmdaddr, &val);
                    if (!is_vatek_success(nres) || val == 0)
                    {
                        is_timeout = 0;
                        break;
                    }
                }
                if (is_timeout)nres = vatek_timeout;
            }
        }
    }
    return nres;
}

vatek_result chip_check_command(hvatek_chip hchip, uint32_t cmdaddr)
{
    uint32_t val = 0;
    vatek_result nres = readhal(cmdaddr, &val);
    if (is_vatek_success(nres))
    {
        if (val != 0)nres = vatek_badstatus;
    }
    return nres;
}

vatek_result chip_result_command(hvatek_chip hchip, uint32_t resultaddr, uint32_t* errcode)
{
    return readhal(resultaddr, errcode);
}

vatek_result chip_send_command(hvatek_chip hchip, uint32_t cmd, uint32_t cmdaddr, uint32_t resultaddr)
{
    vatek_result nres = chip_raise_command(hchip, cmdaddr, cmd, 1);
    if (is_vatek_success(nres))
    {
        uint32_t err = 0;
        nres = chip_result_command(hchip, resultaddr, &err);
        if (is_vatek_success(nres))
        {
            if (err & HALREG_ERR_TAG)nres = vatek_hwfail;
        }
    }
    return nres;
}
