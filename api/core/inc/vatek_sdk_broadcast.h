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

#ifndef _VATEK_SDK_BROADCAST_
#define _VATEK_SDK_BROADCAST_

#include <service/service_broadcast.h>

typedef void* hvatek_broadcast;
typedef void* hvatek_aux;

typedef int32_t(*fpaux_check_update)(hvatek_aux haux);
typedef vatek_result(*fpaux_get_packets)(hvatek_aux haux, uint8_t* ppktbuf, int32_t pktnums);

typedef struct _broadcast_auxstream
{
    hvatek_aux haux;
    usbparam_aux aux;
    fpaux_check_update check_update;
    fpaux_get_packets get_packets;
}broadcast_auxstream;

typedef broadcast_auxstream* Pbroadcast_auxstream;

#ifdef __cplusplus
extern "C" {
#endif

    HAL_API vatek_result vatek_broadcast_check_auxstream(hvatek_chip hchip);
    HAL_API vatek_result vatek_broadcast_open(hvatek_chip hchip,hvatek_broadcast* hbc);

    HAL_API Pbroadcast_info vatek_broadcast_get_info(hvatek_broadcast hbc);

    HAL_API vatek_result vatek_broadcast_start(hvatek_broadcast hbc, Pbroadcast_param pbcparam, Pbroadcast_auxstream paux, r2_param r2param);
    HAL_API vatek_result vatek_broadcast_polling(hvatek_broadcast hbc, Pbroadcast_info* pinfo);
    HAL_API vatek_result vatek_broadcast_stop(hvatek_broadcast hbc);

    HAL_API void vatek_broadcast_close(hvatek_broadcast hbc);

#ifdef __cplusplus
}
#endif

#endif
