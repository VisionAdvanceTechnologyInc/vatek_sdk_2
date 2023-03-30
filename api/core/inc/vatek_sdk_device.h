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

#ifndef _VATEK_SDK_DEVICE_
#define _VATEK_SDK_DEVICE_

#include <core/vatek_base.h>
#include <core/base/chip_define.h>
#include <core/base/output_modulator.h>
#include <core/base/calibration_define.h>
#include <core/base/device_usb.h>
#ifdef __cplusplus
extern "C" {
#endif

	HAL_API vatek_result vatek_device_list_enum(uint32_t bus,hal_service_mode service,hvatek_devices* hdevices);
	HAL_API vatek_result vatek_device_list_enum_by_usbid(uint16_t vid, uint16_t pid, hvatek_devices* hdevices);
	HAL_API uint32_t vatek_device_list_get_bus(hvatek_devices hdevices, int32_t idx);
	HAL_API const char* vatek_device_list_get_name(hvatek_devices hdevices, int32_t idx);
	HAL_API hal_service_mode vatek_device_list_get_service(hvatek_devices hdevices, int32_t idx);
	HAL_API void vatek_device_list_free(hvatek_devices hdevices);
	HAL_API vatek_result vatek_device_open(hvatek_devices hdevices, int32_t idx, hvatek_chip* hchip);

	HAL_API vatek_result vatek_device_start_sine(hvatek_chip hchip,uint32_t freqkhz);
	HAL_API vatek_result vatek_device_start_test(hvatek_chip hchip, Pmodulator_param pmod,uint32_t freqkhz);
	HAL_API vatek_result vatek_device_polling(hvatek_chip hchip);
	HAL_API void vatek_device_stop(hvatek_chip hchip);

	HAL_API uint32_t vatek_device_get_bus(hvatek_chip hchip);
	HAL_API Pchip_info vatek_device_get_info(hvatek_chip hchip);
	HAL_API const char* vatek_device_get_name(hvatek_chip hchip);
	HAL_API vatek_result vatek_device_close(hvatek_chip hchip);

	HAL_API vatek_result vatek_device_close_reboot(hvatek_chip hchip);

	HAL_API vatek_result vatek_device_calibration_load(hvatek_chip hchip, Pcalibration_param pcalibration);
	HAL_API vatek_result vatek_device_calibration_apply(hvatek_chip hchip, Pcalibration_param pcalibration);
	HAL_API vatek_result vatek_device_calibration_save(hvatek_chip hchip, Pcalibration_param pcalibration);
	HAL_API vatek_result vatek_device_r2_apply(hvatek_chip hchip, int r2_power);

	/* used with transform service for usb stream */
	HAL_API vatek_result vatek_device_stream_start(hvatek_chip hchip, Pmodulator_param pmod, uint32_t stream_mode);
	HAL_API vatek_result vatek_device_stream_write(hvatek_chip hchip,uint8_t* pbuf,int32_t size);
	HAL_API vatek_result vatek_device_stream_stop(hvatek_chip hchip);

	/* usb device bulk operations */
	HAL_API vatek_result vatek_device_usbbulk_send(hvatek_chip hchip, Pusbbulk_command pcmd, Pusbbulk_result presult,uint8_t* pbuf,int32_t len);
	HAL_API vatek_result vatek_device_usbbulk_get_result(hvatek_chip hchip, Pusbbulk_result presult);
	HAL_API vatek_result vatek_device_usbbulk_write(hvatek_chip hchip, uint8_t* pbuf, int32_t len);
	HAL_API vatek_result vatek_device_usbbulk_read(hvatek_chip hchip, uint8_t* pbuf, int32_t len);
	HAL_API vatek_result vatek_device_usbbulk_get_size(hvatek_chip hchip);

#ifdef __cplusplus
}
#endif

#endif
