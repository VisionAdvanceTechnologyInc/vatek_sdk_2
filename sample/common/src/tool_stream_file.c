
#include "../inc/tool_stream.h"
#include "../inc/tool_printf.h"
#include "../inc/tool_tspacket.h"


extern vatek_result file_stream_start(hstream_source hsource);
extern vatek_result file_stream_check(hstream_source hsource);
extern uint8_t* file_stream_get(hstream_source hsource);
extern vatek_result file_stream_stop(hstream_source hsource);
extern void file_stream_free(hstream_source hsource);

typedef struct _handle_file
{
	int32_t packet_len;
	int32_t file_size;
	FILE* fhandle;
	uint8_t buffer[CHIP_STREAM_SLICE_LEN];
}handle_file,*Phandle_file;

extern vatek_result file_lock(Phandle_file pfile);
extern vatek_result file_check_sync(FILE* hfile, int32_t pos, int32_t offset);

vatek_result stream_source_file_get(const char* file, Ptsstream_source psource)
{
	Phandle_file pfile = (Phandle_file)malloc(sizeof(handle_file));
	vatek_result nres = vatek_memfail;
	if (pfile)
	{
		memset(pfile, 0, sizeof(handle_file));
		pfile->fhandle = fopen(file, "rb+");
		nres = vatek_format;
		if (pfile->fhandle)
		{
			fseek(pfile->fhandle, 0, SEEK_END);
			pfile->file_size = (int32_t)ftell(pfile->fhandle);
			fseek(pfile->fhandle, 0, SEEK_SET);

			nres = file_lock(pfile);
			if (!is_vatek_success(nres))
			{
				fclose(pfile->fhandle);
				free(pfile);
			}
			else
			{
				psource->hsource = pfile;
				psource->start = file_stream_start;
				psource->stop = file_stream_stop;
				psource->get = file_stream_get;
				psource->check = file_stream_check;
				psource->free = file_stream_free;
				_disp_l("open file - [%s] - %d - %d", file, pfile->packet_len,pfile->file_size);
			}
		}
		if(!is_vatek_success(nres))
			_disp_err("file not current ts format - [%s]", file);
	}
	return nres;
}

vatek_result file_stream_start(hstream_source hsource)
{
	return vatek_success;
}

vatek_result file_stream_check(hstream_source hsource)
{
	Phandle_file pfile = (Phandle_file)hsource;
	int32_t pos = 0;
	uint8_t* ptr = &pfile->buffer[0];
	vatek_result nres = vatek_success;

	while (pos < CHIP_STREAM_SLICE_PACKET_NUMS)
	{
		nres = (vatek_result)fread(ptr, pfile->packet_len, 1, pfile->fhandle);
		if (nres == 0)nres = vatek_badstatus;
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

	if (is_vatek_success(nres))nres = pos;
	return nres;
}

uint8_t* file_stream_get(hstream_source hsource)
{
	Phandle_file pfile = (Phandle_file)hsource;
	return &pfile->buffer[0];
}

vatek_result file_stream_stop(hstream_source hsource)
{
	return vatek_success;
}

void file_stream_free(hstream_source hsource)
{
	Phandle_file pfile = (Phandle_file)hsource;
	fclose(pfile->fhandle);
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