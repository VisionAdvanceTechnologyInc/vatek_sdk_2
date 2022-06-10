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
//----------------------------------------------------------------------------
//
//  MCU bridge function.
//
//----------------------------------------------------------------------------

#include <vatek_sdk_bridge.h>
#include <cross/cross_bridge.h>
#include "device/internal/cross_device_tool.h"

typedef struct _bridge_handle
{
	hvatek_chip hchip;
	Pcross_device hcross;
	hbridge_device hbridge;
	bdevice_info info;
}bridge_handle,*Pbridge_handle;

vatek_result vatek_bridge_open(hvatek_chip hchip, hvatek_bridge* hbridge)
{
	Pcross_device pcross = ((Pvatek_device)hchip)->cross;
	hbridge_device hdevice = cross_get_bridge_handle(pcross);
	vatek_result nres = vatek_unsupport;
	if (hdevice)
	{
		Pbridge_handle newbridge = (Pbridge_handle)malloc(sizeof(bridge_handle));
		nres = vatek_memfail;
		if (newbridge)
		{
			memset(newbridge, 0, sizeof(bridge_handle));
			newbridge->hchip = hchip;
			newbridge->hcross = pcross;
			newbridge->hbridge = hdevice;
			nres = bridge_device_lock(hdevice);
			if (is_vatek_success(nres))
				nres = bridge_device_get_info(hdevice, &newbridge->info);
			if (is_vatek_success(nres))*hbridge = newbridge;
			else free(newbridge);
		}
	}
	return nres;
}

void vatek_bridge_close(hvatek_bridge hbridge)
{
	Pbridge_handle pbridge = (Pbridge_handle)hbridge;
	bridge_device_unlock(pbridge->hbridge);
	free(pbridge);
}

Pbdevice_info vatek_bridge_get_info(hvatek_bridge hbridge)
{
	Pbridge_handle pbridge = (Pbridge_handle)hbridge;
	return &pbridge->info;
}

vatek_result vatek_bridge_get_av_source(hvatek_bridge hbridge, int32_t idx, Pbridge_source psource)
{
	Pbridge_handle pbridge = (Pbridge_handle)hbridge;
	vatek_result nres = vatek_badparam;
	if (idx < (int32_t)pbridge->info.source_nums)
		nres = bridge_device_get_source(pbridge->hbridge, idx, psource);
	return nres;
}

const char* vatek_bridge_get_av_source_name(hvatek_bridge hbridge, Pbridge_source psource)
{
	Pbridge_handle pbridge = (Pbridge_handle)hbridge;
	return bridge_device_get_source_name(pbridge->hbridge, psource);
}

vatek_result vatek_bridge_start_av_source(hvatek_bridge hbridge, Pbridge_source psource)
{
	Pbridge_handle pbridge = (Pbridge_handle)hbridge;
	return bridge_device_start_source(pbridge->hbridge, psource);
}

vatek_result vatek_bridge_get_av_source_status(hvatek_bridge hbridge, Pbridge_source psource)
{
	Pbridge_handle pbridge = (Pbridge_handle)hbridge;
	return bridge_device_get_source_status(pbridge->hbridge, psource);
}

vatek_result vatek_bridge_stop_av_source(hvatek_bridge hbridge)
{
	Pbridge_handle pbridge = (Pbridge_handle)hbridge;
	return bridge_device_stop_source(pbridge->hbridge);
}

vatek_result vatek_bridge_write_section(hvatek_bridge hbridge, int32_t section, uint8_t* pbuffer)
{
	Pbridge_handle pbridge = (Pbridge_handle)hbridge;
	vatek_result nres = bridge_device_storage_write(pbridge->hbridge, section, pbuffer);
	return nres;
}

vatek_result vatek_bridge_read_section(hvatek_bridge hbridge, int32_t section, uint8_t* pbuffer)
{
	Pbridge_handle pbridge = (Pbridge_handle)hbridge;
	vatek_result nres = bridge_device_storage_read(pbridge->hbridge, section, pbuffer);
	return nres;
}