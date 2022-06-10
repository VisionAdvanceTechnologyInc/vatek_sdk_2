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

#ifndef _STORAGE_BROADCAST_
#define _STORAGE_BROADCAST_

#include <core/base/binary_rom.h>
#include <core/base/stream_base.h>
#include <core/base/output_base.h>

#define BIN_SECTION_BROADCAST		(BIN_SECTION_TAG | 0x10)
#define BROADCAST_RAWDATA_LEN	    1024*3

typedef struct _storage_broadcast
{
    modulator_param modulator;
	audio_param audio;							/*!< default audio param */
	mpeg2_param mpeg2[MEDIA_IDX_VIDEO_NUMS];	/*!< default mpeg2 encoder config [SD,HD,FULL HD]*/
	h264_param h264[MEDIA_IDX_VIDEO_NUMS];		/*!< default h264 encoder config [SD,HD,FULL HD]*/
}storage_broadcast;

typedef storage_broadcast* Pstorage_broadcast;

/* storage must less than section_size */
_VASSERT(sizeof(storage_broadcast) < BROADCAST_RAWDATA_LEN, storage_overflow);

#ifdef __cplusplus
extern "C" {
#endif

	HAL_API void audio_param_reset(Paudio_param paudio);
	HAL_API void mpeg2_param_reset(video_resolution resolution, Pmpeg2_param pmpeg2);
	HAL_API void h264_param_reset(video_resolution resolution, Ph264_param ph264);

	HAL_API void storage_broadcast_reset(Pstorage_broadcast pbc);
	HAL_API vatek_result storage_broadcast_set(Pstorage_broadcast pbc,uint8_t* psection);
	HAL_API vatek_result storage_broadcast_get(Pstorage_broadcast pbc,uint8_t* psection);

#ifdef __cplusplus
}
#endif

#endif
