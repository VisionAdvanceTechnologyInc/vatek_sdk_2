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

#include <core/base/device_usb.h>
#include <core/tools/tool_bufstream.h>

const char usbbulk_raw_tag[USBBULK_TAG_LEN] = { 'V','A','T','E','K','-','2','\0', };

int32_t usbbulk_buffer_check(uint8_t* pbuf)
{
	return !strcmp((const char*)&pbuf[0], &usbbulk_raw_tag[0]);
}

vatek_result usbbulk_command_set(Pusbbulk_command pcmd, uint8_t* pbuf)
{
	vatek_result nres = vatek_success;
	if ((pcmd->mode & USBBULK_CMD_MASK) == USBBULK_CMD_BASE)
	{
		bufstream bs = { USBBULK_TAG_LEN, pbuf };
		memcpy(&pbuf[0], &usbbulk_raw_tag[0], USBBULK_TAG_LEN);
		putbuf_uint32_t(&bs, (uint32_t)pcmd->mode);
		if (pcmd->mode == usbbulk_flash)
		{
			putbuf_uint32_t(&bs, (uint32_t)pcmd->_h.flash.mode);
			putbuf_uint32_t(&bs, pcmd->_h.flash.section);
		}
		else if (pcmd->mode == usbbulk_aux)
		{
			putbuf_uint32_t(&bs, (uint32_t)pcmd->_h.aux.mode);
			putbuf_uint32_t(&bs, pcmd->_h.aux.length);
			putbuf_uint32_t(&bs, pcmd->_h.aux.bitrate);
		}
		else nres = vatek_badparam;
	}
	else nres = vatek_badparam;
	return nres;
}

vatek_result usbbulk_command_get(Pusbbulk_command pcmd, uint8_t* pbuf)
{
	vatek_result nres = vatek_format;
	if (usbbulk_buffer_check(pbuf))
	{
		bufstream bs = { USBBULK_TAG_LEN,pbuf };
		uint32_t mode = getbuf_uint32_t(&bs);
		if ((mode & USBBULK_CMD_MASK) == USBBULK_CMD_BASE)
		{
			nres = vatek_success;
			pcmd->mode = (usbbulk_mode)mode;
			if (pcmd->mode == usbbulk_flash)
			{
				pcmd->_h.flash.mode = (usbflash_mode)getbuf_uint32_t(&bs);
				pcmd->_h.flash.section = getbuf_uint32_t(&bs);
			}
			else if (pcmd->mode == usbbulk_aux)
			{
				pcmd->_h.aux.mode = (usbaux_mode)getbuf_uint32_t(&bs);
				pcmd->_h.aux.length = getbuf_uint32_t(&bs);
				pcmd->_h.aux.bitrate = getbuf_uint32_t(&bs);
			}
			else nres = vatek_badparam;
		}
		else nres = vatek_badparam;
	}
	return nres;
}

vatek_result usbbulk_result_set(Pusbbulk_result presult, uint8_t* pbuf)
{
	vatek_result nres = vatek_success;
	if ((presult->mode & USBBULK_CMD_MASK) == USBBULK_CMD_BASE)
	{
		bufstream bs = { USBBULK_TAG_LEN,pbuf };
		memcpy(&pbuf[0], &usbbulk_raw_tag[0], USBBULK_TAG_LEN);
		putbuf_uint32_t(&bs, (uint32_t)presult->mode);
		putbuf_uint32_t(&bs, (uint32_t)presult->result);
		putbuf_uint32_t(&bs, presult->len);
		putbuf_uint32_t(&bs, presult->position);
	}
	else nres = vatek_badparam;
	return nres;
}

vatek_result usbbulk_result_get(Pusbbulk_result presult, uint8_t* pbuf)
{
	vatek_result nres = vatek_format;
	if (usbbulk_buffer_check(pbuf))
	{
		bufstream bs = { USBBULK_TAG_LEN,pbuf };
		uint32_t mode = getbuf_uint32_t(&bs);
		if ((mode & USBBULK_CMD_MASK) == USBBULK_CMD_BASE)
		{
			presult->mode = (usbbulk_mode)mode;
			presult->result = (vatek_result)getbuf_uint32_t(&bs);
			presult->len = getbuf_uint32_t(&bs);
			presult->position = getbuf_uint32_t(&bs);
			nres = vatek_success;
		}
		else nres = vatek_badparam;
	}
	return nres;
}
