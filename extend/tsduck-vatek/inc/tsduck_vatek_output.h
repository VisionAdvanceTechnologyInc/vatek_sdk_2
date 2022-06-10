#ifndef TSDUCK_VATEK_OUTPUT_H
#define TSDUCK_VATEK_OUTPUT_H

#include "tsPluginRepository.h"
#include "tsOutputPlugin.h"
#include "tsObjectRepository.h"
#include "tsPluginRepository.h"
#include "tsVersionInfo.h"

#include <vatek_sdk_usbstream.h>
#include <core/base/output_modulator.h>

namespace ts
{
	typedef enum _tsvatek_bandwidth
	{
		tsvatek_bw_1_7 = 0,
		tsvatek_bw_5 = 5,
		tsvatek_bw_6 = 6,
		tsvatek_bw_7 = 7,
		tsvatek_bw_8 = 8,
		tsvatek_bw_10 = 10,
	}tsvatek_bandwidth;

	typedef vatek_result(*fpmodparam_config)(TSP* tsp,Args* args, Pmodulator_param pmod);
	typedef struct _tsmod_param_config
	{
		modulator_type type;
		fpmodparam_config config;
	}tsmod_param_config,*Ptsmod_param_config;

	class tsduck_vatek_output : public OutputPlugin
	{
		TS_NOBUILD_NOCOPY(tsduck_vatek_output);
	public:
		tsduck_vatek_output(TSP* tsp);
		~tsduck_vatek_output();

		// Implementation of plugin API
		virtual bool start() override;
		virtual bool stop() override;
		virtual bool send(const TSPacket*, const TSPacketMetadata*, size_t) override;
		virtual bool isRealTime() override;
		virtual BitRate getBitrate() override;

	protected:
		hvatek_devices m_hdevices;
		hvatek_chip m_hchip;
		hvatek_usbstream m_husbstream;
		usbstream_param m_param;
		int32_t m_index;
		Pusbstream_slice m_slicebuf;

		vatek_result configParam();

		static const tsmod_param_config m_modtables[];
	};
	TS_REGISTER_OUTPUT_PLUGIN(u"vatek", tsduck_vatek_output);

}

#endif
