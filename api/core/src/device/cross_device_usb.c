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

#include "./internal/cross_device_tool.h"

/* usb_device_chip_api */
extern vatek_result usbdevice_read_register(hcross_device hdev, int32_t addr, uint32_t* val);
extern vatek_result usbdevice_write_register(hcross_device hdev, int32_t addr, uint32_t val);
extern vatek_result usbdevice_read_memory(hcross_device hdev, int32_t addr, uint32_t* val);
extern vatek_result usbdevice_write_memory(hcross_device hdev, int32_t addr, uint32_t val);
extern vatek_result usbdevice_sendcmd(hcross_device hdev, int32_t cmd, int32_t addr, uint8_t* vals, int32_t wlen);
extern vatek_result usbdevice_write_buffer(hcross_device hdev, int32_t addr, uint8_t* buf, int32_t wlen);
extern vatek_result usbdevice_read_buffer(hcross_device hdev, int32_t addr, uint8_t* buf, int32_t wlen);
extern vatek_result usbdevice_free(hcross_device hdev);

extern vatek_result usbdevice_stream_start(hcross_device hdev,cross_stream_mode streammode);
extern vatek_result usbdevice_stream_stop(hcross_device hdev);
extern vatek_result usbdevice_stream_write(hcross_device hdev, uint8_t* pbuf, int32_t len);
extern vatek_result usbdevice_stream_read(hcross_device hdev, uint8_t* pbuf, int32_t len);

static cross_core udev_core = 
{
	.read_memory = usbdevice_read_memory,
	.write_memory = usbdevice_write_memory,
	.read_register = usbdevice_read_register,
	.write_register = usbdevice_write_register,
	.read_buffer = usbdevice_read_buffer,
	.write_buffer = usbdevice_write_buffer,
	.sendcmd = usbdevice_sendcmd,
};

static cross_stream udev_stream = 
{
	.start_stream = usbdevice_stream_start,
	.stop_stream = usbdevice_stream_stop,
	.write_stream = usbdevice_stream_write,
	.read_stream = usbdevice_stream_read,
};

static cross_usbbulk udev_bulk =
{
	.get_size = usb_api_ll_bulk_get_size,
	.send_command = usb_api_ll_bulk_send_command,
	.get_result = usb_api_ll_bulk_get_result,
	.write = usb_api_ll_bulk_write,
	.read = usb_api_ll_bulk_read,
};

vatek_result cross_usb_device_open(husb_device husb, Pcross_device* pcross)
{
	vatek_result nres = usb_api_ll_open(husb);
	if (is_vatek_success(nres))
	{
		uint32_t val = 0;
		nres = udev_core.read_memory((hcross_device)husb, HALREG_SERVICE_MODE, &val);
		if (is_vatek_success(nres))
		{
			hal_service_mode halservice = (hal_service_mode)val;
			Pcross_device newdev = NULL;
			nres = cdevice_malloc(&newdev, halservice);
			if (is_vatek_success(nres))
			{
				newdev->driver = cdriver_usb;
				newdev->bus = DEVICE_BUS_USB;
				newdev->service = halservice;
				newdev->hcross = husb;
				newdev->core = &udev_core;
				if (halservice == service_transform)
					newdev->stream = &udev_stream;
				newdev->bulk = &udev_bulk;
				*pcross = newdev;
			}
		}
		if (!is_vatek_success(nres))usb_api_ll_close(husb);
	}
	return nres;
}

void cross_usb_device_close(Pcross_device pcross)
{
	usb_api_ll_close((husb_device)pcross->hcross);
	cdevice_free(pcross);
}

husb_device cross_get_usb_device(Pcross_device pcross)
{
	if (pcross->driver == cdriver_usb)
		return (husb_device)pcross->hcross;
	return NULL;
}

vatek_result usbdevice_stream_start(hcross_device hdev,cross_stream_mode streammode)
{
	vatek_result nres = vatek_unsupport;
	if (streammode == stream_mode_output)
		nres = usb_api_ll_set_dma((husb_device)hdev, 1);
	else if (streammode == stream_mode_output_nodma)
		nres = usb_api_ll_set_dma((husb_device)hdev, 0);
	return nres;
}

vatek_result usbdevice_stream_stop(hcross_device hdev)
{
	return usb_api_ll_set_dma((husb_device)hdev, 0);
}

vatek_result usbdevice_stream_write(hcross_device hdev, uint8_t* pbuf, int32_t len)
{
	vatek_result nres = vatek_success;
	usb_api_ll_lock((husb_device)hdev);
	nres = usb_api_ll_write((husb_device)hdev, pbuf, len);
	usb_api_ll_unlock((husb_device)hdev);
	return nres;
}

vatek_result usbdevice_stream_read(hcross_device hdev, uint8_t* pbuf, int32_t len)
{
	return vatek_unsupport;
}

vatek_result usbdevice_read_register(hcross_device hdev, int32_t addr, uint32_t* val)
{
	vatek_result nres = vatek_success;
	husb_device husb = (husb_device)hdev;
	uint32_t buf[2];

	usb_api_ll_lock(husb);
	nres = usb_api_ll_command(husb, VATCMD_CLASSV2_SETADDR, addr, NULL);
	if (is_vatek_success(nres))
	{
		nres = usb_api_ll_command(husb, VATCMD_CLASSV2_RDREG, 0, (uint8_t*)&buf[0]);
		if (is_vatek_success(nres))
		{
			buf[0] = vatek_buffer_2_uint32((uint8_t*)&buf[0]);
			*val = vatek_buffer_2_uint32((uint8_t*)&buf[1]);
			if (buf[0] != addr)nres = vatek_badparam;
		}
	}
	usb_api_ll_unlock(husb);
	return nres;
}

vatek_result usbdevice_write_register(hcross_device hdev, int32_t addr, uint32_t val)
{
	vatek_result nres = vatek_success;
	husb_device husb = (husb_device)hdev;
	usb_api_ll_lock(husb);
	nres = usb_api_ll_command(husb, VATCMD_CLASSV2_SETADDR, addr, NULL);
	if (is_vatek_success(nres))
	{
		nres = usb_api_ll_command(husb, VATCMD_CLASSV2_WRREG, val, NULL);
	}
	usb_api_ll_unlock(husb);
	return nres;
}

vatek_result usbdevice_read_memory(hcross_device hdev, int32_t addr, uint32_t* val)
{
	vatek_result nres = vatek_success;
	husb_device husb = (husb_device)hdev;
	uint32_t buf[2];
	usb_api_ll_lock(husb);

	nres = usb_api_ll_command(husb, VATCMD_CLASSV2_SETADDR, addr, NULL);
	if (is_vatek_success(nres))
	{
		nres = usb_api_ll_command(husb, VATCMD_CLASSV2_RDMEM, 0, (uint8_t*)&buf[0]);
		if (is_vatek_success(nres))
		{
			buf[0] = vatek_buffer_2_uint32((uint8_t*)&buf[0]);
			*val = vatek_buffer_2_uint32((uint8_t*)&buf[1]);
			if (buf[0] != addr)nres = vatek_badparam;
		}
	}
	usb_api_ll_unlock(husb);
	return nres;
}

vatek_result usbdevice_write_memory(hcross_device hdev, int32_t addr, uint32_t val)
{
	vatek_result nres = vatek_success;
	husb_device husb = (husb_device)hdev;
	usb_api_ll_lock(husb);
	nres = usb_api_ll_command(husb, VATCMD_CLASSV2_SETADDR, addr, NULL);
	if (is_vatek_success(nres))
	{
		nres = usb_api_ll_command(husb, VATCMD_CLASSV2_WRMEM, val, NULL);
	}
	usb_api_ll_unlock(husb);
	return nres;
}

vatek_result usbdevice_sendcmd(hcross_device hdev, int32_t cmd, int32_t addr, uint8_t* vals, int32_t wlen)
{
	vatek_result nres = vatek_success;
	uint32_t buf[2];
	int32_t endaddr = addr + wlen;
	uint32_t usbcmd = 0;
	int32_t iswrite = 0;
	uint32_t* wvalues = (uint32_t*)vals;
	husb_device husb = (husb_device)hdev;

	if (cmd == CHIP_CMD_RDREG)usbcmd = VATCMD_CLASSV2_RDREG;
	else if (cmd == CHIP_CMD_WRREG)usbcmd = VATCMD_CLASSV2_WRREG;
	else if (cmd == CHIP_CMD_RDMEM || cmd == CHIP_CMD_RDBUF)usbcmd = VATCMD_CLASSV2_RDMEM;
	else if (cmd == CHIP_CMD_WRMEM || cmd == CHIP_CMD_WRBUF)usbcmd = VATCMD_CLASSV2_WRMEM;
	else return vatek_badparam;

	if (usbcmd == VATCMD_CLASSV2_WRREG ||
		usbcmd == VATCMD_CLASSV2_WRMEM)iswrite = 1;

	usb_api_ll_lock(husb);
	nres = usb_api_ll_command(husb, VATCMD_CLASSV2_SETADDR, addr, NULL);
	if (is_vatek_success(nres))
	{
		while (addr < endaddr)
		{
			if (iswrite)
			{
				if (cmd == CHIP_CMD_WRBUF)nres = usb_api_ll_command_buffer(husb, usbcmd, (uint8_t*)wvalues, NULL);
				else nres = usb_api_ll_command(husb, usbcmd, *wvalues, NULL);
			}
			else
			{
				nres = usb_api_ll_command(husb, usbcmd, addr, (uint8_t*)&buf[0]);
				if (is_vatek_success(nres))
				{

					buf[0] = vatek_buffer_2_uint32((uint8_t*)&buf[0]);
					if (cmd != CHIP_CMD_RDBUF)buf[1] = vatek_buffer_2_uint32((uint8_t*)&buf[1]);
					if (buf[0] == addr)*wvalues = buf[1];
				}
			}

			if (!is_vatek_success(nres))break;
			wvalues++;
			addr++;
		}
	}
	usb_api_ll_unlock(husb);
	return nres;
}

vatek_result usbdevice_write_buffer(hcross_device hdev, int32_t addr, uint8_t* buf, int32_t wlen)
{
	return usbdevice_sendcmd(hdev, CHIP_CMD_WRBUF, addr, buf, wlen);
}

vatek_result usbdevice_read_buffer(hcross_device hdev, int32_t addr, uint8_t* buf, int32_t wlen)
{
	return usbdevice_sendcmd(hdev, CHIP_CMD_RDBUF, addr, buf, wlen);
}

vatek_result usbdevice_free(hcross_device hdev)
{
	return usb_api_ll_close(hdev);
}
