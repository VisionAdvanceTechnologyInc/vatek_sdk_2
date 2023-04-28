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

#ifndef _UI_PROPS_CHIP_
#define _UI_PROPS_CHIP_

#include <core/base/chip_define.h>
#include <core/base/binary_rom.h>
#include <service/service_base.h>
#include <core/ui/ui_props_define.h>
#include <core/hal/halservice_base.h>

_ui_enum_start(chip_status)
    _val(chip_status_badstatus,badstatus)
    _val(chip_status_fail_hw,fail_hw)
    _val(chip_status_fail_service,fail_service)
    _val(chip_status_fail_loader,fail_loader)
    _val(chip_status_unknown,unknown)
    _val(chip_status_waitcmd,waitcmd)
    _val(chip_status_running,running)
_ui_enum_end

_ui_enum_start(vatek_ic_module)
    _val(ic_module_nodevice,ic_nodevice)
    _val(ic_module_a1,a1)
    _val(ic_module_b1,b1)
    _val(ic_module_b2,b2)
    _val(ic_module_a3,a3)
    _val(ic_module_b3_lite,b3_lite)
    _val(ic_module_b3_plus,b3_plus)
    _val(ic_module_b2_plus,b2_plus)
    _val(ic_module_unknown,ic_unknown)
_ui_enum_end

_ui_enum_start(hal_service_mode)
    _val(service_unknown,unknown)
    _val(service_rescue,rescue)
    _val(service_broadcast,broadcast)
    _val(service_transform,transform)
_ui_enum_end

_ui_flags_start(chip_info,input_support)
    _val(INPUT_EN_TEST,test)
    _val(INPUT_EN_USB,usb)
    _val(INPUT_EN_TS,ts_in)
    _val(INPUT_EN_ENC,enc)
    _val(VENC_EN_MPEG2,mpeg2)
    _val(VENC_EN_H264,h264)
    _val(VENC_EN_HEVC,hevc)
    _val(VENC_EN_FULLHD,fullhd)
    _val(AENC_EN_MP1_L2,mp1_l2)
    _val(AENC_EN_AAC_LC_ADTS,aac_adts)
    _val(AENC_EN_AC_3,ac3)
    _val(AENC_EN_AAC_LC_LATM,aac_latm)
_ui_flags_end

_ui_flags_start(chip_info, peripheral_en)
    _val(PERIPHERAL_FINTEKR2,r2)
    _val(PERIPHERAL_EP9555E,ep9555e)
    _val(PERIPHERAL_LICENSED,licensed)
_ui_flags_end

_ui_flags_start(chip_info, output_support)
    _val(OUTPUT_EN_MOD,mod)
    _val(OUTPUT_EN_SINEWAVE,sinewave)
    _val(OUTPUT_EN_TS,ts_out)
    _val(OUTPUT_EN_USB,usb)
    _val(OUTPUT_EN_PCIE,pci_e)
    _val(MOD_EN_DVB_T,dvb_t)
    _val(MOD_EN_J83_A,j83a)
    _val(MOD_EN_ATSC,atsc)
    _val(MOD_EN_J83_B,j83b)
    _val(MOD_EN_DTMB,dtmb)
    _val(MOD_EN_ISDB_T,isdb_t)
    _val(MOD_EN_J83_C,j83c)
    _val(MOD_EN_DVB_T2,dvb_t2)
_ui_flags_end

_ui_struct_start(chip_info)
    _prop_enum(chip_info,chip_status,status,"info - chip status",NULLHAL)
    _prop_h32(chip_info,errcode,"info - err code",NULLHAL)
    _prop_enum(chip_info,vatek_ic_module,chip_module,"info - chip id",NULLHAL)
    _prop_enum(chip_info,hal_service_mode,hal_service,"info - hal service",NULLHAL)
    _prop_h32(chip_info,version,"info - version",NULLHAL)
    _prop_flag(chip_info, peripheral_en, "info - peripheral", NULLHAL)
    _prop_flag(chip_info, input_support,"info - input supported", NULLHAL)
    _prop_flag(chip_info, output_support,"info - output supported", NULLHAL)
_ui_struct_end

_ui_flags_start(chip_config,functions)
	_val(CHIP_EN_DAC_EXTR,en_extR)
    _val(CHIP_EN_STATUSLED,en_statusled)
_ui_flags_end

_ui_struct_start(chip_config)
	_prop_flag(chip_config,functions,"chip enable function",NULLHAL)
    _prop_u32(chip_config, status_led,"chip status led",NULLHAL)
_ui_struct_end


#endif
