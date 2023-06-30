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

#ifndef _UI_PROPS_MUX_
#define _UI_PROPS_MUX_

#include <core/ui/ui_props_api.h>
#include <mux/mux_epg.h>

typedef enum _mux_ui_level
{
	mux_ui_level_channel = 0,
	mux_ui_level_program = 1,
}mux_ui_level;

typedef struct _mux_ui_props
{
	mux_ui_level level;
	Pui_prop_item props;
	uint8_t* def_val;
	uint32_t size;
}mux_ui_props, *Pmux_ui_props;

#define _MUX_STRING_TYPE_ASCII					0		/* ASCII or utf-8			*/	
#define _MUX_STRING_TYPE_JIS_ENG				1		/* _arib_english			*/
#define _MUX_STRING_TYPE_JIS_JP					2		/* _arib_shift_jis coding	*/
#define _MUX_STRING_UTF16						3		/* _utf-16 (UNICODE)		*/
#define _MUX_STRING_PSIP_MUTIL					4		/* A65 section 6.1			*/

typedef struct _mux_ui_string
{
	uint8_t idx;
	uint8_t coding;
	uint16_t maxlen;
	const char* name;
}mux_ui_string, *Pmux_ui_string;

/* mux_ui_define_tool */
#define _mux_ui_get_struct(stype)					((Pui_prop_item)&mux_struct_##stype[0])
#define _mux_props(level,desc)						static const mux_ui_props mux_props_##desc = \
													{level,_mux_ui_get_struct(desc),(uint8_t*)&default_##desc,sizeof(default_##desc)};
#define _mux_ui_struct_start(stype)					const static ui_prop_item mux_struct_##stype[] = {
#define _mux_ui_struct_end(stype,level)				_prop_end};\
													_mux_props(level,stype)

#define _mux_string_start(spec)						static const mux_ui_string spec##_string_table[] = {
#define _mux_string_end								{0,0,0,NULL,},};
#define _mux_string(idx,tag,coding,len)				{idx,coding,len,#tag,},

#define _mux_get_string(spec)						(Pmux_ui_string)&spec##_string_table[0]

#define _mux_get_props(desc)						(Pmux_ui_props)&mux_props_##desc

/* mux_ui_properties type */
#define _mux_enum_raw(type,enum,name,desc,uitype)	{#name,desc,uitype,_ui_enum(enum),NULL,offsetof(type,name),},
#define _mux_flag_raw(type,name,desc,vals)			{#name,desc,prop_type_cb32,vals,NULL,offsetof(type,name),},
#define _mux_flag(type,name,desc)					_mux_flag_raw(type,name,desc,_ui_flags(type,name))
#define _mux_enum(type,enum,name,desc)				_mux_enum_raw(type,enum,name,desc,(ui_prop_type)sizeof(enum))
#define _mux_isolang(type,name,desc)				{#name,desc,(ui_prop_type)(prop_type_cblang | sizeof(mux_iso_lang)),NULL,NULL,offsetof(type,name),},
#define _mux_u32(type,name,desc)					{#name,desc,prop_type_uint32,NULL,NULL,offsetof(type,name),},
#define _mux_u16(type,name,desc)					{#name,desc,prop_type_uint16,NULL,NULL,offsetof(type,name),},
#define _mux_u8(type,name,desc)						{#name,desc,prop_type_uint8,NULL,NULL,offsetof(type,name),},
#define _mux_h32(type,name,desc)					{#name,desc,prop_type_hex32,NULL,NULL,offsetof(type,name),},
#define _mux_h16(type,name,desc)					{#name,desc,prop_type_hex16,NULL,NULL,offsetof(type,name),},
#define _mux_h8(type,name,desc)						{#name,desc,prop_type_hex8,NULL,NULL,offsetof(type,name),},

#define _mux_str(type,name,desc,idx)				{#name,desc,(ui_prop_type)(prop_type_strbuf | (idx & 0xFF)),NULL,NULL,offsetof(type,name),},

/* struct mux_param */

_ui_enum_start(mux_psi_mode)
	_val(mux_psi_rawtable,"rawtable")
	_val(mux_psi_specdefault,"default")
	_val(mux_psi_specrule,"rule")
_ui_enum_end

_ui_enum_start(mux_spec_mode)
	_val(mux_spec_undefine,undefine)
	_val(mux_spec_iso,iso13818)
	_val(mux_spec_dvb,dvb)
	_val(mux_spec_dtmb,dtmb)
	_val(mux_spec_arib,arib)
	_val(mux_spec_abnt,abnt)
	_val(mux_spec_psip,psip)
_ui_enum_end

_ui_enum_start(mux_country_code)
	_val(mux_country_undefined,undefine)
	_val(dvb_taiwan, dvb_taiwan)
	_val(dvb_asean, dvb_asean)
	_val(dvb_uk, dvb_uk)
	_val(dvb_spanish, dvb_spanish)
	_val(dvb_france, dvb_france)
	_val(dvb_italian, dvb_italian)
	_val(dvb_colombia, dvb_colombia)
	_val(dvb_india, dvb_india)
	_val(dvb_ghana, dvb_ghana)
	_val(dvb_singapore, dvb_singapore)
	_val(dvb_georgin, dvb_georgin)
	_val(nordig_finland, nordig_finland)
	_val(freetv_australian, freetv_australian)
	_val(freetv_new_zealand, freetv_new_zealand)
	_val(dtmb_china, dtmb_china)
	_val(dtmb_hk, dtmb_hk)
	_val(dtmb_cuba, dtmb_cuba)
	_val(arib_japan, arib_japan)
	_val(abnt_brazil, abnt_brazil)
	_val(abnt_argentina, abnt_argentina)
	_val(abnt_bolivia, abnt_bolivia)
	_val(abnt_botswana, abnt_botswana)
	_val(abnt_costa_rica, abnt_costa_rica)
	_val(abnt_chile, abnt_chile)
	_val(abnt_ecuador, abnt_ecuador)
	_val(abnt_paraguay, abnt_paraguay)
	_val(abnt_philippines, abnt_philippines)
	_val(abnt_peru, abnt_peru)
	_val(abnt_uruguay, abnt_uruguay)
	_val(abnt_venezuela, abnt_venezuela)
	_val(atsc_usa, atsc_usa)
	_val(atsc_korean, atsc_korean)
	_val(atsc_canada, atsc_canada)
	_val(atsc_mexico, atsc_mexico)
	_val(atsc_dominican, atsc_dominican)
_ui_enum_end

_ui_enum_start(mux_content_type)
	_val(mux_content_undefine, undefine)
	_val(mux_content_news, news)
	_val(mux_content_sport, sport)
	_val(mux_content_movie, movie)
	_val(mux_content_drama, drama)
	_val(mux_content_music, music)
	_val(mux_content_tabloidshow, tabloidshow)
	_val(mux_content_varietyshow, varietyshow)
	_val(mux_content_animation, animation)
	_val(mux_content_documentary, documentary)
	_val(mux_content_performance, performance)
	_val(mux_content_education, education)
	_val(mux_content_welfare, welfare)
	_val(mux_content_others, others)
_ui_enum_end

_ui_enum_start(mux_parental_rating)
	_val(rating_disable, disable)
	_val(rating_general, general)
	_val(rating_guidance, guidance)
	_val(rating_restricted, restricted)
_ui_enum_end

_ui_flags_start(mux_epg_param, epg_flags)
	_val(EPG_FLAG_START_HOUR, en_section_start_hour)
_ui_flags_end

_ui_enum_start(mux_epg_mode)
	_val(epg_mode_dvb, epg_dvb)
	_val(epg_mode_arib_0, arib_0)
	_val(epg_mode_arib_1, arib_1)
	_val(epg_mode_simple, simple)
_ui_enum_end

_ui_struct_start(mux_epg_section)
	_prop_u8(mux_epg_section, duration, "epg - schedule duration", EMPTY_HAL)
	_prop_enum(mux_epg_section, mux_content_type, type, "epg - content type", EMPTY_HAL)
	_prop_enum(mux_epg_section, mux_parental_rating, parental_rating, "epg - parental rating", EMPTY_HAL)
	_prop_str(mux_epg_section, title, "epg - title", 64)
	_prop_str(mux_epg_section, content, "epg - content", 256)
_ui_struct_end

_ui_struct_start(mux_epg_param)
	_prop_enum(mux_epg_param, mux_epg_mode, mode, "epg - mode", EMPTY_HAL)
	_prop_h16(mux_epg_param, event_id, "epg - base event id", EMPTY_HAL)
_ui_struct_end

#endif

