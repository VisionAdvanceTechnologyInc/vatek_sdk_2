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

#ifndef _UI_TOOL_COUNTRY_
#define _UI_TOOL_COUNTRY_

#include <mux/mux_define.h>

#if 0

typedef struct _mux_ui_country
{
	const char* name;
	mux_country_code country;
}mux_ui_country, *Pmux_ui_country;

#define _country_table_start			static const mux_ui_country countr_ui_all[] = {
#define _country_table_end				{NULL,mux_country_undefined,},};
#define _country_name(name,code)		{#name,code,},
#define _country(name)					{#name,name,},
#define _country_dvb(name)				{#name,dvb_##name,},
#define _country_nordig(name)			{#name,nordig_##name,},
#define _country_dtmb(name)				{#name,dtmb_##name,},
#define _country_arib(name)				{#name,arib_##name,},
#define _country_abnt(name)				{#name,abnt_##name,},
#define _country_atsc(name)				{#name,atsc_##name,},

_country_table_start
	_country_name(undefined, mux_country_undefined)
	_country_dvb(taiwan)
	_country_dvb(asean)
	_country_dvb(uk)
	_country_dvb(spanish)
	_country_dvb(france)
	_country_dvb(italian)
	_country_dvb(colombia)
	_country_dvb(india)
	_country_dvb(ghana)
	_country_dvb(singapore)
	_country_dvb(georgin)
	_country_nordig(finland)
	_country_dtmb(china)
	_country_dtmb(hk)
	_country_dtmb(cuba)
	_country_arib(japan)
	_country_abnt(brazil)
	_country_abnt(argentina)
	_country_abnt(bolivia)
	_country_abnt(botswana)
	_country_abnt(costa_rica)
	_country_abnt(chile)
	_country_abnt(ecuador)
	_country_abnt(paraguay)
	_country_abnt(philippines)
	_country_abnt(peru)
	_country_abnt(uruguay)
	_country_abnt(venezuela)
	_country_atsc(usa)
	_country_atsc(korean)
	_country_atsc(canada)
	_country_atsc(mexico)
	_country_atsc(dominican)
_country_table_end

#define _country_table			countr_ui_all

#endif

#endif

