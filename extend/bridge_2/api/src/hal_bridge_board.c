#include <hal/hal_bridge_board.h>

_HAL_WEAK(void,hal_system_sleep)(uint32_t ms)
{

}

_HAL_WEAK(uint32_t,hal_system_get_tick)(void)
{
    return (uint32_t)clock();
}

_HAL_WEAK(vatek_result,hal_board_open)(void)
{
    return vatek_unsupport;
}

_HAL_WEAK(vatek_result,hal_gpio_config)(hhal_pin pin,hal_gpio_mode mode)
{
    return vatek_unsupport;
}

_HAL_WEAK(vatek_result,hal_gpio_enable_interrupt)(hhal_pin pin,hal_gpio_trigger trigger,fpgpio_handler phandler,void* param)
{
    return vatek_unsupport;
}

_HAL_WEAK(vatek_result,hal_gpio_disable_interrupt)(hhal_pin pin)
{
    return vatek_unsupport;
}

_HAL_WEAK(vatek_result,hal_gpio_set)(hhal_pin pin,int32_t isset)
{
    return vatek_unsupport;
}

_HAL_WEAK(vatek_result,hal_gpio_get)(hhal_pin pin)
{
    return vatek_unsupport;
}

_HAL_WEAK(vatek_result,hal_i2c_config)(uint32_t speedkhz)
{
    return vatek_unsupport;
}

_HAL_WEAK(vatek_result,hal_i2c_check_device)(uint8_t devaddr)
{
    return vatek_unsupport;
}

_HAL_WEAK(vatek_result,hal_i2c_start)(uint8_t devaddr,int32_t restart)
{
    return vatek_unsupport;
}

_HAL_WEAK(vatek_result,hal_i2c_write)(const uint8_t* pbuf,int32_t len)
{
    return vatek_unsupport;
}

_HAL_WEAK(vatek_result,hal_i2c_read)(uint8_t* pbuf,int32_t len)
{
    return vatek_unsupport;
}

_HAL_WEAK(vatek_result,hal_i2c_stop)(void)
{
    return vatek_unsupport;
}

_HAL_WEAK(vatek_result,hal_uart_config)(uint32_t baudrate)
{
    return vatek_unsupport;
}

_HAL_WEAK(vatek_result,hal_uart_read)(int32_t* nkey,int32_t iswait)
{
    return vatek_unsupport;
}

_HAL_WEAK(vatek_result,hal_uart_write)(const char* szkeys)
{
    return vatek_unsupport;
}

_HAL_WEAK(vatek_result,hal_spi_config)(hal_spi_mode mode,hal_spi_buslen bus,hal_spi_bit bit)
{
    return vatek_unsupport;
}

_HAL_WEAK(vatek_result,hal_spi_write)(uint8_t* pbuf,int32_t len)
{
    return vatek_unsupport;
}

_HAL_WEAK(vatek_result,hal_spi_read)(uint8_t* pbuf,int32_t len)
{
    return vatek_unsupport;
}

_HAL_WEAK(void,hal_board_close)(void)
{

}

_HAL_WEAK(vatek_result,hal_storage_lock_section)(uint8_t** psection)
{
    return vatek_unsupport;
}

_HAL_WEAK(void,hal_storage_unlock_section)(void)
{
    
}

_HAL_WEAK(vatek_result,hal_storage_erase)(uint32_t section)
{
    return vatek_unsupport;
}

_HAL_WEAK(vatek_result,hal_storage_write)(uint32_t section,uint8_t* psection)
{
    return vatek_unsupport;
}

_HAL_WEAK(vatek_result,hal_storage_read)(uint32_t section,uint8_t* psection)
{
    return vatek_unsupport;
}
