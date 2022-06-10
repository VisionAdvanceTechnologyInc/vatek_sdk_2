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

#ifndef _TOOL_BUFSTREAM_
#define _TOOL_BUFSTREAM_

#include <core/vatek_base.h>

typedef struct _bufstream
{
	int32_t pos;
	uint8_t* buf;
}bufstream, *Pbufstream;

#ifdef __cplusplus
extern "C" {
#endif

	HAL_API void putbuf_buf(Pbufstream pbuf, uint8_t* pbufdata, int32_t len);
	HAL_API void getbuf_buf(Pbufstream pbuf, uint8_t* pbufdata, int32_t len);
	HAL_API uint32_t getbuf_uint32_t(Pbufstream pbuf);
	HAL_API void putbuf_uint32_t(Pbufstream pbuf, uint32_t val);
	HAL_API uint8_t* setbuf_offset(Pbufstream pbuf, int32_t pos);

	static inline void putbuf_byte(Pbufstream pbuf, uint8_t val) { pbuf->buf[pbuf->pos++] = val; }
	static inline uint8_t getbuf_byte(Pbufstream pbuf) { return pbuf->buf[pbuf->pos++]; }
	static inline uint8_t* getbuf_ptr(Pbufstream pbuf) { return &pbuf->buf[pbuf->pos]; }
	static inline void setbuf_ptr(Pbufstream pbuf, uint8_t* ptr) { pbuf->pos = (int32_t)(ptr - &pbuf->buf[0]); }

#ifdef __cplusplus
}
#endif

#endif
