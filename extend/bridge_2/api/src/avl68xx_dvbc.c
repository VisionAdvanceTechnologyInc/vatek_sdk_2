#include "internal/avl68xx_internal.h"

typedef struct _AVL_DVBCConfig
{ 
    AVL_InputPath eDVBCInputPath;
    uint32_t uiDVBCIFFreqHz;
    uint32_t uiDVBCSymbolRateSps;
    AVL_AGCPola eDVBCAGCPola;
    AVL_DVBC_Standard eDVBCStandard;
}AVL_DVBCConfig;

typedef enum _AVL_DVBC_TunerType
{
    AVL_DVBC_IF = 0,
    AVL_DVBC_BASEBAND = 1
}AVL_DVBC_TunerType;

typedef enum _AVL_DVBCQAMMode
{
    AVL_DVBC_16QAM = 0,      
    AVL_DVBC_32QAM = 1,
    AVL_DVBC_64QAM = 2,      
    AVL_DVBC_128QAM = 3,
    AVL_DVBC_256QAM = 4
}AVL_DVBCQAMMode;

// Defines the symbol interleave mode of the received DVBC signal, only used for J.83B.
typedef enum _AVL_DVBCInterleaveMode
{
    AVL_DVBC_INTERLEAVE_128_1_0 = 0,
    AVL_DVBC_INTERLEAVE_128_1_1 = 1,
    AVL_DVBC_INTERLEAVE_128_2 = 2,
    AVL_DVBC_INTERLEAVE_64_2 = 3,
    AVL_DVBC_INTERLEAVE_128_3 = 4,
    AVL_DVBC_INTERLEAVE_32_4 = 5,
    AVL_DVBC_INTERLEAVE_128_4 = 6,
    AVL_DVBC_INTERLEAVE_16_8 = 7,
    AVL_DVBC_INTERLEAVE_128_5 = 8,
    AVL_DVBC_INTERLEAVE_8_16 = 9,
    AVL_DVBC_INTERLEAVE_128_6 = 10,
    AVL_DVBC_INTERLEAVE_128_7 = 12,
    AVL_DVBC_INTERLEAVE_128_8 = 14
}AVL_DVBCInterleaveMode;

const AVL_DVBCConfig AVL_DVBCConfigChip0 =
{
    AVL_IF_I,
    5*1000*1000,
    6875*1000,
    AVL_AGC_NORMAL,
    AVL_DVBC_J83A
};

//DVBC config registers offset address
#define rc_DVBC_symbol_rate_Hz_iaddr_offset                     0x00000000
#define rc_DVBC_sample_rate_Hz_iaddr_offset                     0x00000004
#define rc_DVBC_dmd_clk_Hz_iaddr_offset                         0x00000008
#define rc_DVBC_j83b_mode_caddr_offset                          0x00000017
#define rc_DVBC_no_signal_input_iaddr_offset                    0x00000018
#define rc_DVBC_tuner_type_caddr_offset                         0x00000024
#define rc_DVBC_input_format_caddr_offset                       0x00000025
#define rc_DVBC_spectrum_invert_caddr_offset                    0x00000026
#define rc_DVBC_input_select_caddr_offset                       0x00000027
#define rc_DVBC_if_freq_Hz_iaddr_offset                         0x00000028
#define rc_DVBC_qam_mode_iaddr_offset                           0x0000002c
#define rc_DVBC_rfagc_pol_caddr_offset						    0x00000049
#define rc_DVBC_fec_clk_Hz_iaddr_offset                         0x00000050
#define rc_DVBC_get_btr_crl_iaddr_offset                        0x00000080
#define rc_DVBC_qam_mode_scan_control_iaddr_offset              0x00000090
#define rc_DVBC_lock_mode_caddr_offset                          0x00000093
#define rc_DVBC_adc_sel_caddr_offset							0x000001ef
#define rc_DVBC_adc_use_pll_clk_caddr_offset				    0x000001ee

//DVBC status registers offset address
#define rs_DVBC_mode_status_iaddr_offset                        0x00000004
#define rs_DVBC_snr_dB_x100_saddr_offset                        0x0000000e
#define rs_DVBC_j83b_il_mode_caddr_offset                       0x0000001d
#define rs_DVBC_post_viterbi_BER_estimate_x10M_iaddr_offset     0x0000004c

#define _w_dvbc_u8(a,v)     avl_demod_write_u8(uiSlaveAddr,AVL_FW_DVBC_CONFIG_REG_BASE + a,v)
#define _w_dvbc_u32(a,v)    avl_demod_write_u32(uiSlaveAddr,AVL_FW_DVBC_CONFIG_REG_BASE + a,v)

extern AVL_DVBCQAMMode avl_dvbc_get_qam(Pmodulator_param pmod);
extern constellation_mode avl_dvbc_get_constellation(AVL_DVBCQAMMode qam);

vatek_result avl_dvbc_initial(uint8_t uiSlaveAddr,AVL_Demod_Xtal xtal)
{
    vatek_result nres = vatek_badparam;
    const PAVL_PLL_Conf0 pll = avl_get_clock(xtal,AVL_DVBC);
    uint32_t adc_clk = 0;
    
    if(pll)
        adc_clk = avl_get_adc_clock(pll,xtal,AVL_DVBC);
    
    if(adc_clk)
    {
        nres = _w_dvbc_u8(rc_DVBC_j83b_mode_caddr_offset,AVL_DVBCConfigChip0.eDVBCStandard);
        if(is_vatek_success(nres))
            nres = _w_dvbc_u32(rc_DVBC_dmd_clk_Hz_iaddr_offset,pll->m_CoreFrequency_Hz);
        if(is_vatek_success(nres))
            nres = _w_dvbc_u32(rc_DVBC_fec_clk_Hz_iaddr_offset,pll->m_CoreFrequency_Hz);
        if(is_vatek_success(nres))
            nres = _w_dvbc_u8(rc_DVBC_rfagc_pol_caddr_offset,AVL_DVBCConfigChip0.eDVBCAGCPola);

        if(is_vatek_success(nres))
        {
            uint32_t carrier_offset_hz = 0;
            if(AVL_DVBCConfigChip0.uiDVBCIFFreqHz >= adc_clk)
                carrier_offset_hz = AVL_DVBCConfigChip0.uiDVBCIFFreqHz - adc_clk;
            else carrier_offset_hz = AVL_DVBCConfigChip0.uiDVBCIFFreqHz;
            nres = _w_dvbc_u32(rc_DVBC_if_freq_Hz_iaddr_offset,carrier_offset_hz);
        }

        if(is_vatek_success(nres))
            nres = _w_dvbc_u8(rc_DVBC_adc_sel_caddr_offset,AVL_DVBCConfigChip0.eDVBCInputPath ^ 1);
        if(is_vatek_success(nres))
            nres = _w_dvbc_u32(rc_DVBC_symbol_rate_Hz_iaddr_offset,AVL_DVBCConfigChip0.uiDVBCSymbolRateSps);
        if(is_vatek_success(nres))
            nres = _w_dvbc_u8(rc_DVBC_j83b_mode_caddr_offset,AVL_DVBCConfigChip0.eDVBCStandard);

        if(is_vatek_success(nres))
        {   //DDC configuration
            nres = _w_dvbc_u8(rc_DVBC_input_format_caddr_offset,AVL_ADC_IN);
            if(is_vatek_success(nres))
                nres = _w_dvbc_u8(rc_DVBC_input_select_caddr_offset,AVL_OFFBIN);
            if(is_vatek_success(nres))
                nres = _w_dvbc_u8(rc_DVBC_tuner_type_caddr_offset,AVL_DVBC_IF);
        }

        if(is_vatek_success(nres))
        {   //ADC configuration 
            if(xtal == Xtal_16M || xtal == Xtal_24M)
                nres = _w_dvbc_u8(rc_DVBC_adc_use_pll_clk_caddr_offset,1);
            else nres = _w_dvbc_u8(rc_DVBC_adc_use_pll_clk_caddr_offset,0);
            if(is_vatek_success(nres))
                nres = _w_dvbc_u32(rc_DVBC_sample_rate_Hz_iaddr_offset,adc_clk);
        }
    }
    return nres;
}

vatek_result avl_dvbc_config(uint8_t uiSlaveAddr,Pmodulator_param pmod)
{
    vatek_result nres = vatek_success;
    nres = avl_fw_send_rxop(uiSlaveAddr,AVL_FW_CMD_HALT);
    if(is_vatek_success(nres))
        nres = _w_dvbc_u32(rc_DVBC_qam_mode_scan_control_iaddr_offset,0x0101);
    if(is_vatek_success(nres))
        nres = avl_fw_send_rxop(uiSlaveAddr,AVL_FW_CMD_ACQUIRE);
#if 0
    if(isauto)
    {
        nres = avl_fw_send_rxop(uiSlaveAddr,AVL_FW_CMD_HALT);
        if(is_vatek_success(nres))
            nres = _w_dvbc_u32(rc_DVBC_qam_mode_scan_control_iaddr_offset,0x0101);
        if(is_vatek_success(nres))
            nres = avl_fw_send_rxop(uiSlaveAddr,AVL_FW_CMD_ACQUIRE);
    }else
    {
        nres = vatek_badparam;
        if(pmod && pmod->type == modulator_j83a)
        {
            AVL_DVBCQAMMode aqam = avl_dvbc_get_qam(pmod);
            uint32_t symbolrate = pmod->bandwidth_symbolrate * 1000;

            nres = avl_fw_send_rxop(uiSlaveAddr,AVL_FW_CMD_HALT);
            if(is_vatek_success(nres))
            {
                nres = _w_dvbc_u32(rc_DVBC_symbol_rate_Hz_iaddr_offset,symbolrate);
                if(is_vatek_success(nres))
                {
                    uint32_t val = 0;
                    nres = avl_demod_read_u32(uiSlaveAddr,AVL_FW_DVBC_CONFIG_REG_BASE + rc_DVBC_qam_mode_iaddr_offset,&val);
                    if(is_vatek_success(nres))
                    {
                        val &= 0xFFFFFF00;
                        val |= aqam;
                        nres = _w_dvbc_u32(rc_DVBC_qam_mode_iaddr_offset,val);
                    }
                    if(is_vatek_success(nres))
                        nres = _w_dvbc_u8(rc_DVBC_lock_mode_caddr_offset,0);
                }
            }
            if(is_vatek_success(nres))
                nres = avl_fw_send_rxop(uiSlaveAddr,AVL_FW_CMD_ACQUIRE);
        }
    }
#endif
    return nres;
}

vatek_result avl_dvbc_check_lock(uint8_t uiSlaveAddr,Pmodulator_param pmod)
{
    uint32_t val = 0;
    vatek_result nres = avl_demod_read_u32(uiSlaveAddr,AVL_FW_DVBC_STATUS_REG_BASE + rs_DVBC_mode_status_iaddr_offset,&val);
    if(is_vatek_success(nres))
    {
        val &= 0xFF;
        if(val == 0x15)
        {   /* locked */
            if(pmod)
            {
                nres = avl_demod_read_u32(uiSlaveAddr,AVL_FW_DVBC_CONFIG_REG_BASE + rc_DVBC_qam_mode_iaddr_offset,&val);
                if(is_vatek_success(nres))
                {
                    pmod->type = modulator_j83a;
                    pmod->mod.j83a.constellation = avl_dvbc_get_constellation((AVL_DVBCQAMMode)(val& 0x7));
                    nres = (vatek_result)1;
                }
            }
        }
    }
    return nres;
}

AVL_DVBCQAMMode avl_dvbc_get_qam(Pmodulator_param pmod)
{
    switch(pmod->mod.j83a.constellation)
    {
        case j83a_qam16: return AVL_DVBC_16QAM;
        case j83a_qam32: return AVL_DVBC_32QAM;
        case j83a_qam64: return AVL_DVBC_64QAM;
        case j83a_qam128: return AVL_DVBC_128QAM;
        case j83a_qam256: return AVL_DVBC_256QAM;
    };
    return AVL_DVBC_256QAM;
}

constellation_mode avl_dvbc_get_constellation(AVL_DVBCQAMMode qam)
{
    switch (qam)
    {
        case AVL_DVBC_16QAM : return j83a_qam16;
        case AVL_DVBC_32QAM : return j83a_qam32;
        case AVL_DVBC_64QAM : return j83a_qam64;
        case AVL_DVBC_128QAM : return j83a_qam128;
        case AVL_DVBC_256QAM : return j83a_qam256;
    };
    return qam_null;
}
