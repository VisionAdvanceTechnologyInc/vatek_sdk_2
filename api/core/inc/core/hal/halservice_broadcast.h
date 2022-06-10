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

#ifndef _HALSERVICE_BROADCAST_
#define _HALSERVICE_BROADCAST_

#include <core/hal/halreg_mux.h>
#include <core/hal/halservice_rescure.h>

#define HALREG_BROADCAST_CNTL           HALREG_SERVICE_BASE_CNTL
    #define BC_START                    BASE_CMD_START
    #define BC_STOP                     BASE_CMD_STOP
	#define BC_START_SINE				BASE_CMD_TEST_START_SINE
	#define BC_START_TEST				BASE_CMD_TEST_START

    #define BC_REBOOT                   BASE_CMD_REBOOT
    #define BC_REBOOT_RESCURE           BASE_CMD_REBOOT_RESCURE

    #define BC_RFMIXER_START            RFMIXER_CMD_START
    #define BC_RFMIXER_STOP             RFMIXER_CMD_STOP
			
	#define BC_CALIBRATION_SAVE         BASE_CMD_CALIBRATION_SAVE

#define HALREG_BROADCAST_MODE			0x601
	#define RUNMODE_BROADCAST			0x00000003

#define HALREG_BROADCAST_STREAM			HALREG_SERVICE_INPUT

/**
 * @brief Encoder Extend Data Interface
 * 
 */
#define HALREG_ENC_EXTDATA				0x668
	#define ENC_EXTDATA_TAG				0xF8880000
	#define ENC_EXTDATA_LEN_MASK		0x0000FFFF

#define HALREG_EN_EXTDATA_START			0x670
	#define ENC_EXTDATA_MAXLEN			64 

#endif
