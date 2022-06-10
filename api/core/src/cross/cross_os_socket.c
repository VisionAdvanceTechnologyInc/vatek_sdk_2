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

#include <cross/cross_os_api.h>

#ifdef _WIN32
	#ifndef _WIN32_WINNT
		#define _WIN32_WINNT 0x0501  /* Windows XP. */
	#endif
	#include <winsock2.h>
	#include <Ws2tcpip.h>

	#pragma comment (lib, "Ws2_32.lib")
	#pragma comment (lib, "Mswsock.lib")
	#pragma comment (lib, "AdvApi32.lib")
#else
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
	#include <unistd.h> /* Needed for close() */
	#include <unistd.h>
	#include <fcntl.h>

	#define SOCKET 				int32_t
	#define INVALID_SOCKET 		-1
	#define closesocket			close
#endif 

#define MAX_IP_STR_LEN		256

typedef struct _handle_socket
{
	socket_protocol protocol;
	SOCKET socket_fd;
	struct sockaddr_storage addr;
	socket_param param;
	char url_buf[MAX_IP_STR_LEN];
	char* ptr_ip;
	char* prt_port;
}handle_socket, *Phandle_socket;

extern void socket_init();
extern void socket_free();
extern socket_protocol socket_get_protocol(Psocket_param param);
extern vatek_result socket_init_handle(Phandle_socket psocket);
extern vatek_result socket_set_block(Phandle_socket psocket, int32_t isblock);
extern vatek_result socket_mutilcast_group(Phandle_socket psocket);
extern vatek_result socket_set_recvbuf(Phandle_socket psocket);

vatek_result cross_os_create_socket(Psocket_param param, hcross_socket* hsocket)
{
	socket_protocol nprotocol = socket_get_protocol(param);
	vatek_result nres = vatek_unsupport;

	socket_init();

	if (nprotocol != protocol_unknown)
	{
		Phandle_socket psocket = (Phandle_socket)malloc(sizeof(handle_socket));
		nres = vatek_memfail;
		if (psocket)
		{
			memset(psocket, 0, sizeof(handle_socket));
			psocket->protocol = nprotocol;
			psocket->socket_fd = -1;
			memcpy(&psocket->param, param, sizeof(socket_param));
			
			nres = socket_init_handle(psocket);
			if (!is_vatek_success(nres))free(psocket);
			else *hsocket = psocket;
		}
	}

	return nres;
}

socket_protocol cross_os_get_protocol_socket(hcross_socket hsocket)
{
	Phandle_socket psocket = (Phandle_socket)hsocket;
	return psocket->protocol;
}

vatek_result cross_os_connect_socket(hcross_socket hsocket)
{
	Phandle_socket psocket = (Phandle_socket)hsocket;
	vatek_result nres = vatek_badstatus;

	if (psocket->socket_fd == -1)
	{
		struct addrinfo hint = { 0 };
		struct addrinfo *recvptr, *recvaddr;
		memset(&hint, 0, sizeof(struct addrinfo));

		hint.ai_family = AF_UNSPEC;
		hint.ai_socktype = SOCK_DGRAM;
		hint.ai_protocol = IPPROTO_UDP;

		if (psocket->protocol == protocol_tcp)
		{
			hint.ai_socktype = SOCK_STREAM;
			hint.ai_protocol = IPPROTO_TCP;
		}

		nres = (vatek_result)getaddrinfo(psocket->ptr_ip,psocket->prt_port, &hint, &recvaddr);
		if (is_vatek_success(nres))
		{
			recvptr = recvaddr;
			while (recvptr)
			{
				psocket->socket_fd = socket(recvptr->ai_family, recvptr->ai_socktype, recvptr->ai_protocol);
				if (psocket->socket_fd != INVALID_SOCKET)
				{
					int32_t opt = 1;
					if (psocket->protocol != protocol_tcp)
					{
						nres = (vatek_result)setsockopt(psocket->socket_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
						if (is_vatek_success(nres))
						{
							nres = socket_set_block(psocket, 0);
							if (is_vatek_success(nres))
							{
								if (psocket->protocol == protocol_tcp)
									nres = connect(psocket->socket_fd, recvptr->ai_addr, (int32_t)recvptr->ai_addrlen);
								else nres = bind(psocket->socket_fd, recvptr->ai_addr, (int32_t)recvptr->ai_addrlen);
							}
							if (is_vatek_success(nres))
							{
								memcpy(&psocket->addr, recvptr->ai_addr, recvptr->ai_addrlen);
								if (is_vatek_success(nres))nres = socket_set_recvbuf(psocket);
								if (is_vatek_success(nres))nres = socket_mutilcast_group(psocket);
								break;
							}
						}
					}
					closesocket(psocket->socket_fd);
					psocket->socket_fd = -1;
				}
				recvptr = recvptr->ai_next;
			}
			freeaddrinfo(recvaddr);
		}
	}
	return nres;
}

vatek_result cross_os_recv_socket(hcross_socket hsocket, uint8_t* pbuf, int32_t len, int32_t timeout)
{
	Phandle_socket psocket = (Phandle_socket)hsocket;
	vatek_result nres = vatek_unsupport;
	int32_t pos = 0;
	uint32_t tick = cross_os_get_tick_ms();
	while (pos < len)
	{
		int32_t rlen = len - pos;
		nres = (vatek_result)recv(psocket->socket_fd, &pbuf[pos], rlen, 0);
		if (!is_vatek_success(nres))
		{
			if ((cross_os_get_tick_ms() - tick) > (uint32_t)timeout)
			{
				nres = vatek_timeout;
			}
			else
			{
				cross_os_sleep(1);
				nres = vatek_success;
			}
		}

		if (nres >= 0)pos += (int32_t)nres;
		else break;
	}
	return nres;
}

vatek_result cross_os_send_socket(hcross_socket hsocket, uint8_t* pbuf, int32_t len, int32_t timeout)
{
	Phandle_socket psocket = (Phandle_socket)hsocket;
	vatek_result nres = vatek_unsupport;
	if (psocket->protocol == protocol_tcp)
	{
		int32_t pos = 0;
		while (pos < len)
		{
			int32_t rlen = len - pos;
			nres = (vatek_result)send(psocket->socket_fd, &pbuf[pos], rlen, 0);
			if (is_vatek_success(nres))pos += (int32_t)nres;
			else break;
		}
		
	}
	return nres;
}

vatek_result cross_os_disconnect_socket(hcross_socket hsocket)
{
	Phandle_socket psocket = (Phandle_socket)hsocket;
	vatek_result nres = vatek_success;
	if(psocket->socket_fd != -1)
		nres = (vatek_result)closesocket(psocket->socket_fd);
	psocket->socket_fd = -1;
	return nres;
}

void cross_os_free_socket(hcross_socket hsocket)
{
	Phandle_socket psocket = (Phandle_socket)hsocket;
	if (psocket->socket_fd != -1)
		cross_os_disconnect_socket(hsocket);
	free(psocket);

	socket_free();
}

#define TAG_UDP "udp://"
#define TAG_RTP "rtp://"
#define TAG_TCP "//"

vatek_result socket_init_handle(Phandle_socket psocket)
{
	vatek_result nres = vatek_success;
	char* ptr = NULL;
	strncpy(&psocket->url_buf[0], psocket->param.ip, MAX_IP_STR_LEN);

	ptr = strstr(&psocket->url_buf[0], TAG_TCP);
	if (ptr)ptr += 2;
	else nres = vatek_format;

	if (is_vatek_success(nres))
	{
		psocket->ptr_ip = ptr;
		ptr = strstr(ptr, ":");
		if (ptr != NULL)
		{
			psocket->prt_port = ptr + 1;
			*ptr = '\0';
		}
	}
	return nres;
}

socket_protocol socket_get_protocol(Psocket_param param)
{
	socket_protocol nprotocol = protocol_unknown;
	if (strlen(param->ip) < MAX_IP_STR_LEN)
	{
		if (strncmp(param->ip, TAG_UDP, strlen(TAG_UDP)) == 0)
			nprotocol = protocol_udp;
		else if (strncmp(param->ip, TAG_RTP, strlen(TAG_RTP)) == 0)
			nprotocol = protocol_rtp;
		else if (strncmp(param->ip, TAG_TCP,strlen(TAG_TCP)) == 0)
			nprotocol = protocol_tcp;
	}
	return nprotocol;
}

vatek_result socket_set_block(Phandle_socket psocket, int32_t isblock)
{
	vatek_result nres = vatek_unsupport;
#ifdef _WIN32
	u_long	flags;

	flags = !isblock;
	nres = (vatek_result)ioctlsocket(psocket->socket_fd, FIONBIO, (u_long FAR *) &flags);
#else
	int flags = fcntl(psocket->socket_fd, F_GETFL, 0);
	if (isblock)flags &= ~O_NONBLOCK;
	else flags |= O_NONBLOCK;
	nres = (vatek_result)fcntl(psocket->socket_fd, F_SETFL, O_NONBLOCK | flags);

#endif
	return nres;
}

vatek_result socket_mutilcast_group(Phandle_socket psocket)
{
	struct sockaddr_in *addr4 = (struct sockaddr_in *)&psocket->addr;
	vatek_result nres = vatek_success;
	if (IN_MULTICAST(ntohl(addr4->sin_addr.s_addr)))
	{
		struct ip_mreq mreq;
		mreq.imr_multiaddr.s_addr = addr4->sin_addr.s_addr;
		mreq.imr_interface.s_addr = INADDR_ANY;
		nres = setsockopt(psocket->socket_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const void *)&mreq, sizeof(mreq));
	}
	return nres;
}

vatek_result socket_set_recvbuf(Phandle_socket psocket)
{
	vatek_result nres = vatek_success;
	int32_t buflen = psocket->param.buffer_len;
	if (buflen)
		nres = (vatek_result)setsockopt(psocket->socket_fd, SOL_SOCKET, SO_RCVBUF, (void *)&buflen, sizeof(int32_t));
	
	return nres;
}

#ifndef _WIN32

uint32_t socket_get_tick(void)
{
	struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);	
}

void socket_sleep(int32_t ms)
{
    usleep(ms*1000);
}

#endif

#ifdef _WIN32
	static int32_t socket_ref = 0;
#endif
void socket_init()
{
#ifdef _WIN32
	if (!socket_ref)
	{
		WSADATA wsa_data;
		WSAStartup(MAKEWORD(1, 1), &wsa_data);
	}
	socket_ref++;
#endif
}

void socket_free()
{
#ifdef _WIN32
	socket_ref--;
	if (!socket_ref)WSACleanup();
#endif
}