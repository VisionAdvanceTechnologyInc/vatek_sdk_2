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

#include <core/base/stream_base.h>
#include <core/ui/ui_props/ui_props_stream.h>
#include <core/ui/ui_props_api.h>

const Pui_prop_item stream_source_get_ui_props(stream_source source)
{
	if (source == stream_source_tsin)
		return _ui_struct(tsin_param);
	else if (source == stream_source_usb)
		return _ui_struct(usb_param);
	return _ui_struct(encoder_param);
}

vatek_result stream_source_reset(vatek_ic_module icmodule, stream_source source, uint8_t* prawsource)
{
	vatek_result nres = vatek_success;
	if (source == stream_source_tsin)
		memcpy(prawsource, &default_tsin_param, sizeof(tsin_param));
	else if (source == stream_source_usb)
		memcpy(prawsource, &default_usb_param, sizeof(usb_param));
	else if (source == stream_source_encoder)
	{
		if (icmodule == ic_module_b3_lite)
			nres = encoder_param_reset(encvideo_h264, (Pencoder_param)prawsource);
		else nres = encoder_param_reset(encvideo_mpeg2, (Pencoder_param)prawsource);
	}
	else nres = vatek_badparam;
	return nres;
}

vatek_result stream_source_set(hvatek_chip hchip,stream_source source, uint8_t* prawsource)
{
	vatek_result nres = vatek_badparam;
	const Pui_prop_item puiprops = stream_source_get_ui_props(source);
	if (puiprops)
	{
		if (source == stream_source_encoder)
			nres = encoder_param_set(hchip, (Pencoder_param)prawsource);
		else nres = ui_props_write_hal(hchip, puiprops, prawsource);
	}
	return nres;
}

vatek_result stream_source_get(hvatek_chip hchip,stream_source source, uint8_t* prawsource)
{
	vatek_result nres = vatek_badparam;
	const Pui_prop_item puiprops = stream_source_get_ui_props(source);
	if (puiprops)
	{
		if (source == stream_source_encoder)
			nres = encoder_param_get(hchip, (Pencoder_param)prawsource);
		else nres = ui_props_read_hal(hchip, puiprops, prawsource);
	}
	return nres;
}
