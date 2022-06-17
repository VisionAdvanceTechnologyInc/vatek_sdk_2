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

#ifndef _VATEK_SDK_USBMUX_
#define _VATEK_SDK_USBMUX_

#include <service/service_transform.h>
#include <mux/mux_spec.h>
#include <mux/mux_packet.h>

typedef void* hvatek_usbmux;

typedef enum _usbmux_status{
	umux_err_unknown = -1,
	umux_status_idle = 0,
	umux_status_ready = 1,
	umux_status_prepare = 2,
	umux_status_running = 3,
	umux_status_stoping = 4,
	umux_status_stoped = 5,
}usbmux_status;

typedef union _usbmux_detail{
	media_video video;
	media_audio audio;
}usbmux_detail;

typedef struct _usbmux_stream{
	uint16_t pid;
	mux_stream_type type;
	int32_t es_info_len;
	uint8_t* es_info_buf;
	usbmux_detail _stream_info;
	void* private_data;
}usbmux_stream;

typedef usbmux_stream* Pusbmux_stream;

typedef struct _usbmux_program{
	uint16_t pmtpid;
	uint16_t recv;
	Pusbmux_stream streams;
	void* private_data;
}usbmux_program;

typedef usbmux_program* Pusbmux_program;

typedef void* husbmux_source;

typedef vatek_result(*fpusource_start)(husbmux_source hsource);
typedef vatek_result(*fpusource_freeframe)(husbmux_source hsource, Pusbmux_stream pstream, Pmux_pes_frame pframe);
typedef vatek_result(*fpusource_get_padding)(husbmux_source hsource, uint8_t** packet);
typedef void(*fpsource_prepare)(husbmux_source hsource, uint32_t ms);
typedef void(*fpsource_stop)(husbmux_source hsource);

typedef struct _usbmux_source{
	husbmux_source humsource;
	fpusource_start start;
	fpsource_prepare prepare;
	fpsource_stop stop;
	fpusource_freeframe freeframe;
	fpusource_get_padding getpadding;
}usbmux_source;

typedef usbmux_source* Pusbmux_source;

typedef struct _usbmux_param{
	uint16_t pcr_pid;
	uint16_t interval_ms;
	uint16_t latency_ms;
	uint16_t recv;
	r2_param r2param;
	uint32_t bitrate;
	modulator_param modulator;
}usbmux_param;

typedef usbmux_param* Pusbmux_param;

#ifdef __cplusplus
extern "C" {
#endif

	HAL_API vatek_result vatek_usbmux_create(hvatek_chip hchip, Pusbmux_param puparam,hvatek_usbmux* husbmux);
	HAL_API void vatek_usbmux_free(hvatek_usbmux husbmux);

	HAL_API vatek_result vatek_usbmux_open_rule(hvatek_usbmux husbmux, const char* filerule);
	HAL_API vatek_result vatek_usbmux_open_spec(hvatek_usbmux husbmux, mux_spec_mode spec, mux_country_code country);
	HAL_API void vatek_usbmux_close(hvatek_usbmux husbmux);
	
	HAL_API vatek_result vatek_usbmux_reset(hvatek_usbmux husbmux);
	HAL_API vatek_result vatek_usbmux_get_param(hvatek_usbmux husbmux,Pspec_channel* pchparam);
	HAL_API Pbroadcast_info vatek_usbmux_get_info(hvatek_usbmux husbmux);
	HAL_API vatek_result vatek_usbmux_replace_param(hvatek_usbmux husbmux, Pspec_channel pchparam);

	HAL_API mux_spec_mode vatek_usbmux_get_spec(hvatek_usbmux husbmux);
	HAL_API mux_country_code vatek_usbmux_get_country(hvatek_usbmux husbmux);

	HAL_API vatek_result vatek_usbmux_add_program(hvatek_usbmux husbmux, uint16_t pmtpid, Pusbmux_program* program);
	HAL_API vatek_result vatek_usbmux_program_get_param(Pusbmux_program program,Pspec_program* pprogparam);
	HAL_API vatek_result vatek_usbmux_program_replace_param(Pusbmux_program program, Pspec_program pprogram);

	HAL_API vatek_result vatek_usbmux_add_video_stream(Pusbmux_program program, uint16_t pid, Pmedia_video pvideo, Pusbmux_stream* pstream);
	HAL_API vatek_result vatek_usbmux_add_audio_stream(Pusbmux_program program, uint16_t pid, Pmedia_audio paudio, Pusbmux_stream* pstream);
	HAL_API vatek_result vatek_usbmux_add_stream(Pusbmux_program program, uint16_t pid, uint8_t streamtype, Pusbmux_stream* pstream);
	HAL_API vatek_result vatek_usbmux_stream_set_esinfo(Pusbmux_stream pstream, uint8_t* pesbuf, int32_t neslen);

	HAL_API void vatek_usbmux_stream_set_private(Pusbmux_stream pstream, void* param);
	HAL_API void* vatek_usbmux_stream_get_private(Pusbmux_stream pstream);

	HAL_API vatek_result vatek_usbmux_finish(hvatek_usbmux husbmux);

	HAL_API vatek_result vatek_usbmux_start(hvatek_usbmux husbmux,Pusbmux_source psource);

	HAL_API usbmux_status vatek_usbmux_get_status(hvatek_usbmux husbmux, Ptransform_info* pinfo);
	HAL_API vatek_result vatek_usbmux_push_frame(hvatek_usbmux husbmux, Pusbmux_stream pstream, Pmux_pes_frame pframe);

	HAL_API void vatek_usbmux_stop(hvatek_usbmux husbmux);

#ifdef __cplusplus
}
#endif

#endif
