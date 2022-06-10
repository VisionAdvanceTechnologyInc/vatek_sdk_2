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

#ifndef _MUX_TIME_
#define _MUX_TIME_

#include <mux/mux_define.h>

typedef struct _mux_time{
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t recv;
}mux_time;

typedef mux_time* Pmux_time;

#define MUXTIME_DATA_TO_UINT(time)	((time.year << 16) | (time.month << 8) | (time.day))
#define MUXTIME_TIME_TO_UINT(time)	((time.hour << 16) | (time.minute << 8) | (time.second))

#ifdef __cplusplus
extern "C" {
#endif

	HAL_API void muxtime_append_hours(Pmux_time ptime, int32_t hours);
	HAL_API void muxtime_append_days(Pmux_time ptime, int32_t days);
	HAL_API uint32_t muxtime_eclipse_hour(Pmux_time pstart, Pmux_time pcur);
	HAL_API uint8_t muxtime_get_month_days(Pmux_time ptime);
	HAL_API int32_t muxtime_is_leap_year(Pmux_time ptime);
	HAL_API uint32_t muxtime_date_to_reg(Pmux_time ptime);
	HAL_API uint32_t muxtime_clock_to_reg(Pmux_time ptime);
	HAL_API void muxtime_date_from_reg(Pmux_time ptime, uint32_t reg);
	HAL_API void muxtime_clock_from_reg(Pmux_time ptime, uint32_t reg);

#ifdef __cplusplus
}
#endif

#endif