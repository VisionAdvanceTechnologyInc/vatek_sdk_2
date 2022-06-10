#ifndef _HAL_BRIDGE_USB_
#define _HAL_BRIDGE_USB_

#include <hal/hal_bridge_board.h>
#include <bridge/bridge_usb.h>

typedef void* hhal_bridge;

#ifdef __cplusplus
extern "C" {
#endif

    HAL_API vatek_result hal_bridge_usb_open(hhal_bridge* hbridge);
    HAL_API bridge_device_status hal_bridge_usb_get_status(hhal_bridge hbridge,Phid_bridge_cmd* pcmd);

    HAL_API Phid_bridge_cmd hal_bridge_usb_get_command(hhal_bridge hbridge);
    HAL_API Phid_bridge_result hal_bridge_usb_get_result(hhal_bridge hbridge);
    HAL_API vatek_result hal_bridge_usb_commit(hhal_bridge hbridge);

    HAL_API void hal_bridge_usb_close(hhal_bridge hbridge);

#ifdef __cplusplus
}
#endif

#endif
