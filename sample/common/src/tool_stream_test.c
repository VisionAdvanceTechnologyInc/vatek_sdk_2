#include "../inc/tool_stream.h"
#include "../inc/tool_printf.h"
#include "../inc/tool_tspacket.h"

extern vatek_result test_stream_start(hstream_source hsource);
extern vatek_result test_stream_check(hstream_source hsource);
extern uint8_t* test_stream_get(hstream_source hsource);
extern vatek_result test_stream_stop(hstream_source hsource);
extern void test_stream_free(hstream_source hsource);

typedef struct _handle_test
{
	mux_time_tick time;
	uint32_t slice_ns;
	int32_t file_size;
	uint8_t buffer[CHIP_STREAM_SLICE_LEN];
}handle_test,*Phandle_test;

vatek_result stream_source_test_get(Pmodulator_param pmod, Ptsstream_source psource)
{
	uint32_t bitrate = modulator_param_get_bitrate(pmod);
	Phandle_test ptest = (Phandle_test)malloc(sizeof(handle_test));
	vatek_result nres = vatek_memfail;

	if (ptest)
	{
		memset(ptest, 0, sizeof(handle_test));
		ptest->slice_ns = (1000000000 / (bitrate / (TS_PACKET_LEN * 8))) * CHIP_STREAM_SLICE_PACKET_NUMS;
		_disp_l("open test_stream : %d bps - %d ns", bitrate, ptest->slice_ns);
		
		psource->hsource = ptest;
		psource->start = test_stream_start;
		psource->stop = test_stream_stop;
		psource->check = test_stream_check;
		psource->get = test_stream_get;
		psource->free = test_stream_free;
		nres = vatek_success;
	}
	return nres;
}

vatek_result test_stream_start(hstream_source hsource)
{
	Phandle_test ptest = (Phandle_test)hsource;
	ptest->time.ms = 0;
	ptest->time.ns = 0;
	return vatek_success;
}

vatek_result test_stream_check(hstream_source hsource)
{
	return (vatek_result)1;
}

uint8_t* test_stream_get(hstream_source hsource)
{
	Phandle_test ptest = (Phandle_test)hsource;
	int32_t nums = 0;
	uint8_t* ptr = &ptest->buffer[0];

	while (nums < CHIP_STREAM_SLICE_PACKET_NUMS)
	{
		if(nums == 0)memcpy(ptr, tspacket_get_pcr(&ptest->time), TS_PACKET_LEN);
		else memcpy(ptr, tspacket_get_suffing(), TS_PACKET_LEN);
		ptr += TS_PACKET_LEN;
		nums++;
	}
	ptest->time.ns += ptest->slice_ns;
	if (ptest->time.ns >= 1000000)
	{
		ptest->time.ms += ptest->time.ns / 1000000;
		ptest->time.ns = ptest->time.ns % 1000000;
	}
	return &ptest->buffer[0];
}

vatek_result test_stream_stop(hstream_source hsource)
{
	return vatek_success;
}

void test_stream_free(hstream_source hsource)
{
	free(hsource);
}
