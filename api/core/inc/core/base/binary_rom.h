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

#ifndef _BINARY_ROM_
#define _BINARY_ROM_

#include <core/vatek_base.h>
#include <core/base/chip_define.h>

#define BINARY_SECTION_SIZE				4096
#define BINARY_SECTION_LEN(len)			(((len + (BINARY_SECTION_SIZE - 1)) / BINARY_SECTION_SIZE) * BINARY_SECTION_SIZE)

/**
 * @ingroup storage_props
 * firmware file type 
 */
typedef enum _binary_type{
	binary_unknown = -1,
	binary_loader = 0,
	binary_app = 1,
	binary_image = 2,
}binary_type;

/**
 * @ingroup storage_props
 * firmware file support chip platform
 */
typedef enum _binary_chip_arch{
	chip_arch_unknown = -1,
	chip_arch_ax = 0x000FA0A,
	chip_arch_bx = 0x000FA0B,
	chip_arch_a3 = 0x000FA0C,
}binary_chip_arch;

/**
 * @ingroup storage_props
 * loader file header
 */

#define VATEK_LOADER_EXTENSION			".v2loader"
#define LOADER_MAGIC_V2					0xAF551229
#define LOADER_SIZE						0x10000
#define VECTOR_SIZE						0x2000

typedef struct _loader_header{
	uint8_t bin_size[4];		/*!< hardware get loader len in word size */
	uint32_t magic_tag;			/*!< tag [0xAF551229] */
	uint32_t major_version;		/*!< version major */
	uint32_t minor_version;		/*!< version minor */
	binary_chip_arch chip_arch;	/*!< support chip platform */
}loader_header;

typedef loader_header* Ploader_header;

/**
 * @ingroup storage_props
 * app file header
 */

#define VATEK_APP_EXTENSION				".v2app"
#define VATEK_UPDATE_EXTENSION			".v2update"
#define VATEK_IMAGE_EXTENSION			".v2img"

#define APP_MAGIC_V2					0xAF550122
#define APP_POSITION					LOADER_SIZE
#define APP_SECTION_TAG					0xAF550803

#define BIN_SECTION_TAG					0xA7B60000
#define BIN_SECTION_MASK				0xFFFF0000

typedef struct _app_header{
	uint32_t bin_size;				/*!< app file total size */
	uint32_t magic_tag;				/*!< tag [0xAF550122] */
	uint32_t major_version;			/*!< version major */
	uint32_t minor_version;			/*!< version minor */
	binary_chip_arch chip_arch;		/*!< support chip platform */
	hal_service_mode service_mode;
	uint32_t crc32;					/*!< app binary crc */
}app_header;

typedef app_header* Papp_header;

/**
 * @ingroup storage_props
 * chip hardware config 
 */

#define CHIP_CFGV0_TAG					0x200506FF		/* enable dac extend R mode and status led only */
#define CHIP_CFGV1_TAG					0x200507FF		/* added usb customized */

#define CHIP_EN_DAC_EXTR				0x00000001
#define CHIP_EN_STATUSLED				0x00000002
#define CHIP_EN_USBID					0x00000004
#define CHIP_EN_USBSTR					0x00000008
#define CHIP_EN_R2_EXTR					0x00000010      /* 20211119 new function R2 dc-block */

#define CHIP_USBSTR_LEN					16

#define CHIPLED_OFFSET_IDLE				0
#define CHIPLED_OFFSET_RUN				8
#define CHIPLED_OFFSET_ERR				16

#define VENDOR_EN_EXT_VSYNC				0x00000001

typedef struct _chip_config
{
	uint32_t tag;
	uint32_t functions;
	uint32_t status_led;
	uint32_t usb_vid;
	uint32_t usb_pid;
	char usb_str[CHIP_USBSTR_LEN];
	uint32_t vendor_functions;
}chip_config;

typedef chip_config* Pchip_config;

/**
 * @ingroup storage_props
 * binary section resource header
 */

#define BIN_SECTION_NULL				(0)
#define BIN_SECTION_STORAGE				(BIN_SECTION_TAG | 0x01)
#define BIN_SECTION_RESOURCE			(BIN_SECTION_TAG | 0x02)
#define BIN_SECTION_R2TUNE				(BIN_SECTION_TAG | 0x04)
#define BIN_SECTION_GAINTABLE			(BIN_SECTION_TAG | 0x08)

#define is_section_valid(tag)			((tag & BIN_SECTION_MASK) == BIN_SECTION_TAG)

typedef struct _bin_section_header{
	uint32_t tag;
	uint32_t len;
	uint32_t crc32;
}bin_section_header;

typedef bin_section_header* Pbin_section_header;

#define RESOURCE_COLOR_LEN				256
#define RESOURCE_RAW_TAG				0xFAF80122

#define RESOURCE_MB_WIDTH				64
#define RESOURCE_MB_HEIGHT				32
#define RESOURCE_Y_MB_SIZE				(RESOURCE_MB_WIDTH * RESOURCE_MB_HEIGHT)
#define RESOURCE_UV_MB_SIZE				(RESOURCE_Y_MB_SIZE >> 1)

#define RESOURCE_W_MB(v)				((v + (RESOURCE_MB_WIDTH - 1))/RESOURCE_MB_WIDTH)
#define RESOURCE_H_MB(v)				((v + (RESOURCE_MB_HEIGHT - 1))/RESOURCE_MB_HEIGHT)

#define RESOURCE_MB_SIZE(w,h)			(RESOURCE_W_MB(w)*RESOURCE_H_MB(h))
#define RESOURCE_BUF_Y_SIZE(w,h)		(RESOURCE_MB_SIZE(w,h) * RESOURCE_Y_MB_SIZE)
#define RESOURCE_BUF_UV_SIZE(w,h)		(RESOURCE_MB_SIZE(w,h) * RESOURCE_UV_MB_SIZE)
#define RESOURCE_BUF_SIZE(w,h)			(RESOURCE_BUF_Y_SIZE(w,h) + RESOURCE_BUF_UV_SIZE(w,h))

#define RESIDX_TAG						0
#define RESIDX_MB_W						1
#define RESIDX_MB_H						2
#define RESIDX_SIZE_Y					3
#define RESIDX_SIZE_UV					4

typedef struct _bin_resource_section{
	uint32_t width;							/*!< image width */
	uint32_t height;						/*!< image height */
	uint32_t index;							/*!< image index */
	uint32_t bkcolor;						/*!< background color */
	uint32_t table[RESOURCE_COLOR_LEN];		/*!< color table */
	uint8_t* rawpixel;						/*!< raw buffer */
}bin_resource_section;

typedef bin_resource_section* Pbin_resource_section;

#endif
