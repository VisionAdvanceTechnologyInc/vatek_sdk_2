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

#ifndef QTV_DEFINE_H
#define QTV_DEFINE_H

#include <QtGlobal>
#include <QObject>
#include <core/vatek_base.h>
#include <core/base/output_modulator.h>
#include <core/base/stream_base.h>
#include <mux/mux_define.h>
#include <core/ui/ui_props_define.h>
#include <service/service_broadcast.h>

#ifdef LIBVATEK_QT_STATIC
	#define QTV_DECL
#else
	#ifdef LIBVATEK_QT_LIB
		#define QTV_DECL Q_DECL_EXPORT
	#else 
		#define QTV_DECL Q_DECL_IMPORT
	#endif
#endif

typedef enum _qtv_status
{
	qstatus_fail = -1,
	qstatus_idle = 0,
	qstatus_unlock,
	qstatus_lock,
	qstatus_ready,
	qstatus_running,
	qstatus_stop,
	qstatus_finish,
}qtv_status;

typedef enum _qtv_property_type
{
	qproperty_null = 0,
	qproperty_u32,
	qproperty_u16,
	qproperty_u8,
	qproperty_h32,
	qproperty_h16,
	qproperty_h8,
	qproperty_select,
	qproperty_flag,
	qproperty_path,
	qproperty_string,
}qtv_property_type;

typedef enum _qtv_source_mode
{
	qsource_encoder,
	qsource_tsin,
	qsource_bridge_av,
	qsource_bridge_demod,
	qsource_bridge_tsin,
	qsource_usbstream,
}qtv_source_mode;

typedef enum _qtv_muxservice_mode
{
	qmuxservice_default = 0,
	qmuxservice_rawtable,
}qtv_muxservice_mode;

typedef enum _qtv_muxitem_mode
{
	qmuxitem_stream,
	qmuxitem_program,
	qmuxitem_channel,
	qmuxitem_rawtable,
}qtv_muxitem_mode;

typedef enum _qtv_filter_mode
{
	qfilter_disable,
	qfilter_enum,
	qfilter_customer,
}qtv_filter_mode;

typedef enum _qtv_service_mode
{
	qservice_broadcast,
	qservice_transfrom,
	qservice_usbstream,
	qservice_usbmuxer,
}qtv_service_mode;

typedef struct _qtv_locale_param
{
	mux_spec_mode spec;
	mux_country_code country;
	uint32_t frequency;
	modulator_type modulation;
	uint32_t bw_sb;
	video_codec video;
	audio_codec audio;
}qtv_locale_param, * Pqtv_locale_param;

typedef struct _qtv_output_param
{
	r2_param r2param;
	modulator_param modulation;
}qtv_output_param,*Pqtv_output_param;

typedef struct _qtv_source_param
{
	stream_source source;
	qtv_filter_mode filter;
	union _stream
	{
		tsin_param tsin;
		encoder_param encoder;
		mux_param mux;
		usb_param usb;
	}stream;
}qtv_source_param,*Pqtv_source_param;

typedef enum _qtv_property_pathmode
{
	qpathmode_null,
	qpathmode_passthrough,
	qpathmode_file_ts,
	qpathmode_url_udp,
	qpathmode_url_rtp,
}qtv_property_pathmode;

typedef struct _qtv_usbstream_param
{
	qtv_property_pathmode mode;
	qtv_filter_mode filter;
	pcr_adjust_mode pcrmode;
	char url[_UIPROP_STRBUF_LEN_MAX];
}qtv_usbstream_param,*Pqtv_usbstream_param;

#endif
