#ifndef _STM32_USB_BRIDGE_
#define _STM32_USB_BRIDGE_

#include <stm32_core.h>
#include <hal/hal_bridge_usb.h>

typedef void* hstm32_usb;
typedef vatek_result(*fpbridge_recv)(uint8_t* pbuf, int32_t len);

typedef vatek_result(*fpbridge_set_recv)(hstm32_usb hbridge,fpbridge_recv fprecv);
typedef vatek_result(*fpbridge_out_ready)(hstm32_usb hbridge);
typedef vatek_result(*fpbridge_send)(hstm32_usb hbridge,uint8_t* pbuf, int32_t len);

typedef struct _stm32_bridge_usb
{
	hstm32_usb hbridge;
	fpbridge_set_recv setrecv;
	fpbridge_out_ready setready;
	fpbridge_send send;
}stm32_bridge_usb,*Pstm32_bridge_usb;

#ifdef __cplusplus
extern "C" {
#endif

    vatek_result stm32_hal_bridge_start(void);
	Pstm32_bridge_usb stm32_usb_bridge_get(void);

#ifdef __cplusplus
}
#endif

#endif
