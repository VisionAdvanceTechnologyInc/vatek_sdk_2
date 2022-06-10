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

#ifndef _UI_PROPS_MODULATOR_
#define _UI_PROPS_MODULATOR_

#include <core/ui/ui_props_define.h>
#include <core/base/chip_define.h>
#include <core/base/output_modulator.h>

/* output : modulator common */
HALREG_S(MOD_MODE, 8)
HALREG_S(MOD_IFMODE, 3)
HALREG_S(MOD_IFFREQ, 32)
HALREG_S(MOD_DACGAIN, 32)
HALREG_S(MOD_BW_SB, 32)
/* dvb-t */
HALREG_S(DVB_T_CONSTELLATION, 4)
HALREG_S(DVB_T_FFT, 4)
HALREG_S(DVB_T_GUARDINTERVAL, 4)
HALREG_S(DVB_T_CODERATE, 4)
/* j83a */
HALREG_S(J83A_CONSTELLATION, 4)
/* atsc */
HALREG_S(ATSC_CONSTELLATION, 4)
/* j83b */
HALREG_S(J83B_CONSTELLATION, 4)

/* dtmb */
HALREG_S(DTMB_CONSTELLATION, 4)
HALREG_S(DTMB_TIME_INTERLEAVED, 4)
HALREG_S(DTMB_CODERATE, 4)
HALREG_S(DTMB_CARRIERMODE, 4)
HALREG_S(DTMB_SYNCFRAME, 4)
/* isdb-t */
HALREG_S(ISDB_T_CONSTELLATION, 4)
HALREG_S(ISDB_T_FFT, 4)
HALREG_S(ISDB_T_GUARDINTERVAL, 4)
HALREG_S(ISDB_T_CODERATE, 4)
HALREG_S(ISDB_T_TIME_INTERLEAVED, 4)
HALREG_S(ISDB_T_FLAGS,32)
/* j83c */
HALREG_S(J83C_CONSTELLATION, 4)
/* dvb-t2 */
HALREG_S(DVB_T2_VERSION, 32)
HALREG_S(DVB_T2_FLAGS, 32)
HALREG_S(DVB_T2_ISSY, 4)
HALREG_S(DVB_T2_L1_CONSTELLATION, 4)
HALREG_S(DVB_T2_PLP_CONSTELLATION, 4)
HALREG_S(DVB_T2_FFT, 4)
HALREG_S(DVB_T2_CODERATE, 4)
HALREG_S(DVB_T2_GUARDINTERVAL, 4)
HALREG_S(DVB_T2_PILOTPATTERN, 4)
HALREG_S(DVB_T2_FECTYPE, 4)
HALREG_S(DVB_T2_NID, 16)
HALREG_S(DVB_T2_SID, 16)
HALREG_S(DVB_T2_NTI, 16)
HALREG_S(DVB_T2_FECBN, 16)
HALREG_S(DVB_T2_SBN, 16)

/* struct modulator common */
_ui_enum_start(code_rate)
	_val(coderate_1_2, cr_1_2)
	_val(coderate_2_3, cr_2_3)
	_val(coderate_3_4, cr_3_4)
	_val(coderate_5_6, cr_5_6)
	_val(coderate_7_8, cr_7_8)
_ui_enum_end

_ui_enum_start(guard_interval)
	_val(guard_interval_1_32, gi_1_32)
	_val(guard_interval_1_16, gi_1_16)
	_val(guard_interval_1_8, gi_1_8)
	_val(guard_interval_1_4, gi_1_4)
_ui_enum_end

_ui_enum_start(fft_mode)
	_val(fft_2k, fft_2k)
	_val(fft_8k, fft_8k)
	_val(fft_4k, fft_4k)
_ui_enum_end

/* struct dvb_t param */
_ui_enum_start(dvb_t_qam)
	_val(dvb_t_qpsk, qpsk)
	_val(dvb_t_qam16, qam_16)
	_val(dvb_t_qam64, qam_64)
_ui_enum_end

_ui_struct_start(dvb_t_param)
	_prop_enum_dif(dvb_t_param, dvb_t_qam, constellation_mode, constellation, "dvb-t - constellation", DVB_T_CONSTELLATION)
	_prop_enum(dvb_t_param, fft_mode, fft, "dvb-t - fft mode", DVB_T_FFT)
	_prop_enum(dvb_t_param, guard_interval, guardinterval, "dvb-t - guard interval", DVB_T_GUARDINTERVAL)
	_prop_enum(dvb_t_param, code_rate, coderate, "dvb-t - code rate", DVB_T_CODERATE)
_ui_struct_end
/* struct j83a param*/
_ui_enum_start(j83a_qam)
	_val(j83a_qam16, qam_16)
	_val(j83a_qam32, qam_32)
	_val(j83a_qam64, qam_64)
	_val(j83a_qam128, qam_128)
	_val(j83a_qam256, qam_256)
_ui_enum_end

_ui_struct_start(j83a_param)
	_prop_enum_dif(j83a_param, j83a_qam, constellation_mode, constellation, "j83a - constellation", J83A_CONSTELLATION)
_ui_struct_end
/* struct atsc param*/
_ui_enum_start(atsc_qam)
	_val(_8vsb, _8vsb)
_ui_enum_end

_ui_struct_start(atsc_param)
	_prop_enum_dif(atsc_param, atsc_qam, constellation_mode, constellation, "atsc - constellation", ATSC_CONSTELLATION)
_ui_struct_end
/* struct j83b param*/
_ui_enum_start(j83b_qam)
	_val(j83b_qam64, qam_64)
	_val(j83b_qam256, qam_256)
_ui_enum_end

_ui_struct_start(j83b_param)
	_prop_enum_dif(j83b_param, j83b_qam, constellation_mode, constellation, "j83b - constellation", J83B_CONSTELLATION)
	//_prop_enum(j83b_param, r2_cntl_mode, r2_path, "R2 - Path", NULL)
_ui_struct_end

_ui_enum_start(r2_cntl_mode)
	_val(r2_cntl_path_0, path_0)
	_val(r2_cntl_path_1, path_1)
_ui_enum_end

/* struct dtmb param*/
_ui_enum_start(dtmb_time_interleaved)
	_val(dtmb_interleaved_disable, disable)
	_val(dtmb_interleaved_240, ti_240)
	_val(dtmb_interleaved_720, ti_720)
_ui_enum_end

_ui_enum_start(dtmb_carrier_mode)
	_val(dtmb_carrier_3780, 3780)
	_val(dtmb_carrier_1, 1)
_ui_enum_end

_ui_enum_start(dtmb_code_rate)
	_val(dtmb_code_rate_0_4, cr_0_4)
	_val(dtmb_code_rate_0_6, cr_0_6)
	_val(dtmb_code_rate_0_8, cr_0_8)
_ui_enum_end

_ui_enum_start(dtmb_frame_header)
	_val(dtmb_framesync_420, 420)
	_val(dtmb_framesync_945, 945)
	_val(dtmb_framesync_595, 595)
_ui_enum_end

_ui_enum_start(dtmb_qam)
	_val(dtmb_qpsk, qpsk)
	_val(dtmb_qam16, qam_16)
	_val(dtmb_qam64, qam_64)
	_val(dtmb_qam32, qam_32)
_ui_enum_end

_ui_struct_start(dtmb_param)
	_prop_enum_dif(dtmb_param, dtmb_qam, constellation_mode, constellation, "dtmb - constellation", DTMB_CONSTELLATION)
	_prop_enum(dtmb_param, dtmb_time_interleaved, timeinterleaved, "dtmb - time interleaved", DTMB_TIME_INTERLEAVED)
	_prop_enum(dtmb_param, dtmb_code_rate, coderate, "dtmb - code rate", DTMB_CODERATE)
	_prop_enum(dtmb_param, dtmb_carrier_mode, carriermode, "dtmb - carrier mode", DTMB_CARRIERMODE)
	_prop_enum(dtmb_param, dtmb_frame_header, framesync, "dtmb - frame sync mode", DTMB_SYNCFRAME)
_ui_struct_end
/* struct _isdb-t param*/
_ui_enum_start(isdb_t_qam)
	_val(isdb_t_dqpsk,dqpsk)
	_val(isdb_t_qpsk, qpsk)
	_val(isdb_t_qam16, qam_16)
	_val(isdb_t_qam64, qam_64)
_ui_enum_end

_ui_enum_start(isdb_t_time_interleaved)
	_val(isdb_t_interleaved_disable, disable)
	_val(isdb_t_interleaved_mode1, mode_1)
	_val(isdb_t_interleaved_mode2, mode_2)
	_val(isdb_t_interleaved_mode3, mode_3)
_ui_enum_end

_ui_flags_start(isdb_t_param, isdb_t_flags)
	_val(ISDB_T_EN_AC_1,en_ac_1)
	_val(ISDB_T_EN_AC_2,en_ac_2)
	_val(ISDB_T_EN_EMERGENCY,en_emergency_flag)
_ui_flags_end

_ui_struct_start(isdb_t_param)
	_prop_enum_dif(isdb_t_param, isdb_t_qam, constellation_mode, constellation, "isdb-t - constellation", ISDB_T_CONSTELLATION)
	_prop_enum(isdb_t_param, fft_mode, fft, "isdb-t - fft mode", ISDB_T_FFT)
	_prop_enum(isdb_t_param, guard_interval, guardinterval, "isdb-t - guard interval", ISDB_T_GUARDINTERVAL)
	_prop_enum(isdb_t_param, code_rate, coderate, "isdb-t - code rate", ISDB_T_CODERATE)
	_prop_enum(isdb_t_param, isdb_t_time_interleaved, timeinterleaved, "isdb-t - time interleaved mode", ISDB_T_TIME_INTERLEAVED)
	_prop_flag(isdb_t_param, isdb_t_flags,"isdb-t - extend functions",ISDB_T_FLAGS)
_ui_struct_end
/* struct j83c param*/
_ui_enum_start(j83c_qam)
	_val(j83c_qam64, qam_64)
	_val(j83c_qam256, qam_256)
_ui_enum_end

_ui_struct_start(j83c_param)
	_prop_enum_dif(j83c_param, j83c_qam, constellation_mode, constellation, "j83c - constellation", J83C_CONSTELLATION)
_ui_struct_end
/* struct dvb_t2 param*/
_ui_enum_start(t2_version)
	_val(t2_ver_111, 1_1_1)
	_val(t2_ver_121, 1_2_1)
	_val(t2_ver_131, 1_3_1)
	_val(t2_ver_131_lite, 1_3_1_lite)
_ui_enum_end

_ui_enum_start(constellation_t2_l1)
	_val(t2_l1_bpsk, bpsk)
	_val(t2_l1_qpsk, qpsk)
	_val(t2_l1_qam16, qam_16)
	_val(t2_l1_qam64, qam_64)
_ui_enum_end

_ui_enum_start(constellation_t2_plp)
	_val(t2_plp_qpsk, qpsk)
	_val(t2_plp_qam16, qam_16)
	_val(t2_plp_qam64, qam_64)
	_val(t2_plp_qam256, qam_256)
_ui_enum_end

_ui_enum_start(t2_code_rate)
	_val(t2_coderate_1_2, cr_1_2)
	_val(t2_coderate_3_5, cr_3_5)
	_val(t2_coderate_2_3, cr_2_3)
	_val(t2_coderate_3_4, cr_3_4)
	_val(t2_coderate_4_5, cr_4_5)
	_val(t2_coderate_5_6, cr_5_6)
	_val(t2_coderate_1_3, cr_1_3)
	_val(t2_coderate_2_5, cr_2_5)
_ui_enum_end

_ui_enum_start(t2_pilot_pattern)
	_val(pilot_pattern_1, pp1)
	_val(pilot_pattern_2, pp2)
	_val(pilot_pattern_3, pp3)
	_val(pilot_pattern_4, pp4)
	_val(pilot_pattern_5, pp5)
	_val(pilot_pattern_6, pp6)
	_val(pilot_pattern_7, pp7)
	_val(pilot_pattern_8, pp8)
_ui_enum_end

_ui_enum_start(t2_guard_interval)
	_val(t2_gi_1_32, gi_1_32)
	_val(t2_gi_1_16, gi_1_16)
	_val(t2_gi_1_8, gi_1_8)
	_val(t2_gi_1_4, gi_1_4)
	_val(t2_gi_1_128, gi_1_128)
	_val(t2_gi_19_128, gi_19_128)
	_val(t2_gi_19_256, gi_19_256)
_ui_enum_end

_ui_enum_start(t2_fec_length)
	_val(t2_fec_16200, 16200)
	_val(t2_fec_64800, 64800)
_ui_enum_end

_ui_enum_start(t2_fft_mode)
	_val(t2_fft_1k, 1k)
	_val(t2_fft_2k, 2k)
	_val(t2_fft_4k, 4k)
	_val(t2_fft_8k, 8k)
	_val(t2_fft_16k, 16k)
_ui_enum_end

_ui_flags_start(dvb_t2_param, t2_flags)
	_val(T2EN_EXTEND_CARRIER_MODE, en_extend_carrier)
	_val(T2EN_CONSTELLATION_ROTATION, en_constellation_rotation)
	_val(T2EN_INPUT_TS_HEM, en_ts_hem_mode)
	_val(T2EN_DELETE_NULL_PACKET,en_delete_null_packet)
	_val(T2EN_VBR_CODING,en_vbr_coding)
	_val(T2EN_TIME_INTERVAL,en_time_interval)
_ui_flags_end

_ui_enum_start(t2_issy)
	_val(t2_issy_disable,issy_disable)
	_val(t2_issy_short,issy_short)
	_val(t2_issy_long,issy_long)
_ui_enum_end

_ui_struct_start(dvb_t2_param)
	_prop_enum(dvb_t2_param, t2_version, version, "dvb-t2 - version", DVB_T2_VERSION)
	_prop_flag(dvb_t2_param, t2_flags, "dvb-t2 - function flags", DVB_T2_FLAGS)
	_prop_enum(dvb_t2_param, t2_issy, issy, "dvb-t2 - issy", DVB_T2_ISSY)
	_prop_enum(dvb_t2_param, constellation_t2_l1, l1_constellation, "dvb-t2 - L1 constellation", DVB_T2_L1_CONSTELLATION)
	_prop_enum(dvb_t2_param, constellation_t2_plp, plp_constellation, "dvb-t2 - PLP constellation", DVB_T2_PLP_CONSTELLATION)
	_prop_enum(dvb_t2_param, t2_fft_mode, fft, "dvb-t2 - fft mode", DVB_T2_FFT)
	_prop_enum(dvb_t2_param, t2_code_rate, coderate, "dvb-t2 - code rate", DVB_T2_CODERATE)
	_prop_enum(dvb_t2_param, t2_guard_interval, guardinterval, "dvb-t2 - guard intervale", DVB_T2_GUARDINTERVAL)
	_prop_enum(dvb_t2_param, t2_pilot_pattern, pilotpattern, "dvb-t2 - pilot pattern", DVB_T2_PILOTPATTERN)
	_prop_enum(dvb_t2_param, t2_fec_length, fectype, "dvb-t2 - fec block length", DVB_T2_FECTYPE)
	_prop_h16(dvb_t2_param, network_id, "dvb-t2 - network id", DVB_T2_NID)
	_prop_h16(dvb_t2_param, system_id, "dvb-t2 - system id", DVB_T2_SID)
	_prop_u16(dvb_t2_param, ti_ni, "dvb-t2 - ti_nit", DVB_T2_NTI)
	_prop_u16(dvb_t2_param, fecblock_nums, "dvb-t2 - fecblock number", DVB_T2_FECBN)
	_prop_u16(dvb_t2_param, symbol_nums, "dvb-t2 - symbol number", DVB_T2_SBN)
_ui_struct_end


/* struct modulator_param*/
_ui_enum_start(modulator_type)
	_val(modulator_dvb_t, dvb_t)
	_val(modulator_j83a, j83a)
	_val(modulator_atsc, atsc)
	_val(modulator_j83b, j83b)
	_val(modulator_dtmb, dtmb)
	_val(modulator_isdb_t, isdb_t)
	_val(modulator_j83c, j83c)
	_val(modulator_dvb_t2, dvb_t2)
_ui_enum_end

_ui_enum_start(dac_ifmode)
	_val(ifmode_disable, disable)
	_val(ifmode_freq, if_freq)
	_val(ifmode_iqoffset, iq_offset)
	_val(ifmode_inv_freq, if_inv_freq)
//_val(ifmode_inv_iqoffset,iq_inv_offset)
_ui_enum_end

_ui_struct_start(modulator_param)
	_prop_enum(modulator_param, modulator_type, type, "modulator - modulator type", MOD_MODE)
	_prop_u32(modulator_param, bandwidth_symbolrate, "modulator - symbol rate (ksps) or bandwidth(MHz)", MOD_BW_SB)
	_prop_enum(modulator_param, dac_ifmode, ifmode, "dac_if - dac if mode", MOD_IFMODE)
	_prop_u32(modulator_param, iffreq_offset, "dac_if - frequancy", MOD_IFFREQ)
	_prop_u32(modulator_param, dac_gain, "dac_if - dac gain control", MOD_DACGAIN)
_ui_struct_end

#endif
