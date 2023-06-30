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

#ifndef _HALREG_CALIBRATION_
#define _HALREG_CALIBRATION_

#define HALREG_CALIBRATION_CNTL			0x1220
	#define CALIBRATION_EN_TAG			0x78122900
	#define CALIBRATION_APPLY			0x00000001
	
#define HALREG_CALIBRATION_FUNCTION		0x1221
	#define CALIBRATION_EN_R2			0x00000001

#define HALREG_CALIBRATION_CLOCK		0x1228
#define HALREG_CALIBRATION_DAC			0x1229

#define HALREG_CALIBRATION_R2_0_I		0x1280
#define HALREG_CALIBRATION_R2_0_Q		0x1281
#define HALREG_CALIBRATION_R2_0_IMAGE	0x1282
#define HALREG_CALIBRATION_R2_0_PHASE	0x1283

#define HALREG_EXT_R2_GAIN               0x1290
	#define EXT_R2_GAIN_EN_TAG           0x45585400
	#define EXT_R2_GAIN_MASK             0x0000000F
	#define EXT_R2_GAIN_EN_READ          0x45585800

#endif
