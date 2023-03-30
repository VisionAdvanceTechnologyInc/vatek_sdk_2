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

#ifndef _HALREG_OUTPUT_
#define _HALREG_OUTPUT_

/* modulator base param */
#define HALREG_MOD_MODE					0x680
	#define MOD_DVB_T					0x00000000
	#define MOD_J83A					0x00000001
	#define MOD_ATSC					0x00000002
	#define MOD_J83B					0x00000003
	#define MOD_DTMB					0x00000004
	#define MOD_ISDB_T					0x00000005
	#define MOD_J83C					0x00000006
	#define MOD_DVB_T2					0x00000007
	#define MOD_DVB_S					0x00000008
	#define MOD_DVB_S2					0x00000009

#define HALREG_MOD_IFMODE				0x681
	#define IFMODE_DISABLE				0x00000001
	#define IFMODE_IF_FREQ				0x00000002
	#define IFMODE_IQ_OFFSET			0x00000003
	#define IFMODE_IF_INV_FREQ			0x00000004

#define HALREG_MOD_IFFREQ				0x682
#define HALREG_MOD_DACGAIN				0x683
	#define DACGAIN_DEFAULT				0x00000000


#define HALREG_MOD_BW_SB				0x684

#define HALREG_MOD_POWERLEVEL           0x685
    #define POWERLEVEL_ADJ_TAG          0xFAFB0000
/* modulator _dvb-t param */
#define HALREG_DVB_T_CONSTELLATION      0x690
    #define DVB_T_QPSK                  0x00000000
    #define DVB_T_QAM16                 0x00000002
    #define DVB_T_QAM64                 0x00000004

#define HALREG_DVB_T_FFT                0x691
    #define DVB_T_FFT2K                 0x00000000
    #define DVB_T_FFT8K                 0x00000001
    #define DVB_T_FFT4K                 0x00000002

#define HALREG_DVB_T_GUARDINTERVAL      0x692
    #define DVB_T_GI_1_32               0x00000000
    #define DVB_T_GI_1_16               0x00000001
    #define DVB_T_GI_1_8                0x00000002
    #define DVB_T_GI_1_4                0x00000003

#define HALREG_DVB_T_CODERATE           0x693
    #define DVB_T_CODERATE_1_2          0x00000000
    #define DVB_T_CODERATE_2_3          0x00000001
    #define DVB_T_CODERATE_3_4          0x00000002
    #define DVB_T_CODERATE_5_6          0x00000003
    #define DVB_T_CODERATE_7_8          0x00000004

/* modulator j83a param */
#define HALREG_J83A_CONSTELLATION       0x690
    #define J83A_QAM16                  0x00000000
    #define J83A_QAM32                  0x00000001
    #define J83A_QAM64                  0x00000002
    #define J83A_QAM128                 0x00000003
    #define J83A_QAM256                 0x00000004

/* modulator atsc param */
#define HALREG_ATSC_CONSTELLATION       0x690
    #define ATSC_8VSB                   0x00000000

/* modulator j83b param */
#define HALREG_J83B_CONSTELLATION       0x690
    #define J83B_QAM64                  0x00000001
    #define J83B_QAM256                 0x00000003

/* modulator dtmb param */
#define HALREG_DTMB_CONSTELLATION       0x690
    #define DTMB_QPSK                   0x00000000
    #define DTMB_QAM16                  0x00000001
    #define DTMB_QAM64                  0x00000002
    #define DTMB_QAM4_NR                0x00000003
    #define DTMB_QAM32                  0x00000004

#define HALREG_DTMB_TIME_INTERLEAVED    0x691
    #define DTMB_TI_DISABLE             0x00000000
    #define DTMB_TI_240                 0x00000002
    #define DTMB_TI_720                 0x00000003

#define HALREG_DTMB_CODERATE            0x692
    #define DTMB_CODERATE_0_4           0x00000000
    #define DTMB_CODERATE_0_6           0x00000001
    #define DTMB_CODERATE_0_8           0x00000002

#define HALREG_DTMB_CARRIERMODE         0x693
    #define DTMB_CARRIER_3780           0x00000000
    #define DTMB_CARRIER_1              0x00000001

#define HALREG_DTMB_SYNCFRAME           0x694
    #define DTMB_SYNC_420               0x00000000
    #define DTMB_SYNC_945               0x00000001
    #define DTMB_SYNC_595               0x00000002

/* modulator _isdb-t param */
#define HALREG_ISDB_T_CONSTELLATION     0x690
    #define ISDB_T_DQPSK                0x00000000
    #define ISDB_T_QPSK                 0x00000001
    #define ISDB_T_QAM16                0x00000002
    #define ISDB_T_QAM64                0x00000003

#define HALREG_ISDB_T_FFT               0x691
    #define ISDB_T_FFT2K                DVB_T_FFT2K
    #define ISDB_T_FFT8K                DVB_T_FFT8K
    #define ISDB_T_FFT4K                DVB_T_FFT4K

#define HALREG_ISDB_T_GUARDINTERVAL     0x692
    #define ISDB_T_GI_1_32              DVB_T_GI_1_32
    #define ISDB_T_GI_1_16              DVB_T_GI_1_16
    #define ISDB_T_GI_1_8               DVB_T_GI_1_8
    #define ISDB_T_GI_1_4               DVB_T_GI_1_4  

#define HALREG_ISDB_T_CODERATE          0x693
    #define ISDB_T_CODERATE_1_2         DVB_T_CODERATE_1_2
    #define ISDB_T_CODERATE_2_3         DVB_T_CODERATE_2_3
    #define ISDB_T_CODERATE_3_4         DVB_T_CODERATE_3_4
    #define ISDB_T_CODERATE_5_6         DVB_T_CODERATE_5_6
    #define ISDB_T_CODERATE_7_8         DVB_T_CODERATE_7_8

#define HALREG_ISDB_T_TIME_INTERLEAVED  0x694
    #define ISDB_T_TI_DISABLE           0x00000000
    #define ISDB_T_TI_MODE1             0x00000001
    #define ISDB_T_TI_MODE2             0x00000002
    #define ISDB_T_TI_MODE3             0x00000003

#define HALREG_ISDB_T_FLAGS				0x695
	#define ISDB_T_EN_AC_1				0x00000001
	#define ISDB_T_EN_AC_2				0x00000002
	#define ISDB_T_EN_EMERGENCY			0x00000004
/* modulator _j83c param*/

#define HALREG_J83C_CONSTELLATION		0x690
	#define J83C_QAM64                  0x00000002
	#define J83C_QAM256                 0x00000004

/* modulator _dvb-t2 param */
#define HALREG_DVB_T2_VERSION           0x690
    #define T2_VERSION_MAJOR_MASK       0xFFFF0000
    #define T2_VER_1_1_1                0x00000000
    #define T2_VER_1_2_1                0x00010000
    #define T2_VER_1_3_1                0x00020000
    #define T2_VERSION_PROFILE_MASK     0x0000FFFF
    #define T2_PROFILE_LITE             0x00000001

#define HALREG_DVB_T2_FLAGS             0x691
    #define T2EN_EXTEND_CARRIER_MODE    0x00000001
    #define T2EN_CONSTELLATION_ROTATION 0x00000002
    #define T2EN_INPUT_TS_HEM           0x00000004
    #define T2EN_DELETE_NULL_PACKET     0x00000008
	#define T2EN_VBR_CODING				0x00000010
	#define T2EN_TIME_INTERVAL			0x00000020

#define HALREG_DVB_T2_ISSY				0x692			/* ISSY LEN  : 0,2,3 [0 : disable]*/
	#define T2_ISSY_DISABLE				0
	#define T2_ISSY_SHORT				2
	#define T2_ISSY_LONG				3

#define HALREG_DVB_T2_NTI				0x693			/* TI NI LEN : 0 - 3 [0 : disable]*/
	#define T2_NTI_DISABLE				0

#define HALREG_DVB_T2_L1_CONSTELLATION  0x694
    #define T2_L1_BPSK                  0x00000000
    #define T2_L1_QPSK                  0x00000001
    #define T2_L1_QAM16                 0x00000002
    #define T2_L1_QAM64                 0x00000003

#define HALREG_DVB_T2_PLP_CONSTELLATION 0x695
    #define T2_PLP_QPSK                 0x00000000
    #define T2_PLP_QAM16                0x00000001
    #define T2_PLP_QAM64                0x00000002
    #define T2_PLP_QAM256               0x00000003

#define HALREG_DVB_T2_FFT               0x696
    #define T2_FFT_1K                   0x00000000
    #define T2_FFT_2K                   0x00000001
    #define T2_FFT_4K                   0x00000002
    #define T2_FFT_8K                   0x00000003
    #define T2_FFT_16K                  0x00000004
    #define T2_FFT_32K                  0x00000005

#define HALREG_DVB_T2_CODERATE          0x697
    #define T2_CODERATE_1_2             0x00000000
    #define T2_CODERATE_3_5             0x00000001
    #define T2_CODERATE_2_3             0x00000002
    #define T2_CODERATE_3_4             0x00000003
    #define T2_CODERATE_4_5             0x00000004
    #define T2_CODERATE_5_6             0x00000005
    #define T2_CODERATE_1_3             0x00000006
    #define T2_CODERATE_2_5             0x00000007

#define HALREG_DVB_T2_GUARDINTERVAL     0x698
    #define T2_GI_1_32                  0x00000000
    #define T2_GI_1_16                  0x00000001
    #define T2_GI_1_8                   0x00000002
    #define T2_GI_1_4                   0x00000003
    #define T2_GI_1_128                 0x00000004
    #define T2_GI_19_128                0x00000005
    #define T2_GI_19_256                0x00000006

#define HALREG_DVB_T2_PILOTPATTERN      0x699
    #define T2_PP_1                     0x00000000
    #define T2_PP_2                     0x00000001
    #define T2_PP_3                     0x00000002
    #define T2_PP_4                     0x00000003
    #define T2_PP_5                     0x00000004
    #define T2_PP_6                     0x00000005
    #define T2_PP_7                     0x00000006
    #define T2_PP_8                     0x00000007

#define HALREG_DVB_T2_FECTYPE           0x69A
    #define T2_FEC_16200                0x00000000
    #define T2_FEC_64800                0x00000001

#define HALREG_DVB_T2_NID               0x69B
#define HALREG_DVB_T2_SID               0x69C
#define HALREG_DVB_T2_FECBN             0x69D           /* _fec_block numbers	[0 : auto]	*/
#define HALREG_DVB_T2_SBN               0x69E           /* symbol numbers		[0 : auto]	*/

#endif
