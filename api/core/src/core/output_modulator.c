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

#include <core/tools/tool_ofdm.h>
#include <core/ui/ui_props/ui_props_modulator.h>

vatek_result modulator_param_reset(modulator_type type, Pmodulator_param pmod)
{
	Pofdm_modulation pofdm = NULL;
	vatek_result nres = tool_ofdm_get_modulation(type, &pofdm);
	if(is_vatek_success(nres))
	{
		if (type == modulator_isdb_t) {
			pmod->ifmode = ifmode_iqoffset;
			pmod->iffreq_offset = 143;
		}
		else {
			pmod->ifmode = ifmode_disable;
			pmod->iffreq_offset = 0;
		}
		pmod->dac_gain = 0;
		pmod->bandwidth_symbolrate = default_bw_sb[type];
		pmod->type = type;
		pofdm->checkparam;
		memcpy(&pmod->mod.raw_byte, pofdm->defaultparam, pofdm->defaultsize);
		nres = vatek_success;
	}
	return nres;
}

vatek_result modulator_param_reset_dvbt2(modulator_type type, Pmodulator_param pmod)
{
	vatek_result nres = vatek_success;

	pmod->ifmode = ifmode_disable;
	pmod->iffreq_offset = 0;
		
	pmod->dac_gain = 0;
	pmod->bandwidth_symbolrate = default_bw_sb[type];
	pmod->type = type;


	memcpy(&pmod->mod.dvb_t2, &default_dvb_t2_param, sizeof(dvb_t2_param));
	
	return nres;
}

const Pui_prop_item modulator_param_get_ui_props(modulator_type type)
{
	Pofdm_modulation pofdm = NULL;
	vatek_result nres = tool_ofdm_get_modulation(type, &pofdm);
	if (is_vatek_success(nres))return pofdm->uiprops;
	else VWAR("unsupport modulation type : %d", type);
	return NULL;
}

vatek_result modulator_param_set(hvatek_chip hchip, Pmodulator_param pmod)
{
	Pofdm_modulation pofdm = NULL;
	vatek_result nres = tool_ofdm_get_modulation(pmod->type, &pofdm);
	if (is_vatek_success(nres))
	{
		nres = ui_props_write_hal(hchip, _ui_struct(modulator_param), (uint8_t*)pmod);
		if (is_vatek_success(nres))
			nres = ui_props_write_hal(hchip, pofdm->uiprops, (uint8_t*)&pmod->mod);
	}
	return nres;
}

vatek_result modulator_param_get(hvatek_chip hchip, Pmodulator_param pmod)
{
	vatek_result nres = ui_props_read_hal(hchip, _ui_struct(modulator_param), (uint8_t*)pmod);
	if (is_vatek_success(nres))
	{
		Pofdm_modulation pofdm = NULL;
		nres = tool_ofdm_get_modulation(pmod->type, &pofdm);
		if(is_vatek_success(nres))
			nres = ui_props_read_hal(hchip, pofdm->uiprops, (uint8_t*)&pmod->mod);
	}
	return nres;
}

uint32_t modulator_param_get_bitrate(Pmodulator_param pmod)
{
	Pofdm_modulation pofdm = NULL;
	vatek_result nres = tool_ofdm_get_modulation(pmod->type, &pofdm);
	if (is_vatek_success(nres))
		return (vatek_result)pofdm->getbitrate(pmod);
	return 0;
}

#if 0

vatek_result modulator_param_check(Pmodulator_param pmod, uint32_t* errcode)
{
	Pofdm_modulation pofdm = NULL;
	vatek_result nres = tool_ofdm_get_modulation(pmod, &pofdm);
	if (is_vatek_success(nres))
		nres = pofdm->checkparam(pmod);
	return nres;
}

#include <core/tools/tool_dvb_t2.h>
#include <core/tools/tool_isdb_t.h>

typedef uint32_t (*fpmodulator_get_bitrate)(Pmodulator_param pmod);
typedef int32_t (*fpmodulator_check)(Pmodulator_param pmod);

typedef struct _modulator_params_prop
{
    uint32_t len;
    const uint8_t* _default;
    const Pui_prop_item uiprops;
    fpmodulator_get_bitrate get_bitrate;
	fpmodulator_check check;
}modulator_params_prop,*Pmodulator_params_prop;

#define _mparam_prop(type,tag)   {sizeof(tag),(const uint8_t*)&default_##tag,_ui_struct(tag),modulator_##type##_datarate,modulator_##type##_checkparam},

extern uint32_t modulator_atsc_datarate(Pmodulator_param param);
extern uint32_t modulator_j83b_datarate(Pmodulator_param param);
extern uint32_t modulator_dvb_t_datarate(Pmodulator_param param);
extern uint32_t modulator_j83a_datarate(Pmodulator_param param);
extern uint32_t modulator_dtmb_datarate(Pmodulator_param param);
extern uint32_t modulator_isdb_t_datarate(Pmodulator_param param);
extern uint32_t modulator_j83c_datarate(Pmodulator_param param);
extern uint32_t modulator_dvb_t2_datarate(Pmodulator_param param);

extern int32_t modulator_atsc_checkparam(Pmodulator_param param);
extern int32_t modulator_j83b_checkparam(Pmodulator_param param);
extern int32_t modulator_dvb_t_checkparam(Pmodulator_param param);
extern int32_t modulator_j83a_checkparam(Pmodulator_param param);
extern int32_t modulator_dtmb_checkparam(Pmodulator_param param);
extern int32_t modulator_isdb_t_checkparam(Pmodulator_param param);
extern int32_t modulator_j83c_checkparam(Pmodulator_param param);
extern int32_t modulator_dvb_t2_checkparam(Pmodulator_param param);

static const modulator_params_prop modulator_props[modulator_mod_nums] =
{
	_mparam_prop(dvb_t,dvb_t_param)
	_mparam_prop(j83a,j83a_param)
	_mparam_prop(atsc,atsc_param)
	_mparam_prop(j83b,j83b_param)
	_mparam_prop(dtmb,dtmb_param)
	_mparam_prop(isdb_t,isdb_t_param)
	_mparam_prop(j83c,j83c_param)
	_mparam_prop(dvb_t2,dvb_t2_param)
};

uint32_t modulator_param_get_bitrate(Pmodulator_param pmod)
{
    if(is_current_modulator(pmod->type))
    {
        return modulator_props[pmod->type].get_bitrate(pmod);
    }
    return 0;
}

vatek_result modulator_param_check(Pmodulator_param pmod,uint32_t* errcode)
{
	vatek_result nres = vatek_badparam;
	if(is_current_modulator(pmod->type))
	{
		int32_t err = modulator_props[pmod->type].check(pmod);
		if(err == 0)nres = vatek_success;
		else if(errcode)*errcode = (uint32_t)err;
	}
	return nres;
}

const Pui_prop_item modulator_param_get_ui_props(modulator_type type)
{
    if(is_current_modulator(type))
        return modulator_props[type].uiprops;
    return NULL;
}

vatek_result modulator_param_reset(modulator_type type, Pmodulator_param pmod)
{
	vatek_result nres = vatek_badparam;
	if (is_current_modulator(type))
	{
		pmod->ifmode = ifmode_disable;
		pmod->iffreq_offset = 0;
		pmod->dac_gain = 0;
		pmod->bandwidth_symbolrate = default_bw_sb[type];
		pmod->type = type;
		memcpy(&pmod->mod.raw_byte, modulator_props[type]._default, modulator_props[type].len);
		nres = vatek_success;
	}
	return nres;
}

vatek_result modulator_param_set(hvatek_chip hchip,Pmodulator_param pmod)
{
    vatek_result nres = vatek_badparam;
    if(is_current_modulator(pmod->type))
    {
        nres = ui_props_write_hal(hchip,_ui_struct(modulator_param),(uint8_t*)pmod);
        if(is_vatek_success(nres))
            nres = ui_props_write_hal(hchip,modulator_props[pmod->type].uiprops,(uint8_t*)&pmod->mod);
    }
    return nres;
}

vatek_result modulator_param_get(hvatek_chip hchip,Pmodulator_param pmod)
{
    vatek_result nres = ui_props_read_hal(hchip,_ui_struct(modulator_param),(uint8_t*)pmod);
    if(is_vatek_success(nres))
    {
        if(is_current_modulator(pmod->type))
            nres = ui_props_read_hal(hchip,modulator_props[pmod->type].uiprops, (uint8_t*)&pmod->mod);
        else nres = vatek_badparam;
    }
    return nres;
}


uint32_t modulator_atsc_datarate(Pmodulator_param param)
{
    return 19392658;
}

uint32_t modulator_j83b_datarate(Pmodulator_param param)
{
    if(param->mod.j83b.constellation == j83b_qam64)return 26970350;
    else if(param->mod.j83b.constellation == j83b_qam256)return 38810700;
    return 0;
}

uint32_t modulator_dvb_t_datarate(Pmodulator_param param)
{
	dvb_t_super_frame frame;
	uint32_t bitrate = 0;
	vatek_result nres = tool_dvb_t_get_super_frame(param, &frame);
	if (is_vatek_success(nres))
		bitrate = tool_dvb_t_get_bitrate(&frame);
	return bitrate;

#if 0
	uint32_t base = 0;
	uint32_t dem = 0;
	uint64_t value = 0;
	Pdvb_t_param pt = &param->mod.dvb_t;

	/* bitrate = 1512 / 2048 * 188 / 204 * 64 / 56 * bw * cr * qam * qi (bps) */
	base = 423 * dvb_t_qam_bits[pt->constellation] * dvb_t_gi_ratio[pt->guardinterval][0] * dvb_t_cr_ratio[pt->coderate][0];
	dem = 544 * dvb_t_gi_ratio[pt->guardinterval][1] * dvb_t_cr_ratio[pt->coderate][1];

	value = (base * (uint64_t)(param->bandwidth_symbolrate * 1000000));
	value /= dem;
	return (uint32_t)value;
#endif
}

#define J83A_DATARATE(sb,qam)   (((((uint64_t)sb * 1000) * ((uint64_t)qam + 4)) * 188) / 204)
uint32_t modulator_j83a_datarate(Pmodulator_param param)
{
    return J83A_DATARATE(param->bandwidth_symbolrate,param->mod.j83a.constellation);
}

uint32_t modulator_j83c_datarate(Pmodulator_param param)
{
    if(param->mod.j83c.constellation == j83c_qam64)
        return J83A_DATARATE(param->bandwidth_symbolrate,j83c_qam64);
    else return J83A_DATARATE(param->bandwidth_symbolrate,j83c_qam256);
}

uint32_t modulator_dtmb_datarate(Pmodulator_param param)
{
	#define DTMB_CR_BASE	7488
	#define DTMB_BW_6MHZ	(uint32_t)(DTMB_6MHZ*1000000)
	#define DTMB_BW_8MHZ	(uint32_t)(DTMB_8MHZ*1000000)

	uint16_t dtmb_cr[4] = { 3008,4512,6016,0, };
	uint16_t dtmb_frame[4] = { 420,945,595, };
	uint8_t dtmb_qam[5] = { 2,4,6,2,5, };
	Pdtmb_param pt = &param->mod.dtmb;

	uint64_t base = DTMB_BW_6MHZ;
	if (param->bandwidth_symbolrate == 8)base = DTMB_BW_8MHZ;

	base *= (uint64_t)3744 * dtmb_cr[pt->coderate] * dtmb_qam[pt->constellation];
	base /= ((uint64_t)3780 + dtmb_frame[pt->framesync]) * DTMB_CR_BASE;

	return (uint32_t)base;
}

#if 0

uint32_t modulator_isdb_t_datarate(Pmodulator_param param)
{
	dvb_t_super_frame frame;
	vatek_result nres = tool_isdb_t_get_ofdm_frame(param, &frame);
	if (!is_vatek_success(nres))VERR("bad isdb_t param");
	return tool_isdb_t_t_get_bitrate(&frame);

#if 0
	#define ISDB_T_FD(bw)		((bw*1000000)/(108 * 14))
	#define ISDB_T_FD_TI(bw)	{ISDB_T_FD(bw),ISDB_T_FD(bw)/2,ISDB_T_FD(bw)/4,}

	static const uint32_t isdb_t_fd[3][3] =
	{
		ISDB_T_FD_TI(6),
		ISDB_T_FD_TI(7),
		ISDB_T_FD_TI(8),
	};

	static const uint16_t isdb_t_nf[3][2] = { {96,108},{192,216},{384,432,}, };

	const static uint8_t isdb_qam_len[] = { 1,2,4,6,};

	uint64_t base = 0;
	uint32_t dem = 0;
	Pisdb_t_param pt = &param->mod.isdb_t;
	int32_t midx = 0;
	if (pt->fft == fft_4k)midx = 1;
	else if (pt->fft == fft_8k)midx = 2;

	uint32_t fd = isdb_t_fd[param->bandwidth_symbolrate - 6][midx];

	base = (uint64_t)fd * isdb_t_nf[midx][0] * 13 * 188;
	base *= isdb_qam_len[pt->constellation] * dvb_t_gi_ratio[pt->guardinterval][0] * dvb_t_cr_ratio[pt->coderate][0];
	dem = dvb_t_gi_ratio[pt->guardinterval][1] * dvb_t_cr_ratio[pt->coderate][1] * 204;
	base /= dem;
	return (uint32_t)base;
#endif
}

#endif

uint32_t modulator_dvb_t2_datarate(Pmodulator_param param)
{
    t2_frame_param frameparam;
    t2_frame_result frameresult;
    Pdvb_t2_param pdvbt2 = &param->mod.dvb_t2;
	uint32_t symnum = pdvbt2->symbol_nums;
	uint32_t fecnum = pdvbt2->fecblock_nums;

    int32_t nres = -1;
    
    memset(&frameparam,0,sizeof(t2_frame_param));
    memset(&frameresult,0,sizeof(t2_frame_result));

	if (pdvbt2->symbol_nums == 0 || pdvbt2->fecblock_nums == 0 || pdvbt2->ti_ni == 0)
	{
		nres = dvb_t2_get_t2_frame_time_param(param, T2_AUTO_FRAME_TIME, &frameparam);
		if (is_vatek_success(nres))
		{
			symnum = frameparam.max_symbol;
			fecnum = frameparam.max_block; 
		}
	}
	else nres = dvb_t2_get_t2_frame_param(param, &frameparam);

    if (nres >= 0)
    {
		nres = dvb_t2_get_t2_frame_result(param, &frameparam, symnum, fecnum, &frameresult);
        if(nres >= 0)return frameresult.bitrate;
    }
    return 0;
}


int32_t modulator_atsc_checkparam(Pmodulator_param param)
{
	if(param->mod.atsc.constellation == _8vsb)
		return MODPARAM_ERR_SUCCESS;
	else return MODPARAM_ERR_CONSTELLATION;
}

int32_t modulator_j83b_checkparam(Pmodulator_param param)
{
	if (param->mod.j83b.constellation == j83b_qam64 ||
		param->mod.j83b.constellation == j83b_qam256)
		return MODPARAM_ERR_SUCCESS;
	else return MODPARAM_ERR_CONSTELLATION;
}

int32_t modulator_dvb_t_checkparam(Pmodulator_param param)
{
    if(param->bandwidth_symbolrate <= 0 || param->bandwidth_symbolrate >= 10)
        return MODPARAM_ERR_BW;

    if (is_constellation_dvb_t(param->mod.dvb_t.constellation) &&
        is_base_coderate(param->mod.dvb_t.coderate) &&
        is_base_guard_interval(param->mod.dvb_t.guardinterval) &&
        is_base_fft(param->mod.dvb_t.fft))
        return MODPARAM_ERR_SUCCESS;
    return MODPARAM_ERR_NOTSUPPORT;
}

#if 0

int32_t modulator_j83a_checkparam(Pmodulator_param param)
{
    uint32_t qam = (uint32_t)param->mod.j83a.constellation;
    if(!J83A_IS_CURRENT_SB(param->bandwidth_symbolrate))return MODPARAM_ERR_BW;
    if(qam <= j83a_qam256)return MODPARAM_ERR_SUCCESS;
    return MODPARAM_ERR_CONSTELLATION;
}

int32_t modulator_j83c_checkparam(Pmodulator_param param)
{
	if (!J83A_IS_CURRENT_SB(param->bandwidth_symbolrate))return MODPARAM_ERR_BW;
    if(param->mod.j83c.constellation == j83c_qam64 ||
        param->mod.j83c.constellation == j83c_qam256)
        return MODPARAM_ERR_SUCCESS;
    return MODPARAM_ERR_CONSTELLATION;
}

int32_t modulator_isdb_t_checkparam(Pmodulator_param param)
{
	if (param->bandwidth_symbolrate < 6 || param->bandwidth_symbolrate > 8)
		return MODPARAM_ERR_BW;
	if (!is_constellation_isdb_t(param->mod.isdb_t.constellation))
		return MODPARAM_ERR_CONSTELLATION;

	if (is_base_coderate(param->mod.isdb_t.coderate) &&
		is_base_guard_interval(param->mod.isdb_t.guardinterval) &&
		is_base_fft(param->mod.isdb_t.fft) &&
		is_isdb_t_ti_mode(param->mod.isdb_t.timeinterleaved))
	{
		return MODPARAM_ERR_SUCCESS;
	}
	return MODPARAM_ERR_NOTSUPPORT;
}

#endif

int32_t modulator_dtmb_checkparam(Pmodulator_param param)
{
    if(param->bandwidth_symbolrate <= 0 || param->bandwidth_symbolrate >= 10)
        return MODPARAM_ERR_BW;

    if (!is_constellation_dtmb(param->mod.dtmb.constellation) ||
        !is_dtmb_carrier_mode(param->mod.dtmb.carriermode) ||
        !is_dtmb_code_rate(param->mod.dtmb.coderate) ||
        !is_dtmb_frame_sync(param->mod.dtmb.framesync) ||
        !is_dtmb_ti_mode(param->mod.dtmb.timeinterleaved))
        return MODPARAM_ERR_CONSTELLATION;

    if (param->mod.dtmb.carriermode == dtmb_carrier_3780)
    {
        if (param->mod.dtmb.framesync == dtmb_framesync_595)
            return MODPARAM_ERR_SYNCFRAME_1;
        if (param->mod.dtmb.constellation == dtmb_qam32 ||
            param->mod.dtmb.constellation == dtmb_qam4_nr)
            return MODPARAM_ERR_CONSTELLATION;
    }
    else
    {
        if(param->mod.dtmb.framesync != dtmb_framesync_595)
            return MODPARAM_ERR_SYNCFRAME_2;
        if(param->mod.dtmb.constellation == dtmb_qam64)
            return MODPARAM_ERR_CONSTELLATION;
    }
    return MODPARAM_ERR_SUCCESS;
}

int32_t modulator_dvb_t2_checkparam(Pmodulator_param param)
{
    Pdvb_t2_param pt2 = &param->mod.dvb_t2;
    t2_frame_param fparam;
	if (!is_t2_version(pt2->version) ||
		!is_t2_l1_constellation(pt2->l1_constellation) ||
		!is_t2_plp_constellation(pt2->plp_constellation) ||
		!is_t2_coderate(pt2->coderate) ||
		!is_t2_guard_interval(pt2->guardinterval) ||
		!is_t2_fft(pt2->fft) ||
		!is_t2_pilot_pattern(pt2->pilotpattern) ||
		!is_t2_fec_length(pt2->fectype))
	{
		return MODPARAM_ERR_NOTSUPPORT;
	}

	t2_fft_mode fft = param->mod.dvb_t2.fft;
	if (fft == t2_fft_32k)return MODPARAM_ERR_T2FFT32K;
	if (T2_IS_EXTEND_CARRIER(param->mod.dvb_t2.t2_flags))
	{
		if (fft < t2_fft_8k && fft > t2_fft_32k)
			return MODPARAM_ERR_T2EXTCARRIER;
	}

	if (fft == t2_fft_16k &&
		(param->mod.dvb_t2.guardinterval == t2_gi_19_256 || param->mod.dvb_t2.guardinterval == t2_gi_19_128))
		return MODPARAM_ERR_T216KGI;

	if (param->mod.dvb_t2.fectype == t2_fec_64800 &&
		(param->mod.dvb_t2.coderate == t2_coderate_1_3 ||
		 param->mod.dvb_t2.coderate == t2_coderate_2_5))
	{
		return MODPARAM_ERR_T2FECCR;
	}

	if (dvb_t2_get_t2_frame_param(param, &fparam) < 0)
		return MODPARAM_ERR_NOTSUPPORT;

	if (pt2->symbol_nums != 0 && pt2->symbol_nums > fparam.max_symbol)
		return MODPARAM_ERR_T2SYMBOLNUM;
	if (pt2->fecblock_nums != 0 && pt2->fecblock_nums > fparam.max_block)
		return MODPARAM_ERR_T2FECBLOCKNUM;
	return MODPARAM_ERR_SUCCESS;
}

#endif
