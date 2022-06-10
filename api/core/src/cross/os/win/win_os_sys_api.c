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


#define WIN_PATH_MAX_LEN	MAX_PATH

typedef struct _win_ffind
{
	HANDLE hfind;
	char path[WIN_PATH_MAX_LEN];
	char full_path[WIN_PATH_MAX_LEN];
	cross_ffind findptr;
	WIN32_FIND_DATAA winfind;
}win_ffind,*Pwin_ffind;

void cross_os_get_current_path(char* path,int32_t buflen)
{
    char* lastslash = NULL;
    GetModuleFileNameA(NULL,path,buflen);

    while (*path != '\0')
    {
        if(*path == '\\')lastslash = path;
        *path++;
    }

    if (lastslash != NULL)
    {
        lastslash++;
        *lastslash = '\0'; 
    }
}

extern void ffind_put_result(Pwin_ffind pff, Pcross_ffind* pfind);

vatek_result cross_os_findfile_first(hcross_ffind* hffind, const char* path, Pcross_ffind* pfind)
{
	Pwin_ffind newffind = (Pwin_ffind)malloc(sizeof(win_ffind));
	vatek_result nres = vatek_memfail;
	if (newffind != NULL)
	{
		char szsearch[WIN_PATH_MAX_LEN];
		nres = vatek_nodevice;
		memset(newffind, 0, sizeof(win_ffind));
		sprintf(&szsearch[0], "%s\\*.*", path);
		strncpy(&newffind->path[0], &szsearch[0], WIN_PATH_MAX_LEN);
		newffind->path[strlen(&newffind->path[0]) - 3] = '\0';
		

		newffind->hfind = FindFirstFileA(&szsearch[0], &newffind->winfind);
		if (newffind->hfind != INVALID_HANDLE_VALUE)
		{
			nres = vatek_success;
			if (newffind->winfind.cFileName[0] == '.')
				nres = cross_os_findfile_next((hcross_ffind)newffind, pfind);
			else ffind_put_result(newffind, pfind);

			if (is_vatek_success(nres))*hffind = newffind;
			else FindClose(newffind->hfind);
		}

		if (!is_vatek_success(nres))free(newffind);
	}
	return nres;
}

vatek_result cross_os_findfile_next(hcross_ffind hffind, Pcross_ffind* pfind)
{
	vatek_result nres = vatek_success;
	Pwin_ffind pff = (Pwin_ffind)hffind;

	do 
	{
		nres = FindNextFileA(pff->hfind, &pff->winfind);
		if (!nres)
		{
			nres = vatek_nodevice;
			break;
		}
	} while (pff->winfind.cFileName[0] == '.');

	if (is_vatek_success(nres))ffind_put_result(pff, pfind);
	return nres;
}

void cross_os_findfile_close(hcross_ffind hffind)
{
	Pwin_ffind pffind = (Pwin_ffind)hffind;
	FindClose(pffind->hfind);
	free(pffind);
}

hcross_dll cross_os_dll_load(const char* dllfile)
{
    return LoadLibraryA(dllfile);
}

void* cross_os_dll_get_function(hcross_dll hdll, const char* name)
{
    return GetProcAddress(hdll,name);
}

vatek_result cross_os_dll_free(hcross_dll hdll)
{
    if(hdll != NULL)FreeLibrary(hdll);
    return vatek_success;
}

vatek_result cross_os_dll_valid(const char* name)
{
	int32_t len = (int32_t)strlen(name);
	if (strcmp(".dll", &name[len - 4]) == 0)return vatek_success;
	return vatek_badparam;
}

void ffind_put_result(Pwin_ffind pff, Pcross_ffind* pfind)
{
	sprintf(&pff->full_path[0], "%s%s", &pff->path[0], &pff->winfind.cFileName[0]);
	pff->findptr.fullpath = &pff->full_path[0];
	pff->findptr.filename = &pff->full_path[strlen(&pff->path[0])];
	pff->findptr.ff_type = 0;
	if (pff->winfind.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
		pff->findptr.ff_type |= FF_TYPE_FOLDER;
	*pfind = &pff->findptr;
}
