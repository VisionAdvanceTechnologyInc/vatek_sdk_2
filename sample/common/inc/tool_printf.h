#ifndef _TOOL_PRINRF_
#define _TOOL_PRINRF_

#include <stdlib.h>
#include <string.h>
#include <core/base/chip_define.h>
#include <bridge/bridge_device.h>


#define _disp_l(fmt,...)	printf("	"fmt"\r\n",##__VA_ARGS__)
#define _disp_err(fmt,...)	printf("	error - "fmt"\r\n",##__VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

	void printf_chip_info(Pchip_info pchip);
	void printf_bridge_info(Pbdevice_info pbinfo);
	void printf_bridge_avsource(const char* name, Pbridge_source psource);
	void printf_app_end(void);

#ifdef __cplusplus
}
#endif


#endif
