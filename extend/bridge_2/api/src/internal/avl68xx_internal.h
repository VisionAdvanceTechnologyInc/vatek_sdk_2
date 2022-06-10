#ifndef _AVL68xx_INTERBAL_
#define _AVL68xx_INTERBAL_

#include <driver/demod/demod_avl68xx.h>
#include "avl68xx_reg_define.h"
#include "avl68xx_type_define.h"

#define AVL_OPRX_TIMEOUT                                    200
#define AVL_CHIPREADY_TIMEOUT                               2000

#define hw_E2_PLL_SEL_CORE                                  0
#define hw_E2_PLL_SEL_MPEG                                  1
#define hw_E2_PLL_SEL_ADC                                   2

#define AVL68XX_ID                                          0x68624955

#define AVL_FW_CMD_IDLE                                     0
#define AVL_FW_CMD_LD_DEFAULT                               1
#define AVL_FW_CMD_ACQUIRE                                  2
#define AVL_FW_CMD_HALT                                     3 
#define AVL_FW_CMD_DEBUG                                    4
#define AVL_FW_CMD_SLEEP                                    7
#define AVL_FW_CMD_WAKE                                     8
#define AVL_FW_CMD_BLIND_SCAN                               9
#define AVL_FW_CMD_SDRAM_TEST                               16
#define AVL_FW_CMD_INIT_SDRAM                               17
#define AVL_FW_CMD_INIT_ADC                                 18
#define AVL_FW_CMD_CHANGE_MODE                              19

#define AVL_FW_CMD_DMA                                      21
#define AVL_FW_CMD_CALC_CRC                                 22
#define AVL_FW_CMD_PING                                     23
#define AVL_FW_CMD_DECOMPRESS                               24

#define AVL_FW_PAGE_SIZE                                    256

typedef struct _avl_patch
{
    int32_t size;
    int32_t position;
    uint8_t buf[AVL_FW_PAGE_SIZE];
    int32_t page_pos;
    int32_t storage_addr;
}avl_patch,*Pavl_patch;

typedef enum _avl_rxop_status
{
    rxop_fail = -1,
    rxop_idle = 0,
    rxop_busy = 1,
}avl_rxop_status;

typedef vatek_result (*fpdemod_initial)(uint8_t uiSlaveAddr,AVL_Demod_Xtal xtal);
typedef vatek_result (*fpdemod_config)(uint8_t uiSlaveAddr,Pmodulator_param pmod);
typedef vatek_result (*fpdemod_check_lock)(uint8_t uiSlaveAddr,Pmodulator_param pmod);

typedef struct _avl_demod_mode
{
    fpdemod_initial initial;
    fpdemod_config config;
    fpdemod_check_lock check;
}avl_demod_mode,*Pavl_demod_mode;

#ifdef __cplusplus
extern "C" {
#endif

    vatek_result avl_i2c_write(uint8_t addr,uint8_t* pbuf,int32_t len);
    vatek_result avl_i2c_read(uint8_t addr,uint8_t* pbuf,int32_t len);

    vatek_result avl_demod_read(uint8_t uiSlaveAddr,uint32_t uiOffset,uint8_t* pucBuff,uint32_t uiSize);
    vatek_result avl_demod_read_u8(uint8_t uiSlaveAddr,uint32_t uiAddr,uint8_t* ucData);
    vatek_result avl_demod_read_u16(uint8_t uiSlaveAddr,uint32_t uiAddr,uint16_t* ucData);
    vatek_result avl_demod_read_u32(uint8_t uiSlaveAddr,uint32_t uiAddr,uint32_t* ucData);

    vatek_result avl_demod_write(uint8_t uiSlaveAddr,uint8_t* pucBuff,uint32_t uiSize);
    vatek_result avl_demod_write_u8(uint8_t uiSlaveAddr,uint32_t uiAddr,uint8_t ucData);
    vatek_result avl_demod_write_u16(uint8_t uiSlaveAddr,uint32_t uiAddr,uint16_t ucData);
    vatek_result avl_demod_write_u32(uint8_t uiSlaveAddr,uint32_t uiAddr,uint32_t ucData);

    vatek_result avl_get_family_id(uint8_t uiSlaveAddr,uint32_t* familyid);
    vatek_result avl_get_chip_id(uint8_t uiSlaveAddr,uint32_t* chipid);

    vatek_result avl_get_patch_version(uint8_t uiSlaveAddr,uint32_t version);

    vatek_result avl_check_chip_ready(uint8_t uiSlaveAddr);
    vatek_result avl_wait_chip_ready(uint8_t uiSlaveAddr);

    const PAVL_PLL_Conf0 avl_get_clock(AVL_Demod_Xtal xtal,AVL_DemodMode demode);
    uint32_t avl_get_adc_clock(const PAVL_PLL_Conf0 pll,AVL_Demod_Xtal xtal,AVL_DemodMode demode);

    vatek_result avl_set_pll(uint8_t uiSlaveAddr,AVL_Demod_Xtal xtal,AVL_DemodMode demode);
    vatek_result avl_set_sleep_pll(uint8_t uiSlaveAddr,AVL_Demod_Xtal xtal);

    vatek_result avl_fw_reset(uint8_t uiSlaveAddr,AVL_Demod_Xtal xtal,AVL_DemodMode demode);
    vatek_result avl_fw_wait_ready(uint8_t uiSlaveAddr);
    vatek_result avl_fw_send_rxop(uint8_t uiSlaveAddr,uint8_t rxop);
    avl_rxop_status avl_fw_get_rxop_status(uint8_t uiSlaveAddr);

    vatek_result avl_tsout_config(AVL_Demod_Xtal xtal,AVL_DemodMode demode,uint8_t uiSlaveAddr,Pavl_ts_param pts);
    vatek_result avl_tsout_set_enable(uint8_t uiSlaveAddr,int32_t isenable);

    vatek_result avl_gpio_config(uint8_t uiSlaveAddr,AVL_GPIOPinNumber pin,AVL_GPIOPinValue mode);
    vatek_result avl_gpio_get_status(uint8_t uiSlaveAddr,AVL_GPIOPinNumber pin,AVL_GPIOPinValue* mode);

    vatek_result avl_tuner_i2c_config(uint8_t uiSlaveAddr,AVL_Demod_Xtal xtal,AVL_DemodMode demode);
    vatek_result avl_tuner_i2c_set_enable(uint8_t uiSlaveAddr,int32_t isenable);

    vatek_result avl_agc_config(uint8_t uiSlaveAddr,AVL_DemodMode demode,int32_t isenable);

    vatek_result avl_dvbc_initial(uint8_t uiSlaveAddr,AVL_Demod_Xtal xtal);
    vatek_result avl_dvbc_config(uint8_t uiSlaveAddr,Pmodulator_param pmod);
    vatek_result avl_dvbc_check_lock(uint8_t uiSlaveAddr,Pmodulator_param pmod);

    vatek_result avl_dvbtx_initial(uint8_t uiSlaveAddr,AVL_Demod_Xtal xtal);
    vatek_result avl_dvbtx_config(uint8_t uiSlaveAddr,Pmodulator_param pmod);
    vatek_result avl_dvbtx_check_lock(uint8_t uiSlaveAddr,Pmodulator_param pmod);

    vatek_result avl_isdb_t_initial(uint8_t uiSlaveAddr,AVL_Demod_Xtal xtal);
    vatek_result avl_isdb_t_config(uint8_t uiSlaveAddr,Pmodulator_param pmod);
    vatek_result avl_isdb_t_check_lock(uint8_t uiSlaveAddr,Pmodulator_param pmod);

#ifdef __cplusplus
}
#endif

#endif
