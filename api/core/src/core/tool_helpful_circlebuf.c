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

Pth_circlebuf th_circlebuf_create(int32_t slicelen, int32_t nums)
{
	int32_t len = _V_ALIGN_WORD(sizeof(th_circlebuf) + (slicelen * nums));
	uint8_t* praw = (uint8_t*)malloc(len);
	if (praw)
	{
		Pth_circlebuf pcbuf = (Pth_circlebuf)&praw[0];
		memset(pcbuf, 0, len);
		pcbuf->prawbuf = &praw[sizeof(th_circlebuf)];
		pcbuf->slicenums = nums;
		pcbuf->slicetotal = slicelen * nums;
		pcbuf->slicelen = slicelen;
		return pcbuf;
	}
	return NULL;
}

Pth_circlebuf th_circlebuf_create_pool(Pth_mempool pmempool,int32_t slicelen,int32_t nums)
{
	int32_t len = _V_ALIGN_WORD(sizeof(th_circlebuf) + (slicelen * nums));
	uint8_t* praw = (uint8_t*)th_mempool_malloc(pmempool,len);
	if (praw)
	{
		Pth_circlebuf pcbuf = (Pth_circlebuf)&praw[0];
		pcbuf->prawbuf = &praw[sizeof(th_circlebuf)];
		pcbuf->slicenums = nums;
		pcbuf->slicetotal = slicelen * nums;
		pcbuf->slicelen = slicelen;
		pcbuf->pmempool = pmempool;
		return pcbuf;
	}
	return NULL;	
}

void th_circlebuf_free(Pth_circlebuf pcirclebuf)
{
	if(!pcirclebuf->pmempool)
		free(pcirclebuf);
}

void th_circlebuf_reset(Pth_circlebuf pcirclebuf)
{
	pcirclebuf->slicevalid = 0;
	pcirclebuf->nwptr = 0;
	pcirclebuf->nrptr = 0;
}

uint8_t* th_circlebuf_get_wptr(Pth_circlebuf pcirclebuf)
{
	if(pcirclebuf->slicevalid < pcirclebuf->slicenums)
		return &pcirclebuf->prawbuf[pcirclebuf->nwptr];
	return NULL;
}

void th_circlebuf_commit(Pth_circlebuf pcirclebuf)
{
	pcirclebuf->nwptr += pcirclebuf->slicelen;
	if (pcirclebuf->nwptr == pcirclebuf->slicetotal)pcirclebuf->nwptr = 0;
	pcirclebuf->slicevalid++;
	if (pcirclebuf->slicevalid > pcirclebuf->slicenums)VERR("circlebuffer overflow");
}

uint8_t* th_circlebuf_get_rptr(Pth_circlebuf pcirclebuf)
{
	uint8_t* pbuf = NULL;
	if(pcirclebuf->slicevalid)
		pbuf = &pcirclebuf->prawbuf[pcirclebuf->nrptr];
	return pbuf;
}

void th_circlebuf_finish(Pth_circlebuf pcirclebuf)
{
	pcirclebuf->nrptr += pcirclebuf->slicelen;
	if (pcirclebuf->nrptr == pcirclebuf->slicetotal)pcirclebuf->nrptr = 0;
	pcirclebuf->slicevalid--;
	if (pcirclebuf->slicevalid < 0)
	{
		VERR("circlebuffer underflow");
	}
}
