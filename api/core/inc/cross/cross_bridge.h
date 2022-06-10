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

#ifndef _CROSS_BRIDGE_
#define _CROSS_BRIDGE_

#include <bridge/bridge_usb.h>
#include <bridge/bridge_device.h>

typedef void* hbridge_device;
typedef void* hbridge_list;

#define BRIDGE_SETCMD(pcmd,val)             vatek_uint32_2_buffer((uint8_t*)&pcmd->cmd,val)
#define BRIDGE_PARAM_SET(cmd,idx,val)       vatek_uint32_2_buffer((uint8_t*)&cmd->param.base.params[idx],val)
#define BRIDGE_PARAM_RAW_PTR(cmd)           &cmd->param.raw[0]
#define BRIDGE_MOD_SETIDX(cmd,idx)          vatek_uint32_2_buffer((uint8_t*)&cmd->param.mod.index,idx)
#define BRIDGE_MOD_SETADDR(cmd,addr)        vatek_uint32_2_buffer((uint8_t*)&cmd->param.mod.address,addr)
#define BRIDGE_MOD_SETLEN(cmd,lenval)       vatek_uint32_2_buffer((uint8_t*)&cmd->param.mod.len,lenval)
#define BRIDGE_MOD_SETDATATYPE(cmd,type)    vatek_uint32_2_buffer((uint8_t*)&cmd->param.mod.datatype,type)
#define BRIDGE_MOD_GETBUF(cmd)              &cmd->param.mod.buffer[0]
#define BRIDGE_I2C_SETHANDLE(cmd,val)       vatek_uint32_2_buffer((uint8_t*)&cmd->param.i2c.handle,val)
#define BRIDGE_I2C_SETADDR(cmd,addr)        vatek_uint32_2_buffer((uint8_t*)&cmd->param.i2c.address,addr)
#define BRIDGE_I2C_SETLEN(cmd,len)          vatek_uint32_2_buffer((uint8_t*)&cmd->param.i2c.len,len)

#ifdef __cplusplus
extern "C" {
#endif

	/* bridge_device core tool [cross_hal_interface ]*/
	HAL_API vatek_result bridge_device_list_enum_default(hbridge_list* hblist);
	HAL_API vatek_result bridge_device_list_enum_usb(uint16_t vid, uint16_t pid, hbridge_list* hblist);
	HAL_API vatek_result bridge_device_list_get(hbridge_list hblist, int32_t idx, hbridge_device* hbridge);
	HAL_API const char* bridge_device_list_get_name(hbridge_list hblist, int32_t idx);
	HAL_API vatek_result bridge_device_list_free(hbridge_list hbridges);

	HAL_API vatek_result bridge_device_open(hbridge_device hbridge);
    HAL_API const char* bridge_device_get_name(hbridge_device hbridge);
    HAL_API vatek_result bridge_device_close(hbridge_device hbridge);
	HAL_API void bridge_device_lock_command(hbridge_device hbridge);
	HAL_API Phid_bridge_cmd bridge_device_get_command(hbridge_device hbridge);
	HAL_API Phid_bridge_result bridge_device_get_result(hbridge_device hbridge);
	HAL_API vatek_result bridge_device_send_bridge_command(hbridge_device hbridge);
	HAL_API void bridge_device_unlock_command(hbridge_device hbridge);

	/* bridge_device base tool */
	HAL_API vatek_result bridge_device_lock(hbridge_device hbridge);
	HAL_API vatek_result bridge_device_unlock(hbridge_device hbridge);
	HAL_API bridge_device_status bridge_device_get_status(hbridge_device hbridge);
	HAL_API vatek_result bridge_device_bulk_transfer(hbridge_device hbridge,uint32_t type,uint32_t addr,uint8_t* pbuf,uint32_t len);

	/* bridge_device board devices tool */

	HAL_API vatek_result bridge_device_get_info(hbridge_device hbridge, Pbdevice_info pdevinfo);

	HAL_API vatek_result bridge_device_get_demod_info(hbridge_device hbridge, Pbdemod_info pinfo);
	HAL_API vatek_result bridge_device_set_demod_mode(hbridge_device hbridge, Pbdemod_mode_param pmode);
	HAL_API vatek_result bridge_device_get_demod_mode(hbridge_device hbridge, Pbdemod_mode_param pmode);

	HAL_API vatek_result bridge_device_start_demod(hbridge_device hbridge, Pbdemod_op_param param);
	HAL_API vatek_result bridge_device_stop_demod(hbridge_device hbridge);

	HAL_API vatek_result bridge_device_get_source(hbridge_device hbridge, int32_t idx, Pbridge_source pphy);
	HAL_API vatek_result bridge_device_start_source(hbridge_device hbridge, Pbridge_source pvideo);
	HAL_API vatek_result bridge_device_stop_source(hbridge_device hbridge);

	HAL_API vatek_result bridge_device_get_source_status(hbridge_device hbridge, Pbridge_source psource);
	HAL_API const char* bridge_device_get_source_name(hbridge_device hbridge, Pbridge_source psource);

	HAL_API vatek_result bridge_device_start_rfmixer(hbridge_device hbridge, Pbrfmixer_op_param pparam);
	HAL_API vatek_result bridge_device_stop_rfmixer(hbridge_device hbridge);

	HAL_API vatek_result bridge_device_storage_write(hbridge_device hbridge, int32_t nsection, uint8_t* pbuf);
	HAL_API vatek_result bridge_device_storage_read(hbridge_device hbridge, int32_t nsection, uint8_t* pbuf);
	HAL_API vatek_result bridge_device_storage_erase(hbridge_device hbridge, int32_t nsection);

#ifdef __cplusplus
}
#endif

#endif