#include <codec/ffmpeg/ovatek_ffmpeg_qsv.h>
#include <codec/ffmpeg/ovatek_ffmpeg_nvenc.h>
#include <codec/ffmpeg/ovatek_ffmpeg_sw.h>

#include <ui/ovatek_ui_ffmpeg.h>


static const ompeg2_param default_ompeg2 =
{
	{mpeg2_profile_main_lmain,ovatek_rc_cbr,2,{0},15000,19000,},0,
};

/* ovatekFFmpegVideo */

ovatekFFmpegVideo::ovatekFFmpegVideo(ovatekFFmpegFactory* factory, AVCodec* codec) :
	m_ffactory(factory),
	m_avcodec(codec),
	ovatekCodecVideo(factory, codec->name,factory->_param_factory()->createParam())
{
	sprintf(&m_szname[0], "offmpeg-%s", codec->name);
}

ovatekFFmpegVideo::~ovatekFFmpegVideo()
{

}

video_codec ovatekFFmpegVideo::_codec_video_mode()
{
	if (m_avcodec->id == AV_CODEC_ID_MPEG2VIDEO)
		return encvideo_mpeg2;
	else if (m_avcodec->id == AV_CODEC_ID_H264)
		return encvideo_h264;
	else if (m_avcodec->id == AV_CODEC_ID_H265)
		return encvideo_hevc;
	return encvideo_unknown;
}

vatek_result ovatekFFmpegVideo::createVideoCodec(obs_output_t* houtput, obs_encoder_t** encoder)
{
	return ovatekCodec::createVideoCodec(m_avcodec->name, &m_szname[0], houtput, encoder, m_oparam);
}

/* ovatekFactoryMPEG2VIDEO */

vatek_result ovatekFactoryMPEG2VIDEO::create(AVCodec* codec, const struct _offmpeg_codec* ffcodec, ovatekFFmpegFactory** factory)
{
	vatek_result nres = vatek_badparam;
	if (strcmp(codec->name, OVATEK_FFMPEG_MPEG2VIDEO) == 0)
	{
		*factory = new ovatekFactoryMPEG2VIDEO(codec,(const Poffmpeg_codec) ffcodec);
		nres = vatek_success;
	}
	return nres;
}

ovatekFactoryMPEG2VIDEO::ovatekFactoryMPEG2VIDEO(AVCodec* codec,Poffmpeg_codec ffcodec) :
	ovatekFFmpegFactory(ffcodec,OVATEK_FFMPEG_MPEG2VIDEO, qcodec_video, codec, CreatePlugin(), createParamFactory())
{

}

video_format ovatekFactoryMPEG2VIDEO::getVideoFormat(video_format fmt)
{
	if (fmt == VIDEO_FORMAT_I420 || fmt == VIDEO_FORMAT_I422)
		return fmt;
	else return VIDEO_FORMAT_I420;
}

vatek_result ovatekFactoryMPEG2VIDEO::configFFmpegContext(obs_data_t* setting, AVCodecContext* context)
{
	vatek_result nres = vatek_badparam;
	ovatekParam* param = m_paramfactory->createParam();
	if (param)
	{
		Pompeg2_param pmpeg2 = (Pompeg2_param)param->_raw_param();
		Povatek_mpeg2_param pbm2 = &pmpeg2->mpeg2;
		param->setParamObsData(setting);
		ovatekFFmpegHandle::setVideoContextCodec(context, pbm2->rcmode, pbm2->bitrate, pbm2->maxbitrate, pbm2->bframe,0);

		if (pbm2->profile == mpeg2_profile_auto || pbm2->profile == mpeg2_profile_high)
		{
			av_opt_set(context->priv_data, "profile", "high", 0);
			av_opt_set(context->priv_data, "level", "high", 0);
		}
		else
		{
			av_opt_set(context->priv_data, "profile", "main", 0);
			if(pbm2->profile == mpeg2_profile_main_lmain)
				av_opt_set(context->priv_data, "level", "main", 0);
			else av_opt_set(context->priv_data, "level", "high", 0);
		}
		av_opt_set_int(context->priv_data, "a53cc", ((pmpeg2->functions & OMPEG2_EN_A52CC) != 0),0);
		delete param;
		nres = vatek_success;
	}
	return nres;
}

void* ovatekFactoryMPEG2VIDEO::obsCreate(obs_data_t* settings, obs_encoder_t* encoder)
{
	return ovatekFFmpegHandle::obsCreate(OVATEK_FFMPEG_MPEG2VIDEO, settings, encoder);
}

const char* ovatekFactoryMPEG2VIDEO::obsGetName(void* typedata)
{
	UNUSED_PARAMETER(typedata);
	return OVATEK_FFMPEG_MPEG2VIDEO;
}

void ovatekFactoryMPEG2VIDEO::obsDefaults(obs_data_t* settings)
{
	ovatekFFmpegHandle::getOBSDefault(settings, _ui_struct(ovatek_mpeg2_param),_ui_struct(ompeg2_param),(uint8_t*)&default_ompeg2,0);
}

obs_properties_t* ovatekFactoryMPEG2VIDEO::obsProperties(void* unused)
{
	UNUSED_PARAMETER(unused);
	return ovatekFFmpegHandle::createOBSProperties(_ui_struct(ovatek_mpeg2_param), _ui_struct(ompeg2_param));
}

obs_encoder_info* ovatekFactoryMPEG2VIDEO::CreatePlugin()
{
	obs_encoder_info* newplugin = ovatekFFmpegHandle::mallocOBSPlugin(OBS_ENCODER_VIDEO);
	newplugin->id = OVATEK_FFMPEG_MPEG2VIDEO;
	newplugin->codec = "mpeg2";
	newplugin->create = obsCreate;
	newplugin->get_name = obsGetName;
	newplugin->get_defaults = obsDefaults;
	newplugin->get_properties = obsProperties;
	return newplugin;
}

ovatekParamFactory* ovatekFactoryMPEG2VIDEO::createParamFactory()
{
	return new ovatekParamFactory((uint8_t*)&default_ompeg2, 
									sizeof(ompeg2_param), 
									0, 
									_ui_struct(ovatek_mpeg2_param),
									_ui_struct(ompeg2_param));
}
