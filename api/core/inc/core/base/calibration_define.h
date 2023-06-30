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

#ifndef _CALIBRATION_DEFINE_
#define _CALIBRATION_DEFINE_

#include <core/hal/halreg_calibration.h>
#include <core/base/output_rfmixer.h>

typedef struct _calibration_dac
{
	int8_t ioffect;
	int8_t qoffect;
	int8_t igain;
	int8_t qgain;
}calibration_dac;

typedef calibration_dac* Pcalibration_dac;

typedef struct _calibration_param
{
	int32_t clock;
	calibration_dac dac;
	r2_tune r2;
	uint32_t r2_power;
	int dac_power;
}calibration_param;

typedef calibration_param* Pcalibration_param;

#ifdef __cplusplus
extern "C" {
#endif

	HAL_API vatek_result calibration_set(hvatek_chip hchip, Pcalibration_param pcalibration,int32_t isapply);
	HAL_API vatek_result calibration_check(hvatek_chip hchip);
	HAL_API vatek_result calibration_get(hvatek_chip hchip, Pcalibration_param pcalibration);
	HAL_API vatek_result calibration_adjust_gain(hvatek_chip hchip, int8_t gain, Pcalibration_param m_calibration);
#ifdef __cplusplus
}
#endif

#endif
