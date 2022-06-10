#include <cross/cross_os_api.h>
#include "internal/libusb_tools.h"

#if __ANDROID__

vatek_result cross_os_ausb_open(const char* usbfspath,int32_t hfd,hcross_ausb* hausb)
{
    vatek_result nres = libusb_tool_init();
    if(is_vatek_success(nres))
    {
        Pandroid_usb newausb = (Pandroid_usb)malloc(sizeof(android_usb));
        nres = vatek_memfail;
        if(newausb)
        {
            memset(newausb,0,sizeof(android_usb));
            newausb->usb_device = libusb_get_device2(libusb_tool_get_context(),usbfspath);
            if(newausb->usb_device)
            {
                nres = (vatek_result)libusb_get_device_descriptor(newausb->usb_device,&newausb->usb_desc);
                if(is_vatek_success(nres))
                    nres = libusb_open2(newausb->usb_device,&newausb->usb_handle,hfd);

                if(is_vatek_success(nres))
                {
                    libusb_set_auto_detach_kernel_driver(newausb->usb_handle, 1);
                    nres = (vatek_result)libusb_set_configuration(newausb->usb_handle,1);
                    if(is_vatek_success(nres))
                        nres = (vatek_result)libusb_claim_interface(newausb->usb_handle, 0);

                    if(!is_vatek_success(nres))libusb_close(newausb->usb_handle);
                    else *hausb = newausb;
                }
            }

            if(!is_vatek_success(nres))free(newausb);
        }
    }
    return nres;
}

void cross_os_ausb_close(hcross_ausb hausb)
{
    Pandroid_usb pausb = (Pandroid_usb)hausb;
    libusb_close(pausb->usb_handle);
    free(pausb);
    libusb_tool_free();
}

uint16_t cross_os_ausb_get_vid(hcross_ausb hausb)
{
    Pandroid_usb pausb = (Pandroid_usb)hausb;
    return pausb->usb_desc.idVendor;
}

uint16_t cross_os_ausb_get_pid(hcross_ausb hausb)
{
    Pandroid_usb pausb = (Pandroid_usb)hausb;
    return pausb->usb_desc.idProduct;
}

#endif