#include <obs_vatek.h>

typedef struct _ovatek_output_handle
{
	obs_output_t* output;
}ovatek_output_handle,*Povatek_output_handle;

const char* ovatek_output_get_name(void* type)
{
	return "obs-vatek-dtv-broadcast";
}

void* ovatek_output_create(obs_data_t* settings, obs_output_t* output)
{
	Povatek_output_handle newhandle = (Povatek_output_handle)bzalloc(sizeof(ovatek_output_handle));
	newhandle->output = output;
	return newhandle;
}

void ovatek_output_destroy(void* data)
{
	Povatek_output_handle phandle = (Povatek_output_handle)data;
	bfree(phandle);
}

bool ovatek_output_start(void* data)
{
	return true;
}

void ovatek_output_stop(void* data, uint64_t ts)
{

}

void ovatek_output_encoded_packet(void* data, struct encoder_packet* packet)
{
	obsVatekOutput* source = obsVatekHandle::getOutputHandle();
	if (source)
		source->sendEncoderPacket(packet);
}

obs_output_info* obsVatekHandle::getPluginOutput()
{
	static obs_output_info ovatek_output;
	ovatek_output.id = OVATEK_PLUGIN_OUTPUT;
	ovatek_output.flags = OBS_OUTPUT_AV | OBS_OUTPUT_ENCODED;
	ovatek_output.encoded_video_codecs = "h264";
	ovatek_output.encoded_audio_codecs = "aac";
	ovatek_output.get_name = ovatek_output_get_name;
	ovatek_output.create = ovatek_output_create;
	ovatek_output.destroy = ovatek_output_destroy;
	ovatek_output.start = ovatek_output_start;
	ovatek_output.stop = ovatek_output_stop;
	ovatek_output.encoded_packet = ovatek_output_encoded_packet;
	return &ovatek_output;
}