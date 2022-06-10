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

#include <mux/mux_handle.h>
#include "internal/mux_core.h"

vatek_result mux_open_raw(hmux_core hmux,hmux_rawtable* hraw)
{
    vatek_result nres = vatek_badstatus;
    Phandle_mux pmux = (Phandle_mux)hmux;
    if(mux_check_idle(pmux))
    {
        Phandle_raw newraw = (Phandle_raw)th_mempool_malloc(pmux->mempool,sizeof(handle_raw));
        nres = vatek_memfail;
        if(newraw)
        {
            pmux->hmode = hmux_mode_raw;
            pmux->mode = mux_psi_rawtable;
            pmux->_h.raw = newraw;
            if(hraw)*hraw = pmux;
            nres = vatek_success;
        }
    }
    return nres;
}

vatek_result muxraw_get_handle(hmux_core hmux, hmux_rawtable* hraw)
{
    vatek_result nres = vatek_badstatus;
    Phandle_mux pmux = (Phandle_mux)hmux;
    if (pmux->hmode == hmux_mode_raw)
    {
        *hraw = pmux;
        nres = vatek_success;
    }
    return nres;
}

vatek_result muxraw_create_table(hmux_rawtable hraw, int32_t packetnums, int32_t intervalms,Ppsi_table_raw* ptable)
{
    Phandle_mux pmux = (Phandle_mux)hraw;
    vatek_result nres = vatek_badstatus;
    if(pmux->hmode == hmux_mode_raw)
    {
		int32_t len = sizeof(psi_table_raw) + packetnums * TS_PACKET_LEN;
		Ppsi_table_raw newpsi = (Ppsi_table_raw)th_mempool_malloc(pmux->mempool, len);
		nres = vatek_memfail;
		if (newpsi)
		{
			newpsi->interval_ms = intervalms;
			newpsi->packet_nums = packetnums;
			newpsi->packet_buf = &((uint8_t*)newpsi)[sizeof(psi_table_raw)];
			*ptable = newpsi;
			nres = vatek_success;
		}
    }
    return nres;
}

vatek_result muxraw_push_table(hmux_rawtable hraw, Ppsi_table_raw ptable)
{
    Phandle_mux pmux = (Phandle_mux)hraw;
    vatek_result nres = vatek_badstatus;

    if(pmux->hmode == hmux_mode_raw)
    {
		Ppsiraw_handle praw = &pmux->_h.raw->rawtables;
		ptable->next = NULL;
		if (!praw->rawtables)
			praw->rawtables = ptable;
		else praw->lasttable->next = ptable;
		praw->lasttable = ptable;
		nres = vatek_success;
    }
    return nres;
}

vatek_result muxraw_get_table(hmux_rawtable hraw, Ppsi_table_raw* ptables)
{
	Phandle_mux pmux = (Phandle_mux)hraw;
	vatek_result nres = vatek_badstatus;

	if (pmux->hmode == hmux_mode_raw)
	{
        Ppsiraw_handle praw = &pmux->_h.raw->rawtables;
        *ptables = praw->rawtables;
		nres = vatek_success;
	}
	return nres;
}
