
#ifndef _MUX_USBSTREAM_
#define _MUX_USBSTREAM_

#include <vatek_sdk_usbstream.h>
#include <vatek_sdk_usbmux.h>
#include <mux/mux_handle.h>
#include <mux/mux_packet.h>
#include <cross/cross_os_api.h>

#define HUSBPES_FRAME_LEN			(1 << 12)
#define HUSBPES_FRAME_MASK			(HUSBPES_FRAME_LEN - 1)

struct _handle_muxer;

typedef struct _umuxer_stream
{
	Pusbmux_stream umstream;
	uint32_t queryms;
	Pmux_pes_frame frames[HUSBPES_FRAME_LEN];
	int32_t nptrw;
	int32_t nptrr;
	pes_source pessource;
	Ppes_packet pespacket;
	struct _handle_muxer* muxer;
}umuxer_stream, *Pumuxer_stream;

typedef struct _umuxer_tick
{
	uint32_t pcr_packetnums;
	uint32_t pcr_pos;
	uint32_t packet_ns;
	uint32_t slice_ns;
	//uint32_t tick_ms;
	//uint32_t tick_ns;
	mux_time_tick tick;
}umuxer_tick, *Pumuxer_tick;

typedef struct _handle_muxer
{
	hvatek_usbstream hustream;
	Pusbmux_source source;
	usbmux_status status;
	hcross_thread hthread;
	hcross_mutex hlock;
	umuxer_tick streamtick;
	int32_t latencyms;
	Ppcr_packet pcrpacket;
	Pnull_packet nullpacket;
	int32_t stream_pktnums;
	int32_t stream_nums;
	Pumuxer_stream streams;
}handle_muxer, *Phandle_muxer;

#ifdef __cplusplus
extern "C" {
#endif

	vatek_result usbstream_muxer_create(Pth_mempool pmem,Pusbmux_param param,int32_t nums, Phandle_muxer* pmuxer);
	void usbstream_muxer_query_clean(Phandle_muxer pmuxer);
	vatek_result usbstream_muxer_close(Phandle_muxer pmuxer);

	vatek_result usbstream_muxer_start(Phandle_muxer pmuxer, Pusbmux_source psource, hvatek_usbstream hustream);
	void usbstream_muxer_stop(Phandle_muxer pmuxer);

	vatek_result usbstream_muxer_set_stream(Pth_mempool pmem,Phandle_muxer pmuxer, int32_t idx,hmux_stream hstream, Pusbmux_stream pstream);
	vatek_result usbstream_muxer_put_frame(Phandle_muxer pmuxer,int32_t idx, Pmux_pes_frame pframe);

#ifdef __cplusplus
}
#endif

#endif
