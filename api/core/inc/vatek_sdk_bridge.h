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

#ifndef _VATEK_SDK_BRIDGE_
#define _VATEK_SDK_BRIDGE_

#include <vatek_sdk_device.h>
#include <bridge/bridge_device.h>

typedef void* hvatek_bridge;

#ifdef __cplusplus
extern "C" {
#endif

	HAL_API vatek_result vatek_bridge_open(hvatek_chip hchip, hvatek_bridge* hbridge);

	HAL_API Pbdevice_info vatek_bridge_get_info(hvatek_bridge hbridge);
	HAL_API vatek_result vatek_bridge_get_av_source(hvatek_bridge hbridge, int32_t idx,Pbridge_source psource);
	HAL_API const char* vatek_bridge_get_av_source_name(hvatek_bridge hbridge,Pbridge_source psource);

	HAL_API vatek_result vatek_bridge_start_av_source(hvatek_bridge hbridge, Pbridge_source psource);
	HAL_API vatek_result vatek_bridge_get_av_source_status(hvatek_bridge hbridge, Pbridge_source psource);
	HAL_API vatek_result vatek_bridge_stop_av_source(hvatek_bridge hbridge);

	HAL_API vatek_result vatek_bridge_write_section(hvatek_bridge hbridge, int32_t section, uint8_t* pbuffer);
	HAL_API vatek_result vatek_bridge_read_section(hvatek_bridge hbridge, int32_t section, uint8_t* pbuffer);

	HAL_API void vatek_bridge_close(hvatek_bridge hbridge);

#ifdef __cplusplus
}
#endif

#endif
