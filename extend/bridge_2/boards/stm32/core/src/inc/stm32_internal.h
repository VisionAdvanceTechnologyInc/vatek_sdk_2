#ifndef _STM32_INTERNAL_
#define _STM32_INTERNAL_

#include <stm32_define.h>

#ifdef __cplusplus
extern "C" {
#endif

    vatek_result stm32_internal_enable_storage(Pstm32_hw_ip phwip);
    Pstm32_hw_ip stm32_internal_get_main_gpio_ip(void);
    Pstm32_hw_ip stm32_internal_get_ip(stm32_ip_type type);

#ifdef __cplusplus
}
#endif

#endif

