#include <hal/hal_bridge_usb.h>

_HAL_WEAK(vatek_result,hal_bridge_usb_open)(hhal_bridge* hbridge)
{
    return vatek_unsupport;
}

_HAL_WEAK(bridge_device_status,hal_bridge_usb_get_status)(hhal_bridge hbridge,Phid_bridge_cmd* pcmd)
{
    return hid_status_idle;
}

_HAL_WEAK(Phid_bridge_cmd,hal_bridge_usb_get_command)(hhal_bridge hbridge)
{
    return NULL;
}

_HAL_WEAK(Phid_bridge_result,hal_bridge_usb_get_result)(hhal_bridge hbridge)
{
    return NULL;
}

_HAL_WEAK(vatek_result, hal_bridge_usb_commit)(hhal_bridge hbridge)
{
    return vatek_unsupport;
}

_HAL_WEAK(void,hal_bridge_usb_close)(hhal_bridge hbridge)
{

}
