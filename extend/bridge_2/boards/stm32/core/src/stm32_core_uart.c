#include <stm32_core.h>

extern int32_t stm32_get_char(hstm32_uart huart,int32_t stall);
extern vatek_result stm32_put_string(hstm32_uart huart,const char* str);

vatek_result stm32_core_create_uart(UART_HandleTypeDef* uart,Pstm32_uart* puartdrv)
{
    Pstm32_uart newuart = (Pstm32_uart)malloc(sizeof(stm32_uart));
    vatek_result nres = vatek_memfail;
    if(newuart != NULL)
    {
        memset(newuart,0,sizeof(stm32_uart));
        newuart->huart = uart;
        newuart->get_char = stm32_get_char;
        newuart->put_string = stm32_put_string;
        *puartdrv = newuart;
        nres = vatek_success;
    }
    return nres;
}

vatek_result stm32_core_free_uart(Pstm32_uart puartdrv)
{
    free(puartdrv);
    return vatek_success;
}

int32_t stm32_get_char(hstm32_uart huart,int32_t stall)
{
    UART_HandleTypeDef* suart = (UART_HandleTypeDef*)huart;
    if(stall || __HAL_UART_GET_FLAG(suart,UART_FLAG_RXNE))
    {
        uint8_t temp;
        HAL_StatusTypeDef nres = HAL_UART_Receive(suart,&temp,1,100);
        if(nres == HAL_OK)return temp;
    }
    return -1;
}

vatek_result stm32_put_string(hstm32_uart huart,const char* str)
{
    UART_HandleTypeDef* suart = (UART_HandleTypeDef*)huart;
    HAL_StatusTypeDef hres = HAL_UART_Transmit(suart,(uint8_t*)str,strlen(str),100);
    if(hres == HAL_OK)return vatek_success;
    else return vatek_hwfail;
}
