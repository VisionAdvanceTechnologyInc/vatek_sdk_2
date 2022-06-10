#ifndef OVATEK_FFMPEG_SW_H
#define OVATEK_FFMPEG_SW_H

#include <codec/ffmpeg/ovatek_ffmpeg.h>
#include <core/ui/ui_props_define.h>
#include <core/ui/ui_props/ui_props_encoder.h>

/* MPEG2 VIDEO CODEC DEFINE - START */

#define OMPEG2_EN_SEQ_DISP_EXT		0x00000001
#define OMPEG2_EN_A52CC				0x00000002

typedef struct _ompeg2_param
{
	ovatek_mpeg2_param mpeg2;
	uint32_t functions;
}ompeg2_param,*Pompeg2_param;

typedef struct _omp2_param
{
	ovatek_audio_bitrate bitrate;
}omp2_param,*Pomp2_param;

typedef struct _oa52_param
{
	ovatek_audio_bitrate bitrate;
}oa52_param,*Poa52_param;

static const omp2_param default_mp2 = { oaudio_bitrate_128, };
static const oa52_param default_a52 = { oaudio_bitrate_128, };

class ovatekFactoryMPEG2VIDEO : public ovatekFFmpegFactory
{
public:
	static vatek_result create(AVCodec* codec, const struct _offmpeg_codec* ffcodec, ovatekFFmpegFactory** factory);

	explicit ovatekFactoryMPEG2VIDEO(AVCodec* codec, Poffmpeg_codec ffcodec);

	/* ovatekFFmpegFactory */
	video_format getVideoFormat(video_format fmt) override;
	vatek_result configFFmpegContext(obs_data_t* setting, AVCodecContext* context) override;

private:

	static const char* obsGetName(void* typedata);
	static void* obsCreate(obs_data_t* settings, obs_encoder_t* encoder);
	static void obsDefaults(obs_data_t* settings);
	static obs_properties_t* obsProperties(void* unused);

	static obs_encoder_info* CreatePlugin();
	static ovatekParamFactory* createParamFactory();
};

/* MPEG2 VIDEO CODEC DEFINE - END */

class ovatekFactoryFFmpegAudio : public ovatekFFmpegFactory
{
public:
	static vatek_result create(AVCodec* codec, const struct _offmpeg_codec* ffcodec, ovatekFFmpegFactory** factort);

	explicit ovatekFactoryFFmpegAudio(const char* name,AVCodec* codec, Poffmpeg_codec ffcodec);

	/* ovatekFFmpegFactory */
	video_format getVideoFormat(video_format fmt) override { return VIDEO_FORMAT_NONE; }
	vatek_result configFFmpegContext(obs_data_t* setting, AVCodecContext* context) override;

protected:

	static obs_encoder_info* CreatePlugin(const Poffmpeg_codec ffcodec);
	static ovatekParamFactory* createParamFactory(const Poffmpeg_codec ffcodec);

private:

	static const char* obsGetNameMP2(void* typedata);
	static void* obsCreateMP2(obs_data_t* settings, obs_encoder_t* encoder);
	static void obsDefaultsMP2(obs_data_t* settings);
	static obs_properties_t* obsPropertiesMP2(void* unused);

	static const char* obsGetNameA52(void* typedata);
	static void* obsCreateA52(obs_data_t* settings, obs_encoder_t* encoder);
	static void obsDefaultsA52(obs_data_t* settings);
	static obs_properties_t* obsPropertiesA52(void* unused);
};

#endif