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

#ifndef _DEVICE_USB_BASE_
#define _DEVICE_USB_BASE_

#include <core/vatek_base.h>

#define USBDEV_VID								0x2C42
#define USBDEV_VID_OLD							0x1934

#define USB_DEVICE_MAX_PACKET_LEN 				512

typedef enum _usbdevice_type
{
	usb_type_unknown = -1,
	usb_type_broadcast = 0,
	usb_type_rescure = 1,
	usb_type_all = 2,
}usbdevice_type;

typedef struct _usbdevice_id
{
	usbdevice_type type;
	uint16_t vid;
	uint16_t pid;
}usbdevice_id, *Pusbdevice_id;

#define VATCMD_CLASSV2_MASK                     0x40

#define VATCMD_CLASSV2_SET_MODE					0x41
	#define CLASSV2_MODE_NORMAL					0x0000
	#define CLASSV2_MODE_HWDMA					0x0001

/* put data to usb_cache then used write_ram, write_rom and run code	*/
/* param_0 : address position											*/
#define VATCMD_CLASSV2_PUTDATA					0x42

/* put 4K code then write to mcu_ram									*/
/* param_0 : section number												*/

#define VATCMD_CLASSV2_WRITE_RAM				0x43
/* put 4K code then write to mcu_rom									*/
/* param_0 : section number												*/
#define VATCMD_CLASSV2_WRITE_ROM				0x44

/* put 8K _mcu vector code then run start run code						*/
#define VATCMD_CLASSV2_RUNCODE					0x45		


#define VATCMD_CLASSV2_SETADDR                  0x4A
#define VATCMD_CLASSV2_RDREG                    0x4C
#define VATCMD_CLASSV2_WRREG                    0x4D
#define VATCMD_CLASSV2_RDMEM                    0x4E
#define VATCMD_CLASSV2_WRMEM                    0x4F

#define is_vacmd_classv2(r)                     ((r & VATCMD_CLASSV2_MASK) == VATCMD_CLASSV2_MASK)

#define USBDEV_TIMEOUT							1000
#define USBDEV_BULK_WRITE_EP					0x01
#define USBDEV_BULK_READ_EP						0x81

#define USBBULK_TAG_LEN							8
#define USBBULK_CMD_MASK						0x00FF0000
#define USBBULK_CMD_BASE						0x00FA0000

typedef enum _usbbulk_mode
{
	usbbulk_null = 0,
	usbbulk_flash = USBBULK_CMD_BASE,
	usbbulk_aux,
}usbbulk_mode;

typedef enum _usbflash_mode
{
	usbflash_null,
	usbflash_write,
	usbflash_read,
	usbflash_erase,
}usbflash_mode;

typedef struct _usbparam_flash
{
	usbflash_mode mode;
	uint32_t section;
}usbparam_flash;

typedef usbparam_flash* Pusbparam_flash;

typedef enum _usbaux_mode
{
	usbaux_null,
	usbaux_async,
	usbaux_sync,
}usbaux_mode;

typedef struct _usbparam_aux
{
	usbaux_mode mode;
	uint32_t length;
	uint32_t bitrate;
}usbparam_aux;

typedef usbparam_aux* Pusbparam_aux;

typedef struct _usbbulk_command
{
	usbbulk_mode mode;
	union
	{
		usbparam_flash flash;
		usbparam_aux aux;
	}_h;
}usbbulk_command;

typedef usbbulk_command* Pusbbulk_command;

typedef struct _usbbulk_result
{
	usbbulk_mode mode;
	vatek_result result;
	uint32_t len;
	uint32_t position;
}usbbulk_result;

typedef usbbulk_result* Pusbbulk_result;


#ifdef __cplusplus
extern "C" {
#endif

	HAL_API int32_t usbbulk_buffer_check(uint8_t* pbuf);

	HAL_API vatek_result usbbulk_command_set(Pusbbulk_command pcmd, uint8_t* pbuf);
	HAL_API vatek_result usbbulk_command_get(Pusbbulk_command pcmd, uint8_t* pbuf);

	HAL_API vatek_result usbbulk_result_set(Pusbbulk_result presult, uint8_t* pbuf);
	HAL_API vatek_result usbbulk_result_get(Pusbbulk_result presult, uint8_t* pbuf);

#ifdef __cplusplus
}
#endif


#endif
