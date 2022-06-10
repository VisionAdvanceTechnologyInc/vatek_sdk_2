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

#ifndef _SERVICE_TRANSFORM_
#define _SERVICE_TRANSFORM_

#include <service/service_base.h>
#include <core/hal/halservice_transform.h>
#include <mux/mux_handle.h>

#define TRCAPTURE_MAX_PACKETNUMS			10
#define TRCAPTURE_DEFAILT_TIMEOUT			10000
#define TRSLICE_PACKET_NUMS					(CHIP_STREAM_SLICE_LEN/TS_PACKET_LEN)

/**
 * @ingroup transform_api
 * @brief transform operation mode 
 */
typedef enum _transform_mode{
	trmode_null = 0,
	trmode_enum = TRANSFORM_ENUM,				/*!< enumerate ts stream context */
	trmode_capture = TRANSFORM_CAPTURE,			/*!< capture input ts stream packets */
	trmode_broadcast = TRANSFORM_BROADCAST,		/*!< start broadcast (transform source) */
}transform_mode;

#define is_transform_mode(m)	(m >= trmode_enum && m <= trmode_broadcast)

/**
 * @ingroup transform_api
 * @brief transform information parameters
 * 
 */
typedef struct _transform_info{
	broadcast_info info;	/*!< common base information */
	transform_mode mode;	/*!< current operation mode */
}transform_info;

typedef transform_info* Ptransform_info;

/**
 * @ingroup transform_api
 * @brief transform source detail parameters
 * 
 */
typedef union _transform_source
{
	tsin_param tsin;		/*!< tsif parameters */
	usb_param usb;			/*!< usb parameters */
}transform_source;

typedef transform_source* Ptransform_source;

typedef struct _transform_capture{
	stream_source source;
	transform_source stream;
	uint16_t pid;			/*!< capture pid */
	uint16_t timeout;		/*!< timeout config */
	uint16_t section_num;
	uint16_t packet_nums;
}transform_capture;

typedef transform_capture* Ptransform_capture;

typedef struct _transform_enum
{
	stream_source source;
	transform_source stream;
	uint16_t recv;
	uint16_t timeout;
}transform_enum,*Ptransform_enum;

typedef struct _transform_broadcast {
	stream_source source;
	transform_source stream;
	pid_filter filters;
	modulator_param modulator;	/*!< output modulation param */
}transform_broadcast;

typedef transform_broadcast* Ptransform_broadcast;

#ifdef __cplusplus
extern "C" {
#endif

	HAL_API vatek_result transform_enum_reset(vatek_ic_module icchip, stream_source source, Ptransform_enum penum);
	HAL_API vatek_result transform_enum_set(hvatek_chip hchip,Ptransform_enum penum);
	HAL_API vatek_result transform_enum_get(hvatek_chip hchip,Ptransform_enum penum);

	HAL_API vatek_result transform_capture_reset(vatek_ic_module icchip,stream_source source, Ptransform_capture pcapture);
	HAL_API vatek_result transform_capture_set(hvatek_chip hchip,Ptransform_capture pcapture);
	HAL_API vatek_result transform_capture_get(hvatek_chip hchip,Ptransform_capture pcapture);

	HAL_API vatek_result transform_broadcast_reset(vatek_ic_module icchip,stream_source source, Ptransform_broadcast pbc);
	HAL_API vatek_result transform_broadcast_set(hvatek_chip hchip, Ptransform_broadcast pbc);
	HAL_API vatek_result transform_broadcast_get(hvatek_chip hchip, Ptransform_broadcast pbc);

	HAL_API vatek_result transform_mode_set(hvatek_chip hchip, transform_mode mode);
	HAL_API vatek_result transform_mode_get(hvatek_chip hchip, transform_mode* mode);

	HAL_API stream_mode transform_source_get_stream_mode(stream_source source, Ptransform_source pstream);

#ifdef __cplusplus
}
#endif

#endif
