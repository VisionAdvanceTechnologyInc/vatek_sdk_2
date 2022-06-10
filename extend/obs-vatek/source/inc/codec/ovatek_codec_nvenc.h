#ifndef OVATEK_CODEC_NVENC_H
#define OVATEK_CODEC_NVENC_H

#include <codec/ovatek_codec.h>
#include <core/ui/ui_props/ui_props_encoder.h>

/*
	obs_data_set_default_int(settings, "bitrate", 2500);
	obs_data_set_default_int(settings, "max_bitrate", 5000);
	obs_data_set_default_int(settings, "keyint_sec", 0);
	obs_data_set_default_int(settings, "cqp", 20);
	obs_data_set_default_string(settings, "rate_control", "CBR");
	obs_data_set_default_string(settings, "preset", "hq");
	obs_data_set_default_string(settings, "profile", "high");
	obs_data_set_default_bool(settings, "psycho_aq", true);
	obs_data_set_default_int(settings, "gpu", 0);
	obs_data_set_default_int(settings, "bf", 2);
	obs_data_set_default_bool(settings, "repeat_headers", false);
*/

typedef struct _obs_nvench264_param
{
	ovatek_h264_param base;
}obs_nvench264_param,*Pobs_nvench264_param;

class ovatekCodecFactoryNVENC : public qtvCodecFactory
{
public:
	explicit ovatekCodecFactoryNVENC();
	~ovatekCodecFactoryNVENC();

	vatek_result createCodec(qtvCodec** codec) override;
	void freeCodec(qtvCodec* codec) override;

protected:
	ovatekParamFactory m_paramfactory;
};

class ovatekCodecVideoNVENC : public ovatekCodecVideo
{
public:
	explicit ovatekCodecVideoNVENC(ovatekCodecFactoryNVENC* factory, ovatekParam* param);
	~ovatekCodecVideoNVENC();

	video_codec _codec_video_mode() override { return encvideo_h264; }
	vatek_result createVideoCodec(obs_output_t* houtput, obs_encoder_t** encoder) override;
};

#endif
