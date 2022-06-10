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
//  VATek MCU bridge sample code.
//
//----------------------------------------------------------------------------

#include <vatek_sdk_bridge.h>

#include "../common/inc/tool_printf.h"

int main(int argc, char *argv[])
{
	hvatek_devices hdevlist = NULL;
	hvatek_chip hchip = NULL;
	hvatek_bridge hbridge = NULL;
	vatek_result nres = vatek_success;
	Pchip_info pinfo = NULL;
	Pbdevice_info pbinfo = NULL;
	/* 
		step 1 :
		- initialized supported device and open service
	*/

	nres = vatek_device_list_enum(DEVICE_BUS_BRIDGE, service_unknown, &hdevlist);
	if (is_vatek_success(nres))
	{
		if (nres == 0)
		{
			nres = vatek_nodevice;
			_disp_err("can not found device.");
		}
		else
		{
			nres = vatek_device_open(hdevlist, 0, &hchip);
			if (!is_vatek_success(nres))_disp_err("open device fail : %d", nres);
			else
			{
				pinfo = vatek_device_get_info(hchip);
				printf_chip_info(pinfo);
				nres = vatek_badstatus;
				if (pinfo->status != chip_status_waitcmd)_disp_err("device not ready : %d", pinfo->status);
				else nres = vatek_success;
			}
		}
	}
	else _disp_err("enum device fail : %d", nres);

	if (is_vatek_success(nres))
	{
		nres = vatek_bridge_open(hchip, &hbridge);
		if (!is_vatek_success(nres))_disp_err("open bridge device fail : %d", nres);
		else
		{
			pbinfo = vatek_bridge_get_info(hbridge);
			printf_bridge_info(pbinfo);
		}
	}

	if (is_vatek_success(nres))
	{
		int32_t i = 0;
		bridge_source bsource;
		for (i = 0; i < pbinfo->source_nums; i++)
		{
			nres = vatek_bridge_get_av_source(hbridge, i, &bsource);
			if (is_vatek_success(nres))
			{
				printf_bridge_avsource(vatek_bridge_get_av_source_name(hbridge, &bsource), &bsource);
				if (bsource.status == bstatus_active || bsource.status == bstatus_active_protect)
				{
					nres = vatek_bridge_start_av_source(hbridge, &bsource);
					if (is_vatek_success(nres))
					{
						int32_t is_stop = 0;
						_disp_l("bridge start output. press any key to stop");
						while (!is_stop)
						{
							nres = vatek_bridge_get_av_source_status(hbridge, &bsource);
							if (is_vatek_success(nres))
							{
								if (bsource.status == bstatus_active || bsource.status == bstatus_active_protect)
								{

								}
								else
								{
									_disp_l("bridge source status not valid : %d", bsource.status);
									is_stop = 1;
									nres = vatek_badstatus;
								}
							}
							if (!is_vatek_success(nres))break;
						}
					}
					break;
				}
			}
			else _disp_err("get av source fail : [%d:%d]",i,nres);
			if(!is_vatek_success(nres))break;
		}
	}

	if (hbridge)vatek_bridge_close(hbridge);
	if (hchip)vatek_device_close(hchip);
	if (hdevlist)vatek_device_list_free(hdevlist);
	printf_app_end();
	return (int32_t)1;
}
