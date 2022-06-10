#include <codec/ovatek_codec_nvenc.h>
#include <ui/ovatek_ui_codec.h>

static const obs_nvench264_param default_nvench264 =
{
	{h264_profile_high,h264_level_4_0,ovatek_rc_cbr,2,1,0,15000,19000,},
};

/* ovatekCodecFactoryNVENC */

ovatekCodecFactoryNVENC::ovatekCodecFactoryNVENC() :
	qtvCodecFactory(CODEC_NAME_FFMPEGNVENC, qcodec_video),
	m_paramfactory((uint8_t*)&default_nvench264,sizeof(obs_nvench264_param),0,_ui_struct(ovatek_h264_param),NULL)
{

}

ovatekCodecFactoryNVENC::~ovatekCodecFactoryNVENC()
{

}

vatek_result ovatekCodecFactoryNVENC::createCodec(qtvCodec** codec)
{
	vatek_result nres = vatek_success;
	*codec = new ovatekCodecVideoNVENC(this,m_paramfactory.createParam());
	return nres;
}

void ovatekCodecFactoryNVENC::freeCodec(qtvCodec* codec)
{
	ovatekCodecVideoNVENC* pnvenc = static_cast<ovatekCodecVideoNVENC*>(codec);
	delete pnvenc;
}

/* ovatekCodecVideoNVENC */

ovatekCodecVideoNVENC::ovatekCodecVideoNVENC(ovatekCodecFactoryNVENC* factory, ovatekParam* param) :
	ovatekCodecVideo(factory, CODEC_NAME_FFMPEGNVENC, param)
{

}

ovatekCodecVideoNVENC::~ovatekCodecVideoNVENC()
{

}

vatek_result ovatekCodecVideoNVENC::createVideoCodec(obs_output_t* houtput, obs_encoder_t** encoder)
{
	Povatek_h264_param ph264 = (Povatek_h264_param)m_oparam->_raw_param();
	obs_data_t* settings = obs_data_create();
	vatek_result nres = vatek_memfail;
	if (settings)
	{
		obs_data_set_int(settings, "bitrate", ph264->bitrate);
		obs_data_set_int(settings, "max_bitrate", ph264->maxbitrate);
		obs_data_set_int(settings, "keyint_sec", ph264->keysec);
		obs_data_set_int(settings, "bf", ph264->bframe);

		obs_encoder_t* newenc = obs_video_encoder_create(CODEC_NAME_FFMPEGNVENC, "ovatek-nvenc", settings, NULL);
		nres = vatek_unsupport;
		if (newenc)
		{
			obs_encoder_set_video(newenc, obs_output_video(houtput));
			*encoder = newenc;
			nres = vatek_success;
		}
		obs_data_release(settings);
	}
	return nres;
	//return ovatekCodec::createVideoCodec(CODEC_NAME_FFMPEGNVENC, "ovatek-nvenc", houtput, encoder, m_oparam);
}