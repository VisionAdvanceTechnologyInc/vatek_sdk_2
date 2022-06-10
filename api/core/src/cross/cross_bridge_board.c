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

extern vatek_result map_bridge_to_buffer(uint8_t* bbuf, uint8_t* bdata, const Pstruct_bridge_param params);
extern vatek_result map_buffer_to_bridge(uint8_t* bbuf, uint8_t* bdata, const Pstruct_bridge_param params);

//extern vatek_result bridge_send_cmd(hbridge_device hbridge,int32_t cmd);
extern vatek_result bridge_send_cmd(hbridge_device hbridge, int32_t cmd, uint8_t* praw, const Pstruct_bridge_param pbparam, Phid_bridge_result* presult);
extern const char* bridge_get_tag_string(uint32_t tag);
extern vatek_result bridge_get_source(Phid_bridge_result presult,Pbridge_source psource);

vatek_result bridge_device_get_info(hbridge_device hbridge, Pbdevice_info pdevinfo)
{
	vatek_result nres = vatek_success;
	Phid_bridge_result presult = NULL;
	bridge_device_lock_command(hbridge);
	nres = bridge_send_cmd(hbridge, BDEVICE_CMD_GET_INFO,NULL,NULL,&presult);
	if (is_vatek_success(nres))
		nres = map_bridge_to_buffer(&presult->data.raw[0], (uint8_t*)pdevinfo, _bparam_struct_get(bdevice_info));
	bridge_device_unlock_command(hbridge);
	return nres;
}

vatek_result bridge_device_get_demod_info(hbridge_device hbridge, Pbdemod_info pinfo)
{
	vatek_result nres = vatek_success;
	Phid_bridge_result presult = NULL;
	bridge_device_lock_command(hbridge);
	nres = bridge_send_cmd(hbridge, BDEMOD_CMD_GETINFO,NULL,NULL,&presult);
	if (is_vatek_success(nres))
		nres = map_bridge_to_buffer(&presult->data.raw[0], (uint8_t*)pinfo, _bparam_struct_get(bdemod_info));
	bridge_device_unlock_command(hbridge);
	return nres;
}

vatek_result bridge_device_set_demod_mode(hbridge_device hbridge, Pbdemod_mode_param pmode)
{
	vatek_result nres = vatek_success;
	bridge_device_lock_command(hbridge);
	nres = bridge_send_cmd(hbridge, BDEMOD_CMD_SET_MODE, (uint8_t*)pmode, _bparam_struct_get(bdemod_mode_param), NULL);
	bridge_device_unlock_command(hbridge);
	return nres;
}

vatek_result bridge_device_get_demod_mode(hbridge_device hbridge, Pbdemod_mode_param pmode)
{
	vatek_result nres = vatek_success;
	Phid_bridge_result presult = NULL;
	bridge_device_lock_command(hbridge);
	nres = bridge_send_cmd(hbridge, BDEMOD_CMD_GET_MODE,NULL,NULL,&presult);
	if (is_vatek_success(nres))
		nres = map_bridge_to_buffer(&presult->data.raw[0], (uint8_t*)pmode, _bparam_struct_get(bdemod_mode_param));
	bridge_device_unlock_command(hbridge);
	return nres;
}

vatek_result bridge_device_start_demod(hbridge_device hbridge, Pbdemod_op_param param)
{
	vatek_result nres = vatek_success;
	bridge_device_unlock_command(hbridge);
	nres = bridge_send_cmd(hbridge, BDEMOD_CMD_START, (uint8_t*)param, _bparam_struct_get(bdemod_op_param), NULL);
	bridge_device_unlock_command(hbridge);
	return nres;
}

vatek_result bridge_device_stop_demod(hbridge_device hbridge)
{
	vatek_result nres = vatek_success;
	bridge_device_lock_command(hbridge);
		nres = bridge_send_cmd(hbridge, BDEMOD_CMD_STOP,NULL,NULL,NULL);
	bridge_device_unlock_command(hbridge);
	return nres;
}

vatek_result bridge_device_get_source(hbridge_device hbridge, int32_t idx, Pbridge_source pphy)
{
	vatek_result nres = vatek_success;
	Phid_bridge_result presult = NULL;
	bridge_device_lock_command(hbridge);
	nres = bridge_send_cmd(hbridge, BSOURCE_CMD_STATUS | (idx & 0xFF), (uint8_t*)pphy, _bparam_struct_get(bridge_source),&presult);
	if (is_vatek_success(nres))
	{
		nres = bridge_get_source(presult, pphy);
#if 0
		nres = map_bridge_to_buffer(&presult->data.raw[0], (uint8_t*)pphy, _bparam_struct_get(bridge_source));
		if (is_vatek_success(nres))
		{
			nres = map_bridge_to_buffer(&presult->data.raw[0], (uint8_t*)&pphy->video_info, _bparam_struct_get(bridge_video_info));
			if (is_vatek_success(nres))
				nres = map_bridge_to_buffer(&presult->data.raw[0], (uint8_t*)&pphy->audio_info, _bparam_struct_get(bridge_audio_info));
		}
#endif
	}
	bridge_device_unlock_command(hbridge);
	return nres;
}

vatek_result bridge_device_start_source(hbridge_device hbridge, Pbridge_source pvideo)
{
	vatek_result nres = vatek_success;
	bridge_source_cmd cmd;
	memset(&cmd, 0, sizeof(bridge_source_cmd));
	cmd.source_cntl = BSOURCE_CNTL_START;
	cmd.source_id = pvideo->source_id;
	bridge_device_lock_command(hbridge);
	nres = bridge_send_cmd(hbridge, BSOURCE_CMD_CNTL, (uint8_t*)&cmd, _bparam_struct_get(bridge_source_cmd), NULL);
	bridge_device_unlock_command(hbridge);
	return nres;
}

vatek_result bridge_device_stop_source(hbridge_device hbridge)
{
	vatek_result nres = vatek_success;
	bridge_source_cmd cmd;
	memset(&cmd, 0, sizeof(bridge_source_cmd));
	cmd.source_cntl = BSOURCE_CNTL_STOP;
	bridge_device_lock_command(hbridge);
	nres = bridge_send_cmd(hbridge, BSOURCE_CMD_CNTL, (uint8_t*)&cmd, _bparam_struct_get(bridge_source_cmd), NULL);
	bridge_device_unlock_command(hbridge);
	return nres;
}

vatek_result bridge_device_get_source_status(hbridge_device hbridge, Pbridge_source psource)
{
	vatek_result nres = vatek_success;
	Phid_bridge_result presult = NULL;
	bridge_device_lock_command(hbridge);
	nres = bridge_send_cmd(hbridge, BSOURCE_CMD_GETACTIVE,NULL,NULL,&presult);
	if (is_vatek_success(nres))
	{
		nres = bridge_get_source(presult, psource);
#if 0
		nres = map_bridge_to_buffer(&presult->data.raw[0], (uint8_t*)psource, _bparam_struct_get(bridge_source));
		if (is_vatek_success(nres))
		{
			nres = map_bridge_to_buffer(&presult->data.raw[0], (uint8_t*)&psource->video_info, _bparam_struct_get(bridge_video_info));
			if (is_vatek_success(nres))
				nres = map_bridge_to_buffer(&presult->data.raw[0], (uint8_t*)&psource->audio_info, _bparam_struct_get(bridge_audio_info));
		}
#endif
	}
	bridge_device_unlock_command(hbridge);
	return nres;
}

const char* bridge_device_get_source_name(hbridge_device hbridge, Pbridge_source psource)
{
	return bridge_get_tag_string((uint32_t)psource->source_id);
}

vatek_result bridge_device_start_rfmixer(hbridge_device hbridge, Pbrfmixer_op_param pparam)
{
	vatek_result nres = vatek_success;
	bridge_device_lock_command(hbridge);
	nres = bridge_send_cmd(hbridge, BRFMIXER_CMD_START, (uint8_t*)pparam, _bparam_struct_get(brfmixer_op_param), NULL);
	bridge_device_unlock_command(hbridge);
	return nres;
}

vatek_result bridge_device_stop_rfmixer(hbridge_device hbridge)
{
	vatek_result nres = vatek_success;
	bridge_device_lock_command(hbridge);
	nres = bridge_send_cmd(hbridge, BRFMIXER_CMD_STOP,NULL,NULL,NULL);
	bridge_device_unlock_command(hbridge);
	return nres;
}

vatek_result bridge_device_storage_write(hbridge_device hbridge, int32_t nsection, uint8_t* pbuf)
{
	vatek_result nres = vatek_success;
	bflash_command cmd;
	memset(&cmd, 0, sizeof(bflash_command));
	cmd.bcmd = BFLASH_CNTL_WRITE;
	cmd.nsection = nsection;

	bridge_device_lock_command(hbridge);
	nres = bridge_send_cmd(hbridge, BDEVICE_CMD_BFLASH_START, (uint8_t*)&cmd, _bparam_struct_get(bflash_command), NULL);
	if (is_vatek_success(nres))
	{
		int32_t len = BSTORAGE_SECTION_LEN;
		int32_t pos = 0;
		Phid_bridge_cmd pcmd = bridge_device_get_command(hbridge);

		while (len)
		{
			int32_t rlen = len;
			if (rlen > BRIDGE_PARAM_MAX_LEN)rlen = BRIDGE_PARAM_MAX_LEN;
			else memset(&pcmd->param.raw[0], 0, BRIDGE_PARAM_MAX_LEN);
			memcpy(&pcmd->param.raw[0], &pbuf[pos], rlen);
			nres = bridge_send_cmd(hbridge, BDEVICE_CMD_BFLASH_POS,NULL,NULL,NULL);
			if (!is_vatek_success(nres))break;
			len -= rlen;
			pos += rlen;
		}

		if (is_vatek_success(nres))
			nres = bridge_send_cmd(hbridge, BDEVICE_CMD_BFLASH_END,NULL,NULL,NULL);
	}
	bridge_device_unlock_command(hbridge);
	return nres;
}

vatek_result bridge_device_storage_read(hbridge_device hbridge, int32_t nsection, uint8_t* pbuf)
{
	vatek_result nres = vatek_success;
	Phid_bridge_result presult = NULL;
	bflash_command cmd;
	memset(&cmd, 0, sizeof(bflash_command));
	cmd.bcmd = BFLASH_CNTL_READ;
	cmd.nsection = nsection;

	bridge_device_lock_command(hbridge);
	nres = bridge_send_cmd(hbridge, BDEVICE_CMD_BFLASH_START, (uint8_t*)&cmd, _bparam_struct_get(bflash_command), &presult);
	if (is_vatek_success(nres))
	{
		int32_t len = BSTORAGE_SECTION_LEN;
		int32_t pos = 0;
		while (len)
		{
			nres = bridge_send_cmd(hbridge, BDEVICE_CMD_BFLASH_POS, NULL, NULL, &presult);
			if (is_vatek_success(nres))
			{
				int32_t rlen = len;
				if (rlen > BRIDGE_PARAM_MAX_LEN)rlen = BRIDGE_PARAM_MAX_LEN;
				memcpy(&pbuf[pos], &presult->data.raw[0], rlen);
				len -= rlen;
				pos += rlen;
			}else break;
		}
		if (is_vatek_success(nres))
			nres = bridge_send_cmd(hbridge, BDEVICE_CMD_BFLASH_END, NULL, NULL, NULL);
	}
	bridge_device_unlock_command(hbridge);
	return nres;
}

vatek_result bridge_device_storage_erase(hbridge_device hbridge, int32_t nsection)
{
	bflash_command cmd;
	memset(&cmd, 0, sizeof(bflash_command));
	cmd.bcmd = BFLASH_CNTL_ERASE;
	cmd.nsection = nsection;
	vatek_result nres = vatek_success;
	bridge_device_lock_command(hbridge);
	nres = bridge_send_cmd(hbridge, BDEVICE_CMD_BFLASH_START, (uint8_t*)&cmd, _bparam_struct_get(bflash_command),NULL);
	bridge_device_unlock_command(hbridge);
	return nres;
}

vatek_result bridge_send_cmd(hbridge_device hbridge, int32_t cmd, uint8_t* praw, const Pstruct_bridge_param pbparam, Phid_bridge_result* presult)
{
	vatek_result nres = vatek_success;
	Phid_bridge_cmd pcmd = bridge_device_get_command(hbridge);
	BRIDGE_SETCMD(pcmd, cmd);
	if(praw && pbparam)
		nres = map_buffer_to_bridge(&pcmd->param.raw[0], praw, pbparam);

	if (is_vatek_success(nres))
	{
		nres = bridge_device_send_bridge_command(hbridge);
		if (is_vatek_success(nres))
		{
			Phid_bridge_result pcurresult = bridge_device_get_result(hbridge);
			nres = pcurresult->result;
			if (is_vatek_success(nres) && presult)*presult = pcurresult;
		}
	}
	return nres;
}

vatek_result bridge_get_source(Phid_bridge_result presult, Pbridge_source psource)
{
	vatek_result nres = map_bridge_to_buffer(&presult->data.raw[0], (uint8_t*)psource, _bparam_struct_get(bridge_source));
	if (is_vatek_success(nres))
	{
		nres = map_bridge_to_buffer(&presult->data.raw[0], (uint8_t*)&psource->video_info, _bparam_struct_get(bridge_video_info));
		if (is_vatek_success(nres))
			nres = map_bridge_to_buffer(&presult->data.raw[0], (uint8_t*)&psource->audio_info, _bparam_struct_get(bridge_audio_info));
	}
	return nres;
}

const char* bridge_get_tag_string(uint32_t tag)
{
	int32_t i = 0;
	for (i = 0; i < BTAG_NUMS; i++)
	{
		if (bridge_tags[i].tag == tag)return bridge_tags[i].name;
	}
	return "_unknown";
}

vatek_result map_bridge_to_buffer(uint8_t* bbuf, uint8_t* bdata,const Pstruct_bridge_param params)
{
	int32_t i = 0;
	vatek_result nres = vatek_success;

	while (params[i].type != BPARAM_TYPE_NULL)
	{
		Pstruct_bridge_param pbstruct = &params[i];
		uint8_t* pbridge = &bbuf[pbstruct->b_offset];
		uint8_t* pbuffer = &bdata[pbstruct->s_offset];

		if (pbstruct->type == BPARAM_TYPE_STR)memcpy(pbuffer, pbridge, BPARAM_STR_LEN);
		else if (pbstruct->type == BPARAM_TYPE_ENUM || pbstruct->type == BPARAM_TYPE_UINT)
		{
			int32_t val = (pbridge[0] << 24) | (pbridge[1] << 16) | (pbridge[2] << 8) | pbridge[3];
			if (pbstruct->typelen == 1)*((int8_t*)pbuffer) = (int8_t)val;
			else if (pbstruct->typelen == 2)*((int16_t*)pbuffer) = (int16_t)val;
			else *((int32_t*)pbuffer) = val;
		}
		else if (pbstruct->type == BPARAM_TYPE_RAW)memcpy(pbuffer, pbridge, pbstruct->typelen);
		else nres = vatek_badparam;
		if (!is_vatek_success(nres))break;
		i++;
	}
	return nres;
}

vatek_result map_buffer_to_bridge(uint8_t* bbuf, uint8_t* bdata,const Pstruct_bridge_param params)
{
	int32_t i = 0;
	vatek_result nres = vatek_success;

	while (params[i].type != BPARAM_TYPE_NULL)
	{
		Pstruct_bridge_param pbstruct = &params[i];
		uint8_t* pbridge = &bbuf[pbstruct->b_offset];
		uint8_t* pbuffer = &bdata[pbstruct->s_offset];

		if (pbstruct->type == BPARAM_TYPE_STR)memcpy(pbridge, pbuffer, BPARAM_STR_LEN);
		else if (pbstruct->type == BPARAM_TYPE_ENUM || pbstruct->type == BPARAM_TYPE_UINT)
		{
			int32_t val = 0;
			if (pbstruct->typelen == 1)val = (int32_t)(*((int8_t*)pbuffer));
			else if (pbstruct->typelen == 2)val = (int32_t)(*((int16_t*)pbuffer));
			else val = *(int32_t*)pbuffer;
			pbridge[0] = (uint8_t)(val >> 24);
			pbridge[1] = (uint8_t)(val >> 16);
			pbridge[2] = (uint8_t)(val >> 8);
			pbridge[3] = (uint8_t)(val);
		}
		else if (pbstruct->type == BPARAM_TYPE_RAW)memcpy(pbridge, pbuffer, pbstruct->typelen);
		else nres = vatek_badparam;
		if (!is_vatek_success(nres))break;
		i++;
	}
	return nres;
}
