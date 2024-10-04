//----------------------------------------------------------------------------
//
// Vision Advance Technology - Software Development Kit
// Copyright (c) 2014-2023, Vision Advance Technology Inc.
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

//----------------------------------------------------------------------------
//
//  VATek modulator sample code.
//
//----------------------------------------------------------------------------

#include <vatek_sdk_usbstream.h>
#include <vatek_sdk_device.h>
#include <cross/cross_os_api.h>
#include <service/service_transform.h>

#include <vatek_sdk_transform.h>

#include "../common/inc/tool_printf.h"
#include "../common/inc/tool_tspacket.h"
#include "../common/inc/tool_stream.h"
#include <core/tools/tool_dvb_t2.h>

#define TRANSFORM_RF_TEST			0		// Test RF output signal
#define TRANSFORM_NORMAL			1

#define TRANSFORM_PSI_TYPE        BYPASS_PSITABLE
	#define DEF_PSITABLE					1
	#define RAW_PSITABLE					2
	#define BYPASS_PSITABLE					3

static usbstream_param usbcmd =
{
	.mode = ustream_mode_sync,
	.remux = ustream_remux_pcr,
	.pcradjust = pcr_adjust,			 // PCR mode
	.r2param.freqkhz = 473000,			 // output _rf frequency
	.r2param.mode = r2_cntl_path_0,		 // J83b auto change IQ_swap
	.modulator =
	{
		6,
		modulator_dvb_t,
		ifmode_disable,0,0,
		.mod = {.dvb_t = {dvb_t_qam64, fft_8k, guard_interval_1_16, coderate_5_6},},
	},
	.sync = {NULL,NULL,NULL,
		.tick_adjust = {.up_lock_valus[0] = 100000, .up_lock_valus[1] = 150000, .up_lock_valus[2] = 200000, .up_lock_valus[3] = 250000,
						.lo_lock_valus[0] = -100000,.lo_lock_valus[1] = -150000,.lo_lock_valus[2] = -200000,.lo_lock_valus[3] = -250000,
						.times = 10,
						.up_adjust_value[0] = -30, .up_adjust_value[1] = -35, .up_adjust_value[2] = -40, .up_adjust_value[3] = -45,
						.lo_adjust_value[0] = 30, .lo_adjust_value[1] = 35, .lo_adjust_value[2] = 40, .lo_adjust_value[3] = 45,}
	},
	.pcr_pid = 0x111,					// RETAGv2 - create new pcr pid
	.latency = 500,						// RETAGv2 - pcr latency
};

extern vatek_result source_async_get_buffer(Ptsstream_source* param, hvatek_usbstream* husstream);
extern vatek_result source_sync_get_buffer(void* param, uint8_t** pslicebuf);
//extern vatek_result source_sync_get_encoder_buffer(void* param, uint32_t** buffer_size);
extern vatek_result parser_cmd_source(int32_t argc, char** argv,Ptsstream_source psource,Pusbstream_param pustream);

long int num_index = 0;
char* endptr;

int main(int argc, char *argv[])
{
	hvatek_chip hchip = NULL;
	hvatek_usbstream hustream = NULL;
	hmux_core hmux = NULL;
	hvatek_devices hdevlist = NULL;

	tsstream_source streamsource = { 0, };
	vatek_result nres = vatek_success;

	nres = parser_cmd_source(argc, argv, &streamsource,&usbcmd);

/*-----------------------------------------------------------------------------------------
	step 1 :
		- initialized supported device and open.
-----------------------------------------------------------------------------------------*/
	if (num_index != vatek_alldevice)
	{
		if (is_vatek_success(nres))
		{

			nres = vatek_device_list_enum(DEVICE_BUS_USB, service_transform, &hdevlist, num_index);

			if (is_vatek_success(nres)) {
				if (nres == 0)
				{
					nres = vatek_nodevice;
					_disp_err("can not found device.");
				}
				else if (nres == vatek_alldevice) {
					return vatek_alldevice;
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
			}
			else _disp_err("enum device fail : %d", nres);
		}


		/*-----------------------------------------------------------------------------------------
			step 2-1 :
				- Insert PSI table
					# used default psi table service
		-----------------------------------------------------------------------------------------*/
#if (TRANSFORM_PSI_TYPE == DEF_PSITABLE)

		//PSI table param
		hmux_program m_hprogram;
		hmux_channel m_hchannel = NULL;
		hmux_stream hstream = NULL;

		media_video pvideo = { 0 };
		media_audio paudio = { 0 };

		pvideo.aspectrate = aspectrate_16_9;
		pvideo.resolution = resolution_uhd;
		pvideo.vcodec = encvideo_hevc;
		pvideo.framerate = framerate_30;

		paudio.acodec = encaudio_mp2;
		paudio.channel = channel_stereo;
		paudio.samplerate = sample_rate_32;

		if (is_vatek_success(nres))
			nres = mux_handle_create(&hmux);
		if (is_vatek_success(nres))
			nres = mux_open_channel_default(hmux, mux_spec_dvb, dvb_taiwan, &m_hchannel);

		// Add PMT (PSI table)
		if (is_vatek_success(nres))
			nres = muxchannel_add_program(m_hchannel, 0x300, 0x111, &m_hprogram);
		// PMT (PSI table) add video & audio
		if (is_vatek_success(nres))
			nres = muxprogram_add_video(m_hprogram, 0x100, &pvideo, &hstream);
		if (is_vatek_success(nres))
			nres = muxprogram_add_audio(m_hprogram, 0x101, &paudio, &hstream);

		if (is_vatek_success(nres))
			nres = mux_handle_set_hal(hmux, hchip);
#endif

		/*-----------------------------------------------------------------------------------------
			step 2-2 :
				- Insert PSI table
					# used register psi table service (self-registration by users)
		-----------------------------------------------------------------------------------------*/
#if (TRANSFORM_PSI_TYPE == RAW_PSITABLE)

		// set PSITABLE
		uint8_t PAT[] =
		{
			0x47, 0x40, 0x00, 0x18,		/* PID : 0x0 */
			0x00, 0x00, 0xB0, 0x0D,
			0x00, 0x01,
			0xC3, 0x00, 0x00,
			0x00, 0x01,
			0xE3, 0x00,					/* PMT_PID: 0x0300 */
			0xD6, 0xA5, 0x10, 0x50,		/* CRC */
		};

		uint8_t PMT[] =
		{
			0x47, 0x43, 0x00, 0x18,		/* PID : 0x0300 */
			0x00, 0x02, 0xB0, 0x17,
			0x00, 0x01,
			0xC3, 0x00,	0x00,
			0xE1, 0x11,					/* PCR_PID: 0x100 */
			0xF0, 0x00,
			0x24, 0xE1, 0x01,			/* stream type : 0x24 (video H.265), PID : 0x1001 */
			0xF0, 0x00,
			0x03, 0xE1,	0x02,			/* stream type : 0x03 (audio mpeg1-l2), PID : 0x1002 */
			0xF0, 0x00,
			0x8E, 0x6E, 0xEE, 0xE8		/* CRC */
		};

		//PSI table param
		hmux_rawtable hraw = NULL;

		//PAT 188 Bytes
		uint8_t* pat_buf = (uint8_t*)malloc(PSI_TSPACKET_LEN);
		memset(pat_buf, 0xFF, PSI_TSPACKET_LEN);
		memcpy(&pat_buf[0], PAT, sizeof(PAT) / sizeof(uint8_t));

		//PMT 188 Bytes
		uint8_t* pmt_buf = (uint8_t*)malloc(PSI_TSPACKET_LEN);
		memset(pmt_buf, 0xFF, PSI_TSPACKET_LEN);
		memcpy(&pmt_buf[0], PMT, sizeof(PMT) / sizeof(uint8_t));

		if (is_vatek_success(nres))
			nres = mux_handle_create(&hmux);

		if (is_vatek_success(nres))
			nres = mux_open_raw(hmux, &hraw);
		if (is_vatek_success(nres))
		{
			Ppsi_table_raw prawtable_pat = NULL;
			Ppsi_table_raw prawtable_pmt = NULL;

			nres = muxraw_create_table(hraw, 1, 200, &prawtable_pat);
			nres = muxraw_create_table(hraw, 1, 200, &prawtable_pmt);

			if (is_vatek_success(nres))
			{
				// copy packet buffer to _prawtable->packet_buf
				prawtable_pat->packet_buf = pat_buf;
				prawtable_pmt->packet_buf = pmt_buf;
				nres = muxraw_push_table(hraw, prawtable_pat);
				nres = muxraw_push_table(hraw, prawtable_pmt);
			}
		}

		if (is_vatek_success(nres))
			nres = mux_handle_set_hal(hmux, hchip);
#endif

		/*-----------------------------------------------------------------------------------------
			step 3 :
				- open usb_stream open
				- config used usb_stream sync mode and start stream
		-----------------------------------------------------------------------------------------*/

#if TRANSFORM_NORMAL
		if (is_vatek_success(nres))
		{
			nres = vatek_usbstream_open(hchip, &hustream);

			/* Select the passing PID (Whitelist) */

			//if(is_vatek_success(nres))
			//	nres =  vatek_usbstream_filter(hustream);

			if (!is_vatek_success(nres))
				_disp_err("open usb_stream fail - %d", nres);
			else
			{
				// Use sync mode, async mode (circular buffer) please refer source code.
				usbcmd.mode = ustream_mode_sync;
				usbcmd.sync.param = &streamsource;
				usbcmd.sync.getbuffer = source_sync_get_buffer;
				//usbcmd.sync.get_encoder_buffer = source_sync_get_encoder_buffer;

				nres = vatek_usbstream_start(hustream, &usbcmd);

				if (!is_vatek_success(nres))
					_disp_err("start usb_stream fail : %d", nres);
				else
					printf_modulation_param(usbcmd.modulator, usbcmd.r2param);
			}
		}


		/*-----------------------------------------------------------------------------------------
			step 4 :
				- source_sync_get_buffer would call from internal when usb_stream had valid buffer.
				- main loop could polling usb_stream status and check information.
				- when finished close usb_stream.
		-----------------------------------------------------------------------------------------*/

		if (is_vatek_success(nres))
		{
			_disp_l("USB Stream Start.");
			_disp_l("Press any key to stop.\r\n");
			_disp_l("=====================================\r\n");
			int32_t is_stop = 0;
			Ptransform_info pinfo = NULL;
			uint32_t ntickms = cross_os_get_tick_ms();
			int	error = 0;
			uint32_t* buffer_size = 0;
			while (!is_stop)
			{
				usbstream_status status = vatek_usbstream_get_status(hustream, &pinfo);
				if (status == usbstream_status_running)
				{
					if (cross_os_get_tick_ms() - ntickms > 1000)
					{
						ntickms = cross_os_get_tick_ms();
						//source_sync_get_encoder_buffer(usbcmd.sync.param, &buffer_size, NULL);

						/* Adjust A3. */
						//if (usbcmd.sync.tick_adjust.times)
						//	usbstream_adjust_stream(hustream, buffer_size);

						_disp_l("Data：[%d]  Current：[%d]",
							pinfo->info.data_bitrate,
							pinfo->info.cur_bitrate);
						if (pinfo->info.data_bitrate == 0 | pinfo->info.cur_bitrate == 0) {
							error++;
						}
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
	}
	else {
		nres = vatek_device_list_enum(DEVICE_BUS_USB, service_transform, &hdevlist, vatek_alldevice);
		_disp_l("enum all device ");
	}


/*-----------------------------------------------------------------------------------------
	setp 5 :
		- before quit demo stop and free both device and source.
-----------------------------------------------------------------------------------------*/

	if (hchip) {
		vatek_device_close_reboot(hchip);
	}
	if (hdevlist)vatek_device_list_free(hdevlist);
	if (streamsource.hsource)
		streamsource.free(streamsource.hsource);
	printf_app_end();
	cross_os_sleep(10);
	return (int32_t)1;
#endif

// Test RF output signal
#if TRANSFORM_RF_TEST
	vatek_device_start_test(hchip, (Pmodulator_param)&usbcmd.modulator, 900000);
	vatek_device_start_sine(hchip, 900000);
#endif
}

vatek_result source_sync_get_buffer(void* param, uint8_t** pslicebuf)
{
	Ptsstream_source ptssource = (Ptsstream_source)param;
	static uint32_t ntickms = 0;
	ntickms = cross_os_get_tick_ms();
	vatek_result nres = ptssource->check(ptssource->hsource);
	if (nres > vatek_success)
	{
		*pslicebuf = ptssource->get(ptssource->hsource);
		nres = (vatek_result)1;
	}
	if (cross_os_get_tick_ms() - ntickms >= 1000) {
		printf("write data over %d ms\n", cross_os_get_tick_ms() - ntickms);
	}
	return nres;
}

//vatek_result source_sync_get_encoder_buffer(void* param, uint32_t** buffer_size)
//{
//	Ptsstream_source ptssource = (Ptsstream_source)param;
//	vatek_result nres = vatek_success;
//	*buffer_size = ptssource->get_size(ptssource->hsource);
//	return nres;
//}

vatek_result source_async_get_buffer(Ptsstream_source* param, hvatek_usbstream* husstream)
{
	Ptsstream_source ptssource = param;
	Pusbstream_slice m_slicebuf = NULL;
	vatek_result nres = ptssource->check(ptssource->hsource);
	if (nres > vatek_success)
	{
		size_t remain = CHIP_STREAM_SLICE_LEN;
		while (remain > 0) {
			char* data = ptssource->get(ptssource->hsource);

			if (!m_slicebuf) {
				nres = vatek_ustream_async_get_buffer(husstream, &m_slicebuf);
			}
			if (nres > vatek_success) {
				size_t pktnums = m_slicebuf->packet_len - m_slicebuf->packet_pos;
				if (pktnums > remain) {
					pktnums = remain;
				}
				size_t pktsize = pktnums * TS_PACKET_LEN;
				memcpy(m_slicebuf->ptrbuf, data, pktsize);
				m_slicebuf->ptrbuf += pktsize;
				data += pktsize;
				m_slicebuf->packet_pos += (int32_t)pktnums;
				if (m_slicebuf->packet_pos == m_slicebuf->packet_len) {
					nres = vatek_ustream_async_commit_buffer(husstream, m_slicebuf);
					m_slicebuf = NULL;
				}
				remain -= pktnums;
			}
			else if (nres == vatek_success) {
				cross_os_sleep(1);
			}
			if (!is_vatek_success(nres)) {
				break;
			}
		}
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
			else if (strcmp(argv[4], "smooth") == 0) pustream->remux = ustream_smooth;
			else pustream->remux = ustream_remux_pcr;
		}
		if (argc == 6) {
			num_index = strtol(argv[5], &endptr, 10);
			if (*endptr != '\0') {
				printf("Index fail.\n");
			}
		}

	}

	if (strcmp(argv[1], "--device") == 0) {
		num_index = vatek_alldevice;
		//nres = vatek_device_list_enum(DEVICE_BUS_USB, service_transform, &hdevlist, num_index);

	}
	
	if((nres == vatek_unsupport && num_index != vatek_alldevice) || strcmp(argv[1], "--help") == 0 || argc == 1 )
	{
		_disp_l("support command below : ");
		_disp_l("	- app_stream test： test stream mode in app_stream.c");
		_disp_l("	- app_stream [modulation] file [*.ts|*.trp] [remux|passthrough]");
		_disp_l("	- app_stream [modulation] udp  [ip address] [remux|passthrough]");
		_disp_l("	- app_stream [modulation] rtp  [ip address] [remux|passthrough]");
	}

	return nres;
}