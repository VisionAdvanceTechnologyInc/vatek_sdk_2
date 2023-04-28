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
//  VATek ENMODER sample code.
//
//----------------------------------------------------------------------------

#include <vatek_sdk_broadcast.h>
#include <vatek_sdk_device.h>
#include <vatek_sdk_bridge.h>
#include <cross/cross_os_api.h>
#include <mux/mux_handle.h>

#include "../common/inc/tool_printf.h"
#include "../common/inc/tool_tspacket.h"
#include "../common/inc/tool_stream.h"


#define BROADCAST_EN_AUX		0
#define BROADCAST_EN_BML		0
#define BROADCAST_EN_COLORBAR	1
#define BROADCAST_EN_BRIDGE		0

#define BROADCAST_RF_TEST		0
#define BROADCAST_NORMAL		1

/* broadcast parameters*/
static broadcast_param bc_param =
{
	.enc =
	{
#if BROADCAST_EN_COLORBAR
		.mode = encoder_source_colorbar,
#else
		.mode = encoder_source_vi_0,
#endif
		.pmt_pid = 0x1000,
		.recv = 0,
		.encoder_flags = 1,
		.encoder_tag = 0,
		.video = { encvideo_mpeg2,resolution_1080i,framerate_59_94,aspectrate_16_9 },
		.quality = { rc_vbr,5,10,16,500,19000000, },
		.viparam = { EP9555E_VI_FLAG,148500,0,0, },
#if BROADCAST_EN_COLORBAR
		.audio = { encaudio_aac_lc_adts,sample_rate_48,channel_mute, },
#else
		.audio = { encaudio_aac_lc_adts,sample_rate_48,channel_stereo, },
#endif
		.video_pid = 0x1002,
		.audio_pid = 0x1003,
	},
	.mod =
	{
		.bandwidth_symbolrate = 6,
		.type = modulator_isdb_t,
		.ifmode = ifmode_iqoffset,.iffreq_offset = 143,.dac_gain = 0,
		.mod = {.isdb_t = {isdb_t_qam64, fft_8k, guard_interval_1_32, coderate_2_3, isdb_t_interleaved_mode3},},
	},
	.mux =
	{
		.pcr_pid = 0x100,
		.padding_pid = 0x1FFF,
		.bitrate = 0,0,0,0,
	},
};

#if BROADCAST_EN_BML

typedef struct _bml_stream
{
	uint16_t pid;
	uint8_t tag;
	uint8_t recv;
	int32_t eslen;
	uint8_t esbuf[64];
}bml_stream, * Pbml_stream;

extern void bmlesinfo_create(Pbml_stream pbml);

static bml_stream bmltests[3] =
{
	{0x320,0x40,0,0,{0,}},
	{0x321,0x50,0,0,{0,}},
	{0x322,0x60,0,0,{0,}},
};

#endif

#if BROADCAST_EN_AUX

extern Pbroadcast_auxstream auxsource_test_open(const char* filename, usbaux_mode mode, uint32_t bitrate);
extern void auxsource_test_close(Pbroadcast_auxstream pauxtest);

#endif

int main(int argc, char* argv[])
{
	hvatek_devices hdevlist = NULL;
	hvatek_chip hchip = NULL;
	hvatek_broadcast hbc = NULL;
	vatek_result nres = vatek_badparam;
	Pchip_info pinfo = NULL;
	hmux_core hmux = NULL;
	Pbroadcast_auxstream pauxstream = NULL;

#if BROADCAST_EN_AUX

	uint32_t auxbitrate = 4000000;
	usbaux_mode auxmode = usbaux_sync;

	if (argc == 4)
	{
		int32_t cmdbitrate = atoi(argv[2]);
		if (cmdbitrate > 0)auxbitrate = cmdbitrate;
		if (strcmp(argv[1], "async") == 0)auxmode = usbaux_async;

		pauxstream = auxsource_test_open(argv[3], auxmode, auxbitrate);
		if (!pauxstream)_disp_err("open aux test file fail : %s", argv[3]);
		else
		{
			_disp_l("enable aux stream - [%d:%d:%s]", auxmode, auxbitrate, argv[3]);
			nres = vatek_success;
		}
	}
	else nres = vatek_success;

#else
	nres = vatek_success;
#endif

#if 1
	/*
		the easiest way to changed modulation mode used modulator_param_reset.
		example :
			modulator_param_reset(modulator_atsc,&usbcmd.modulator);
			modulator_param_reset(modulator_j83b,&usbcmd.modulator);
			modulator_param_reset(modulator_dvb_t,&usbcmd.modulator);
			modulator_param_reset(modulator_isdb_t,&usbcmd.modulator);

	*/
	//modulator_param_reset(modulator_isdb_t, &bc_param.mod);
	//bc_param.mod.ifmode = ifmode_iqoffset;
	//bc_param.mod.iffreq_offset = 143;
	//nres = modulator_param_get_bitrate(&bc_param.mod);
#endif
	/*
		step 1 :
		- initialized supported device and open service
	*/

	if (is_vatek_success(nres))
	{
		nres = vatek_device_list_enum(DEVICE_BUS_USB, service_broadcast, &hdevlist);
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
		if (is_vatek_success(nres))
		{
			nres = vatek_broadcast_open(hchip, &hbc);
			if (!is_vatek_success(nres))_disp_err("open broadcast service fail : %d", nres);
		}
	}

	/*
		step 2 :
			- check video and audio source ready or used bootlogo_ or colorbar_
	*/
#if BROADCAST_RF_TEST
	//vatek_device_start_sine(hchip, 473000);
	//vatek_device_start_test(hchip, (Pmodulator_param)&bc_param.mod, 900000);

	r2_param r2param;
	nres = rfmixer_r2_get_param(hchip, &r2param);
	if (is_vatek_success(nres)) {
		r2param.mode = r2_cntl_path_1;          // switch j83b qam inversion used r2_cntl_path_1
		r2param.freqkhz = 473000;
		nres = rfmixer_r2_set_param(hchip, &r2param);
		nres = rfmixer_r2_start(hchip, 0x600, &r2param);
	}



#endif



#if BROADCAST_NORMAL
#if BROADCAST_EN_BRIDGE

	if (is_vatek_success(nres))
	{
		hvatek_devices hblists = NULL;
		hvatek_chip hbchip = NULL;
		nres = vatek_device_list_enum(DEVICE_BUS_BRIDGE, service_broadcast, &hblists);
		if (is_vatek_success(nres))
		{
			if (nres > vatek_success)nres = vatek_device_open(hblists, 0, &hbchip);
			else if (nres == vatek_success)nres = vatek_nodevice;
		}

		_disp_l("connect to bridge device .... [%d]", nres);

		if (is_vatek_success(nres))
		{
			hvatek_bridge hbridge = NULL;
			nres = vatek_bridge_open(hbchip, &hbridge);
			if (is_vatek_success(nres))
			{
				Pbdevice_info pbinfo = vatek_bridge_get_info(hbridge);
				printf_bridge_info(pbinfo);
				if (pbinfo->source_nums)
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
								if (!is_vatek_success(nres))_disp_err("start bridge avsource fail : %d", nres);
								else
								{
									Pencoder_param penc = &bc_param.enc;
									penc->mode = encoder_source_vi_0;
									penc->video.resolution = bsource.video_info.resolution;
									penc->video.framerate = bsource.video_info.framerate;
									penc->video.aspectrate = bsource.video_info.aspectrate;
									penc->viparam.offset_x = bsource.video_info.offset_x;
									penc->viparam.offset_y = bsource.video_info.offset_y;
									penc->viparam.vi_flags = bsource.video_info.signal_flag;
									penc->viparam.vi_pixelclk = bsource.video_info.pixelclk;
									penc->audio.channel = bsource.audio_info.channel;
									penc->audio.samplerate = bsource.audio_info.samplerate;
								}
								break;
							}
						}
					}
				}
			}
			else _disp_err("open bridge fail : %d", nres);
		}

		if (hbchip)vatek_device_close(hbchip);
		if (hblists)vatek_device_list_free(hblists);
		nres = vatek_success;
	}
#endif

	if (is_vatek_success(nres))
	{
		Pencoder_param penc = &bc_param.enc;
		/*
			if found valid video and audio source config video, audio, vi_param and encoder parameters
			penc->mode = encoder_source_vi_0;
			penc->viparam .... (bt656/1120 interface Signal sync flag ...)
			penc->video .... (video encoder and input information)
			penc->audio .... (audio encoder and input information)
		*/
		if (pinfo->chip_module == ic_module_b3_lite)
			penc->video.vcodec = encvideo_h264;			/* b3 lite only support h264 codec */

		if (bc_param.mod.type == modulator_isdb_t)
			penc->audio.acodec = encaudio_aac_lc_adts;
		else if (bc_param.mod.type == modulator_atsc ||
			bc_param.mod.type == modulator_j83b)
			penc->audio.acodec = encaudio_ac3;
#if 0
		penc->mode = encoder_source_colorbar;
		penc->video.resolution = resolution_1080i;
		penc->video.framerate = framerate_60;
		penc->audio.channel = channel_mute;
#endif
	}

	/*
		step 3 :
			- enable extend stream
			- enable psi table service
	*/
	if (is_vatek_success(nres))
	{
		/* used default psi table service */
		nres = mux_handle_create(&hmux);
		if (is_vatek_success(nres))
		{
			Pencoder_param penc = &bc_param.enc;
			Pmux_param pmux = &bc_param.mux;
			hmux_broadcast hmuxbc = NULL;
			nres = mux_open_broadcast_default(hmux, pmux->pcr_pid, penc, mux_spec_arib, arib_japan, &hmuxbc);
			if (is_vatek_success(nres))
			{
				Pspec_channel pch = NULL;
				nres = muxbroadcast_get_channel(hmuxbc, &pch);
				if (is_vatek_success(nres))
				{
					Pmux_spec_dvb_channel pdvbch = &pch->_spec.dvb;
					/* config channel detail param */
					pdvbch->network_id = 0;
				}
			}
			if (is_vatek_success(nres))
			{
				Pspec_program pprog = NULL;
				nres = muxbroadcast_get_program(hmuxbc, &pprog);
				if (is_vatek_success(nres))
				{
					Pmux_spec_arib_program pdvbprog = &pprog->_spec.arib;
					/* config program detail param */
					pdvbprog->service_no = 0;
				}
			}

#if BROADCAST_EN_BML
			if (is_vatek_success(nres))
			{
				bmlesinfo_create(&bmltests[0]);
				bmlesinfo_create(&bmltests[1]);
				bmlesinfo_create(&bmltests[2]);

				nres = muxbroadcast_add_stream(hmuxbc, bmltests[0].pid, STREAMTYPE_ISO13818_6_TYPE_D, &bmltests[0].esbuf[0], bmltests[0].eslen);
				if (is_vatek_success(nres))
					nres = muxbroadcast_add_stream(hmuxbc, bmltests[1].pid, STREAMTYPE_ISO13818_6_TYPE_D, &bmltests[1].esbuf[0], bmltests[1].eslen);
				if (is_vatek_success(nres))
					nres = muxbroadcast_add_stream(hmuxbc, bmltests[2].pid, STREAMTYPE_ISO13818_6_TYPE_D, &bmltests[2].esbuf[0], bmltests[2].eslen);
			}
#endif
		}

		/*
			or could used customized psi table used raw series functions
			example below :

			hmux_rawtable hraw = NULL;
			nres = mux_open_raw(hmux, &hraw);
			if (is_vatek_success(nres))
			{
				Ppsi_table_raw prawtable = NULL;
				nres = muxraw_create_table(hraw, 1, 500, &prawtable);
				if (is_vatek_success(nres))
				{
					// copy packet buffer to _prawtable->packet_buf
					nres = muxraw_push_table(hraw, prawtable);
				}
			}
		*/

		if (is_vatek_success(nres))
		{
			nres = mux_handle_set_hal(hmux, hchip);
		}
	}

	/*
	step 4 :
		- enable psi table service
		- start broadcast
	*/

	r2_param r2param;
	nres = rfmixer_r2_get_param(hchip, &r2param);
	if (is_vatek_success(nres)) {
		r2param.mode = r2_cntl_path_0;          // switch j83b qam inversion used r2_cntl_path_1
		r2param.freqkhz = 473000;
	}

	if (is_vatek_success(nres))
	{
		nres = vatek_broadcast_start(hbc, &bc_param, pauxstream, r2param);
		if (!is_vatek_success(nres))_disp_err("start broadcast fail : %d", nres);
	}

	if (is_vatek_success(nres))
	{
		int32_t is_stop = 0;
		int* index = 0;
		uint8_t* pktbuf = NULL;
		Pbroadcast_info pbcinfo = NULL;
		uint32_t ntickms = cross_os_get_tick_ms();
		_disp_l("broadcast start. press any key to stop");

		while (!is_stop)
		{
			nres = vatek_broadcast_polling(hbc, &pbcinfo);

			if (is_vatek_success(nres))
			{
				if (pbcinfo->status == bcstatus_wait_source)
				{
					if (cross_os_get_tick_ms() - ntickms > 5000)
					{
						nres = vatek_timeout;
						_disp_err("broadcast - wait source timeout");
					}
				}
				else if (pbcinfo->status == bcstatus_broadcast)
				{

					if (cross_os_get_tick_ms() - ntickms > 1000)
					{
						ntickms = cross_os_get_tick_ms();
						_disp_l("broadcast - [%d:%d:%d]", pbcinfo->status, pbcinfo->data_bitrate, pbcinfo->cur_bitrate);
					}
				}
				else
				{
					_disp_l("broadcast status not valid : %d", pbcinfo->status);
					is_stop = 1;
					nres = vatek_badstatus;
				}
			}
			if (!is_vatek_success(nres))break;
			if (cross_os_get_ch_no_wait() != -1)is_stop = 1;
			else cross_os_sleep(1);
		}

		nres = vatek_broadcast_stop(hbc);
		if (!is_vatek_success(nres))_disp_err("stop broadcast fail : %d", nres);
	}
	
	/*
		setp 5 :
		before quit demo stop and free both device and source
	*/

	_disp_l("broadcast finiah : %d", nres);

#if BROADCAST_EN_AUX
	if (pauxstream)auxsource_test_close(pauxstream);
#endif
	if (hchip) {
		vatek_device_close_reboot(hchip);
	}
	if (hmux)mux_handle_free(hmux);
	if (hbc)vatek_broadcast_close(hbc);
	if (hchip) vatek_device_close(hchip);
	if (hdevlist)vatek_device_list_free(hdevlist);
#endif
	printf_app_end();
	return (int32_t)1;
}

#if BROADCAST_EN_AUX

#define ASYNC_TEST_UPDATE	30000 /*  10 second */
#define AUXPID_MAX_NUMS		10

typedef struct _aux_pid
{
	uint16_t pid;
	uint16_t ncontinue;
}aux_pid, * Paux_pid;

typedef struct _aux_test_handle
{
	FILE* file;
#if ASYNC_TEST_UPDATE
	uint32_t tick;
#endif
	int32_t pktnums;
	int32_t pktvalid;
	uint8_t* buffer[CHIP_STREAM_SLICE_LEN];
	int32_t pidpos;
	aux_pid pids[AUXPID_MAX_NUMS];
}aux_test_handle, * Paux_test_handle;

static uint8_t null_packet[TS_PACKET_LEN] =
{
	0x47,0x1F,0xFF,0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
};

extern void auxsource_pid_continue(Paux_test_handle ptest, uint8_t* ppacket);
extern int32_t auxsource_test_check_update(hvatek_aux haux);
extern vatek_result auxsource_test_get_packets(hvatek_aux haux, uint8_t* pbuf, int32_t len);

static broadcast_auxstream bcaux_stream = { NULL,{usbaux_null,0,0,},auxsource_test_check_update,auxsource_test_get_packets, };

Pbroadcast_auxstream auxsource_test_open(const char* filename, usbaux_mode mode, uint32_t bitrate)
{
	FILE* hfile = fopen(filename, "rb");
	if (hfile)
	{
		Paux_test_handle newaux = (Paux_test_handle)malloc(sizeof(aux_test_handle));
		if (newaux)
		{
			memset(newaux, 0, sizeof(aux_test_handle));
			fseek(hfile, 0, SEEK_END);
			bcaux_stream.aux.bitrate = bitrate;
			bcaux_stream.aux.mode = mode;
			bcaux_stream.aux.length = (int32_t)ftell(hfile);
			newaux->file = hfile;
			if ((bcaux_stream.aux.length % TS_PACKET_LEN) == 0)
			{
				newaux->pktnums = bcaux_stream.aux.length / TS_PACKET_LEN;
				newaux->pktvalid = newaux->pktnums;
				bcaux_stream.haux = newaux;
#if ASYNC_TEST_UPDATE
				newaux->tick = cross_os_get_tick_ms();
#endif
				fseek(hfile, 0, SEEK_SET);
				return &bcaux_stream;
			}
		}
		fclose(hfile);
	}
	return NULL;
}

void auxsource_test_close(Pbroadcast_auxstream pauxtest)
{
	Paux_test_handle paux = (Paux_test_handle)pauxtest->haux;
	fclose(paux->file);
	free(paux);
	memset(&bcaux_stream.aux, 0, sizeof(usbparam_aux));
	bcaux_stream.haux = NULL;
}

int32_t auxsource_test_check_update(hvatek_aux haux)
{
#if ASYNC_TEST_UPDATE
	Paux_test_handle paux = (Paux_test_handle)haux;
	if (bcaux_stream.aux.mode == usbaux_async)
	{
		if ((cross_os_get_tick_ms() - paux->tick) > ASYNC_TEST_UPDATE)
		{
			paux->tick = cross_os_get_tick_ms();
			return 1;
		}
	}
#endif
	return 0;
}

vatek_result auxsource_test_get_packets(hvatek_aux haux, uint8_t* ppktbuf, int32_t pktnums)
{
	Paux_test_handle paux = (Paux_test_handle)haux;
	vatek_result nres = vatek_success;
	uint8_t* ptrbuf = &ppktbuf[0];
	while (pktnums)
	{
		if (paux->pktvalid)
		{
			
			nres = (vatek_result)fread(ptrbuf, TS_PACKET_LEN, 1, paux->file);
			if (nres == 1)
			{
				if (bcaux_stream.aux.mode == usbaux_sync)
					auxsource_pid_continue(paux, ptrbuf);
				paux->pktvalid--;
				if (!paux->pktvalid)
				{
					fseek(paux->file, 0, SEEK_SET);
					paux->pktvalid = paux->pktnums;
				}
			}
			else nres = vatek_hwfail;
		}
		else memcpy(ptrbuf, &null_packet[0], TS_PACKET_LEN);
		if (!is_vatek_success(nres))break;
		ptrbuf += TS_PACKET_LEN;
		pktnums--;
	}
	return nres;
}

void auxsource_pid_continue(Paux_test_handle ptest, uint8_t* ppacket)
{
	uint32_t pid = ((ppacket[1] & 0x1F) << 8) | ppacket[2];
	if (pid != 0x1FFF)
	{
		int32_t i;
		for (i = 0; i < ptest->pidpos; i++)
		{
			if (ptest->pids[i].pid == pid)
			{
				ppacket[3] &= 0xF0;
				ppacket[3] |= (ptest->pids[i].ncontinue++ & 0xF);
				return;
			}
		}
		ptest->pids[ptest->pidpos].pid = pid;
		ptest->pids[ptest->pidpos].ncontinue = ppacket[3] & 0x0F;
		ptest->pidpos++;
	}
}

#endif

#if BROADCAST_EN_BML

static const uint8_t bml_addinfo_40[] = { 0x33,0x3F,0x00,0x03,0x00,0x00,0xFF,0xBF, };
static const uint8_t bml_addinfo_60[] = { 0x1F,0xFF,0xBF, };

void bmlesinfo_create(Pbml_stream pstream)
{
	uint8_t* ptr = &pstream->esbuf[0];
	memset(ptr, 0, 64);

	ptr[0] = 0x52;
	ptr[1] = 1;
	ptr[2] = pstream->tag;
	ptr[3] = 0xFD;
	ptr[5] = 0x00;
	ptr[6] = 0x0C;

	if (pstream->tag == 0x40)
	{
		ptr[4] = 2 + sizeof(bml_addinfo_40);
		memcpy(&ptr[7], &bml_addinfo_40[0], sizeof(bml_addinfo_40));
		pstream->eslen = 7 + sizeof(bml_addinfo_40);
	}
	else
	{
		ptr[4] = 2 + sizeof(bml_addinfo_60);
		memcpy(&ptr[7], &bml_addinfo_60[0], sizeof(bml_addinfo_60));
		pstream->eslen = 7 + sizeof(bml_addinfo_60);
	}
}

#endif
