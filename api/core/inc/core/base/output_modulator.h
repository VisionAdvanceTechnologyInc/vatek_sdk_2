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

#ifndef _OUTPUT_MODULATOR_
#define _OUTPUT_MODULATOR_

#include <core/vatek_base.h>
#include <core/hal/halreg_define.h>
#include <core/hal/halreg_output.h>
#include <core/ui/ui_props_api.h>
#include <core/ui/ui_props_api.h>
#include <core/base/output_rfmixer.h>

/**
 * @ingroup mod_props
 * Modulation Type 
 */
typedef enum _modulator_type{
	modulator_unknown = -1,
	modulator_dvb_t = MOD_DVB_T,		/*!< DVB-T              */
	modulator_j83a = MOD_J83A,			/*!< J83a (DVB-C)       */
	modulator_atsc = MOD_ATSC,			/*!< ATSC (8vsb)	    */
	modulator_j83b = MOD_J83B,			/*!< J83b (ClearQAM)    */
	modulator_dtmb = MOD_DTMB,			/*!< DTMB               */
	modulator_isdb_t = MOD_ISDB_T,		/*!< ISDB-T             */
	modulator_j83c = MOD_J83C,			/*!< J83c (ISDB-C)      */
	modulator_dvb_t2 = MOD_DVB_T2,		/*!< DVB-T2             */
	modulator_mod_nums,
}modulator_type;

/**
 * @ingroup mod_props
 * Modulation Constellation Mode
 */
typedef enum _constellation_mode{
	qam_null = -1,
	_8vsb = ATSC_8VSB,					/*!< 8VSB (ATSC)    */
	j83b_qam64 = J83B_QAM64,			/*!< QAM64 (J83b)	*/
	j83b_qam256 = J83B_QAM256,			/*!< QAM256 (J83b)	*/
	dvb_t_qpsk = DVB_T_QPSK,			/*!< QPSK (DVB-T)	*/
	dvb_t_qam16 = DVB_T_QAM16,			/*!< QAM16 (DVB-T)	*/
	dvb_t_qam64 = DVB_T_QAM64,			/*!< QAM64 (DVB-T)	*/
	j83a_qam16 = J83A_QAM16,			/*!< QAM16 (J83a)	*/
	j83a_qam32 = J83A_QAM32,			/*!< QAM32 (J83a)	*/
	j83a_qam64 = J83A_QAM64,			/*!< QAM64 (J83a)	*/
	j83a_qam128 = J83A_QAM128,			/*!< QAM128 (J83a)	*/
	j83a_qam256 = J83A_QAM256,			/*!< QAM256 (J83a)	*/
	isdb_t_dqpsk = ISDB_T_DQPSK,			
	isdb_t_qpsk = ISDB_T_QPSK,			/*!< QPSK (ISDB-T)	*/
	isdb_t_qam16 = ISDB_T_QAM16,		/*!< QAM16 (ISDB-T)	*/
	isdb_t_qam64 = ISDB_T_QAM64,		/*!< QAM64 (ISDB-T)	*/
	dtmb_qpsk = DTMB_QPSK,				/*!< QPSK (DTMB)    */
	dtmb_qam16 = DTMB_QAM16,			/*!< QAM16 (DTMB)	*/
	dtmb_qam64 = DTMB_QAM64,			/*!< QAM64 (DTMB)	*/
	dtmb_qam4_nr = DTMB_QAM4_NR,
	dtmb_qam32 = DTMB_QAM32,			/*!< QAM32 (DTMB)	*/
	j83c_qam64 = J83C_QAM64,			/*!< QAM64 (J83c)	*/
	j83c_qam256 = J83C_QAM256,			/*!< QAM256 (J83c)	*/
}constellation_mode;


#define MOD_DVB_T_BIT		(1 << MOD_DVB_T)
#define MOD_J83A_BIT		(1 << MOD_J83A)
#define MOD_ATSC_BIT		(1 << MOD_ATSC)
#define MOD_J83B_BIT		(1 << MOD_J83B)
#define MOD_DTMB_BIT		(1 << MOD_DTMB)
#define MOD_ISDB_T_BIT		(1 << MOD_ISDB_T)
#define MOD_J83C_BIT		(1 << MOD_J83C)
#define MOD_DVB_T2_BIT		(1 << MOD_DVB_T2)
#define MOD_DVB_S_BIT		(1 << MOD_DVB_S)
#define MOD_DVB_S2_BIT		(1 << MOD_DVB_S2)


#define is_constellation_dvb_t(a)			(((int32_t)a == dvb_t_qpsk) || ((int32_t)a == dvb_t_qam16) || ((int32_t)a == dvb_t_qam64))
#define is_constellation_j83a(a)			((int32_t)a <= j83a_qam256)
#define is_constellation_isdb_t(a)			((int32_t)a <= isdb_t_qam64)
#define is_constellation_j83b(a)			(((int32_t)a == j83b_qam64) || ((int32_t)a == j83b_qam256))
#define is_constellation_dtmb(a)			(((int32_t)a >= dtmb_qpsk) && ((int32_t)a <= dtmb_qam32))
#define is_constellation_atsc(a)			((int32_t)a == _8vsb)
#define is_base_coderate(a)					((int32_t)a >= coderate_1_2 && (int32_t)a <= coderate_7_8)
#define is_base_guard_interval(a)			((int32_t)a >= guard_interval_1_32 && (int32_t)a <= guard_interval_1_4)
#define is_base_fft(a)						((int32_t)a >= fft_2k && (int32_t)a <= fft_4k)
#define is_current_modulator(a)				((modulator_type)a >= modulator_dvb_t && (modulator_type)a < modulator_mod_nums)

/**
 * @ingroup mod_props
 * Modulation Code Rate 
 */
typedef enum _code_rate{
	coderate_null = -1,
	coderate_1_2 = DVB_T_CODERATE_1_2,		/*!< 1/2		*/
	coderate_2_3 = DVB_T_CODERATE_2_3,		/*!< 2/3		*/
	coderate_3_4 = DVB_T_CODERATE_3_4,		/*!< 3/4		*/
	coderate_5_6 = DVB_T_CODERATE_5_6,		/*!< 5/6		*/
	coderate_7_8 = DVB_T_CODERATE_7_8,		/*!< 7/8		*/
}code_rate;

/**
 * @ingroup mod_props
 * Modulation Guard Interval
 */
typedef enum _guard_interval{
	guard_interval_null = -1,
	guard_interval_1_32 = DVB_T_GI_1_32,		/*!< 1/32		*/
	guard_interval_1_16 = DVB_T_GI_1_16,		/*!< 1/16		*/
	guard_interval_1_8 = DVB_T_GI_1_8,			/*!< 1/8		*/
	guard_interval_1_4 = DVB_T_GI_1_4,			/*!< 1/4		*/
}guard_interval;

/**
 * @ingroup mod_props
 * Modulation FFT
 * 
 */
typedef enum _fft_mode{
	fft_null = -1,
	fft_2k = DVB_T_FFT2K,			/*!< 2K(2048)	*/	
	fft_8k = DVB_T_FFT8K,			/*!< 8K(8192)	*/	
	fft_4k = DVB_T_FFT4K,			/*!< 8K(4096)	*/	
}fft_mode;

/**
 * @ingroup mod_props
 * atsc parameters
 */
typedef struct _atsc_param{
    constellation_mode constellation;   /*!< constellation (_8vsb)*/
}atsc_param;

typedef atsc_param* Patsc_param;

/** default atsc parameters */
static const atsc_param default_atsc_param = { _8vsb, };

/**
 * @ingroup mod_props
 * J83b Parameters
 */
typedef struct _j83b_param{
    constellation_mode constellation;    /*!< constellation qam64,qam256 */
    r2_cntl_mode r2_path;
}j83b_param;

typedef j83b_param* Pj83b_param;

static const j83b_param default_j83b_param = { j83b_qam256, };

/**
 * @ingroup mod_props
 * DVB-T parameters
 */

typedef struct _dvb_t_param{
    constellation_mode constellation;
    fft_mode fft;
    guard_interval guardinterval;
    code_rate coderate;
}dvb_t_param;

typedef dvb_t_param* Pdvb_t_param;

/** default dvb-t parameters values */
static const dvb_t_param default_dvb_t_param =
{
    dvb_t_qam64,fft_8k,guard_interval_1_16,coderate_5_6,
};

/**
 * @ingroup mod_props
 * J83a Parameters
 */
typedef struct _j83a_param
{
    constellation_mode constellation;   /*!< constellation qam16,qam32,qam64,qam128,qam256 */
}j83a_param;

typedef j83a_param* Pj83a_param;

static const j83a_param default_j83a_param = { j83a_qam256, };

/**
 * @ingroup mod_props
 * DTMB Interleaving Mode
 */
typedef enum _dtmb_time_interleaved
{
	dtmb_interleaved_null = -1,
    dtmb_interleaved_disable = DTMB_TI_DISABLE,
    dtmb_interleaved_240 = DTMB_TI_240,
    dtmb_interleaved_720 = DTMB_TI_720,
}dtmb_time_interleaved;

/**
 * @ingroup mod_props
 * DTMB Carrier Mode
 */
typedef enum _dtmb_carrier_mode{
	dtmb_carrier_null = -1,
    dtmb_carrier_3780 = DTMB_CARRIER_3780,
    dtmb_carrier_1  = DTMB_CARRIER_1,
}dtmb_carrier_mode;

/**
 * @ingroup mod_props
 * DTMB Code Rate
 */
typedef enum _dtmb_code_rate{
	dtmb_code_rate_null = -1,
    dtmb_code_rate_0_4 = DTMB_CODERATE_0_4,
    dtmb_code_rate_0_6 = DTMB_CODERATE_0_6,
    dtmb_code_rate_0_8 = DTMB_CODERATE_0_8,
}dtmb_code_rate;

/**
 * @ingroup mod_props
 * DTMB Frame Header
 */
typedef enum _dtmb_frame_header
{
	dtmb_framesync_null = -1,
    dtmb_framesync_420 = DTMB_SYNC_420,
    dtmb_framesync_945 = DTMB_SYNC_945,
    dtmb_framesync_595 = DTMB_SYNC_595,
}dtmb_frame_header;

/**
 * @ingroup mod_props
 * dtmb parameters
 */
typedef struct _dtmb_param{
    constellation_mode constellation;       /*!< constellation */
    dtmb_time_interleaved timeinterleaved;  /*!< timeinterleaved */
    dtmb_code_rate coderate;                /*!< coderate */
    dtmb_carrier_mode carriermode;          /*!< carriermode */
    dtmb_frame_header framesync;            /*!< framesync */
}dtmb_param;

typedef dtmb_param* Pdtmb_param;

/** default dtmb parameters values */
static const dtmb_param default_dtmb_param =
{
    dtmb_qam64,dtmb_interleaved_720,dtmb_code_rate_0_8,dtmb_carrier_3780,dtmb_framesync_945,
};

#define is_dtmb_vaild_bw(a)         (a == 6 || a == 8)
#define is_dtmb_ti_mode(a)          ((int32_t)a >= dtmb_interleaved_disable && (int32_t)a <= dtmb_interleaved_720)
#define is_dtmb_carrier_mode(a)     ((int32_t)a == dtmb_carrier_1 || (int32_t)a == dtmb_carrier_3780)
#define is_dtmb_frame_sync(a)       ((int32_t)a >= dtmb_framesync_420 && (int32_t)a <= dtmb_framesync_595)
#define is_dtmb_code_rate(a)        ((int32_t)a >= dtmb_code_rate_0_4 && (int32_t)a <= dtmb_code_rate_0_8)

/**
 * @ingroup mod_props
 * ISDB-T Time Interleaving Mode
 */
typedef enum _isdb_t_time_interleaved{
	isdb_t_interleaved_null = -1,
    isdb_t_interleaved_disable = ISDB_T_TI_DISABLE,
    isdb_t_interleaved_mode1 = ISDB_T_TI_MODE1,
    isdb_t_interleaved_mode2 = ISDB_T_TI_MODE2,
    isdb_t_interleaved_mode3 = ISDB_T_TI_MODE3,
}isdb_t_time_interleaved;

/**
 * @ingroup mod_props
 * ISDB-T Parameters
 */
typedef struct _isdb_t_param{
    constellation_mode constellation;           /*!< constellation qpsk, qam16, qam64 */
    fft_mode fft;                               /*!< fft mode */
    guard_interval guardinterval;               /*!< guard interval */
    code_rate coderate;                         /*!< code rate */
    isdb_t_time_interleaved timeinterleaved;    /*!< mode 0,1,2 */
	uint32_t isdb_t_flags;
}isdb_t_param;

typedef isdb_t_param* Pisdb_t_param;

#define is_isdb_t_ti_mode(a)		((int32_t)a >= isdb_t_interleaved_disable && (int32_t)a <= isdb_t_interleaved_mode3)
#define is_isdb_t_en_ac1(a)			(a & ISDB_T_EN_AC_1)
#define is_isdb_t_en_ac2(a)			(a & ISDB_T_EN_AC_2)
#define is_isdb_t_en_emergency(a)	(a & ISDB_T_EN_EMERGENCY)

static const isdb_t_param default_isdb_t_param =
{
	isdb_t_qam64,fft_8k,guard_interval_1_16,coderate_5_6,isdb_t_interleaved_mode3,0,
};

/**
 * @ingroup mod_props
 * J83c Parameters
 */
typedef struct _j83c_param{
	constellation_mode constellation;	/*!< constellation qam64,qam256 */
}j83c_param;

typedef j83c_param* Pj83c_param;

static const j83c_param default_j83c_param = { j83c_qam256, };


#define T2_AUTO_FRAME_TIME                  90
#define T2_AUTO_NTI                         4

/**
 * @ingroup mod_props
 * DVB-T2 Version 
 */
typedef enum _t2_version{
	t2_ver_null = -1,
    t2_ver_111 = T2_VER_1_1_1,                          /*!< version 1.1.1 */
    t2_ver_121 = T2_VER_1_2_1,                          /*!< version 1.2.1 */
    t2_ver_131 = T2_VER_1_3_1,                          /*!< version 1.3.1 */
    t2_ver_131_lite = T2_VER_1_3_1 | T2_PROFILE_LITE,   /*!< version 1.3.1 lite*/
}t2_version;

/**
 * @ingroup mod_props
 * DVB-T2 L1 Constellation
 */
typedef enum _constellation_t2_l1{
	t2_l1_null = -1,
    t2_l1_bpsk = T2_L1_BPSK,        /*!< _bpsk */
    t2_l1_qpsk = T2_L1_QPSK,        /*!< _qpsk */
    t2_l1_qam16 = T2_L1_QAM16,      /*!< qam16 */
    t2_l1_qam64 = T2_L1_QAM64,      /*!< qam64 */
}constellation_t2_l1;

/**
 * @ingroup mod_props
 * DVB-T2 PLP Constellation
 */
typedef enum _constellation_t2_plp{
	t2_plp_null = -1,
    t2_plp_qpsk = T2_PLP_QPSK,      /*!< _qpsk */
    t2_plp_qam16 = T2_PLP_QAM16,    /*!< qam16 */
    t2_plp_qam64 = T2_PLP_QAM64,    /*!< qam64 */
    t2_plp_qam256 = T2_PLP_QAM256,  /*!< qam256 */
}constellation_t2_plp;

/**
 * @ingroup mod_props
 * DVB-T2 Code Rate
 */
typedef enum _t2_code_rate{
	t2_coderate_null = -1,
    t2_coderate_1_2 = T2_CODERATE_1_2,  /*!< 1/2 */
    t2_coderate_3_5 = T2_CODERATE_3_5,  /*!< 3/5 */
    t2_coderate_2_3 = T2_CODERATE_2_3,  /*!< 2/3 */
    t2_coderate_3_4 = T2_CODERATE_3_4,  /*!< 3/4 */
    t2_coderate_4_5 = T2_CODERATE_4_5,  /*!< 4/5 */
    t2_coderate_5_6 = T2_CODERATE_5_6,  /*!< 5/6 */
    t2_coderate_1_3 = T2_CODERATE_1_3,	/*!< 1/3 only _fec-length 16200 */
    t2_coderate_2_5 = T2_CODERATE_2_5,	/*!< 2/5 only _fec-length 16200 */
}t2_code_rate;

/**
 * @ingroup mod_props
 * DVB-T2 Pilot Pattern
 */
typedef enum _t2_pilot_pattern{
	pilot_pattern_null = -1,
    pilot_pattern_1 = T2_PP_1,      /*!< pp_1 */
    pilot_pattern_2 = T2_PP_2,      /*!< pp_2 */
    pilot_pattern_3 = T2_PP_3,      /*!< pp_3 */
    pilot_pattern_4 = T2_PP_4,      /*!< pp_4 */
    pilot_pattern_5 = T2_PP_5,      /*!< pp_5 */
    pilot_pattern_6 = T2_PP_6,      /*!< pp_6 */
    pilot_pattern_7 = T2_PP_7,      /*!< pp_7 */
    pilot_pattern_8 = T2_PP_8,      /*!< pp_8 */
}t2_pilot_pattern;

/**
 * @ingroup mod_props
 * DVB-T2 GUARD INTERVAL
 */
typedef enum _t2_guard_interval{
	t2_gi_1_null = -1,
    t2_gi_1_32 = T2_GI_1_32,        /*!< 1/32 */
    t2_gi_1_16 = T2_GI_1_16,        /*!< 1/16 */
    t2_gi_1_8 = T2_GI_1_8,          /*!< 1/8 */
    t2_gi_1_4 = T2_GI_1_4,          /*!< 1/4 */
    t2_gi_1_128 = T2_GI_1_128,      /*!< 1/128 */
    t2_gi_19_128 = T2_GI_19_128,    /*!< 19/128 */
    t2_gi_19_256 = T2_GI_19_256,    /*!< 19/256 */
}t2_guard_interval;

/**
 * @ingroup mod_props
 * DVB-T2 ISSY MODE
 */
typedef enum _t2_issy{
	t2_issy_disable = T2_ISSY_DISABLE,  /*!< disable issy */
	t2_issy_short = T2_ISSY_SHORT,      /*!< short length */
	t2_issy_long = T2_ISSY_LONG,        /*!< long length */
}t2_issy;

/**
 * @ingroup mod_props
 * DVB-T2 FEC BLOCK LENGTH
 */
typedef enum _t2_fec_length{
	t2_fec_null = -1,
    t2_fec_16200 = T2_FEC_16200,        /*!< _fec-block length 16200 */
    t2_fec_64800 = T2_FEC_64800,        /*!< _fec-block length 64800 */
}t2_fec_length;

/**
 * @ingroup mod_props
 * DVB-T2 FFT 
 */
typedef enum _t2_fft_mode{
	t2_fft_null = -1,
    t2_fft_1k = T2_FFT_1K,              /*!< 1k (1024) */
    t2_fft_2k = T2_FFT_2K,              /*!< 2k (2048) */
    t2_fft_4k = T2_FFT_4K,              /*!< 4k (4096) */
    t2_fft_8k = T2_FFT_8K,              /*!< 8k (8192) */
    t2_fft_16k = T2_FFT_16K,            /*!< 16k (16384) */
    t2_fft_32k = T2_FFT_32K,
}t2_fft_mode;

#define is_t2_version(a)                ((int32_t)a == t2_ver_111 || (int32_t)a == t2_ver_121 || (int32_t)a == t2_ver_131 || (int32_t)a == t2_ver_131_lite)
#define is_t2_l1_constellation(a)       ((int32_t)a >= t2_l1_bpsk && (int32_t)a <= t2_l1_qam64)
#define is_t2_plp_constellation(a)      ((int32_t)a >= t2_plp_qpsk && (int32_t)a <= t2_plp_qam256)
#define is_t2_coderate(a)               ((int32_t)a >= t2_coderate_1_2 && (int32_t)a <= t2_coderate_2_5)
#define is_t2_pilot_pattern(a)          ((int32_t)a >= pilot_pattern_1 && (int32_t)a <= pilot_pattern_8)
#define is_t2_guard_interval(a)         ((int32_t)a >= t2_gi_1_32 && (int32_t)a <= t2_gi_19_256)
#define is_t2_fec_length(a)             ((int32_t)a == t2_fec_16200 || (int32_t)a == t2_fec_64800)
#define is_t2_fft(a)                    ((int32_t)a >= t2_fft_1k && (int32_t)a <= t2_fft_16k)

#define T2_IS_EXTEND_CARRIER(a)         ((a & T2EN_EXTEND_CARRIER_MODE) == T2EN_EXTEND_CARRIER_MODE)
#define T2_IS_CONSTELLATION_ROTATION(a) ((a & T2EN_CONSTELLATION_ROTATION) == T2EN_CONSTELLATION_ROTATION)
#define T2_IS_INPUT_TS_HEM(a)           ((a & T2EN_INPUT_TS_HEM) == T2EN_INPUT_TS_HEM)
#define T2_IS_DELETE_NULL_PACKETM(a)    ((a & T2EN_DELETE_NULL_PACKET) == T2EN_DELETE_NULL_PACKET)
#define T2_IS_VBR_CODING(a)				((a & T2EN_VBR_CODING) == T2EN_VBR_CODING)
#define T2_IS_TI(a)						((a & T2EN_TIME_INTERVAL) == T2EN_TIME_INTERVAL)
#define T2_VER_IS_LITE(ver)             ((ver & 1) == 1)

/**
 * @ingroup mod_props
 * DVB-T2 Parameters
 * Only support SISO one PLP 
 */
typedef struct _dvb_t2_param{
    t2_version version;                         /*!< T2 version */
	uint32_t t2_flags;                          /*!< T2 Function Flags use T2EN_XXXXX define to config) */
    constellation_t2_l1 l1_constellation;       /*!< L1 constellation */
    constellation_t2_plp plp_constellation;     /*!< PLP constellation */
	t2_issy issy;                               /*!< ISSY */
    t2_fft_mode fft;                            /*!< FFT mode */
    t2_code_rate coderate;                      /*!< Code Rate */
    t2_guard_interval guardinterval;            /*!< Guard Interval */
    t2_pilot_pattern pilotpattern;              /*!< Pilot Pattern */
    t2_fec_length fectype;                      /*!< FEC-Block length */
    uint16_t network_id;                        /** Network id  */
    uint16_t system_id;                         /** System id   */
    uint16_t fecblock_nums;                     /** FEC block numbers (0 mean auto) */
    uint16_t symbol_nums;                       /** SYMBOL numbers (0 mean auto) */
	uint16_t ti_ni;                             /** Interleaving NI number */
	uint16_t recv;
}dvb_t2_param;

typedef dvb_t2_param* Pdvb_t2_param;

#define T2_FLAG  (T2EN_CONSTELLATION_ROTATION | T2EN_INPUT_TS_HEM)

static const dvb_t2_param default_dvb_t2_param = {
    t2_ver_131,
    T2_FLAG,
    t2_l1_qam16,
    t2_plp_qam256,
	t2_issy_disable,
    t2_fft_8k,
    t2_coderate_4_5,
    t2_gi_1_32,
    pilot_pattern_7,
    t2_fec_16200,
    0x3085,
    0x8001,
    0,			/*	_fecblock nums, symbol nums and _nti set 0 mean auto	*/
    0,			
	0,			
	0,
};

/** 
 * @ingroup mod_props 
 * DAC output mode
*/
typedef enum _dac_ifmode
{
	ifmode_disable = IFMODE_DISABLE,        /*!< IQ mode    */
	ifmode_freq = IFMODE_IF_FREQ,           /*!< IF mode with frequency */
	ifmode_iqoffset = IFMODE_IQ_OFFSET,     /*!< IQ mode with offset frequency  */
    ifmode_inv_freq = IFMODE_IF_INV_FREQ,   /*!< IF mode with frequency and spectrum inversion  */
}dac_ifmode;

/** check is current DAC output mode*/
#define is_current_dac_ifmode(mode) (mode >= ifmode_disable && mode <= ifmode_inv_freq)

/**
 * @ingroup mod_props
 * modulation detail parameters
 */
typedef union _modulator_detail
{
    uint8_t raw_byte;       
    isdb_t_param isdb_t;    /*!< isdb-t parameters*/
    dvb_t_param dvb_t;      /*!< dvb-t parameters*/
    dtmb_param dtmb;        /*!< dtmb parameters*/
    atsc_param atsc;        /*!< atsc parameters*/
    j83a_param j83a;        /*!< j83a parameters*/
    j83b_param j83b;        /*!< j83b parameters*/
    j83c_param j83c;        /*!< j83c parameters*/
    dvb_t2_param dvb_t2;    /*!< dvb-t2 parameters*/
}modulator_detail;

/**
 * @ingroup mod_props
 * Modulator Parameters
 */
typedef struct _modulator_param
{
    uint32_t bandwidth_symbolrate;      /*!< config bandwidth [DVB-T|T2,ISDB-T,DTMB] or symbol rate [J83a]   */
    modulator_type type;                /*!< modulation type */
	dac_ifmode ifmode;                  /*!< select current DAC output mode */
	uint32_t iffreq_offset;             /*!< DAC output frequency (KHz)  */
    uint32_t dac_gain;                  /*!< DAC output gain control */
    modulator_detail mod;               /*!< detail modulation parameters with type field selected*/
}modulator_param;

typedef modulator_param* Pmodulator_param;

static const uint32_t default_bw_sb[modulator_mod_nums] = { 6,5720,0,0,8,6,5309,8, };

#ifdef __cplusplus
extern "C" {
#endif

    HAL_API const Pui_prop_item modulator_param_get_ui_props(modulator_type type);

    HAL_API uint32_t modulator_param_get_bitrate(Pmodulator_param pmod);
    HAL_API vatek_result modulator_param_reset(modulator_type type, Pmodulator_param pmod);
    HAL_API vatek_result modulator_param_reset_dvbt2(modulator_type type, Pmodulator_param pmod);
    HAL_API vatek_result modulator_param_set(hvatek_chip hchip,Pmodulator_param pmod);
    HAL_API vatek_result modulator_param_get(hvatek_chip hchip,Pmodulator_param pmod);

#ifdef __cplusplus
}
#endif

#endif
