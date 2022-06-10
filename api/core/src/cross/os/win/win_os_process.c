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


#define PROCESS_BUF_LEN	8192
#define PROCESS_END_TAG "cross_process_end_jswu"

typedef struct _win_process_handle
{
	PROCESS_INFORMATION proccessinfo;
	HANDLE hthread;
	HANDLE hpipeout;
	HANDLE hpipein;
	int32_t isrunning;
	DWORD threadid;
	uint8_t rawbuf[PROCESS_BUF_LEN];
	Pcross_proccess_param param;
}win_process_handle,*Pwin_process_handle;

extern DWORD process_thread(LPVOID lpThreadParameter);
extern int32_t process_is_valid(Pwin_process_handle phprocess);

hcross_process cross_os_create_process(Pcross_proccess_param pprocess)
{
	Pwin_process_handle newprocess = (Pwin_process_handle)malloc(sizeof(win_process_handle));
	
	SECURITY_ATTRIBUTES secatt;
	BOOL bsuccess = FALSE;

	memset(newprocess, 0, sizeof(win_process_handle));
	memset(&secatt, 0, sizeof(SECURITY_ATTRIBUTES));

	newprocess->param = pprocess;
	secatt.nLength = sizeof(SECURITY_ATTRIBUTES);
	secatt.bInheritHandle = TRUE;
	secatt.lpSecurityDescriptor = NULL;

	bsuccess = CreatePipe(&newprocess->hpipeout, &newprocess->hpipein, &secatt, 0);
	if (bsuccess)
	{
		bsuccess = SetHandleInformation(newprocess->hpipeout, HANDLE_FLAG_INHERIT, 0);
		if (bsuccess)
		{
			newprocess->isrunning = 1;
			if (bsuccess)newprocess->hthread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)process_thread, newprocess, 0, &newprocess->threadid);
			if (newprocess->hthread != INVALID_HANDLE_VALUE)
			{
				STARTUPINFOA startinfo;
				memset(&startinfo, 0, sizeof(STARTUPINFOA));
				startinfo.cb = sizeof(STARTUPINFO);
				startinfo.hStdError = newprocess->hpipein;
				startinfo.hStdOutput = newprocess->hpipein;
				startinfo.dwFlags = STARTF_USESTDHANDLES;

				bsuccess = CreateProcessA(NULL,
					pprocess->command,
					NULL,
					NULL,
					TRUE,
					CREATE_NO_WINDOW,
					NULL,
					pprocess->path,
					&startinfo,
					&newprocess->proccessinfo);
				if (!bsuccess)cross_os_printf("create fail : %08x", GetLastError());
			}
			else bsuccess = 0;
		}
	}

	if (!bsuccess)
	{
		cross_os_free_process(newprocess);
		return NULL;
	}
	return newprocess;
}

void cross_os_free_process(hcross_process hprocess)
{
	Pwin_process_handle phprocess = (Pwin_process_handle)hprocess;
	if (cross_os_check_process(hprocess))
	{
		BOOL bsuccess = TerminateProcess(phprocess->proccessinfo.hProcess,0);
		if (!bsuccess)cross_os_printf("TerminateProcess fail : %08d", GetLastError());
	}

	if (phprocess->isrunning)
	{
		DWORD rlen = 0;
		while (phprocess->isrunning)
		{
			if (!WriteFile(phprocess->hpipein, (uint8_t*)PROCESS_END_TAG, (uint32_t)strlen(PROCESS_END_TAG), &rlen, NULL))
				cross_os_printf("write end cmd fail");
			cross_os_sleep(10);
		}
	}

	CloseHandle(phprocess->hpipeout);
	CloseHandle(phprocess->hpipein);
	CloseHandle(phprocess->hthread);
	CloseHandle(phprocess->proccessinfo.hProcess);
	CloseHandle(phprocess->proccessinfo.hThread);
	free(phprocess);
}

vatek_result cross_os_check_process(hcross_process hprocess)
{
	Pwin_process_handle phprocess = (Pwin_process_handle)hprocess;
	DWORD ncode = 0;
	if (GetExitCodeProcess(phprocess->proccessinfo.hProcess,&ncode))
	{
		if (ncode == STILL_ACTIVE)return (vatek_result)1;
		else return vatek_success;
	}
	return vatek_unknown;
}

DWORD process_thread(LPVOID lpThreadParameter)
{
	Pwin_process_handle phprocess = (Pwin_process_handle)lpThreadParameter;

	while (phprocess->isrunning)
	{
		DWORD nrlen = 0;
		BOOL bread = ReadFile(phprocess->hpipeout, &phprocess->rawbuf[0], PROCESS_BUF_LEN, &nrlen, NULL);
		if (bread && nrlen)
		{
			char* pend = strstr((char*)&phprocess->rawbuf[0], PROCESS_END_TAG);
			int32_t isend = (pend != NULL);

			if(isend)pend[0] = '\0';
			nrlen = (uint32_t)strlen(&phprocess->rawbuf[0]);
			if(nrlen)phprocess->param->parser(phprocess->param->param, &phprocess->rawbuf[0], nrlen);

			if (isend)break;
		}
	}


	CloseHandle(phprocess->hthread);
	phprocess->hthread = NULL;
	phprocess->threadid = 0;
	phprocess->isrunning = 0;
	return TRUE;
}

int32_t process_is_valid(Pwin_process_handle phprocess)
{
	DWORD ncode;
	if (GetExitCodeProcess(phprocess->proccessinfo.hProcess, &ncode))
	{
		if (ncode == STILL_ACTIVE)return 1;
	}
	return 0;
}
