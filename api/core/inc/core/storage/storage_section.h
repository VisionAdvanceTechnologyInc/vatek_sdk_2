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

#ifndef _STORAGE_SECTION_
#define _STORAGE_SECTION_

#include <core/base/binary_rom.h>
#include <core/storage/storage_broadcast.h>
#include <core/storage/storage_transform.h>
#include <core/storage/storage_r2tune.h>
#include <core/storage/storage_config.h>

#define STORAGE_APP_ADDRESS     LOADER_SIZE
#define STORAGE_PROTECT_1		((LOADER_SIZE - BINARY_SECTION_SIZE)/BINARY_SECTION_SIZE)
#define STORAGE_PROTECT_0		((LOADER_SIZE - (BINARY_SECTION_SIZE << 1))/BINARY_SECTION_SIZE)

#ifdef __cplusplus
extern "C" {
#endif

    HAL_API vatek_result storage_section_check_tag(uint8_t* psection,uint32_t* tag,uint32_t* sectionnums);

    HAL_API vatek_result storage_section_check_loader(uint8_t* psection);
    HAL_API vatek_result storage_section_check_app(uint8_t* psection);

    HAL_API vatek_result storage_section_get_loader(uint8_t* psection,Ploader_header plhdr);
    HAL_API vatek_result storage_section_get_app(uint8_t* psection,Papp_header papphdr);

    HAL_API vatek_result storage_section_set_loader(uint8_t* psection, Ploader_header plhdr);
    HAL_API vatek_result storage_section_set_app(uint8_t* psection, Papp_header papphdr);

    /* return size of raw data used */
    HAL_API vatek_result storage_section_put_resource(uint8_t* psection,int32_t size, Pbin_resource_section pbin,uint8_t* praw);
    /* return position of raw data */
    HAL_API vatek_result storage_section_get_resource(uint8_t* psection, Pbin_resource_section pbin);

#ifdef __cplusplus
}
#endif

#endif
