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

#ifndef _UI_SERVICE_BASE_
#define _UI_SERVICE_BASE_

#include <core/ui/ui_props_define.h>
#include <service/service_base.h>
#include <core/ui/ui_props/ui_props_stream.h>
#include <core/ui/ui_props/ui_props_output.h>

HALREG_S(BCINFO_STREAM	, 8	)
HALREG_S(BCINFO_OUTPUT	, 8	)
HALREG_S(BCINFO_STATUS	, 32)
HALREG_S(BCINFO_MODRATE	, 32)
HALREG_S(BCINFO_MUXRATE	, 32)
HALREG_S(BCINFO_CURRATE	, 32)
HALREG_S(BCINFO_DATARATE, 32)
HALREG_S(BCINFO_QUERYBUF, 32)

_ui_enum_start(broadcast_status)
	_val(bcstatus_fail_unknown, err_unknown)
	_val(bcstatus_fail_source, err_source)
	_val(bcstatus_fail_timeout, err_timeout)
	_val(bcstatus_fail_drop, err_drop)
	_val(bcstatus_fail_buffer, err_buffer)
	_val(bcstatus_fail_muxer, err_muxer)
	_val(bcstatus_fail_encode, err_encoder)
	_val(bcstatus_fail_media, err_media)
	_val(bcstatus_fail_demux, err_demux)
	_val(bcstatus_idle, idle)
	_val(bcstatus_wait_source, wait_source)
	_val(bcstatus_broadcast, broadcast)
	_val(bcstatus_finish, finish)
_ui_enum_end

_ui_struct_start(broadcast_info)
	_prop_enum(broadcast_info, stream_source, source, "info - stream source", BCINFO_STREAM)
	_prop_enum(broadcast_info, broadcast_status, status, "info - broadcast status", BCINFO_STATUS)
	_prop_u32(broadcast_info, mod_bitrate, "info - modulator bitrate", BCINFO_MODRATE)
	_prop_u32(broadcast_info, mux_bitrate, "info - mux bitrate", BCINFO_MUXRATE)
	_prop_u32(broadcast_info, cur_bitrate, "info - current bitrate", BCINFO_CURRATE)
	_prop_u32(broadcast_info, data_bitrate, "info - data bitrate", BCINFO_DATARATE)
	_prop_u32(broadcast_info, query_buffer,"info - query buffer", BCINFO_QUERYBUF)
_ui_struct_end

#endif
