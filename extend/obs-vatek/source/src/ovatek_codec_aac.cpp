#include <codec/ovatek_codec_aac.h>
#include <ui/ovatek_ui_codec.h>
/* ovatekCodecFactoryAAC */

static const ovatek_aac_param default_aac_param =
{
	ovatek_aac_adts,oaudio_bitrate_128,
};

ovatekCodecFactoryAAC::ovatekCodecFactoryAAC() :
	qtvCodecFactory(CODEC_NAME_AAC, qcodec_audio),
	m_paramfactory((uint8_t*)&default_aac_param,sizeof(ovatek_aac_param),0,_ui_struct(ovatek_aac_param),NULL)
{

}

ovatekCodecFactoryAAC::~ovatekCodecFactoryAAC()
{

}

vatek_result ovatekCodecFactoryAAC::createCodec(qtvCodec** codec)
{
	vatek_result nres = vatek_success;
	*codec = new ovatekCodecAudioAAC(this,m_paramfactory.createParam());
	return nres;
}

void ovatekCodecFactoryAAC::freeCodec(qtvCodec* codec)
{
	ovatekCodecAudioAAC* paac = static_cast<ovatekCodecAudioAAC*>(codec);
	delete paac;
}

/* ovatekCodecVideoQSV */

ovatekCodecAudioAAC::ovatekCodecAudioAAC(ovatekCodecFactoryAAC* factory,ovatekParam* param) :
	ovatekCodecAudio(factory, CODEC_NAME_AAC,param)
{

}

ovatekCodecAudioAAC::~ovatekCodecAudioAAC()
{

}

audio_codec ovatekCodecAudioAAC::_codec_audio_mode()
{
	if (m_param.mode == ovatek_aac_latm)
		return encaudio_aac_lc_latm;
	return encaudio_aac_lc_adts;
}

vatek_result ovatekCodecAudioAAC::createAudioCodec(obs_output_t* houtput, obs_encoder_t** encoder)
{
	Povatek_aac_param paac = (Povatek_aac_param)m_oparam->_raw_param();
	obs_data_t* settings = obs_data_create();
	vatek_result nres = vatek_memfail;
	if (settings)
	{
		int32_t bitrate = getAudioBitrate(paac->bitrate);
		obs_data_set_int(settings, "bitrate", bitrate);
		obs_encoder_t* newenc = obs_audio_encoder_create(CODEC_NAME_AAC, "ovatek-aac", settings, 0, NULL);
		nres = vatek_unsupport;
		if (newenc)
		{
			obs_encoder_set_audio(newenc, obs_output_audio(houtput));
			*encoder = newenc;
			nres = vatek_success;
		}
		obs_data_release(settings);
	}
	return nres;
}