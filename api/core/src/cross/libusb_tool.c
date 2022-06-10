
#include "./internal/libusb_tools.h"

static libusb_context* cross_libusb_context = NULL;
static int32_t libusb_ref = 0;

vatek_result libusb_tool_init()
{
	vatek_result nres = vatek_success;
	if (!libusb_ref)
	{
		nres = (vatek_result)libusb_init(&cross_libusb_context);
		if (!is_vatek_success(nres))nres = vatek_hwfail;
		else libusb_ref++;
	}
	return nres;
}

void libusb_tool_free()
{
	libusb_ref--;
	if (!libusb_ref)
	{
		libusb_exit(cross_libusb_context);
		cross_libusb_context = NULL;
	}
}

struct libusb_context* libusb_tool_get_context()
{
	return cross_libusb_context;
}
