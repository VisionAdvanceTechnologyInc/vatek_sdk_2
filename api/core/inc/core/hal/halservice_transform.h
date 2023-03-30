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

#ifndef _HALSERVICE_TRANSFORM_
#define _HALSERVICE_TRANSFORM_

#include <core/hal/halservice_rescure.h>

#define HALREG_TRANSFORM_CNTL				HALREG_SERVICE_BASE_CNTL				/* inheritance HALREG_BROADCAST_CNTL 						*/
    #define TR_START                    	BASE_CMD_START
    #define TR_STOP                     	BASE_CMD_STOP
	#define TR_START_SINE					BASE_CMD_TEST_START_SINE
	#define TR_START_TEST					BASE_CMD_TEST_START

    #define TR_REBOOT                   	BASE_CMD_REBOOT
    #define TR_REBOOT_RESCURE           	BASE_CMD_REBOOT_RESCURE

    #define TR_RFMIXER_START            	RFMIXER_CMD_START
    #define TR_RFMIXER_STOP             	RFMIXER_CMD_STOP

	#define TR_CALIBRATION_SAVE				BASE_CMD_CALIBRATION_SAVE

#define HALREG_TRANSFORM_MODE				0x601
	#define TRANSFORM_ENUM					0x00000003
	#define TRANSFORM_CAPTURE				0x00000004
	#define TRANSFORM_BROADCAST				0x00000005

#define HALREG_TRANSFORM_INPUT				HALREG_SERVICE_INPUT

#define HALREG_TRCAPTURE_PID				0x608				/* capture packet put start at HALRANGE_PLAYLOAD_START		*/
#define HALREG_TRCAPTURE_TIMEOUT			0x609
#define HALREG_TRCAPTURE_SECTION_NUM		0x60A
#define HALREG_TRCAPTURE_PACKET_NUMS		0x60B

#define HALREG_TRENUM_TIMEOUT				0x608
#define HALREG_TRADJUST_TICK                0x60C
#define HALREG_TRADJUST_SYMBOLRATE          0x60D

/* extended information	*/

#define HALREG_TRINFO_MODE					0x628
#define HALREG_TRINFO_PACKETNUMS			0x629
#define HALREG_TRINFO_TICK_HI               0x62A
#define HALREG_TRINFO_TICK_LOW              0x62B

#define HALREG_TRINFO_ADJUST_WARN			0x62C
#define HALREG_TRINFO_WARN_CNT				0x62D
	#define HDR_ERR_CNT_MASK				0xF

#endif
