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

#ifndef _UI_PROPS_STREAM_
#define _UI_PROPS_STREAM_

#include <core/base/stream_base.h>
#include <core/ui/ui_props/ui_props_encoder.h>

/* stream : usb stream mode*/
HALREG_S(USB_INPUT_MODE,8)
HALREG_S(USB_STREAM_FLAGS,32)
HALREG_S(USB_PCR_MODE, 8)
/* stream : ts_in*/
HALREG_S(TSIN_MODE, 8)
HALREG_S(TSIN_FLAGS, 32)
HALREG_S(TSIN_PCR_MODE,8)

_ui_enum_start(stream_source)
	_val(stream_source_tsin, tsin)
	_val(stream_source_usb, usb)
	_val(stream_source_encoder, encoder)
	_val(stream_source_sine,sinewave)
	_val(stream_source_test,test)
_ui_enum_end

/* struct tsin param*/
_ui_enum_start(stream_mode)
	_val(stream_smooth, Smooth)
	_val(stream_remux, Remux)
	_val(stream_passthrogh,Passthrough)
_ui_enum_end

_ui_enum_start(pcr_adjust_mode)
	_val(pcr_disable,disable)
	_val(pcr_adjust,adjust)
	_val(pcr_retag,retag)
_ui_enum_end

_ui_flags_start(tsin_param, ts_flags)
	_val(TS_PIN_SERIAL, en_tsin_serial)
	_val(TS_PIN_NEGATIVE_CLK, clk_negative)
	_val(TS_PIN_NEGATIVE_VALID, valid_negative)
	_val(TS_PIN_NEGATIVE_SYNC, sync_negative)
_ui_flags_end

_ui_struct_start(tsin_param)
	_prop_enum(tsin_param, stream_mode, streammode, "tsin - stream mode", TSIN_MODE)
	_prop_flag(tsin_param, ts_flags, "tsin - function flags", TSIN_FLAGS)
	_prop_enum(tsin_param, pcr_adjust_mode,pcrmode,"tsin - pcr mode",TSIN_PCR_MODE)
_ui_struct_end

_ui_flags_start(usb_param, usb_flags)
_ui_flags_end

_ui_struct_start(usb_param)
	_prop_enum(usb_param, stream_mode, mode, "usb - input mode", USB_INPUT_MODE)
	_prop_flag(usb_param, usb_flags,"usb - function flags",USB_STREAM_FLAGS)
	_prop_enum(usb_param, pcr_adjust_mode, pcrmode, "usb - pcr mode", USB_PCR_MODE)
_ui_struct_end

#endif
