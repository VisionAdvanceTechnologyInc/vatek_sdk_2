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


#include <vatek_sdk_broadcast.h>
#include <vatek_sdk_device.h>

#include <cross/cross_os_api.h>
#include <service/ui/ui_service_broadcast.h>

typedef struct _handle_broadcast
{
	hvatek_chip hchip;
	int32_t is_rfmixer;
	int32_t is_rfstart;
	int32_t is_aux;
	int32_t is_auxupdate;
	uint32_t tick;
	broadcast_info info;
	usbbulk_command usbcmd;
	usbbulk_result usbresult;
	Pbroadcast_auxstream paux;
	uint32_t auxpackets;
	uint8_t usbbuffer[CHIP_STREAM_SLICE_LEN];
}handle_broadcast,*Phandle_broadcast;

extern vatek_result broadcast_usb_start_aux(Phandle_broadcast pbc, Pbroadcast_auxstream paux);

vatek_result vatek_broadcast_check_auxstream(hvatek_chip hchip)
{
	Pchip_info pinfo = vatek_device_get_info(hchip);
	vatek_result nres = vatek_unsupport;
	if (vatek_device_get_bus(hchip) == DEVICE_BUS_USB)
	{
		if (pinfo->peripheral_en & PERIPHERAL_AUXSTREAM_USB)
			nres = vatek_success;
	}
	return nres;
}

vatek_result vatek_broadcast_open(hvatek_chip hchip,hvatek_broadcast* hbc)
{
	Pchip_info pinfo = vatek_device_get_info(hchip);
	vatek_result nres = vatek_success;
	if(pinfo->hal_service != service_broadcast)nres = vatek_unsupport;
	else
	{
		Phandle_broadcast newbc = (Phandle_broadcast)malloc(sizeof(handle_broadcast));
		nres = vatek_memfail;
		if(newbc)
		{
			memset(newbc,0,sizeof(handle_broadcast));
			nres = rfmixer_check_support(hchip);
			if(is_vatek_success(nres))newbc->is_rfmixer = 1;
			else nres = vatek_success;
			nres = vatek_broadcast_check_auxstream(hchip);
			if (is_vatek_success(nres))newbc->is_aux = 1;
			else nres = vatek_success;
			newbc->hchip = hchip;
			*hbc = newbc;
		}
	}
	return nres;
}

void vatek_broadcast_close(hvatek_broadcast hbc)
{
	Phandle_broadcast pbc = (Phandle_broadcast)hbc;
	if(is_vatek_success(chip_status_check(pbc->hchip,chip_status_running)))
		vatek_broadcast_stop(hbc);
	free(pbc);
}

vatek_result vatek_broadcast_start(hvatek_broadcast hbc, Pbroadcast_param pbcparam, Pbroadcast_auxstream paux, r2_param r2param)
{
	Phandle_broadcast pbc = (Phandle_broadcast)hbc;
	vatek_result nres = chip_status_check(pbc->hchip,chip_status_waitcmd);
	if(is_vatek_success(nres))
	{
		if (paux && pbc->is_aux)
		{
			nres = broadcast_usb_start_aux(pbc, paux);
			if (is_vatek_success(nres))pbc->paux = paux;
		}

		if (is_vatek_success(nres))
		{
			pbc->is_rfstart = pbc->is_rfmixer && r2param.freqkhz;
			if (pbc->is_rfstart)nres = rfmixer_r2_start(pbc->hchip, HALREG_BROADCAST_CNTL, &r2param);
			else nres = vatek_success;
		}

		if (is_vatek_success(nres))
		{
			if (pbcparam->enc.encoder_flags && ENC_EN_DISABLE_DEINTERLACED)
				pbcparam->enc.viparam.vi_flags = pbcparam->enc.viparam.vi_flags | VI_FIELD_INVERSE;
			nres = broadcast_param_set(pbc->hchip, pbcparam);
			if (is_vatek_success(nres))
				nres = chip_send_command(pbc->hchip, BC_START, HALREG_BROADCAST_CNTL, HALREG_SYS_ERRCODE);
		}
		
		if (is_vatek_success(nres))
		{	
			nres = broadcast_info_get(pbc->hchip, &pbc->info);
			if (pbc->paux)
				nres = vatek_chip_read_memory(pbc->hchip, HALREG_AUXDATA_PACKETNUMS, &pbc->auxpackets);

			if (is_vatek_success(nres))pbc->tick = cross_os_get_tick_ms();
			else vatek_broadcast_stop(hbc);
		}
	}
	return nres;
}

Pbroadcast_info vatek_broadcast_get_info(hvatek_broadcast hbc)
{
	Phandle_broadcast pbc = (Phandle_broadcast)hbc;
	return &pbc->info;
}

vatek_result vatek_broadcast_stop(hvatek_broadcast hbc)
{
	Phandle_broadcast pbc = (Phandle_broadcast)hbc;
	vatek_result nres = chip_status_check(pbc->hchip,chip_status_running);
	if (is_vatek_success(nres) && pbc->is_rfmixer)
		nres = rfmixer_stop(pbc->hchip, HALREG_BROADCAST_CNTL);
	if (is_vatek_success(nres))
		nres = chip_send_command(pbc->hchip, BC_STOP, HALREG_BROADCAST_CNTL, HALREG_SYS_ERRCODE);
	return nres;
}

vatek_result vatek_broadcast_polling(hvatek_broadcast hbc, Pbroadcast_info* pinfo)
{
	Phandle_broadcast pbc = (Phandle_broadcast)hbc;
	vatek_result nres = chip_status_check(pbc->hchip,chip_status_running);
	if(is_vatek_success(nres))
	{
		nres = broadcast_status_get(pbc->hchip, &pbc->info.status);
		if (is_vatek_success(nres))
		{
			if (is_bcstatus_fail(pbc->info.status))nres = vatek_badstatus;
			else
			{
				if (pbc->paux)
				{
					Pbroadcast_auxstream paux = pbc->paux;
					Pusbparam_aux pusbaux = &paux->aux;
					if (pusbaux->mode == usbaux_sync)
					{
						if (pbc->auxpackets)
						{
							nres = paux->get_packets(paux->haux, &pbc->usbbuffer[0], CHIP_STREAM_SLICE_PACKET_NUMS);
							if (is_vatek_success(nres))
								nres = vatek_device_usbbulk_write(pbc->hchip, &pbc->usbbuffer[0], CHIP_STREAM_SLICE_LEN);
							if (is_vatek_success(nres))
							{
								cross_os_sleep(10);
								pbc->auxpackets -= CHIP_STREAM_SLICE_PACKET_NUMS;
								if (!pbc->auxpackets)nres = vatek_chip_write_memory(pbc->hchip, HALREG_AUXDATA_PACKETNUMS, 0);
							}
							else VERR("write bulk fail : %d", nres);
						}
						else
						{
							nres = vatek_chip_read_memory(pbc->hchip, HALREG_AUXDATA_PACKETNUMS, &pbc->auxpackets);
							if (is_vatek_success(nres) && !pbc->auxpackets)cross_os_sleep(1);
						}
					}
					else if (pusbaux->mode == usbaux_async)
					{
						if (pbc->is_auxupdate)
						{
							uint32_t val = 0;
							nres = vatek_chip_read_memory(pbc->hchip, HALREG_AUXDATA_CNTL, &val);
							if (is_vatek_success(nres) && !val)
							{
								nres = broadcast_usb_start_aux(pbc, paux);
								if (is_vatek_success(nres))pbc->is_auxupdate = 0;
							}
						}
						else
						{
							if (paux->check_update(paux->haux))
							{
								nres = vatek_chip_write_memory(pbc->hchip, HALREG_AUXDATA_CNTL, AUXDATA_EN_STOP);
								if (is_vatek_success(nres))pbc->is_auxupdate = 1;
							}
						}
					}
				}

				if (is_vatek_success(nres) && pinfo)
				{
					if ((cross_os_get_tick_ms() - pbc->tick) > 1000)
					{
						nres = broadcast_info_get(pbc->hchip, &pbc->info);
						if (is_vatek_success(nres))pbc->tick = cross_os_get_tick_ms();
					}
					*pinfo = &pbc->info;
				}
			}
		}
	}
	return nres;
}

vatek_result broadcast_usb_start_aux(Phandle_broadcast pbc, Pbroadcast_auxstream paux)
{
	vatek_result nres = vatek_device_usbbulk_get_size(pbc->hchip);
	Pusbbulk_command pcmd = &pbc->usbcmd;
	Pusbbulk_result presult = &pbc->usbresult;
	Pusbparam_aux pusbaux = &pcmd->_h.aux;
	if (is_vatek_success(nres))
	{
		int32_t nepsize = nres;
		Pusbparam_aux pusbaux = &paux->aux;
		nres = vatek_badparam;
		if (pusbaux->mode == usbaux_sync || pusbaux->mode == usbaux_async)
		{
			pcmd->mode = usbbulk_aux;
			memcpy(&pcmd->_h.aux, pusbaux, sizeof(usbparam_aux));
			nres = vatek_success;
		}

		if (is_vatek_success(nres))
		{
			nres = vatek_device_usbbulk_send(pbc->hchip, pcmd, presult, NULL, 0);
			if (is_vatek_success(nres))
			{
				if (!is_vatek_success(presult->result))
					nres = presult->result;
			}

			if (is_vatek_success(nres) && pusbaux->mode == usbaux_async)
			{
				int32_t totalpkts = pusbaux->length / TS_PACKET_LEN;
				while (totalpkts)
				{
					int32_t neach = CHIP_STREAM_SLICE_PACKET_NUMS;
					if (neach > totalpkts)neach = totalpkts;
					nres = paux->get_packets(paux->haux, &pbc->usbbuffer[0], neach);
					if (is_vatek_success(nres))
					{
						int32_t bulksize = neach * TS_PACKET_LEN;
						if (bulksize % nepsize != 0)
							bulksize = ((bulksize + (nepsize - 1)) / nepsize) * nepsize;
						nres = vatek_device_usbbulk_write(pbc->hchip, &pbc->usbbuffer[0], bulksize);
					}

					if (!is_vatek_success(nres))break;
					totalpkts -= neach;
					if (!totalpkts)
						nres = vatek_device_usbbulk_get_result(pbc->hchip, presult);
				}

				if (is_vatek_success(nres))
				{
					if (!is_vatek_success(nres))nres = presult->result;
				}
			}
		}
	}
	return nres;
}
