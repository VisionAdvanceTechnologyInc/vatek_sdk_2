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

#include <core/tools/tool_helpful.h>

Pth_fifo th_fifo_create_pool(int32_t len,Pth_mempool pmpool)
{
	int32_t size = sizeof(th_fifo) + sizeof(void*) * len;
	uint8_t* pbuf = th_mempool_malloc(pmpool,size);
	if (pbuf)
	{
		Pth_fifo pfifo = (Pth_fifo)pbuf;
		pfifo->pmempool = pmpool;
		pfifo->len = len;
		pfifo->fifo_list = (void**)&pbuf[sizeof(th_fifo)];
		return pfifo;
	}
	return NULL;
}

Pth_fifo th_fifo_create(int32_t len)
{
	int32_t size = sizeof(th_fifo) + sizeof(void*) * len;
	uint8_t* pbuf = malloc(size);
	if (pbuf)
	{
		Pth_fifo pfifo = (Pth_fifo)pbuf;
		memset(pbuf, 0, size);
		pfifo->len = len;
		pfifo->fifo_list = (void**)&pbuf[sizeof(th_fifo)];
		return pfifo;
	}
	return NULL;
}

void th_fifo_free(Pth_fifo pfifo)
{
	if(!pfifo->pmempool)
		free(pfifo);
}

void th_fifo_reset(Pth_fifo pfifo)
{
	pfifo->pos = 0;
}

void th_fifo_push(Pth_fifo pfifo, void* ptrobj)
{
	if (pfifo->pos < (pfifo->len - 1))
	{
		pfifo->fifo_list[pfifo->pos++] = ptrobj;
	}
	else VERR("th_fifo_push overflow");
}

void* th_fifo_pop(Pth_fifo pfifo)
{
	void* ptrobj = th_fifo_peek(pfifo);
	if (ptrobj)pfifo->pos--;
	return ptrobj;
}

void* th_fifo_peek(Pth_fifo pfifo)
{
	if (pfifo->pos)
		return pfifo->fifo_list[pfifo->pos - 1];
	return NULL;
}
