#ifndef OVATEK_CODEC_AAC_H
#define OVATEK_CODEC_AAC_H

/*
obs_data_set_default_int(settings, "bitrate", 128);
*/

#include <codec/ovatek_codec.h>

class ovatekCodecFactoryAAC : public qtvCodecFactory
{
public:
	explicit ovatekCodecFactoryAAC();
	~ovatekCodecFactoryAAC();

	vatek_result createCodec(qtvCodec** codec) override;
	void freeCodec(qtvCodec* codec) override;

protected:
	ovatekParamFactory m_paramfactory;
};

class ovatekCodecAudioAAC : public ovatekCodecAudio
{
public:
	explicit ovatekCodecAudioAAC(ovatekCodecFactoryAAC* factory,ovatekParam* param);
	~ovatekCodecAudioAAC();

	audio_codec _codec_audio_mode() override;
	vatek_result createAudioCodec(obs_output_t* houtput, obs_encoder_t** encoder) override;

protected:
	ovatek_aac_param m_param;
};

#endif
