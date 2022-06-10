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

#ifndef _HALREG_RFMIXER_
#define _HALREG_RFMIXER_

#define RFMIXER_CMD_START				0x00001000
#define RFMIXER_CMD_STOP				0x00002000

#define HALREG_RFMIXER_ADDR_START       0x1200			/* used with different rf_mixer define		*/
#define HALREG_RFMIXER_ADDR_END         0x1600


#define HALREG_RF_COM_STATUS            0x1200			
	#define RF_STATUS_IDLE				0x00000001
	#define RF_STATUS_ACTIVE			0x00000002
	#define RF_STATUS_FAIL				0x80000000

#define HALREG_RF_COM_FREQ              0x1202			/* param used with rfmixer_cmd_start (KHz)	*/

#define R2MIXER_CMD_R2_TUNE			    0x00010000

/* r2 function flag define */
#define R2FLAG_I_PN_SWAP			    0x00000010
#define R2FLAG_Q_PN_SWAP			    0x00000001
#define R2FLAG_EN_CALIBRATION			0x08000000

#define HALREG_R2_EN_CNTL			    0x1208
	#define R2_EN_TAG_BASE			    0xA5FF0000
	#define R2_EN_PATH_0			    0xA5FF0000		/* used default param with path_0	*/
	#define R2_EN_PATH_1			    0xA5FF0001		/* used default param with path_1	*/
	#define R2_EN_PARAM				    0xA5FF0002		/* used customized param			*/

#define HALREG_R2_FLAGS				    0x1209
	
#define HALREG_R2_PARAM_I               0x120A
#define HALREG_R2_PARAM_Q               0x120B
#define HALREG_R2_PARAM_IMAGE           0x120C
#define HALREG_R2_PARAM_PHASE           0x120D
#define HALREG_R2_PARAM_GPIO            0x120E
#define HALREG_R2_PARAM_GAIN            0x120F

#endif
