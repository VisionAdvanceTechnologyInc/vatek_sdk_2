#ifndef _HAL_BRIDGE_BOARD_
#define _HAL_BRIDGE_BOARD_

#include <core/vatek_base.h>

#define HAL_STORAGE_SECTION_LEN         4096

#define HAL_PIN_NULL                    -1
#define HAL_PIN_01_3V                   0
#define HAL_PIN_02_5V			        0
#define HAL_PIN_03_SDA			        3
#define HAL_PIN_04_5V			        0
#define HAL_PIN_05_SCL			        5
#define HAL_PIN_06_GND			        0
#define HAL_PIN_07_GPIO			        7
#define HAL_PIN_08_UART_TX		        8	
#define HAL_PIN_09_GND			        0
#define HAL_PIN_10_UART_RX		        10
#define HAL_PIN_11_GPIO			        11
#define HAL_PIN_12_GPIO			        12
#define HAL_PIN_13_GPIO			        13
#define HAL_PIN_14_GND			        0
#define HAL_PIN_15_GPIO			        15
#define HAL_PIN_16_GPIO			        16
#define HAL_PIN_17_3V			        0
#define HAL_PIN_18_GPIO			        18
#define HAL_PIN_19_MOSI			        19
#define HAL_PIN_20_GND			        0
#define HAL_PIN_21_MISO			        21
#define HAL_PIN_22_GPIO			        22
#define HAL_PIN_23_SCK			        23
#define HAL_PIN_24_NSS			        24
#define HAL_PIN_25_GND			        0
#define HAL_PIN_26_GPIO			        26
#define HAL_PIN_27_NC			        0
#define HAL_PIN_28_NC			        0
#define HAL_PIN_29_GPIO			        29
#define HAL_PIN_30_GND			        0
#define HAL_PIN_31_GPIO			        31
#define HAL_PIN_32_GPIO			        32
#define HAL_PIN_33_GPIO			        33
#define HAL_PIN_34_GND			        0
#define HAL_PIN_35_GPIO			        35
#define HAL_PIN_36_GPIO			        36
#define HAL_PIN_37_GPIO			        37
#define HAL_PIN_38_GPIO			        38
#define HAL_PIN_39_GND			        0
#define HAL_PIN_40_NRST			        40

#define HAL_IO_CHIP_RESET		        HAL_PIN_22_GPIO
#define HAL_IO_CHIP_RESCUE		        HAL_PIN_31_GPIO
#define HAL_IO_AV_RESET		            HAL_PIN_38_GPIO
#define HAL_IO_HDMI_HPA		            HAL_PIN_07_GPIO
#define HAL_IO_HDMI_INT		            HAL_PIN_11_GPIO
#define HAL_IO_HDMI_NIM_RESET	        HAL_PIN_13_GPIO
#define HAL_IO_HDMI_IO_0		        HAL_PIN_18_GPIO

#define HAL_IO_AUDIO_SWITCH             HAL_PIN_32_GPIO

#define HAL_IO_SPI_NSS                  HAL_PIN_24_NSS

/*(OPTION) - R2 control by MCU*/
#define HAL_IO_R2_DI			        HAL_PIN_29_GPIO
#define HAL_IO_R2_DO			        HAL_PIN_35_GPIO
#define HAL_IO_R2_CS			        HAL_PIN_37_GPIO
#define HAL_IO_R2_CLK			        HALLITE_PIN_33_GPIO

typedef int32_t hhal_pin;

typedef void* hhal_board;

/* hal gpio interface */
typedef enum _hal_gpio_mode
{
    hal_gpio_input = 0,
    hal_gpio_output = 1,
    hal_gpio_output_pp = 2,
}hal_gpio_mode;
    
typedef enum _hal_gpio_trigger
{
    hal_trigger_raising = 0,
    hal_trigger_falling = 1,
}hal_gpio_trigger;

typedef void (*fpgpio_handler)(void* param);


/* hal spi master */

/* 
    Mode	CPOL	CPHA
    0	    0	    0
    1	    0	    1
    2	    1	    0
    3	    1	    1
*/

typedef enum _hal_spi_mode
{
    spi_mode_0 = 0,     
    spi_mode_1 = 1,
    spi_mode_2 = 2,
    spi_mode_3 = 3,
}hal_spi_mode;

typedef enum _hal_spi_buslen
{
    spi_bus_len_8 = 0,
    spi_bus_len_16 = 1,
}hal_spi_buslen;

typedef enum _hal_spi_bit
{
    spi_bit_msb = 0,
    spi_bit_lsb = 1,
}hal_spi_bit;

typedef struct _hal_spi_param
{
    hal_spi_buslen buslen;
    hal_spi_bit bit;
    hal_spi_mode mode;
}hal_spi_param,*Phal_spi_param;

#if __UVISION_VERSION
    #define _HAL_WEAK(r,n)   __weak r n
#else
    #define _HAL_WEAK(r,n)   r __attribute__((weak)) n
#endif

#define _HAL_LOG(ip,fmt,...)    vatek_debug_print(debug_level_all,"[%s]- " fmt "\r\n",#ip,##__VA_ARGS__)
#define _HAL_ERR(ip,fmt,...)    vatek_debug_print(debug_level_error,"[%s]- " fmt "\r\n",#ip,##__VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

    HAL_API void hal_system_sleep(uint32_t ms);
    HAL_API uint32_t hal_system_get_tick(void);

    HAL_API vatek_result hal_board_open(void);

    HAL_API vatek_result hal_gpio_config(hhal_pin pin,hal_gpio_mode mode);
    HAL_API vatek_result hal_gpio_enable_interrupt(hhal_pin pin,hal_gpio_trigger trigger,fpgpio_handler phandler,void* param);
    HAL_API vatek_result hal_gpio_disable_interrupt(hhal_pin pin);
    HAL_API vatek_result hal_gpio_set(hhal_pin pin,int32_t isset);
    HAL_API vatek_result hal_gpio_get(hhal_pin pin);

    HAL_API vatek_result hal_i2c_config(uint32_t speedkhz);
    HAL_API vatek_result hal_i2c_check_device(uint8_t devaddr);
    HAL_API vatek_result hal_i2c_start(uint8_t devaddr,int32_t restart);
    HAL_API vatek_result hal_i2c_write(const uint8_t* pbuf,int32_t len);
    HAL_API vatek_result hal_i2c_read(uint8_t* pbuf,int32_t len);
    HAL_API vatek_result hal_i2c_stop(void);

    HAL_API vatek_result hal_uart_config(uint32_t baudrate);
    HAL_API vatek_result hal_uart_read(int32_t* nkey,int32_t iswait);
    HAL_API vatek_result hal_uart_write(const char* szkeys);

    HAL_API vatek_result hal_spi_config(hal_spi_mode mode,hal_spi_buslen bus,hal_spi_bit bit);
    HAL_API vatek_result hal_spi_write(uint8_t* pbuf,int32_t len);
    HAL_API vatek_result hal_spi_read(uint8_t* pbuf,int32_t len);

    HAL_API vatek_result hal_storage_lock_section(uint8_t** psection);
    HAL_API void hal_storage_unlock_section(void);
    HAL_API vatek_result hal_storage_erase(uint32_t section);
    HAL_API vatek_result hal_storage_write(uint32_t section,uint8_t* psection);
    HAL_API vatek_result hal_storage_read(uint32_t section,uint8_t* psection);

    HAL_API void hal_board_close(void);
    
#ifdef __cplusplus
}
#endif

#endif
