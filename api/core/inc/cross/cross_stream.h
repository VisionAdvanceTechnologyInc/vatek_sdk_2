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

#ifndef _CROSS_STREAM_
#define _CROSS_STREAM_

#include <cross/cross_os_api.h>
#include <core/base/media_video.h>
#include <core/base/media_audio.h>
#include <core/base/output_modulator.h>

typedef void* hcross_stream;

#ifdef __cplusplus
extern "C" {
#endif

	HAL_API vatek_result cross_stream_open_test(uint32_t bitrate, hcross_stream* hcstream);
	HAL_API vatek_result cross_stream_open_file(const char* szfilename, hcross_stream* hcstream);
	HAL_API vatek_result cross_stream_open_udp(const char* szurl, hcross_stream* hcstream);
	HAL_API vatek_result cross_stream_open_mux(Pmodulator_param pmod, hcross_stream* hcstream);

	HAL_API vatek_result cross_stream_start(hcross_stream hcstream);
	HAL_API vatek_result cross_stream_get_slice(hcross_stream hcstream,uint8_t** pslice);
	HAL_API uint32_t cross_stream_get_bitrate(hcross_stream hcstream);
	HAL_API void cross_stream_stop(hcross_stream hcstream);

	HAL_API void cross_stream_close(hcross_stream hcstream);

#ifdef __cplusplus
}
#endif

#endif
