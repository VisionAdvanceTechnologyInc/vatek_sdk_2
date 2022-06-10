#ifndef _CMD_BRIDGE_
#define _CMD_BRIDGE_

#include <vatek_bridge_2.h>
#include <hal/hal_bridge_usb.h>

#ifdef __cplusplus
extern "C" {
#endif

    vatek_result cmd_bridge_source(hvatek_bridge hbridge, Phid_bridge_cmd pcmd, Phid_bridge_result pres);
    vatek_result cmd_bridge_device(hvatek_bridge hbridge, Phid_bridge_cmd pcmd, Phid_bridge_result pres);
    vatek_result cmd_bridge_demod(hvatek_bridge hbridge, Phid_bridge_cmd pcmd, Phid_bridge_result pres);
    vatek_result cmd_bridge_rfmixer(hvatek_bridge hbridge, Phid_bridge_cmd pcmd, Phid_bridge_result pres);

#ifdef __cplusplus
}
#endif

#endif

