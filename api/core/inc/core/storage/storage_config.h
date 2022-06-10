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

#ifndef _STORAGE_CONFIG_
#define _STORAGE_CONFIG_

#include <core/base/binary_rom.h>

#define BIN_SECTION_CONFIG		(BIN_SECTION_TAG | 0x40)

#define CONFIG_CHIP_BASE_LEN		256
#define CONFIG_MOD_SRRC_LEN			(64 * sizeof(uint32_t))

#define CONFIG_MODSRRC_DVB_T		MOD_DVB_T
#define CONFIG_MODSRRC_J83A			MOD_J83A
#define CONFIG_MODSRRC_ATSC			MOD_ATSC
#define CONFIG_MODSRRC_J83BQ64		MOD_J83B
#define CONFIG_MODSRRC_DTMB			MOD_DTMB
#define CONFIG_MODSRRC_ISDB_T		MOD_ISDB_T
#define CONFIG_MODSRRC_J83C			MOD_J83C
#define CONFIG_MODSRRC_DVB_T2		MOD_DVB_T2
#define CONFIG_MODSRRC_J83BQ256		(CONFIG_MODSRRC_DVB_T2 + 1)

#define CONFIG_MODSRRC_NUMS			9

#define CONFIG_CHIPIDX_B2			0
#define CONFIG_CHIPIDX_B2_plus		1
#define CONFIG_CHIPIDX_B3			2
#define CONFIG_CHIPIDX_A3			3
#define CONFIG_CHIPIDX_NUMS			4

/**
 * @brief mod_config
 * atsc hardware config param
 */

typedef struct _atsc_config {
	uint32_t dc_offect;         /* [7:0] */
	uint32_t dac_gain;
}atsc_config;

typedef atsc_config* Patsc_config;

typedef struct _j83a_config
{
	uint32_t gain[6];
}j83a_config;

typedef j83a_config* Pj83a_config;

/**
 * @brief mod_config
 * j83b hardware config param
 */

typedef struct _j83b_config
{
	uint32_t gain_q64;
	uint32_t gain_q256;
}j83b_config;

typedef j83b_config* Pj83b_config;

/**
 * @brief mod_config
 * dvb-t hardware config param
 */

typedef struct _dvb_t_config
{
	uint32_t scl_0;
	uint32_t scl_1;
	uint32_t scl_2;
	uint32_t fft_scale[3];
	uint32_t gain[3];
}dvb_t_config;

typedef dvb_t_config* Pdvb_t_config;

typedef struct _isdb_t_config
{
	uint32_t scl_0;
	uint32_t scl_1;
	uint32_t scl_2;
	uint32_t fft_scale[3];
	uint32_t gain[3];
}isdb_t_config;

typedef isdb_t_config* Pisdb_t_config;

#define DTMB_IDX_NUMS       2

typedef struct _dtmb_config
{
	uint32_t pn_mag[DTMB_IDX_NUMS];
	uint32_t ofdm_mag[DTMB_IDX_NUMS][6];
	uint32_t tps_scale[6];
	uint32_t fft_scale[6];
	uint32_t gain[DTMB_IDX_NUMS][6];
}dtmb_config;

typedef dtmb_config* Pdtmb_config;

typedef struct _dvb_t2_config
{
	uint32_t gain[6];
	uint32_t fftscale[6];
}dvb_t2_config;

typedef dvb_t2_config* Pdvb_t2_config;

typedef struct _modulation_config
{
	dvb_t_config dvb_t;
	j83a_config j83a;
	atsc_config atsc;
	j83b_config j83b;
	dtmb_config dtmb;
	isdb_t_config isdb_t;
	dvb_t2_config dvb_t2;
}modulation_config;

typedef modulation_config* Pmodulation_config;

typedef struct _storage_chip_config
{
	uint32_t tag;
	union
	{
		chip_config config;
		uint8_t raw[CONFIG_CHIP_BASE_LEN];
	}_chip;
	uint8_t srrc[CONFIG_MODSRRC_NUMS][CONFIG_MOD_SRRC_LEN];
	modulation_config modulation[CONFIG_CHIPIDX_NUMS];
}storage_chip_config;

typedef storage_chip_config* Pstorage_chip_config;

#ifdef __cplusplus
extern "C" {
#endif

	HAL_API void storage_chip_config_reset(Pstorage_chip_config pcfg);
	HAL_API vatek_result storage_chip_config_set(Pstorage_chip_config pcfg, uint8_t* psection);
	HAL_API vatek_result storage_chip_config_get(Pstorage_chip_config pcfg, uint8_t* psection);

#ifdef __cplusplus
}
#endif

#endif
