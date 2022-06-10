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

#include <core/tools/tool_binary_crc.h>
#include <core/tools/tool_crc32.h>

vatek_result tool_binary_reset_crc(uint32_t* crc)
{
	tool_crc32_step_reset(crc);
	return vatek_success;
}

vatek_result tool_binary_crc_sector(int32_t idx, uint8_t* pbuf, uint32_t* crc)
{
	vatek_result nres = vatek_success;

	if(idx == 0)memset(&pbuf[0], 0, TOOL_BINARY_CRC_HEADER_OFFSET);
	nres = tool_crc32_step_put_buffer(crc, pbuf, BINARY_SECTION_SIZE);
	return nres;
}

static uint8_t binary_app_crc_sections[] = { 0,1,16,17, };

#define APP_CRC_SECTION_NUMS	sizeof(binary_app_crc_sections)

vatek_result tool_binary_app_get_crc(uint32_t* crc, fpget_section fpget,void* param,uint8_t* psection)
{
	int32_t i = 0;
	vatek_result nres = vatek_success;

	tool_crc32_step_reset(crc);

	for (i = 0; i < APP_CRC_SECTION_NUMS; i++)
	{
		nres = (vatek_result)fpget(param,binary_app_crc_sections[i],psection);
		if (is_vatek_success(nres))
			nres = tool_binary_crc_sector(i, psection, crc);
		if (!is_vatek_success(nres))break;
	}
	return nres;
}
