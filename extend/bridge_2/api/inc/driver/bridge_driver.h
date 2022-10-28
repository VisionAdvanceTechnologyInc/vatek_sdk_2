#ifndef _BRIDGE_DRIVER_
#define _BRIDGE_DRIVER_

#include <hal/hal_bridge_board.h>
#include <bridge/bridge_device.h>
#include <core/base/output_modulator.h>

typedef void* hbridge_source;

typedef union _bavsource_param
{
    ep9351_param ep9351;
    ep9555e_param ep9555e;
    adv718x_param adv718x;
    h1_param h1;
}bavsource_param;

typedef bavsource_param* Pbavsource_param;

typedef vatek_result (*fpbsource_check_support)(void);
typedef vatek_result (*fpbsource_open)(hbridge_source* hsource);
typedef vatek_result (*fpbsource_config)(hbridge_source hsource,Pbavsource_param param);
typedef vatek_result (*fpbsource_get_status)(hbridge_source hsource,Pbridge_source pbsourcesttaus);
typedef vatek_result (*fpbsource_set_output)(hbridge_source hsource,int32_t isoutput);
typedef void (*fpbsource_close)(hbridge_source hsource);

typedef struct _bdriver_source
{
    const char* name;
    uint32_t id;
    fpbsource_check_support check_support;
    fpbsource_open open;
    fpbsource_config config;
    fpbsource_get_status get_status;
    fpbsource_set_output set_open;
    fpbsource_close close;
}bdriver_source,*Pbdriver_source;


#ifdef __cplusplus
extern "C" {
#endif

    HAL_API vatek_result bridge_source_get_default(bridge_source source,Pbavsource_param pparam);

#ifdef __cplusplus
}
#endif


#endif
