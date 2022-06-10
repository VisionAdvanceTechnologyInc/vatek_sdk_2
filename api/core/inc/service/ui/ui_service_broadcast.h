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

#ifndef _UI_SERVICE_BROADCAST_
#define _UI_SERVICE_BROADCAST_

#include <service/ui/ui_service_base.h>
#include <service/service_broadcast.h>

/* stream : common*/
HALREG_S(MUX_PCR_PID, 13)
HALREG_S(MUX_PADDING_PID, 13)
HALREG_S(MUX_BITRATE, 32)
HALREG_S(MUX_STUFFING_PID, 13)
HALREG_S(MUX_FLAGS, 32)

_ui_flags_start(mux_param, en_function)
	//_val(MUX_FLAG_EN_PSIP, en_psip)
	//_val(MUX_FLAG_DISABLE_PCR, en_disable_pcr)
_ui_flags_end

_ui_struct_start(mux_param)
	_prop_h16(mux_param, pcr_pid, "mux - pcr pid", MUX_PCR_PID)
	_prop_h16(mux_param, padding_pid, "mux - padding pid", MUX_PADDING_PID)
	_prop_u32(mux_param, bitrate, "mux - bitrate (bps)", MUX_BITRATE)
	_prop_u16(mux_param, stuffing_pid, "mux - suffing pid", MUX_STUFFING_PID)
	_prop_flag(mux_param, en_function, "mux - functions", MUX_FLAGS)
_ui_struct_end

#endif
