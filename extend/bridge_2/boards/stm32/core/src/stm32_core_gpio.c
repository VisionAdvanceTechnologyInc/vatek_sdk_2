#include <stm32_core.h>
#include "inc/stm32_internal.h"

Pstm32_pin stm32_core_get_pin(hhal_pin pin)
{
    Pstm32_hw_ip phwpins = stm32_internal_get_main_gpio_ip();
    if(phwpins)
    {
        int32_t i = 0;
        while(phwpins->pins[i].port)
        {
            if(phwpins->pins[i].map == pin)
                return (Pstm32_pin)&phwpins->pins[i];
			i++;
        }
    }
    return NULL;
}

vatek_result stm32_core_pin_config(Pstm32_pin spin,hal_gpio_mode mode,int32_t isset)
{
    GPIO_InitTypeDef gpio;
    gpio.Pin = spin->index;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_LOW;
    if(mode == hal_gpio_output)gpio.Mode = GPIO_MODE_OUTPUT_OD;
    else if(mode == hal_gpio_output_pp)gpio.Mode = GPIO_MODE_OUTPUT_PP;
    else 
    {
        gpio.Mode = GPIO_MODE_INPUT;
        gpio.Pull = GPIO_PULLUP;
    }
    HAL_GPIO_Init(spin->port, &gpio);
    return stm32_core_pin_set_status(spin,isset);
}

vatek_result stm32_core_pin_set_status(Pstm32_pin spin,int32_t isset)
{
    if(isset)HAL_GPIO_WritePin(spin->port,spin->index,GPIO_PIN_SET);
    else HAL_GPIO_WritePin(spin->port,spin->index,GPIO_PIN_RESET);
    return vatek_success;
}

vatek_result stm32_core_pin_get_status(Pstm32_pin spin)
{
    GPIO_PinState status = HAL_GPIO_ReadPin(spin->port,spin->index);
    if(status == GPIO_PIN_SET)return (vatek_result)1;
    else if(status == GPIO_PIN_RESET)return vatek_success;
    return vatek_hwfail;
}
