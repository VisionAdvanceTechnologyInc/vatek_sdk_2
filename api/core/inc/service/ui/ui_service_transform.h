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

#ifndef _UI_SERVICE_TRANSFORM_
#define _UI_SERVICE_TRANSFORM_

#include <core/ui/ui_props/ui_props_chip.h>
#include <service/ui/ui_service_base.h>
#include <service/service_transform.h>

_ui_enum_start(transform_mode)
	_val(trmode_enum,enum)
	_val(trmode_capture, capture)
	_val(trmode_broadcast, broadcast)
_ui_enum_end

HALREG_S(TRENUM_TIMEOUT, 32)
HALREG_S(TRANSFORM_INPUT, 16)

HALREG_S(TRCAPTURE_PID, 16)
HALREG_S(TRCAPTURE_TIMEOUT, 16)
HALREG_S(TRCAPTURE_SECTION_NUM, 16)
HALREG_S(TRCAPTURE_PACKET_NUMS, 16)

_ui_struct_start(transform_enum)
	_prop_enum(transform_enum, stream_source, source, "stream source", TRANSFORM_INPUT)
	_prop_u16(transform_enum, timeout, "process timeout [ms]", TRENUM_TIMEOUT)
_ui_struct_end

_ui_struct_start(transform_capture)
	_prop_enum(transform_capture, stream_source, source, "stream source", TRANSFORM_INPUT)
	_prop_h16(transform_capture, pid, "capture pid", TRCAPTURE_PID)
	_prop_u16(transform_capture, timeout, "capture timeout [ms]", TRCAPTURE_TIMEOUT)
	_prop_u16(transform_capture, section_num, "capture psi table section number", TRCAPTURE_SECTION_NUM)
	_prop_u16(transform_capture, packet_nums, "capture max packet number", TRCAPTURE_PACKET_NUMS)
_ui_struct_end

_ui_struct_start(transform_broadcast)
	_prop_enum(transform_capture, stream_source, source, "stream source", TRANSFORM_INPUT)
_ui_struct_end

#endif
