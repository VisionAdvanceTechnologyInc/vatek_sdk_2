//----------------------------------------------------------------------------
//
// Vision Advance Technology - Software Development Kit
// Copyright (c) 2014-2022, Vision Advance Technology Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//

#ifndef _CROSS_USB_DEVICE_WINUSB_
#define _CROSS_USB_DEVICE_WINUSB_

#include <core/vatek_base.h>
#include <core/base/device_usb.h>

#include <initguid.h>

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <winusb.h>
#include <usb.h>

#include <SetupAPI.h>
#include <stdlib.h>

typedef void* husb_device;
typedef void* husb_device_list;

typedef struct _DEVICE_DATA {

	BOOL                    HandlesOpen;
	WINUSB_INTERFACE_HANDLE WinusbHandle;
	HANDLE                  DeviceHandle;
	TCHAR                   DevicePath[MAX_PATH];

} DEVICE_DATA, * PDEVICE_DATA;

typedef struct _PIPE_ID
{
	UCHAR  PipeInId;
	UCHAR  PipeOutId;
} PIPE_ID;

BOOL QueryDeviceEndpoints(WINUSB_INTERFACE_HANDLE hDeviceHandle, PIPE_ID* pipeid);


#define _usb_table_start	static const  usbdevice_id usb_device_ids[] = {
#define _usb_broadcast(pid)	{ usb_type_broadcast	,USBDEV_VID	,pid,},
#define _usb_rescure(pid)	{ usb_type_rescure		,USBDEV_VID	,pid,},
#define _usb_table_end		{ usb_type_unknown,0,0, },};

#define _usb_table 			((const Pusbdevice_id)&usb_device_ids[0])

_usb_table_start
_usb_broadcast(0x2011)
_usb_broadcast(0x2021)
_usb_broadcast(0x1011)
_usb_broadcast(0x1031)
_usb_broadcast(0x2031)
_usb_rescure(0x1010)
_usb_rescure(0x1030)
_usb_rescure(0x2010)
_usb_rescure(0x2030)
_usb_table_end

#define USBDEVICE_ID_NUMS		(sizeof(usb_device_ids)/sizeof(usbdevice_id))

HRESULT
RetrieveDevicePath(
	_Out_bytecap_(BufLen) LPTSTR DevicePath,
	_In_                  ULONG  BufLen,
	_Out_opt_             PBOOL  FailureDeviceNotFound
);

HRESULT
OpenDevice(
	_Out_     PDEVICE_DATA DeviceData,
	_Out_opt_ PBOOL        FailureDeviceNotFound
);

VOID
CloseDevice(
	_Inout_ PDEVICE_DATA DeviceData
);

BOOL QueryDeviceEndpoints(WINUSB_INTERFACE_HANDLE hDeviceHandle, PIPE_ID* pipeid);


#ifdef __cplusplus
extern "C" {
#endif

	/* usb_device list tools */
	HAL_API vatek_result usb_api_ll_enum(usbdevice_type type, husb_device_list* hlist);
	HAL_API vatek_result usb_api_ll_enum_by_id(uint16_t vid, uint16_t pid, husb_device_list* hlist);
	HAL_API vatek_result usb_api_ll_list_get_device(husb_device_list hlist, int32_t idx, husb_device* husb);
	HAL_API const char* usb_api_ll_list_get_name(husb_device_list hlist, int32_t idx);
	HAL_API vatek_result usb_api_ll_free_list(husb_device_list hlist);
	HAL_API vatek_result usb_api_ll_open(husb_device husb);
	HAL_API vatek_result usb_api_ll_close(husb_device husb);
	HAL_API const char* usb_api_ll_get_name(husb_device husb);

	/* usb_device bulk transfer */
	HAL_API void usb_api_ll_lock(husb_device husb);
	HAL_API void usb_api_ll_unlock(husb_device husb);
	HAL_API vatek_result usb_api_ll_set_dma(husb_device husb, int32_t isdma);
	HAL_API vatek_result usb_api_ll_write(husb_device husb, uint8_t* pbuf, int32_t len);
	HAL_API vatek_result usb_api_ll_read(husb_device husb, uint8_t* pbuf, int32_t len);

	/* usb_device control transfer */
	HAL_API vatek_result usb_api_ll_command(husb_device husb, uint8_t cmd, uint32_t param0, uint8_t* rxbuf);
	HAL_API vatek_result usb_api_ll_command_buffer(husb_device husb, uint8_t cmd, uint8_t* pbuf, uint8_t* rxbuf);

	HAL_API vatek_result usb_api_ll_bulk_get_size(husb_device husb);
	HAL_API vatek_result usb_api_ll_bulk_send_command(husb_device husb, Pusbbulk_command pcmd);
	HAL_API vatek_result usb_api_ll_bulk_get_result(husb_device husb, Pusbbulk_result presult);
	HAL_API vatek_result usb_api_ll_bulk_write(husb_device husb, uint8_t* pbuf, int32_t len);
	HAL_API vatek_result usb_api_ll_bulk_read(husb_device husb, uint8_t* pbuf, int32_t len);

#ifdef __cplusplus
}
#endif

#endif
