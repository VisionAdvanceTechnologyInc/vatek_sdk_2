#ifndef _CMD_MOD_
#define _CMD_MOD_

#include <vatek_bridge_2.h>
#include <hal/hal_bridge_usb.h>

#ifdef __cplusplus
extern "C" {
#endif

    vatek_result cmd_mod(hvatek_bridge hbridge, Phid_bridge_cmd pcmd, Phid_bridge_result pres);

#ifdef __cplusplus
}
#endif

#endif
