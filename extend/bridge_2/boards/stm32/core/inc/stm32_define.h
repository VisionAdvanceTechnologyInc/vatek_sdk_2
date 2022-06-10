#ifndef _STM32_DEFINE_
#define _STM32_DEFINE_

#if defined(STM32F407xx) || defined(STM32F401xC)
    #include <stm32f4xx.h>
    #include <usbd_def.h>
#endif

#include <core/vatek_base.h>
#include <hal/hal_bridge_board.h>

/* stm32 common define */

typedef enum _stm32_ip_type
{
    stm32_ip_unknown        = 0,
    stm32_ip_i2c            = 1,
    stm32_ip_uart           = 2,
    stm32_ip_spi            = 3,
    stm32_ip_gpio           = 4,
    stm32_ip_sys_rtc        = 6,
    stm32_ip_bridge_usb     = 7,
    stm32_ip_tsout          = 8,    	/* ts interface out */
	stm32_ip_extram			= 9,
}stm32_ip_type;

typedef struct _stm32_pin
{
    GPIO_TypeDef* port;
    uint16_t index;
    uint16_t map;
}stm32_pin,*Pstm32_pin;

typedef void* hstm32_pin;

typedef void* hstm32_i2c;
typedef vatek_result(*fpi2c_set_speed)(hstm32_i2c hi2c, int32_t speedkhz);
typedef vatek_result(*fpi2c_start)(hstm32_i2c hi2c, uint8_t devaddr,int32_t restart);		/* include write|write bit*/
typedef vatek_result(*fpi2c_write)(hstm32_i2c hi2c,const uint8_t* pbuf, int32_t len);
typedef vatek_result(*fpi2c_read)(hstm32_i2c hi2c, uint8_t* pbuf, int32_t len);
typedef vatek_result(*fpi2c_stop)(hstm32_i2c hi2c);

typedef struct _stm32_i2c
{
	hstm32_i2c hi2c;
    fpi2c_set_speed set_speed;
	fpi2c_start start;
	fpi2c_write write;
	fpi2c_read read;
	fpi2c_stop stop;
}stm32_i2c,*Pstm32_i2c;

typedef void* hstm32_spi; 
typedef vatek_result (*fpspi_config)(hstm32_spi hspi,Phal_spi_param param);
typedef vatek_result (*fpspi_set_cs)(hstm32_spi hspi,int32_t set);
typedef vatek_result (*fpspi_write)(hstm32_spi hspi,uint8_t* pbuf,int32_t len);
typedef vatek_result (*fpspi_read)(hstm32_spi hspi,uint8_t* pbuf,int32_t len);

typedef struct _stm32_spi
{
    hstm32_spi hspi;
    fpspi_config config;
    fpspi_set_cs set_cs;
    fpspi_write write;
    fpspi_read read;
}stm32_spi,*Pstm32_spi;

typedef void* hstm32_uart;
typedef int32_t (*fpuart_get_char)(hstm32_uart huart,int32_t stall);
typedef vatek_result (*fpuart_put_string)(hstm32_uart huart,const char* str);

typedef struct _stm32_uart
{
    hstm32_uart huart;
    fpuart_get_char get_char;
    fpuart_put_string put_string;
}stm32_uart,*Pstm32_uart;

/* stm32 spi define */

#define STM32_SPI_IO_CLK                0
#define STM32_SPI_IO_MISO               1
#define STM32_SPI_IO_MOSI               2
#define STM32_SPI_IO_CS                 3

#define STM32_I2C_IO_SCL                0
#define STM32_I2C_IO_SDA                1

#define STM32_UART_IO_TX                0
#define STM32_UART_IO_RX                1

#define STM32_USB_IO_DM                 0
#define STM32_USB_IO_DP                 1

#define STM32_TSOUT_DAT                 0
#define STM32_TSOUT_CLK                 1
#define STM32_TSOUT_VLD                 2
#define STM32_TSOUT_SYNC                3


/* stm32 tools main param*/

typedef void* hstm32_ip;
typedef void (*fip_init)(void);

typedef struct _stm32_ext_ram
{
	uint32_t size;
	uint8_t* addr;
}stm32_ext_ram,*Pstm32_ext_ram;

typedef struct _stm32_hw_ip
{
    stm32_ip_type ip;
    fip_init init;
    Pstm32_pin pins;
    hstm32_ip handle;
}stm32_hw_ip,*Pstm32_hw_ip;

#define STM32_PIN_NULL                  {NULL,0,0,},           
#define STM32_PIN_START(name)           static const stm32_pin pins_##name[] = {
#define STM32_PIN_GPIO_START            static const stm32_pin pins_gpio_pins[] = {
#define STM32_PIN_END                   STM32_PIN_NULL};

#define STM32_EXTRAM(name,len,addr)		static const stm32_ext_ram extram_##name = {len,(uint8_t*)addr};

#define STM32_PIN_GET(name)             (Pstm32_pin)&pins_##name[0]
#define STM32_EXTRAM_GET(name)			(Pstm32_pin)&extram_##name

#define STM32_PIN(port,idx)             {port,idx,0},

#define STM32_GPIO_PIN(map,port,idx)    {port,idx,map,},
#define STM32_GPIO_PIN_NULL(map)        {NULL,0,map,},

#define STM32_BOARD_START               static const stm32_hw_ip stm32_board[] = {
#define STM32_BOARD_END                 {stm32_ip_unknown,NULL,},};
#define STM32_BOARD_GET                 ((Pstm32_hw_ip)&stm32_board[0])

#define STM32_IP_I2C(i2c,fpinit)		{stm32_ip_i2c,fpinit,STM32_PIN_GET(i2c),&i2c,},
#define STM32_IP_UART(uart,fpinit)		{stm32_ip_uart,fpinit,STM32_PIN_GET(uart),&uart,},
#define STM32_IP_SPI(spi,fpinit)		{stm32_ip_spi,fpinit,STM32_PIN_GET(spi),&spi,},
#define STM32_IP_GPIO(fpinit)			{stm32_ip_gpio,fpinit,STM32_PIN_GET(gpio_pins),NULL,},
#define STM32_IP_BRIDGE_USB(usb,fpinit)	{stm32_ip_bridge_usb,fpinit,STM32_PIN_GET(usb),&usb,},
#define STM32_IP_EXTRAM(name)			{stm32_ip_extram,NULL,STM32_EXTRAM_GET(name),NULL,},

//#define STM32_IP_TSOUT(spi,fpinit)     	{stm32_ip_tsout,fpinit,STM32_PIN_GET(spi),&spi,},
 
#endif

