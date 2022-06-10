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

#ifndef _SPEC_ARIB_
#define _SPEC_ARIB_

#include <mux/mux_define.h>

#define ARIBSTR_NETWORKNAME_LEN		20
#define ARIBSTR_SERVICE_NAME_LEN	20
#define ARIBSTR_TS_NAME_LEN			20

/**
 * @ingroup mspec_arib
 * Copy Protected Mode 
 */
typedef enum _arib_abnt_copy{
	arib_abnt_free = 0,
	arib_abnt_copyonce = 1,
	arib_abnt_forbidden = 2,
}arib_abnt_copy;

#define _AC(ch)	(ch + 0x80)
#define _AC_ASCII(ch)	(ch - 0x80)

static const uint8_t _arib_network_name[] = { 0x1b,0x7E,_AC('V'),_AC('A') ,_AC('T'),_AC('E'),_AC('K'),0x00,0x00, };
static const uint8_t _arib_service_name[] = { 0x1b,0x7E,_AC('V'),_AC('A') ,_AC('-'),_AC('T'),_AC('V'),0x00,0x00, };
static const uint8_t _arib_ts_name[] = { 0x1b,0x7E,_AC('V'),_AC('A') ,_AC('-'),_AC('T'),_AC('V'),0x00,0x00, };

static const mux_string arib_string_network_name = 
{ 
	sizeof(_arib_network_name) - 2,
	(uint8_t*)&_arib_network_name[0],
	ARIBSTR_NETWORKNAME_LEN,
};

static const mux_string arib_string_service_name = 
{ 
	sizeof(_arib_service_name) - 2,
	(uint8_t*)&_arib_service_name[0],
	ARIBSTR_SERVICE_NAME_LEN,
};

static const mux_string arib_string_ts_name = 
{ 
	sizeof(_arib_ts_name) - 2,
	(uint8_t*)&_arib_ts_name[0],
	ARIBSTR_TS_NAME_LEN,
};

/**
 * @ingroup mprop_arib
 * ARIB PSI Channel Properties 
 */
typedef struct _mux_spec_arib_channel{
	uint8_t region_id;					/*!< region area id [1 - 63] */
	uint8_t broadcaster_id;				/*!< broadcast studio id [0 - 15] */
	uint8_t remote_control_key_id;		/*!< remote control ket [1 - 12] */
	uint8_t recv;
	Pmux_string network_name;			/*!< network name */
}mux_spec_arib_channel;

typedef mux_spec_arib_channel* Pmux_spec_arib_channel;

/**
 * @ingroup mprop_arib
 * ARIB PSI Channel Properties 
 */
typedef struct _mux_spec_arib_program {
	uint8_t service_no;				/*!< service no [1 - 7] */
	uint8_t recv[3];
	mux_iso_lang main_lang;			/*!< language main */
	mux_iso_lang sub_lang;			/*!< language sub */
	arib_abnt_copy copy_flag;		/*!< copy mode */
	Pmux_string service_name;		/*!< service name */
	Pmux_string ts_name;			/*!< ts information name */
}mux_spec_arib_program;

typedef mux_spec_arib_program* Pmux_spec_arib_program;

static const mux_spec_arib_channel default_mux_spec_arib_channel =
{
	23,
	1,
	1,
	0,
	(Pmux_string)&arib_string_network_name,
};

static const mux_spec_arib_program default_mux_spec_arib_program =
{
	1,
	{0,0,0,},
#ifdef __cplusplus
	_ISOLANG_JPN,
	_ISOLANG_ENG,
#else
	{.raw = _ISOLANG_JPN,},
	{.raw = _ISOLANG_ENG,},
#endif
	arib_abnt_free,
	(Pmux_string)&arib_string_service_name,
	(Pmux_string)&arib_string_ts_name,
};

/**
 * @ingroup mprop_abnt
 * ABNT PSI Channel Properties 
 */
typedef struct _mux_spec_abnt_channel {
	uint8_t remote_control_key_id;	/*!< remote control key [1 - 12] */
	uint8_t recv_0;
	uint16_t original_network_id;	/*!< original network id (on_id) */
	uint16_t area_code;				/*!< abnt area code [1 - 99] */
	uint16_t recv_1;
	Pmux_string network_name;		/*!< network name */
}mux_spec_abnt_channel;

typedef mux_spec_abnt_channel* Pmux_spec_abnt_channel;

/**
 * @ingroup mprop_abnt
 * ABNT PSI Program Properties 
 */
typedef struct _mux_spec_abnt_program {
	uint8_t service_no;				/*!< service no [1 - 7] */
	uint8_t recv[3];
	mux_iso_lang main_lang;			/*!< language main */
	mux_iso_lang sub_lang;			/*!< language sub	*/
	arib_abnt_copy copy_flag;		/*!< copy flag */
	Pmux_string service_name;		/*!< service name */
	Pmux_string ts_name;			/*!< ts information name */
}mux_spec_abnt_program;

typedef mux_spec_abnt_program* Pmux_spec_abnt_program;

static const mux_spec_abnt_channel default_mux_spec_abnt_channel =
{
	1,
	0,
	1,
	0,
	0,
	(Pmux_string)&arib_string_network_name,
};

static const mux_spec_abnt_program default_mux_spec_abnt_program =
{
	1,
	{0,0,0,},
#ifdef __cplusplus
	_ISOLANG_JPN,
	_ISOLANG_ENG,
#else
	{.raw = _ISOLANG_JPN,},
	{.raw = _ISOLANG_ENG,},
#endif
	arib_abnt_free,
	(Pmux_string)&arib_string_service_name,
	(Pmux_string)&arib_string_ts_name,
};

#endif
