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

#ifndef _UI_MUX_SPEC_DVB_
#define _UI_MUX_SPEC_DVB_

#include <mux/ui/ui_props_mux.h>
#include <mux/spec/spec_dvb.h>

_mux_string_start(mux_spec_iso)
_mux_string_end

_mux_string_start(mux_spec_dvb)
	_mux_string(0, network_name, _MUX_STRING_TYPE_ASCII, 16)
	_mux_string(1, service_name, _MUX_STRING_TYPE_ASCII, 16)
_mux_string_end

/* _dvb channel */
_mux_ui_struct_start(mux_spec_dvb_channel)
	_mux_h16(mux_spec_dvb_channel, transport_stream_id, "transport_stream_id")
	_mux_h16(mux_spec_dvb_channel, network_id, "network_id")
	_mux_str(mux_spec_dvb_channel, network_name, "network_name",0)
_mux_ui_struct_end(mux_spec_dvb_channel, mux_ui_level_channel)

/* _dvb program */
_mux_ui_struct_start(mux_spec_dvb_program)
	_mux_h16(mux_spec_dvb_program, original_network_id, "original_network_id")
	_mux_h16(mux_spec_dvb_program, program_number, "program_number")
	_mux_h16(mux_spec_dvb_program, channel_no, "channel_no")
	_mux_str(mux_spec_dvb_program, service_name,"service_name",1)
_mux_ui_struct_end(mux_spec_dvb_program, mux_ui_level_program)



#endif
