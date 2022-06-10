#include "internal/avl68xx_internal.h"


#define rc_ISDBT_fund_rate_Hz_iaddr_offset                                0x00000004
#define rc_ISDBT_rf_agc_pol_caddr_offset                                  0x0000000f
#define rc_ISDBT_sample_rate_Hz_iaddr_offset                              0x00000008
#define rc_ISDBT_tuner_type_caddr_offset                                  0x00000040
#define rc_ISDBT_input_format_caddr_offset                                0x00000041
#define rc_ISDBT_input_select_caddr_offset                                0x00000043
#define rc_ISDBT_nom_carrier_freq_Hz_iaddr_offset                         0x00000044
#define rc_ISDBT_TS_clk_rate_Hz_iaddr_offset                              0x0000006c
#define rc_ISDBT_adc_sel_caddr_offset                                     0x00000077
#define rc_ISDBT_adc_use_pll_clk_offset                                   0x00000076
#define rc_ISDBT_layer_out_auto_filt_offset                               0x00000082
#define rc_ISDBT_mult_layer_op_mode_offset                                0x00000083

#define rs_ISDBT_mode_caddr_offset                                        0x00000001
#define rs_ISDBT_system_type_caddr_offset                                 0x00000002
#define rs_ISDBT_GI_saddr_offset                                          0x0000000c
#define rs_ISDBT_layA_constel_size_caddr_offset                           0x0000001c
#define rs_ISDBT_layA_fec_rate_caddr_offset                               0x0000001d
#define rs_ISDBT_layA_itlv_len_caddr_offset                               0x0000001e
#define rs_ISDBT_layA_seg_no_caddr_offset                                 0x0000001f
#define rs_ISDBT_partial_reception_caddr_offset                           0x00000023
#define rs_ISDBT_layB_constel_size_caddr_offset                           0x00000024
#define rs_ISDBT_layB_fec_rate_caddr_offset                               0x00000025
#define rs_ISDBT_layB_itlv_len_caddr_offset                               0x00000026
#define rs_ISDBT_layB_seg_no_caddr_offset                                 0x00000027
#define rs_ISDBT_layC_constel_size_caddr_offset                           0x00000028
#define rs_ISDBT_layC_fec_rate_caddr_offset                               0x00000029
#define rs_ISDBT_layC_itlv_len_caddr_offset                               0x0000002a
#define rs_ISDBT_layC_seg_no_caddr_offset                                 0x0000002b
#define rs_ISDBT_fec_lock_caddr_offset                                    0x000000ce
#define rs_ISDBT_snr_dB_x100_saddr_offset                                 0x000000d2
#define rs_ISDBT_ISDBT_sys_state_iaddr_offset                             0x000000e0
#define rs_ISDBT_Signal_Presence_iaddr_offset                             0x0000010c
#define rs_ISDBT_layA_tot_pkts_iaddr_offset                               0x00000110
#define rs_ISDBT_layA_err_pkts_iaddr_offset                               0x00000114
#define rs_ISDBT_layB_tot_pkts_iaddr_offset                               0x00000118
#define rs_ISDBT_layB_err_pkts_iaddr_offset                               0x0000011c
#define rs_ISDBT_layC_tot_pkts_iaddr_offset                               0x00000120
#define rs_ISDBT_layC_err_pkts_iaddr_offset                               0x00000124

#define rs_errstat_clear__offset                                          0x0000002c /* type = RW */

typedef enum _AVL_ISDBT_TunerType
{
    AVL_ISDBT_REAL_IF = 0,
    AVL_ISDBT_COMPLEX_BASEBAND = 1,
    AVL_ISDBT_REAL_IF_FROM_Q = 2
}AVL_ISDBT_TunerType;

typedef enum AVL_ISDBT_BandWidth
{
    AVL_ISDBT_BW_6M = 0,
    AVL_ISDBT_BW_8M = 1,
}AVL_ISDBT_BandWidth;

typedef enum _AVL_ISDBT_Mode
{
    AVL_ISDBT_Mode1 = 0,
    AVL_ISDBT_Mode2 = 1,
    AVL_ISDBT_Mode3 = 2
}AVL_ISDBT_Mode;

typedef enum AVL_ISDBT_GuardInterval
{
    AVL_ISDBT_GUARD_1_32 = 0,
    AVL_ISDBT_GUARD_1_16 = 1,
    AVL_ISDBT_GUARD_1_8 = 2,
    AVL_ISDBT_GUARD_1_4 = 3
}AVL_ISDBT_GuardInterval;


typedef struct AVL_ISDBTConfig
{ 
    AVL_InputPath eISDBTInputPath;
    AVL_ISDBT_BandWidth eISDBTBandwidth;
    uint32_t uiISDBTIFFreqHz;
    AVL_AGCPola eISDBTAGCPola;
} AVL_ISDBTConfig;

const AVL_ISDBTConfig AVL_ISDBTConfigChip0 =
{
    AVL_IF_I,
    AVL_ISDBT_BW_6M,
    5*1000*1000,
    AVL_AGC_NORMAL
};

static uint32_t BW_ISDBT_FFT_Table[2]=
{
    8126984,    //bw=6.0MHz
    10835979    //bw=8.0MHz
};

#define _w_isdbt_u8(a,v)     avl_demod_write_u8(uiSlaveAddr,AVL_FW_ISDBT_CONFIG_REG_BASE + a,v)
#define _w_isdbt_u32(a,v)    avl_demod_write_u32(uiSlaveAddr,AVL_FW_ISDBT_CONFIG_REG_BASE + a,v)
#define _r_isdbt_u8(a,v)     avl_demod_read_u8(uiSlaveAddr,AVL_FW_ISDBT_STATUS_REG_BASE + a,&v)

extern vatek_result mod_to_isdbt_bw(Pmodulator_param pmod,AVL_ISDBT_BandWidth* isdbbw);

vatek_result avl_isdb_t_initial(uint8_t uiSlaveAddr,AVL_Demod_Xtal xtal)
{
    vatek_result nres = vatek_badparam;
    const PAVL_PLL_Conf0 pll = avl_get_clock(xtal,AVL_ISDBT);
    uint32_t adc_clk = 0;
    
    if(pll)
        adc_clk = avl_get_adc_clock(pll,xtal,AVL_ISDBT);
    
    if(adc_clk)
    {
        nres = _w_isdbt_u32(rc_ISDBT_sample_rate_Hz_iaddr_offset,adc_clk);
        if(is_vatek_success(nres))
            nres = _w_isdbt_u32(rc_ISDBT_TS_clk_rate_Hz_iaddr_offset,pll->m_MPEGFrequency_Hz);
        if(is_vatek_success(nres))
            nres = _w_isdbt_u8(rc_ISDBT_input_format_caddr_offset,AVL_OFFBIN);
        if(is_vatek_success(nres))
            nres = _w_isdbt_u8(rc_ISDBT_input_select_caddr_offset,AVL_ADC_IN);
        if(is_vatek_success(nres))
            nres = _w_isdbt_u8(rc_ISDBT_tuner_type_caddr_offset,AVL_ISDBT_REAL_IF_FROM_Q);
        if(is_vatek_success(nres))
            nres = _w_isdbt_u8(rc_ISDBT_rf_agc_pol_caddr_offset,AVL_ISDBTConfigChip0.eISDBTAGCPola);

        if(is_vatek_success(nres))
        {
            uint32_t carrier_offset_hz = 0;
            if(AVL_ISDBTConfigChip0.uiISDBTIFFreqHz >= adc_clk)
                carrier_offset_hz = AVL_ISDBTConfigChip0.uiISDBTIFFreqHz - adc_clk;
            else carrier_offset_hz = AVL_ISDBTConfigChip0.uiISDBTIFFreqHz;
            nres = _w_isdbt_u32(rc_ISDBT_nom_carrier_freq_Hz_iaddr_offset,carrier_offset_hz);
        }

        if(is_vatek_success(nres))
            nres = _w_isdbt_u8(rc_ISDBT_adc_sel_caddr_offset,AVL_ISDBTConfigChip0.eISDBTInputPath ^ 1);

        if(is_vatek_success(nres))
        {
            if(xtal == Xtal_16M || xtal == Xtal_24M)
                nres = _w_isdbt_u8(rc_ISDBT_adc_use_pll_clk_offset,1);
            else nres = _w_isdbt_u8(rc_ISDBT_adc_use_pll_clk_offset,0);
        }
    }
    return nres;
}

vatek_result avl_isdb_t_config(uint8_t uiSlaveAddr,Pmodulator_param pmod)
{
    AVL_ISDBT_BandWidth abw = AVL_ISDBT_BW_6M;
    vatek_result nres = mod_to_isdbt_bw(pmod,&abw);
    if(is_vatek_success(nres))
    {
        nres = avl_fw_send_rxop(uiSlaveAddr,AVL_FW_CMD_HALT);
        if(is_vatek_success(nres))
            nres = _w_isdbt_u32(rc_ISDBT_fund_rate_Hz_iaddr_offset,BW_ISDBT_FFT_Table[abw]);
        if(is_vatek_success(nres))
            nres = avl_fw_send_rxop(uiSlaveAddr,AVL_FW_CMD_ACQUIRE);
    }
    return nres;
}

vatek_result avl_isdb_t_check_lock(uint8_t uiSlaveAddr,Pmodulator_param pmod)
{
    uint8_t val = 0;
    vatek_result nres = _r_isdbt_u8(rs_ISDBT_fec_lock_caddr_offset,val);
    if(is_vatek_success(nres))
    {
        AVL_LockStatus lock = (AVL_LockStatus)val;
        if(lock == AVL_STATUS_LOCK)
        {
            if(pmod)
            {
                Pisdb_t_param pisdbt = &pmod->mod.isdb_t;
                pmod->type = modulator_isdb_t;
                nres = _r_isdbt_u8(rs_ISDBT_mode_caddr_offset,val);
                if(is_vatek_success(nres))
                {
                    pisdbt->timeinterleaved = (isdb_t_time_interleaved)(val + 1);
                    nres = _r_isdbt_u8(rs_ISDBT_GI_saddr_offset,val);
                }

                if(is_vatek_success(nres))
                    pisdbt->guardinterval = (guard_interval)val;
            }
            nres = (vatek_result)1;
        }
    }
    return nres;
}

vatek_result mod_to_isdbt_bw(Pmodulator_param pmod,AVL_ISDBT_BandWidth* isdbbw)
{
    vatek_result nres = vatek_unsupport;
    if(pmod && pmod->type == modulator_isdb_t)
    {
        nres = vatek_success;
        if(pmod->bandwidth_symbolrate == 6)*isdbbw = AVL_ISDBT_BW_6M;
        else if(pmod->bandwidth_symbolrate == 8)*isdbbw = AVL_ISDBT_BW_8M;
        else nres = vatek_unsupport;
    }
    return nres;
}
