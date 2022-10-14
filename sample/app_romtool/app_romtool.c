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
//----------------------------------------------------------------------------
//
//  VATek firmware burn in sample code.
//
//----------------------------------------------------------------------------

#include <vatek_sdk_device.h>
#include <vatek_sdk_storage.h>
#include <cross/cross_os_api.h>

#include "../common/inc/tool_printf.h"

extern vatek_result cmd_parser_romfile(int argc, char *argv[],char** romfile);
extern void storage_process_handler(rom_progress_msg msg,void* progressval,void* param);

int main(int argc, char *argv[])
{
	Promfile_handle promfile = NULL;
	char* szromfile = NULL;
	vatek_result nres = cmd_parser_romfile(argc,argv,&szromfile);
	if(is_vatek_success(nres))
	{
		/* 
			step 1 :
			create vimage file handle 
		*/
		nres = vatek_storage_romfile_create(szromfile,&promfile);
		if(is_vatek_success(nres))
		{
			hvatek_devices hdevlist = NULL;
			hvatek_chip hchip = NULL;

		/*
			step 2 : 
			find current device to update rom.
		*/
			nres = vatek_device_list_enum(DEVICE_BUS_ALL,service_unknown,&hdevlist);
			if(nres > vatek_success)
				nres = vatek_device_open(hdevlist,0,&hchip);
			else if (nres == vatek_success)
			{
				_disp_err("can not found device.");
				nres = vatek_nodevice;
			}

			if(is_vatek_success(nres))
			{
			/*
				setp 3 : 
				create storage handle and write vimage file to device rom.
			*/
				Pstorage_handle pstorage = NULL;
				nres = vatek_storage_create_chip_handle(hchip,&pstorage,storage_process_handler,NULL);
				if(is_vatek_success(nres))
				{
					hvatek_storage hstorage = NULL;
					nres = vatek_storage_open(pstorage,&hstorage,0);
					if(is_vatek_success(nres))
					{
						nres = vatek_storage_write_app(hstorage,promfile);
						if(!is_vatek_success(nres))_disp_err("update rom file fail : %d",nres);
						else _disp_l("update rom file finish - %d",nres);
						vatek_storage_close(hstorage);
					}else _disp_err("open storage handle fail : %d",nres);
					vatek_storage_free_handle(pstorage);
				}else _disp_err("create storage handle fail : %d",nres);
			}

			/*
				step 4 : 
				clean all handle and resources.
			*/
			if (hchip)vatek_device_close_reboot(hchip);
			//if(hchip)vatek_device_close(hchip);
			if(hdevlist)vatek_device_list_free(hdevlist);
			vatek_storage_romfile_free(promfile);
		}else _disp_err("open rom file fail : %d [bad format]",nres);
	}else if(nres = vatek_badparam)_disp_err("bad romfile param : %s",argv[1]);

	printf_app_end();
	cross_os_sleep(10);
	return (int32_t)1;
}

void storage_process_handler(rom_progress_msg msg, void* progressval, void* param)
{
	static int32_t pos = 0;
	if (msg == rom_msg_set_str)
	{
		printf("%s \r\n", (char*)progressval);
		pos = 0;
	}
	else if (msg == rom_msg_set_pos)
	{
		uint32_t posval = (uint32_t)(uintptr_t)progressval;
		while (posval > pos)
		{
			printf(">");
			pos += 10;
		}
		if (posval == 100)printf("\r\n");
	}
}

vatek_result cmd_parser_romfile(int argc, char *argv[],char** romfile)
{
	vatek_result nres = vatek_unsupport;
	if(argc == 2)
	{
		FILE* from = fopen(argv[1],"rb");
		if(from)
		{
			nres = vatek_success;
			*romfile = argv[1];
			fclose(from);
		}else nres = vatek_badparam;
	}
	
	if(nres == vatek_unsupport)
	{
		_disp_l("support param below : ");
		_disp_l("	- app_romtool [filename]");
	}

	return nres;
}
