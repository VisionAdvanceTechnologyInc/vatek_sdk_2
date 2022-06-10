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

#include <core/base/stream_encoder.h>
#include <core/ui/ui_props/ui_props_encoder.h>
#include <core/ui/ui_props_api.h>

vatek_result encoder_param_set(hvatek_chip hchip,Pencoder_param penc)
{
    vatek_result nres = vatek_badparam;
    if(is_current_encmode(penc->mode))
    {
        nres = ui_props_write_hal(hchip,_ui_struct(encoder_param),(uint8_t*)penc);
        if(is_vatek_success(nres))
            nres = ui_props_write_hal(hchip,_ui_struct(vi_param),(uint8_t*)&penc->viparam);
        if(is_vatek_success(nres))
            nres = ui_props_write_hal(hchip,_ui_struct(quality_param),(uint8_t*)&penc->quality);
        if(is_vatek_success(nres))
            nres = ui_props_write_hal(hchip,_ui_struct(media_video),(uint8_t*)&penc->video);
        if(is_vatek_success(nres))
            nres = ui_props_write_hal(hchip,_ui_struct(media_audio),(uint8_t*)&penc->audio);
    }
    return nres;
}

vatek_result encoder_param_get(hvatek_chip hchip,Pencoder_param penc)
{
    vatek_result  nres = ui_props_read_hal(hchip,_ui_struct(encoder_param),(uint8_t*)penc);
    if(is_vatek_success(nres))
    {
        if(is_current_encmode(penc->mode))
        {
            nres = ui_props_read_hal(hchip,_ui_struct(vi_param),(uint8_t*)&penc->viparam);
            if(is_vatek_success(nres))
                nres = ui_props_read_hal(hchip,_ui_struct(quality_param),(uint8_t*)&penc->quality);
            if(is_vatek_success(nres))
                nres = ui_props_read_hal(hchip,_ui_struct(media_video),(uint8_t*)&penc->video);
            if(is_vatek_success(nres))
                nres = ui_props_read_hal(hchip,_ui_struct(media_audio),(uint8_t*)&penc->audio);
        }else nres = vatek_badparam;
    }
    return nres;
}

vatek_result encoder_param_reset(video_codec vcodec, Pencoder_param penc)
{
    memcpy(penc, (uint8_t*)&default_encoder_param, sizeof(encoder_param));
    penc->video.vcodec = vcodec;
    if (vcodec != encvideo_mpeg2)
    {
        penc->quality.minq = 10;
        penc->quality.maxq = 25;
    }
    return vatek_success;
}
