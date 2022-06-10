#ifndef _BRIDGE_INTERNAL_
#define _BRIDGE_INTERNAL_

#include <vatek_bridge_2.h>
#include <hal/hal_bridge_board.h>
#include <hal/hal_bridge_usb.h>
#include <stm32_core.h>

#define BRIDGE_2_VERSION    0x02201210
#define BRIDGE_SECTION_LEN  4096

typedef struct _bsource_list
{
    struct _bsource_list* next;
    bdevice_source device;
}bsource_list,*Pbsource_list;

typedef struct _bstorage_handle
{
	bflash_command cmd;
    uint8_t* section;
	int32_t pos;
}bstorage_handle,*Pbstorage_handle;

typedef struct _bridge_handle
{
    bdevice_info info;
    hhal_bridge hbridgeusb;
    int32_t source_count;
    Pbsource_list source_root;
    Pbsource_list source_last;
    Pbdevice_source source_active;
    bstorage_handle storage;
}bridge_handle,*Pbridge_handle;

#ifdef __cplusplus
extern "C" {
#endif

    uint32_t hal_bridge_get_uint32(uint32_t value);
    uint32_t hal_bridge_get_result(vatek_result nres);

    vatek_result map_bridge_to_buffer(uint8_t* bbuf, uint8_t* bdata, Pstruct_bridge_param params);
    vatek_result map_buffer_to_bridge(uint8_t* bbuf, uint8_t* bdata, Pstruct_bridge_param params);

    vatek_result bridge_source_get(Pbridge_handle phandle);
    void bridge_source_reset(Pbridge_handle phandle);

    vatek_result bstorage_start(Pbridge_handle phandle,Pbflash_command pfcmd);
    vatek_result bstorage_put(Pbridge_handle phandle,uint8_t* pbuf,int32_t len);
    vatek_result bstorage_get(Pbridge_handle phandle,uint8_t** pbuf,int32_t len);
    vatek_result bstorage_stop(Pbridge_handle phandle);

    void bstorage_reset(Pbridge_handle phandle);

#ifdef __cplusplus
}
#endif

#endif
