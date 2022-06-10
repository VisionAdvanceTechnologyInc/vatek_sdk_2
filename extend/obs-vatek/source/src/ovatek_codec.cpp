#include <codec/ovatek_codec_nvenc.h>
#include <codec/ovatek_codec_qsv.h>
#include <codec/ovatek_codec_aac.h>

extern vatek_result obscodec_check_support(const char* name);
extern vatek_result obscodec_create_factory(const char* name, qtvCodecFactory** factory);

/* ovatekCodecVideo */

ovatekCodecVideo::ovatekCodecVideo(qtvCodecFactory* factory, QString name, ovatekParam* param) :
	m_oparam(param),
	qtvCodecVideo(factory,name,param->_properties())
{

}

ovatekCodecVideo::~ovatekCodecVideo()
{
	delete m_oparam;
}

void ovatekCodecVideo::resetParam()
{
	m_oparam->reset();
}

/* ovatekCodecAudio */

ovatekCodecAudio::ovatekCodecAudio(qtvCodecFactory* factory, QString name, ovatekParam* param) :
	m_oparam(param),
	qtvCodecAudio(factory, name, param->_properties())
{

}

ovatekCodecAudio::~ovatekCodecAudio()
{
	delete m_oparam;
}

void ovatekCodecAudio::resetParam()
{
	m_oparam->reset();
}

/* ovatekCodec */

vatek_result ovatekCodec::createVideoProvider(qtvCodecSimpleProvider** provider)
{
	const char* obsvideo_codec[] = { CODEC_NAME_INTELQSV ,CODEC_NAME_FFMPEGNVENC ,NULL };
	qtvCodecSimpleProvider* newprovider = new qtvCodecSimpleProvider(qcodec_video);
	vatek_result nres = vatek_success;
	int32_t pos = 0;

	while (obsvideo_codec[pos])
	{
		qtvCodecFactory* newfactory = NULL;
		nres = obscodec_create_factory(obsvideo_codec[pos],&newfactory);
		if (is_vatek_success(nres))
		{
			blog(LOG_INFO, "support video codec : %s", obsvideo_codec[pos]);
			newprovider->addCodec(newfactory);
		}
		else blog(LOG_WARNING, "unsupport video codec : %s", obsvideo_codec[pos]);
		pos++;
	}
	*provider = newprovider;
	return vatek_success;
}

vatek_result ovatekCodec::createAudioProvider(qtvCodecSimpleProvider** provider)
{
	const char* obsaudio_codec[] = { CODEC_NAME_AAC ,NULL };
	qtvCodecSimpleProvider* newprovider = new qtvCodecSimpleProvider(qcodec_audio);
	vatek_result nres = vatek_success;
	int32_t pos = 0;

	while (obsaudio_codec[pos])
	{
		qtvCodecFactory* newfactory = NULL;
		nres = obscodec_create_factory(obsaudio_codec[pos], &newfactory);
		if (is_vatek_success(nres))
		{
			newprovider->addCodec(newfactory);
			blog(LOG_INFO, "support audio codec : %s", obsaudio_codec[pos]);
		}
		else blog(LOG_INFO, "support audio codec : %s", obsaudio_codec[pos]);
		pos++;
	}

	*provider = newprovider;
	return vatek_success;
}

vatek_result ovatekCodec::createVideoCodec(const char* codecname, const char* name, obs_output_t* houtput, obs_encoder_t** encoder, ovatekParam* param)
{
	vatek_result nres = vatek_memfail;
	obs_data_t* data = param->createObsData();
	if (data)
	{
		obs_encoder_t* newenc = obs_video_encoder_create(codecname, name, data, NULL);
		nres = vatek_unsupport;
		if (newenc)
		{
			obs_encoder_set_video(newenc, obs_output_video(houtput));
			*encoder = newenc;
			nres = vatek_success;
		}
		obs_data_release(data);
	}
	return nres;
}

vatek_result ovatekCodec::createAudioCodec(const char* codecname, const char* name, obs_output_t* houtput, obs_encoder_t** encoder, ovatekParam* param)
{
	vatek_result nres = vatek_memfail;
	obs_data_t* data = param->createObsData();
	if (data)
	{
		obs_encoder_t* newenc = obs_audio_encoder_create(codecname, name, data,0, NULL);
		nres = vatek_unsupport;
		if (newenc)
		{
			obs_encoder_set_audio(newenc, obs_output_audio(houtput));
			*encoder = newenc;
			nres = vatek_success;
		}
		obs_data_release(data);
	}
	return nres;
}

vatek_result obscodec_check_support(const char* name)
{
	int32_t pos = 0;
	const char* obscodec = NULL;
	vatek_result nres = vatek_unsupport;
	while (obs_enum_encoder_types(pos, &obscodec))
	{
		if (strcmp(name, obscodec) == 0)
		{
			nres = vatek_success;
			break;
		}
		pos++;
	}
	return nres;
}

vatek_result obscodec_create_factory(const char* name, qtvCodecFactory** factory)
{
	vatek_result nres = obscodec_check_support(name);
	if (is_vatek_success(nres))
	{
		if (strcmp(name, CODEC_NAME_INTELQSV) == 0)*factory = new ovatekCodecFactoryQSV();
		else if (strcmp(name, CODEC_NAME_FFMPEGNVENC) == 0)*factory = new ovatekCodecFactoryNVENC();
		else if (strcmp(name, CODEC_NAME_AAC) == 0)*factory = new ovatekCodecFactoryAAC();
		else nres = vatek_unsupport;
	}
	return nres;
}