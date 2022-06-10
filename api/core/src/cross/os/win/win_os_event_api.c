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

vatek_result cross_os_create_event(const char* tag, hcross_event* hevent)
{
	HANDLE newevent = INVALID_HANDLE_VALUE;
	//newevent = CreateEventA(NULL, FALSE, FALSE, win_get_global_name(tag));
	newevent = CreateEventA(NULL, FALSE, FALSE,tag);
	if (newevent != INVALID_HANDLE_VALUE)
	{
		*hevent = newevent;
		return vatek_success;
	}
	else win_get_last_error();
	return vatek_memfail;
}

vatek_result cross_os_open_event(const char* tag, hcross_event* hevent)
{
	HANDLE newevent = INVALID_HANDLE_VALUE;
	//newevent = OpenEventA(0, FALSE, win_get_global_name(tag));
	newevent = OpenEventA(0, FALSE, tag);
	if (newevent != INVALID_HANDLE_VALUE)
	{
		*hevent = newevent;
		return vatek_success;
	}
	else win_get_last_error();
	return vatek_memfail;
}

vatek_result cross_os_wait_event_timeout(hcross_event hevent, int32_t ms)
{
	uint32_t ncode = WaitForSingleObject((HANDLE)hevent, ms);
	if (ncode == WAIT_OBJECT_0)return vatek_success;
	else return vatek_badstatus;
}

vatek_result cross_os_wait_event(hcross_event hevent)
{
	uint32_t ncode = WaitForSingleObject((HANDLE)hevent, INFINITE);
	if(ncode == WAIT_OBJECT_0)return vatek_success;
	else
	{
		cross_os_printf("wait event fail : %08x", ncode);
		return vatek_badstatus;
	}
}

vatek_result cross_os_raise_event(hcross_event hevent)
{
	if (SetEvent((HANDLE)hevent))return vatek_success;
	else return vatek_unsupport;
}

void cross_os_free_event(hcross_event hevent)
{
	CloseHandle(hevent);
}
