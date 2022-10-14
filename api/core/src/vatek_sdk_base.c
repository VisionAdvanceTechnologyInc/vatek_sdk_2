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


#include <core/vatek_base.h>
#include "device/internal/cross_device_tool.h"
#include <stdarg.h>
#if _MSC_VER
	#include <windows.h>
#endif

vatek_debug_level m_debug_level = debug_level_warning;

uint32_t vatek_get_tick_ms(void)
{
	return cross_os_get_tick_ms();
}

void vatek_sleep_ms(uint32_t ms)
{
	cross_os_sleep(ms);
}

void vatek_debug_set(vatek_debug_level level)
{
	m_debug_level = level;
}

void vatek_print(const char* fmt, ...)
{
#if _MSC_VER
	char sztmp[256];
	va_list arg;
	va_start(arg, fmt);
	vsprintf(sztmp, fmt, arg);
	va_end(arg);
	OutputDebugStringA(sztmp);
#else
	va_list arg;
	va_start(arg, fmt);
	vprintf(fmt, arg);
	va_end(arg);
#endif
}

void vatek_debug_print(vatek_debug_level level, const char* fmt, ...)
{
	if (level >= m_debug_level)
	{
#if _MSC_VER
		char sztmp[256];
		va_list arg;
		va_start(arg, fmt);
		vsprintf(sztmp, fmt, arg);
		va_end(arg);
		OutputDebugStringA(sztmp);
#else
		va_list arg;
		va_start(arg, fmt);
		vprintf(fmt, arg);
		va_end(arg);
#endif
	}
}

vatek_result vatek_chip_read_register(hvatek_chip hchip, int32_t addr, uint32_t* val)
{
	Pcross_device pcross = ((Pvatek_device)hchip)->cross;
	return pcross->core->read_register(pcross->hcross, addr, val);
}

vatek_result vatek_chip_write_register(hvatek_chip hchip, int32_t addr, uint32_t val)
{
	Pcross_device pcross = ((Pvatek_device)hchip)->cross;
	return pcross->core->write_register(pcross->hcross, addr, val);
}

vatek_result vatek_chip_read_memory(hvatek_chip hchip, int32_t addr, uint32_t* val)
{
	Pcross_device pcross = ((Pvatek_device)hchip)->cross;
	return pcross->core->read_memory(pcross->hcross, addr, val);
}

vatek_result vatek_chip_write_memory(hvatek_chip hchip, int32_t addr, uint32_t val)
{
	Pcross_device pcross = ((Pvatek_device)hchip)->cross;
	return pcross->core->write_memory(pcross->hcross, addr, val);
}

vatek_result vatek_chip_write_buffer(hvatek_chip hchip, int32_t addr, uint8_t* buf, int32_t wlen)
{
	Pcross_device pcross = ((Pvatek_device)hchip)->cross;
	return pcross->core->write_buffer(pcross->hcross, addr, buf, wlen);
}

vatek_result vatek_chip_read_buffer(hvatek_chip hchip, int32_t addr, uint8_t* buf, int32_t wlen)
{
	Pcross_device pcross = ((Pvatek_device)hchip)->cross;
	return pcross->core->read_buffer(pcross->hcross, addr, buf, wlen);
}

vatek_result vatek_version()
{
	return VATEK_VERSION;
}