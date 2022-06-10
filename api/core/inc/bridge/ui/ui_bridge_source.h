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

#ifndef UI_BRIDGE_SOURCE
#define UI_BRIDGE_SOURCE

#include <core/ui/ui_props_define.h>
#include <core/ui/ui_props/ui_props_encoder.h>
#include <bridge/bridge_source.h>

_ui_enum_start(bsource_scale)
	_val(sscale_bypass, bypass)
	_val(sscale_i_2_p, i2p)
	_val(sscale_fixed_480p, fixed_480p)
	_val(sscale_fixed_576p, fixed_576p)
	_val(sscale_fixed_720p, fixed_720p)
	_val(sscale_fixed_1080p, fixed_1080p)
_ui_enum_end

_ui_enum_start(bsource_baseclk)
	_val(sbaseclk_1000, clk_1000)
	_val(sbaseclk_1001, clk_1001)
_ui_enum_end

_ui_enum_start(adv718x_input)
	_val(adv718x_in_cvbs_ain_1, cvbs_ain_1)
	_val(adv718x_in_cvbs_ain_2, cvbs_ain_2)
	_val(adv718x_in_cvbs_ain_3, cvbs_ain_3)
	_val(adv718x_in_cvbs_ain_4, cvbs_ain_4)
	_val(adv718x_in_cvbs_ain_5, cvbs_ain_5)
	_val(adv718x_in_cvbs_ain_6, cvbs_ain_6)
	_val(adv718x_in_svideo_ain_1_2, svideo_ain_1_2)
	_val(adv718x_in_svideo_ain_3_4, svideo_ain_3_4)
	_val(adv718x_in_svideo_ain_5_6, svideo_ain_5_6)
	_val(adv718x_in_ypbpr_ain_1_2_3, ypbpr_ain_1_2_3)
	_val(adv718x_in_ypbpr_ain_4_5_6, ypbpr_ain_4_5_6)
_ui_enum_end

_ui_enum_start(adv718x_std)
	_val(adv718x_std_ad_pal_bg_ntsc_j_secam, ad_pal_bg_ntsc_j_secam)
	_val(adv718x_std_ad_pal_bg_ntsc_j_secam_ped, ad_pal_bg_ntsc_j_secam_ped)
	_val(adv718x_std_ad_pal_n_ntsc_j_secam, ad_pal_n_ntsc_j_secam)
	_val(adv718x_std_ad_pal_n_ntsc_m_secam, ad_pal_n_ntsc_m_secam)
	_val(adv718x_std_ntsc_j, ntsc_j)
	_val(adv718x_std_ntsc_m, ntsc_m)
	_val(adv718x_std_pal_60, pal_60)
	_val(adv718x_std_ntsc_443, ntsc_443)
	_val(adv718x_std_pal_bg, pal_bg)
	_val(adv718x_std_pal_n, pal_n)
	_val(adv718x_std_pal_m, pal_m)
	_val(adv718x_std_pal_m_ped, pal_m_ped)
	_val(adv718x_std_pal_comb_n, pal_comb_n)
	_val(adv718x_std_pal_comb_n_ped, pal_comb_n_ped)
	_val(adv718x_std_pal_secam, pal_secam)
	_val(adv718x_std_pal_secam_ped, secam_ped)
_ui_enum_end

#endif
