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

#ifndef _SERVICE_BROADCAST_
#define _SERVICE_BROADCAST_

#include <service/service_base.h>
#include <core/hal/halservice_broadcast.h>
#include <mux/mux_handle.h>

#define BCUSB_ESINFO_MAXLEN				48

/**
 * @ingroup broadcast_api
 * @brief mpeg_ts stream multiplexer parameters
 * 
 */
typedef struct _mux_param{
	uint16_t pcr_pid;									/*!< pcr_pid */
	uint16_t padding_pid;								/*!< pid used with padding packet [0x1FFF default] */
	uint32_t bitrate;									/*!< mux ip output bitrate [0 mean auto ] */
	uint16_t stuffing_pid;								/*!< pid used with stuffing packet [0 mean disable] */
	uint16_t recv;
	uint32_t en_function;								/*!< mux function en flags with MUX_FLAG_EN_XX define */
}mux_param,*Pmux_param;

static const mux_param default_mux_param =
{
	0x100,0x1FFF,19000000,0,0,0,
};

/**
 * @ingroup broadcast_api
 * @brief broadcast parameters used to start DTV broadcasting
 */
typedef struct _broadcast_param{
	encoder_param enc;				/*!< encoder parameters */
    modulator_param mod;			/*!< output DTV modulation parameters */
    mux_param mux;					/*!< MPEG-TS stream multiplexer parameters */
}broadcast_param;

typedef broadcast_param* Pbroadcast_param;

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * @ingroup broadcast_api
	 * @brief reset broadcast parameters to default used vatek_ic_module.
	 * 
	 * @param[in] icchip target vatek_ic_module.
	 * @param[out] pbc broadcast parameters
	 * @return >= 0 mean success, other error  
	 */
	HAL_API vatek_result broadcast_param_reset(vatek_ic_module icchip, Pbroadcast_param pbc);

	/**
	 * @ingroup broadcast_api
	 * @brief write broadcast parameters to device (HAL_REGISTER).
	 * 
	 * @param[in] hchip device handle
	 * @param[in] pbc broadcast parameters
	 * @return >= 0 mean success, or error code.
	 */
	HAL_API vatek_result broadcast_param_set(hvatek_chip hchip,Pbroadcast_param pbc);

	/**
	 * @ingroup broadcast_api
	 * @brief read broadcast parameters from device (HAL_REGISTER)
	 * 
	 * @param[in] hchip device handle
	 * @param[out] pbc broadcast parameters
	 * @return >= 0 mean success, other error  
	 */
	HAL_API vatek_result broadcast_param_get(hvatek_chip hchip,Pbroadcast_param pbc);

#ifdef __cplusplus
}
#endif


#endif
