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

#ifndef _TOOL_HELPFUL_
#define _TOOL_HELPFUL_

#include <core/vatek_base.h>

#define _V_ALIGN_WORD(n)	(((n + 3) >> 2)<< 2)

typedef struct _th_mempool
{
	uint8_t* psurbuf;
	uint8_t* prawbuf;
	uint8_t* pendbuf;
	uint8_t* pcurptr;
}th_mempool, *Pth_mempool;

typedef struct _th_memquery
{
	int32_t size;
	uint8_t* prawbuf;
	uint8_t* pendbuf;
	uint8_t* pwptr;
	uint8_t* pnextptr;
	uint8_t* pnextend;
	int32_t refptr;
	uint8_t* prptr;
	Pth_mempool mempool;
}th_memquery,*Pth_memquery;

typedef struct _th_fifo
{
	Pth_mempool pmempool;
	int32_t len;
	int32_t pos;
	void** fifo_list;
}th_fifo,*Pth_fifo;

typedef struct _th_circlebuf
{
	Pth_mempool pmempool;
	uint8_t* prawbuf;
	int32_t slicetotal;
	int32_t slicelen;
	int32_t slicenums;
	int32_t slicevalid;
	uint32_t nwptr;
	uint32_t nrptr;
}th_circlebuf, *Pth_circlebuf;

typedef struct _th_objectlink
{
	struct _th_objectlink* next;
	void* qobject;
	int32_t ref;
}th_objectlink, *Pth_objectlink;

typedef struct _th_objectlist
{
	int32_t nums;
	Pth_mempool pool;
	Pth_objectlink root;
	Pth_objectlink last;
	Pth_objectlink ptr;
}th_objectlist, *Pth_objectlist;

typedef struct _th_bitstream
{
	uint8_t* pbuf;
	int32_t size;
	uint8_t value;
	int32_t bitrecv;
	int32_t pos;
}th_bitstream,*Pth_bitstream;

#ifdef __cplusplus
extern "C" {
#endif

	/* simple bit stream */
	HAL_API Pth_bitstream th_bs_create_pool(Pth_mempool pmempool, uint8_t* pbuf, int32_t len);
	HAL_API void th_bs_config(Pth_bitstream pbs, uint8_t* pbuf, int32_t len);

	HAL_API uint32_t th_bs_read(Pth_bitstream pbs, int32_t bits);
	HAL_API void th_bs_write(Pth_bitstream pbs, uint32_t val, int32_t bits);

	/* simple fifo */
	HAL_API Pth_fifo th_fifo_create_pool(int32_t len,Pth_mempool pmpool);
	HAL_API Pth_fifo th_fifo_create(int32_t len);
	HAL_API void th_fifo_reset(Pth_fifo pfifo);
	HAL_API void th_fifo_free(Pth_fifo pfifo);
	HAL_API void th_fifo_push(Pth_fifo pfifo,void* ptrobj);
	HAL_API void* th_fifo_pop(Pth_fifo pfifo);
	HAL_API void* th_fifo_peek(Pth_fifo pfifo);

	/* simple memory pool */
	HAL_API void th_mempool_config_raw(Pth_mempool pmempool,uint8_t* praw,int32_t len);
	HAL_API Pth_mempool th_mempool_create(int32_t size);
	HAL_API void th_mempool_free(Pth_mempool pmempool);
	HAL_API void th_mempool_reset(Pth_mempool pmempool);
	HAL_API uint8_t* th_mempool_malloc(Pth_mempool pmempool, int32_t size);
	HAL_API int32_t th_mempool_get_pointer(Pth_mempool pmempool, uint8_t** ptrbuf);

	/* simple memory query */
	HAL_API Pth_memquery th_memquery_create(uint32_t size);
	HAL_API Pth_memquery th_memquery_create_pool(Pth_mempool pmempool, uint8_t* pbuf, int32_t size);
	HAL_API uint8_t* th_memquery_malloc(Pth_memquery pquery, int32_t size);
	HAL_API void th_memquery_push(Pth_memquery pquery, uint8_t* ptrbuf);
	HAL_API vatek_result th_memquery_free(Pth_memquery pquery, uint8_t* ptrbuf);
	HAL_API uint8_t* th_memquery_get_raw_buffer(Pth_memquery pquery);
	HAL_API void th_memquery_destory(Pth_memquery pquery);

	/* circle buffer */
	HAL_API Pth_circlebuf th_circlebuf_create_pool(Pth_mempool pmempool,int32_t slicelen,int32_t nums);
	HAL_API Pth_circlebuf th_circlebuf_create(int32_t slicelen,int32_t nums);
	HAL_API void th_circlebuf_free(Pth_circlebuf pcirclebuf);
	HAL_API void th_circlebuf_reset(Pth_circlebuf pcirclebuf);

	HAL_API uint8_t* th_circlebuf_get_wptr(Pth_circlebuf pcirclebuf);
	HAL_API void th_circlebuf_commit(Pth_circlebuf pcirclebuf);

	HAL_API uint8_t* th_circlebuf_get_rptr(Pth_circlebuf pcirclebuf);
	HAL_API void th_circlebuf_finish(Pth_circlebuf pcirclebuf);


	/* object list */
	HAL_API void th_object_list_reset(Pth_objectlist pqlist);
	HAL_API int32_t th_object_list_exists(Pth_objectlist pqlist, void* pobj);
	HAL_API void th_object_list_push(Pth_mempool pmemp,Pth_objectlist plist, void* pobj);

	typedef void* thobj_position;

	HAL_API thobj_position th_object_list_get_first(Pth_objectlist plist);
	HAL_API void* th_object_list_get_next(thobj_position* pos);

#ifdef __cplusplus
}
#endif

#endif
