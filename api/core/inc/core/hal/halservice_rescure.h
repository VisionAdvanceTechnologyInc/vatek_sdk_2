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

#ifndef _HALSERVICE_RESCURE_
#define _HALSERVICE_RESCURE_

#include <core/hal/halservice_base.h>

#define HALREG_RESCUE_CNTL				0x31
	#define RESCUE_CNTL_TAG				0xFA000000
	#define RESCUE_CNTL_WRITE			0xFA010000
	#define RESCUE_CNTL_READ			0xFA020000
	#define RESCUE_CNTL_REBOOT			0xFA0300FB
	#define RESCUE_SECTION_MASK			0x0000FFFF
	#define RESCUE_CNTL_TAG_MASK		0xFF000000

#define IS_RESCUE_CNTL_VALID(val)		((val & RESCUE_CNTL_TAG_MASK) == RESCUE_CNTL_TAG)
#define IS_RESCUR_CMD(val,cmd)			((val & 0xFFFF0000) == cmd)

#define HALREG_RESCUE_CRC32				0x32
#define HALREG_RESCUE_RESULT            0x33
	#define RESCUE_ERR_SUCCESS          0x00000000
	#define RESCUE_ERR_CRC              0x80000001
	#define RESCUE_ERR_SPIFLASH         0x80000002
	#define RESCUE_ERR_BADCNTL			0x80000003
	#define RESCUE_ERR_BADSTATUS		0x80000004

#define HALREG_APP_DEBUG                0x3E
	#define APPDBG_EN_PLAYLOAD			0x00000001

#define HALREG_SYS_DEBUG                0x3F

#define HALREG_RESCUE_DATA				0x3C00

#define HALREG_RESCUE_EN				0x4000
	#define RESCUE_EN_TAG				0xFF1229FF  /* next reboot enter rescue mode */
    
#endif
