#ifndef _DRIVER_ADV718X_
#define _DRIVER_ADV718X_

#include <driver/bridge_driver.h>

#ifdef __cplusplus
extern "C" {
#endif

    HAL_API vatek_result adv718x_check_support(void);
    HAL_API vatek_result adv718x_open(hbridge_source* hsource);
    HAL_API vatek_result adv718x_config(hbridge_source hsource,Pbavsource_param param);
    HAL_API vatek_result adv718x_get_status(hbridge_source hsource,Pbridge_source pbsourcesttaus);
    HAL_API vatek_result adv718x_set_output(hbridge_source hsource,int32_t isoutput);
    HAL_API void adv718x_close(hbridge_source hsource);

#ifdef __cplusplus
}
#endif

#endif
