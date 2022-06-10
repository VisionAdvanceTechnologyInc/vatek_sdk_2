#include <hal/hal_bridge_board.h>
#include <stm32_core.h>
#include <stm32_usb_bridge.h>

extern void SystemClock_Config(void);   /* implemented in main.c */

void hal_system_sleep(uint32_t ms)
{
    HAL_Delay(ms);
}

uint32_t hal_system_get_tick(void)
{
    return HAL_GetTick();
}

vatek_result hal_board_open(void)
{
    static int32_t stmhal_init = 0;
    vatek_result nres = vatek_success;
    if(!stmhal_init)
    {
        HAL_Init();
        SystemClock_Config();
        MX_GPIO_Init();
        MX_USART1_UART_Init();

        nres = stm32_core_init(&huart1);
        if(is_vatek_success(nres))
        {
        }
        stmhal_init = 1;
    }
    return nres;
}

vatek_result hal_gpio_config(hhal_pin pin,hal_gpio_mode mode)
{
	Pstm32_pin spin = stm32_core_get_pin(pin);
	vatek_result nres = vatek_badparam;
	if(spin)
		nres = stm32_core_pin_config(spin,mode,0);
    return nres;
}

vatek_result hal_gpio_enable_interrupt(hhal_pin pin,hal_gpio_trigger trigger,fpgpio_handler phandler,void* param)
{
    return vatek_unsupport;
}

vatek_result hal_gpio_disable_interrupt(hhal_pin pin)
{
    return vatek_unsupport;
}

vatek_result hal_gpio_set(hhal_pin pin,int32_t isset)
{
	Pstm32_pin spin = stm32_core_get_pin(pin);
	vatek_result nres = vatek_badparam;
	if(spin)
		nres = stm32_core_pin_set_status(spin,isset);
    return nres;
}

vatek_result hal_gpio_get(hhal_pin pin)
{
	Pstm32_pin spin = stm32_core_get_pin(pin);
	vatek_result nres = vatek_badparam;
	if(spin)
		nres = stm32_core_pin_get_status(spin);
    return nres;
}

vatek_result hal_i2c_config(uint32_t speedkhz)
{
    vatek_result nres = vatek_unsupport;
    Pstm32_i2c pi2c = stm32_core_get_main_i2c();
    if(pi2c)
        nres = pi2c->set_speed(pi2c->hi2c,speedkhz);
    return nres;
}

vatek_result hal_i2c_check_device(uint8_t devaddr)
{
	return stm32_core_main_i2c_check_device(devaddr);
}

vatek_result hal_i2c_start(uint8_t devaddr,int32_t restart)
{
    vatek_result nres = vatek_unsupport;
    Pstm32_i2c pi2c = stm32_core_get_main_i2c();
    if(pi2c)
        nres = pi2c->start(pi2c->hi2c,devaddr,restart);
    return nres;  
}

vatek_result hal_i2c_write(const uint8_t* pbuf,int32_t len)
{
    vatek_result nres = vatek_unsupport;
    Pstm32_i2c pi2c = stm32_core_get_main_i2c();
    if(pi2c)
        nres = pi2c->write(pi2c->hi2c,pbuf,len);
    return nres; 
}

vatek_result hal_i2c_read(uint8_t* pbuf,int32_t len)
{
    vatek_result nres = vatek_unsupport;
    Pstm32_i2c pi2c = stm32_core_get_main_i2c();
    if(pi2c)
        nres = pi2c->read(pi2c->hi2c,pbuf,len);
    return nres; 
}

vatek_result hal_i2c_stop(void)
{
    vatek_result nres = vatek_unsupport;
    Pstm32_i2c pi2c = stm32_core_get_main_i2c();
    if(pi2c)
        nres = pi2c->stop(pi2c->hi2c);
    return nres; 
}

vatek_result hal_uart_config(uint32_t baudrate)
{
    vatek_result nres = vatek_unsupport;
    Pstm32_uart puart = stm32_core_get_main_uart();
    if(puart)nres = vatek_success;
    return nres;
}

vatek_result hal_uart_read(int32_t* nkey,int32_t iswait)
{
    vatek_result nres = vatek_success;
    Pstm32_uart puart = stm32_core_get_main_uart();
    if(puart)*nkey = puart->get_char(puart->huart,iswait);
    else nres = vatek_unsupport;
    return nres;
}

vatek_result hal_uart_write(const char* szkeys)
{
    vatek_result nres = vatek_unsupport;
    Pstm32_uart puart = stm32_core_get_main_uart();
    if(puart)nres = puart->put_string(puart->huart,szkeys);
    return nres;
}

vatek_result hal_spi_config(hal_spi_mode mode,hal_spi_buslen bus,hal_spi_bit bit)
{
    vatek_result nres = vatek_unsupport;
    Pstm32_spi pspi = stm32_core_get_main_spi();
    if(pspi)
    {
        hal_spi_param param;
        param.bit = bit;
        param.buslen = bus;
        param.mode = mode;
        nres = pspi->config(pspi->hspi,&param);
    }
    return nres;
}

vatek_result hal_spi_write(uint8_t* pbuf,int32_t len)
{
    vatek_result nres = vatek_unsupport;
    Pstm32_spi pspi = stm32_core_get_main_spi();
    if(pspi)nres = pspi->write(pspi->hspi,pbuf,len);
    return nres;
}

vatek_result hal_spi_read(uint8_t* pbuf,int32_t len)
{
    vatek_result nres = vatek_unsupport;
    Pstm32_spi pspi = stm32_core_get_main_spi();
    if(pspi)nres = pspi->read(pspi->hspi,pbuf,len);
    return nres;
}

void hal_board_close(void)
{

}

vatek_result hal_storage_lock_section(uint8_t** psection)
{
    return vatek_unsupport;
}

void hal_storage_unlock_section(void)
{

}

vatek_result hal_storage_erase(uint32_t section)
{
    return vatek_unsupport;
}

vatek_result hal_storage_write(uint32_t section,uint8_t* psection)
{
    return vatek_unsupport;
}

vatek_result hal_storage_read(uint32_t section,uint8_t* psection)
{
    return vatek_unsupport;
}
