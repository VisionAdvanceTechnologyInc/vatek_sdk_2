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

#ifndef _SERVICE_BASE_
#define _SERVICE_BASE_

#include <vatek_sdk_device.h>
#include <core/hal/halservice_base.h>
#include <core/ui/ui_props/ui_props_output.h>
#include <core/ui/ui_props/ui_props_stream.h>

/**
 * @ingroup broadcast_api
 * broadcast status when running show current status
 */
typedef enum _broadcast_status{
	bcstatus_fail_unknown = BCSTATUS_FAIL_UNKNOWN,		/*!< unknown fail */
	bcstatus_fail_source = BCSTATUS_FAIL_SOURCE,		/*!< stream (input) fail*/
	bcstatus_fail_timeout = BCSTATUS_FAIL_TIMEOUT,		/*!< wait stream (input) timeout */
	bcstatus_fail_drop = BCSTATUS_FAIL_CODECDROP,		/*!< video encoder drop frame */
	bcstatus_fail_buffer = BCSTATUS_FAIL_BUFFER,		/*!< buffer fail */
	bcstatus_fail_muxer = BCSTATUS_FAIL_MUXER,			/*!< muxer fail */
	bcstatus_fail_encode = BCSTATUS_FAIL_ENCODE,		/*!< encoder fail */
	bcstatus_fail_media = BCSTATUS_FAIL_MEDIA,			/*!< media fail */
	bcstatus_fail_demux = BCSTATUS_FAIL_DEMUX,			/*!< _demux fail */
	bcstatus_idle = BCSTATUS_IDLE,						/*!< not running */
	bcstatus_wait_source = BCSTATUS_WAIT_SOURCE,		/*!< wait stream (input) ready */
	bcstatus_broadcast = BCSTATUS_BROADCAST,			/*!< running */
	bcstatus_finish = BCSTATUS_FINISH,					/*!< operation finish */
}broadcast_status;

#define is_bcstatus_fail(status)	(status & bcstatus_fail_unknown)
#define is_bcstatus_running(status)	(status > bcstatus_idle && status <= bcstatus_finish)

/**
 * @ingroup broadcast_api
 * when broadcast status running show information
 */
typedef struct _broadcast_info{
	stream_source source;								/*!< current stream (input) */
	broadcast_status status;							/*!< current broadcast status */
	uint32_t mod_bitrate;								/*!< output current data bitrate */
	uint32_t mux_bitrate;								/*!< mux output bitrate */
	uint32_t cur_bitrate;								/*!< current muxer output bitrate */
	uint32_t data_bitrate;								/*!< muxer input valid data bitrate */
	uint32_t query_buffer;
}broadcast_info;

typedef broadcast_info* Pbroadcast_info;

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * @ingroup service_api
	 * @brief write broadcast information parameters to device (HAL_REGISTER)
	 *
	 * @param[in] hchip device handle
	 * @param[in] pinfo broadcast information parameters
	 * @return >= 0 mean success, other error
	 */
	HAL_API vatek_result broadcast_info_set(hvatek_chip hchip, Pbroadcast_info pinfo);

	/**
	 * @ingroup service_api
	 * @brief read broadcast information parameters from device (HAL_REGISTER)
	 *
	 * @param[in] hchip device handle
	 * @param[out] pinfo broadcast information parameters
	 * @return >= 0 mean success, other error
	 */
	HAL_API vatek_result broadcast_info_get(hvatek_chip hchip, Pbroadcast_info pinfo);

	/**
	 * @ingroup service_api
	 * @brief read broadcast status from device (HAL_REGISTER)
	 *
	 * @param[in] hchip device handle
	 * @param[out] status broadcast status
	 * @return HAL_API
	 */
	HAL_API vatek_result broadcast_status_get(hvatek_chip hchip, broadcast_status* status);

	/**
	 * @ingroup service_api
	 * @brief write broadcast status to device (HAL_REGISTER)
	 *
	 * @param[in] hchip device handle
	 * @param[in] status broadcast status
	 * @return HAL_API
	 */
	HAL_API vatek_result broadcast_status_set(hvatek_chip hchip, broadcast_status status);

#ifdef __cplusplus
}
#endif

#endif
