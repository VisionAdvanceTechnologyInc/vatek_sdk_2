#include "./internal/avl68xx_internal.h"
#include <hal/hal_bridge_board.h>

#define MAX_II2C_READ_SIZE          64
#define MAX_II2C_WRITE_SIZE         64
#define EACH_II2C_WRITE_SIZE        (MAX_II2C_WRITE_SIZE - 3)

extern void ChunkAddr_Demod(uint32_t uiaddr, uint8_t* pBuff);
extern vatek_result avl_set_sys_clock(uint8_t uiSlaveAddr,const PAVL_PLL_Conf0 pPLL_Conf);
extern vatek_result avl_set_mpegts_clock(uint8_t uiSlaveAddr,const PAVL_PLL_Conf0 pPLL_Conf);
extern vatek_result avl_set_adc_clock(uint8_t uiSlaveAddr,const PAVL_PLL_Conf0 pPLL_Conf);

extern vatek_result avl_set_tsmode(uint8_t uiSlaveAddr,const PAVL_PLL_Conf0 pll,Pavl_ts_param pts,AVL_DemodMode demod);

#define ChunkU32_Buf(b) ((b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3])
#define ChunkU16_Buf(b) ((b[0] << 8) | b[1])

vatek_result avl_i2c_write(uint8_t addr,uint8_t* pbuf,int32_t len)
{
    vatek_result nres = hal_i2c_start((uint8_t)(addr << 1),0);
    if(is_vatek_success(nres))nres = hal_i2c_write(pbuf,len);
    if(is_vatek_success(nres))nres = hal_i2c_stop();
    return nres;
}

vatek_result avl_i2c_read(uint8_t addr,uint8_t* pbuf,int32_t len)
{
    vatek_result nres = hal_i2c_start((uint8_t)((addr << 1) | 1),0);
    if(is_vatek_success(nres))nres = hal_i2c_read(pbuf,len);
    if(is_vatek_success(nres))nres = hal_i2c_stop();
    return nres;
}

vatek_result avl_demod_read(uint8_t uiSlaveAddr,uint32_t uiOffset,uint8_t* pucBuff,uint32_t uiSize)
{
    vatek_result nres = vatek_success;
    uint8_t uctemp[3] = {0};
    ChunkAddr_Demod(uiOffset,&uctemp[0]);
    nres = avl_i2c_write(uiSlaveAddr,&uctemp[0],3);
    if(is_vatek_success(nres))
    {
        uint8_t* ptr = pucBuff;
        while(uiSize >= MAX_II2C_READ_SIZE)
        {
            nres = avl_i2c_read(uiSlaveAddr,ptr,MAX_II2C_READ_SIZE);
            if(is_vatek_success(nres))
            {
                ptr += MAX_II2C_READ_SIZE;
                uiSize -= MAX_II2C_READ_SIZE;
            }else break;
        }

        if(is_vatek_success(nres) && uiSize)
            nres = avl_i2c_read(uiSlaveAddr,ptr,uiSize);
    }
	return nres;
}

vatek_result avl_demod_read_u8(uint8_t uiSlaveAddr,uint32_t uiAddr,uint8_t* ucData)
{
    return avl_demod_read(uiSlaveAddr,uiAddr,ucData,1);
}

vatek_result avl_demod_read_u16(uint8_t uiSlaveAddr,uint32_t uiAddr,uint16_t* ucData)
{
    uint8_t pBuff[2] = {0};
    vatek_result nres = avl_demod_read(uiSlaveAddr,uiAddr,&pBuff[0],2);
    if(is_vatek_success(nres))
    {
        *ucData = ChunkU16_Buf(pBuff);
    }
    return nres;
}

vatek_result avl_demod_read_u32(uint8_t uiSlaveAddr,uint32_t uiAddr,uint32_t* ucData)
{
    uint8_t pBuff[4] = {0};
    vatek_result nres = avl_demod_read(uiSlaveAddr,uiAddr,&pBuff[0],4);
    if(is_vatek_success(nres))
    {
        *ucData = ChunkU32_Buf(pBuff);
    }
    return nres;
}

vatek_result avl_demod_write(uint8_t uiSlaveAddr,uint8_t* pucBuff,uint32_t uiSize)
{
    vatek_result nres = vatek_badparam;
    if(uiSize >= 3)
    {
        uint32_t addr = (pucBuff[0] << 16) | (pucBuff[1] << 8) | pucBuff[2];
        uint8_t* ptr = pucBuff;
        uint8_t bkaddr[4];
        uiSize -= 3;

        while(uiSize >= EACH_II2C_WRITE_SIZE)
        {
            memcpy(&bkaddr[0],ptr,4);
            ChunkAddr_Demod(addr,ptr);
            nres =avl_i2c_write(uiSlaveAddr,ptr,MAX_II2C_WRITE_SIZE);
            memcpy(ptr,&bkaddr[0],4);
            if(is_vatek_success(nres))
            {
                ptr += EACH_II2C_WRITE_SIZE;
                uiSize -= EACH_II2C_WRITE_SIZE;
            }else break;
        }

        if(is_vatek_success(nres) && uiSize)
        {
            memcpy(&bkaddr[0],ptr,4);
            ChunkAddr_Demod(addr,ptr);
            nres =avl_i2c_write(uiSlaveAddr,ptr,uiSize + 3);
            memcpy(ptr,&bkaddr[0],4);
        }
    }
    return nres;
}

vatek_result avl_demod_write_u8(uint8_t uiSlaveAddr,uint32_t uiAddr,uint8_t ucData)
{
    uint8_t pBuff[4] = {0};
    ChunkAddr_Demod(uiAddr, &pBuff[0]);
    pBuff[3] = ucData;
    return avl_demod_write(uiSlaveAddr,&pBuff[0],4);
}

vatek_result avl_demod_write_u16(uint8_t uiSlaveAddr,uint32_t uiAddr,uint16_t ucData)
{
    uint8_t pBuff[5] = {0};
    ChunkAddr_Demod(uiAddr, &pBuff[0]);
    pBuff[0] = (uint8_t)(ucData >> 8);
    pBuff[1] = (uint8_t)(ucData & 0xff);
    return avl_demod_write(uiSlaveAddr,&pBuff[0],5);
}

vatek_result avl_demod_write_u32(uint8_t uiSlaveAddr,uint32_t uiAddr,uint32_t ucData)
{
    uint8_t pBuff[7] = {0};
    ChunkAddr_Demod(uiAddr, &pBuff[0]);
    pBuff[0] = (uint8_t)(ucData >> 24);
    pBuff[1] = (uint8_t)(ucData >> 16);
    pBuff[2] = (uint8_t)(ucData >> 8);
    pBuff[3] = (uint8_t)(ucData & 0xff);
    return avl_demod_write(uiSlaveAddr,&pBuff[0],7);
}

void ChunkAddr_Demod(uint32_t uiaddr, uint8_t* pBuff)
{
    pBuff[0] =(uint8_t)(uiaddr>>16);
    pBuff[1] =(uint8_t)(uiaddr>>8);
    pBuff[2] =(uint8_t)(uiaddr);
}

const PAVL_PLL_Conf0 avl_get_clock(AVL_Demod_Xtal xtal,AVL_DemodMode demode)
{
    static const AVL_PLL_Conf0 avlclock_tables[] = 
    {
        // Note: All the unit of clock frequency in the following is Hz.
        //----------------------------------------------------------------------------------------------------------------------------------------------
        //| REF_Hz    | C_R | C_F | C_Q | M_R | M_F | M_Q | A_R | A_F | A_Q | CORE_Hz | MPEG_Hz |  ADC_Hz | D_S | D_Q | S_S | S_Q |  DDC_Hz | SDRAM_Hz |
        //----------------------------------------------------------------------------------------------------------------------------------------------
        {30000000,  3,  100,    8,   1,   36,   8,    3,  100,    8, 250000000, 270000000, 250000000, 2,   12,   2,   14, 166666667, 142857143},
        //{30000000,  3,  100,    8,   1,   36,   12,   3,  100,    8, 250000000, 180000000, 250000000, 2,   12,   2,   14, 166666667, 142857143},//please change "#if 1" to "#if 0" located at function "SetTSMode_Demod" if want to confige serial MPEG clk to 90MHz for DVBTx mode.
        //{30000000,  3,  100,    8,   1,   36,   10,   3,  100,    8, 250000000, 216000000, 250000000, 2,   12,   2,   14, 166666667, 142857143},
        {16000000,  1,   55,    7,   1,   68,   8,    1,   60,   32, 251428571, 272000000, 60000000,  2,   12,   2,   14, 160000000, 137142857},
        {24000000,  2,   84,    8,   2,   90,   8,    2,   80,   32, 252000000, 270000000, 60000000,  2,   12,   2,   14, 160000000, 137142857},
        //{24000000,  2,   84,    8,   2,   90,   12,    2,   80,   32, 252000000, 180000000, 60000000,  2,   12,   2,   14, 160000000, 137142857},//please change "#if 1" to "#if 0" located at function "SetTSMode_Demod" if want to confige serial MPEG clk to 90MHz for DVBTx mode.
        //{24000000,  2,   84,    8,   2,   90,   10,    2,   80,   32, 252000000, 216000000, 60000000,  2,   12,   2,   14, 160000000, 137142857},
        {27000000,  2,   65,    7,   2,   80,   8,    2,   74,    8, 250714286, 270000000, 249750000, 2,   12,   2,   14, 165000000, 142714286},
        {30000000,  3,  100,    8,   1,   36,   8,    3,  100,    8, 250000000, 270000000, 250000000, 2,   12,   2,   14, 166666667, 142857143}, //DVBS 
        {16000000,  1,   55,    7,   1,   68,   8,    1,   62,    8, 251428571, 272000000, 248000000, 2,   12,   2,   14, 165333333, 141714286}, 
        {24000000,  2,   84,    8,   2,   90,   8,    2,   83,    8, 252000000, 270000000, 249000000, 2,   12,   2,   14, 166000000, 142857143},
        {27000000,  2,   65,    7,   2,   80,   8,    2,   74,    8, 250714286, 270000000, 249750000, 2,   12,   2,   14, 166500000, 142714286}
    };

    if(is_current_xtal(xtal) && is_current_demodmode(demode))
    {
        if(demode == AVL_DVBSX)
        {
            return (const PAVL_PLL_Conf0)&avlclock_tables[xtal + 4];
        }else return (const PAVL_PLL_Conf0)&avlclock_tables[xtal];
    }
    return NULL;
}

uint32_t avl_get_adc_clock(const PAVL_PLL_Conf0 pll,AVL_Demod_Xtal xtal,AVL_DemodMode demode)
{
    if(demode == AVL_DVBSX)return (pll->m_ADCFrequency_Hz / 2);
    else
    {
        if(xtal == Xtal_16M || xtal == Xtal_24M)
            return (pll->m_ADCFrequency_Hz / 2);
        else if(xtal == Xtal_27M || xtal == Xtal_30M)
            return pll->m_RefFrequency_Hz;
    }
    return 0;
}

vatek_result avl_set_pll(uint8_t uiSlaveAddr,AVL_Demod_Xtal xtal,AVL_DemodMode demode)
{
    const PAVL_PLL_Conf0 pPLL_Conf = avl_get_clock(xtal,demode);
    vatek_result nres = vatek_badparam;
    if(pPLL_Conf)
    {
        nres = avl_set_sys_clock(uiSlaveAddr,pPLL_Conf);
        if(is_vatek_success(nres))
            nres = avl_set_mpegts_clock(uiSlaveAddr,pPLL_Conf);
        if(is_vatek_success(nres))
            nres = avl_set_adc_clock(uiSlaveAddr,pPLL_Conf);

        if(is_vatek_success(nres))
        {
            nres = avl_demod_write_u32(uiSlaveAddr, 0x100000, 0);
            if(is_vatek_success(nres))
                nres = avl_demod_write_u32(uiSlaveAddr, 0x100000, 1);//no I2C ACK 
        }
        if(is_vatek_success(nres))
        {
            hal_system_sleep(1);
            nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_dll_out_phase, 96);
            if(is_vatek_success(nres))
                nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_dll_rd_phase, 0);
            if(is_vatek_success(nres))
                nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_deglitch_mode, 1);
            if(is_vatek_success(nres))
                nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_dll_init, 1);
            if(is_vatek_success(nres))
                nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_dll_init, 0);
        }
    }
    return nres;
}

vatek_result avl_set_sleep_pll(uint8_t uiSlaveAddr,AVL_Demod_Xtal xtal)
{
    const PAVL_PLL_Conf0 pPLL_Conf = avl_get_clock(xtal,AVL_DVBTX);
    vatek_result nres = vatek_badparam;
    if(pPLL_Conf)
    {
        nres = avl_set_sys_clock(uiSlaveAddr,pPLL_Conf);
        if(is_vatek_success(nres))
            nres = avl_set_mpegts_clock(uiSlaveAddr,pPLL_Conf);
        if(is_vatek_success(nres))
            nres = avl_set_adc_clock(uiSlaveAddr,pPLL_Conf);
        if(is_vatek_success(nres))
        {
            nres = avl_demod_write_u32(uiSlaveAddr, 0x100000, 0);
            if(is_vatek_success(nres))
            nres = avl_demod_write_u32(uiSlaveAddr, 0x100000, 1);//no I2C ACK 
        }
        if(is_vatek_success(nres))hal_system_sleep(1);
    }
    return nres;
}

vatek_result avl_set_sys_clock(uint8_t uiSlaveAddr,const PAVL_PLL_Conf0 pPLL_Conf)
{
    uint32_t DivRefHz = pPLL_Conf->m_RefFrequency_Hz / (uint32_t)pPLL_Conf->m_PLL_CoreClock_DivR;
    uint32_t PLLRange = 1;
    vatek_result nres = vatek_success;

    if(DivRefHz < 16000000)PLLRange = 1;
    else if(DivRefHz < 25000000)PLLRange = 2;
    else PLLRange = 3;

    nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_sys_pll_divr, pPLL_Conf->m_PLL_CoreClock_DivR-1);//DIVR
    if(is_vatek_success(nres))
        nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_sys_pll_divf, pPLL_Conf->m_PLL_CoreClock_DivF-1);//DIVF
    if(is_vatek_success(nres))
        nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_sys_pll_divq, pPLL_Conf->m_PLL_CoreClock_DivQ-1);//DIVQ1
    if(is_vatek_success(nres))
        nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_sys_pll_range, PLLRange);//range
    if(is_vatek_success(nres))
        nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_sys_pll_divq2, pPLL_Conf->m_PLL_DDCClock_DivQ-1);//DIVQ2
    if(is_vatek_success(nres))
        nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_sys_pll_divq3, pPLL_Conf->m_PLL_SDRAMClock_DivQ-1);//DIVQ3
    if(is_vatek_success(nres))
        nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_sys_pll_enable2, (pPLL_Conf->m_PLL_DDCClock_sel == hw_E2_PLL_SEL_CORE));
    if(is_vatek_success(nres))
        nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_sys_pll_enable3, (pPLL_Conf->m_PLL_SDRAMClock_sel == hw_E2_PLL_SEL_CORE));
    return nres;
}

vatek_result avl_set_mpegts_clock(uint8_t uiSlaveAddr,const PAVL_PLL_Conf0 pPLL_Conf)
{
    uint32_t DivRefHz = pPLL_Conf->m_RefFrequency_Hz / (uint32_t)pPLL_Conf->m_PLL_MPEGClock_DivR;
    uint32_t PLLRange = 1;
    vatek_result nres = vatek_success;

    if(DivRefHz < 16000000)PLLRange = 1;
    else if(DivRefHz < 25000000)PLLRange = 2;
    else PLLRange = 3;

    nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_mpeg_pll_divr, pPLL_Conf->m_PLL_MPEGClock_DivR-1);
    if(is_vatek_success(nres))
        nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_mpeg_pll_divf, pPLL_Conf->m_PLL_MPEGClock_DivF-1);
    if(is_vatek_success(nres))
        nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_mpeg_pll_divq, pPLL_Conf->m_PLL_MPEGClock_DivQ-1);
    if(is_vatek_success(nres))
        nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_mpeg_pll_range, PLLRange);
    if(is_vatek_success(nres))
        nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_mpeg_pll_divq2, pPLL_Conf->m_PLL_DDCClock_DivQ-1);
    if(is_vatek_success(nres))
        nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_mpeg_pll_divq3, pPLL_Conf->m_PLL_SDRAMClock_DivQ-1);
    if(is_vatek_success(nres))
        nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_mpeg_pll_enable2, (pPLL_Conf->m_PLL_DDCClock_sel == hw_E2_PLL_SEL_MPEG));
    if(is_vatek_success(nres))
        nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_mpeg_pll_enable3, (pPLL_Conf->m_PLL_SDRAMClock_sel == hw_E2_PLL_SEL_MPEG));
    return nres;
}

vatek_result avl_set_adc_clock(uint8_t uiSlaveAddr,const PAVL_PLL_Conf0 pPLL_Conf)
{
    //adc_pll
    uint32_t DivRefHz = pPLL_Conf->m_RefFrequency_Hz / (uint32_t)pPLL_Conf->m_PLL_ADCClock_DivR;
    uint32_t PLLRange = 1;
    vatek_result nres = vatek_success;

    if(DivRefHz < 16000000)PLLRange = 1;
    else if(DivRefHz < 25000000)PLLRange = 2;
    else PLLRange = 3;

    nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_adc_pll_divr, pPLL_Conf->m_PLL_ADCClock_DivR-1);
    if(is_vatek_success(nres))
        nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_adc_pll_divf, pPLL_Conf->m_PLL_ADCClock_DivF-1);
    if(is_vatek_success(nres))
        nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_adc_pll_divq, pPLL_Conf->m_PLL_ADCClock_DivQ-1);
    if(is_vatek_success(nres))
        nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_adc_pll_range, PLLRange);
    if(is_vatek_success(nres))
        nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_adc_pll_divq2, pPLL_Conf->m_PLL_DDCClock_DivQ-1);
    if(is_vatek_success(nres))
        nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_adc_pll_divq3, pPLL_Conf->m_PLL_SDRAMClock_DivQ-1);
    if(is_vatek_success(nres))
        nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_adc_pll_enable2, (pPLL_Conf->m_PLL_DDCClock_sel == hw_E2_PLL_SEL_ADC));
    if(is_vatek_success(nres))
        nres = avl_demod_write_u32(uiSlaveAddr, hw_E2_AVLEM61_adc_pll_enable3, (pPLL_Conf->m_PLL_SDRAMClock_sel == hw_E2_PLL_SEL_ADC));
    return nres;   
}

vatek_result avl_get_family_id(uint8_t uiSlaveAddr,uint32_t* familyid)
{
    #define AVL_FAMILYID_REG 0x40000
    return avl_demod_read_u32(uiSlaveAddr,AVL_FAMILYID_REG,familyid);
}

vatek_result avl_get_chip_id(uint8_t uiSlaveAddr,uint32_t* chipid)
{
    return avl_demod_read_u32(uiSlaveAddr,AVL_MEMBER_ID_BASE,chipid);
}

vatek_result avl_get_patch_version(uint8_t uiSlaveAddr,uint32_t version)
{
    return avl_demod_read_u32(uiSlaveAddr,AVL_FW_STATUS_REG_BASE + rs_patch_ver_iaddr_offset,&version);
}

vatek_result avl_check_chip_ready(uint8_t uiSlaveAddr)
{
    vatek_result nres = vatek_success;
    uint32_t uiCoreReadyWord = 0;
    uint32_t uiCoreRunning = 0;

    nres = avl_demod_read_u32(uiSlaveAddr,AVL_MCU_RESET_BASE,&uiCoreRunning);
    if(is_vatek_success(nres))
    {
        nres = avl_demod_read_u32(uiSlaveAddr,rs_core_ready_word_iaddr_offset,&uiCoreReadyWord);
        if(is_vatek_success(nres))
        {
            if(uiCoreReadyWord != 0x5aa57ff7 || uiCoreRunning == 1)
                nres = vatek_badstatus;
        }
    }
    return nres;
}

avl_rxop_status avl_fw_get_rxop_status(uint8_t uiSlaveAddr)
{
    uint16_t uiCmd = 0;
    vatek_result nres = avl_demod_read_u16(uiSlaveAddr,AVL_FW_CONFIG_REG_BASE + rc_fw_command_saddr_offset,&uiCmd);
    if(is_vatek_success(nres))
    {
        if(uiCmd)return rxop_busy;
        else return rxop_idle;
    }else return rxop_fail;
}

vatek_result avl_fw_wait_ready(uint8_t uiSlaveAddr)
{
    vatek_result nres = vatek_success;
    avl_rxop_status status = avl_fw_get_rxop_status(uiSlaveAddr);
    uint32_t timeouttick = hal_system_get_tick();
    while(status != rxop_idle)
    {
        if(status == rxop_fail)nres = vatek_hwfail;
        else 
        {
            if((hal_system_get_tick() - timeouttick) > AVL_OPRX_TIMEOUT)nres = vatek_timeout;
            else hal_system_sleep(10);
        }
        if(!is_vatek_success(nres))break;
    }
    return nres;
}

vatek_result avl_wait_chip_ready(uint8_t uiSlaveAddr)
{
    vatek_result nres = vatek_success;
    uint32_t timeouttick = hal_system_get_tick();
    while((hal_system_get_tick() - timeouttick) < AVL_CHIPREADY_TIMEOUT)
    {
        nres = avl_fw_wait_ready(uiSlaveAddr);
        if(nres == vatek_badstatus)hal_system_sleep(20);
        else break;
    }
    return nres;
}

vatek_result avl_fw_send_rxop(uint8_t uiSlaveAddr,uint8_t rxop)
{
    vatek_result nres = avl_fw_wait_ready(uiSlaveAddr);
    if(is_vatek_success(nres))
    {
        uint8_t pucBuff[4] = {0};
        uint16_t uitemp = 0;
        pucBuff[0] = 0;
        pucBuff[1] = rxop;
        uitemp = ChunkU16_Buf(pucBuff);
        nres = avl_demod_write_u16(uiSlaveAddr,AVL_FW_CONFIG_REG_BASE + rc_fw_command_saddr_offset,uitemp);
        if(is_vatek_success(nres))
            nres = avl_fw_wait_ready(uiSlaveAddr);
    }
    return nres;
}

vatek_result avl_fw_reset(uint8_t uiSlaveAddr,AVL_Demod_Xtal xtal,AVL_DemodMode demode)
{
    vatek_result nres = avl_fw_send_rxop(uiSlaveAddr,AVL_FW_CMD_HALT);
    if(is_vatek_success(nres))
        nres = avl_fw_send_rxop(uiSlaveAddr,AVL_FW_CMD_LD_DEFAULT);
    if(is_vatek_success(nres))
        nres = avl_fw_send_rxop(uiSlaveAddr,AVL_FW_CMD_INIT_SDRAM);
    if(is_vatek_success(nres))
        nres = avl_fw_send_rxop(uiSlaveAddr,AVL_FW_CMD_INIT_ADC);
	return nres;
}

vatek_result avl_tsout_config(AVL_Demod_Xtal xtal,AVL_DemodMode demode,uint8_t uiSlaveAddr,Pavl_ts_param pts)
{
    vatek_result nres = vatek_badparam;
    const PAVL_PLL_Conf0 pll = avl_get_clock(xtal,demode);
    if(pll)
        nres = avl_set_tsmode(uiSlaveAddr,pll,pts,demode);
    return nres;
}

vatek_result avl_tsout_set_enable(uint8_t uiSlaveAddr,int32_t isenable)
{
    uint32_t val = 0xfff;
    if(isenable)val = 0;
    return avl_demod_write_u32(uiSlaveAddr,AVL_TS_TRI_STATE_CNTRL_BASE,val);
}

vatek_result avl_set_tsmode(uint8_t uiSlaveAddr,const PAVL_PLL_Conf0 pll,Pavl_ts_param pts,AVL_DemodMode demod)
{
    vatek_result nres = vatek_badparam;
    if(is_current_demodmode(demod))
    {
        nres = avl_demod_write_u8(uiSlaveAddr,AVL_FW_CONFIG_REG_BASE + rc_ts_serial_caddr_offset,pts->outputmode);
        if(is_vatek_success(nres))
            nres = avl_demod_write_u8(uiSlaveAddr,AVL_FW_CONFIG_REG_BASE + rc_ts_clock_edge_caddr_offset,pts->clockedge);

        if(pts->clockmode == AVL_TS_CONTINUOUS_ENABLE)
        {
            nres = avl_demod_write_u8(uiSlaveAddr,AVL_FW_CONFIG_REG_BASE + rc_enable_ts_continuous_caddr_offset,1);
            if(is_vatek_success(nres))
                nres = avl_demod_write_u32(uiSlaveAddr,AVL_FW_CONFIG_REG_BASE + rc_ts_cntns_clk_frac_d_iaddr_offset,pll->m_MPEGFrequency_Hz);
            if(is_vatek_success(nres))
            {
                if(pts->outputmode == AVL_TS_SERIAL)
                {
                    uint32_t tsclock = pll->m_MPEGFrequency_Hz;
                    if(demod != AVL_DVBSX)tsclock /= 2;
                    nres = avl_demod_write_u32(uiSlaveAddr,AVL_FW_CONFIG_REG_BASE + rc_ts_cntns_clk_frac_n_iaddr_offset,tsclock);
                }else nres = avl_demod_write_u32(uiSlaveAddr,AVL_FW_CONFIG_REG_BASE + rc_ts_cntns_clk_frac_n_iaddr_offset,pll->m_MPEGFrequency_Hz/8);
            }
        }else nres = avl_demod_write_u8(uiSlaveAddr,AVL_FW_CONFIG_REG_BASE + rc_enable_ts_continuous_caddr_offset,0);

        if(is_vatek_success(nres))
        {
            nres = avl_demod_write_u8(uiSlaveAddr,AVL_FW_CONFIG_REG_BASE + rc_ts_serial_outpin_caddr_offset,pts->serialpin);
            if(is_vatek_success(nres))
                nres = avl_demod_write_u8(uiSlaveAddr,AVL_FW_CONFIG_REG_BASE + rc_ts_serial_msb_caddr_offset,pts->serialorder);
            if(is_vatek_success(nres))
                nres = avl_demod_write_u8(uiSlaveAddr,AVL_FW_CONFIG_REG_BASE + rc_ts_sync_pulse_caddr_offset,pts->serialsyncpulse);
            //if(is_vatek_success(nres))
            //    nres = avl_demod_write_u8(uiSlaveAddr,AVL_FW_CONFIG_REG_BASE + rc_ts_error_bit_en_caddr_offset,pts->errbit);
            if(is_vatek_success(nres))
                nres = avl_demod_write_u8(uiSlaveAddr,AVL_FW_CONFIG_REG_BASE + rc_ts_error_polarity_caddr_offset,pts->errpolarity);
            if(is_vatek_success(nres))
                nres = avl_demod_write_u8(uiSlaveAddr,AVL_FW_CONFIG_REG_BASE + rc_ts_valid_polarity_caddr_offset,pts->validpolarity);
            if(is_vatek_success(nres))
                nres = avl_demod_write_u8(uiSlaveAddr,AVL_FW_CONFIG_REG_BASE + rc_ts_packet_len_caddr_offset,pts->packagelen);
            if(is_vatek_success(nres))
                nres = avl_demod_write_u8(uiSlaveAddr,AVL_FW_CONFIG_REG_BASE + rc_ts_packet_order_caddr_offset,pts->parallelorder);
            if(is_vatek_success(nres))
                nres = avl_demod_write_u8(uiSlaveAddr,AVL_FW_CONFIG_REG_BASE + ts_clock_phase_caddr_offset,pts->parallelphase);
        }     
    }
    return nres;
}

vatek_result avl_gpio_config(uint8_t uiSlaveAddr,AVL_GPIOPinNumber pin,AVL_GPIOPinValue mode)
{
    uint32_t addr = AVL_GPIO_DEBUG_BASE;
    uint32_t setval = (uint8_t)mode;
    vatek_result nres = vatek_success;
    if(pin == AVL_Pin37)addr += lnb_cntrl_0_sel_offset;
    else if(pin == AVL_Pin38)addr += lnb_cntrl_1_sel_offset;
    else if(pin == AVL_Pin15)addr += gpio_module_002_gpio_config_offset;
    else nres = vatek_badparam;

    if(is_vatek_success(nres))
    {
        if(mode == AVL_LOGIC_0)
        {
            if(pin == AVL_Pin15)setval = 0x02;
        }else if(mode == AVL_LOGIC_1)
        {
            if(pin == AVL_Pin15)setval = 0x03;
        }else if(mode == AVL_HIGH_Z)
        {
            if(pin == AVL_Pin15)nres = vatek_unsupport;
        }else nres = vatek_badparam;
    }

    if(is_vatek_success(nres))
        nres = avl_demod_write_u32(uiSlaveAddr,addr,setval);
    return nres;
}

vatek_result avl_gpio_get_status(uint8_t uiSlaveAddr,AVL_GPIOPinNumber pin,AVL_GPIOPinValue* mode)
{
    uint32_t addr = AVL_GPIO_DEBUG_BASE;
    uint32_t addi = AVL_GPIO_DEBUG_BASE;
    uint32_t setval = 0;
    vatek_result nres = vatek_success;
    if(pin == AVL_Pin37)
    {
        addr += lnb_cntrl_0_sel_offset;
        addi += lnb_cntrl_0_i_offset;
    }
    else if(pin == AVL_Pin38)
    {
        addr += lnb_cntrl_1_sel_offset;
        addi += lnb_cntrl_1_i_offset;
    }
    else if(pin == AVL_Pin15)
    {
        addr += gpio_module_002_gpio_config_offset;
        nres = avl_demod_read_u32(uiSlaveAddr,addr,&setval);
        if(is_vatek_success(nres))
        {
            if(setval == 0x02)*mode = AVL_LOGIC_0;
            else if(setval == 0x03)*mode = AVL_LOGIC_1;
            else nres = vatek_format;
        }
        return nres;
    }
    else nres = vatek_badparam;

    if(is_vatek_success(nres))
    {
        nres = avl_demod_write_u32(uiSlaveAddr,addr,AVL_HIGH_Z);
        if(is_vatek_success(nres))
            nres = avl_demod_read_u32(uiSlaveAddr,addi,&setval);
        if(is_vatek_success(nres))
        {
            if(setval == AVL_LOGIC_0)*mode = AVL_LOGIC_0;
            else if(setval == AVL_LOGIC_1)*mode = AVL_LOGIC_1;
            else if(setval == AVL_HIGH_Z)*mode = AVL_HIGH_Z;
            else nres = vatek_format;
        }
    }
    return nres;
}

vatek_result avl_tuner_i2c_config(uint8_t uiSlaveAddr,AVL_Demod_Xtal xtal,AVL_DemodMode demode)
{
    vatek_result nres = vatek_badparam;
    const PAVL_PLL_Conf0 pll = avl_get_clock(xtal,demode);
    if(pll)
    {
        uint32_t val = 0;
        nres = avl_demod_write_u32(uiSlaveAddr,AVL_TUNER_I2C_BASE+tuner_i2c_srst_offset,1);
        if(is_vatek_success(nres))
            nres = avl_demod_write_u32(uiSlaveAddr,AVL_TUNER_I2C_BASE + tuner_i2c_bit_rpt_cntrl_offset,6);
        if(is_vatek_success(nres))
        {
            nres = avl_demod_read_u32(uiSlaveAddr,AVL_TUNER_I2C_BASE + tuner_i2c_cntrl_offset,&val);
            if(is_vatek_success(nres))
            {
                val &= 0xFFFFFFFE;
                nres = avl_demod_write_u32(uiSlaveAddr,AVL_TUNER_I2C_BASE + tuner_i2c_cntrl_offset,val);
            }
        }

        if(is_vatek_success(nres))
        {
            uint32_t clk = 0x2A * (pll->m_CoreFrequency_Hz/1000) / (240*1000);
            nres = avl_demod_write_u32(uiSlaveAddr,AVL_TUNER_I2C_BASE + tuner_i2c_bit_rpt_clk_div_offset,clk);
            if(is_vatek_success(nres))
                nres = avl_demod_write_u32(uiSlaveAddr,AVL_TUNER_I2C_BASE + tuner_i2c_srst_offset,0);
        }
    }
    return nres;
}

vatek_result avl_tuner_i2c_set_enable(uint8_t uiSlaveAddr,int32_t isenable)
{
    vatek_result nres = vatek_success;
    uint32_t val = 0x06;
    int32_t i = 0;

    if(isenable)val = 0x07;
    for(i = 0;i < 3;i++)
    {
        nres = avl_demod_write_u32(uiSlaveAddr,AVL_TUNER_I2C_BASE + tuner_i2c_bit_rpt_cntrl_offset,val);
        if(!is_vatek_success(nres))break;
    }
    return nres;
}

vatek_result avl_agc_config(uint8_t uiSlaveAddr,AVL_DemodMode demode,int32_t isenable)
{
    uint32_t val = 2;
    vatek_result nres = vatek_success;
    if(isenable)val = 6;

    if(demode == AVL_DVBSX)nres = avl_demod_write_u32(uiSlaveAddr,AVL_GPIO_DEBUG_BASE + agc2_sel_offset,val);
    else nres = avl_demod_write_u32(uiSlaveAddr,AVL_GPIO_DEBUG_BASE + agc1_sel_offset,val);
    return nres;
}
