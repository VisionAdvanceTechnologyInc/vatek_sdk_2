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

#ifndef _VATEK_SDK_USBSTREAM_
#define _VATEK_SDK_USBSTREAM_

#include <service/service_transform.h>

typedef void* hvatek_usbstream;

typedef struct _usbstream_slice{
	int32_t packet_len;			/*!< recv packet length (packet numbers)*/
	int32_t packet_pos;			/*!< current buffer pos */
	uint8_t* buf;				/*!< raw buffer */
	uint8_t* ptrbuf;			/*!< buffer pointer */
}usbstream_slice;

typedef usbstream_slice* Pusbstream_slice;

typedef enum _usbstream_status{
	usbstream_err_unknown = -1,		/*!< unknown fail */
	usbstream_status_idle = 0,		/*!< idle */
	usbstream_status_running = 1,	/*!< running (streaming)*/
	usbstream_status_moredata = 2,	/*!< more data (need more data to start) */
	usbstream_status_stopping = 3,	/*!< wait stop */
	usbstream_status_stop = 4,		/*!< stop finish */
}usbstream_status;

typedef enum _usbstream_mode{
	ustream_mode_async = 0,			/*!< async */
	ustream_mode_sync = 1,			/*!< sync */
}usbstream_mode;

typedef enum _usbstream_remux
{
	ustream_remux_pcr,
	ustream_remux_passthrough,
}usbstream_remux;

typedef vatek_result(*fpsync_get_buffer)(void* param, uint8_t** slicebuf);

typedef struct _usbstream_sync{
	void* param;
	fpsync_get_buffer getbuffer;
}usbstream_sync;

typedef usbstream_sync* Pusbstream_sync;

typedef enum _uasync_status
{
	uasync_status_unsupport = -1,
	uasync_status_idle = 0,
	uasync_status_prepare,
	uasync_status_streaming,
	uasync_status_pause,
}uasync_status;

typedef enum _uasync_mode
{
	uasync_mode_normal = 0,
	uasync_mode_cbr,
}uasync_mode;

typedef struct _usbstream_async
{
	uasync_mode mode;
	uint32_t bitrate;			/* config source bitrate	[0: mean auto]*/
	uint32_t prepare_ms;		/* config prepare time ms	[0: mean auto]*/
}usbstream_async;

typedef usbstream_async* Pusbstream_async;

typedef struct _usbstream_param{
	usbstream_mode mode;
	usbstream_remux remux;
	pcr_adjust_mode pcradjust;
	//uint32_t freq_khz;
	r2_param r2param;
	modulator_param modulator;
	union
	{
		usbstream_sync sync;
		usbstream_async async;
	};
}usbstream_param;

typedef usbstream_param* Pusbstream_param;

#ifdef __cplusplus
extern "C" {
#endif

	HAL_API vatek_result vatek_usbstream_open(hvatek_chip hchip, hvatek_usbstream* husstream);
	HAL_API vatek_result vatek_usbstream_check(hvatek_usbstream husstream);
	HAL_API Pbroadcast_info vatek_usbstream_get_info(hvatek_usbstream husstream);
	HAL_API vatek_result vatek_usbstream_start(hvatek_usbstream husstream,Pusbstream_param puparam);
	HAL_API usbstream_status vatek_usbstream_get_status(hvatek_usbstream husstream,Ptransform_info* ptrinfo);

	HAL_API uasync_status vatek_ustream_async_get_status(hvatek_usbstream hustream);
	HAL_API vatek_result vatek_ustream_async_get_buffer(hvatek_usbstream husstream, Pusbstream_slice* pslicebuf);
	HAL_API vatek_result vatek_ustream_async_commit_buffer(hvatek_usbstream husstream, Pusbstream_slice pslicebuf);
	
	HAL_API vatek_result vatek_usbstream_stop(hvatek_usbstream husstream);
	HAL_API void vatek_usbstream_close(hvatek_usbstream husstream);

#ifdef __cplusplus
}
#endif

#endif
