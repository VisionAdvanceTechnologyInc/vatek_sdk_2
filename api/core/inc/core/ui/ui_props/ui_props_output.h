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

#ifndef _UI_PROPS_OUTPUT_
#define _UI_PROPS_OUTPUT_

#include <core/base/output_base.h>
#include <core/base/output_rfmixer.h>
#include <core/base/calibration_define.h>
#include <core/ui/ui_props/ui_props_modulator.h>

HALREG_S(R2_PARAM_I, 16)
HALREG_S(R2_PARAM_Q, 16)
HALREG_S(R2_PARAM_IMAGE, 16)
HALREG_S(R2_PARAM_PHASE, 16)
HALREG_S(R2_PARAM_GPIO, 16)
HALREG_S(R2_PARAM_GAIN, 16)

HALREG_S(CALIBRATION_R2_0_I, 16)
HALREG_S(CALIBRATION_R2_0_Q, 16)
HALREG_S(CALIBRATION_R2_0_IMAGE, 16)
HALREG_S(CALIBRATION_R2_0_PHASE, 16)

_ui_struct_start(r2_tune)
_prop_u16(r2_tune, ioffset, "i offect", R2_PARAM_I)
	_prop_u16(r2_tune, qoffset, "q offect", R2_PARAM_Q)
	_prop_u16(r2_tune, imgoffset, "image offect", R2_PARAM_IMAGE)
	_prop_u16(r2_tune, phaseoffset, "phase offect", R2_PARAM_PHASE)
_ui_struct_end

_ui_struct_start(r2_tune_rule)
_prop_u16(r2_tune_rule, pagain, "gain", R2_PARAM_GAIN)
	_prop_u16(r2_tune_rule, gpiocntl, "gpio", R2_PARAM_GPIO)
_ui_struct_end

_ui_struct_start(r2_tune_calibration0)
	_prop_u16(r2_tune, ioffset, "i offect", CALIBRATION_R2_0_I)
	_prop_u16(r2_tune, qoffset, "q offect", CALIBRATION_R2_0_Q)
	_prop_u16(r2_tune, imgoffset, "image offect", CALIBRATION_R2_0_IMAGE)
	_prop_u16(r2_tune, phaseoffset, "phase offect", CALIBRATION_R2_0_PHASE)
_ui_struct_end

#endif
