#ifndef _AVL68XX_REG_DEFINE_
#define _AVL68XX_REG_DEFINE_

#define AVL_MCU_RESET_BASE                                  0x110840
#define AVL_MCU_SYSTEM_RESET_BASE                           0x110010
#define AVL_ESM_BASE                                        0x132050
#define AVL_TUNER_I2C_BASE                                  0x118000
#define AVL_GPIO_CONTROL_BASE                               0x124000
#define AVL_GPIO_DEBUG_BASE                                 0x124000
#define AVL_GPIO_MODU_002_BASE                              0x124010
#define AVL_EMERALD_IO_BASE                                 0x10c000
#define AVL_TS_TRI_STATE_CNTRL_BASE                         0x10c000
#define AVL_AGC_TRI_STATE_CNTRL_BASE                        0x120000   
#define AVL_DISEQC_BASE                                     0x16c000
#define AVL_PLP_LIST_BASE                                   0x16c000
#define AVL_BLIND_SCAN_INFO_BASE                            0x200C00
#define AVL_MEMBER_ID_BASE                                  0x108000
#define AVL_DMA_SYS_STATUS_BASE                             0x110048
#define AVL_DMA_SYS_CMD_BASE                                0x110050
#define AVL_DVB_GEN1_FEC_BASE                               0x140000


//Emerald2  PLL
#define hw_E2_AVLEM61_reset_register                        0x00100000
#define hw_E2_AVLEM61_dll_init                              0x00100008
#define hw_E2_AVLEM61_deglitch_mode                         0x00100010
#define hw_E2_AVLEM61_sys_pll_bypass                        0x00100040
#define hw_E2_AVLEM61_sys_pll_enable                        0x00100044
#define hw_E2_AVLEM61_sys_pll_divr                          0x00100048
#define hw_E2_AVLEM61_sys_pll_divf                          0x0010004c
#define hw_E2_AVLEM61_sys_pll_divq                          0x00100050
#define hw_E2_AVLEM61_sys_pll_range                         0x00100054
#define hw_E2_AVLEM61_sys_pll_lock                          0x00100058
#define hw_E2_AVLEM61_mpeg_pll_bypass                       0x0010005c
#define hw_E2_AVLEM61_mpeg_pll_enable                       0x00100060
#define hw_E2_AVLEM61_mpeg_pll_divr                         0x00100064
#define hw_E2_AVLEM61_mpeg_pll_divf                         0x00100068
#define hw_E2_AVLEM61_mpeg_pll_divq                         0x0010006c
#define hw_E2_AVLEM61_mpeg_pll_range                        0x00100070
#define hw_E2_AVLEM61_mpeg_pll_lock                         0x00100074
#define hw_E2_AVLEM61_adc_pll_bypass                        0x00100078 
#define hw_E2_AVLEM61_adc_pll_enable                        0x0010007c
#define hw_E2_AVLEM61_adc_pll_divr                          0x00100080
#define hw_E2_AVLEM61_adc_pll_divf                          0x00100084
#define hw_E2_AVLEM61_adc_pll_divq                          0x00100088
#define hw_E2_AVLEM61_adc_pll_range                         0x0010008c
#define hw_E2_AVLEM61_adc_pll_lock                          0x00100090
#define hw_E2_AVLEM61_mpeg_pll_reset                        0x00100094
#define hw_E2_AVLEM61_adc_pll_reset                         0x00100098
#define hw_E2_AVLEM61_sys_pll_reset                         0x0010009c
#define hw_E2_AVLEM61_sys_pll_enable2                       0x001000b4
#define hw_E2_AVLEM61_sys_pll_enable3                       0x001000b8
#define hw_E2_AVLEM61_sys_pll_divq2                         0x001000bc
#define hw_E2_AVLEM61_sys_pll_divq3                         0x001000c0
#define hw_E2_AVLEM61_mpeg_pll_enable2                      0x001000c4
#define hw_E2_AVLEM61_mpeg_pll_enable3                      0x001000c8
#define hw_E2_AVLEM61_mpeg_pll_divq2                        0x001000cc
#define hw_E2_AVLEM61_mpeg_pll_divq3                        0x001000d0
#define hw_E2_AVLEM61_adc_pll_enable2                       0x001000d4
#define hw_E2_AVLEM61_adc_pll_enable3                       0x001000d8
#define hw_E2_AVLEM61_adc_pll_divq2                         0x001000dc
#define hw_E2_AVLEM61_adc_pll_divq3                         0x001000e0
#define hw_E2_AVLEM61_ddc_clk_sel                           0x001000e4
#define hw_E2_AVLEM61_sdram_clk_sel                         0x001000e8
#define hw_E2_AVLEM61_dll_out_phase                         0x00100100
#define hw_E2_AVLEM61_dll_rd_phase                          0x00100104

#define AVL_FW_CONFIG_REG_BASE                              0x200
#define AVL_FW_STATUS_REG_BASE                              0x0a4
#define AVL_FW_DVBTX_CONFIG_REG_BASE                        0xa00
#define AVL_FW_DVBTX_STATUS_REG_BASE                        0x800
#define AVL_FW_DVBT2_DATA_PLP_CONFIG_REG_BASE               0x868
#define AVL_FW_DVBT2_COMMON_PLP_CONFIG_REG_BASE             0x884
#define AVL_FW_DVBT2_P1_REG_BASE                            0x800
#define AVL_FW_DVBT2_L1_PRE_REG_BASE                        0x808
#define AVL_FW_DVBT2_L1_POST_CONFIG_REG_BASE                0x830
#define AVL_FW_DVBT_TPS_REG_BASE                            0x8f0
#define AVL_FW_DVBSX_CONFIG_REG_BASE                        0xe00
#define AVL_FW_DVBSX_STATUS_REG_BASE                        0xc00
#define AVL_FW_ISDBT_CONFIG_REG_BASE                        0xa00
#define AVL_FW_ISDBT_STATUS_REG_BASE                        0x800
#define AVL_FW_DVBC_CONFIG_REG_BASE                         0x600
#define AVL_FW_DVBC_STATUS_REG_BASE                         0x400

// TS OUTPUT OFFECT REGISTER
#define rc_ts_cntns_clk_frac_d_iaddr_offset                 0x0000007c
#define rc_ts_cntns_clk_frac_n_iaddr_offset                 0x00000078
#define rc_enable_ts_continuous_caddr_offset                0x0000003a
#define rc_ts_clock_edge_caddr_offset                       0x0000003b
#define rc_ts_serial_caddr_offset                           0x0000003c
#define rc_ts_serial_outpin_caddr_offset                    0x0000003f
#define rc_ts_serial_msb_caddr_offset                       0x0000003e
#define rc_ts_packet_len_caddr_offset                       0x00000039
#define rc_ts_packet_order_caddr_offset                     rc_ts_serial_msb_caddr_offset
#define rc_ts_error_bit_en_caddr_offset                     0x000000000
#define rc_ts_error_polarity_caddr_offset                   0x00000041
#define rc_ts_valid_polarity_caddr_offset                   0x00000040
#define rc_ts_sync_pulse_caddr_offset                       0x00000097
#define ts_clock_phase_caddr_offset                         0x00000096

#define rc_fw_command_saddr_offset                          0x00
//#define rc_fw_last_command_saddr_offset                     0x02
#define rs_core_ready_word_iaddr_offset                     0xa0
//#define rc_sdram_test_return_iaddr_offset                   0x3C
//#define rc_sdram_test_result_iaddr_offset                   0x40

#define rs_patch_ver_iaddr_offset                           0x00000004

// Tuner I2C Register
#define tuner_i2c_srst_offset                               0x0
#define tuner_i2c_cntrl_offset                              0x4
#define tuner_i2c_bit_rpt_clk_div_offset                    0x18
#define tuner_i2c_bit_rpt_cntrl_offset                      0x1C

// AGC
#define agc1_sel_offset                                     0x00
#define agc2_sel_offset                                     0x10
// GPIO PIN37 & PIN38
#define lnb_cntrl_1_sel_offset                              0x08 
#define lnb_cntrl_0_sel_offset                              0x0c 
#define lnb_cntrl_1_i_offset                                0x48
#define lnb_cntrl_0_i_offset                                0x4c
// GPIO PIN15
#define gpio_module_002_gpio_config_offset                  0x00 

#endif
