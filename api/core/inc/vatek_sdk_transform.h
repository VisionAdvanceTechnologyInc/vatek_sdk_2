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

#ifndef _VATEK_SDK_TRANSFORM_
#define _VATEK_SDK_TRANSFORM_

#include <service/service_transform.h>
#include <mux/mux_handle.h>

typedef void* hvatek_transform;

#ifdef __cplusplus
extern "C" {
#endif

	HAL_API vatek_result vatek_transform_open(hvatek_chip hchip,hvatek_transform* htr);

	HAL_API vatek_result vatek_transform_start_enum(hvatek_transform htr, Ptransform_enum penum);
	HAL_API vatek_result vatek_transform_start_capture(hvatek_transform htr, Ptransform_capture pcapture);
	HAL_API vatek_result vatek_transform_start_broadcast(hvatek_transform htr, Ptransform_broadcast pbc, r2_param r2param);

	HAL_API vatek_result vatek_transform_polling(hvatek_transform htr, Ptransform_info* pinfo);
	HAL_API Ptransform_info vatek_transform_get_info(hvatek_transform htr);

	HAL_API vatek_result vatek_transform_get_packets(hvatek_transform htr, uint32_t* pktnums);
	HAL_API vatek_result vatek_transform_commit_packets(hvatek_transform htr);

	HAL_API vatek_result vatek_transform_stop(hvatek_transform htr);

	HAL_API vatek_result vatek_transform_enum_get_context(hvatek_transform htr, hmux_channel* hchannel);

	HAL_API void vatek_transform_close(hvatek_transform htr);

#ifdef __cplusplus
}
#endif

#endif
