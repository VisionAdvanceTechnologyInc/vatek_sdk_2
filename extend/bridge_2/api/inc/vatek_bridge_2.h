#ifndef _VATEK_BRIDGE_2
#define _VATEK_BRIDGE_2

#include <core/vatek_base.h>
#include <driver/bridge_driver.h>

typedef void* hvatek_bridge;

typedef struct _bdevice_source
{
    hbridge_source hsource;
    Pbdriver_source driver;
}bdevice_source,*Pbdevice_source;

typedef void* bpointer;

#ifdef __cplusplus
extern "C" {
#endif

    HAL_API vatek_result vatek_bridge_open(hvatek_bridge* hbridge);

    HAL_API Pbdevice_info vatek_bridge_get_info(hvatek_bridge hbridge);

    HAL_API void vatek_source_get_first(hvatek_bridge hbridge,bpointer* pos);
    HAL_API Pbdevice_source vatek_source_get_next(bpointer* pos); 

    HAL_API vatek_result vatek_bridge_start(hvatek_bridge hbridge);
    HAL_API vatek_result vatek_bridge_polling(hvatek_bridge hbridge);
    HAL_API vatek_result vatek_bridge_stop(hvatek_bridge hbridge);
	
	HAL_API vatek_result vatek_chip_open(hvatek_bridge hbridge,hvatek_chip* hchip);
    HAL_API void vatek_chip_close(hvatek_chip hchip);
	
    HAL_API void vatek_bridge_close(hvatek_bridge hbridge);

#ifdef __cplusplus
}
#endif

#endif
