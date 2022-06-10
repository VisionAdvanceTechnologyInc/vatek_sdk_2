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

typedef struct _win_thread
{
	cross_thread_param param;
	HANDLE hhandle;
	DWORD threadid;
}win_thread, *Pwin_thread;

hcross_thread cross_os_create_thread(fpcross_thread_function fpfun, void* userparam)
{
	Pwin_thread newthread = (Pwin_thread)malloc(sizeof(win_thread));
	memset(newthread, 0, sizeof(win_thread));
	newthread->param.userparam = userparam;

	newthread->hhandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)fpfun, &newthread->param, 0, &newthread->threadid);
	if (newthread->hhandle != INVALID_HANDLE_VALUE)
		return newthread;

	free(newthread);
	return NULL;
}

vatek_result cross_os_free_thread(hcross_thread hthread)
{
	Pwin_thread winthread = (Pwin_thread)hthread;
	vatek_result nres = cross_os_join_thread(hthread);
	CloseHandle(winthread->hhandle);
	free(winthread);
	return nres;
}

vatek_result cross_os_join_thread(hcross_thread hthread)
{
	Pwin_thread winthread = (Pwin_thread)hthread;
	uint32_t nres = WaitForSingleObject(winthread->hhandle, 1000);
	if (nres == WAIT_OBJECT_0)
		return winthread->param.result;
	else if (nres == WAIT_TIMEOUT)
	{
		cross_os_printf("WaitForSingleObject timeout");
		return vatek_timeout;
	}
	cross_os_printf("WaitForSingleObject unknown : %d", nres);
	return vatek_unknown;
}

vatek_result cross_os_get_thread_result(hcross_thread hthread)
{
	Pwin_thread winthread = (Pwin_thread)hthread;
	return winthread->param.result;
}
