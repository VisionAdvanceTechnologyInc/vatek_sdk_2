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

#include <cross/cross_stream.h>
#include "./internal/stream_handle.h"

vatek_result cross_stream_open_test(uint32_t bitrate, hcross_stream* hcstream)
{
	Pcstream_handler phandler = NULL;
	vatek_result nres = cross_stream_test_get(bitrate, &phandler);
	if (is_vatek_success(nres))*hcstream = phandler;
	return nres;
}

vatek_result cross_stream_open_file(const char* szfilename, hcross_stream* hcstream)
{
	Pcstream_handler phandler = NULL;
	vatek_result nres = cross_stream_file_get(szfilename, &phandler);
	if (is_vatek_success(nres))*hcstream = phandler;
	return nres;
}

vatek_result cross_stream_open_udp(const char* szurl, hcross_stream* hcstream)
{
	Pcstream_handler phandler = NULL;
	vatek_result nres = cross_stream_udp_get(szurl, &phandler);
	if (is_vatek_success(nres))*hcstream = phandler;
	return nres;
}

vatek_result cross_stream_start(hcross_stream hcstream)
{
	Pcstream_handler phandler = (Pcstream_handler)hcstream;
	vatek_result nres = phandler->start(phandler->hstream);
	return nres;
}

vatek_result cross_stream_get_slice(hcross_stream hcstream, uint8_t** pslice)
{
	Pcstream_handler phandler = (Pcstream_handler)hcstream;
	return phandler->get_slice(phandler->hstream, pslice);
}

uint32_t cross_stream_get_bitrate(hcross_stream hcstream)
{
	Pcstream_handler phandler = (Pcstream_handler)hcstream;
	return phandler->get_bitrate(phandler->hstream);
}

void cross_stream_stop(hcross_stream hcstream)
{
	Pcstream_handler phandler = (Pcstream_handler)hcstream;
	phandler->stop(phandler->hstream);
}

void cross_stream_close(hcross_stream hcstream)
{
	Pcstream_handler phandler = (Pcstream_handler)hcstream;
	phandler->close(phandler->hstream);
}
