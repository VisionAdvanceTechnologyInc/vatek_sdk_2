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

#include <cross/cross_bridge.h>
#include <cross/cross_os_api.h>

#include <windows.h>
#include <setupapi.h>
#include <winioctl.h>

#define HID_DEVICE_MAX_NUMS         32
/* not used DDK to compiler so hid define self. (orange in ddk/hid.h) */

typedef USHORT USAGE, *PUSAGE;

typedef struct _HIDD_ATTRIBUTES {
	ULONG  Size;
	USHORT VendorID;
	USHORT ProductID;
	USHORT VersionNumber;
} HIDD_ATTRIBUTES, *PHIDD_ATTRIBUTES;

typedef struct _HIDP_CAPS {
	USAGE  Usage;
	USAGE  UsagePage;
	USHORT InputReportByteLength;
	USHORT OutputReportByteLength;
	USHORT FeatureReportByteLength;
	USHORT Reserved[17];
	USHORT NumberLinkCollectionNodes;
	USHORT NumberInputButtonCaps;
	USHORT NumberInputValueCaps;
	USHORT NumberInputDataIndices;
	USHORT NumberOutputButtonCaps;
	USHORT NumberOutputValueCaps;
	USHORT NumberOutputDataIndices;
	USHORT NumberFeatureButtonCaps;
	USHORT NumberFeatureValueCaps;
	USHORT NumberFeatureDataIndices;
} HIDP_CAPS, *PHIDP_CAPS;

typedef struct _HIDP_PREPARSED_DATA * PHIDP_PREPARSED_DATA;

typedef BOOLEAN(__stdcall *HidD_GetAttributes_)(HANDLE device, PHIDD_ATTRIBUTES attrib);
typedef BOOLEAN(__stdcall *HidD_GetSerialNumberString_)(HANDLE device, PVOID buffer, ULONG buffer_len);
typedef BOOLEAN(__stdcall *HidD_GetManufacturerString_)(HANDLE handle, PVOID buffer, ULONG buffer_len);
typedef BOOLEAN(__stdcall *HidD_GetProductString_)(HANDLE handle, PVOID buffer, ULONG buffer_len);
typedef BOOLEAN(__stdcall *HidD_SetFeature_)(HANDLE handle, PVOID data, ULONG length);
typedef BOOLEAN(__stdcall *HidD_GetFeature_)(HANDLE handle, PVOID data, ULONG length);
typedef BOOLEAN(__stdcall *HidD_GetIndexedString_)(HANDLE handle, ULONG string_index, PVOID buffer, ULONG buffer_len);
typedef BOOLEAN(__stdcall *HidD_GetPreparsedData_)(HANDLE handle, PHIDP_PREPARSED_DATA *preparsed_data);
typedef BOOLEAN(__stdcall *HidD_FreePreparsedData_)(PHIDP_PREPARSED_DATA preparsed_data);
typedef NTSTATUS(__stdcall *HidP_GetCaps_)(PHIDP_PREPARSED_DATA preparsed_data, HIDP_CAPS *caps);
typedef BOOLEAN(__stdcall *HidD_SetNumInputBuffers_)(HANDLE handle, ULONG number_buffers);

static HidD_GetAttributes_ HidD_GetAttributes;
static HidD_GetSerialNumberString_ HidD_GetSerialNumberString;
static HidD_GetManufacturerString_ HidD_GetManufacturerString;
static HidD_GetProductString_ HidD_GetProductString;
static HidD_SetFeature_ HidD_SetFeature;
static HidD_GetFeature_ HidD_GetFeature;
static HidD_GetIndexedString_ HidD_GetIndexedString;
static HidD_GetPreparsedData_ HidD_GetPreparsedData;
static HidD_FreePreparsedData_ HidD_FreePreparsedData;
static HidP_GetCaps_ HidP_GetCaps;
static HidD_SetNumInputBuffers_ HidD_SetNumInputBuffers;

#define HID_PACKET_LEN              65
#define HID_PACKET_BUFFER_LEN       68
#define HID_PACKET_DATA_OFFSET      4
#define HID_PACKET_START_OFFSET     3
#define HID_TIMEOUT                 1000

typedef struct _win_hid_device
{
	struct _win_hid_device* pnext;
	uint16_t hid_vid;
	uint16_t hid_pid;
	HANDLE hid_handle;
	hcross_mutex lock_cmd;
	char hid_path[MAX_PATH];
	OVERLAPPED hid_overlapped;
	uint8_t rawbuf_tx[HID_PACKET_BUFFER_LEN];
	uint8_t rawbuf_rx[HID_PACKET_BUFFER_LEN];
}win_hid_device, *Pwin_hid_device;

#pragma comment(lib, "Setupapi.lib")
#define LOADFUN(fun) fun = (fun##_)GetProcAddress(hid_api_lib,#fun); if(fun == NULL)return vatek_unknown;

const GUID hid_class_guid = { 0x4d1e55b2, 0xf16f, 0x11cf,{ 0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } };

static HMODULE hid_api_lib = NULL;

extern vatek_result win_hid_api_write(Pwin_hid_device pdevice, uint8_t* ppacket);
extern vatek_result win_hid_api_read(Pwin_hid_device pdevice, uint8_t* ppacket);

vatek_result bridge_device_init(void)
{
	vatek_result nres = vatek_success;
	if (hid_api_lib == NULL)
	{
		hid_api_lib = LoadLibraryA("hid.dll");
		if (hid_api_lib != NULL)
		{
			LOADFUN(HidD_GetAttributes);
			LOADFUN(HidD_GetSerialNumberString);
			LOADFUN(HidD_GetManufacturerString);
			LOADFUN(HidD_GetProductString);
			LOADFUN(HidD_SetFeature);
			LOADFUN(HidD_GetFeature);
			LOADFUN(HidD_GetIndexedString);
			LOADFUN(HidD_GetPreparsedData);
			LOADFUN(HidD_FreePreparsedData);
			LOADFUN(HidP_GetCaps);
			LOADFUN(HidD_SetNumInputBuffers);
		}
		else nres = vatek_memfail;
	}
	return nres;
}

vatek_result bridge_device_free(void)
{
	if (hid_api_lib)
	{
		FreeLibrary(hid_api_lib);
		hid_api_lib = NULL;
	}
	return vatek_success;
}

vatek_result bridge_device_list_enum_default(hbridge_list* hblist)
{
	vatek_result nres = bridge_device_list_enum_usb(USB_BRIDGE_VID, USB_BRIDGE_PID, hblist);
	if(nres == vatek_success)
		nres = bridge_device_list_enum_usb(USB_BRIDGE_VID_OLD, USB_BRIDGE_PID, hblist);
	return nres;
}

vatek_result bridge_device_list_enum_usb(uint16_t vid, uint16_t pid, hbridge_list* hblist)
{
	SP_DEVINFO_DATA devinfo_data;
	SP_DEVICE_INTERFACE_DATA device_interface_data;
	HDEVINFO hinfo = INVALID_HANDLE_VALUE;
	vatek_result nres = bridge_device_init();
	if (!is_vatek_success(nres))return nres;
	memset(&devinfo_data, 0, sizeof(SP_DEVINFO_DATA));
	devinfo_data.cbSize = sizeof(SP_DEVINFO_DATA);
	device_interface_data.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	hinfo = SetupDiGetClassDevsA(&hid_class_guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (hinfo != INVALID_HANDLE_VALUE)
	{
		int32_t index = 0;
		int32_t datalen = -1;
		int32_t nums = 0;
		Pwin_hid_device proot = NULL;
		Pwin_hid_device pnext = NULL;
		SP_DEVICE_INTERFACE_DETAIL_DATA_A *intf_detail_info = NULL;

		nres = vatek_success;

		while (SetupDiEnumDeviceInterfaces(hinfo, NULL, &hid_class_guid, index, &device_interface_data))
		{
			BOOL bres = SetupDiGetDeviceInterfaceDetailA(hinfo, &device_interface_data, NULL, 0, &datalen, NULL);
			index++;
			if (datalen > 0)
			{
				intf_detail_info = (SP_DEVICE_INTERFACE_DETAIL_DATA_A*)malloc(datalen);
				intf_detail_info->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);
				bres = SetupDiGetDeviceInterfaceDetailA(hinfo, &device_interface_data, intf_detail_info, datalen, NULL, NULL);
				if (bres)
				{
					HANDLE hhid = CreateFileA(intf_detail_info->DevicePath,
						GENERIC_WRITE | GENERIC_READ,
						FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,
						FILE_FLAG_OVERLAPPED,/*FILE_ATTRIBUTE_NORMAL,*/
						0);

					if (hhid != INVALID_HANDLE_VALUE)
					{
						HIDD_ATTRIBUTES hidattr;
						hidattr.Size = sizeof(HIDD_ATTRIBUTES);
						bres = HidD_GetAttributes(hhid, &hidattr);
						if (bres && (vid == hidattr.VendorID) && (pid == hidattr.ProductID))
						{
							Pwin_hid_device pnewhid = (Pwin_hid_device)malloc(sizeof(win_hid_device));
							nres = vatek_memfail;
							if (pnewhid)
							{
								memset(pnewhid, 0, sizeof(win_hid_device));
								strncpy_s(&pnewhid->hid_path[0], MAX_PATH, &intf_detail_info->DevicePath[0], MAX_PATH);
								pnewhid->hid_vid = hidattr.VendorID;
								pnewhid->hid_pid = hidattr.ProductID;

								if (pnext == NULL)proot = pnewhid;
								else pnext->pnext = pnewhid;
								pnext = pnewhid;
								nums++;
								nres = vatek_success;
							}
						}
						CloseHandle(hhid);
					}
				}
				free(intf_detail_info);
			}
		}
		
		if (is_vatek_success(nres))
		{
			nres = (vatek_result)nums;
			*hblist = proot;
		}
	}
	return nres;
}

vatek_result bridge_device_list_free(hbridge_list hbridges)
{
	Pwin_hid_device proot = (Pwin_hid_device)hbridges;

	while (proot)
	{
		if (proot->hid_handle != NULL)return vatek_badstatus;
		proot = proot->pnext;
	}

	proot = (Pwin_hid_device)hbridges;
	while (proot)
	{
		Pwin_hid_device pnext = proot->pnext;
		cross_os_free_mutex(proot->lock_cmd);
		free(proot);
		proot = pnext;
	}
	return vatek_success;
}

vatek_result bridge_device_list_get(hbridge_list hblist, int32_t idx, hbridge_device* hbridge)
{
	Pwin_hid_device proot = (Pwin_hid_device)hblist;
	int32_t nums = 0;
	while (proot)
	{
		if (nums == idx)
		{
			*hbridge = proot;
			return vatek_success;
		}
		proot = proot->pnext;
	}
	return vatek_badparam;
}

const char* bridge_device_list_get_name(hbridge_list hblist, int32_t idx)
{
	hbridge_device hbridge = NULL;
	vatek_result nres = bridge_device_list_get(hblist, idx, &hbridge);
	if (is_vatek_success(nres))
		return &((Pwin_hid_device)hbridge)->hid_path[0];
	return NULL;
}

vatek_result bridge_device_open(hbridge_device hbridge)
{
	vatek_result nres = vatek_badstatus;
	Pwin_hid_device phid = (Pwin_hid_device)hbridge;
	if (phid->hid_handle == NULL)
	{
		hcross_mutex hlock;
		nres = cross_os_create_mutex(&hlock);
		if (is_vatek_success(nres))
		{
			phid->hid_handle = CreateFileA(&phid->hid_path[0],
				GENERIC_WRITE | GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_OVERLAPPED,/*FILE_ATTRIBUTE_NORMAL,*/
				0);

			if (phid->hid_handle != INVALID_HANDLE_VALUE)
			{
				phid->hid_overlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
				if (phid->hid_overlapped.hEvent != INVALID_HANDLE_VALUE)
				{
					Phid_bridge_cmd pcmd = (Phid_bridge_cmd)&phid->rawbuf_tx[HID_PACKET_DATA_OFFSET];
					memset(&phid->rawbuf_tx[0], 0, HID_PACKET_BUFFER_LEN);
					memset(&phid->rawbuf_rx[0], 0, HID_PACKET_BUFFER_LEN);
					memcpy(&pcmd->tag[0], &hid_bridge_tag[0], 4);
					phid->lock_cmd = hlock;
					return vatek_success;
				}
				CloseHandle(phid->hid_handle);
				phid->hid_handle = NULL;
			}

			if (!is_vatek_success(nres))cross_os_free_mutex(hlock);
		}
	}
	return nres;
}

void bridge_device_lock_command(hbridge_device hbridge)
{
	Pwin_hid_device phid = (Pwin_hid_device)hbridge;
	cross_os_lock_mutex(phid->lock_cmd);
}

void bridge_device_unlock_command(hbridge_device hbridge)
{
	Pwin_hid_device phid = (Pwin_hid_device)hbridge;
	cross_os_release_mutex(phid->lock_cmd);
}

const char* bridge_device_get_name(hbridge_device hbridge)
{
	Pwin_hid_device phid = (Pwin_hid_device)hbridge;
	return &phid->hid_path[0];
}

vatek_result bridge_device_close(hbridge_device hbridge)
{
	Pwin_hid_device phid = (Pwin_hid_device)hbridge;
	if (phid->hid_handle != NULL)
	{
		cross_os_free_mutex(phid->lock_cmd);
		CloseHandle(phid->hid_overlapped.hEvent);
		CloseHandle(phid->hid_handle);
	}
	phid->hid_overlapped.hEvent = NULL;
	phid->hid_handle = NULL;
	phid->lock_cmd = NULL;
	return vatek_success;
}


Phid_bridge_cmd bridge_device_get_command(hbridge_device hbridge)
{
	Pwin_hid_device phid = (Pwin_hid_device)hbridge;
	return (Phid_bridge_cmd)&phid->rawbuf_tx[HID_PACKET_DATA_OFFSET];
}

Phid_bridge_result bridge_device_get_result(hbridge_device hbridge)
{
	Pwin_hid_device phid = (Pwin_hid_device)hbridge;
	return (Phid_bridge_result)&phid->rawbuf_rx[HID_PACKET_DATA_OFFSET];
}

vatek_result bridge_device_send_bridge_command(hbridge_device hbridge)
{
	Pwin_hid_device phid = (Pwin_hid_device)hbridge;
	vatek_result nres = win_hid_api_write(phid, &phid->rawbuf_tx[HID_PACKET_START_OFFSET]);
	if (is_vatek_success(nres))
	{
		nres = win_hid_api_read(phid, &phid->rawbuf_rx[HID_PACKET_START_OFFSET]);
		if (is_vatek_success(nres))
		{
			Phid_bridge_result presult = bridge_device_get_result(hbridge);
			presult->result = vatek_buffer_2_uint32((uint8_t*)&presult->result);
			presult->cmd = vatek_buffer_2_uint32((uint8_t*)&presult->cmd);
			if (strncmp(&presult->tag[0], &hid_bridge_tag[0], 4) != 0)nres = vatek_badparam;
			else nres = (vatek_result)presult->result;
		}
	}
	return nres;
}

vatek_result win_hid_api_write(Pwin_hid_device pdevice, uint8_t* ppacket)
{
	uint32_t nwrite = 0;
	vatek_result nres = vatek_badstatus;
	if (pdevice->hid_handle)
	{
		BOOL bres = WriteFile(pdevice->hid_handle, ppacket, HID_PACKET_LEN, &nwrite, &pdevice->hid_overlapped);
		nres = vatek_hwfail;
		if (bres || (GetLastError() == ERROR_IO_PENDING))
		{
			int32_t nerr = (vatek_result)WaitForSingleObject(pdevice->hid_overlapped.hEvent, INFINITE);
			if (nerr == WAIT_OBJECT_0)
			{
				bres = GetOverlappedResult(pdevice->hid_handle, &pdevice->hid_overlapped, &nwrite, TRUE);
				if (bres && nwrite == HID_PACKET_LEN)nres = vatek_success;
			}
		}
		if (!is_vatek_success(nres))CancelIo(pdevice->hid_handle);
	}
	return nres;
}

vatek_result win_hid_api_read(Pwin_hid_device pdevice, uint8_t* ppacket)
{
	uint32_t nread = 0;
	vatek_result nres = vatek_badstatus;
	if (pdevice->hid_handle)
	{
		BOOL bres = ReadFile(pdevice->hid_handle, ppacket, HID_PACKET_LEN, &nread, &pdevice->hid_overlapped);
		nres = vatek_hwfail;
		if (bres || (GetLastError() == ERROR_IO_PENDING))
		{
			int32_t nerr = (vatek_result)WaitForSingleObject(pdevice->hid_overlapped.hEvent, INFINITE);
			if (nerr == WAIT_OBJECT_0)
			{
				bres = GetOverlappedResult(pdevice->hid_handle, &pdevice->hid_overlapped, &nread, TRUE);
				if (bres && nread == HID_PACKET_LEN)nres = vatek_success;
			}
		}
		if(!is_vatek_success(nres))CancelIo(pdevice->hid_handle);
	}
	return nres;
}
