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

extern vatek_result bridge_read_register(hcross_device hdev, int32_t addr, uint32_t* val);
extern vatek_result bridge_write_register(hcross_device hdev, int32_t addr, uint32_t val);
extern vatek_result bridge_read_memory(hcross_device hdev, int32_t addr, uint32_t* val);
extern vatek_result bridge_write_memory(hcross_device hdev, int32_t addr, uint32_t val);
extern vatek_result bridge_sendcmd(hcross_device hdev, int32_t cmd, int32_t addr, uint8_t* vals, int32_t wlen);
extern vatek_result bridge_write_buffer(hcross_device hdev, int32_t addr, uint8_t* buf, int32_t wlen);
extern vatek_result bridge_read_buffer(hcross_device hdev, int32_t addr, uint8_t* buf, int32_t wlen);

static cross_core bridge_core = 
{
	.read_memory = bridge_read_memory,
	.write_memory = bridge_write_memory,
	.read_register = bridge_read_register,
	.write_register = bridge_write_register,
	.read_buffer = bridge_read_buffer,
	.write_buffer = bridge_write_buffer,
	.sendcmd = bridge_sendcmd,
};

vatek_result cross_bridge_open(hbridge_device hbridge, Pcross_device* pcross)
{
	vatek_result nres = bridge_device_open(hbridge);
	if (is_vatek_success(nres))
	{
		nres = bridge_device_lock(hbridge);
		if (is_vatek_success(nres))
		{
			uint32_t val = 0;
			nres = bridge_core.read_memory((hcross_device)hbridge, HALREG_SERVICE_MODE, &val);
			if (is_vatek_success(nres))
			{
				hal_service_mode halservice = (hal_service_mode)val;
				Pcross_device newdev = NULL;
				nres = cdevice_malloc(&newdev, halservice);
				if (is_vatek_success(nres))
				{
					newdev->driver = cdriver_bridge;
					newdev->bus = DEVICE_BUS_BRIDGE;
					newdev->service = halservice;
					newdev->hcross = hbridge;
					newdev->core = &bridge_core;
					newdev->stream = NULL;
					*pcross = newdev;
				}
			}
			if (!is_vatek_success(nres))bridge_device_unlock(hbridge);
		}
		if (!is_vatek_success(nres))bridge_device_close(hbridge);
	}
	return nres;
}

hbridge_device cross_get_bridge_handle(Pcross_device pcross)
{
	if (pcross->driver == cdriver_bridge)
		return (hbridge_device)pcross->hcross;
	return NULL;
}

void cross_bridge_close(Pcross_device pcross)
{
	hbridge_device hbridge = (hbridge_device)pcross->hcross;
	bridge_device_unlock(hbridge);
	bridge_device_close(hbridge);
	cdevice_free(pcross);
}

vatek_result bridge_read_register(hcross_device hdev, int32_t addr, uint32_t* val)
{
	uint8_t buf[4];
	vatek_result nres = bridge_device_bulk_transfer((hbridge_device)hdev, MOD_RD_REG, addr, (uint8_t*)buf, 1);
	if (is_vatek_success(nres))
	{
		*val = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
	}
	return nres;
}

vatek_result bridge_write_register(hcross_device hdev, int32_t addr, uint32_t val)
{
	uint8_t buf[4];
	/* tip : when used bulk transfer buffer order is follow i2c order (_vatek_chip is big endian) */
	buf[3] = val >> 24;
	buf[2] = val >> 16;
	buf[1] = val >> 8;
	buf[0] = val;
	return bridge_device_bulk_transfer((hbridge_device)hdev, MOD_WR_REG, addr, buf, 1);
}

vatek_result bridge_read_memory(hcross_device hdev, int32_t addr, uint32_t* val)
{
	uint8_t buf[4];
	vatek_result nres = bridge_device_bulk_transfer((hbridge_device)hdev, MOD_RD_MEM, addr, (uint8_t*)buf, 1);
	if (is_vatek_success(nres))
	{
		*val = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
	}
	return nres;
}

vatek_result bridge_write_memory(hcross_device hdev, int32_t addr, uint32_t val)
{
	uint8_t buf[4];
	buf[3] = val >> 24;
	buf[2] = val >> 16;
	buf[1] = val >> 8;
	buf[0] = val;
	return bridge_device_bulk_transfer((hbridge_device)hdev, MOD_WR_MEM, addr, buf, 1);
}

extern vatek_result bridge_cmd_ip_transfer(hcross_device hdev, int32_t cmd, uint8_t* pbuf);

vatek_result bridge_sendcmd(hcross_device hdev, int32_t cmd, int32_t addr, uint8_t* vals, int32_t wlen)
{
	uint32_t bridgecmd = 0;

	if (cmd == CHIP_CMD_RDREG)bridgecmd = MOD_RD_REG;
	else if (cmd == CHIP_CMD_WRREG)bridgecmd = MOD_WR_REG;
	else if (cmd == CHIP_CMD_RDMEM)bridgecmd = MOD_RD_MEM;
	else if (cmd == CHIP_CMD_WRMEM)bridgecmd = MOD_WR_MEM;
	else if (cmd == CHIP_CMD_RDCODE || cmd == CHIP_CMD_RDBUF)bridgecmd = MOD_RD_CODE;
	else if (cmd == CHIP_CMD_WRCODE || cmd == CHIP_CMD_WRBUF)bridgecmd = MOD_WR_CODE;
	else if (cmd == CHIP_CMD_BRIDGE)return bridge_cmd_ip_transfer((hbridge_device)hdev, addr, vals);
	else return vatek_badparam;

	return bridge_device_bulk_transfer((hbridge_device)hdev, bridgecmd, addr, vals, wlen);
}

vatek_result bridge_write_buffer(hcross_device hdev, int32_t addr, uint8_t* buf, int32_t wlen)
{
	return bridge_sendcmd((hbridge_device)hdev, CHIP_CMD_WRBUF, addr, buf, wlen);
}

vatek_result bridge_read_buffer(hcross_device hdev, int32_t addr, uint8_t* buf, int32_t wlen)
{
	return bridge_sendcmd((hbridge_device)hdev, CHIP_CMD_RDBUF, addr, buf, wlen);
}

vatek_result bridge_cmd_ip_transfer(hcross_device hdev, int32_t cmd, uint8_t* pbuf)
{
	hbridge_device hhid = (hbridge_device)hdev;
	Phid_bridge_cmd pcmd = bridge_device_get_command(hhid);
	vatek_result nres = vatek_unknown;

	bridge_device_lock_command(hdev);

	BRIDGE_SETCMD(pcmd, cmd);

	memcpy(&pcmd->param.raw[0], pbuf, BRIDGE_PARAM_MAX_LEN);
	nres = bridge_device_send_bridge_command(hhid);

	if (is_vatek_success(nres))
	{
		Phid_bridge_result presult = bridge_device_get_result(hhid);
		nres = presult->result;
		if (is_vatek_success(nres))
			memcpy(pbuf, &presult->data.raw[0], BRIDGE_PARAM_MAX_LEN);
	}

	bridge_device_unlock_command(hdev);
	return nres;
}
