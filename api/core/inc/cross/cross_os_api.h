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

#ifndef _CROSS_OS_API_
#define _CROSS_OS_API_

#include <core/vatek_base.h>

#ifdef __cplusplus
extern "C" {
#endif

/* cross_os_normal_api */
	HAL_API int32_t cross_os_get_ch_no_wait(void);
	HAL_API void cross_os_printf(const char* fmt, ...);
	HAL_API void cross_os_error(const char* fmt,...);

/* cross_os_filesystem_api */

	HAL_API void cross_os_get_current_path(char* path, int32_t buflen);
	HAL_API void cross_os_append_path(char* path, const char* append);

	typedef void* hcross_ffind;
	#define FF_TYPE_FOLDER	0x00000001

	typedef struct _cross_ffind{
		uint32_t ff_type;
		char* filename;
		char* fullpath;
	}cross_ffind;

	typedef cross_ffind* Pcross_ffind;

	HAL_API vatek_result cross_os_findfile_first(hcross_ffind* hffind,const char* path, Pcross_ffind* pfind);
	HAL_API vatek_result cross_os_findfile_next(hcross_ffind hffind, Pcross_ffind* pfind);
	HAL_API void cross_os_findfile_close(hcross_ffind hffind);

	/* cross_os_time_api */
	HAL_API void cross_os_sleep(int32_t ms);
	HAL_API void cross_os_sleep_yield();			/*  relinquish the CPU */
	HAL_API uint32_t cross_os_get_tick_ms(void);
    HAL_API void cross_os_usleep(uint32_t us);
    HAL_API void cross_os_wait_unit(struct timespec* target);
    HAL_API void cross_os_time_plus_ms(struct timespec* tp,int32_t ms);
    HAL_API void cross_os_time_plus_time(struct timespec* tp,struct timespec* tappend);

    HAL_API vatek_result cross_os_get_time(struct timespec * tp);
    HAL_API uint64_t cross_os_get_time_us();
    HAL_API uint64_t cross_os_time_to_us(struct timespec* tp);

    HAL_API vatek_result cross_os_time_eclipse(struct timespec* st,struct timespec* eclipse);
 
	/* cross_os_thread_api */
    typedef void* hcross_thread;

    typedef struct _cross_thread_param{
        void* userparam;
        vatek_result result;
    }cross_thread_param;

	typedef cross_thread_param* Pcross_thread_param;

    typedef void (*fpcross_thread_function)(Pcross_thread_param param);

    HAL_API hcross_thread cross_os_create_thread(fpcross_thread_function fpfun,void* userparam);
    HAL_API vatek_result cross_os_join_thread(hcross_thread hthread);
    HAL_API vatek_result cross_os_free_thread(hcross_thread hthread);
    HAL_API vatek_result cross_os_get_thread_result(hcross_thread hthread);

	/* cross_os_mutex_api */
    typedef void* hcross_mutex;

	HAL_API vatek_result cross_os_create_mutex_name(const char* tag, hcross_mutex* hmutex);
	HAL_API vatek_result cross_os_open_mutex_name(const char* tag, hcross_mutex* hmuxtex);
    HAL_API vatek_result cross_os_create_mutex(hcross_mutex* hmutex);
    HAL_API void cross_os_lock_mutex(hcross_mutex hmutex);
	HAL_API vatek_result cross_os_lock_mutex_timeout(hcross_mutex hmutex, uint32_t ms);
    HAL_API vatek_result cross_os_trylock_mutex(hcross_mutex hmutex);
    HAL_API void cross_os_release_mutex(hcross_mutex hmutex);
    HAL_API vatek_result cross_os_free_mutex(hcross_mutex hmutex);


	/* cross_os_event_api */
	typedef void* hcross_event;

	HAL_API vatek_result cross_os_create_event(const char* tag, hcross_event* hevent);
	HAL_API vatek_result cross_os_open_event(const char* tag, hcross_event* hevent);

	HAL_API vatek_result cross_os_wait_event_timeout(hcross_event hevent, int32_t ms);
	HAL_API vatek_result cross_os_wait_event(hcross_event hevent);
	HAL_API vatek_result cross_os_raise_event(hcross_event hevent);

	HAL_API void cross_os_free_event(hcross_event hevent);

	/* cross_os_share_memory_api */

	#define CROSS_OS_SMEM_PAGE_SIZE			4096 

	typedef void* hcross_smem;
	
	HAL_API vatek_result cross_os_create_smem(const char* tag, hcross_smem* hsmem,int32_t size);
	HAL_API vatek_result cross_os_open_smem(const char* tag, hcross_smem* hsmem,int32_t size);

	HAL_API uint8_t* cross_os_get_smem(hcross_smem hsmem);
	HAL_API vatek_result cross_os_close_smem(hcross_smem hsmem);

	/* cross_os_dynamic_library_api */

    typedef void* hcross_dll;

    HAL_API hcross_dll cross_os_dll_load(const char* dllfile);
    HAL_API void* cross_os_dll_get_function(hcross_dll hdll,const char* name);
    HAL_API vatek_result cross_os_dll_free(hcross_dll hdll);
	HAL_API vatek_result cross_os_dll_valid(const char* name);

	/* cross_os_process_api */

	typedef void* hcross_process;
	typedef void(*fpprocess_parser)(void* param,uint8_t* ptr,int32_t len);

	typedef struct _cross_proccess_param{
		char* path;
		char* command;
		void* param;
		fpprocess_parser parser;
	}cross_proccess_param;

	typedef cross_proccess_param* Pcross_proccess_param;

	HAL_API hcross_process cross_os_create_process(Pcross_proccess_param pprocess);
	HAL_API vatek_result cross_os_check_process(hcross_process hprocess);
	HAL_API void cross_os_free_process(hcross_process hprocess);

	/* cross_os_socket_api */

	typedef void* hcross_socket;

	typedef enum _socket_mode{
		socket_client,
		socket_service,
	}socket_mode;

	typedef enum _socket_protocol{
		protocol_unknown = -1,
		protocol_rtp = 0,
		protocol_udp = 1,
		protocol_tcp = 2,
	}socket_protocol;

	typedef struct _socket_param{
		socket_mode mode;
		const char* ip;
		int32_t buffer_len;
	}socket_param;

	typedef socket_param* Psocket_param;
	
	/*
		supported : 
			rtp://xxx.xxx.xxx.xxx:pppp
			udp://xxx.xxx.xxx.xxx:pppp
			//xxx.xxx.xxx.xxx:PPPP			[_tcp]
	*/

	HAL_API vatek_result cross_os_create_socket(Psocket_param param, hcross_socket* hsocket);
	HAL_API socket_protocol cross_os_get_protocol_socket(hcross_socket hsocket);
	HAL_API vatek_result cross_os_connect_socket(hcross_socket hsocket);
	HAL_API vatek_result cross_os_recv_socket(hcross_socket hsocket, uint8_t* pbuf, int32_t len, int32_t timeout);
	HAL_API vatek_result cross_os_send_socket(hcross_socket hsocket, uint8_t* pbuf, int32_t len, int32_t timeout);
	HAL_API vatek_result cross_os_disconnect_socket(hcross_socket hsocket);
	HAL_API void cross_os_free_socket(hcross_socket hsocket);

	#ifdef _MSC_VER
		#define _cos_log(format,...) cross_os_printf("[%s:%d] " format "\r\n",__func__, __LINE__, ##__VA_ARGS__)
		#define _cos_err(format,...) cross_os_error("[%s:%d] error - " format "\r\n",__func__, __LINE__,##__VA_ARGS__)
	#else
		#define _cos_log(format, args...) cross_os_printf("[%s:%d] " format "\r\n",__func__, __LINE__, ##args)
		#define _cos_err(format, args...) cross_os_error("[%s:%d] error - " format "\r\n",__func__, __LINE__,##args)
	#endif

#ifdef __cplusplus
}
#endif

#endif
