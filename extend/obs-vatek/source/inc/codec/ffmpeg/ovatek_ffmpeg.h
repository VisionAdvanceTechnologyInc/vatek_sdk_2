#ifndef OVATEK_FFMPEG_H
#define OVATEK_FFMPEG_H

#include <codec/ovatek_codec.h>
#include <util/darray.h>
#include <media-io/audio-io.h>

#ifdef __cplusplus
extern "C" {
#endif

	#include <libavcodec/avcodec.h>
	#include <libavutil/opt.h>
	#include <libavutil/pixdesc.h>

#ifdef __cplusplus
}
#endif

#define OVATEK_FFMPEG_QSV_MPEG2		"mpeg2_qsv"
#define OVATEK_FFMPEG_QSV_H264		"h264_qsv"
#define OVATEK_FFMPEG_QSV_HEVC		"hevc_qsv"

#define OVATEK_FFMPEG_NVENC_H264	"h264_nvenc"
#define OVATEK_FFMPEG_NVENC_HEVC	"h264_hevc"

#define OVATEK_FFMPEG_MPEG2VIDEO	"mpeg2video"

#define OVATEK_FFMPEG_MP2			"mp2fixed"
#define OVATEK_FFMPEG_A52			"ac3_fixed"
#define OVATEK_FFMPEG_AAC			"aac"

class ovatekFFmpegFactory;
struct _offmpeg_codec;
typedef vatek_result(*fpcreate)(AVCodec* codec,const struct _offmpeg_codec* ffcodec, ovatekFFmpegFactory** factory);

typedef struct _offmpeg_codec
{
	const char* name;
	AVCodecID codeid;
	obs_encoder_type type;
	AVHWDeviceType hw;
	fpcreate create;
}offmpeg_codec, * Poffmpeg_codec;

typedef enum _offmpeg_frame_mode
{
	ff_frame_bypass,
	ff_frame_hwframe,
	ff_frame_audio,
}offmpeg_frame_mode;

typedef struct _offmpeg_video_handle
{
	AVBufferRef* hwdeviceref;
	AVBufferRef* hwframeref;
	uint8_t* seibuf;
	size_t seilen;
	video_format pixelfmt;
}offmpeg_video_handle,* Poffmpeg_video_handle;

typedef struct _offmpeg_audio_handle
{
	enum audio_format aformat;
	size_t aplanes;
	size_t asize;
	int32_t aframesize;
	int32_t aframesizebytes;
	uint8_t* asamples[MAX_AV_PLANES];
	int64_t atotalsamples;
}offmpeg_audio_handle,*Poffmpeg_audio_handle;

typedef struct _offmpeg_handle
{
	obs_encoder_type type;
	union
	{
		offmpeg_video_handle video;
		offmpeg_audio_handle audio;
	}_handle;
}offmpeg_handle,*Poffmpeg_handle;

class ovatekFFmpegHandle
{
public:
	explicit ovatekFFmpegHandle(ovatekFFmpegFactory* factory);
	~ovatekFFmpegHandle();

	vatek_result openCodec(obs_data_t* settings, obs_encoder_t* encoder);
	void closeCodec();

	/* obs ffmpeg tools */

	static obs_encoder_info* mallocOBSPlugin(obs_encoder_type type);
	static void getOBSDefault(obs_data_t* settings, const Pui_prop_item uiprops,const Pui_prop_item advui, uint8_t* param,int32_t advoffect);
	static obs_properties_t* createOBSProperties(const Pui_prop_item uiprops, const Pui_prop_item uiadv);
	static enum AVPixelFormat getOBSPixelFormatToFFmpeg(enum video_format pixfmt);
	static uint64_t getOBSChannelLayoutToFFmpeg(enum speaker_layout layout);
	static enum audio_format getOBSAudioFormatFromFFmpeg(enum AVSampleFormat format);
	static enum speaker_layout getOBSSpeakerLayoutFromFFmpeg(uint64_t nchannel);

	static void setVideoContextCodec(AVCodecContext* context,ovatek_rcmode rcmode, uint32_t bitrate, uint32_t maxbitrate,uint8_t bframe,int32_t keysec);
	static void setAudioContextCodec(AVCodecContext* context, uint32_t bitrate);

	/* obs_plugin_gate */
	static void* obsCreate(const char* name, obs_data_t* settings, obs_encoder_t* encoder);
	static void obsDestroy(void* data);
	static bool obsExtraData(void* data, uint8_t** extra_data, size_t* size);

	static bool obsEncodeVideo(void* data, struct encoder_frame* frame, struct encoder_packet* packet, bool* received_packet);
	static void obsGetVideoInfo(void* data, struct video_scale_info* info);
	static bool obsSeiData(void* data, uint8_t** extra_data, size_t* size);

	static bool obsEncodeAudio(void* data, struct encoder_frame* frame, struct encoder_packet* packet, bool* received_packet);
	static void obsGetAudioInfo(void* data, struct audio_convert_info* info);
	static size_t obsGetFrameSize(void* data);

protected:
	AVCodec* m_codec;
	const Poffmpeg_codec m_ffcodec;
	AVCodecContext* m_context;
	Poffmpeg_handle m_ffhandle;

	AVFrame* m_frame;
	ovatekFFmpegFactory* m_factory;
	offmpeg_frame_mode m_framemode;
	
	uint32_t m_framecount;
	int32_t m_isfirstpkt;
	obs_encoder_t* m_obsencoder;
	DARRAY(uint8_t) m_dynbuffer;
	uint8_t* m_hdrbuf;
	size_t m_hdrlen;


	vatek_result openVideoCodec(obs_data_t* settings, obs_encoder_t* encoder);
	vatek_result openVideoFrame();
	vatek_result openVideoFFmpeg();

	void closeVideoFFmpeg();
	void closeVideoFrame();
	void closeVideoCodec();

	vatek_result openAudioCodec(obs_data_t* settings, obs_encoder_t* encoder);
	void closeAudioCodec();

	static offmpeg_frame_mode setAudioContext(AVCodecContext* context,AVCodec* codec, Poffmpeg_audio_handle pffaudio, obs_encoder_t* enc);
	static offmpeg_frame_mode setVideoContext(AVCodecContext* context, obs_encoder_t* enc,ovatekFFmpegFactory* factory, video_format* pixelfmt);
	static Poffmpeg_handle malloc_ffhandle(obs_encoder_type type);
	static void free_ffhandle(Poffmpeg_handle pffhandle);
};

class ovatekFFmpegAudio : public ovatekCodecAudio
{
public:
	explicit ovatekFFmpegAudio(ovatekFFmpegFactory* factory, AVCodec* codec);
	~ovatekFFmpegAudio();

	/* qtvCodecVideo */
	audio_codec _codec_audio_mode() override;
	/* ovatekCodecAudio */
	vatek_result createAudioCodec(obs_output_t* houtput, obs_encoder_t** encoder) override;
	/* ovatekFFmpegAudio */
	virtual bool checkIsAACLatm() { return false; }

protected:
	char m_szname[32];
	AVCodec* m_avcodec;
	ovatekFFmpegFactory* m_ffactory;
};

class ovatekFFmpegVideo : public ovatekCodecVideo
{
public:
	explicit ovatekFFmpegVideo(ovatekFFmpegFactory* factory,AVCodec* codec);
	~ovatekFFmpegVideo();

	/* qtvCodecVideo */
	video_codec _codec_video_mode() override;
	/* ovatekCodecVideo */
	vatek_result createVideoCodec(obs_output_t* houtput, obs_encoder_t** encoder) override;
protected:
	ovatekFFmpegFactory* m_ffactory;
	AVCodec* m_avcodec;
	char m_szname[32];
};

class ovatekFFmpegFactory : public qtvCodecFactory
{
public:
	static vatek_result registerCodecs(qtvCodecSimpleProvider* vprovider, qtvCodecSimpleProvider* aprovider);

	explicit ovatekFFmpegFactory(const Poffmpeg_codec ffcodec,
								 const char* name, 
							     qtv_codec_mode mode,
								 AVCodec* codec, 
								 obs_encoder_info* obsinfo, 
								 ovatekParamFactory* factory);

	obs_encoder_info* _obs_plugin() { return m_obsplugin;}
	ovatekParamFactory* _param_factory() { return m_paramfactory; }
	const Poffmpeg_codec _ff_codec() { return m_ffcodec; }
	AVCodec* _codec() { return m_codec; }

	vatek_result createCodec(qtvCodec** codec) override;
	void freeCodec(qtvCodec* codec) override;


	virtual video_format getVideoFormat(video_format fmt) = 0;
	virtual vatek_result configFFmpegContext(obs_data_t* setting, AVCodecContext* context) = 0;

protected:
	AVCodec* m_codec;
	const Poffmpeg_codec m_ffcodec;
	obs_encoder_info* m_obsplugin;
	ovatekParamFactory* m_paramfactory;
};

#endif
