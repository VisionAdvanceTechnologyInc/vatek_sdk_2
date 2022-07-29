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

#ifndef QTV_UI_DEFINE_H
#define QTV_UI_DEFINE_H

#include <core/qtv_define.h>
#include <core/qtv_service_usb.h>
#include <core/ui/ui_props_define.h>
#include <core/ui/ui_props/ui_props_stream.h>

_ui_enum_start(qtv_filter_mode)
	_val(qfilter_disable, disable)
	_val(qfilter_enum, enum)
	//_val(qfilter_customer, "customer")
_ui_enum_end

_ui_struct_start(qtv_source_param)
	_prop_enum(qtv_source_param, stream_source, source, "source - select current stream source", NULLHAL)
	//_prop_enum(qtv_source_param, qtv_filter_mode, filter, "source - transform pid filter", NULLHAL)
_ui_struct_end

_ui_struct_start(qtv_source_param_filter_only)
	//_prop_enum(qtv_source_param, qtv_filter_mode, filter, "source - transform pid filter", NULLHAL)
_ui_struct_end

_ui_enum_start(qtv_property_pathmode)
	_val(qpathmode_file_ts, Remux_File)
	_val(qpathmode_url_udp, Remux_UDP)
	_val(qpathmode_passthrough, Passthrough_File)
	_val(qpathmode_null, Test)

_ui_enum_end

_ui_struct_start(qtv_usbstream_param)
	_prop_enum(qtv_usbstream_param, qtv_property_pathmode,mode,"usbstream - source mode(path)",NULLHAL)
	//_prop_enum(qtv_usbstream_param, qtv_filter_mode, filter,"usbstream - filter mode",NULLHAL)
	_prop_path(qtv_usbstream_param, url,"usbstream - source [url | path]", _UIPROP_STRBUF_LEN_MAX)
_ui_struct_end

_ui_struct_start(qtv_usbstream_pcronly)
	_prop_enum(usb_param,pcr_adjust_mode,pcrmode,"usbstream - pcr adjust mode",NULLHAL)
_ui_struct_end

_ui_struct_start(qtv_usbstream_param_test)
	_prop_enum(qtv_usbstream_param, qtv_property_pathmode, mode, "usbstream - source mode(path)", NULLHAL)
_ui_struct_end

_ui_struct_start(qtv_usbstream_passthrough)
	_prop_enum(qtv_usbstream_param, qtv_property_pathmode, mode, "usbstream - source mode(path)", NULLHAL)
	_prop_path(qtv_usbstream_param, url, "usbstream - source [url | path]", _UIPROP_STRBUF_LEN_MAX)
_ui_struct_end

_ui_struct_start(qtv_output_param)
	_prop_u32(qtv_output_param, r2param.freqkhz,"output - frequency(KHz)",NULLHAL)
	_prop_enum(r2_param, r2_cntl_mode, mode, "output - R2 mode", NULLHAL)
_ui_struct_end

_ui_struct_start(qtv_usbmuxer_param)
	_prop_h16(qtv_usbmuxer_param, pcr_pid, "usbmux - pcr pid", NULLHAL)
	_prop_u16(qtv_usbmuxer_param, pcr_interval, "usbmux - pcr interval ms", NULLHAL)
	_prop_u16(qtv_usbmuxer_param, latency_ms,"usbmux - latency ms",NULLHAL)
_ui_struct_end



#endif
