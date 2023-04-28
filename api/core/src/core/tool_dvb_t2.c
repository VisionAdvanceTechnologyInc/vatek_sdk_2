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

#include <core/tools/tool_dvb_t2.h>

/*
    DVB-T2 TIPS - spec EN 302 755_1.3.1

    - Total carrier (bandwidth * 8MHz)/7
    - FFT Length (carrier numbers)
    - Symbol (FFT Length + (FFT Length * GI))
    - Cell constellation bits (with each carrier)
    - FEC Block - 16200, 64800
*/

#define DVB_T2_SUPERFRAME_NUMS          2
#define DVB_T2_PLP_NUMS                 1
#define DVB_T2_AUX_NUMS                 0

/* dvb_t2 implement with fixed l1 fixed len (1 _plp, no aux and _fef)*/
#define L1_POST_CONFIG_LEN              (35 + 35 + (89 * DVB_T2_PLP_NUMS) + 32 + (DVB_T2_AUX_NUMS * 32)) 
#define L1_POST_DYNAMIC_LEN             (71 + (48 * DVB_T2_PLP_NUMS) + 8 + (DVB_T2_AUX_NUMS*48))
#define L1_POST_EXTENSION_LEN           0
#define L1_POST_INFO_SIZE               (L1_POST_CONFIG_LEN + L1_POST_DYNAMIC_LEN + L1_POST_EXTENSION_LEN)

/* L1 */
#define KBCH_1_2                        7032
#define KSIG_POST                       (L1_POST_INFO_SIZE + 32) /* L1_POST_INFO_SIZE + 32 (crc32)*/
#define NBCH_PARITY                     168

#define N_punc_temp		                ((6 * (KBCH_1_2 - KSIG_POST)) / 5)
#define N_post_temp		                (KSIG_POST + NBCH_PARITY + 9000 - N_punc_temp)

#define _UNSUPPORT						0
#define CELL_FFT_NUMS					9		/* 1,2,4,8_N,16_N,32_N,8_E,16_E,32_E*/
#define CELL_PP_NUMS					8
#define CELL_FEC_NUMS					2
#define CELL_QAM_NUMS					4
#define CELL_GI_NUMS					7
#define CELL_CR_NUMS					8

#define PP1                             0x01
#define PP2                             0x02
#define PP3                             0x04
#define PP4                             0x08
#define PP5                             0x10
#define PP6                             0x20
#define PP7                             0x40
#define PP8                             0x80
#define PP0                             0

#define _FFTSIZE(n)     (n*1024)

static const uint16_t T2_BCH[CELL_FEC_NUMS][CELL_CR_NUMS] =
{   /*              1_2   3_5    2_3   3_4   4_5   5_6   1_3 2_5    */
    /*FEC_16200 */{ 7032 ,9552 ,10632,11712,12432,13152,5232,6312, },
    /*FEC_64800 */{ 32208,38688,43040,48408,51648,53840,_UNSUPPORT ,_UNSUPPORT },
};

/* Table 17: Parameters for time interval  */
static const uint16_t T2_TI_NCELLS[CELL_FEC_NUMS][CELL_QAM_NUMS] =
{
    /*FEC_16200*/{8100,4050,2700,2025,},
    /*FEC_64800*/{32400,16200,10800,8100,},
};

/* Table 47: Number of active cells C_P2 in one P2 symbol */
static const uint16_t T2_CELL_C_P2[2][CELL_FFT_NUMS - 3] =
{
    /* SISO */ {558,1118,2236,4472,8944,22432,},
    /* MISO */ {546,1098,2198,4398,8814,17612,},
};

/* Table 51: Number of P2 symbols denoted by N_P2 for different FFT modes */
static const uint8_t T2_CELL_N_P2[CELL_FFT_NUMS - 3] = { 16,8,4,2,1,1, };

static const uint8_t T2_ETA_MOD[4] = { 1,2,4,6 }; /* L1 MOD */

/*
    Table 48: Number of active cells C_data in one normal symbol
    _UNSUPPORT indicates that the corresponding combination of FFT size and pilot patternis never used.
*/
static const uint16_t T2_CELL_C_DATA[CELL_FFT_NUMS][CELL_PP_NUMS] =
{
    /* LINE    _UNSUPPORT,_UNSUPPORT,_UNSUPPORT,_UNSUPPORT,_UNSUPPORT,_UNSUPPORT,_UNSUPPORT,_UNSUPPORT */
    /* 1k  */{        764,       768,        798,       804,       818, _UNSUPPORT,_UNSUPPORT,_UNSUPPORT, },
    /* 2k  */{       1522,       1532,      1596,       1602,     1632, _UNSUPPORT,      1646,_UNSUPPORT, },
    /* 4k  */{       3084,       3092,      3228,       3234,     3298, _UNSUPPORT,      3328,_UNSUPPORT, },
    /* 8K  */{       6208,       6214,      6494,       6498,      6634,_UNSUPPORT,      6698,      6698, },
    /* 16K */{      12418,      12436,     12988,      13002,     13272,     13288,     13416,     13406, },
    /* 32K */{ _UNSUPPORT,      24886,_UNSUPPORT,      26022,_UNSUPPORT,     26592,     26836,     26812, },
    /* 8K-E*/{       6296,       6298,      6584,       6588,      6728,_UNSUPPORT,      6788,      6788, },
    /*16K-E*/{      12678,      12698,     13262,      13276,     13552,     13568,     13698,     13688, },
    /*32K-E*/{ _UNSUPPORT,      25412,_UNSUPPORT,      26572,_UNSUPPORT,     27152,     27404,     27376, },
};

/*
    Table 49: Number of data cells N_FC in the frame closing symbol
    _UNSUPPORT indicates that frame closing symbols are never used for the corresponding combination of
    FFT size and pilot pattern.
*/

static const uint16_t T2_CELL_N_FC[CELL_FFT_NUMS][CELL_PP_NUMS] =
{
    /* LINE    _UNSUPPORT,_UNSUPPORT,_UNSUPPORT,_UNSUPPORT,_UNSUPPORT,_UNSUPPORT,_UNSUPPORT,_UNSUPPORT */
    /* 1k  */{        568,       710,       710,       780,       780,_UNSUPPORT,_UNSUPPORT,_UNSUPPORT, },
    /* 2k  */{       1136,      1420,      1420,      1562,      1562,_UNSUPPORT,      1632,_UNSUPPORT, },
    /* 4k  */{       2272,      2840,      2840,      3124,      3124,_UNSUPPORT,      3266,_UNSUPPORT, },
    /* 8K  */{       4544,      5680,      5680,      6248,      6248,_UNSUPPORT,      6532,_UNSUPPORT, },
    /* 16K */{       9088,     11360,     11360,     12496,     12496,     13064,     13064,_UNSUPPORT, },
    /* 32K */{ _UNSUPPORT,     22720,_UNSUPPORT,     24992,_UNSUPPORT,     26128,_UNSUPPORT,_UNSUPPORT, },
    /* 8K-E*/{       4608,      5760,      5760,      6336,      6336,_UNSUPPORT,      6624,_UNSUPPORT, },
    /*16K-E*/{       9280,     11600,     11600,     12760,     12760,     13340,     13340,_UNSUPPORT, },
    /*32K-E*/{ _UNSUPPORT,     23200,_UNSUPPORT,     25520,_UNSUPPORT,     26680,_UNSUPPORT,_UNSUPPORT, },
};

/*
    Table 50: Number of active cells C_FC in the frame closing symbol
    _UNSUPPORT indicates that frame closing symbols are never used for the corresponding combination of
    FFT size and pilot pattern.
*/
static const uint16_t T2_CELL_C_FC[CELL_FFT_NUMS][CELL_PP_NUMS] =
{
    /* LINE    _UNSUPPORT,_UNSUPPORT,_UNSUPPORT,_UNSUPPORT,_UNSUPPORT,_UNSUPPORT,_UNSUPPORT,_UNSUPPORT */
    /* 1k  */{        402,       654,       490,       707,       544,_UNSUPPORT,_UNSUPPORT,_UNSUPPORT, },
    /* 2k  */{        804,      1309,       980,      1415,      1088,_UNSUPPORT,      1396,_UNSUPPORT, },
    /* 4k  */{       1609,      2619,      1961,      2831,      2177,_UNSUPPORT,      2792,_UNSUPPORT, },
    /* 8K  */{       3218,      5238,      3922,      5662,      4354,_UNSUPPORT,      5585,_UNSUPPORT, },
    /* 16K */{       6437,     10476,      7845,     11324,      8709,     11801,     11170,_UNSUPPORT, },
    /* 32K */{ _UNSUPPORT,     20952,_UNSUPPORT,     22649,_UNSUPPORT,     23603,_UNSUPPORT,_UNSUPPORT, },
    /* 8K-E*/{       3264,      5312,      3978,      5742,      4416,_UNSUPPORT,      5664,_UNSUPPORT, },
    /*16K-E*/{       6573,     10697,      8011,     11563,      8893,     12051,     11406,_UNSUPPORT, },
    /*32K-E*/{ _UNSUPPORT,     21395,_UNSUPPORT,     23127,_UNSUPPORT,     24102,_UNSUPPORT,_UNSUPPORT, },
};

const uint8_t t2_pp_support[CELL_FFT_NUMS-3][CELL_GI_NUMS] =
{           /* GI_1_32      GI_1_16                 GI_1_8              GI_1_4      GI_1_128        GI_19_128           GI_19_256   */
/*1K*/      {PP0            ,PP4 | PP5              ,PP2 | PP3          ,PP1        ,PP0            ,PP0                ,PP0,                   },
/*2K*/      {PP7 | PP4      ,PP4 | PP5              ,PP2 | PP3          ,PP1        ,PP0            ,PP0                ,PP0,                   },
/*4K*/      {PP7 | PP4      ,PP4 | PP5              ,PP2 | PP3          ,PP1        ,PP0            ,PP0                ,PP0,                   },
/*8K*/      {PP7 | PP4      ,PP8 | PP4 | PP5        ,PP2 | PP3 | PP8    ,PP1 | PP8  ,PP7            ,PP2 | PP3 | PP8    ,PP4 | PP5 | PP8        },
/*16K*/     {PP7 | PP4 | PP6,PP2 | PP8 | PP4 | PP5  ,PP2 | PP3 | PP8    ,PP1 | PP8  ,PP7            ,PP2 | PP3 | PP8    ,PP4 | PP5 | PP8 | PP2  },
/*32K*/     {PP4 | PP6      ,PP2 | PP8 | PP4        ,PP2 | PP8          ,PP0        ,PP7            ,PP2 | PP8          ,PP4 | PP8 | PP2        },
};


const uint32_t t2_fft_size[] = { _FFTSIZE(1),_FFTSIZE(2),_FFTSIZE(4),_FFTSIZE(8),_FFTSIZE(16),_FFTSIZE(32), };
const uint16_t t2_gi_base[CELL_GI_NUMS][2] =
{
	{1,32,},{1,16,},{1,8,},{1,4,},{1,128,},{19,128,},{19,256,},
};

extern uint32_t t2_get_total_carrier(Pmodulator_param pmod);
extern uint32_t t2_get_symbol_carrier(Pmodulator_param pmod);
extern int32_t t2_get_frame_cells_data(Pdvb_t2_cell pcell,int32_t sb);

vatek_result tool_dvb_t2_frame_reset(Pmodulator_param pmod, Pdvb_t2_frame pframe)
{
    vatek_result nres = tool_dvb_t2_check_param(pmod);
	if (is_vatek_success(nres))
	{   /* each t2 frame cost tick and packet numbers */
        Pdvb_t2_param pt2 = &pmod->mod.dvb_t2;
        uint32_t tcarrier = t2_get_total_carrier(pmod);
        uint32_t fcarrier = t2_get_symbol_carrier(pmod);
        uint32_t sbnums = pt2->symbol_nums;
        uint32_t fbits = pt2->fecblock_nums * (T2_BCH[pt2->fectype][pt2->coderate] - 80);

        if (T2_IS_INPUT_TS_HEM(pt2->t2_flags))
            fbits = (uint32_t)(((uint64_t)fbits * 188) / 187);

        tool_dvb_t2_get_cell(pmod, &pframe->cell);
		sbnums += T2_CELL_N_P2[pt2->fft];
		fcarrier = (fcarrier * sbnums) + 2048;/* P1 symbol */

        pframe->ofdm.frametick = (((uint64_t)fcarrier * 27000000) / tcarrier);
        pframe->ofdm.packetnums = fbits / (TS_PACKET_LEN * 8);
        pframe->ofdm.framerecv = fbits % (TS_PACKET_LEN * 8);
	}
	return nres;
}

uint32_t tool_dvb_t2_get_bitrate(Pmodulator_param pmod)
{
    vatek_result nres = tool_dvb_t2_check_param(pmod);
    if (is_vatek_success(nres))
    {
        Pdvb_t2_param pt2 = &pmod->mod.dvb_t2;
		uint32_t tcarrier = t2_get_total_carrier(pmod);
        uint32_t fcarrier = t2_get_symbol_carrier(pmod);
        uint32_t sbnums = pt2->symbol_nums;
		uint32_t fbits = pt2->fecblock_nums * (T2_BCH[pt2->fectype][pt2->coderate] - 80);

		if (T2_IS_INPUT_TS_HEM(pt2->t2_flags))
			fbits = (uint32_t)(((uint64_t)fbits * 188) / 187);
        sbnums += T2_CELL_N_P2[pt2->fft];
        fcarrier = (fcarrier * sbnums) + 2048;/* P1 symbol */
        return (uint32_t)(((uint64_t)fbits * tcarrier) / fcarrier);
    }
    return 0;
}

vatek_result tool_dvb_t2_check_param(Pmodulator_param pmod)
{
    Pdvb_t2_param pt2 = &pmod->mod.dvb_t2;
    vatek_result nres = vatek_success;
    uint32_t nerr = MODPARAM_ERR_NOTSUPPORT;
    if (!is_t2_version(pt2->version) ||
        !is_t2_l1_constellation(pt2->l1_constellation) ||
        !is_t2_plp_constellation(pt2->plp_constellation) ||
        !is_t2_coderate(pt2->coderate) ||
        !is_t2_guard_interval(pt2->guardinterval) ||
        !is_t2_fft(pt2->fft) ||
        !is_t2_pilot_pattern(pt2->pilotpattern) ||
        !is_t2_fec_length(pt2->fectype))nres = vatek_badparam;

    if (is_vatek_success(nres))
    {
        nerr = MODPARAM_ERR_BW;
        if (pmod->bandwidth_symbolrate != 0 &&
            pmod->bandwidth_symbolrate != 5 &&
            pmod->bandwidth_symbolrate != 6 &&
            pmod->bandwidth_symbolrate != 7 && 
            pmod->bandwidth_symbolrate != 8 &&
            pmod->bandwidth_symbolrate != 10)nres = vatek_badparam;
    }

    if (is_vatek_success(nres))
    {
        nerr = MODPARAM_ERR_SUCCESS;
        if (T2_IS_EXTEND_CARRIER(pt2->t2_flags) && pt2->fft < t2_fft_8k)nerr = MODPARAM_ERR_T2EXTCARRIER;
        else
        {
            uint8_t gipp = t2_pp_support[pt2->fft][pt2->guardinterval];
            nerr = MODPARAM_ERR_NOTSUPPORT;
            if ((gipp & (1 << pt2->pilotpattern)) == 0)nres = vatek_badparam;
        }
    }

    if (is_vatek_success(nres))
    {
        nerr = MODPARAM_ERR_T2FECCR;
        if (T2_BCH[pt2->fectype][pt2->coderate] == _UNSUPPORT)nres = vatek_badparam;
    }

    if (is_vatek_success(nres))
    {
        dvb_t2_cell t2cell;
        tool_dvb_t2_get_cell(pmod, &t2cell);
        nerr = MODPARAM_ERR_T2SYMBOLNUM;
        nres = tool_dvb_t2_get_symbol_nums_max(pmod, &t2cell);
        if (is_vatek_success(nres))
        {
            if (!pt2->symbol_nums)pt2->symbol_nums = tool_dvb_t2_get_symbol_nums(pmod,&t2cell);
            else if (nres < pt2->symbol_nums)nres = vatek_badparam;
        }

        if (is_vatek_success(nres))
        {
            nerr = MODPARAM_ERR_T2FECBLOCKNUM;
            nres = tool_dvb_t2_get_fecblock_nums(&t2cell, pt2->symbol_nums);
            if (is_vatek_success(nres))
            {
                if (!pt2->fecblock_nums)pt2->fecblock_nums = nres;
                else if (nres < pt2->fecblock_nums)nres = vatek_badparam;
            }
        }
    }
    if (!is_vatek_success(nres))VWAR("dvb-t2 param fail - [%08x]", nerr);
    return nres;
}

vatek_result tool_dvb_t2_get_symbol_nums(Pmodulator_param pmod, Pdvb_t2_cell pcell)
{
	Pdvb_t2_param pt2 = &pmod->mod.dvb_t2;
	uint32_t totc = t2_get_total_carrier(pmod);
	uint32_t framec = t2_get_symbol_carrier(pmod);

	framec += 2048;                 /* append P1 carrier */
	totc /= 20;                     /* max frame length 250ms */
    //totc >>= 2;
	totc /= framec;
	totc -= T2_CELL_N_P2[pt2->fft]; /* p2 cost symbol numbers */

	totc = (totc >> 1) << 1;
	return (vatek_result)totc;
}

vatek_result tool_dvb_t2_get_symbol_nums_max(Pmodulator_param pmod, Pdvb_t2_cell pcell)
{
	Pdvb_t2_param pt2 = &pmod->mod.dvb_t2;
	uint32_t totc = t2_get_total_carrier(pmod);
	uint32_t framec = t2_get_symbol_carrier(pmod);

	framec += 2048;                 /* append P1 carrier */
    totc >>= 2;                     /* max frame length 250ms */
	totc /= framec;
	totc -= T2_CELL_N_P2[pt2->fft]; /* p2 cost symbol numbers */

    totc = (totc >> 1) << 1;
	return (vatek_result)totc;
}

vatek_result tool_dvb_t2_get_fecblock_nums(Pdvb_t2_cell pcell, int32_t symbolnums)
{
	int32_t cells = 0;
	if (pcell->n_fc == 0)cells = (symbolnums * pcell->c_data);        /* normal frame */
	else cells = ((symbolnums - 1) * pcell->c_data) + pcell->c_fc;    /* closing frame */

	return (vatek_result)((cells - pcell->d_l1) / pcell->cells);
}

void tool_dvb_t2_get_cell(Pmodulator_param pmod, Pdvb_t2_cell pcell)
{
    Pdvb_t2_param pt2 = &pmod->mod.dvb_t2;
	int32_t fftidx = pt2->fft;

	if (T2_IS_EXTEND_CARRIER(pt2->t2_flags))fftidx += 3;
	pcell->bch = T2_BCH[pt2->fectype][pt2->coderate];                   /* fec_block valid buffer bites (DFL)   */
	pcell->n_p2 = T2_CELL_N_P2[pt2->fft];                               /* how many P2 symbol with fft mode     */
    pcell->c_p2 = T2_CELL_C_P2[0][pt2->fft];
	pcell->etamod = T2_ETA_MOD[pt2->l1_constellation];                  /* l1 cell bits                         */

    pcell->cells = T2_TI_NCELLS[pt2->fectype][pt2->plp_constellation];  /* fec_block total cost cells           */
	pcell->c_data = T2_CELL_C_DATA[fftidx][pt2->pilotpattern];          /* normal frame data cells numbers      */    
	pcell->n_fc = T2_CELL_N_FC[fftidx][pt2->pilotpattern];              /* closing frame */
	pcell->c_fc = T2_CELL_C_FC[fftidx][pt2->pilotpattern];

	if ((pt2->guardinterval == t2_gi_1_128 && pt2->pilotpattern == pilot_pattern_7) ||
		(pt2->guardinterval == t2_gi_1_32 && pt2->pilotpattern == pilot_pattern_4) ||
		(pt2->guardinterval == t2_gi_1_16 && pt2->pilotpattern == pilot_pattern_2) ||
		(pt2->guardinterval == t2_gi_19_256 && pt2->pilotpattern == pilot_pattern_2))
	{
		pcell->n_fc = _UNSUPPORT;
		pcell->c_fc = _UNSUPPORT;
	}

    if (pcell->n_p2 == 1)pcell->n_post = (N_post_temp / (pcell->etamod * 2)) * (2 * pcell->etamod);
    else pcell->n_post = (N_post_temp / (pcell->etamod * pcell->n_p2)) * (pcell->etamod * pcell->n_p2);
    if (pcell->n_post % pcell->etamod)pcell->n_post++;

    pcell->d_l1 = (pcell->n_post / pcell->etamod) + 1804;
}

int32_t tool_dvb_t2_get_L1_size(Pdvb_t2_cell pcell)
{
    #define N_POST_FEC_BLOCK    1
    int32_t n_mod_total = pcell->n_post / pcell->etamod;
	return n_mod_total * N_POST_FEC_BLOCK;
}

int32_t tool_dvb_t2_get_n_punc(Pdvb_t2_cell pcell)
{
    return N_punc_temp - (pcell->n_post - N_post_temp);
}

int32_t t2_get_frame_cells_data(Pdvb_t2_cell pcell, int32_t sb)
{
	int32_t cells = 0;
	if (pcell->n_fc == 0)cells = (pcell->n_p2 * pcell->c_p2) + (sb * pcell->c_data);        /* normal frame */
	else cells = (pcell->n_p2 * pcell->c_p2) + ((sb - 1) * pcell->c_data) + pcell->c_fc;    /* closing frame */
	return cells;
}

uint32_t t2_get_total_carrier(Pmodulator_param pmod)
{
	uint32_t base = 131000000;
	uint32_t dec = 71;
	if (pmod->bandwidth_symbolrate != 0)
	{
		base = pmod->bandwidth_symbolrate * 8000000;
		dec = 7;
	}
	return base / dec;
}

uint32_t t2_get_symbol_carrier(Pmodulator_param pmod)
{
	Pdvb_t2_param pt2 = &pmod->mod.dvb_t2;
	uint32_t fftsb = t2_fft_size[pt2->fft] * (t2_gi_base[pt2->guardinterval][0] + t2_gi_base[pt2->guardinterval][1]);
	return fftsb / t2_gi_base[pt2->guardinterval][1];
}

#if 0

#define T_1_7MHZ        (1.0/(131000000.0/71.0))
#define T_5MHZ          (1.0/(40000000.0/7.0))
#define T_6MHZ          (1.0/(48000000.0/7.0))
#define T_7MHZ          (1.0/(56000000.0/7.0))
#define T_8MHZ          (1.0/(64000000.0/7.0))
#define T_10MHZ         (1.0/(80000000.0/7.0))

#define FFT_SIZE_1K     (1*1024)
#define FFT_SIZE_2K     (2*1024)
#define FFT_SIZE_4K     (4*1024)
#define FFT_SIZE_8K     (8*1024)
#define FFT_SIZE_16K    (16*1024)
#define FFT_SIZE_32K    (32*1024)

#define BW_1_7          0
#define BW_5            1
#define BW_6            2
#define BW_7            3
#define BW_8            4
#define BW_10           5

typedef uint64_t uint32_fp;

#define FP32_ONE        (((uint64_t)1) <<32)
#define FP32_F2FP(x)    ((uint32_fp)((x) * FP32_ONE + 0.5))
#define T2048_32(t)     FP32_F2FP((2048.0*t))

const static uint32_fp T2_T2048_32[] =
{
    T2048_32(T_1_7MHZ),T2048_32(T_5MHZ),T2048_32(T_6MHZ),T2048_32(T_7MHZ),T2048_32(T_8MHZ),T2048_32(T_10MHZ),
};

const static uint32_fp T2_TU[6][6] =
{
	{ FP32_F2FP(T_1_7MHZ*FFT_SIZE_1K),FP32_F2FP(T_1_7MHZ*FFT_SIZE_2K),FP32_F2FP(T_1_7MHZ*FFT_SIZE_4K),
	  FP32_F2FP(T_1_7MHZ*FFT_SIZE_8K),FP32_F2FP(T_1_7MHZ*FFT_SIZE_16K),FP32_F2FP(T_1_7MHZ*FFT_SIZE_32K)},
	{ FP32_F2FP(T_5MHZ*FFT_SIZE_1K),FP32_F2FP(T_5MHZ*FFT_SIZE_2K),FP32_F2FP(T_5MHZ*FFT_SIZE_4K),
	  FP32_F2FP(T_5MHZ*FFT_SIZE_8K),FP32_F2FP(T_5MHZ*FFT_SIZE_16K),FP32_F2FP(T_5MHZ*FFT_SIZE_32K) },
	{ FP32_F2FP(T_6MHZ*FFT_SIZE_1K),FP32_F2FP(T_6MHZ*FFT_SIZE_2K),FP32_F2FP(T_6MHZ*FFT_SIZE_4K),
	  FP32_F2FP(T_6MHZ*FFT_SIZE_8K),FP32_F2FP(T_6MHZ*FFT_SIZE_16K),FP32_F2FP(T_6MHZ*FFT_SIZE_32K) },
	{ FP32_F2FP(T_7MHZ*FFT_SIZE_1K),FP32_F2FP(T_7MHZ*FFT_SIZE_2K),FP32_F2FP(T_7MHZ*FFT_SIZE_4K),
	  FP32_F2FP(T_7MHZ*FFT_SIZE_8K),FP32_F2FP(T_7MHZ*FFT_SIZE_16K),FP32_F2FP(T_7MHZ*FFT_SIZE_32K) },
	{ FP32_F2FP(T_8MHZ*FFT_SIZE_1K),FP32_F2FP(T_8MHZ*FFT_SIZE_2K),FP32_F2FP(T_8MHZ*FFT_SIZE_4K),
	  FP32_F2FP(T_8MHZ*FFT_SIZE_8K),FP32_F2FP(T_8MHZ*FFT_SIZE_16K),FP32_F2FP(T_8MHZ*FFT_SIZE_32K) },
	{ FP32_F2FP(T_10MHZ*FFT_SIZE_1K),FP32_F2FP(T_10MHZ*FFT_SIZE_2K),FP32_F2FP(T_10MHZ*FFT_SIZE_4K),
	  FP32_F2FP(T_10MHZ*FFT_SIZE_8K),FP32_F2FP(T_10MHZ*FFT_SIZE_16K),FP32_F2FP(T_10MHZ*FFT_SIZE_32K) },
};

#define GI_T_32(a,b)   FP32_F2FP((a/b))

const static uint32_fp T2_GI[] =
{
    GI_T_32(1.0,32.0),GI_T_32(1.0,16.0),GI_T_32(1.0,8.0),GI_T_32(1.0,4.0),GI_T_32(1.0,128.0),GI_T_32(19.0,128.0),GI_T_32(19.0,256.0),
};

extern int32_t t2_get_bw_index(Pmodulator_param param);
extern int32_t t2_get_bch(Pmodulator_param param);
extern int32_t t2_get_frame_cell(Pmodulator_param param,Pt2_frame_cell pcell);
extern int32_t t2_get_max_symbol(Pmodulator_param param,Pt2_frame_cell pcell);
extern int32_t t2_get_max_block(Pmodulator_param param,Pt2_frame_cell pcell,int32_t symbolnum);

extern int32_t t2_get_symbol_time(Pmodulator_param param,Pt2_frame_cell pcell,int32_t ms);

extern uint32_fp t2_get_ts(Pmodulator_param param);
extern uint32_fp t2_get_tf(Pmodulator_param param, uint32_t symbolnum);
extern uint32_fp uint32_fp_div_fp(uint32_fp a, uint32_fp b);
extern uint32_fp uint32_fp_mul(uint32_fp a, uint32_fp b);

extern int32_t t2_get_cells_n_fc(Pt2_frame_cell pcell, int32_t symbolnum);
extern int32_t t2_get_cells_c_fc(Pt2_frame_cell pcell, int32_t symbolnum);
extern int32_t t2_get_n_post(Pt2_frame_cell pcell);
extern int32_t t2_get_d_l1(Pt2_frame_cell pcell);
extern int32_t t2_get_dummy(Pt2_frame_cell pcell, int32_t symbolnum, int32_t blocknum);

int32_t dvb_t2_get_N_post(Pt2_frame_param t2frame)
{
    return t2_get_n_post(&t2frame->cell_param);
}

#define N_POST_FEC_BLOCK    1  
/* L1-POST Length always less than KBCH_1_2*/
int32_t dvb_t2_get_N_mod_total(Pt2_frame_param t2frame)
{
    int32_t n_post = t2_get_n_post(&t2frame->cell_param);
    int32_t n_mod_total = n_post/t2frame->cell_param.eta_mod;
    return n_mod_total * N_POST_FEC_BLOCK;
}

int32_t dvb_t2_get_N_punc(Pt2_frame_param t2frame)
{
    int32_t n_post = t2_get_n_post(&t2frame->cell_param);
    return N_punc_temp - (n_post - N_post_temp);
}

int32_t dvb_t2_get_t2_frame_param(Pmodulator_param param, Pt2_frame_param t2frame)
{
    int32_t res = t2_get_frame_cell(param,&t2frame->cell_param);
    if(res >= 0)
    {
        res = t2_get_max_symbol(param,&t2frame->cell_param);
        if(res >= 0)
        {
            t2frame->max_symbol = res;
            t2frame->max_block = t2_get_max_block(param,&t2frame->cell_param,res);
        }
    }
    return res;
}

int32_t dvb_t2_get_t2_frame_time_param(Pmodulator_param param,int32_t ms,Pt2_frame_param t2frame)
{
    int32_t res = t2_get_frame_cell(param,&t2frame->cell_param);
    if(res >= 0 && (ms < 0 || ms >= 250))return -1;
    else
    {
        t2frame->max_symbol = t2_get_symbol_time(param,&t2frame->cell_param,ms);
        t2frame->max_block = t2_get_max_block(param,&t2frame->cell_param,t2frame->max_symbol);
        return 0;
    }
}

int32_t dvb_t2_get_t2_frame_result(Pmodulator_param param, Pt2_frame_param pframe, int32_t sbnum, int32_t blknum, Pt2_frame_result presult)
{
    uint32_fp tf = 0;
    uint32_fp feclen = (blknum * ((t2_get_bch(param) - 80) - 0));
    uint32_t bitrate = 0;

    tf = t2_get_tf(param, sbnum + pframe->cell_param.n_p2);
    presult->t_framems = (int32_t)((tf * 1000) >> 32);
    if (presult->t_framems >= 250)return -1;

    tf = uint32_fp_div_fp(FP32_ONE, tf);
    if (T2_IS_INPUT_TS_HEM(param->mod.dvb_t2.t2_flags))
        tf = uint32_fp_mul(tf, FP32_F2FP((188.0 / 187.0)));

    bitrate = (uint32_t)(feclen * (tf >> 32));
    feclen *= (tf & 0xFFFFFFFF);
    feclen >>= 32;
    bitrate += (uint32_t)feclen;

    presult->bitrate = bitrate;
    presult->cells = t2_get_cells_n_fc(&pframe->cell_param,sbnum);
    presult->d_l1 = t2_get_d_l1(&pframe->cell_param);
    presult->dummy = t2_get_dummy(&pframe->cell_param,sbnum,blknum);
    presult->n_post = dvb_t2_get_N_post(pframe);
    presult->n_punc = dvb_t2_get_N_punc(pframe);
    presult->n_mod_total = dvb_t2_get_N_mod_total(pframe);
    presult->n_punc_groups = presult->n_punc / 360;
    presult->n_punc_last = presult->n_punc - presult->n_punc_groups * 360;
    if(presult->dummy < 0)return -1;
    return presult->bitrate;
}

uint32_fp t2_get_ts(Pmodulator_param param)
{
    uint32_fp tu = T2_TU[t2_get_bw_index(param)][param->mod.dvb_t2.fft];
    uint32_fp gi = T2_GI[param->mod.dvb_t2.guardinterval];
    uint32_fp tu_gi = (tu * gi) >> 32;
    return tu + tu_gi;
}

uint32_fp t2_get_tf(Pmodulator_param param, uint32_t symbolnum)
{
    /* 
        TF is the duration of a frame, TF = LF*TS + TP1;  
        LF number of OFDM symbols per frame
        TS is the total symbol duration for all symbols except P1
        TP1 is the duration of the P1 symbol, given by TP1=2048T
    */
    uint32_fp tf = t2_get_ts(param) * symbolnum;
    return tf + T2_T2048_32[t2_get_bw_index(param)];
}

int32_t t2_get_bw_index(Pmodulator_param param)
{
    if (param->bandwidth_symbolrate == 0)return BW_1_7;
    else if (param->bandwidth_symbolrate == 10)return BW_10;
    else return BW_5 + (param->bandwidth_symbolrate - 5);
}

int32_t t2_get_bch(Pmodulator_param param)
{
    Pdvb_t2_param pdvbt2 = &param->mod.dvb_t2;
    return T2_BCH[pdvbt2->fectype][param->mod.dvb_t2.coderate];
}

int32_t t2_get_frame_cell(Pmodulator_param param, Pt2_frame_cell pcell)
{
    int32_t fftidx = param->mod.dvb_t2.fft;
    int32_t ppidx = param->mod.dvb_t2.pilotpattern;
    Pdvb_t2_param pdvbt2 = &param->mod.dvb_t2;

    if (T2_IS_EXTEND_CARRIER(param->mod.dvb_t2.t2_flags))
        fftidx += 3;

    pcell->eta_mod = T2_ETA_MOD[pdvbt2->l1_constellation];
    pcell->n_p2 = T2_CELL_N_P2[pdvbt2->fft];
    pcell->c_p2 = T2_CELL_C_P2[0][pdvbt2->fft]; /* SISO ONLY */

    pcell->c_data = T2_CELL_C_DATA[fftidx][ppidx];
    if(pcell->c_data == _UNSUPPORT)return _UNSUPPORT;
    pcell->n_fc = T2_CELL_N_FC[fftidx][ppidx];
    pcell->c_fc = T2_CELL_C_FC[fftidx][ppidx];

    if(pdvbt2->fectype == t2_fec_64800)
        pcell->n_cells = T2_TI_NCELLS[pdvbt2->fectype][pdvbt2->plp_constellation];
    else pcell->n_cells = T2_TI_NCELLS[pdvbt2->fectype][pdvbt2->plp_constellation];

    /* SISO ONLY */
    if((pdvbt2->guardinterval == t2_gi_1_128 && pdvbt2->pilotpattern == pilot_pattern_7) ||
       (pdvbt2->guardinterval == t2_gi_1_32 && pdvbt2->pilotpattern == pilot_pattern_4) ||
       (pdvbt2->guardinterval == t2_gi_1_16 && pdvbt2->pilotpattern == pilot_pattern_2) ||
       (pdvbt2->guardinterval == t2_gi_19_256 && pdvbt2->pilotpattern == pilot_pattern_2))
    {
         pcell->n_fc = _UNSUPPORT;
         pcell->c_fc = _UNSUPPORT;
    }

    return 0;
}

int32_t t2_get_symbol_time(Pmodulator_param param,Pt2_frame_cell pcell,int32_t ms)
{
    uint32_fp ts = t2_get_ts(param);
    uint32_fp maxfs = (ms * FP32_ONE)/1000;
    int32_t sbnum = (int32_t)(maxfs / ts);
    return (sbnum >> 1) << 1;
}

int32_t t2_get_max_symbol(Pmodulator_param param, Pt2_frame_cell pcell)
{
    uint32_fp ts = t2_get_ts(param);
    uint32_fp maxfs = FP32_F2FP(0.25);
    int32_t sbnum = (int32_t)(maxfs / ts);
    uint32_fp tf = 0;
    int32_t framems = 0xFFFF;

    tf = t2_get_tf(param, sbnum);
    framems = (int32_t)((tf * 1000) >> 32);
    if(framems >= 250)sbnum--;

    if (param->mod.dvb_t2.fft == t2_fft_32k)sbnum = (sbnum >> 1) << 1;
    sbnum -= pcell->n_p2;

    return sbnum;
}

int32_t t2_get_cells_n_fc(Pt2_frame_cell pcell, int32_t symbolnum)
{
    int32_t cells = 0;
    if (pcell->n_fc == 0)cells = (pcell->n_p2 * pcell->c_p2) + (symbolnum * pcell->c_data);
    else cells = (pcell->n_p2 * pcell->c_p2) + ((symbolnum - 1) * pcell->c_data) + pcell->n_fc;
    return cells;
}

int32_t t2_get_cells_c_fc(Pt2_frame_cell pcell, int32_t symbolnum)
{
    int32_t cells = 0;
    if (pcell->c_fc == 0)cells = (pcell->n_p2 * pcell->c_p2) + (symbolnum * pcell->c_data);
    else cells = (pcell->n_p2 * pcell->c_p2) + ((symbolnum - 1) * pcell->c_data) + pcell->c_fc;
    return cells;
}

int32_t t2_get_n_post(Pt2_frame_cell pcell)
{
    int32_t N_post = 0;
    int32_t etamod2 = 0;

    if (pcell->n_p2 == 1)etamod2 = pcell->eta_mod * 2;
    else etamod2 = pcell->eta_mod * pcell->n_p2;

    N_post = N_post_temp / etamod2;
    if (N_post_temp % etamod2 != 0)N_post++;
    N_post *= etamod2;
    return N_post;
}

int32_t t2_get_d_l1(Pt2_frame_cell pcell)
{
    return (t2_get_n_post(pcell) / pcell->eta_mod) + 1840;
}

int32_t t2_get_dummy(Pt2_frame_cell pcell, int32_t symbolnum, int32_t blocknum)
{
    int32_t N_post = t2_get_n_post(pcell);
    int32_t cells = t2_get_cells_n_fc(pcell,symbolnum);

    return cells - (pcell->n_cells * (int)blocknum) - 1840 - (N_post / pcell->eta_mod) - (pcell->n_fc - pcell->c_fc);
}

/* symbolnum not include N_P2*/
int32_t t2_get_max_block(Pmodulator_param param, Pt2_frame_cell pcell, int32_t symbolnum)
{
    int32_t cells = t2_get_cells_c_fc(pcell,symbolnum);
    int32_t D_L1 = t2_get_d_l1(pcell);
    return (cells - D_L1) / pcell->n_cells;
}

uint32_fp uint32_fp_div_fp(uint32_fp a, uint32_fp b)
{
    int32_t i = 0;
    uint32_fp div = a;
    uint32_fp res = 0;

    for (i = 0; i < 5; i++)
    {
        int32_t offect = (4 - i) << 3;
        res |= ((div / b) << offect);
        div = (div % b) << 8;
    }
    return res;
}

uint32_fp uint32_fp_mul(uint32_fp a, uint32_fp b)
{
    return (a * (b >> 8)) >> 24;
}
#endif