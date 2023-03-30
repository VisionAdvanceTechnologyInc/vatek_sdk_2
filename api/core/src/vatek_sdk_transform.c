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


#include <vatek_sdk_transform.h>
#include <core/base/output_rfmixer.h>
#include <service/ui/ui_service_broadcast.h>
#include <service/ui/ui_service_transform.h>

typedef struct _handle_transform
{
	hvatek_chip hchip;
	int32_t is_rfmixer;
	stream_mode streammode;
	transform_info info;
	uint32_t tick;
	transform_mode mux_mode;
	hmux_core hmux;
}handle_transform,*Phandle_transform;

extern vatek_result transform_check_status(hvatek_chip hchip,chip_status status);

vatek_result vatek_transform_open(hvatek_chip hchip, hvatek_transform* htr)
{
	vatek_result nres = vatek_unsupport;
	Pchip_info pinfo = vatek_device_get_info(hchip);
	if(pinfo->hal_service == service_transform)
	{
		Phandle_transform newtr = (Phandle_transform)malloc(sizeof(handle_transform));
		nres = vatek_memfail;
		if(newtr)
		{
			chip_status cstatus = chip_status_get(hchip, NULL);
			memset(newtr,0,sizeof(handle_transform));

			nres = mux_handle_create(&newtr->hmux);
			if (is_vatek_success(nres))
			{
				newtr->mux_mode = trmode_null;
				newtr->hchip = hchip;
				nres = rfmixer_check_support(hchip);
				if (is_vatek_success(nres))newtr->is_rfmixer = 1;
				else nres = vatek_success;
			}

			if (is_vatek_success(nres))
			{
				if(cstatus == chip_status_waitcmd)newtr->info.mode = trmode_null;
				else
				{
					if (is_chip_status_valid(cstatus))
					{
						uint32_t val;
						nres = vatek_chip_read_memory(hchip, HALREG_TRANSFORM_MODE, &val);
						if (is_vatek_success(nres))
						{
							newtr->info.mode = (transform_mode)val;
							nres = broadcast_info_get(hchip, &newtr->info.info);
						}
					}
					if (is_vatek_success(nres))nres = vatek_transform_stop((hvatek_transform)newtr);
				}
			}

			if (!is_vatek_success(nres))
			{
				if (newtr->hmux)mux_handle_free(newtr->hmux);
				free(newtr);
			}
			else *htr = newtr;
		}
	}
	return nres;
}

vatek_result vatek_transform_start_enum(hvatek_transform htr, Ptransform_enum penum)
{
	Phandle_transform phtr = (Phandle_transform)htr;
	vatek_result nres = vatek_badstatus;
	if (phtr->info.mode == trmode_null)
	{
		phtr->mux_mode = trmode_null;
		mux_handle_reset(phtr->hmux);

		nres = transform_enum_set(phtr->hchip, penum);
		if (is_vatek_success(nres))
			nres = chip_send_command(phtr->hchip, BC_START, HALREG_TRANSFORM_CNTL, HALREG_SYS_ERRCODE);
		if(is_vatek_success(nres))
			nres = broadcast_info_get(phtr->hchip, &phtr->info.info);
		if (is_vatek_success(nres))
		{
			phtr->tick = vatek_get_tick_ms();
			phtr->info.mode = trmode_enum;
		}
	}
	return nres;
}

vatek_result vatek_transform_start_capture(hvatek_transform htr, Ptransform_capture pcapture)
{
	Phandle_transform phtr = (Phandle_transform)htr;
	vatek_result nres = vatek_badstatus;
	if (phtr->info.mode == trmode_null)
	{
		phtr->mux_mode = trmode_null;
		mux_handle_reset(phtr->hmux);

		nres = transform_capture_set(phtr->hchip, pcapture);
		if (is_vatek_success(nres))
			nres = chip_send_command(phtr->hchip, BC_START, HALREG_TRANSFORM_CNTL, HALREG_SYS_ERRCODE);
		if(is_vatek_success(nres))
			nres = broadcast_info_get(phtr->hchip, &phtr->info.info);
		if (is_vatek_success(nres))
		{
			phtr->tick = vatek_get_tick_ms();
			phtr->info.mode = trmode_capture;
		}
	}
	return nres;
}

vatek_result vatek_transform_start_broadcast(hvatek_transform htr, Ptransform_broadcast pbc, r2_param r2param)
{
	Phandle_transform phtr = (Phandle_transform)htr;
	vatek_result nres = vatek_badstatus;
	if (phtr->info.mode == trmode_null)
	{
		phtr->mux_mode = trmode_null;
		mux_handle_reset(phtr->hmux);

		nres = transform_broadcast_set(phtr->hchip, pbc);

		if (is_vatek_success(nres))
		{
			if (phtr->is_rfmixer && r2param.freqkhz) {
				nres = rfmixer_r2_start(phtr->hchip, HALREG_TRANSFORM_CNTL, &r2param);
			}
		}

		if (is_vatek_success(nres))
			nres = chip_send_command(phtr->hchip, BC_START, HALREG_TRANSFORM_CNTL, HALREG_SYS_ERRCODE);
		if (is_vatek_success(nres))
		{
			nres = broadcast_info_get(phtr->hchip, &phtr->info.info);
			if (is_vatek_success(nres))
			{
				phtr->tick = vatek_get_tick_ms();
				phtr->info.mode = trmode_broadcast;
			}
		}
	}
	return nres;
}

vatek_result vatek_transform_polling(hvatek_transform htr, Ptransform_info* pinfo)
{
	Phandle_transform phtr = (Phandle_transform)htr;
	uint32_t val = 0;

	vatek_result nres = vatek_badstatus;
	if (phtr->info.mode != trmode_null)
	{

		nres = transform_check_status(phtr->hchip, chip_status_running);
		if (!is_vatek_success(nres))nres = broadcast_info_get(phtr->hchip, &phtr->info.info);
		else
		{
			int32_t eclipse = vatek_get_tick_ms() - phtr->tick;
			if (eclipse > 1000)
			{
				nres = broadcast_info_get(phtr->hchip, &phtr->info.info);
				phtr->tick = vatek_get_tick_ms();
			}
		}
	}
	if (is_vatek_success(nres))
	{
		if(pinfo)*pinfo = &phtr->info;
	}
	return nres;
}

Ptransform_info vatek_transform_get_info(hvatek_transform htr)
{
	Phandle_transform phtr = (Phandle_transform)htr;
	return &phtr->info;
}

vatek_result vatek_transform_stop(hvatek_transform htr)
{
	Phandle_transform phtr = (Phandle_transform)htr;
	vatek_result nres = vatek_badstatus;
	if (phtr->info.mode != trmode_null)
	{
		nres = vatek_success;
		if (phtr->info.mode == trmode_enum)
		{
			Pbroadcast_info pinfo = &phtr->info.info;
			nres = broadcast_info_get(phtr->hchip, pinfo);
			if (is_vatek_success(nres))
			{
				if(pinfo->status == bcstatus_finish)
					nres = mux_handle_get_hal_channel(phtr->hmux, phtr->hchip);
			}
				
			if(is_vatek_success(nres))phtr->mux_mode = trmode_enum;
			else VWAR("load enum context fail : %d", nres);
		}

		if (is_vatek_success(nres))
		{
			nres = chip_send_command(phtr->hchip, BC_STOP, HALREG_TRANSFORM_CNTL, HALREG_SYS_ERRCODE);
			if (is_vatek_success(nres))
			{
				if (phtr->info.mode == trmode_broadcast)
				{
					if (phtr->is_rfmixer)
						nres = rfmixer_stop(phtr->hchip, HALREG_TRANSFORM_CNTL);
				}
			}
			phtr->info.mode = trmode_null;
		}
	}
	return nres;
}

vatek_result vatek_transform_enum_get_context(hvatek_transform htr, hmux_channel* hchannel)
{
	vatek_result nres = vatek_badstatus;
	Phandle_transform phtr = (Phandle_transform)htr;
	if (phtr->mux_mode == trmode_enum)
		nres = mux_handle_get_channel(phtr->hmux, hchannel);
	return nres;
}

vatek_result vatek_transform_get_packets(hvatek_transform htr,uint32_t* pktnums)
{
	Phandle_transform phtr = (Phandle_transform)htr;
	vatek_result nres = vatek_badstatus;
	if (phtr->info.mode != trmode_null)
	{
		uint32_t val = 0;
		nres = vatek_chip_read_memory(phtr->hchip,HALREG_TRINFO_PACKETNUMS, &val);
		if (is_vatek_success(nres))*pktnums = val;
	}
	return nres;
}

vatek_result vatek_transform_commit_packets(hvatek_transform htr)
{
	Phandle_transform phtr = (Phandle_transform)htr;
	vatek_result nres = vatek_badstatus;
	if (phtr->info.mode != trmode_null)
	{
		nres = vatek_chip_write_memory(phtr->hchip, HALREG_TRINFO_PACKETNUMS, 0);
	}
	return nres;
}

void vatek_transform_close(hvatek_transform htr)
{
	Phandle_transform phtr = (Phandle_transform)htr;
	if(phtr->info.mode != trmode_null)
		vatek_transform_stop(htr);
	if (phtr->hmux)
		mux_handle_free(phtr->hmux);
	free(phtr);
}

vatek_result transform_check_status(hvatek_chip hchip,chip_status status)
{
	Pchip_info pinfo = vatek_device_get_info(hchip);
	vatek_result nres = vatek_success;
	if(!is_chip_status_valid(pinfo->status))nres = vatek_hwfail;
	else if(pinfo->status != status)nres = vatek_badstatus;
	return nres;	
}
