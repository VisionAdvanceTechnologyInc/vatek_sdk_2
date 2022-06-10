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

Pth_mempool th_mempool_create(int32_t size)
{
	uint8_t* prawbuf = NULL;
	size += sizeof(th_mempool);
	size = _V_ALIGN_WORD(size);

	prawbuf = (uint8_t*)malloc(size);
	if (prawbuf)
	{
		Pth_mempool pmpool = (Pth_mempool)&prawbuf[0];
		memset(prawbuf, 0, size);
		pmpool->prawbuf = &prawbuf[sizeof(th_mempool)];
		pmpool->pendbuf = prawbuf + size;
		pmpool->pcurptr = pmpool->prawbuf;
		return pmpool;
	}else VERR("malloc mempool null : [%s]",size);
	return NULL;
}

void th_mempool_config_raw(Pth_mempool pmempool,uint8_t* praw,int32_t len)
{
	pmempool->psurbuf = praw;
	pmempool->prawbuf = praw;
	pmempool->pendbuf = pmempool->prawbuf + len;
	pmempool->pcurptr = pmempool->prawbuf;
}

void th_mempool_free(Pth_mempool pmempool)
{
	if(!pmempool->psurbuf)
		free(pmempool);
}

void th_mempool_reset(Pth_mempool pmempool)
{
	pmempool->pcurptr = pmempool->prawbuf;
}

uint8_t* th_mempool_malloc(Pth_mempool pmempool, int32_t size)
{
	uint8_t* pnext = pmempool->pcurptr;
	size = _V_ALIGN_WORD(size);
	pnext += size;
	if (pnext > pmempool->pendbuf)VERR("qmempool buffer overflow");
	else
	{
		uint8_t* ptr = pmempool->pcurptr;
		memset(ptr, 0, size);
		pmempool->pcurptr = pnext;
		return ptr;
	}
	return NULL;
}

int32_t th_mempool_get_pointer(Pth_mempool pmempool, uint8_t** ptrbuf)
{
	int32_t size = (int32_t)(pmempool->pendbuf - pmempool->pcurptr);
	*ptrbuf = pmempool->pcurptr;

	return size;
}
