#include <stm32_core.h>
#include "./inc/stm32_internal.h"

#define DEBUG_OUTPUT_TIMEOUT        200
#define STM32_I2C_CHECK_RETRY       2
#define STM32_I2C_CHECK_TIMEOUT     2

typedef struct _sextram_handle
{
	Pstm32_ext_ram ext_ram;
	uint32_t pos;
}sextram_handle,*Psextram_handle;

static UART_HandleTypeDef* stm32_sys_uart = NULL;

static UART_HandleTypeDef* stm32_main_uart = NULL;
static I2C_HandleTypeDef* stm32_main_i2c = NULL;
static SPI_HandleTypeDef* stm32_main_spi = NULL;
static Pstm32_hw_ip stm32_main_gpio = NULL;
static sextram_handle stm32_main_extram = {NULL,0,};

vatek_result stm32_core_init(UART_HandleTypeDef* uart)
{
    vatek_result nres = vatek_badstatus;
    if(stm32_sys_uart != NULL)_S_ERR("stm32_core already initialied");
    else
    {
        Pstm32_hw_ip boards = STM32_BOARD_GET;
        int32_t i = 0;
        stm32_sys_uart = uart;
        while(boards[i].ip != stm32_ip_unknown)
        {
            if(boards[i].ip == stm32_ip_i2c)stm32_main_i2c = (I2C_HandleTypeDef*)boards[i].handle;
            else if(boards[i].ip == stm32_ip_uart)stm32_main_uart = (UART_HandleTypeDef*)boards[i].handle;
            else if(boards[i].ip == stm32_ip_spi)stm32_main_spi = (SPI_HandleTypeDef*)boards[i].handle;
            else if(boards[i].ip == stm32_ip_gpio)stm32_main_gpio = (Pstm32_hw_ip)&boards[i];
			else if(boards[i].ip == stm32_ip_extram)
			{
				Pstm32_ext_ram pextram = (Pstm32_ext_ram)boards[i].pins;
				stm32_main_extram.ext_ram= pextram;
				stm32_main_extram.pos = 0;
			}
            if(boards[i].ip != stm32_ip_bridge_usb && boards[i].init != NULL)boards[i].init();
            i++;
        }
        nres = vatek_success; 
    }
    
    return nres;   
}

uint8_t* stm32_core_extram_malloc(int32_t size)
{
	int32_t len = ((size + 3) >> 2) << 2;
	uint8_t* ptr = NULL;
	
	if(stm32_main_extram.ext_ram)
	{
		
		if(len + stm32_main_extram.pos > stm32_main_extram.ext_ram->size)
			_S_ERR("stm32 extend ram overflow : [%d]",stm32_main_extram.ext_ram->size);
		else
		{
			ptr = &stm32_main_extram.ext_ram->addr[stm32_main_extram.pos];
			stm32_main_extram.pos += len;
		}
	}else ptr = malloc(len);
	
	memset(ptr,0,len);
	return ptr;
}

#if 0
void stm32_core_extram_rest(void)
{
	if(stm32_main_extram.ext_ram)
	{
		stm32_main_extram.pos = 0;
	}else _S_ERR("not support extend ram");
}
#endif

vatek_result stm32_core_main_i2c_check_device(uint8_t devaddr)
{
    vatek_result nres = vatek_badstatus;
    if(stm32_main_i2c != NULL)
    {
        HAL_StatusTypeDef sres = HAL_I2C_IsDeviceReady(stm32_main_i2c,devaddr,STM32_I2C_CHECK_RETRY,STM32_I2C_CHECK_TIMEOUT);
        if(sres == HAL_OK)nres = vatek_success;
        else nres = vatek_unsupport;
    }
    return nres;
}

Pstm32_i2c stm32_core_get_main_i2c(void)
{
    static Pstm32_i2c main_i2c = NULL;
    if(main_i2c == NULL && stm32_main_i2c != NULL)
    {
        vatek_result nres = stm32_core_create_i2c(stm32_main_i2c,&main_i2c);
        if(!is_vatek_success(nres))_S_ERR("create i2c driver fail");
    }
    return main_i2c;
}

Pstm32_spi stm32_core_get_main_spi(void)
{
    static Pstm32_spi main_spi = NULL;
    if(stm32_main_spi != NULL && main_spi == NULL)
    {
        vatek_result nres = stm32_core_create_spi(stm32_main_spi,NULL,&main_spi);
        if(!is_vatek_success(nres))_S_ERR("create spi driver fail : %d",nres);   
    }
    return main_spi;
}

Pstm32_uart stm32_core_get_main_uart(void)
{
    static Pstm32_uart main_uart = NULL;
    if(stm32_main_uart != NULL && main_uart == NULL)
    {
        vatek_result nres = stm32_core_create_uart(stm32_main_uart,&main_uart);
        if(!is_vatek_success(nres))_S_ERR("create uart driver fail : %d",nres);
    }
    return main_uart;
}

Pstm32_hw_ip stm32_internal_get_main_gpio_ip(void)
{
    return stm32_main_gpio;
}

Pstm32_hw_ip stm32_core_get_hwip(stm32_ip_type type)
{
    Pstm32_hw_ip boards = STM32_BOARD_GET;
    int32_t i = 0;
    while(boards[i].ip != stm32_ip_unknown)
    {
        if(boards[i].ip == type)return &boards[i];
        i++;
    }
    return NULL;
}

/* hal_lite_base implemented */

uint32_t hal_lite_get_tick(void)
{
    return HAL_GetTick();
}

void hal_lite_sleep_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

void hal_lite_sleep_us(uint32_t us)
{
    if(us > 1000)
    {
        HAL_Delay(us/1000);
        us %= 1000;
    }
    while(us--);
}

int32_t stm32_core_get_char_no_wait(void)
{
    if(stm32_sys_uart == NULL)return -1;
    if(__HAL_UART_GET_FLAG(stm32_sys_uart,UART_FLAG_RXNE))
    {
        return fgetc(stderr);
    }
    return -1;
}

#ifdef __GNUC__

int __io_putchar(int ch)
{
	uint8_t temp[1]={ch};
	if(stm32_sys_uart == NULL)return -1;
	HAL_UART_Transmit(stm32_sys_uart,temp,1,DEBUG_OUTPUT_TIMEOUT);
	return ch;
}

int __io_getchar (void)
{
	uint8_t temp = 0;
	if(stm32_sys_uart != NULL)
	{
		HAL_StatusTypeDef nres = HAL_UART_Receive(stm32_sys_uart,&temp,1,DEBUG_OUTPUT_TIMEOUT);
		if(nres == HAL_OK)return temp;
	}
	return -1;
}

#else
	/* hook keil c putc and getc implemented */
	int fputc(int ch, FILE *f)
	{
		uint8_t temp[1]={ch};
		if(stm32_sys_uart == NULL)return -1;
		HAL_UART_Transmit(stm32_sys_uart,temp,1,DEBUG_OUTPUT_TIMEOUT);
		return ch;
	}

	int fgetc(FILE *f)
	{
		uint8_t temp = 0;
		if(stm32_sys_uart != NULL)
		{
			HAL_StatusTypeDef nres = HAL_UART_Receive(stm32_sys_uart,&temp,1,DEBUG_OUTPUT_TIMEOUT);
			if(nres == HAL_OK)return temp;
		}
		return -1;
	}
#endif



