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

#ifndef _MUX_SPEC_
#define _MUX_SPEC_

#include <mux/ui/ui_props_mux.h>
#include <mux/spec/spec_iso_13818.h>
#include <mux/spec/spec_dvb.h>
#include <mux/spec/spec_arib.h>
#include <mux/spec/spec_psip.h>

/**
 * @ingroup mux_spec
 * PSI Channel Detail 
 */
typedef union _spec_channel_detail{
	mux_spec_iso_channel iso;		/*!< iso */
	mux_spec_dvb_channel dvb;		/*!< dvb */
	mux_spec_arib_channel arib;		/*!< arib */
	mux_spec_abnt_channel abnt;		/*!< abnt */
	mux_spec_psip_channel psip;		/*!< psip */
}spec_channel_detail;

/**
 * @ingroup mux_spec
 * Spec Channel Properties
 */
typedef struct _spec_channel{
	mux_spec_mode spec;			/*!< spec mode */
	mux_country_code country;	/*!< spec country */
	spec_channel_detail _spec;	/*!< detail */
}spec_channel;

typedef spec_channel* Pspec_channel;

/**
 * @ingroup mux_spec
 * PSI Program Detail 
 */
typedef union _spec_program_detail
{
	mux_spec_iso_program iso;	/*!< iso */
	mux_spec_dvb_program dvb;	/*!< dvb */
	mux_spec_arib_program arib;	/*!< arib */
	mux_spec_abnt_program abnt;	/*!< abnt */
	mux_spec_psip_program psip;	/*!< psip */
}spec_program_detail;

/**
 * @ingroup mux_spec 
 * Spec Program Properties
 */
typedef struct _spec_program{
	mux_spec_mode spec;
	spec_program_detail _spec;
}spec_program;

typedef spec_program* Pspec_program;

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * @ingroup mspec_api
	 * Check specifily spec and countru is supported
	 * @param spec 
	 * @param country 
	 * @return HAL_API 
	 */
	HAL_API vatek_result muxspec_check_support(mux_spec_mode spec, mux_country_code country);

	/**
	 * @ingroup mspec_api
	 * malloc spec channel 
	 * @param[in] pmem memory pool handle
	 * @param[in] spec spec
	 * @param[in] country country 
	 * @param[inout] pchannel 
	 * @return HAL_API 
	 */
	HAL_API vatek_result muxspec_malloc_channel(Pth_mempool pmem, mux_spec_mode spec, mux_country_code country, Pspec_channel* pchannel);

	/**
	 * @ingroup mspec_api
	 * malloc spec program 
	 * @param[in] pmem memory pool handle
	 * @param[in] spec mux spec mode
	 * @param[inout] pprogram 
	 * @return HAL_API 
	 */
	HAL_API vatek_result muxspec_malloc_program(Pth_mempool pmem, mux_spec_mode spec, Pspec_program* pprogram);

	/**
	 * @ingroup mspec_api
	 * set spec program defualt
	 * @param[in] tag program serial number 
	 * @param[inout] pspec 
	 * @return HAL_API 
	 */
	HAL_API vatek_result muxspec_set_program_default(uint32_t tag, Pspec_program pspec);

	HAL_API vatek_result muxspec_get_channel_uiprops(mux_spec_mode spec, Pui_prop_item* pprops);
	HAL_API vatek_result muxspec_get_program_uiprops(mux_spec_mode spec, Pui_prop_item* pprops);
	HAL_API vatek_result muxspec_get_spec_string(mux_spec_mode spec,Pui_prop_item puiprop, Pmux_ui_string* pstrings);
	HAL_API vatek_result muxspec_set_spec_string(Pmux_ui_string pmuxstr, Pmux_string pstring, const char* szval);
	HAL_API vatek_result muxspec_get_spec_string_display(Pmux_ui_string puistring, Pmux_string pvstring, char* pbuf, int32_t len);


	HAL_API vatek_result muxspec_set_uiprops_spec_string(mux_spec_mode spec, Pui_prop_item props, const char* propname, uint8_t* praw, const char* value);
	HAL_API vatek_result muxspec_set_channel_spec_string(mux_spec_mode spec, const char* propname, uint8_t* praw, const char* value);
	HAL_API vatek_result muxspec_set_program_spec_string(mux_spec_mode spec, const char* propname, uint8_t* praw, const char* value);
	/**
	 * @ingroup mspec_api
	 * Get channel properties buffer length 
	 * @param[in] spec 
	 * @return HAL_API 
	 */
	HAL_API vatek_result muxspec_get_channel_len(mux_spec_mode spec);

	/**
	 * @ingroup mspec_api
	 * Get program properties buffer length 
	 * @param[in] spec 
	 * @return HAL_API 
	 */
	HAL_API vatek_result muxspec_get_program_len(mux_spec_mode spec);

	/**
	 * @ingroup mspec_api
	 * Set spec string with ascii coding  
	 * @param pstring spec string
	 * @param szeng new string
	 * @return HAL_API 
	 */
	HAL_API vatek_result muxspec_set_string_ascii(Pmux_string pstring, const char* szeng);

	/**
	 * @ingroup mspec_api
	 * Get spec string with ascii coding
	 * @param[in] pstring spec string
	 * @param[inout] pbuf 
	 * @param[in] len 
	 * @return HAL_API 
	 */
	HAL_API vatek_result muxspec_get_string_ascii(Pmux_string pstring, char* pbuf, int32_t len);

	/**
	 * @ingroup mspec_api
	 * 
	 * @param pstring 
	 * @param szeng 
	 * @return HAL_API 
	 */
	HAL_API vatek_result muxspec_set_string_jiseng(Pmux_string pstring, const char* szeng);
	HAL_API vatek_result muxspec_get_string_jiseng(Pmux_string pstring, char* pbuf, int32_t len);
	
	HAL_API vatek_result muxspec_set_string_utf16(Pmux_string pstring, const char* szeng);
	HAL_API vatek_result muxspec_get_string_utf16(Pmux_string pstring, char* pbuf, int32_t len);

	/**
	 * @ingroup mspec_api
	 * 
	 * @param pstring 
	 * @param szeng 
	 * @return HAL_API 
	 */
	HAL_API vatek_result muxspec_set_string_psip_mutil(Pmux_string pstring, const char* szeng);
	
	HAL_API vatek_result muxspec_get_string_psip_mutil(Pmux_string pstring, char* pbuf, int32_t len);

	HAL_API vatek_result muxspec_printf_channel(const char* fmt,Pspec_channel pch);
	HAL_API vatek_result muxspec_printf_program(const char* fmt,Pspec_program program);

#ifdef __cplusplus
}
#endif

#endif
