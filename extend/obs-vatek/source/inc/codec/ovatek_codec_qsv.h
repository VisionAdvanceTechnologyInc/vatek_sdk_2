#ifndef OVATEK_CODEC_QSV_
#define OVATEK_CODEC_QSV_

#include <codec/ovatek_codec.h>

/*
	obs_data_set_default_string(settings, "target_usage", "balanced");
	obs_data_set_default_int(settings, "bitrate", 2500);
	obs_data_set_default_int(settings, "max_bitrate", 3000);
	obs_data_set_default_string(settings, "profile", "high");
	obs_data_set_default_string(settings, "rate_control", "CBR");

	obs_data_set_default_int(settings, "accuracy", 1000);
	obs_data_set_default_int(settings, "convergence", 1);
	obs_data_set_default_int(settings, "qpi", 23);
	obs_data_set_default_int(settings, "qpp", 23);
	obs_data_set_default_int(settings, "qpb", 23);
	obs_data_set_default_int(settings, "icq_quality", 23);

	obs_data_set_default_int(settings, "keyint_sec", 3);
	obs_data_set_default_string(settings, "latency", "normal");
	obs_data_set_default_int(settings, "bframes", 3);
	obs_data_set_default_bool(settings, "enhancements", false);
*/

/* end vatek ui_properties define */

typedef struct _obs_qsvh264_param
{
	ovatek_h264_param base;
}obs_qsvh264_param,*Pobs_qsvh264_param;

class ovatekCodecFactoryQSV : public qtvCodecFactory
{
public:
	explicit ovatekCodecFactoryQSV();
	~ovatekCodecFactoryQSV();

	vatek_result createCodec(qtvCodec** codec) override;
	void freeCodec(qtvCodec* codec) override;

protected:
	ovatekParamFactory m_paramfactory;
};

class ovatekCodecVideoQSV : public ovatekCodecVideo
{
public:
	explicit ovatekCodecVideoQSV(ovatekCodecFactoryQSV* factory, ovatekParam* param);

	video_codec _codec_video_mode() { return encvideo_h264; }
	vatek_result createVideoCodec(obs_output_t* houtput, obs_encoder_t** encoder) override;
};

#endif
