#include "internal/avl68xx_internal.h"

#define rc_DVBTx_fund_rate_Hz_iaddr_offset                       0x00000004
#define rc_DVBTx_sample_rate_Hz_iaddr_offset                     0x00000008
#define rc_DVBTx_rf_agc_pol_caddr_offset                         0x0000000f
#define rc_DVBTx_tuner_type_caddr_offset                         0x00000040
#define rc_DVBTx_input_format_caddr_offset                       0x00000041
#define rc_DVBTx_spectrum_invert_caddr_offset                    0x00000042
#define rc_DVBTx_input_select_caddr_offset                       0x00000043
#define rc_DVBTx_nom_carrier_freq_Hz_iaddr_offset                0x00000044
#define rc_DVBTx_l1_proc_only_caddr_offset                       0x00000054
#define rc_DVBTx_common_PLP_present_caddr_offset                 0x00000055
#define rc_DVBTx_common_PLP_ID_caddr_offset                      0x00000056
#define rc_DVBTx_data_PLP_ID_caddr_offset                        0x00000057
#define rc_DVBTx_dvbt_layer_select_caddr_offset                  0x0000006a
#define rc_DVBTx_acquire_mode_caddr_offset                       0x0000006b
#define rc_DVBTx_mpeg_clk_rate_Hz_iaddr_offset                   0x0000006c
#define rc_DVBTx_adc_sel_caddr_offset							 0x00000077
#define rc_DVBTx_adc_use_pll_clk_caddr_offset                    0x00000076

#define rs_DVBTx_rx_mode_caddr_offset                            0x000000d0
#define rs_DVBTx_fec_lock_caddr_offset                           0x000000d2
#define rs_DVBTx_snr_dB_x100_saddr_offset                        0x000000d6
#define rs_DVBTx_post_viterbi_BER_estimate_x10M_iaddr_offset     0x00000114
#define rs_DVBTx_post_LDPC_BER_estimate_x1B_iaddr_offset         0x00000118
#define rs_DVBTx_pre_LDPC_BER_estimate_x10M_iaddr_offset         0x0000011c
#define rs_DVBTx_plp_list_request_caddr_offset                   0x00000133

#define rs_DVBTx_data_PLP_ID_caddr_offset                       0x00000000
#define rs_DVBTx_data_PLP_TYPE_caddr_offset                     0x00000001
#define rs_DVBTx_data_PLP_COD_caddr_offset                      0x00000007
#define rs_DVBTx_data_PLP_MOD_caddr_offset                      0x00000008
#define rs_DVBTx_data_PLP_ROTATION_caddr_offset                 0x00000009
#define rs_DVBTx_data_PLP_FEC_TYPE_caddr_offset                 0x0000000b

#define rs_DVBTx_common_PLP_ID_caddr_offset                     0x00000000
#define rs_DVBTx_common_PLP_COD_caddr_offset                    0x00000007
#define rs_DVBTx_common_PLP_MOD_caddr_offset                    0x00000008
#define rs_DVBTx_common_PLP_ROTATION_caddr_offset               0x00000009
#define rs_DVBTx_common_PLP_FEC_TYPE_caddr_offset               0x0000000b

#define rs_DVBTx_P1_S2_field_2_caddr_offset                     0x00000003
#define rs_DVBTx_MISO_SISO_caddr_offset                         0x00000005
#define rs_DVBTx_T2_profile_caddr_offset                        0x00000006
#define rs_DVBTx_FFT_size_caddr_offset                          0x00000007

#define rs_DVBTx_NUM_PLP_caddr_offset                           0x00000002

#define rs_DVBTx_constellation_caddr_offset                     0x00000001
#define rs_DVBTx_hierarchy_caddr_offset                         0x00000002
#define rs_DVBTx_HP_code_rate_caddr_offset                      0x00000003
#define rs_DVBTx_LP_code_rate_caddr_offset                      0x00000004
#define rs_DVBTx_guard_interval_caddr_offset                    0x00000005
#define rs_DVBTx_transmission_mode_caddr_offset                 0x00000006

#define rs_DVBTx_BWT_EXT_caddr_offset                           0x00000001
#define rs_DVBTx_GUARD_INTERVAL_caddr_offset                    0x00000005
#define rs_DVBTx_PAPR_caddr_offset                              0x00000006
#define rs_DVBTx_L1_MOD_caddr_offset                            0x00000007
#define rs_DVBTx_PILOT_PATTERN_caddr_offset                     0x00000014
#define rs_DVBTx_CELL_ID_saddr_offset                           0x00000016
#define rs_DVBTx_NETWORK_ID_saddr_offset                        0x00000018
#define rs_DVBTx_T2_SYSTEM_ID_saddr_offset                      0x0000001a
#define rs_DVBTx_NUM_T2_FRAMES_caddr_offset                     0x0000001d
#define rs_DVBTx_NUM_DATA_SYMBOLS_saddr_offset                  0x0000001e

#define rs_DVBTx_Signal_Presence_iaddr_offset                   0x00000150

typedef enum _AVL_DVBTx_TunerType
{
    AVL_DVBTX_REAL_IF = 0,
    AVL_DVBTX_COMPLEX_BASEBAND = 1,
    AVL_DVBTX_REAL_IF_FROM_Q = 2
}AVL_DVBTx_TunerType;

typedef enum _AVL_DVBTxBandWidth
{
    AVL_DVBTx_BW_1M7 = 0,
    AVL_DVBTx_BW_5M = 1,
    AVL_DVBTx_BW_6M = 2,
    AVL_DVBTx_BW_7M = 3,
    AVL_DVBTx_BW_8M = 4
}AVL_DVBTxBandWidth;

typedef enum _AVL_DVBTx_LockMode
{
    AVL_DVBTx_LockMode_T2BASE_T = 0,
    AVL_DVBTx_LockMode_T2LITE_T = 1,
    AVL_DVBTx_LockMode_T2BASE = 2,
    AVL_DVBTx_LockMode_T2LITE = 3,
    AVL_DVBTx_LockMode_T_ONLY = 4,
	AVL_DVBTx_LockMode_ALL = 5,
}AVL_DVBTx_LockMode;

typedef struct _AVL_DVBTxConfig
{
    AVL_InputPath eDVBTxInputPath;
    uint32_t  uiDVBTxIFFreqHz;
    AVL_AGCPola eDVBTxAGCPola;
}AVL_DVBTxConfig;

typedef enum _AVL_DVBTx_Standard
{
    AVL_DVBTx_Standard_T = 0,                  //the DVB-T standard
    AVL_DVBTx_Standard_T2 = 1,                   //the DVB-T2 standard
}AVL_DVBTx_Standard;

typedef enum _AVL_DVBT_FFTSize
{
    AVL_DVBT_FFT_2K = 0, 
    AVL_DVBT_FFT_8K = 1,
    AVL_DVBT_FFT_UNKNOWN = 2
}AVL_DVBT_FFTSize;

typedef enum _AVL_DVBT_GuardInterval
{
    AVL_DVBT_GUARD_1_32 = 0,
    AVL_DVBT_GUARD_1_16 = 1,
    AVL_DVBT_GUARD_1_8 = 2,
    AVL_DVBT_GUARD_1_4 = 3
}AVL_DVBT_GuardInterval;

typedef enum _AVL_DVBT_ModulationMode
{
    AVL_DVBT_QPSK = 0,
    AVL_DVBT_16QAM = 1,
    AVL_DVBT_64QAM = 2,
    AVL_DVBT_MOD_UNKNOWN = 3
}AVL_DVBT_ModulationMode;

typedef enum _AVL_DVBT_CodeRate
{
    AVL_DVBT_CR_1_2 = 0,
    AVL_DVBT_CR_2_3 = 1,
    AVL_DVBT_CR_3_4 = 2,
    AVL_DVBT_CR_5_6 = 3,
    AVL_DVBT_CR_7_8 = 4
}AVL_DVBT_CodeRate;

typedef enum _AVL_DVBT2_FFTSize
{
    AVL_DVBT2_FFT_1K = 0,
    AVL_DVBT2_FFT_2K = 1,
    AVL_DVBT2_FFT_4K = 2,
    AVL_DVBT2_FFT_8K = 3,
    AVL_DVBT2_FFT_16K = 4,
    AVL_DVBT2_FFT_32K = 5
}AVL_DVBT2_FFTSize;

const AVL_DVBTxConfig AVL_DVBTxConfigChip0 =
{
    AVL_IF_I,
    5*1000*1000,
    AVL_AGC_NORMAL//AVL_AGC_INVERTED
};

const uint32_t BW_DVBTX_FFT_Table[5]=
{
    1845070,    //bw=1.7MHz
    5714285,    //bw=5.0MHz
    6857143,    //bw=6.0MHz
    8000000,    //bw=7.0MHz
    9142857     //bw=8.0MHz
};

#define _w_dvbtx_u8(a,v)     avl_demod_write_u8(uiSlaveAddr,AVL_FW_DVBTX_CONFIG_REG_BASE + a,v)
#define _w_dvbtx_u32(a,v)    avl_demod_write_u32(uiSlaveAddr,AVL_FW_DVBTX_CONFIG_REG_BASE + a,v)

#define _r_dvbtx_u8(a,v)     avl_demod_read_u8(uiSlaveAddr,AVL_FW_DVBTX_STATUS_REG_BASE + a,&v)

extern vatek_result mod_to_avl_dvbtbw(Pmodulator_param pmod,AVL_DVBTxBandWidth* pavbw);

vatek_result avl_dvbtx_initial(uint8_t uiSlaveAddr,AVL_Demod_Xtal xtal)
{
    vatek_result nres = vatek_badparam;
    const PAVL_PLL_Conf0 pll = avl_get_clock(xtal,AVL_DVBTX);
    uint32_t adcclk = 0;
    if(pll)adcclk = avl_get_adc_clock(pll,xtal,AVL_DVBTX);
    if(adcclk)
    {
        nres = _w_dvbtx_u32(rc_DVBTx_sample_rate_Hz_iaddr_offset,adcclk);
        if(is_vatek_success(nres))
            nres = _w_dvbtx_u32(rc_DVBTx_mpeg_clk_rate_Hz_iaddr_offset,pll->m_MPEGFrequency_Hz);
        ////DDC configuration
        if(is_vatek_success(nres))
            nres = _w_dvbtx_u8(rc_DVBTx_input_format_caddr_offset,AVL_OFFBIN);
        if(is_vatek_success(nres))
            nres = _w_dvbtx_u8(rc_DVBTx_input_select_caddr_offset,AVL_ADC_IN);
        if(is_vatek_success(nres))
            nres = _w_dvbtx_u8(rc_DVBTx_tuner_type_caddr_offset,AVL_DVBTX_REAL_IF);
        if(is_vatek_success(nres))
            nres = _w_dvbtx_u8(rc_DVBTx_rf_agc_pol_caddr_offset,AVL_DVBTxConfigChip0.eDVBTxAGCPola);
        if(is_vatek_success(nres))
        {
            uint32_t carrier_offset_hz = 0;
            if(AVL_DVBTxConfigChip0.uiDVBTxIFFreqHz >= adcclk)
                carrier_offset_hz = AVL_DVBTxConfigChip0.uiDVBTxIFFreqHz - adcclk;
            else carrier_offset_hz = AVL_DVBTxConfigChip0.uiDVBTxIFFreqHz;
            nres = _w_dvbtx_u32(rc_DVBTx_nom_carrier_freq_Hz_iaddr_offset,carrier_offset_hz);
        }
        if(is_vatek_success(nres))
            nres = _w_dvbtx_u8(rc_DVBTx_adc_sel_caddr_offset,AVL_DVBTxConfigChip0.eDVBTxInputPath ^ 1);

        if(is_vatek_success(nres))
        {
            if(xtal == Xtal_16M || xtal == Xtal_24M)
                nres = _w_dvbtx_u8(rc_DVBTx_adc_use_pll_clk_caddr_offset,1);
            else nres = _w_dvbtx_u8(rc_DVBTx_adc_use_pll_clk_caddr_offset,0);
        }
    }
    return nres;
}

vatek_result avl_dvbtx_config(uint8_t uiSlaveAddr,Pmodulator_param pmod)
{
    AVL_DVBTxBandWidth abw = AVL_DVBTx_BW_6M;
    vatek_result nres = mod_to_avl_dvbtbw(pmod,&abw);
    if(is_vatek_success(nres))
    {
        nres = avl_fw_send_rxop(uiSlaveAddr,AVL_FW_CMD_HALT);
        if(is_vatek_success(nres))
        {
            nres = _w_dvbtx_u8(rc_DVBTx_l1_proc_only_caddr_offset,0);
            if(is_vatek_success(nres))
                nres = _w_dvbtx_u32(rc_DVBTx_fund_rate_Hz_iaddr_offset,BW_DVBTX_FFT_Table[abw]);
            if(is_vatek_success(nres))
                nres = _w_dvbtx_u8(rc_DVBTx_acquire_mode_caddr_offset,AVL_DVBTx_LockMode_ALL);
            if(is_vatek_success(nres))
                nres = _w_dvbtx_u8(rc_DVBTx_spectrum_invert_caddr_offset,AVL_SPECTRUM_AUTO);
            if(is_vatek_success(nres))
                nres = _w_dvbtx_u8(rc_DVBTx_dvbt_layer_select_caddr_offset,0);
            if(is_vatek_success(nres))
                nres = _w_dvbtx_u8(rc_DVBTx_data_PLP_ID_caddr_offset,0);
            if(is_vatek_success(nres))
                nres = _w_dvbtx_u8(rc_DVBTx_common_PLP_ID_caddr_offset,0);
            if(is_vatek_success(nres))
                nres = _w_dvbtx_u8(rc_DVBTx_common_PLP_present_caddr_offset,0);
            if(is_vatek_success(nres))
                nres = avl_fw_send_rxop(uiSlaveAddr,AVL_FW_CMD_ACQUIRE);
        }
        
    }
    return nres;
}

extern vatek_result avl_dvbtx_get_t_info(uint8_t uiSlaveAddr,Pdvb_t_param pdvb);
extern vatek_result avl_dvbtx_get_t2_info(uint8_t uiSlaveAddr,Pdvb_t2_param pdvbt2);

vatek_result avl_dvbtx_check_lock(uint8_t uiSlaveAddr,Pmodulator_param pmod)
{
    uint8_t val = 0;
    vatek_result nres = _r_dvbtx_u8(rs_DVBTx_fec_lock_caddr_offset,val);
    if(is_vatek_success(nres))
    {
        AVL_LockStatus lock = (AVL_LockStatus)val;
        if(lock == AVL_STATUS_LOCK)
        {
            if(pmod)
            {
                nres = _r_dvbtx_u8(rs_DVBTx_rx_mode_caddr_offset,val);
                if(val == AVL_DVBTx_Standard_T)
                {
                    pmod->type = modulator_dvb_t;
                    nres = avl_dvbtx_get_t_info(uiSlaveAddr,&pmod->mod.dvb_t);
                }else
                {
                    pmod->type = modulator_dvb_t2;
                    nres = avl_dvbtx_get_t2_info(uiSlaveAddr,&pmod->mod.dvb_t2);
                }
            }
            nres = (vatek_result)1;
        }
    }
    return nres;
}

vatek_result mod_to_avl_dvbtbw(Pmodulator_param pmod,AVL_DVBTxBandWidth* pavbw)
{
    vatek_result nres = vatek_unsupport;
    if(pmod && (pmod->type == modulator_dvb_t2 || pmod->type == modulator_dvb_t))
    {
        nres = vatek_success;
        if(pmod->bandwidth_symbolrate == 5)*pavbw = AVL_DVBTx_BW_5M;
        else if(pmod->bandwidth_symbolrate == 6)*pavbw = AVL_DVBTx_BW_6M;
        else if(pmod->bandwidth_symbolrate == 7)*pavbw = AVL_DVBTx_BW_7M;
        else if(pmod->bandwidth_symbolrate == 8)*pavbw = AVL_DVBTx_BW_8M;
        else if(pmod->bandwidth_symbolrate == 17)*pavbw = AVL_DVBTx_BW_1M7;
        else nres = vatek_unsupport;
    }
    return nres;
}

vatek_result avl_dvbtx_get_t_info(uint8_t uiSlaveAddr,Pdvb_t_param pdvb)
{
    #define _r_tps(a,v)  avl_demod_read_u8(uiSlaveAddr,AVL_FW_DVBT_TPS_REG_BASE + a,&v)

    uint8_t val = 0;
    vatek_result nres = _r_tps(rs_DVBTx_transmission_mode_caddr_offset,val);
    if(is_vatek_success(nres))
    {
        pdvb->fft = (fft_mode)val;
        nres = _r_tps(rs_DVBTx_guard_interval_caddr_offset,val);
    }

    if(is_vatek_success(nres))
    {
        pdvb->guardinterval = (guard_interval)val;
        nres = _r_tps(rs_DVBTx_constellation_caddr_offset,val);
    }

    if(is_vatek_success(nres))
    {
        if(val == AVL_DVBT_QPSK)pdvb->constellation = dvb_t_qpsk;
        else if(val == AVL_DVBT_16QAM)pdvb->constellation = dvb_t_qam16;
        else if(val == AVL_DVBT_64QAM)pdvb->constellation = dvb_t_qam64;
        else pdvb->constellation = qam_null;
        nres = _r_tps(rs_DVBTx_HP_code_rate_caddr_offset,val);
    }

    if(is_vatek_success(nres))
    {
        pdvb->coderate = (code_rate)val;
    }
    return nres;
}

vatek_result avl_dvbtx_get_t2_info(uint8_t uiSlaveAddr,Pdvb_t2_param pdvbt2)
{
    #define _r_p1(a,v)          avl_demod_read_u8(uiSlaveAddr,AVL_FW_DVBT2_P1_REG_BASE + a,&v)
    #define _r_l1pre(a,v)       avl_demod_read_u8(uiSlaveAddr,AVL_FW_DVBT2_L1_PRE_REG_BASE + a,&v)
    #define _r_plpdata(a,v)     avl_demod_read_u8(uiSlaveAddr,AVL_FW_DVBT2_DATA_PLP_CONFIG_REG_BASE + a,&v)

    uint8_t val = 0;
    vatek_result nres = _r_p1(rs_DVBTx_FFT_size_caddr_offset,val);
    if(is_vatek_success(nres))
    {
        pdvbt2->fft = (t2_fft_mode)val;
    }
    return nres;
}
