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

vatek_result cross_os_create_mutex_name(const char* tag, hcross_mutex* hmutex)
{
	HANDLE newmux = INVALID_HANDLE_VALUE;
	vatek_result nres = vatek_memfail;

	newmux = CreateMutexA(NULL, FALSE,win_get_global_name(tag));
	if (newmux != INVALID_HANDLE_VALUE)
	{
		*hmutex = newmux;
		nres = vatek_success;
	}
	else win_get_last_error();
	return nres;
}

vatek_result cross_os_open_mutex_name(const char* tag, hcross_mutex* hmuxtex)
{
	HANDLE newmux = INVALID_HANDLE_VALUE;
	newmux = OpenMutexA(0, FALSE, win_get_global_name(tag));
	if (newmux != INVALID_HANDLE_VALUE)
	{
		*hmuxtex = newmux;
		return vatek_success;
	}
	else win_get_last_error();
	return vatek_memfail;
}

vatek_result cross_os_create_mutex(hcross_mutex* hmutex)
{
	HANDLE newmutex = CreateMutexA(NULL, FALSE, NULL);
	vatek_result nres = vatek_memfail;
	if (newmutex != INVALID_HANDLE_VALUE)
	{
		*hmutex = newmutex;
		nres = vatek_success;
	}
	else win_get_last_error();
	return nres;
}

void cross_os_lock_mutex(hcross_mutex hmutex)
{
	HANDLE hmux = (HANDLE)hmutex;
	uint32_t nres = WaitForSingleObject(hmux, INFINITE);

	if (nres != WAIT_OBJECT_0)
		cross_os_printf("lock mutex fail : 0x%08x", hmutex);
}

vatek_result cross_os_lock_mutex_timeout(hcross_mutex hmutex, uint32_t ms)
{
	HANDLE hmux = (HANDLE)hmutex;
	uint32_t nres = WaitForSingleObject(hmux, ms);

	if (nres == WAIT_OBJECT_0)return vatek_success;
	else if (nres == WAIT_TIMEOUT)return vatek_timeout;
	else
	{
		cross_os_printf("lock mutex fail : 0x%08x", nres);
		return vatek_unknown;
	}
}

vatek_result cross_os_trylock_mutex(hcross_mutex hmutex)
{
	HANDLE hmux = (HANDLE)hmutex;
	uint32_t nres = WaitForSingleObject(hmux, 0);

	if (nres == WAIT_OBJECT_0)return vatek_success;
	else if (nres == WAIT_TIMEOUT)return vatek_timeout;
	return vatek_unknown;
}

void cross_os_release_mutex(hcross_mutex hmutex)
{
	if (!ReleaseMutex((HANDLE)hmutex))win_get_last_error();
}

vatek_result cross_os_free_mutex(hcross_mutex hmutex)
{
	CloseHandle((HANDLE)hmutex);
	return vatek_success;
}
