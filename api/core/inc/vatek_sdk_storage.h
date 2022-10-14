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

#ifndef _VATEK_SDK_STORAGE_
#define _VATEK_SDK_STORAGE_

#include <stdio.h>
#include <core/base/binary_rom.h>
#include <core/base/output_rfmixer.h>
#include <core/storage/storage_config.h>
#include <core/storage/storage_broadcast.h>
#include <core/storage/storage_transform.h>

typedef void* hvatek_storage;

typedef vatek_result(*fprom_read_section)(int32_t nsection, uint8_t* psection,void* param);
typedef vatek_result(*fprom_write_section)(int32_t nsection, uint8_t* psection, void* param);
typedef uint8_t* (*fprom_malloc_memory)(int32_t len, void* param);
typedef void (*fprom_free_memory)(uint8_t* pptr, void* param);

typedef enum _rom_progress_msg{
	rom_msg_set_pos = 0,
	rom_msg_set_str = 1,
}rom_progress_msg;

typedef void(*fprom_progress)(rom_progress_msg msg,void* progressval,void* param);

typedef struct _storage_handle{
	void* param;
	fprom_read_section read;
	fprom_write_section write;
	fprom_malloc_memory malloc;
	fprom_free_memory free;
	fprom_progress progress;
}storage_handle;

typedef storage_handle* Pstorage_handle;

typedef struct _storage_resource{
	struct _storage_resource* next;
	uint32_t pos;
	Pbin_resource_section resource;
}storage_resource;

typedef storage_resource* Pstorage_resource;

typedef vatek_result (*fpfile_get_length)(void* param);

typedef struct _romfile_handle{
	void* param;
	fpfile_get_length get_length;
	fprom_read_section get_section;
}romfile_handle;

typedef romfile_handle* Promfile_handle;

#ifdef __cplusplus
extern "C" {
#endif

	HAL_API vatek_result vatek_storage_create_chip_handle(hvatek_chip hchip, Pstorage_handle* phandle, fprom_progress fpcb,void* cbparam);
	HAL_API vatek_result vatek_storage_create_file_handle(const char* fimage, 
														  const char* floader, 
														  const char* fapp,
														  Pstorage_handle* phandle, 
														  fprom_progress fpcb, 
														  void* cbparam);

	HAL_API vatek_result vatek_storage_open_file_handle(const char* filename, Pstorage_handle* phandle, fprom_progress fpcb, void* cbparam);
	HAL_API void vatek_storage_free_handle(Pstorage_handle phandle);

	HAL_API vatek_result vatek_storage_open(Pstorage_handle phandler, hvatek_storage* hstorage,int32_t isresource);
	HAL_API vatek_result vatek_storage_get_app(hvatek_storage hstorage, Papp_header* papp);
	HAL_API vatek_result vatek_storage_get_loader(hvatek_storage hstorage, Ploader_header* ploader);
	HAL_API vatek_result vatek_storage_get_resource(hvatek_storage hstorage, Pstorage_resource* pres);
	HAL_API vatek_result vatek_storage_add_resource(hvatek_storage hstorage, Pstorage_resource pres);
	HAL_API vatek_result vatek_storage_del_resource(hvatek_storage hstorage, Pstorage_resource pres);
	HAL_API vatek_result vatek_storage_update_resource(hvatek_storage hstorage);

	HAL_API Pr2_tune_handle vatek_storage_get_r2tune(hvatek_storage hstorage);
	HAL_API Pstorage_broadcast vatek_storage_get_broadcast(hvatek_storage hstorage);
	HAL_API Pstorage_transform vatek_storage_get_transform(hvatek_storage hstorage);
	HAL_API Pstorage_chip_config vatek_storage_get_config(hvatek_storage hstorage);

	HAL_API vatek_result vatek_storage_save(hvatek_storage hstorage, const char* filename);
	HAL_API vatek_result vatek_storage_close(hvatek_storage hstorage);

	HAL_API vatek_result vatek_storage_romfile_create(const char* romfle, Promfile_handle* promfile);
	HAL_API vatek_result vatek_storage_romfile_free(Promfile_handle promfile);

	HAL_API vatek_result vatek_storage_update(hvatek_storage hstorage, Promfile_handle pimage);
	HAL_API vatek_result vatek_storage_dump(hvatek_storage hstorage, FILE* hfile);

	HAL_API vatek_result vatek_storage_write_image(hvatek_storage hstorage, Promfile_handle pimage);
	HAL_API vatek_result vatek_storage_write_update(hvatek_storage hstorage, Promfile_handle pimage);
	HAL_API vatek_result vatek_storage_write_app(hvatek_storage hstorage, Promfile_handle papp);

	HAL_API uint32_t vatek_storage_crc(uint8_t* pbuf, int32_t len);

	HAL_API vatek_result vatek_storage_resource_create(hvatek_storage hstorage, uint32_t w, uint32_t h, Pstorage_resource* pres);

#ifdef __cplusplus
}
#endif

#endif
