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

#include "internal/win_os_common.h"
#include <stdio.h>

#define _smem_name_tag	"Global\\%s"

typedef struct _win_smem
{
	char sztag[64];
	int32_t is_create;
	int32_t len;
	uint8_t* ptr;
	HANDLE hsmem;
}win_smem,*Pwin_smem;

vatek_result cross_os_create_smem(const char* tag, hcross_smem* hsmem, int32_t size)
{
	vatek_result nres = vatek_memfail;
	Pwin_smem newsmem = (Pwin_smem)malloc(sizeof(win_smem));
	if (newsmem)
	{
		memset(newsmem, 0, sizeof(win_smem));
		sprintf(&newsmem->sztag[0], _smem_name_tag, tag);
		newsmem->hsmem = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, &newsmem->sztag[0]);
		nres = vatek_hwfail;
		if (newsmem->hsmem != INVALID_HANDLE_VALUE)
		{
			newsmem->ptr = MapViewOfFile(newsmem->hsmem, FILE_MAP_ALL_ACCESS, 0, 0,size);
			if (newsmem->ptr != NULL)
			{
				newsmem->is_create = 1;
				newsmem->len = size;
				*hsmem = newsmem;
				nres = vatek_success;
			}
		}
		else win_get_last_error();

		if (!is_vatek_success(nres))free(newsmem);
	}

	return nres;
}

vatek_result cross_os_open_smem(const char* tag, hcross_smem* hsmem,int32_t size)
{
	vatek_result nres = vatek_memfail;
	Pwin_smem newsmem = (Pwin_smem)malloc(sizeof(win_smem));
	if (newsmem)
	{
		memset(newsmem, 0, sizeof(win_smem));
		sprintf(&newsmem->sztag[0], _smem_name_tag, tag);
		newsmem->hsmem = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, &newsmem->sztag[0]);
		nres = vatek_hwfail;
		if (newsmem->hsmem != INVALID_HANDLE_VALUE)
		{
			newsmem->ptr = MapViewOfFile(newsmem->hsmem, FILE_MAP_ALL_ACCESS, 0, 0, size);
			if (newsmem->ptr != NULL)
			{
				newsmem->is_create = 1;
				newsmem->len = size;
				*hsmem = newsmem;
				nres = vatek_success;
			}
		}
		else win_get_last_error();

		if (!is_vatek_success(nres))free(newsmem);
	}

	return nres;
}

uint8_t* cross_os_get_smem(hcross_smem hsmem)
{
	Pwin_smem psmem = (Pwin_smem)hsmem;
	return psmem->ptr;
}


vatek_result cross_os_close_smem(hcross_smem hsmem)
{
	Pwin_smem psmem = (Pwin_smem)hsmem;

	if (!UnmapViewOfFile(psmem->ptr))win_get_last_error();
	CloseHandle(psmem->hsmem);
	free(psmem);

	return vatek_success;
}