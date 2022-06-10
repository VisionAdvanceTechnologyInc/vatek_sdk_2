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

#ifndef _HALREG_MUX_
#define _HALREG_MUX_

/* mux common define */

#define MUX_STREAM_TYPE_VIDEO			0x00000000
#define MUX_STREAM_TYPE_AUDIO			0x00000001
#define MUX_STREAM_TYPE_DATA			0x00000002

#define MUX_SPEC_UNKNOWN				0x00000000
#define MUX_SPEC_ISO					0x00000001
#define MUX_SPEC_DVB					0x00000002
#define MUX_SPEC_PSIP					0x00000003

#define HALREG_MUX_PCR_PID				0x610				
#define HALREG_MUX_PADDING_PID			0x611				
#define HALREG_MUX_BITRATE				0x612				/* mux stream target bitrate								*/
#define HALREG_MUX_STUFFING_PID			0x613
#define HALREG_MUX_FLAGS				0x614
#if 0
	#define MUX_FLAG_EN_PSIP			0x00000001			/* pid_0x1FFB_psi_table would used same continue			*/
	#define MUX_FLAG_DISABLE_PCR		0x00000002			/* mux stream no create _pcr_packet							*/	
	#define MUX_FLAG_SW_PCR				0x00000004			/* mux stream _pcr_packet clock source in _sw or _hw		*/
#endif

/* auxiliary_stream_ip				*/

#define HALREG_AUXDATA_CNTL				0x630				/* auxiliary stream control									*/
	#define AUXDATA_EN_STOP				0x00000001			/* stop async auxiliary and wait stream update */
	#define AUXDATA_EN_RESET			0x00000002			/* reset auxiliary stream buffer */


#define HALREG_AUXDATA_STATUS			0x631
	#define AUXDATA_IDLE				0x00000000
	#define AUXDATA_ASYNC				0x00000001
	#define AUXDATA_SYNC				0x00000002

#define HALREG_AUXDATA_PACKETNUMS       0x632				/* auxiliary stream valid packet len						*/

/* private_stream_ip				*/

#define HALREG_PRIVATE_START            0x639				/* private stream _playload start address					*/
#define HALREG_PRIVATE_END              0x63A				/* private stream _playload end address						*/
#define HALREG_PRIVATE_CNTL             0x63B				/* private stream control									*/
	#define PRIVATE_EN_CONTINUE         0x80000000			/* - mux when write and read pointer not equal				*/
	#define PRIVATE_EN_TIMES            0x00000001			/* - mux onces after finish auto clear						*/	

#define HALREG_PRIVATE_WPTR				0x63C				/* private stream write pointer (update by user)			*/
#define HALREG_PRIVATE_RPTR				0x63D				/* private stream read pointer	(update by hardware)		*/

#define HALREG_MUXPSI_MODE				0x2000
	#define MUX_PURE					0x00000000
	#define MUX_ISO13818				0x00000001
	#define MUX_SPEC					0x00000002
	#define MUX_DEFSPEC					0x00000003

#define HALREG_MUXPSI_ISO_TSID			0x2005
#define HALREG_MUXPSI_ISO_PROGRAM_NUM	0x2006

/**
 * @brief MUX Time Update Interface (When active TOT/TDT Table with rule )
 *
 */
#define HALREG_MUX_TIME_CNTL			0x2080
	#define MUX_TIME_EN_UPDATE			0x00000001

#define HALREG_MUX_TIME_DATE			0x2081
	#define MUX_TIME_YEAR_MASK			0xFFFF0000
	#define MUX_TIME_MONTH_MASK			0x0000FF00
	#define MUX_TIME_DAY_MASK			0x000000FF

#define HALREG_MUX_TIME_CLOCK			0x2082
	#define MUX_TIME_HOUR_MASK			0x00FF0000
	#define MUX_TIME_MIN_MASK			0x0000FF00
	#define MUX_TIME_SEC_MASK			0x000000FF

#endif
