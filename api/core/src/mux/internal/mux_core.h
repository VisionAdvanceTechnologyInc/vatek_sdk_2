
#ifndef _MUX_CORE_
#define _MUX_CORE_

#include <mux/mux_spec.h>
#include <mux/mux_rule.h>
#include <mux/mux_epg.h>

#define MUX_CORE_MEMPOOL_SIZE	128*1024

struct _handle_mux;

typedef struct _psiraw_handle
{
	Ppsi_table_raw lasttable;
	Ppsi_table_raw rawtables;
}psiraw_handle, *Ppsiraw_handle;

typedef struct _psispec_stream
{
	mux_stream mstream;
	void* private_data;
	struct _handle_mux* hpsi;
}psispec_stream, *Ppsispec_stream;

typedef struct _psispec_program
{
	mux_program mprogram;
	Pspec_program specprogram;
	Pmux_epg_param pepg;
	Pmux_stream laststream;
	struct _handle_mux* hpsi;
}psispec_program, *Ppsispec_program;

typedef struct _psispec_handle
{
	Pspec_channel specchannel;
	Pmux_program lastprogram;
	Pmux_program programs;
}psispec_handle, *Ppsispec_handle;

typedef struct _psiparam_buffer
{
	int32_t ch_len;
	uint8_t* ch_buf;
	int32_t prog_len;
	uint8_t* prog_buf;
}psiparam_buffer,*Ppsiparam_buffer;

typedef enum _handle_mux_mode
{
	hmux_mode_null,
	hmux_mode_raw,
	hmux_mode_broadcast,
	hmux_mode_channel,
}handle_mux_mode;

typedef struct _handle_raw
{
	psiraw_handle rawtables;
}handle_raw,*Phandle_raw;

typedef struct _handle_channel
{
	mux_spec_mode spec;
	mux_country_code country;
	psiparam_buffer buffer;
	psispec_handle context;
}handle_channel,*Phandle_channel;

typedef struct _handle_mux
{
	Pth_mempool mempool;
	handle_mux_mode hmode;
	mux_psi_mode mode;
	Pmux_rule_block rule;
	uint32_t index;
	union 
	{
		void* handle;
		Phandle_raw raw;
		Phandle_channel channel;
	}_h;
}handle_mux, *Phandle_mux;

#ifdef __cplusplus
extern "C" {
#endif

	int32_t mux_check_idle(Phandle_mux pmux);
	void mux_reset(Phandle_mux pmux);

	vatek_result mux_malloc_psiparam_buffer_rule(Pth_mempool pmem,Pmux_rule_block prule,Ppsiparam_buffer pbuffer);
	vatek_result mux_malloc_psiparam_buffer_default(Pth_mempool pmem,Pspec_channel pch,Ppsiparam_buffer pbuffer);

#ifdef __cplusplus
}
#endif

#endif
