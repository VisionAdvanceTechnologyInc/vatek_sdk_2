#ifndef _CROSS_SIMULATION_TOOL_
#define _CROSS_SIMULATION_TOOL_

#include <cross/cross_os_api.h>
#include <core/base/chip_define.h>

typedef void* hsim_service;

#define SIM_MAPMEM_SIZE				64*1024

struct _sim_handle;

typedef enum _sim_status
{
	sim_idle,
	sim_running,
	sim_stopping,
}sim_status;

typedef vatek_result(*fpsimservice_open)(struct _sim_handle* psim, hvatek_chip hchip, hsim_service* hservice);
typedef vatek_result(*fpsimservice_polling)(hsim_service hsiim);
typedef void (*fpsimservice_close)(hsim_service hsiim);

typedef struct _sim_service
{
	fpsimservice_open open;
	fpsimservice_polling polling;
	fpsimservice_close close;
}sim_service, * Psim_service;

typedef struct _sim_handle
{
	vatek_ic_module chip;
	hal_service_mode service;
	uint8_t* mapmem;
	hcross_mutex lock;
	sim_status status;
	hcross_thread hsimrun;
	FILE* fstream;
	chip_info info;
	hsim_service hservice;
	Psim_service handler;
}sim_handle, * Psim_handle;

#ifdef __cplusplus
extern "C" {
#endif

	vatek_result simulation_create_hanle(vatek_ic_module icchip, hal_service_mode service, Psim_handle* psim);
	void simulation_free_handle(Psim_handle psim);

	Psim_service simservice_broadcast_get();
	Psim_service simservice_transfrom_get();

	vatek_result simtransform_start_stream(hsim_service hservice);
	void simtransform_stop_stream(hsim_service hservice);

#ifdef __cplusplus
}
#endif

#endif
