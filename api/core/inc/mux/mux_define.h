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

#ifndef _MUX_DEFINE_
#define _MUX_DEFINE_

#include <core/vatek_base.h>
#include <core/tools/tool_helpful.h>
#include <core/base/media_audio.h>
#include <core/base/media_video.h>
#include <core/hal/halreg_mux.h>

#define TS_PACKET_LEN										188
#define TS_PACKET_SYNC_TAG									0x47
#define TS_PID_MASK											0x1FFF
#define TS_PID_NULL											0x1FFF
#define TS_PID_PAT_TABLE									0x0000
#define TS_PID(id)											(id & TS_PID_MASK)

#define TS_TABLEID_PAT										0x00
#define TS_TABLEID_CAT										0x01
#define TS_TABLEID_PMT										0x02

#define PSIP_COMMON_PID										0x1FFB

#define PES_STREAM_ES_INFO_LEN 								64

/* _iso-13818-1 table 2-29	- stream type */

#define STREAMTYPE_ISO11172_VIDEO							0x01	/* MPEG1 VIDEO	*/
#define STREAMTYPE_ISO13818_2_VIDEO							0x02	/* MPEG2 VIDEO	*/
#define STREAMTYPE_ISO11172_AUDIO							0x03	/* MPEG1_LAYOUT II AUDIO*/
#define STREAMTYPE_ISO13818_3_AUDIO							0x04		
#define STREAMTYPE_ISO_PRIVATE_SECTIONS						0x05
#define STREAMTYPE_PES_PRIVATE_DATA							0x06	/* ISO PRIVATE DATA [ AC-3 | VBI | TELETEXT ]*/
#define STREAMTYPE_ISO13522_MHEG							0x07
#define STREAMTYPE_DSM_CC									0x08
#define STREAMTYPE_H222_1									0x09
#define STREAMTYPE_ISO13818_6_TYPE_A						0x0a
#define STREAMTYPE_ISO13818_6_TYPE_B						0x0b
#define STREAMTYPE_ISO13818_6_TYPE_C						0x0c
#define STREAMTYPE_ISO13818_6_TYPE_D						0x0d
#define STREAMTYPE_ISO13818_1_AUXILIARY						0x0e

#define STREAMTYPE_ISO144962_2_VISUAL						0x10

#define STREAMTYPE_ISO144962_1_SL_PES						0x12
#define STREAMTYPE_ISO144962_1_SL_SECTION					0x13
#define STREAMTYPE_SYNCHRONIZED_DOWNLOAD					0x14

#define STREAMTYPE_ISO13818_7_ADTS							0x0f
#define STREAMTYPE_ISO144962_3_LATM							0x11

#define STREAMTYPE_ATSC_A53_AC3								0x81	
#define STREAMTYPE_ATSC_A53_E_AC3							0x87
#define STREAMTYPE_ISO14496_10_VIDEO						0x1b	/* H264 */
#define STREAMTYPE_ISO23008_2_VIDEO							0x24	/* H265 */
#define STREAMTYPE_CHINA_AVS_VIDEO							0x42


#define STREAMID_PROGRAM_STREAM_MAP							0xbc
#define STREAMID_PRIVATE_STREAM_1							0xbd
#define STREAMID_PADDING_STREAM								0xbe
#define STREAMID_PRIVATE_STREAM_2							0xbf

#define STREAMID_AUDIO_STREAM_BASE							0xc0
#define STREAMID_AUDIO_STREAM_MASK							0x1f
#define STREAMID_VIDEO_STREAM_BASE							0xe0
#define STREAMID_VIDEO_STREAM_MASK							0x0f

#define STREAMID_ECM_STREAM									0xf0
#define STREAMID_EMM_STREAM									0xf1
#define STREAMID_DSM_CC_STREAM								0xf2
#define STREAMID_IEC13522_STREAM							0xf3
#define STREAMID_H222_1_TYPE_A								0xf4
#define STREAMID_H222_1_TYPE_B								0xf5
#define STREAMID_H222_1_TYPE_C								0xf6
#define STREAMID_H222_1_TYPE_D								0xf7
#define STREAMID_H222_1_TYPE_E								0xf8
#define STREAMID_ANCILLARY_STREAM							0xf9
#define STREAMID_SL_PACKETIZED_STREAM						0xfa
#define STREAMID_FLEXMUX_STREAM								0xfb
#define STREAMID_PROGRAM_STREAM_DIRECTORY					0xff

#define DESCTAG_DVB_VBI_DATA								0x45
#define DESCTAG_TELETEXT									0x56
#define DESCTAG_DVB_AC3										0x6A

/**
 * @ingroup mux_props
 * raw psi table 
 */
typedef struct _psi_table_raw{
	struct _psi_table_raw* next;	/*!< link next psi table */
	uint16_t interval_ms;			/*!< interval time (ms) */
	uint16_t packet_nums;			/*!< length in packet numbers (188) */
	uint8_t* packet_buf;			/*!< packet buffer */
}psi_table_raw;

typedef psi_table_raw* Ppsi_table_raw;

/**
 * @ingroup mux_props
 * define dtv psi service spec id
 */
typedef enum _mux_spec_mode{
	mux_spec_undefine = 0,				/*!< undefine (normal)*/
	mux_spec_iso = 0x00010000,			/*!< iso 13818-1 */
	mux_spec_dvb = 0x00020000,			/*!< dvb */
	mux_spec_dtmb = 0x00030000,			/*!< dtmb */
	mux_spec_arib = 0x00040000,			/*!< arib */
	mux_spec_abnt = 0x00050000,			/*!< abnt */
	mux_spec_psip = 0x00060000,			/*!< psip */
	mux_spec_common = 0x08000000,
}mux_spec_mode;

/* spec is dvb_spec (used dvb_channel and program props )*/
#define mux_spec_is_dvb(spec)			(spec == mux_spec_dvb || spec == mux_spec_dtmb)
#define mux_spec_is_psip(spec)			(spec == mux_spec_psip)
#define mux_spec_is_arib(spec)			(spec == mux_spec_arib || spec == mux_spec_abnt)
#define mux_spec_is_iso(spec)			(spec == mux_spec_iso)
#define mux_spec_get_main(spec)			(spec & 0xFFFF0000)

/* spec is base on _dvb spec (used deferent channel and program props with _dvb)*/
#define mux_spec_is_dvb_spec(spec)		(spec >= mux_spec_dvb && spec <= mux_spec_abnt)

/**
 * @ingroup mux_props
 * country code for psi service
 */
typedef enum _mux_country_code
{
	mux_country_undefined = 0,				/*!< undefined */
	dvb_taiwan = mux_spec_dvb,
	dvb_asean,
	dvb_uk,
	dvb_spanish,
	dvb_france,
	dvb_italian,
	dvb_colombia,
	dvb_india,
	dvb_ghana,
	dvb_singapore,
	dvb_georgin,
	nordig_finland = mux_spec_dvb | 0x100,
	freetv_australian = mux_spec_dvb | 0x200,
	freetv_new_zealand,
	dtmb_china = mux_spec_dtmb,
	dtmb_hk,
	dtmb_cuba,
	arib_japan = mux_spec_arib,
	abnt_brazil = mux_spec_abnt,
	abnt_argentina,
	abnt_bolivia,
	abnt_botswana,
	abnt_costa_rica,
	abnt_chile,
	abnt_ecuador,
	abnt_paraguay,
	abnt_philippines,
	abnt_peru,
	abnt_uruguay,
	abnt_venezuela,
	atsc_usa = mux_spec_psip,
	atsc_korean,
	atsc_canada,
	atsc_mexico,
	atsc_dominican,
}mux_country_code;

#define mux_country_to_spec(country)	(country & 0xFFFFF000)

/**
 * @ingroup mux_props
 * psi provider mode 
 */
typedef enum _mux_psi_mode{
	mux_psi_undefine = -1,
	mux_psi_pure = MUX_PURE,			/*!< disable default psi service */
	mux_psi_iso13818 = MUX_ISO13818,	/*!< iso-13818-1 psi service */
	mux_psi_specrule = MUX_SPEC,		
	mux_psi_specdefault = MUX_DEFSPEC,	/*!< default psi config by spec */
	mux_psi_rawtable,
}mux_psi_mode;

#define is_current_mux_psi_mode(m)	(m >= mux_psi_pure && m <= mux_psi_specdefault)

typedef struct _mux_iso_13818{
	uint16_t transport_stream_id;
	uint16_t program_number;
}mux_iso_13818;

typedef mux_iso_13818* Pmux_iso_13818;

typedef union _mux_iso_lang
{
	uint8_t raw[4];
	uint32_t value;
}mux_iso_lang;

typedef mux_iso_lang* Pmux_iso_lang;

#define _ISOLANG_JPN {'j','p','n','\0',}
#define _ISOLANG_ENG {'e','n','g','\0',}
#define _ISOLANG_KOR {'k','o','r','\0',}
#define _ISOLANG_SPA {'s','p','a','\0',}


typedef struct _mux_string
{
	uint32_t len;
	uint8_t* text;
	uint32_t maxlen;
}mux_string;

typedef mux_string* Pmux_string;


/**
 * @ingroup mux_props
 * mux tick 
 */
typedef struct _mux_clock_tick
{
	uint64_t _90KHz;
	uint32_t _27MHz;
}mux_clock_tick;

typedef mux_clock_tick* Pmux_clock_tick;

/**
 * @ingroup core_props
 * time tick 
 */
typedef struct _mux_time_tick{
	uint32_t ms;	/*!< time ms */
	uint32_t ns;	/*!< time ns */
}mux_time_tick;

typedef mux_time_tick* Pmux_time_tick;

/**
 * @ingroup mux_props
 * stream type 
 */
typedef enum _mux_stream_type
{
	mux_stream_video = MUX_STREAM_TYPE_VIDEO,	/*!< video */
	mux_stream_audio = MUX_STREAM_TYPE_AUDIO,	/*!< audio */
	mux_stream_data = MUX_STREAM_TYPE_DATA,		/*!< data */
}mux_stream_type;

/**
 * @ingroup mux_props
 * define context stream  
 */
typedef struct _mux_stream{
	struct _mux_stream* next;
	mux_stream_type type;
	union
	{
		media_video video;
		media_audio audio;
	}_stream_info;
	uint16_t stream_pid;
	uint8_t stream_type;
	uint8_t es_info_len;
	uint8_t* es_info_buf;
}mux_stream;

typedef mux_stream* Pmux_stream;

/**
 * @ingroup mux_props
 * define context program  
 */
typedef struct _mux_program{
	struct _mux_program* next;
	uint16_t pcr_pid;
	uint16_t pmt_pid;
	Pmux_stream streams;
}mux_program;

typedef mux_program* Pmux_program;

/**
 * @ingroup mux_props
 * define context channel  
 */
typedef struct _mux_channel{
	Pmux_program programs;
}mux_channel;

typedef mux_channel* Pmux_channel;

#ifdef __cplusplus
extern "C" {
#endif

	static inline uint32_t mux_clock_get_eclipse(Pmux_clock_tick pclk0, Pmux_clock_tick pclk1)
	{
		if(pclk0->_90KHz > pclk1->_90KHz)
		{
			uint32_t ntick = (uint32_t)((((uint64_t)0x1FFFFFFFFL) - pclk0->_90KHz) + pclk1->_90KHz);
			ntick *= 300;
			ntick += (300 - pclk0->_27MHz) + pclk1->_27MHz;
			return ntick;
			#if 0
			uint32_t ntick = (uint32_t)(((uint64_t)0x1FFFFFFFFL) - pclk0->_90KHz) * 300;
			return ntick - (300 - pclk0->_27MHz) + pclk1->_27MHz;
			#endif
		}else
		{
			uint32_t ntick = (uint32_t)(pclk1->_90KHz - pclk0->_90KHz) * 300;
			int32_t n27 = pclk1->_27MHz - pclk0->_27MHz;
			return ntick + n27;
		}
	}

	static inline void mux_clock_append(Pmux_clock_tick pclk, uint32_t t27mhz)
	{
		pclk->_90KHz += t27mhz / 300;
		pclk->_27MHz += t27mhz % 300;
		pclk->_90KHz += pclk->_27MHz / 300;
		pclk->_27MHz %= 300;
	}

	static inline void mux_clock_append_clock(Pmux_clock_tick pclk,Pmux_clock_tick pclkappend)
	{
		pclk->_90KHz += pclkappend->_90KHz;
		pclk->_27MHz += pclkappend->_27MHz;
		pclk->_90KHz += pclk->_27MHz / 300;
		pclk->_27MHz %= 300;
	}

	static inline uint32_t mux_clock_get_ms(Pmux_clock_tick pclk)
	{
		return (uint32_t)(pclk->_90KHz / 90);
	}

	static inline void mux_time_append_ns(Pmux_time_tick ptime, uint32_t ns)
	{
		ptime->ms += ns / 1000000;
		ptime->ns += ns % 1000000;
		ptime->ms += ptime->ns / 1000000;
		ptime->ns %= 1000000;
	}

#ifdef __cplusplus
}
#endif

#endif
