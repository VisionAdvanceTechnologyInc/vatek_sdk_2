#ifndef _DRIVER_EP9351_
#define _DRIVER_EP9351_

#include <driver/bridge_driver.h>

#ifdef __cplusplus
extern "C" {
#endif

    HAL_API vatek_result ep9351_check_support(void);
    HAL_API vatek_result ep9351_open(hbridge_source* hsource);
    HAL_API vatek_result ep9351_config(hbridge_source hsource,Pbavsource_param param);
    HAL_API vatek_result ep9351_get_status(hbridge_source hsource,Pbridge_source pbsourcesttaus);
    HAL_API vatek_result ep9351_set_output(hbridge_source hsource,int32_t isoutput);
    HAL_API void ep9351_close(hbridge_source hsource);

#ifdef __cplusplus
}
#endif

#endif
