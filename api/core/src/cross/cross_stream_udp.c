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

#include "./internal/stream_handle.h"
#include <signal.h>


#define UDP_FRAME_LEN			1316
#define UDP_RTP_HDR_LEN			12
#define UDP_MAX_FRAME_LEN		(UDP_FRAME_LEN + UDP_RTP_HDR_LEN)
#define UDP_SLICE_BUF_NUMS		32
#define UDP_SLICE_BUF_MASK		0x1F
#define UDP_BUFFER_LEN			(((4*1024*1024)/UDP_FRAME_LEN) * UDP_FRAME_LEN)

#define UDP_STATUS_IDLE			0
#define UDP_STATUS_RUN			1
#define UDP_STATUS_STOP			2

typedef struct _handle_udp
{
	cstream_handler handle;
	hcross_socket hsocket;
	hcross_thread hrecv;
	hcross_mutex hlock;
	int32_t buf_rptr;
	int32_t buf_wptr;
	uint8_t buf_pool[UDP_SLICE_BUF_NUMS][TSSLICE_BUFFER_LEN];
	int32_t isrunning;
	uint32_t packetnums;
	uint32_t tick;
}handle_udp, * Phandle_udp;

extern vatek_result cstream_udp_start(hcstream hstream);
extern vatek_result cstream_udp_get_slice(hcstream hstream, uint8_t** pslice);
extern uint32_t cstream_udp_get_bitrate(hcstream hstream);
extern void cstream_udp_stop(hcstream hstream);
extern void cstream_udp_close(hcstream hstream);

extern void udp_recv_handle(Pcross_thread_param param);
extern uint8_t* get_write_buffer(Phandle_udp pudp);
extern void commit_write_buffer(Phandle_udp pudp);
extern uint8_t* get_valid_buffer(Phandle_udp pudp);
extern int32_t check_valid_buffer(Phandle_udp pudp);

vatek_result cross_stream_udp_get(const char* url, Pcstream_handler* pcstream)
{
	socket_param sparam = { 0, };
	vatek_result nres = vatek_success;
	hcross_socket hsocket = NULL;
	hcross_mutex hlock = NULL;

	sparam.buffer_len = UDP_BUFFER_LEN;
	sparam.ip = url;
	sparam.mode = socket_client;

	nres = cross_os_create_socket(&sparam, &hsocket);
	if (is_vatek_success(nres))
		nres = cross_os_create_mutex(&hlock);

	if (is_vatek_success(nres))
	{
		Phandle_udp pudp = (Phandle_udp)malloc(sizeof(handle_udp));
		nres = vatek_memfail;
		if (pudp)
		{
			memset(pudp, 0, sizeof(handle_udp));
			pudp->hsocket = hsocket;
			pudp->hlock = hlock;
			pudp->handle.hstream = pudp;
			pudp->handle.start = cstream_udp_start;
			pudp->handle.stop = cstream_udp_stop;
			pudp->handle.get_slice = cstream_udp_get_slice;
			pudp->handle.get_bitrate = cstream_udp_get_bitrate;
			pudp->handle.close = cstream_udp_close;
			*pcstream = &pudp->handle;
			nres = vatek_success;
		}
	}

	if (!is_vatek_success(nres))
	{
		if (hsocket)cross_os_free_socket(hsocket);
		if (hlock)cross_os_free_mutex(hlock);
	}

	return  nres;
}

vatek_result cstream_udp_start(hcstream hstream)
{
	vatek_result nres = vatek_badstatus;
	Phandle_udp pudp = (Phandle_udp)hstream;
	if (!pudp->hrecv)
	{
		nres = cross_os_connect_socket(pudp->hsocket);
		if (is_vatek_success(nres))
		{
			cross_os_sleep(1000);					//wait A3 chip ready
			pudp->tick = cross_os_get_tick_ms();
			pudp->isrunning = UDP_STATUS_RUN;
			pudp->hrecv = cross_os_create_thread(udp_recv_handle, pudp);
			if (!pudp->hrecv)nres = vatek_hwfail;

			if (!is_vatek_success(nres))
				nres = cross_os_disconnect_socket(pudp->hsocket);
		}
	}
	return nres;
}

uint32_t cstream_udp_get_bitrate(hcstream hstream)
{
	Phandle_udp pudp = (Phandle_udp)hstream;
	if (pudp->isrunning)
	{
		int32_t eclipse = cross_os_get_tick_ms() - pudp->tick;
		if (eclipse)
		{
			uint32_t bitrate = (pudp->packetnums * TS_PACKET_LEN * 8 * 1000) / eclipse;
			pudp->tick = cross_os_get_tick_ms();
			pudp->packetnums = 0;
			return bitrate;
		}
	}
	return 0;
}

vatek_result cstream_udp_get_slice(hcstream hstream, uint8_t** pslice)
{
	Phandle_udp pudp = (Phandle_udp)hstream;
	vatek_result nres = vatek_badstatus;
	if (pudp->isrunning)
	{
		int32_t valid = check_valid_buffer(pudp);
		nres = (vatek_result)1;
		if (valid) {
			*pslice = get_valid_buffer(pudp);
			VERR("show ip : %d", pslice);
		}
		else nres = vatek_success;
	}
	return nres;
}

void cstream_udp_stop(hcstream hstream)
{
	Phandle_udp pudp = (Phandle_udp)hstream;
	if (pudp->isrunning == UDP_STATUS_RUN)
	{
		pudp->isrunning = UDP_STATUS_STOP;
		while (pudp->isrunning)cross_os_sleep(100);
		cross_os_free_thread(pudp->hrecv);
		cross_os_disconnect_socket(pudp->hsocket);
	}
	pudp->hrecv = NULL;
	pudp->isrunning = UDP_STATUS_IDLE;
}

void cstream_udp_close(hcstream hstream)
{
	Phandle_udp pudp = (Phandle_udp)hstream;
	if (pudp->isrunning)
		cstream_udp_stop(hstream);
	cross_os_free_mutex(pudp->hlock);
	free(pudp);
}


void udp_recv_handle(Pcross_thread_param param)
{
	Phandle_udp pudp = (Phandle_udp)param->userparam;
	vatek_result nres = vatek_success;
	socket_protocol nprotocol = cross_os_get_protocol_socket(pudp->hsocket);
	int32_t framelen = UDP_FRAME_LEN;
	int32_t frameoffset = 0;
	uint8_t* pbuf = (uint8_t*)malloc(UDP_MAX_FRAME_LEN);

	int32_t pktnums = 0;
	int32_t retry = 0;
	uint8_t* pktbuf = NULL;

	if(pbuf)memset(pbuf, 0, UDP_MAX_FRAME_LEN);

	if (nprotocol == protocol_rtp)
	{
		framelen = UDP_MAX_FRAME_LEN;
		frameoffset = UDP_RTP_HDR_LEN;
	}

	while (pudp->isrunning == UDP_STATUS_RUN)
	{
		nres = cross_os_recv_socket(pudp->hsocket, pbuf, framelen, 1000);
		if (is_vatek_success(nres))
		{
			uint8_t* precv = &pbuf[frameoffset];
			int32_t pos = frameoffset;

			while (pos < framelen && pudp->isrunning)
			{
				if (pktbuf == NULL)
				{
					pktbuf = get_write_buffer(pudp);
					pktnums = 0;
				}

				if (!pktbuf)cross_os_sleep(1);
				else
				{
					if (precv[0] == TS_PACKET_SYNC_TAG)
						memcpy(&pktbuf[pktnums * TS_PACKET_LEN], precv, TS_PACKET_LEN);
					else nres = vatek_format;

					if (is_vatek_success(nres))
					{
						precv += TS_PACKET_LEN;
						pos += TS_PACKET_LEN;
						pktnums++;
						pudp->packetnums++;

						if (pktnums == TSSLICE_PACKET_NUM)
						{
							commit_write_buffer(pudp);
							pktbuf = NULL;
						}
					}
					else break;
				}
			}
		}
		if (!is_vatek_success(nres))
		{
			if (nres == vatek_timeout)retry++;
			else break;
			if (retry > 10)break;;
		}
		else retry = 0;
	}
	pudp->isrunning = UDP_STATUS_IDLE;
	free(pbuf);
}

uint8_t* get_write_buffer(Phandle_udp pudp)
{
	uint8_t* ptr = NULL;
	int32_t nptr;
	cross_os_lock_mutex(pudp->hlock);
	nptr = (pudp->buf_wptr + 1) & UDP_SLICE_BUF_MASK;
	if (nptr != pudp->buf_rptr)
		ptr = &pudp->buf_pool[pudp->buf_wptr][0];
	cross_os_release_mutex(pudp->hlock);
	return ptr;
}

void commit_write_buffer(Phandle_udp pudp)
{
	cross_os_lock_mutex(pudp->hlock);
	pudp->buf_wptr++;
	pudp->buf_wptr &= UDP_SLICE_BUF_MASK;
	cross_os_release_mutex(pudp->hlock);
}

uint8_t* get_valid_buffer(Phandle_udp pudp)
{
	uint8_t* ptr = NULL;
	cross_os_lock_mutex(pudp->hlock);
	ptr = &pudp->buf_pool[pudp->buf_rptr][0];
	pudp->buf_rptr++;
	pudp->buf_rptr &= UDP_SLICE_BUF_MASK;
	cross_os_release_mutex(pudp->hlock);
	return ptr;
}

int32_t check_valid_buffer(Phandle_udp pudp)
{
	int32_t len;
	cross_os_lock_mutex(pudp->hlock);
	len = pudp->buf_wptr - pudp->buf_rptr;
	if (len < 0)len += UDP_SLICE_BUF_NUMS;
	cross_os_release_mutex(pudp->hlock);
	return len;
}
