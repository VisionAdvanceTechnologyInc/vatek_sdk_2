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

#ifndef _SPEC_PSIP_
#define _SPEC_PSIP_

#include <mux/mux_define.h>

#define PSIPSTR_SHORT_NAME_LEN	14
#define PSIPSTR_LONG_NAME_LEN	32

/**
 * @ingroup mprop_psip
 * PSIP CC Mode 
 */
typedef enum _psip_cc_mode{
	cc_mode_disable = 0,
	cc_mode_608 = 1,
}psip_cc_mode;

/* short name _unicode		*/
static const uint8_t _psip_short_name[] = { 0x00,'V',0x00,'A',0x00,'T',0x00,'E',0x00,'K',0x00,0x00, };
/* A65 _mutil-string 6.10	*/
static const uint8_t _psip_long_name[] = { 0x01,'e','n','g',0x01,0x00,0x00,0x04,'V','A','T','V',0x00,0x00,0x00,0x00 };

static const mux_string _psip_string_short_name = 
{ 
	sizeof(_psip_short_name) - 2,
	(uint8_t*)&_psip_short_name[0],
	PSIPSTR_SHORT_NAME_LEN,
};

static const mux_string _psip_string_long_name = 
{ 
	sizeof(_psip_long_name) - 4,
	(uint8_t*)&_psip_long_name[0],
	PSIPSTR_LONG_NAME_LEN,
};

/**
 * @ingroup mprop_psip
 * PSIP Channel Properties
 */
typedef struct _mux_spec_psip_channel {
	uint32_t psip_flags;			/*!< psip function flags */
	uint16_t transport_stream_id;	/*!< psip ts id */
	uint8_t gps_utc_offset;			/*!< time gps_utc offect */
	uint8_t recv_0;
	uint16_t daylight_saving;		/*!< daylight saving */
	uint16_t recv_1;
	psip_cc_mode cc_mode;			/*!< cc mode */
	Pmux_string short_name;		/*!< short name 7*2 */
}mux_spec_psip_channel;

typedef mux_spec_psip_channel* Pmux_spec_psip_channel;

/**
 * @ingroup mprop_psip
 * PSIP Program Properties
 */
typedef struct _mux_spec_psip_program {
	uint16_t program_number;		/*!< program number */
	uint16_t source_id;				/*!< source id */
	uint16_t channel_major;			/*!< major id */
	uint16_t channel_minor;			/*!< minor id */
	Pmux_string long_name;		/*!< extend channel name */
}mux_spec_psip_program;

typedef mux_spec_psip_program* Pmux_spec_psip_program;

static const mux_spec_psip_channel default_mux_spec_psip_channel = { 0,1,0,0,0,0,cc_mode_608,(Pmux_string)&_psip_string_short_name, };
static const mux_spec_psip_program default_mux_spec_psip_program = { 1,1,31,1,(Pmux_string)&_psip_string_long_name, };

#endif
