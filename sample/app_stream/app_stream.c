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
//  VATek modulator sample code.
//
//----------------------------------------------------------------------------

#include <vatek_sdk_usbstream.h>
#include <vatek_sdk_device.h>
#include <cross/cross_os_api.h>

#include "../common/inc/tool_printf.h"
#include "../common/inc/tool_tspacket.h"
#include "../common/inc/tool_stream.h"
#include <core/tools/tool_dvb_t2.h>

#define TRANSFORM_RF_TEST		0
#define TRANSFORM_NORMAL		1

static const char* _app_logo[] = {
"	    _     ___          _                              _				\r\n",
"	   /_\\   / __| ___ _ _(_)___ ___  ___ __ _ _ __  _ __| |___		\r\n",
"	  / _ \\  \\__ \\/ -_) '_| / -_|_-< (_-</ _` | '  \\| '_ \\ / -_)   \r\n",
"	 /_/ \\_\\ |___/\\___|_| |_\\___/__/ /__/\\__,_|_|_|_| .__/_\\___|  \r\n",
"	                                                |_|					\r\n",
"	-----------------------------------------------------------------	\r\n",
	"\r\n",
"	Copyright (c) 2023, Vision Advance Technology Inc.（VATek）.	\r\n",
	"\r\n",
"	-----------------------------------------------------------------	\r\n",
	"\r\n",
	NULL,
};

static usbstream_param usbcmd =
{
	.mode = ustream_mode_sync,
	.remux = ustream_remux_pcr,
	.pcradjust = pcr_adjust,
	.r2param.freqkhz = 473000,							/* output _rf frequency */
	.r2param.mode = r2_cntl_path_0,
	.modulator =
	{
		6,
		modulator_dvb_t,
		ifmode_disable,0,0,
		.mod = {.dvb_t = {dvb_t_qam64, fft_8k, guard_interval_1_16, coderate_5_6},},
	},
	.sync = {NULL,NULL},
};

extern vatek_result source_sync_get_buffer(void* param, uint8_t** pslicebuf);
extern vatek_result parser_cmd_source(int32_t argc, char** argv,Ptsstream_source psource,Pusbstream_param pustream);

int main(int argc, char *argv[])
{
	hvatek_devices hdevlist = NULL;
	hvatek_chip hchip = NULL;
	hvatek_usbstream hustream = NULL;
	tsstream_source streamsource = { 0, };
	vatek_result nres = vatek_success;
	hmux_core hmux = NULL;
	hmux_channel m_hchannel = NULL;

	printf_logo(_app_logo);

#if 0
	/* 
		the easiest way to changed modulation mode used modulator_param_reset.
		example : 
			modulator_param_reset(modulator_atsc,&usbcmd.modulator);
			modulator_param_reset(modulator_j83b,&usbcmd.modulator);
			modulator_param_reset(modulator_dvb_t,&usbcmd.modulator);
			modulator_param_reset(modulator_isdb_t,&usbcmd.modulator);

	*/
	modulator_param_reset(modulator_isdb_t, &usbcmd.modulator);
	usbcmd.modulator.ifmode = ifmode_iqoffset;
	usbcmd.modulator.iffreq_offset = 143;
	nres = modulator_param_get_bitrate(&usbcmd.modulator);
#endif
	nres = parser_cmd_source(argc, argv, &streamsource,&usbcmd);
	/* 
		step 1 :
		- initialized supported device and open
	*/

	if (is_vatek_success(nres))
	{
		nres = vatek_device_list_enum(DEVICE_BUS_USB,service_transform,&hdevlist);
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
				if (!is_vatek_success(nres))
					_disp_err("open device fail : %d", nres);
				else
				{
					Pchip_info pinfo = vatek_device_get_info(hchip);
					printf_chip_info(pinfo);
					nres = streamsource.start(streamsource.hsource);
				}
			}
		}else _disp_err("enum device fail : %d",nres);
	}

	/*
		step 2:
			- open usb_stream open
			- config used usb_stream sync mode and start stream
	*/

#if TRANSFORM_RF_TEST
	vatek_device_start_test(hchip, (Pmodulator_param)&usbcmd.modulator, 900000);
	vatek_device_start_sine(hchip, 900000);
#endif

#if TRANSFORM_NORMAL
	if (is_vatek_success(nres))
	{
		nres = vatek_usbstream_open(hchip, &hustream);
		if (!is_vatek_success(nres))
			_disp_err("open usb_stream fail - %d", nres);
		else
		{
			usbcmd.mode = ustream_mode_sync;
			usbcmd.sync.param = &streamsource;
			usbcmd.sync.getbuffer = source_sync_get_buffer;

			nres = vatek_usbstream_start(hustream, &usbcmd);

			printf_modulation_param(usbcmd);

			if (!is_vatek_success(nres))
				_disp_err("start usb_stream fail : %d", nres);
		}
	}


	/* 
		step 3 : 
			- source_sync_get_buffer would call from internal when usb_stream had valid buffer
			- main loop could polling usb_stream status and check information
			- when finished close usb_stream 
	*/

	if (is_vatek_success(nres))
	{
		_disp_l("USB Stream Start.");
		_disp_l("Press any key to stop.\r\n");
		_disp_l("=====================================\r\n");
		int32_t is_stop = 0;
		Ptransform_info pinfo = NULL;
		uint32_t ntickms = cross_os_get_tick_ms();
		int count = 0;
		int	error = 0;
		while (!is_stop)
		{
			usbstream_status status = vatek_usbstream_get_status(hustream,&pinfo);
			if (status == usbstream_status_running)
			{
				if (cross_os_get_tick_ms() - ntickms > 1000)
				{
					ntickms = cross_os_get_tick_ms();
					_disp_l("Data:[%d]  Current:[%d]",
						pinfo->info.data_bitrate,
						pinfo->info.cur_bitrate);
					if (pinfo->info.data_bitrate == 0 | pinfo->info.cur_bitrate == 0) {
						error++;
					}
					count++;
					if (error >= 30) {
						_disp_l("A3 Fail. Press any key to stop.\r\n");
					}
				}
			}
	
			if (!is_vatek_success(nres))break;
			if (cross_os_get_ch_no_wait() != -1)is_stop = 1;
			else cross_os_sleep(1);
		}

		nres = vatek_usbstream_stop(hustream);
		if (!is_vatek_success(nres))_disp_err("stop usb_stream fail : %d", nres);
		vatek_usbstream_close(hustream);
	}

	/* 
		setp 4 : 
		before quit demo stop and free both device and source
	*/

	if (hchip) {
		//reboot chip
		vatek_device_close_reboot(hchip);
	}
	if (hdevlist)vatek_device_list_free(hdevlist);
	if (streamsource.hsource)
		streamsource.free(streamsource.hsource);
	printf_app_end();
	cross_os_sleep(10);
	return (int32_t)1;
#endif
}

vatek_result source_sync_get_buffer(void* param, uint8_t** pslicebuf)
{
	Ptsstream_source ptssource = (Ptsstream_source)param;
	vatek_result nres = ptssource->check(ptssource->hsource);
	if (nres > vatek_success)
	{
		*pslicebuf = ptssource->get(ptssource->hsource);
		nres = (vatek_result)1;
	}
	return nres;
}

vatek_result parser_cmd_source(int32_t argc, char** argv, Ptsstream_source psource, Pusbstream_param pustream)
{
	vatek_result nres = vatek_unsupport;


	if (argc >= 2) {
		if (strcmp(argv[1], "atsc") == 0) modulator_param_reset(modulator_atsc, &usbcmd.modulator);
		else if (strcmp(argv[1], "dvbt") == 0) modulator_param_reset(modulator_dvb_t, &usbcmd.modulator);
		else if (strcmp(argv[1], "isdbt") == 0) {
			modulator_param_reset(modulator_isdb_t, &usbcmd.modulator);
			usbcmd.modulator.ifmode = ifmode_iqoffset;
			usbcmd.modulator.iffreq_offset = 143;
		}
		else if (strcmp(argv[1], "j83a") == 0) modulator_param_reset(modulator_j83a, &usbcmd.modulator);
		else if (strcmp(argv[1], "j83b") == 0) modulator_param_reset(modulator_j83b, &usbcmd.modulator);
		else if (strcmp(argv[1], "j83c") == 0) modulator_param_reset(modulator_j83c, &usbcmd.modulator);
		else if (strcmp(argv[1], "dtmb") == 0) modulator_param_reset(modulator_dtmb, &usbcmd.modulator);
		else if (strcmp(argv[1], "dvbt2") == 0) {
			modulator_param_reset(modulator_dvb_t2, &usbcmd.modulator);
		}
		else if (strcmp(argv[1], "test") == 0) nres = stream_source_test_get(&usbcmd.modulator, psource);
		else nres = vatek_unsupport;

		if (argc >= 3) {
			if (strcmp(argv[2], "file") == 0)
				nres = stream_source_file_get(argv[3], psource);
			else if (strcmp(argv[2], "udp") == 0 || strcmp(argv[2], "rtp") == 0)
				nres = stream_source_udp_get(argv[3], psource);
			else nres = vatek_unsupport;
		}
		if (argc == 5) {
			if (strcmp(argv[4], "passthrough") == 0) pustream->remux = ustream_remux_passthrough;
			else pustream->remux = ustream_remux_pcr;
		}
	}
	
	
	if(nres == vatek_unsupport || strcmp(argv[1], "--help") == 0 || argc == 1)
	{
		_disp_l("support command below : ");
		_disp_l("	- app_stream test: test stream mode in app_stream.c");
		_disp_l("	- app_stream [modulation] file [*.ts|*.trp] [remux|passthrough]");
		_disp_l("	- app_stream [modulation] udp  [ip address] [remux|passthrough]");
		_disp_l("	- app_stream [modulation] rtp  [ip address] [remux|passthrough]");
	}

	return nres;
}

