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

#include <cross/cross_bridge.h>
#include <cross/cross_os_api.h>

vatek_result bridge_device_lock(hbridge_device hbridge)
{
    vatek_result nres = vatek_success;
    if (bridge_device_get_status(hbridge) == hid_status_idle)
    {
		Phid_bridge_cmd pcmd = bridge_device_get_command(hbridge);
		bridge_device_lock_command(hbridge);
		BRIDGE_SETCMD(pcmd, USB_CMD_LOCK);
		nres = bridge_device_send_bridge_command(hbridge);
		bridge_device_unlock_command(hbridge);

		if (is_vatek_success(nres))
		{
			int32_t count = 0;
			cross_os_sleep(10);
			while (count < 10)
			{
				nres = bridge_device_get_status(hbridge);
				if (is_vatek_success(nres) && nres == hid_status_locked)break;
				cross_os_sleep(10);
				count++;
				if (count == 10)
				{
					nres = vatek_timeout;
					break;
				}
			}
		}

		if (is_vatek_success(nres))
		{
			bridge_device_lock_command(hbridge);
			BRIDGE_SETCMD(pcmd, MODULATOR_CMD_OPEN);
			BRIDGE_MOD_SETIDX(pcmd, 0);
			nres = bridge_device_send_bridge_command(hbridge);
			bridge_device_unlock_command(hbridge);
		}
    }	
    return nres;
}

vatek_result bridge_device_unlock(hbridge_device hbridge)
{
	vatek_result nres = vatek_success;
	if (bridge_device_get_status(hbridge) == hid_status_locked)
	{
		Phid_bridge_cmd pcmd = bridge_device_get_command(hbridge);
		bridge_device_lock_command(hbridge);
		BRIDGE_SETCMD(pcmd, MODULATOR_CMD_CLOSE);
		BRIDGE_MOD_SETIDX(pcmd, 0);
		nres = bridge_device_send_bridge_command(hbridge);
		bridge_device_unlock_command(hbridge);

		if (is_vatek_success(nres))
		{
			bridge_device_lock_command(hbridge);
			BRIDGE_SETCMD(pcmd, USB_CMD_UNLOCK);
			nres = bridge_device_send_bridge_command(hbridge);
			bridge_device_unlock_command(hbridge);

			if (is_vatek_success(nres))
			{
				int32_t count = 0;
				cross_os_sleep(10);
				while (count < 10)
				{
					nres = bridge_device_get_status(hbridge);
					if (is_vatek_success(nres) && nres == hid_status_idle)return nres;
					cross_os_sleep(10);
					count++;
				}
				nres = vatek_timeout;
			}
		}
	}
    return nres;
}

bridge_device_status bridge_device_get_status(hbridge_device hbridge)
{
    Phid_bridge_cmd pcmd = bridge_device_get_command(hbridge);
	bridge_device_status status;
    BRIDGE_SETCMD(pcmd, USB_CMD_GETSTATUS);
	status = bridge_device_send_bridge_command(hbridge);
	return status;
}

vatek_result bridge_device_bulk_transfer(hbridge_device hbridge,uint32_t type,uint32_t addr,uint8_t* pbuf,uint32_t len)
{
	Phid_bridge_cmd pcmd = bridge_device_get_command(hbridge);
	Pbridge_mod_param modparam = &pcmd->param.mod;
	vatek_result nres = vatek_unknown;
	int32_t eachlen = 0;
	uint8_t iswrite = 0;
	int32_t blen = len * 4;

	if (MOD_DATA_IS_WRITE(type))iswrite = 1;
	while (blen > 0)
	{
		eachlen = BRIDGE_MOD_BUFFER_LEN;
		if (eachlen > (int32_t)blen)
			eachlen = blen;

		bridge_device_lock_command(hbridge);

		BRIDGE_MOD_SETADDR(pcmd, addr);
		BRIDGE_MOD_SETLEN(pcmd, eachlen / 4);
		BRIDGE_SETCMD(pcmd, MODULATOR_CMD_TRANSFER);
		BRIDGE_MOD_SETIDX(pcmd, 0);
		BRIDGE_MOD_SETDATATYPE(pcmd, type);

		if (iswrite)memcpy(&modparam->buffer[0], pbuf, eachlen);
		nres = bridge_device_send_bridge_command(hbridge);
		if (is_vatek_success(nres))
		{
			Phid_bridge_result presult = bridge_device_get_result(hbridge);
			if (!iswrite && is_vatek_success(presult->result))
				memcpy(pbuf, &presult->data.mod.buffer[0], eachlen);
			else nres = presult->result;
			blen -= eachlen;
			pbuf += eachlen;
			addr += eachlen / 4;
		}
		bridge_device_unlock_command(hbridge);
		if (!is_vatek_success(nres))break;
	}
	return nres;
}
