#ifndef _STM32_CORE_
#define _STM32_CORE_

#include <stm32_define.h>
/* board bsp */
#include <config/stm32_board_define.h>

#define STM32_TIMEOUT_MS    100

#define _S_ERR(fmt,...)
#define _S_LOG(fmt,...)

#ifdef __cplusplus
extern "C" {
#endif
    
    /* 
        40 pins extend uart intrrupt handler function weak function internal
        if already processed return == 1 bypass default HAL_UART_IRQHandler called in stm32_fxxx_it.c 
    */ 
    Pstm32_hw_ip stm32_core_get_hwip(stm32_ip_type type);
	
    int32_t stm32_extend_uart_interrupt_handler(void);
    typedef int32_t (*fpextend_uart_rx_hook)(uint8_t nchar);

    /* stm32_core_api system uart api [system uart used with core]*/
    vatek_result stm32_core_init(UART_HandleTypeDef* uart);
    int32_t stm32_core_get_char_no_wait(void);
    
    /* 
        strm32 used hw resource base on raspberry pi 40-pins define below : 
        main i2c    : pin_03[sda]   ,pin_05[scl]
        mian uart   : pin_08[tx]    ,pin_10[rx]
        main spi    : pin_19[mosi]  ,pin_21[miso],pin_23[sck]
        main gpio   : pin_7,9,11,12,13,15,16,18,22,24,26,27,28,29,31,32,33,35,36,37,38,40, 
    */
    vatek_result stm32_core_main_i2c_check_device(uint8_t devaddr);
    Pstm32_i2c stm32_core_get_main_i2c(void); 
    
    /* stm32 uart api */
    Pstm32_uart stm32_core_get_main_uart(void);
    
    /* stm32 spi api */
    Pstm32_spi stm32_core_get_main_spi(void);
	

    Pstm32_pin stm32_core_get_pin(hhal_pin pin);
    vatek_result stm32_core_pin_config(Pstm32_pin spin,hal_gpio_mode mode,int32_t isset);
    vatek_result stm32_core_pin_set_status(Pstm32_pin spin,int32_t isset);
    vatek_result stm32_core_pin_get_status(Pstm32_pin spin);
    
	/* stm32 ext_ram */
	uint8_t* stm32_core_extram_malloc(int32_t size);
	//void stm32_core_extram_rest(void);
	
    /* stm32 extend api - to used hw resource that over the 40-pins define */ 
    vatek_result stm32_core_create_i2c(I2C_HandleTypeDef* i2c,Pstm32_i2c* pi2cdrv);
    vatek_result stm32_core_free_i2c(Pstm32_i2c pi2cdrv);
    
    vatek_result stm32_core_create_spi(SPI_HandleTypeDef* spi,Pstm32_pin pincs,Pstm32_spi* pspidrv);
    vatek_result stm32_core_free_spi(Pstm32_spi pspidrv);
    
    vatek_result stm32_core_create_uart(UART_HandleTypeDef* uart,Pstm32_uart* puartdrv);
    vatek_result stm32_core_free_uart(Pstm32_uart puartdrv);
    
#ifdef __cplusplus
}
#endif

#endif

