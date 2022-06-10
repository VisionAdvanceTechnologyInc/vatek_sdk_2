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

#include <core/tools/tool_ofdm.h>
#include <core/tools/tool_8vsb.h>
#include <core/tools/tool_dvb.h>
#include <core/tools/tool_dtmb.h>
#include <core/tools/tool_isdb_t.h>
#include <core/tools/tool_j83abc.h>
#include <core/tools/tool_dvb_t2.h>

#include <core/ui/ui_props/ui_props_modulator.h>

#define _mparam_prop(type,tag)   {sizeof(tag),(const uint8_t*)&default_##tag,_ui_struct(tag),tool_##type##_get_bitrate,tool_##type##_check_param},
#define _mparam_j83(type,tag)    {sizeof(tag),(const uint8_t*)&default_##tag,_ui_struct(tag),tool_j83_get_bitrate,tool_j83_check_param},

static const ofdm_modulation modulations[modulator_mod_nums] =
{
	_mparam_prop(dvb_t,dvb_t_param)
	_mparam_j83(j83a,j83a_param)
	_mparam_prop(atsc,atsc_param)
	_mparam_j83(j83b,j83b_param)
	_mparam_prop(dtmb,dtmb_param)
	_mparam_prop(isdb_t,isdb_t_param)
	_mparam_j83(j83c,j83c_param)
	_mparam_prop(dvb_t2,dvb_t2_param)
};

vatek_result tool_ofdm_get_modulation(modulator_type type, Pofdm_modulation* pofdm)
{
	vatek_result nres = vatek_badparam;
	if (is_current_modulator(type))
	{
		*pofdm = (Pofdm_modulation)&modulations[type];
		nres = vatek_success;
	}
	return nres;
}