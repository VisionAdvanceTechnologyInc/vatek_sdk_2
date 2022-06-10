#include <stm32_core.h>

extern vatek_result stm32_spi_config(hstm32_spi hspi,Phal_spi_param param);
extern vatek_result stm32_spi_set_cs(hstm32_spi hspi,int32_t set);
extern vatek_result stm32_spi_write(hstm32_spi hspi,uint8_t* pbuf,int32_t len);
extern vatek_result stm32_spi_read(hstm32_spi hspi,uint8_t* pbuf,int32_t len);

typedef struct _stm32_spi_handle
{
    SPI_HandleTypeDef* hspi;
    Pstm32_pin hpin_cs;
}stm32_spi_handle,*Pstm32_spi_handle;

vatek_result stm32_core_create_spi(SPI_HandleTypeDef* spi,Pstm32_pin pincs,Pstm32_spi* pspidrv)
{
    Pstm32_spi newspi = (Pstm32_spi)malloc(sizeof(stm32_spi) + sizeof(stm32_spi_handle));
    vatek_result nres = vatek_success;
    if(newspi == NULL)_S_ERR("malloc spi fail");
    else
    {
        Pstm32_spi_handle psspi = (Pstm32_spi_handle)&((uint8_t*)newspi)[sizeof(stm32_spi)];
        memset(newspi,0,sizeof(stm32_spi) + + sizeof(stm32_spi_handle));
        
        if(pincs != NULL)
        {
                nres = stm32_core_pin_config(pincs,hal_gpio_output_pp,1);     
        }
        
        if(!is_vatek_success(nres))_S_ERR("create spi cs pin fail : %d",nres);
        else
        {
            psspi->hspi = spi;
			psspi->hpin_cs = pincs;
			
            newspi->hspi = psspi;
            newspi->config = stm32_spi_config;
            newspi->set_cs = stm32_spi_set_cs;
            newspi->write = stm32_spi_write;
            newspi->read = stm32_spi_read;
            *pspidrv = newspi;    
        }      
    }

    return nres;
}

vatek_result stm32_core_free_spi(Pstm32_spi pspidrv)
{
    free(pspidrv);
    return vatek_success;
}

vatek_result stm32_spi_config(hstm32_spi hspi,Phal_spi_param param)
{
    vatek_result nres = vatek_hwfail;
    HAL_StatusTypeDef sres = HAL_OK;
    SPI_HandleTypeDef* spi = ((Pstm32_spi_handle)hspi)->hspi;
    
    if(param->buslen == spi_bus_len_16)spi->Init.DataSize = SPI_DATASIZE_16BIT;
    else spi->Init.DataSize = SPI_DATASIZE_8BIT;
    
    spi->Init.CLKPolarity = SPI_POLARITY_LOW;
    spi->Init.CLKPhase = SPI_PHASE_1EDGE;
    
    if(param->mode == spi_mode_1)
    {
        spi->Init.CLKPolarity = SPI_POLARITY_LOW;
        spi->Init.CLKPhase = SPI_PHASE_2EDGE;        
    }else if(param->mode == spi_mode_2)
    {
        spi->Init.CLKPolarity = SPI_POLARITY_HIGH;
        spi->Init.CLKPhase = SPI_PHASE_1EDGE;       
    }else if(param->mode == spi_mode_3)
    {
        spi->Init.CLKPolarity = SPI_POLARITY_HIGH;
        spi->Init.CLKPhase = SPI_PHASE_2EDGE;          
    }
    
    if(param->bit == spi_bit_lsb)spi->Init.FirstBit = SPI_FIRSTBIT_LSB;
    else spi->Init.FirstBit = SPI_FIRSTBIT_MSB;
    
    sres = HAL_SPI_Init(spi);
    if (sres != HAL_OK)_S_ERR("spi config fail : %d",sres);
    else nres = vatek_success;
    return nres;
}

vatek_result stm32_spi_set_cs(hstm32_spi hspi,int32_t set)
{
    Pstm32_spi_handle pspi = (Pstm32_spi_handle)hspi;
    if(pspi->hpin_cs == NULL)return vatek_success;
    else return stm32_core_pin_set_status(pspi->hpin_cs,set);
}

vatek_result stm32_spi_write(hstm32_spi hspi,uint8_t* pbuf,int32_t len)
{
    SPI_HandleTypeDef* spi = ((Pstm32_spi_handle)hspi)->hspi;
    HAL_StatusTypeDef sres;
    vatek_result nres = vatek_hwfail;
    
    sres = HAL_SPI_Transmit(spi,pbuf,len,STM32_TIMEOUT_MS);
    if(sres != HAL_OK)_S_ERR("spi transmit fail : %d",sres);
    else nres = vatek_success;
    return nres;
}

vatek_result stm32_spi_read(hstm32_spi hspi,uint8_t* pbuf,int32_t len)
{
    SPI_HandleTypeDef* spi = ((Pstm32_spi_handle)hspi)->hspi;
    HAL_StatusTypeDef sres;
    vatek_result nres = vatek_hwfail;
    
    sres = HAL_SPI_Receive(spi,pbuf,len,STM32_TIMEOUT_MS);
    if(sres != HAL_OK)_S_ERR("spi transmit fail : %d",sres);
    else nres = vatek_success;
    return nres;
}
