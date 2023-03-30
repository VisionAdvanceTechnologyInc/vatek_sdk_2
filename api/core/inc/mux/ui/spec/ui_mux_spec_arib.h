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

#ifndef _UI_MUX_SPEC_ARIB_
#define _UI_MUX_SPEC_ARIB_

#include <mux/ui/ui_props_mux.h>
#include <mux/spec/spec_arib.h>

_ui_enum_start(arib_abnt_copy)
	_val(arib_abnt_free, free)
	_val(arib_abnt_copyonce, copy_once)
	_val(arib_abnt_forbidden, forbidden)
_ui_enum_end

/* _arib string table */

_mux_string_start(mux_spec_arib)
	_mux_string(0, network_name, _MUX_STRING_TYPE_JIS_ENG, 20)
	_mux_string(1, ts_name, _MUX_STRING_TYPE_JIS_ENG, 20)
	_mux_string(2, service_name, _MUX_STRING_TYPE_JIS_ENG, 20)
_mux_string_end

_mux_string_start(mux_spec_abnt)
	_mux_string(0, network_name, _MUX_STRING_TYPE_JIS_ENG, 20)
	_mux_string(1, ts_name, _MUX_STRING_TYPE_JIS_ENG, 20)
	_mux_string(2, service_name, _MUX_STRING_TYPE_JIS_ENG, 20)
_mux_string_end

/* _arib channel and program */
_mux_ui_struct_start(mux_spec_arib_channel)
	_mux_u8(mux_spec_arib_channel, region_id, "region_id")
	_mux_u8(mux_spec_arib_channel, broadcaster_id, "broadcaster_id")
	_mux_u8(mux_spec_arib_channel, remote_control_key_id, "remote_control_key_id")
	_mux_str(mux_spec_arib_channel, network_name, "network_name", 0)
_mux_ui_struct_end(mux_spec_arib_channel, mux_ui_level_channel)

_mux_ui_struct_start(mux_spec_arib_program)
	_mux_u8(mux_spec_arib_program, service_no, "arib - service number")
	_mux_isolang(mux_spec_arib_program, main_lang, "arib - main_lang")
	_mux_isolang(mux_spec_arib_program, sub_lang, "arib - sub_lang")
	_mux_enum(mux_spec_arib_program, arib_abnt_copy, copy_flag, "copy_flag")
	_mux_str(mux_spec_arib_program, service_name, "service_name", 2)
	_mux_str(mux_spec_arib_program, ts_name, "ts_name", 1)
_mux_ui_struct_end(mux_spec_arib_program, mux_ui_level_program)

/* _abnt channel and program */

_mux_ui_struct_start(mux_spec_abnt_channel)
	_mux_u8(mux_spec_abnt_channel, remote_control_key_id, "remote_control_key_id")
	_mux_h16(mux_spec_abnt_channel, original_network_id, "original_network_id")
	_mux_h16(mux_spec_abnt_channel, area_code, "area_code")
	_mux_str(mux_spec_abnt_channel, network_name, "network_name", 0)
_mux_ui_struct_end(mux_spec_abnt_channel, mux_ui_level_channel)

_mux_ui_struct_start(mux_spec_abnt_program)
	_mux_u8(mux_spec_abnt_program, service_no, "arib - service number")
	_mux_isolang(mux_spec_abnt_program, main_lang, "arib - main_lang")
	_mux_isolang(mux_spec_abnt_program, sub_lang, "arib - sub_lang")
	_mux_enum(mux_spec_abnt_program, arib_abnt_copy, copy_flag, "copy_flag")
	_mux_str(mux_spec_abnt_program, service_name, "service_name", 2)
	_mux_str(mux_spec_abnt_program, ts_name, "ts_name", 1)
_mux_ui_struct_end(mux_spec_abnt_program, mux_ui_level_program)

#endif
