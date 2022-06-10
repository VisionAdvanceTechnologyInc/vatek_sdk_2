#ifndef _DERIVER_EP9555E_
#define _DERIVER_EP9555E_

#include <driver/bridge_driver.h>

#ifdef __cplusplus
extern "C" {
#endif

    HAL_API vatek_result ep9555e_check_support(void);
    HAL_API vatek_result ep9555e_open(hbridge_source* hsource);
    HAL_API vatek_result ep9555e_config(hbridge_source hsource,Pbavsource_param param);
    HAL_API vatek_result ep9555e_get_status(hbridge_source hsource,Pbridge_source pbsourcesttaus);
    HAL_API vatek_result ep9555e_set_output(hbridge_source hsource,int32_t isoutput);
    HAL_API void ep9555e_close(hbridge_source hsource);

#ifdef __cplusplus
}
#endif

#endif
