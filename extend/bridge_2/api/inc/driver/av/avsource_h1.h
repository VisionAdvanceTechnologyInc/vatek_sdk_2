#ifndef _DRIVER_H1_
#define _DRIVER_H1_

#include <driver/bridge_driver.h>
#include <stm32_define.h>
#ifdef __cplusplus
extern "C" {
#endif

    HAL_API vatek_result h1_check_support(void);
    HAL_API vatek_result h1_open(hbridge_source* hsource);
    HAL_API vatek_result h1_config(hbridge_source hsource,Pbavsource_param param);
    HAL_API vatek_result h1_get_status(hbridge_source hsource,Pbridge_source pbsourcesttaus);
    HAL_API vatek_result h1_set_output(hbridge_source hsource, int32_t isoutput, Pbridge_source psource);
    HAL_API void h1_close(hbridge_source hsource);

    //create by me
    vatek_result h1_enable();
    uint32_t h1_version();
#ifdef __cplusplus
}
#endif

#endif
