#ifndef _STREAM_HANDLE_
#define _STREAM_HANDLE_

#include <cross/cross_stream.h>
#include <mux/mux_define.h>

#define TSSLICE_BUFFER_LEN			CHIP_STREAM_SLICE_LEN
#define TSSLICE_PACKET_NUM			(TSSLICE_BUFFER_LEN/TS_PACKET_LEN)

typedef void* hcstream;
typedef vatek_result (*fpcstream_start)(hcstream hstream);
typedef vatek_result (*fpcstream_get_slice)(hcstream hstream, uint8_t** pslice);
typedef uint32_t(*fpcstream_get_bitrate)(hcstream hstream);
typedef void (*fpcstream_stop)(hcstream hstream);
typedef void (*fpcstream_close)(hcstream hstream);

typedef struct _cstream_handler
{
	hcstream hstream;
	fpcstream_start start;
	fpcstream_get_slice get_slice;
	fpcstream_get_bitrate get_bitrate;
	fpcstream_stop stop;
	fpcstream_close close;
}cstream_handler,*Pcstream_handler;

#ifdef __cplusplus
extern "C" {
#endif

	vatek_result cross_stream_file_get(const char* filename, Pcstream_handler* pcstream);
	vatek_result cross_stream_udp_get(const char* url, Pcstream_handler* pcstream);
	vatek_result cross_stream_test_get(uint32_t bitrate, Pcstream_handler* pcstream);

	uint8_t* tspacket_get_pcr(Pmux_time_tick ptime);
	uint8_t* tspacket_get_null(void);
	uint8_t* tspacket_get_suffing(void);


#ifdef __cplusplus
}
#endif

#endif
