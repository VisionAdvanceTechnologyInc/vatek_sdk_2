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

typedef struct _handle_file
{
	cstream_handler handle;
	int32_t packet_len;
	int32_t file_size;
	FILE* fhandle;
	uint8_t buffer[TSSLICE_BUFFER_LEN + 16];	/* both 188 and 204 packet len */
	uint32_t packetnums;
	uint32_t tick;
}handle_file, * Phandle_file;

extern vatek_result file_lock(Phandle_file pfile);
extern vatek_result file_check_sync(FILE* hfile, int32_t pos, int32_t offset);

extern vatek_result cstream_file_start(hcstream hstream);
extern vatek_result cstream_file_get_slice(hcstream hstream, uint8_t** pslice);
extern uint32_t cstream_file_get_bitrate(hcstream hstream);
extern void cstream_file_stop(hcstream hstream);
extern void cstream_file_close(hcstream hstream);

vatek_result cross_stream_file_get(const char* filename, Pcstream_handler* pcstream)
{
	Phandle_file pfile = (Phandle_file)malloc(sizeof(handle_file));
	vatek_result nres = vatek_memfail;
	
	if (pfile)
	{
		memset(pfile, 0, sizeof(handle_file));
		pfile->fhandle = fopen(filename, "rb");
		nres = vatek_format;
		if (pfile->fhandle)
		{
			fseek(pfile->fhandle, 0, SEEK_END);
			pfile->file_size = (int32_t)ftell(pfile->fhandle);
			fseek(pfile->fhandle, 0, SEEK_SET);

			nres = file_lock(pfile);
			if (is_vatek_success(nres))
			{
				pfile->handle.hstream = pfile;
				pfile->handle.start = cstream_file_start;
				pfile->handle.get_slice = cstream_file_get_slice;
				pfile->handle.get_bitrate = cstream_file_get_bitrate;
				pfile->handle.stop = cstream_file_stop;
				pfile->handle.close = cstream_file_close;
				*pcstream = &pfile->handle;
			}
			if (!is_vatek_success(nres))fclose(pfile->fhandle);
		}
		if(!is_vatek_success(nres))free(pfile);
	}
	return nres;
}

vatek_result cstream_file_start(hcstream hstream)
{
	Phandle_file pfile = (Phandle_file)hstream;
	if (pfile->fhandle)
	{
		cross_os_sleep(1000);					//wait A3 chip ready
		pfile->tick = cross_os_get_tick_ms();
		return vatek_success;
	}
	return vatek_badstatus;
}

uint32_t cstream_file_get_bitrate(hcstream hstream)
{
	Phandle_file pfile = (Phandle_file)hstream;
	int32_t eclipse = cross_os_get_tick_ms() - pfile->tick;
	if (eclipse)
	{
		uint32_t bitrate = (pfile->packetnums * TS_PACKET_LEN * 8 * 1000) / eclipse;
		pfile->tick = cross_os_get_tick_ms();
		pfile->packetnums = 0;
		return bitrate;
	}
	return 0;
}

vatek_result cstream_file_get_slice(hcstream hstream, uint8_t** pslice)
{
	Phandle_file pfile = (Phandle_file)hstream;
	int32_t pos = 0;
	uint8_t* ptr = &pfile->buffer[0];
	vatek_result nres = vatek_success;

	while (pos < TSSLICE_PACKET_NUM)
	{
		nres = (vatek_result)fread(ptr, pfile->packet_len, 1, pfile->fhandle);
		if (nres == 0)
		{
			fseek(pfile->fhandle, 0, SEEK_SET);
			nres = file_lock(pfile);
			if(is_vatek_success(nres))continue;
		}
		else if (nres == 1)
		{
			if (ptr[0] == TS_PACKET_SYNC_TAG)
			{
				pos++;
				ptr += TS_PACKET_LEN;
			}
			else nres = file_lock(pfile);
		}
		if (!is_vatek_success(nres))break;
	}

	if (is_vatek_success(nres))
	{
		pfile->packetnums += TSSLICE_PACKET_NUM;
		*pslice = &pfile->buffer[0];
		nres = (vatek_result)1;
	}
	return nres;
}

void cstream_file_stop(hcstream hstream)
{
	Phandle_file pfile = (Phandle_file)hstream;
	if (pfile->fhandle)
	{

	}
}

void cstream_file_close(hcstream hstream)
{
	Phandle_file pfile = (Phandle_file)hstream;
	cstream_file_stop(hstream);
	if (pfile->fhandle)
	{
		fclose(pfile->fhandle);
		pfile->fhandle = NULL;
	}
	free(pfile);
}

vatek_result file_lock(Phandle_file pfile)
{
	vatek_result nres = vatek_badstatus;
	uint8_t sync;
	uint32_t count = 0;

	for (;;)
	{
		size_t pos = ftell(pfile->fhandle);
		nres = (vatek_result)fread(&sync, 1, 1, pfile->fhandle);
		if (nres != 1)nres = vatek_hwfail;
		else if (nres == 0)nres = vatek_badstatus;
		else
		{
			if (sync == TS_PACKET_SYNC_TAG)
			{
				pfile->packet_len = 0;
				nres = file_check_sync(pfile->fhandle, (int32_t)pos, TS_PACKET_LEN);
				if (is_vatek_success(nres))pfile->packet_len = TS_PACKET_LEN;
				else
				{
					nres = file_check_sync(pfile->fhandle, (int32_t)pos, 204);
					if (is_vatek_success(nres))pfile->packet_len = 204;
				}
				if (nres == vatek_format)nres = vatek_success;
				else if (pfile->packet_len != 0)
				{
					nres = (vatek_result)fseek(pfile->fhandle, (int32_t)pos, SEEK_SET);
					break;
				}
			}
		}

		if (!is_vatek_success(nres))break;
		count++;
		if (count > 1000)return vatek_timeout;
	}
	return nres;
}

vatek_result file_check_sync(FILE* hfile, int32_t pos, int32_t offset)
{
	vatek_result nres = (vatek_result)fseek(hfile, pos + offset, SEEK_SET);
	if (is_vatek_success(nres))
	{
		uint8_t tag = 0;
		nres = (vatek_result)fread(&tag, 1, 1, hfile);
		if (nres == 1)
		{
			if (tag == TS_PACKET_SYNC_TAG)nres = (vatek_result)1;
			else nres = vatek_format;
		}
		else nres = vatek_hwfail;
	}

	return nres;
}