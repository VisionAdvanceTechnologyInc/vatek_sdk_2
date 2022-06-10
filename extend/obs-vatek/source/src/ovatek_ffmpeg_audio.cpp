#include <codec/ffmpeg/ovatek_ffmpeg_sw.h>
#include <ui/ovatek_ui_ffmpeg.h>

/* ovatekFFmpegAudio */

ovatekFFmpegAudio::ovatekFFmpegAudio(ovatekFFmpegFactory* factory, AVCodec* codec) :
	m_ffactory(factory),
	m_avcodec(codec),
	ovatekCodecAudio(factory, codec->name, factory->_param_factory()->createParam())
{
	sprintf(&m_szname[0], "offmpeg-%s", codec->name);
}

ovatekFFmpegAudio::~ovatekFFmpegAudio()
{

}

audio_codec ovatekFFmpegAudio::_codec_audio_mode()
{
	if (m_avcodec->id == AV_CODEC_ID_MP2)
		return encaudio_mp2;
	else if (m_avcodec->id == AV_CODEC_ID_AC3)
		return encaudio_ac3;
	else if (m_avcodec->id == AV_CODEC_ID_AAC)
	{
		if (checkIsAACLatm())return encaudio_aac_lc_latm;
		return encaudio_aac_lc_adts;
	}
	return encaudio_unknown;
}

vatek_result ovatekFFmpegAudio::createAudioCodec(obs_output_t* houtput, obs_encoder_t** encoder)
{
	return ovatekCodec::createAudioCodec(m_avcodec->name, &m_szname[0], houtput, encoder, m_oparam);
}

/* ovatekFactoryFFmpegAudio */

vatek_result ovatekFactoryFFmpegAudio::create(AVCodec* codec, const struct _offmpeg_codec* ffcodec, ovatekFFmpegFactory** factory)
{
	vatek_result nres = vatek_success;
	if (strcmp(codec->name, OVATEK_FFMPEG_MP2) == 0)
		*factory = new ovatekFactoryFFmpegAudio(OVATEK_FFMPEG_MP2,codec, (const Poffmpeg_codec)ffcodec);
	else if (strcmp(codec->name, OVATEK_FFMPEG_A52) == 0)
		*factory = new ovatekFactoryFFmpegAudio(OVATEK_FFMPEG_A52,codec, (const Poffmpeg_codec)ffcodec);
	else nres = vatek_badparam;
	return nres;
}

ovatekFactoryFFmpegAudio::ovatekFactoryFFmpegAudio(const char* name,AVCodec* codec, Poffmpeg_codec ffcodec) :
	ovatekFFmpegFactory(ffcodec, name, qcodec_audio, codec, CreatePlugin(ffcodec), createParamFactory(ffcodec))
{

}

vatek_result ovatekFactoryFFmpegAudio::configFFmpegContext(obs_data_t* setting, AVCodecContext* context)
{
	vatek_result nres = vatek_memfail;
	ovatekParam* param = m_paramfactory->createParam();
	if (param)
	{
		nres = vatek_success;
		if (strcmp(m_ffcodec->name, OVATEK_FFMPEG_MP2) == 0)
		{
			Pomp2_param pmp2 = (Pomp2_param)param->_raw_param();
			param->setParamObsData(setting);
			ovatekFFmpegHandle::setAudioContextCodec(context, ovatekCodecAudio::getAudioBitrate(pmp2->bitrate));
		}
		else if (strcmp(m_ffcodec->name, OVATEK_FFMPEG_A52) == 0)
		{
			Poa52_param pa52 = (Poa52_param)param->_raw_param();
			param->setParamObsData(setting);
			ovatekFFmpegHandle::setAudioContextCodec(context, ovatekCodecAudio::getAudioBitrate(pa52->bitrate));
		}
		else nres = vatek_badparam;
		delete param;
	}
	return nres;
}

obs_encoder_info* ovatekFactoryFFmpegAudio::CreatePlugin(const Poffmpeg_codec ffcodec)
{
	obs_encoder_info* newplugin = ovatekFFmpegHandle::mallocOBSPlugin(OBS_ENCODER_AUDIO);

	if (strcmp(ffcodec->name, OVATEK_FFMPEG_MP2) == 0)
	{
		newplugin->id = OVATEK_FFMPEG_MP2;
		newplugin->codec = "mp2";
		newplugin->create = obsCreateMP2;
		newplugin->get_name = obsGetNameMP2;
		newplugin->get_defaults = obsDefaultsMP2;
		newplugin->get_properties = obsPropertiesMP2;
	}
	else if (strcmp(ffcodec->name, OVATEK_FFMPEG_A52) == 0)
	{
		newplugin->id = OVATEK_FFMPEG_A52;
		newplugin->codec = "ac-3";
		newplugin->create = obsCreateA52;
		newplugin->get_name = obsGetNameA52;
		newplugin->get_defaults = obsDefaultsA52;
		newplugin->get_properties = obsPropertiesA52;
	}
	else
	{
		free(newplugin);
		newplugin = NULL;
	}

	return newplugin;
}

ovatekParamFactory* ovatekFactoryFFmpegAudio::createParamFactory(const Poffmpeg_codec ffcodec)
{
	if (strcmp(ffcodec->name, OVATEK_FFMPEG_MP2) == 0)
		return new ovatekParamFactory((uint8_t*)&default_mp2, sizeof(omp2_param), 0, _ui_struct(omp2_param),NULL);
	else if (strcmp(ffcodec->name, OVATEK_FFMPEG_A52) == 0)
		return new ovatekParamFactory((uint8_t*)&default_a52, sizeof(oa52_param), 0, _ui_struct(oa52_param),NULL);
	return NULL;
}

const char* ovatekFactoryFFmpegAudio::obsGetNameMP2(void* typedata)
{
	UNUSED_PARAMETER(typedata);
	return OVATEK_FFMPEG_MP2;
}

void* ovatekFactoryFFmpegAudio::obsCreateMP2(obs_data_t* settings, obs_encoder_t* encoder)
{
	return ovatekFFmpegHandle::obsCreate(OVATEK_FFMPEG_MP2, settings, encoder);
}

void ovatekFactoryFFmpegAudio::obsDefaultsMP2(obs_data_t* settings)
{
	ovatekFFmpegHandle::getOBSDefault(settings, _ui_struct(omp2_param),NULL, (uint8_t*)&default_mp2, 0);
}

obs_properties_t* ovatekFactoryFFmpegAudio::obsPropertiesMP2(void* unused)
{
	UNUSED_PARAMETER(unused);
	return ovatekFFmpegHandle::createOBSProperties(_ui_struct(omp2_param), NULL);
}

const char* ovatekFactoryFFmpegAudio::obsGetNameA52(void* typedata)
{
	UNUSED_PARAMETER(typedata);
	return OVATEK_FFMPEG_A52;
}

void* ovatekFactoryFFmpegAudio::obsCreateA52(obs_data_t* settings, obs_encoder_t* encoder)
{
	return ovatekFFmpegHandle::obsCreate(OVATEK_FFMPEG_A52, settings, encoder);
}

void ovatekFactoryFFmpegAudio::obsDefaultsA52(obs_data_t* settings)
{
	ovatekFFmpegHandle::getOBSDefault(settings, _ui_struct(oa52_param), NULL, (uint8_t*)&default_a52, 0);
}

obs_properties_t* ovatekFactoryFFmpegAudio::obsPropertiesA52(void* unused)
{
	UNUSED_PARAMETER(unused);
	return ovatekFFmpegHandle::createOBSProperties(_ui_struct(oa52_param), NULL);
}
