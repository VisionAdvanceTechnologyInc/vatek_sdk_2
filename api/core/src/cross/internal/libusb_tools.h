
#ifndef _LUBUSB_TOOL_
#define _LUBUSB_TOOL_

#include <core/vatek_base.h>
#include <libusb-1.0/libusb.h>

#if CC_ANDROID

	typedef struct _android_usb
	{
		libusb_device* usb_device;
		libusb_device_handle* usb_handle;
		struct libusb_device_descriptor usb_desc;
	}android_usb,*Pandroid_usb;
	
#endif

#ifdef __cplusplus
extern "C" {
#endif

	vatek_result libusb_tool_init();
	void libusb_tool_free();
	struct libusb_context* libusb_tool_get_context();

#ifdef __cplusplus
}
#endif

#endif
