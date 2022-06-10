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

void th_object_list_reset(Pth_objectlist pqlist)
{
	memset(pqlist, 0, sizeof(th_objectlist));
}

int32_t th_object_list_exists(Pth_objectlist pqlist, void* pobj)
{
	Pth_objectlink plink = pqlist->root;
	while (plink)
	{
		if (plink->qobject == pobj)
			return 1;
		plink = plink->next;
	}
	return 0;
}

void th_object_list_push(Pth_mempool pmemp, Pth_objectlist plist, void* pobj)
{
	Pth_objectlink plink = plist->root;
	while (plink)
	{
		if (plink->qobject == pobj)
		{
			plink->ref++;
			return;
		}
		plink = plink->next;
	}

	plink = (Pth_objectlink)th_mempool_malloc(pmemp, sizeof(th_objectlink));
	plink->qobject = pobj;
	plink->ref = 1;
	if (!plist->root)plist->root = plink;
	else plist->last->next = plink;
	plist->last = plink;
	plist->nums++;
}

thobj_position th_object_list_get_first(Pth_objectlist plist)
{
	if (plist->root)
		return plist->root;
	return NULL;
}

void* th_object_list_get_next(thobj_position* pos)
{
	void* pval = ((Pth_objectlink)*pos)->qobject;
	*pos = ((Pth_objectlink)*pos)->next;
	return pval;
}

#if 0

void* qobject_list_get_first(Pqobject_list plist)
{
	plist->ptr = plist->root;

	if (plist->ptr)
		return plist->ptr->qobject;
	return NULL;
}

void* qobject_list_get_next(Pqobject_list plist)
{
	if (plist->ptr)
		plist->ptr = plist->ptr->next;

	if (plist->ptr)
		return plist->ptr->qobject;
	return NULL;
}

#endif
