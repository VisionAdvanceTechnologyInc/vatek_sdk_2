
#ifndef _TOOL_STREAM_
#define _TOOL_STREAM_

#include <core/base/output_modulator.h>

typedef void* hstream_source;
typedef vatek_result(*fpstream_source_start)(hstream_source hsource);
typedef vatek_result(*fpstream_source_check)(hstream_source hsource);
typedef uint8_t*(*fpstream_source_get)(hstream_source hsource);
typedef vatek_result(*fpstream_source_stop)(hstream_source hsource);
typedef void(*fpstream_source_free)(hstream_source hsource);

typedef struct _tsstream_source
{
	hstream_source hsource;
	fpstream_source_start start;
	fpstream_source_check check;
	fpstream_source_get get;
	fpstream_source_stop stop;
	fpstream_source_free free;
}tsstream_source,*Ptsstream_source;

#ifdef __cplusplus
extern "C" {
#endif

	vatek_result stream_source_test_get(Pmodulator_param pmod, Ptsstream_source psource);
	vatek_result stream_source_file_get(const char* file, Ptsstream_source psource);
	vatek_result stream_source_udp_get(const char* ipaddr, Ptsstream_source psource);

#ifdef __cplusplus
}
#endif

#endif
