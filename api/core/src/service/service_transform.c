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

#include <service/service_transform.h>
#include <core/ui/ui_props/ui_props_stream.h>

extern vatek_result transform_source_reset(vatek_ic_module icmodule, stream_source source, Ptransform_source psource);
extern vatek_result transform_source_set(hvatek_chip hchip,stream_source source, Ptransform_source psource);
extern vatek_result transform_source_get(hvatek_chip hchip,stream_source* source, Ptransform_source psource);

vatek_result transform_enum_reset(vatek_ic_module icmodule, stream_source source, Ptransform_enum penum)
{
    vatek_result nres = vatek_badparam;
    if (source == stream_source_tsin || source == stream_source_usb)
        nres = transform_source_reset(icmodule,source, &penum->stream);
    if (is_vatek_success(nres))
    {
        penum->source = source;
        penum->timeout = 5000;
    }
    return nres;
}

vatek_result transform_enum_set(hvatek_chip hchip,Ptransform_enum penum)
{
    vatek_result nres = writehal(HALREG_TRANSFORM_MODE,TRANSFORM_ENUM);
    if(is_vatek_success(nres))
        nres = writehal(HALREG_TRENUM_TIMEOUT,penum->timeout);
    if (is_vatek_success(nres))
        nres = transform_source_set(hchip,penum->source, &penum->stream);
    return nres;
}

vatek_result transform_enum_get(hvatek_chip hchip,Ptransform_enum penum)
{
    uint32_t val = 0;
    vatek_result nres = readhal(HALREG_TRANSFORM_MODE,&val);
    if(is_vatek_success(nres))
    {
        transform_mode mode = (transform_mode)val;
        if(is_vatek_success(nres))
        {
            if(mode != trmode_enum)nres = vatek_badparam;
            else
            {
                memset(penum,0,sizeof(transform_enum));
                nres = readhal(HALREG_TRENUM_TIMEOUT,&val);
                if(is_vatek_success(nres))
                {
                    penum->timeout = (uint16_t)val;
                    nres = transform_source_get(hchip ,&penum->source, &penum->stream);
                }       
            }
        }
    }
    return nres;
}

vatek_result transform_capture_reset(vatek_ic_module icchip, stream_source source, Ptransform_capture pcapture)
{
    vatek_result nres = vatek_badparam;
    if (source == stream_source_tsin || source == stream_source_usb)
        nres = transform_source_reset(icchip,source, &pcapture->stream);
    if (is_vatek_success(nres))
    {
        pcapture->source = source;
        pcapture->timeout = 5000;
        pcapture->pid = 0x0000;
        pcapture->section_num = 0;
        pcapture->packet_nums = 0;
    }
    return nres;
}

vatek_result transform_capture_set(hvatek_chip hchip,Ptransform_capture pcapture)
{
    vatek_result nres = writehal(HALREG_TRANSFORM_MODE,TRANSFORM_CAPTURE);
    if(is_vatek_success(nres))
        nres = writehal(HALREG_TRCAPTURE_TIMEOUT,pcapture->timeout);
    if(is_vatek_success(nres))
        nres = writehal(HALREG_TRCAPTURE_PID,pcapture->pid);
    if (is_vatek_success(nres))
        nres = writehal(HALREG_TRCAPTURE_SECTION_NUM, pcapture->section_num);
    if (is_vatek_success(nres))
        nres = writehal(HALREG_TRCAPTURE_PACKET_NUMS, pcapture->packet_nums);
    if (is_vatek_success(nres))
        nres = transform_source_set(hchip,pcapture->source, &pcapture->stream);
    return nres;
}

vatek_result transform_capture_get(hvatek_chip hchip,Ptransform_capture pcapture)
{
    uint32_t val = 0;
    vatek_result nres = readhal(HALREG_TRANSFORM_MODE,&val);
    if(is_vatek_success(nres))
    {
        transform_mode mode = (transform_mode)val;
        if(is_vatek_success(nres))
        {
            if(mode != trmode_capture)nres = vatek_badparam;
            else
            {
                memset(pcapture,0,sizeof(transform_capture));
                nres = readhal(HALREG_TRCAPTURE_TIMEOUT,&val);
                if(is_vatek_success(nres))
                {
                    pcapture->timeout = (uint16_t)val;
                    nres = readhal(HALREG_TRCAPTURE_PID,&val);
                    if(is_vatek_success(nres))
                    {
                        pcapture->pid = (uint16_t)val;
                        nres = readhal(HALREG_TRCAPTURE_SECTION_NUM, &val);
                    }
                    if (is_vatek_success(nres))
                    {
                        pcapture->section_num = (uint16_t)val;
                        nres = readhal(HALREG_TRCAPTURE_PACKET_NUMS, &val);
                        if (is_vatek_success(nres))pcapture->packet_nums = (uint16_t)val;
                    }
                }      
                if(is_vatek_success(nres)) nres = transform_source_get(hchip, &pcapture->source, &pcapture->stream);
            }
        }
    }
    return nres;
}

vatek_result transform_broadcast_reset(vatek_ic_module icchip, stream_source source, Ptransform_broadcast pbc)
{
    vatek_result nres = vatek_success;
    memset(pbc,0,sizeof(transform_broadcast));
    nres = transform_source_reset(icchip,source, &pbc->stream);
    if (is_vatek_success(nres))
        nres = modulator_param_reset(modulator_dvb_t, &pbc->modulator);
    if(is_vatek_success(nres))
        pbc->source = source;
    return nres;
}

vatek_result transform_broadcast_set(hvatek_chip hchip, Ptransform_broadcast pbc)
{
    vatek_result nres = writehal(HALREG_TRANSFORM_MODE,TRANSFORM_BROADCAST);
    if (is_vatek_success(nres))
        nres = transform_source_set(hchip, pbc->source, &pbc->stream);
    if(is_vatek_success(nres))
        nres = modulator_param_set(hchip,&pbc->modulator);
    
    if(is_vatek_success(nres))
    {
        Ppid_filter pfilter = &pbc->filters;

        nres = vatek_badparam;
        if (pfilter->filter_nums < MUX_FILTER_NAX_NUMS)
        {
            if (!pfilter->filter_nums)nres = writehal(HALREG_MUX_FILTER_EN, 0);
            else
            {
                uint32_t val = MUX_FILTER_EN_TAG | (pfilter->filter_nums & MUX_FILTER_MASK);
                nres = writehal(HALREG_MUX_FILTER_EN, val);
                if (is_vatek_success(nres))
                {
                    int32_t i = 0;
                    for (i = 0; i < (int32_t)pfilter->filter_nums; i++)
                    {
                        if (pfilter->filters[i].orange_pid !=
                            pfilter->filters[i].replace_pid)
                        {
                            val = FILTER_REPLACE(pfilter->filters[i].orange_pid, pfilter->filters[i].replace_pid);
                        }
                        else val = FILTER_NO_REPLACE(pfilter->filters[i].orange_pid);
                        nres = writehal((HALREG_MUX_FILTER_0 + i), val);
                        if (!is_vatek_success(nres))break;
                    }
                }
            }
        }
    }
    return nres;
}

vatek_result transform_broadcast_get(hvatek_chip hchip, Ptransform_broadcast pbc)
{
    uint32_t val = 0;
    vatek_result nres = readhal(HALREG_TRANSFORM_MODE,&val);
    if(is_vatek_success(nres))
    {
        transform_mode mode = (transform_mode)val;
        nres = vatek_badparam;
        if(mode == trmode_broadcast)
        {
            memset(pbc, 0, sizeof(transform_broadcast));
            nres = transform_source_get(hchip, &pbc->source, &pbc->stream);
            if(is_vatek_success(nres))
                nres = modulator_param_get(hchip,&pbc->modulator);
            if (is_vatek_success(nres))
            {
                uint32_t val = 0;
                nres = readhal(HALREG_MUX_FILTER_EN, &val);
                if (is_vatek_success(nres))
                {
                    if ((val & 0xFFFFFF00) == MUX_FILTER_EN_TAG)
                    {
                        int32_t len = (int32_t)(val & MUX_FILTER_MASK);
                        if (len < MUX_FILTER_NAX_NUMS)
                        {
                            int32_t i = 0;
                            Ppid_filter pfilter = &pbc->filters;
                            pfilter->filter_nums = len;
                            for (i = 0; i < len; i++)
                            {
                                nres = readhal((HALREG_MUX_FILTER_0 + i), &val);
                                if (!is_vatek_success(nres))break;
                                pfilter->filters[i].orange_pid = (uint16_t)((val >> 16) & TS_PID_MASK);
                                pfilter->filters[i].replace_pid = (uint16_t)(val & TS_PID_MASK);
                            }
                        }
                    }
                    else if (val != 0)VWAR("unknown filter_en_tag : [%08x]", val);
                }
            }
        }
    }
    return nres;
}

vatek_result transform_source_reset(vatek_ic_module icmodule,stream_source source, Ptransform_source psource)
{
    vatek_result nres = vatek_success;
    memset(psource, 0, sizeof(transform_source));
    nres = stream_source_reset(icmodule, source, (uint8_t*)psource);
    return nres;
}

vatek_result transform_source_set(hvatek_chip hchip,stream_source source, Ptransform_source psource)
{
    vatek_result nres = stream_source_set(hchip, source, (uint8_t*)psource);
    if (is_vatek_success(nres))
        nres = writehal(HALREG_TRANSFORM_INPUT, source);
    return nres;
}

vatek_result transform_source_get(hvatek_chip hchip,stream_source* source, Ptransform_source psource)
{
    uint32_t val = 0;
    vatek_result nres = readhal(HALREG_TRANSFORM_INPUT, &val);
    if (is_vatek_success(nres))
    {
        stream_source cursource = (stream_source)val;
        memset(psource, 0, sizeof(transform_source));
        nres = stream_source_get(hchip, cursource, (uint8_t*)psource);
        if (is_vatek_success(nres))*source = cursource;
    }
    return nres;
}

vatek_result transform_mode_set(hvatek_chip hchip, transform_mode mode)
{
    vatek_result nres = writehal(HALREG_TRINFO_MODE, (uint32_t)mode);
    return nres;
}

vatek_result transform_mode_get(hvatek_chip hchip, transform_mode* mode)
{
    uint32_t val = 0;
    vatek_result nres = readhal(HALREG_TRINFO_MODE, &val);
    if (is_vatek_success(nres))
    {
        if (is_transform_mode((transform_mode)val))*mode = (transform_mode)val;
        else *mode = trmode_null;
    }
    return nres;
}

stream_mode transform_source_get_stream_mode(stream_source source, Ptransform_source pstream)
{
    stream_mode mode = stream_remux;
    if (source == stream_source_tsin)
    {
        Ptsin_param ptsin = &pstream->tsin;
        mode = ptsin->streammode;
    }
    else if (source == stream_source_usb)
    {
        Pusb_param pusb = &pstream->usb;
        mode = pusb->mode;
    }
    return mode;
}