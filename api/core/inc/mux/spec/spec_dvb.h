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

#ifndef _SPEC_DVB_
#define _SPEC_DVB_

#include <mux/mux_define.h>

#define DVBSTR_NETWORK_NAME_LEN	16
#define DVBSTR_SERVICE_NAME_LEN	16

static const uint8_t network_name[6] = { 'V','A','T','E','K','\0', };
static const mux_string def_network_name = { 5,(uint8_t*)&network_name[0],DVBSTR_NETWORK_NAME_LEN };

static const uint8_t service_name[6] = { 'V','A','-','T','V','\0', };
static const mux_string def_service_name = { 5,(uint8_t*)&service_name[0],DVBSTR_SERVICE_NAME_LEN };

/**
 * @ingroup mprop_dvb
 * DVB PSI Channel Properties
 */
typedef struct _mux_spec_dvb_channel{
	uint16_t transport_stream_id;	/*!< transport_stream_id */
	uint16_t network_id;			/*!< network_id */
	Pmux_string network_name;		/*!< network_name */
}mux_spec_dvb_channel;

typedef mux_spec_dvb_channel* Pmux_spec_dvb_channel;

/**
 * @ingroup mprop_dvb
 * DVB PSI Program Properties
 */
typedef struct _mux_spec_dvb_program{
	uint16_t original_network_id;	/*!< original_network_id */
	uint16_t program_number;		/*!< program_number */
	uint16_t channel_no;			/*!< channel_no */
	uint16_t recv;
	Pmux_string service_name;		/*!< service_name */
}mux_spec_dvb_program;

typedef mux_spec_dvb_program* Pmux_spec_dvb_program;

static const mux_spec_dvb_channel default_mux_spec_dvb_channel = { 1,1,(Pmux_string)&def_network_name };
static const mux_spec_dvb_program default_mux_spec_dvb_program = { 1,1,1,0,(Pmux_string)&def_service_name };

#endif
