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

#define TIME_SECOND_TO_NS		1000000000
#define TIME_SECOND_TO_US		1000000

void cross_os_sleep(int32_t ms)
{
	cross_os_usleep((ms * 1000) + 1);
}

uint32_t cross_os_get_tick_ms(void)
{
	return (uint32_t)GetTickCount64();
}

void cross_os_sleep_yield()
{
	timeBeginPeriod(1);
	Sleep(0);
	timeEndPeriod(1);
}

#pragma comment(lib,"Winmm.lib") 

static int32_t qf_init = 0;
static LARGE_INTEGER qf_freq;
static long qf_nano_tick;

void convert_tick_to_timespec(LARGE_INTEGER tick, struct timespec *tv)
{
	tv->tv_sec = tick.QuadPart / qf_freq.QuadPart;
	tv->tv_nsec = (long)(tick.QuadPart % qf_freq.QuadPart) * qf_nano_tick;
}

void init_queryperformance()
{
	if (!qf_init)
	{
		QueryPerformanceFrequency(&qf_freq);
		qf_nano_tick = (long)(TIME_SECOND_TO_NS / qf_freq.QuadPart);
		qf_init = 1;
	}
}

void cross_os_usleep(uint32_t us)
{
	LARGE_INTEGER start, end;
	LONGLONG count;
	int32_t ms = 0;

	if (us > 1000)
	{
		ms = us / 1000;
		us = us % 1000;
	}

	init_queryperformance();
	QueryPerformanceCounter(&start);
	count = (us * qf_freq.QuadPart) / 1000000;
	count = count + start.QuadPart;

	if (ms > 0)
	{
		timeBeginPeriod(1);
		Sleep(ms);
		timeEndPeriod(1);
	}

	do
	{
		QueryPerformanceCounter(&end);
	} while (end.QuadPart < count);
}

vatek_result cross_os_get_time(struct timespec * tp)
{
	LARGE_INTEGER curtime;
	init_queryperformance();
	QueryPerformanceCounter(&curtime);
	convert_tick_to_timespec(curtime, tp);
	return 0;
}

uint64_t cross_os_get_time_us()
{
	LARGE_INTEGER curtime;
	init_queryperformance();
	QueryPerformanceCounter(&curtime);
	return ((uint64_t)(curtime.QuadPart * TIME_SECOND_TO_US) / qf_freq.QuadPart);
}

uint64_t cross_os_time_to_us(struct timespec* tp)
{
	return (tp->tv_sec * 1000000) + (tp->tv_nsec / 1000);
}

vatek_result cross_os_time_eclipse(struct timespec* st, struct timespec* eclipse)
{
	struct timespec curtime;
	vatek_result nres = cross_os_get_time(&curtime);
	if (is_vatek_success(nres))
	{
		if (curtime.tv_nsec > st->tv_nsec)
		{
			eclipse->tv_nsec = st->tv_nsec + (1000000000UL - curtime.tv_nsec);
			eclipse->tv_sec = (curtime.tv_sec - st->tv_sec) - 1;
		}
		else
		{
			eclipse->tv_nsec = curtime.tv_nsec - st->tv_nsec;
			eclipse->tv_sec = curtime.tv_sec - st->tv_sec;
		}
	}
	return nres;
}

void cross_os_wait_unit(struct timespec* target)
{
	uint64_t curtick = cross_os_get_time_us();
	uint64_t timetick = cross_os_time_to_us(target);

	if (curtick > timetick)return;
	curtick = timetick - curtick;
	cross_os_usleep((uint32_t)curtick);
}

void cross_os_time_plus_ms(struct timespec* tp, int32_t ms)
{
	if (ms >= 1000)
	{
		tp->tv_sec += ms / 1000;
		ms %= 1000;
	}

	tp->tv_nsec += ms * 1000;
	if (tp->tv_nsec >= TIME_SECOND_TO_NS)
	{
		tp->tv_sec++;
		tp->tv_nsec -= TIME_SECOND_TO_NS;
	}
}

void cross_os_time_plus_time(struct timespec* tp, struct timespec* tappend)
{
	tp->tv_nsec += tappend->tv_nsec;
	tp->tv_sec += tappend->tv_sec;

	if (tp->tv_nsec > TIME_SECOND_TO_NS)
	{
		tp->tv_sec++;
		tp->tv_nsec -= TIME_SECOND_TO_NS;
	}
}
