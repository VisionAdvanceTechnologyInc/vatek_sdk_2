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


#include <vatek_sdk_storage.h>
#include <vatek_sdk_device.h>
#include <vatek_sdk_bridge.h>

#include <cross/cross_bridge.h>
#include <cross/cross_os_api.h>
#include <core/tools/tool_crc32.h>
#include <core/hal/halservice_rescure.h>
#include <core/storage/storage_section.h>

typedef struct _storage_entry
{
	Pstorage_handle phandle;
	int32_t is_v2app;
	app_header app;
	loader_header loader;
	uint32_t pos;
	uint32_t resource_pos;
	uint8_t* section_buf;
	storage_chip_config chipconfig;
	r2_tune_handle r2tune;
	union
	{
		storage_broadcast broadcast;
		storage_transform transform;
	}_service;
	Pstorage_resource resources;
}storage_entry,*Pstorage_entry;

#define storage_h(h)							(((Pstorage_entry)(h))->phandle)
#define storage_resource_root(h)				(((Pstorage_entry)(h))->resources)
#define storage_malloc(h,len)					storage_h(h)->malloc(len,storage_h(h)->param)
#define storage_free(h,ptr)						storage_h(h)->free((uint8_t*)ptr,storage_h(h)->param)	
#define storage_is_valid(h)						(((Pstorage_entry)(h))->is_v2app)
#define storage_buf(h)							(((Pstorage_entry)(h))->section_buf)
#define storage_pos(h)							(((Pstorage_entry)(h))->pos)
#define storage_resource_pos(h)					(((Pstorage_entry)(h))->resource_pos)

#define storage_write_section(h,pos,pbuf)		storage_h(h)->write(pos,pbuf,storage_h(h)->param)
#define storage_read_section(h,pos,pbuf)		storage_h(h)->read(pos,pbuf,storage_h(h)->param)


#ifdef _MSC_VER
	#define storage_progress_str(h,msg)			storage_h(h)->progress(rom_msg_set_str,(void*)msg,storage_h(h))
	#define storage_progress_pos(h,pos)			storage_h(h)->progress(rom_msg_set_pos, (void*)(intptr_t)pos, storage_h(h))
#else
	#define storage_progress_str(h,msg)			storage_h(h)->progress(rom_msg_set_str,(void*)msg,storage_h(h))
	#define storage_progress_pos(h,pos)			storage_h(h)->progress(rom_msg_set_pos, (void*)(long)pos, storage_h(h))
#endif

#define romfile_h(f)						((Promfile_handle)f)
#define romfile_get_section(f,a,s)			romfile_h(f)->get_section(a,s,romfile_h(f)->param)
#define romfile_get_length(f)				romfile_h(f)->get_length(romfile_h(f)->param)

extern vatek_result storage_get_loader_header(hvatek_storage hstorage, Ploader_header ploader);
extern vatek_result storage_get_app_header(hvatek_storage hstorage, Papp_header papp);
extern vatek_result storage_reset(hvatek_storage hstorage,hal_service_mode service);
extern vatek_result storage_find_resource(hvatek_storage hstorage);
extern vatek_result storage_write_resource(hvatek_storage hstorage, uint32_t pos, uint8_t* psection, Pbin_resource_section pres);
extern vatek_result storage_read_resource(hvatek_storage hstorage, uint8_t* psection, uint32_t pos, int32_t len, Pstorage_resource* pres);
extern int32_t storage_getlen_resource(Pbin_resource_section pres);
extern vatek_result filehandle_write_resource(hvatek_storage hstorage,FILE* fdest, Pbin_resource_section pres, uint8_t* psection);

vatek_result vatek_storage_open(Pstorage_handle phandler, hvatek_storage* hstorage, int32_t isresource)
{
	vatek_result nres = vatek_memfail;
	int32_t len = (sizeof(storage_entry) + BINARY_SECTION_SIZE);
	uint8_t* rawbuf = phandler->malloc(len, phandler->param);

	if (rawbuf != NULL)
	{
		Pstorage_entry newstorage = (Pstorage_entry)&rawbuf[0];
		memset(rawbuf, 0, len);
		newstorage->section_buf = (uint8_t*)&rawbuf[sizeof(storage_entry)];
		newstorage->phandle = phandler;
		*hstorage = newstorage;

		VERR("storage_read_section");
		storage_progress_str(newstorage, "load header ...");
		nres = storage_get_loader_header(*hstorage, &newstorage->loader);
		if(is_vatek_success(nres))
			nres = storage_get_app_header(*hstorage, &newstorage->app);
		if (is_vatek_success(nres))
			nres = storage_reset(*hstorage, newstorage->app.service_mode);
		

		if (is_vatek_success(nres))
		{
			newstorage->is_v2app = 1;
			newstorage->pos = BINARY_SECTION_LEN(LOADER_SIZE + newstorage->app.bin_size);
			if (isresource)
			{
				storage_progress_str(newstorage, "load resource ...");
				storage_find_resource(*hstorage);
			}
		}
		storage_progress_pos(newstorage, 100);
	}
	return nres;
}


vatek_result vatek_storage_get_app(hvatek_storage hstorage, Papp_header* papp)
{
	Pstorage_entry pstorage = (Pstorage_entry)hstorage;
	if (pstorage->is_v2app)
	{
		*papp = &pstorage->app;
		return vatek_success;
	}
	return vatek_unsupport;
}

vatek_result vatek_storage_get_loader(hvatek_storage hstorage, Ploader_header* ploader)
{
	Pstorage_entry pstorage = (Pstorage_entry)hstorage;
	if (pstorage->is_v2app)
	{
		*ploader = &pstorage->loader;
		return vatek_success;
	}
	return vatek_unsupport;
}

Pr2_tune_handle vatek_storage_get_r2tune(hvatek_storage hstorage)
{
	Pstorage_entry pstorage = (Pstorage_entry)hstorage;
	if (pstorage->is_v2app)
		return &pstorage->r2tune;
	return NULL;
}

Pstorage_broadcast vatek_storage_get_broadcast(hvatek_storage hstorage)
{
	Pstorage_entry pstorage = (Pstorage_entry)hstorage;
	if (pstorage->is_v2app && pstorage->app.service_mode == service_broadcast)
		return &pstorage->_service.broadcast;
	return NULL;
}

Pstorage_transform vatek_storage_get_transform(hvatek_storage hstorage)
{
	Pstorage_entry pstorage = (Pstorage_entry)hstorage;
	if (pstorage->is_v2app && pstorage->app.service_mode == service_transform)
		return &pstorage->_service.transform;
	return NULL;
}

Pstorage_chip_config vatek_storage_get_config(hvatek_storage hstorage)
{
	Pstorage_entry pstorage = (Pstorage_entry)hstorage;
	if (pstorage->is_v2app)
		return &pstorage->chipconfig;
	return NULL;
}

vatek_result vatek_storage_get_resource(hvatek_storage hstorage, Pstorage_resource* pres)
{
	Pstorage_entry pstorage = (Pstorage_entry)hstorage;
	if (pstorage->is_v2app)
	{
		*pres = pstorage->resources;
		return vatek_success;
	}
	return vatek_unsupport;
}

vatek_result vatek_storage_save(hvatek_storage hstorage, const char* filename)
{
	vatek_result nres = vatek_badparam;
	Pstorage_entry pstorage = (Pstorage_entry)hstorage;
	
	FILE* fimage = fopen(filename, "wb+");
	if (fimage)
	{
		int32_t pos = 0;
		int32_t baselen = BINARY_SECTION_LEN(LOADER_SIZE + pstorage->app.bin_size);
		uint8_t* psection = storage_buf(hstorage);

		baselen /= BINARY_SECTION_SIZE;

		while (baselen)
		{
			nres = storage_read_section(hstorage, pos, psection);
			if (nres > vatek_success)
			{
				nres = (vatek_result)fwrite(psection, BINARY_SECTION_SIZE, 1, fimage);
				if (nres <= vatek_success)nres = vatek_hwfail;
			}
			else nres = vatek_memfail;
			if (!is_vatek_success(nres))break;
			baselen--;
			pos++;
		}

		if (is_vatek_success(nres))
		{
			if (pstorage->app.service_mode == service_broadcast)
				nres = storage_broadcast_set(&pstorage->_service.broadcast, psection);
			else if (pstorage->app.service_mode == service_transform)
				nres = storage_transform_set(&pstorage->_service.transform, psection);
			else nres = vatek_badstatus;

			if (is_vatek_success(nres))
			{
				nres = (vatek_result)fwrite(psection, BINARY_SECTION_SIZE, 1, fimage);
				if (nres <= vatek_success)nres = vatek_hwfail;
			}
		}

		if (is_vatek_success(nres))
		{	
			nres = storage_r2tune_set(&pstorage->r2tune, psection);
			if (is_vatek_success(nres))
			{
				nres = (vatek_result)fwrite(psection, BINARY_SECTION_SIZE, 1, fimage);
				if (nres <= vatek_success)nres = vatek_hwfail;
			}
			
		}

		if (is_vatek_success(nres))
		{
			const char* filename = "modulation.mudul";
			FILE* output_file = fopen(filename, "wb+");
			if (!output_file) {
				perror("fopen");
				exit(EXIT_FAILURE);
			}

			nres = storage_chip_config_set(&pstorage->chipconfig, psection);
			if (is_vatek_success(nres))
			{
				fwrite(&pstorage->chipconfig, sizeof(storage_chip_config), 1, output_file);
				fseek(output_file, SEEK_SET, 0);
				printf("Done Writing!\n");
				fclose(output_file);

				nres = (vatek_result)fwrite(psection, BINARY_SECTION_SIZE, 1, fimage);
				if (nres <= vatek_success)nres = vatek_hwfail;
			}
		}

		if (is_vatek_success(nres))
		{
			memset(psection, 0xFF, BINARY_SECTION_SIZE);
			if (storage_resource_root(hstorage) == NULL)
			{
				nres = fwrite(psection, BINARY_SECTION_SIZE, 1, fimage);
				if (nres <= vatek_success)nres = vatek_hwfail;
			}
			else
			{
				Pstorage_resource pres = storage_resource_root(hstorage);
				int32_t len;
				while (pres)
				{
					nres = filehandle_write_resource(hstorage, fimage, pres->resource, psection);
					if (!is_vatek_success(nres))break;
					pres = pres->next;
				}
			}
		}

		fclose(fimage);
		if (!is_vatek_success(nres))remove(filename);
	}
	return nres;
}

vatek_result vatek_storage_close(hvatek_storage hstorage)
{
	Pstorage_entry pstorage = (Pstorage_entry)hstorage;
	Pstorage_resource pres = storage_resource_root(hstorage);
	while (pres)
	{
		Pstorage_resource pnext = pres->next;
		storage_free(hstorage, pres);
		pres = pnext;
	}

	storage_free(hstorage, pstorage);
	return vatek_success;
}

vatek_result vatek_storage_dump(hvatek_storage hstorage, FILE* hfile)
{
	Pstorage_entry pstorage = (Pstorage_entry)hstorage;
	vatek_result nres = fseek(hfile, 0, SEEK_SET);
	if (is_vatek_success(nres))
	{
		int32_t slen = (pstorage->app.bin_size + LOADER_SIZE)/ BINARY_SECTION_SIZE;
		int32_t pos = 0;
		uint8_t* psection = pstorage->section_buf;
		storage_progress_str(hstorage, "dump rom_code ....");
		storage_progress_pos(hstorage, 0);

		while (pos < slen)
		{
			if (pos == STORAGE_PROTECT_1 || pos == STORAGE_PROTECT_0)
				memset(psection, 0xFF, BINARY_SECTION_SIZE);
			else nres = storage_read_section(hstorage, pos, psection);

			if (nres > vatek_success)
				nres = (vatek_result)fwrite(psection, BINARY_SECTION_SIZE, 1, hfile);
			else nres = vatek_memfail;

			if (!is_vatek_success(nres))break;
			pos++;
			storage_progress_pos(hstorage, ((pos * 100) / slen));
		}

		if (is_vatek_success(nres))
		{
			while (is_vatek_success(nres))
			{
				uint32_t sectiontag = 0;
				uint32_t sectionlen = 0;

				nres = storage_read_section(hstorage, pos, psection);
				if (nres > vatek_success)
					nres = storage_section_check_tag(psection, &sectiontag, &sectionlen);
				else break;

				if (is_vatek_success(nres))
				{
					int32_t rlen = (sectionlen / BINARY_SECTION_SIZE) - 1;
					char szres[32];
					sprintf(szres, "dump resource - [%08x-%d]", sectiontag, sectionlen);
					storage_progress_str(hstorage, &szres[0]);
					nres = (vatek_result)fwrite(psection, BINARY_SECTION_SIZE, 1, hfile);
					pos++;
					if (is_vatek_success(nres) && rlen)
					{
						while (rlen)
						{
							nres = storage_read_section(hstorage, pos, psection);
							if (nres > vatek_success)
								nres = (vatek_result)fwrite(psection, BINARY_SECTION_SIZE, 1, hfile);
							else nres = vatek_memfail;
							rlen--;
							pos++;
							if (!is_vatek_success(nres))break;
						}
					}
				}
				if (!is_vatek_success(nres))break;
			}
		}
	}
	return nres;
}

vatek_result vatek_storage_update(hvatek_storage hstorage, Promfile_handle pimage)
{
	vatek_result nres = vatek_format;
	uint8_t* psection = &storage_buf(hstorage)[0];

	nres = romfile_get_section(pimage, 0, psection);
	if (is_vatek_success(nres))
	{
		nres = storage_section_check_loader(psection);
		if(is_vatek_success(nres))
			nres = vatek_storage_write_image(hstorage, pimage);
		else
		{
			nres = storage_section_check_app(psection);
			if(is_vatek_success(nres))
				nres = vatek_storage_write_update(hstorage, pimage);
		}
	}
	return nres;
}

vatek_result vatek_storage_write_image(hvatek_storage hstorage, Promfile_handle pimage)
{
	vatek_result nres = vatek_format;
	uint8_t* psection = &storage_buf(hstorage)[0];

	storage_progress_str(hstorage, "verify image file ...");
	nres = romfile_get_section(pimage, 0, storage_buf(hstorage));
	if (is_vatek_success(nres) && nres > 0)
	{
		nres = storage_section_check_loader(psection);
		if (is_vatek_success(nres))
		{
			storage_progress_pos(hstorage, 50);
			nres = romfile_get_section(pimage, LOADER_SIZE / BINARY_SECTION_SIZE, storage_buf(hstorage));
			if (is_vatek_success(nres))
				nres = storage_section_check_app(psection);
			storage_progress_pos(hstorage, 100);
		}
	}

	if (is_vatek_success(nres))
	{
		int32_t section_len = romfile_get_length(pimage);
		nres = vatek_format;
		if (section_len % BINARY_SECTION_SIZE == 0)
		{
			int32_t section_pos = 0;
			section_len = section_len / BINARY_SECTION_SIZE;
			storage_progress_str(hstorage, "write image file ...");
			for (section_pos = 0; section_pos < section_len; section_pos++)
			{
				storage_progress_pos(hstorage, ((section_pos * 100) / section_len));
				nres = romfile_get_section(pimage, section_pos, storage_buf(hstorage));
				if (is_vatek_success(nres))
					nres = storage_write_section(hstorage, section_pos, storage_buf(hstorage));
				if (!is_vatek_success(nres))break;
			}

			if (is_vatek_success(nres))
			{
				memset(storage_buf(hstorage), 0xFF, BINARY_SECTION_SIZE);
				nres = storage_write_section(hstorage, section_pos, storage_buf(hstorage));
			}
			storage_progress_pos(hstorage, 100);
		}
	}

	return nres;
}

// only write application
vatek_result vatek_storage_write_app(hvatek_storage hstorage, Promfile_handle papp)
{
	vatek_result nres = vatek_format;
	uint8_t* psection = &storage_buf(hstorage)[0];

	storage_progress_str(hstorage, "verify app file ...");
	nres = romfile_get_section(papp, 0, storage_buf(hstorage));
	if (is_vatek_success(nres) && nres > 0)
	{
		nres = storage_section_check_loader(psection);
		if (is_vatek_success(nres))
		{
			storage_progress_pos(hstorage, 50);
			nres = romfile_get_section(papp, LOADER_SIZE / BINARY_SECTION_SIZE, storage_buf(hstorage));
			if (is_vatek_success(nres))
				nres = storage_section_check_app(psection);
			storage_progress_pos(hstorage, 100);
		}
	}

	if (is_vatek_success(nres))
	{
		int32_t section_len = romfile_get_length(papp);
		nres = vatek_format;
		if (section_len % BINARY_SECTION_SIZE == 0)
		{
			int32_t section_pos = 0;
			section_len = section_len / BINARY_SECTION_SIZE;
			storage_progress_str(hstorage, "write image file ...");
			for (section_pos = 16; section_pos < section_len; section_pos++)
			{
				storage_progress_pos(hstorage, ((section_pos * 100) / section_len));
				nres = romfile_get_section(papp, section_pos, storage_buf(hstorage));
				if (is_vatek_success(nres))
					nres = storage_write_section(hstorage, section_pos, storage_buf(hstorage));
				if (!is_vatek_success(nres))break;
			}

			if (is_vatek_success(nres))
			{
				memset(storage_buf(hstorage), 0xFF, BINARY_SECTION_SIZE);
				nres = storage_write_section(hstorage, section_pos, storage_buf(hstorage));
			}
			storage_progress_pos(hstorage, 100);
		}
	}

	return nres;
}

vatek_result vatek_storage_write_update(hvatek_storage hstorage, Promfile_handle pimage)
{
	vatek_result nres = vatek_format;
	uint8_t* psection = &storage_buf(hstorage)[0];

	storage_progress_str(hstorage, "verify update file ...");
	nres = romfile_get_section(pimage, 0, storage_buf(hstorage));
	if (is_vatek_success(nres))
	{
		nres = storage_section_check_app(psection);
		storage_progress_pos(hstorage, 100);
	}

	if (is_vatek_success(nres))
	{
		int32_t section_len = romfile_get_length(pimage);
		nres = vatek_format;
		if (section_len % BINARY_SECTION_SIZE == 0)
		{
			int32_t section_pos = 0;
			section_len  = section_len / BINARY_SECTION_SIZE;
			
			storage_progress_str(hstorage, "write update file ...");
			for (section_pos = 0; section_pos < section_len; section_pos++)
			{
				int32_t curpos = section_pos + (LOADER_SIZE / BINARY_SECTION_SIZE);
				storage_progress_pos(hstorage, ((section_pos * 100) / section_len));
				nres = romfile_get_section(pimage, curpos, psection);
				if (is_vatek_success(nres))
					nres = storage_write_section(hstorage, curpos, psection);
				if (!is_vatek_success(nres))break;
			}
			storage_progress_pos(hstorage, 100);
		}
	}

	return nres;
}

uint32_t vatek_storage_crc(uint8_t* pbuf, int32_t len)
{
	return tool_crc32(pbuf, len);
}

vatek_result vatek_storage_update_resource(hvatek_storage hstorage)
{
	vatek_result nres = vatek_badstatus;
	if (storage_is_valid(hstorage))
	{
		memset(storage_buf(hstorage), 0xFF, BINARY_SECTION_SIZE);
		if (storage_resource_root(hstorage) == NULL)
			nres = storage_write_section(hstorage, storage_resource_pos(hstorage) / BINARY_SECTION_SIZE, storage_buf(hstorage));
		else
		{
			Pstorage_resource pres = storage_resource_root(hstorage);
			uint32_t pos = storage_resource_pos(hstorage);
			int32_t len;
			while (pres)
			{
				len = sizeof(bin_section_header) + sizeof(bin_resource_section) + storage_getlen_resource(pres->resource);
				nres = storage_write_resource(hstorage, pos, storage_buf(hstorage), pres->resource);
				if (!is_vatek_success(nres))break;
				pos += BINARY_SECTION_LEN(len);
				pres = pres->next;
			}
		}
	}
	return nres;
}

vatek_result vatek_storage_del_resource(hvatek_storage hstorage, Pstorage_resource pres)
{
	Pstorage_resource pnext = storage_resource_root(hstorage);
	Pstorage_resource prev = NULL;
	int32_t bdel = 0;
	if (pnext == pres)
	{
		bdel = 1;
		storage_resource_root(hstorage) = storage_resource_root(hstorage)->next;
		if (storage_resource_root(hstorage) != NULL)
			storage_resource_root(hstorage)->resource->index--;
	}
	else
	{
		while (pnext)
		{
			if (!bdel)
			{
				if (pnext == pres)
				{
					prev->next = pres->next;
					pnext = prev->next;
					bdel = 1;
					continue;
				}
			}
			else pnext->resource->index--;
			prev = pnext;
			pnext = pnext->next;
		}
	}

	if (!bdel)return vatek_badparam;
	storage_free(hstorage,pres);
	return vatek_success;
}

vatek_result vatek_storage_add_resource(hvatek_storage hstorage, Pstorage_resource pres)
{
	int32_t nums = 0;
	Pstorage_resource pnext = storage_resource_root(hstorage);
	if (pnext == NULL)
	{
		pres->resource->index = 0;
		storage_resource_root(hstorage) = pres;
	}
	else
	{
		nums++;
		while (pnext->next)
		{
			nums++;
			pnext = pnext->next;
		}
		pres->resource->index = nums;
		pnext->next = pres;
	}
	return vatek_success;
}

vatek_result vatek_storage_resource_create(hvatek_storage hstorage, uint32_t w, uint32_t h, Pstorage_resource* pres)
{	/* malloc max resource buffer */
	vatek_result nres = vatek_memfail;
	int32_t len = sizeof(storage_resource) + sizeof(bin_resource_section) + RESOURCE_BUF_SIZE(w, h);
	uint8_t* rawbuf = storage_malloc(hstorage, len);
	if (rawbuf != NULL)
	{
		Pstorage_resource newres = (Pstorage_resource)&rawbuf[0];
		memset(rawbuf, 0, len);
		len = sizeof(storage_resource);

		newres->resource = (Pbin_resource_section)&rawbuf[len];
		len += sizeof(bin_resource_section);
		newres->resource->width = w;
		newres->resource->height = h;
		newres->resource->rawpixel = &rawbuf[len];
		
		
		
		*pres = newres;
		nres = vatek_success;

	}
	return nres;
}

vatek_result storage_get_app_header(hvatek_storage hstorage, Papp_header papp)
{
	uint8_t* psection = ((Pstorage_entry)hstorage)->section_buf;
	vatek_result nres = storage_read_section(hstorage, LOADER_SIZE / BINARY_SECTION_SIZE, psection);
	if (nres > vatek_success)
		nres = storage_section_get_app(psection, papp);
	else nres = vatek_hwfail;
	return nres;
}


vatek_result storage_get_loader_header(hvatek_storage hstorage, Ploader_header ploader)
{
	uint8_t* psection = ((Pstorage_entry)hstorage)->section_buf;
	vatek_result nres = storage_read_section(hstorage, 0, psection);
	if (nres > vatek_success)
		nres = storage_section_get_loader(psection, ploader);
	else nres = vatek_hwfail;
	return nres;
}

vatek_result storage_reset(hvatek_storage hstorage, hal_service_mode service)
{
	Pstorage_entry pstorage = (Pstorage_entry)hstorage;
	vatek_result nres = vatek_success;
	storage_chip_config_reset(&pstorage->chipconfig);
	
	//storage_r2tune_reset(&pstorage->r2tune);
	if (service == service_broadcast)
		storage_broadcast_reset(&pstorage->_service.broadcast);
	else if (service == service_transform)
		storage_transform_reset(&pstorage->_service.transform);
	else vatek_unsupport;
	return nres;
}

int32_t storage_getlen_resource(Pbin_resource_section pres)
{
	int32_t len = pres->width * pres->height;
	if (pres->table[RESIDX_TAG] == RESOURCE_RAW_TAG)
		len = RESOURCE_BUF_SIZE(pres->width, pres->height);
	return len;
}

vatek_result storage_write_resource(hvatek_storage hstorage, uint32_t pos, uint8_t* psection, Pbin_resource_section pres)
{
	uint8_t* ptrpixel = pres->rawpixel;
	int32_t len = sizeof(bin_section_header) + sizeof(bin_resource_section) + storage_getlen_resource(pres);
	vatek_result nres = vatek_success;
	uint32_t sectionpos = pos / BINARY_SECTION_SIZE;
	int32_t totalsection = len / BINARY_SECTION_SIZE;

	storage_progress_str(hstorage, "write resource ...");
	nres = storage_section_put_resource(psection, len, pres, ptrpixel);

	if (is_vatek_success(nres))
	{
		ptrpixel += nres;
		len -= (nres + sizeof(bin_section_header) + sizeof(bin_resource_section));
		nres = storage_write_section(hstorage, sectionpos++, psection);
	}

	if (is_vatek_success(nres))
	{
		int32_t nstep = 0;
		storage_progress_pos(hstorage, ((sectionpos * 100) / totalsection));
		while (len)
		{
			if (len > BINARY_SECTION_SIZE)nstep = BINARY_SECTION_SIZE;
			else nstep = len;

			if (nstep != BINARY_SECTION_SIZE)
				memset(storage_buf(hstorage), 0xFF, BINARY_SECTION_SIZE);
			memcpy(storage_buf(hstorage), ptrpixel, nstep);

			ptrpixel += nstep;
			len -= nstep;

			nres = storage_write_section(hstorage, sectionpos++, storage_buf(hstorage));
			if (!is_vatek_success(nres))break;
			storage_progress_pos(hstorage, ((sectionpos++ * 100) / totalsection));
		}
	}

	storage_progress_pos(hstorage, 100);
	return nres;
}

vatek_result filehandle_write_resource(hvatek_storage hstorage, FILE* fdest, Pbin_resource_section pres, uint8_t* psection)
{
	uint8_t* ptrpixel = pres->rawpixel;
	int32_t len = sizeof(bin_section_header) + sizeof(bin_resource_section) + storage_getlen_resource(pres);
	vatek_result nres = vatek_success;
	int32_t totalsection = len / BINARY_SECTION_SIZE;
	uint32_t pos = storage_resource_pos(hstorage);
	uint32_t sectionpos = 0;

	storage_progress_str(hstorage, "write resource ...");
	nres = storage_section_put_resource(psection, len, pres, ptrpixel);

	if (is_vatek_success(nres))
	{
		ptrpixel += 3040;
		len -= 3040 ;
		nres = fwrite(psection, BINARY_SECTION_SIZE, 1, fdest);
		if (nres <= vatek_success)nres = vatek_hwfail;
		//sectionpos++;
	}

	if (is_vatek_success(nres))
	{
		int32_t nstep = 0;
		storage_progress_pos(hstorage, ((sectionpos++ * 100) / totalsection));
		while (len)
		{
			if (len > BINARY_SECTION_SIZE)nstep = BINARY_SECTION_SIZE;
			else nstep = len;

			if (nstep != BINARY_SECTION_SIZE)
				memset(storage_buf(hstorage), 0xFF, BINARY_SECTION_SIZE);
			memcpy(storage_buf(hstorage), ptrpixel, nstep);

			ptrpixel += nstep;
			len -= nstep;

			nres = fwrite(psection,BINARY_SECTION_SIZE,1,fdest);
			if (nres <= vatek_success)nres = vatek_hwfail;
			//sectionpos++;
			if (!is_vatek_success(nres))break;
			storage_progress_pos(hstorage, ((sectionpos++ * 100) / totalsection));
		}
	}

	storage_progress_pos(hstorage, 100);
	return nres;
}

vatek_result storage_read_resource(hvatek_storage hstorage, uint8_t* psection, uint32_t pos, int32_t len, Pstorage_resource* pres)
{
	int32_t totalsection = len / BINARY_SECTION_SIZE;
	int32_t possection = 0;
	int32_t startsection = pos / BINARY_SECTION_SIZE;
	bin_resource_section tmpres;
	vatek_result nres = storage_section_get_resource(psection, &tmpres);

	if (is_vatek_success(nres))
	{
		Pstorage_resource newres = NULL;
		possection = nres;

		nres = vatek_storage_resource_create(hstorage, tmpres.width, tmpres.height, &newres);
		if (is_vatek_success(nres))
		{
			uint8_t* praw = &newres->resource->rawpixel[0];
			int32_t nlen = storage_getlen_resource(&tmpres);
			int32_t posraw = 0;
			int32_t rawlen = BINARY_SECTION_SIZE - possection;
			/* copy raw data in first section */

			memcpy(&newres->resource[0], &tmpres, sizeof(bin_resource_section));
			newres->resource->rawpixel = praw;
			memcpy(&praw[0], &psection[possection], rawlen);

			possection = startsection + 1;
			nlen -= rawlen;
			posraw += rawlen;

			storage_progress_pos(hstorage, 0);

	 		while (nlen)
			{
				nres = storage_read_section(hstorage, possection, psection);
				if (nres > vatek_success)
				{
					rawlen = BINARY_SECTION_SIZE;
					if (nlen < rawlen)rawlen = nlen;
					memcpy(&praw[posraw], &psection[0], rawlen);
					nlen -= rawlen;
					posraw += rawlen;
					possection++;
				}
				else nres = vatek_memfail;

				if (!is_vatek_success(nres))break;
				storage_progress_pos(hstorage, ((possection - startsection) * 100 / totalsection));
			}
			storage_progress_pos(hstorage, 100);
		}
		if (is_vatek_success(nres))*pres = newres;
	}
	return nres;
}

vatek_result storage_find_resource(hvatek_storage hstorage)
{
	Pstorage_entry pstorage = (Pstorage_entry)hstorage;

	uint8_t* psection = pstorage->section_buf;
	uint32_t startpos = pstorage->pos;
	Pstorage_resource presource = NULL;
	vatek_result nres = (vatek_result)1;

	pstorage->resource_pos = 0;

	while (is_vatek_success(nres))
	{
		uint32_t sectiontag = 0;
		uint32_t sectionlen = 0;

		nres = storage_read_section(hstorage, startpos / BINARY_SECTION_SIZE, psection);
		if (nres > vatek_success)
		{
			nres = storage_section_check_tag(psection, &sectiontag, &sectionlen);
			if (nres == vatek_badparam)
			{
				nres = vatek_success;
				break;
			}
		}
		else break;

		if (is_vatek_success(nres))
		{
			if (sectiontag == BIN_SECTION_RESOURCE)
			{
				Pstorage_resource newres = NULL;
				nres = storage_read_resource(hstorage, psection, startpos, sectionlen,&newres);


				if (is_vatek_success(nres))
				{
					if (pstorage->resource_pos == 0)
						pstorage->resource_pos = startpos;
					if (pstorage->resources == NULL)pstorage->resources = newres;
					else presource->next = newres;
					presource = newres;
				}
			}
			else if (sectiontag == BIN_SECTION_CONFIG) {
				nres = storage_chip_config_get(&pstorage->chipconfig, psection);
			}
			else if (sectiontag == BIN_SECTION_R2TUNE) {
				nres = storage_r2tune_get(&pstorage->r2tune, psection);
			}
			else if (sectiontag == BIN_SECTION_BROADCAST)
				nres = storage_broadcast_get(&pstorage->_service.broadcast, psection);
			else if (sectiontag == BIN_SECTION_TRANSFORM)
				nres = storage_transform_get(&pstorage->_service.transform, psection);
			else VWAR("unknown resource not load : [%08x:%08x]", sectiontag, sectionlen);
			if (is_vatek_success(nres))
				startpos = BINARY_SECTION_LEN(startpos + sectionlen);
		}
		if (!is_vatek_success(nres))break;
	}

	/* no resource found set position to last */
	if (pstorage->resource_pos == 0)
		pstorage->resource_pos = startpos;

	return nres;
}

/* stroage_handle implemented */

extern vatek_result device_read_section(int32_t nsection, uint8_t* psection, void* param);
extern vatek_result device_write_section(int32_t nsection, uint8_t* psection, void* param);
extern vatek_result devicebulk_read_section(int32_t nsection, uint8_t* psection, void* param);
extern vatek_result devicebulk_write_section(int32_t nsection, uint8_t* psection, void* param);
extern vatek_result image_read_section(int32_t nsection, uint8_t* psection, void* param);
extern vatek_result image_write_section(int32_t nsection, uint8_t* psection, void* param);
extern vatek_result bridge_read_section(int32_t nsection, uint8_t* psection, void* param);
extern vatek_result bridge_write_section(int32_t nsection, uint8_t* psection, void* param);

extern uint8_t* rom_malloc_memory(int32_t len, void* param);
extern void rom_free_memory(uint8_t* pptr, void* param);
extern void rom_process_cb(rom_progress_msg msg, void* progressval, void* param);

extern uint8_t* rom_malloc_memory(int32_t len, void* param);
extern void rom_free_memory(uint8_t* pptr, void* param);

extern vatek_result file_read_section(int32_t nsection, uint8_t* psection, void* param);
extern vatek_result file_get_length(void* param);
extern vatek_result filehandle_copy(FILE* fdest, FILE* fsource, uint8_t* psection,int32_t size);
extern vatek_result filehandle_offset(FILE* fdest, int32_t position, uint8_t* psection);

typedef enum _hstorage_mode
{
	hstorage_file,
	hstorage_bridge,
	hstorage_usb,
	hstorage_usbbulk,
}hstorage_mode, *Phstorage_mode;

typedef struct _storage_usbbulk
{
	hvatek_chip hchip;
	usbbulk_command cmd;
	usbbulk_result result;
}storage_usbbulk,*Pstorage_usbbulk;

typedef struct _hstorage_entry
{
	hstorage_mode mode;
	union
	{
		FILE* hfile;
		hvatek_chip hchip;
		hvatek_bridge hbridge;
		Pstorage_usbbulk husbbulk;
	}_handle;
	void* cbparam;
	fprom_progress cbhandler;
}hstorage_entry, *Phstorage_entry;

extern Pstorage_handle create_shandle(void);

vatek_result vatek_storage_create_chip_handle(hvatek_chip hchip, Pstorage_handle* phandle, fprom_progress fpcb, void* cbparam)
{
	vatek_result nres = vatek_memfail;
	Pstorage_handle newstorage = create_shandle();
	if (newstorage)
	{
		Phstorage_entry psentry = (Phstorage_entry)newstorage->param;
		uint32_t bus = vatek_device_get_bus(hchip);

		psentry->cbparam = cbparam;
		psentry->cbhandler = fpcb;
		newstorage->free = rom_free_memory;
		newstorage->malloc = rom_malloc_memory;
		newstorage->progress = rom_process_cb;

		nres = vatek_badparam;

		if (bus == DEVICE_BUS_BRIDGE)
		{
#if 0
			psentry->mode = hstorage_bridge;
			nres = vatek_bridge_open(hchip, &psentry->_handle.hbridge);
			if (is_vatek_success(nres))
			{
				newstorage->read = bridge_read_section;
				newstorage->write = bridge_write_section;
			}
#endif
			psentry->mode = hstorage_bridge;
			psentry->_handle.hchip = hchip;
			newstorage->read = device_read_section;
			newstorage->write = device_write_section;
			nres = vatek_success;
		}
		else if (bus == DEVICE_BUS_USB)
		{
			Pchip_info pinfo = vatek_device_get_info(hchip);
			if (pinfo->peripheral_en & PERIPHERAL_USBBULK)
			{
				psentry->mode = hstorage_usbbulk;
				psentry->_handle.husbbulk = (Pstorage_usbbulk)malloc(sizeof(storage_usbbulk));
				nres = vatek_memfail;
				if (psentry->_handle.husbbulk)
					memset(psentry->_handle.husbbulk, 0, sizeof(storage_usbbulk));
				psentry->_handle.husbbulk->hchip = hchip;
				newstorage->read = devicebulk_read_section;
				newstorage->write = devicebulk_write_section;
			}
			else
			{
				psentry->mode = hstorage_usb;
				psentry->_handle.hchip = hchip;
				newstorage->read = device_read_section;
				newstorage->write = device_write_section;
			}
			nres = vatek_success;
		}

		if (!is_vatek_success(nres))free(newstorage);
		else *phandle = newstorage;
	}
	return nres;
}

vatek_result vatek_storage_create_file_handle(const char* fimage, const char* floader, const char* fapp, Pstorage_handle* phandle, fprom_progress fpcb, void* cbparam)
{
	uint8_t* pbuf = malloc(BINARY_SECTION_SIZE);
	vatek_result nres = vatek_memfail;

	if (pbuf)
	{
		FILE* hloader = fopen(floader, "rb+");
		FILE* happ = fopen(fapp, "rb+");
		nres = vatek_badparam;

		if (happ && hloader)
		{
			FILE* hrom = fopen(fimage, "wb+");
			if (hrom)
			{
				nres = filehandle_copy(hrom, hloader,pbuf,-1);
				if (is_vatek_success(nres))
					nres = filehandle_offset(hrom, APP_POSITION,pbuf);
				if (is_vatek_success(nres))
					nres = filehandle_copy(hrom, happ, pbuf,-1);

				if (is_vatek_success(nres))
				{
					Pstorage_handle newstorage = create_shandle();
					nres = vatek_memfail;
					if (newstorage)
					{
						Phstorage_entry psentry = (Phstorage_entry)newstorage->param;
						psentry->cbparam = cbparam;
						psentry->cbhandler = fpcb;
						psentry->mode = hstorage_file;
						psentry->_handle.hfile = hrom;
						newstorage->free = rom_free_memory;
						newstorage->malloc = rom_malloc_memory;
						newstorage->progress = rom_process_cb;
						newstorage->read = image_read_section;
						newstorage->write = image_write_section;
						nres = vatek_success;
						*phandle = newstorage;
					}
				}

				if (!is_vatek_success(nres))
				{
					fclose(hrom);
					remove(fimage);
				}
			}
		}

		if (hloader)fclose(hloader);
		if (happ)fclose(happ);

		free(pbuf);
	}
	return nres;
}

vatek_result vatek_storage_open_file_handle(const char* filename, Pstorage_handle* phandle, fprom_progress fpcb, void* cbparam)
{
	FILE* hfile = fopen(filename, "rb+");
	vatek_result nres = vatek_hwfail;
	if (hfile)
	{
		Pstorage_handle newstorage = create_shandle();
		nres = vatek_memfail;
		if (newstorage)
		{
			Phstorage_entry psentry = (Phstorage_entry)newstorage->param;
			psentry->cbparam = cbparam;
			psentry->cbhandler = fpcb;
			psentry->mode = hstorage_file;
			psentry->_handle.hfile = hfile;
			newstorage->free = rom_free_memory;
			newstorage->malloc = rom_malloc_memory;
			newstorage->progress = rom_process_cb;
			newstorage->read = image_read_section;
			newstorage->write = image_write_section;
			nres = vatek_success;
			*phandle = newstorage;
		}
		if (!is_vatek_success(nres))fclose(hfile);
	}
	return nres;
}

void vatek_storage_free_handle(Pstorage_handle phandle)
{
	Phstorage_entry pentry = (Phstorage_entry)phandle->param;
	if (pentry->mode == hstorage_file)
		fclose(pentry->_handle.hfile);
#if 0
	else if (pentry->mode == hstorage_bridge)
		vatek_bridge_close(pentry->_handle.hbridge);
#endif
	else if (pentry->mode == hstorage_usbbulk)
		free(pentry->_handle.husbbulk);
	free(phandle);
}

vatek_result vatek_storage_romfile_create(const char* romfle, Promfile_handle* promfile)
{
	FILE* hfile = fopen(romfle, "rb+");
	vatek_result nres = vatek_badparam;
	if (hfile)
	{
		Promfile_handle newrom = (Promfile_handle)malloc(sizeof(romfile_handle));
		nres = vatek_memfail;
		if (newrom)
		{
			memset(newrom, 0, sizeof(romfile_handle));
			newrom->param = hfile;
			newrom->get_length = file_get_length;
			newrom->get_section = file_read_section;
			*promfile = newrom;
			nres = vatek_success;
		}

		if (!is_vatek_success(nres))fclose(hfile);
	}
	return nres;
}

vatek_result vatek_storage_romfile_free(Promfile_handle promfile)
{
	FILE* hfile = promfile->param;
	fclose(hfile);
	free(promfile);
	return vatek_success;
}

#define SHANDLE_LEN		(sizeof(hstorage_entry) + sizeof(storage_handle))

Pstorage_handle create_shandle(void)
{
	uint8_t* praw = (uint8_t*)malloc(SHANDLE_LEN);
	if (praw)
	{
		Pstorage_handle hstorage = (Pstorage_handle)praw;
		memset(praw, 0, SHANDLE_LEN);
		hstorage->param = &praw[sizeof(storage_handle)];
		return hstorage;
	}
	return NULL;
}

void rom_process_cb(rom_progress_msg msg, void* progressval, void* param)
{
	Pstorage_handle hstorage = (Pstorage_handle)param;
	Phstorage_entry psentry = (Phstorage_entry)hstorage->param;
	psentry->cbhandler(msg, progressval, psentry->cbparam);
}

vatek_result image_read_section(int32_t nsection, uint8_t* psection, void* param)
{
	Phstorage_entry prom = (Phstorage_entry)param;
	vatek_result nres = (vatek_result)fseek(prom->_handle.hfile, (long)nsection * BINARY_SECTION_SIZE, SEEK_SET);
	if (nres != vatek_success)nres = vatek_hwfail;
	else nres = (vatek_result)fread(psection, BINARY_SECTION_SIZE, 1, prom->_handle.hfile);
	return nres;
}

vatek_result image_write_section(int32_t nsection, uint8_t* psection, void* param)
{
	Phstorage_entry prom = (Phstorage_entry)param;
	vatek_result nres = (vatek_result)fseek(prom->_handle.hfile, (long)nsection * BINARY_SECTION_SIZE, SEEK_SET);
	if (nres != vatek_success)nres = vatek_hwfail;
	else nres = (vatek_result)fwrite(psection, BINARY_SECTION_SIZE, 1, prom->_handle.hfile);
	return nres;
}

#define SECTION_W	(BINARY_SECTION_SIZE >> 2)

vatek_result device_read_section(int32_t nsection, uint8_t* psection, void* param)
{
	uint32_t val = 0;
	Phstorage_entry prom = (Phstorage_entry)param;
	hvatek_chip hchip = prom->_handle.hchip;
	vatek_result nres = vatek_chip_read_memory(hchip, HALREG_RESCUE_CNTL, &val);
	if (is_vatek_success(nres))
	{
		nres = vatek_badstatus;
		if (val == 0)
		{
			val = RESCUE_CNTL_READ | (nsection & RESCUE_SECTION_MASK);
			nres = vatek_chip_write_memory(hchip, HALREG_RESCUE_CNTL, val);
		}

		if (is_vatek_success(nres))
		{
			do
			{
				cross_os_sleep(10);
				nres = vatek_chip_read_memory(hchip, HALREG_RESCUE_CNTL, &val);
				if (is_vatek_success(nres))
				{
					if (val == 0)
					{
						nres = vatek_chip_read_memory(hchip, HALREG_RESCUE_RESULT, &val);
						if (is_vatek_success(nres))
						{
							if (val != RESCUE_ERR_SUCCESS)nres = vatek_hwfail;
							else
							{
								nres = vatek_chip_read_buffer(hchip, HALREG_RESCUE_DATA, psection, SECTION_W);
								if (is_vatek_success(nres))
									nres = vatek_chip_read_memory(hchip, HALREG_RESCUE_CRC32, &val);
							}
						}
						break;
					}
				}
			} while (is_vatek_success(nres));

			if (is_vatek_success(nres))
			{
				if (vatek_storage_crc(psection, BINARY_SECTION_SIZE) != val)nres = vatek_crcfail;
				else return (vatek_result)1;
			}
		}
	}
	return nres;
}


vatek_result device_write_section(int32_t nsection, uint8_t* psection, void* param)
{
	uint32_t val = 0;
	Phstorage_entry prom = (Phstorage_entry)param;
	hvatek_chip hchip = prom->_handle.hchip;
	vatek_result nres = vatek_chip_read_memory(hchip, HALREG_RESCUE_CNTL, &val);
	if (is_vatek_success(nres))
	{
		nres = vatek_badstatus;
		if (val == 0)
		{
			val = vatek_storage_crc(psection, BINARY_SECTION_SIZE);
			nres = vatek_chip_write_memory(hchip, HALREG_RESCUE_CRC32, val);
			if (is_vatek_success(nres))
			{
				nres = vatek_chip_write_buffer(hchip, HALREG_RESCUE_DATA, psection, SECTION_W);
				if (is_vatek_success(nres))
				{
					val = RESCUE_CNTL_WRITE | (nsection & RESCUE_SECTION_MASK);
					nres = vatek_chip_write_memory(hchip, HALREG_RESCUE_CNTL, val);
				}
			}
		}

		if (is_vatek_success(nres))
		{
			do
			{
				cross_os_sleep(10);
				nres = vatek_chip_read_memory(hchip, HALREG_RESCUE_CNTL, &val);
				if (is_vatek_success(nres))
				{
					if (val == 0)
					{
						nres = vatek_chip_read_memory(hchip, HALREG_RESCUE_RESULT, &val);
						if (is_vatek_success(nres))
						{
							if (val != RESCUE_ERR_SUCCESS)
								nres = vatek_hwfail;
						}
						break;
					}
				}
			} while (is_vatek_success(nres));
		}
	}
	return nres;
}

vatek_result bridge_read_section(int32_t nsection, uint8_t* psection, void* param)
{
	Phstorage_entry prom = (Phstorage_entry)param;
	vatek_result nres = vatek_bridge_read_section(prom->_handle.hbridge, nsection, psection);
	if (is_vatek_success(nres))nres = (vatek_result)1;
	return nres;
}

vatek_result bridge_write_section(int32_t nsection, uint8_t* psection, void* param)
{
	Phstorage_entry prom = (Phstorage_entry)param;
	return vatek_bridge_write_section(prom->_handle.hbridge, nsection, psection);
}

vatek_result devicebulk_read_section(int32_t nsection, uint8_t* psection, void* param)
{
	Phstorage_entry prom = (Phstorage_entry)param;
	Pstorage_usbbulk pbulk = prom->_handle.husbbulk;
	vatek_result nres = vatek_success;
	pbulk->cmd.mode = usbbulk_flash;
	pbulk->cmd._h.flash.mode = usbflash_read;
	pbulk->cmd._h.flash.section = nsection;

	nres = vatek_device_usbbulk_send(pbulk->hchip, &pbulk->cmd, &pbulk->result, psection, BINARY_SECTION_SIZE);
	if (is_vatek_success(nres))
	{
		if (is_vatek_success(pbulk->result.result))nres = (vatek_result)1;
		else nres = pbulk->result.result;
	}
	return nres;
}

vatek_result devicebulk_write_section(int32_t nsection, uint8_t* psection, void* param)
{
	Phstorage_entry prom = (Phstorage_entry)param;
	Pstorage_usbbulk pbulk = prom->_handle.husbbulk;
	vatek_result nres = vatek_success;
	pbulk->cmd.mode = usbbulk_flash;
	pbulk->cmd._h.flash.mode = usbflash_write;
	pbulk->cmd._h.flash.section = nsection;

	nres = vatek_device_usbbulk_send(pbulk->hchip, &pbulk->cmd, &pbulk->result, psection, BINARY_SECTION_SIZE);
	if (is_vatek_success(nres))
	{
		if (is_vatek_success(pbulk->result.result))nres = (vatek_result)1;
		else nres = pbulk->result.result;
	}
	return nres;
}

uint8_t* rom_malloc_memory(int32_t len, void* param)
{
	return (uint8_t*)malloc(len);
}

void rom_free_memory(uint8_t* pptr, void* param)
{
	free(pptr);
}

vatek_result file_read_section(int32_t nsection, uint8_t* psection, void* param)
{
	FILE* hfile = (FILE*)param;
	vatek_result nres = fseek(hfile, nsection * BINARY_SECTION_SIZE, SEEK_SET);
	if (nres == 0)nres = fread(psection, BINARY_SECTION_SIZE, 1, hfile);
	else nres = vatek_hwfail;
	return nres;
}

vatek_result file_get_length(void* param)
{
	FILE* hfile = (FILE*)param;
	vatek_result nres = fseek(hfile, 0, SEEK_END);
	if (nres == 0)nres = (vatek_result)ftell(hfile);
	else nres = vatek_hwfail;
	return nres;
}

vatek_result filehandle_copy(FILE* fdest, FILE* fsource, uint8_t* psection,int32_t size)
{
	int32_t len = 0;
	vatek_result nres = (vatek_result)fseek(fsource, 0, SEEK_END);
	if (is_vatek_success(nres))
	{
		len = (int32_t)ftell(fsource);
		if (size != -1)
		{
			if (size > len)nres = vatek_badparam;
			else if (len > size)len = size;
		}
	}
	if (is_vatek_success(nres))
		nres = (vatek_result)fseek(fsource, 0, SEEK_SET);

	if (is_vatek_success(nres))
	{
		while (len)
		{
			int32_t neach = BINARY_SECTION_SIZE;
			if (len < BINARY_SECTION_SIZE)
			{
				memset(psection, 0xFF, BINARY_SECTION_SIZE);
				neach = len;
			}
			nres = (vatek_result)fread(psection, neach, 1, fsource);
			if (nres > 0)nres = (vatek_result)fwrite(psection, BINARY_SECTION_SIZE, 1, fdest);
			else nres = vatek_hwfail;

			if (!is_vatek_success(nres))break;
			len -= neach;
		}
	}
	return nres;
}

vatek_result filehandle_offset(FILE* fdest, int32_t position, uint8_t* psection)
{
	vatek_result nres = fseek(fdest, 0, SEEK_END);
	int32_t len = (int32_t)ftell(fdest);
	if (is_vatek_success(nres))
	{
		if (len > position || ((len % BINARY_SECTION_SIZE) != 0))nres = vatek_badparam;
		else
		{
			memset(psection, 0xFF, BINARY_SECTION_SIZE);
			while (len < position)
			{
				nres = (vatek_result)fwrite(psection, BINARY_SECTION_SIZE, 1, fdest);
				if (nres > 0)len += BINARY_SECTION_SIZE;
				else nres = vatek_hwfail;

				if (!is_vatek_success(nres))break;
			}
		}
	}
	return nres;
}
