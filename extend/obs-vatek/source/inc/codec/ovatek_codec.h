#ifndef OVATEK_CODEC_H
#define OVATEK_CODEC_H

#include <obs-module.h>

#include <core/qtv_media.h>
#include <core/tools/tool_helpful.h>
#include <cross/cross_os_api.h>

#include <vatek_sdk_usbmux.h>

#define MEDIA_DEFAULT_CLOCK			90000

#define CODEC_NAME_INTELQSV			"obs_qsv11"
#define CODEC_NAME_FFMPEGNVENC		"ffmpeg_nvenc"
#define CODEC_NAME_X264				"obs_x264"
#define CODEC_NAME_AAC				"ffmpeg_aac"


#define OSTREAM_VIDEO_BUFFER_LEN	(50*1024*1024)
#define OSTREAM_AUDIO_BUFFER_LEN	(2*1024*1024)

typedef enum _ovatek_rcmode
{
	ovatek_rc_cbr,
	ovatek_rc_vbr,
}ovatek_rcmode;

typedef enum _ovatek_aac_mode
{
	ovatek_aac_adts,
	ovatek_aac_latm,
}ovatek_aac_mode;

typedef enum _ovatek_audio_bitrate
{
	oaudio_bitrate_128,
	oaudio_bitrate_192,
	oaudio_bitrate_256,
}ovatek_audio_bitrate;

typedef struct _ovatek_h264_param
{
	h264_profile profile;
	h264_level level;
	ovatek_rcmode rcmode;
	uint8_t bframe;
	uint8_t keysec;
	uint16_t recv;
	uint32_t bitrate;
	uint32_t maxbitrate;
}ovatek_h264_param,*Povatek_h264_param;

typedef struct _ovatek_mpeg2_param
{
	mpeg2_profile profile;
	ovatek_rcmode rcmode;
	uint8_t bframe;
	uint8_t recv[3];
	uint32_t bitrate;
	uint32_t maxbitrate;
}ovatek_mpeg2_param,*Povatek_mpeg2_param;

class ovatekCodecStream;
typedef struct _ovatekFrame
{
	struct _ovatekFrame* prev;
	struct _ovatekFrame* next;
	mux_pes_frame pes_frame;
	uint64_t tick;
	uint8_t* rawbuf;
	ovatekCodecStream* stream;
}ovatekFrame, * PovatekFrame;

typedef struct _ovatek_aac_param
{
	ovatek_aac_mode mode;
	ovatek_audio_bitrate bitrate;
}ovatek_aac_param, * Povatek_aac_param;

class ovatekParam;
class ovatekParamFactory
{
public:
	explicit ovatekParamFactory(const uint8_t* def, int32_t size, int32_t advoffect, const Pui_prop_item uiprop, const Pui_prop_item uiadv);

	bool _is_advance() { return (m_uiadv != NULL); }
	const uint8_t* _default_param() { return m_defparam; }
	int32_t _param_size() { return m_size; }

	virtual ovatekParam* createParam();
	virtual void getObsDefault(obs_data_t* settings);
	virtual void getObsData(obs_data_t* settings, uint8_t* param);
	virtual void setParamObsData(uint8_t* param, obs_data_t* data);

	static void getOBSProperties(obs_properties_t* props, const Pui_prop_item uiprops);
	static void setObsDataWithUIPropsDefault(obs_data_t* settings, const Pui_prop_item uiprops,const uint8_t* raw);
	static void setObsDataWithUIProps(obs_data_t* settings, const Pui_prop_item uiprops, const uint8_t* raw);
	static void getObsDataWithUIProps(obs_data_t* settings, const Pui_prop_item uiprops,uint8_t* raw);
protected:
	const Pui_prop_item m_uiprop;
	const Pui_prop_item m_uiadv;
	const uint8_t* m_defparam;
	int32_t m_advoffect;
	int32_t m_size;
};

class ovatekParam
{
public:
	explicit ovatekParam(ovatekParamFactory* factory, uint8_t* raw,qtvPropertyCollection* properties);
	~ovatekParam();

	qtvPropertyCollection* _properties() { return m_properties; }
	uint8_t* _raw_param() { return m_rawbuf; }

	obs_data_t* createObsData();
	void reset();

	void setParamObsData(obs_data_t* settings);

protected:
	qtvPropertyCollection* m_properties;
	uint8_t* m_rawbuf;
	ovatekParamFactory* m_factory;
};

class ovatekFrameHandle
{
public:

	static vatek_result createFrameHandle(qtvCodec* codec, ovatekFrameHandle** frame);

	virtual vatek_result getHeader(PovatekFrame pframe, encoder_packet* encpkt) = 0;
	virtual vatek_result freeHeader(PovatekFrame pframe) = 0;
};

class ovatekCodec
{
public:
	static vatek_result createVideoProvider(qtvCodecSimpleProvider** provider);
	static vatek_result createAudioProvider(qtvCodecSimpleProvider** provider);
	static vatek_result createVideoCodec(const char* codecname, const char* name, obs_output_t* houtput, obs_encoder_t** encoder, ovatekParam* param);
	static vatek_result createAudioCodec(const char* codecname, const char* name, obs_output_t* houtput, obs_encoder_t** encoder, ovatekParam* param);
};

class ovatekCodecVideo : public qtvCodecVideo
{
public:
	explicit ovatekCodecVideo(qtvCodecFactory* factory, QString name, ovatekParam* param);
	~ovatekCodecVideo();

	/* qtvCodec*/
	void resetParam() override;

	virtual vatek_result createVideoCodec(obs_output_t* houtput, obs_encoder_t** encoder) = 0;
protected:
	ovatekParam* m_oparam;
};

class ovatekCodecAudio : public qtvCodecAudio
{
public:
	explicit ovatekCodecAudio(qtvCodecFactory* factory, QString name, ovatekParam* param);
	~ovatekCodecAudio();

	/* qtvCodec*/
	void resetParam() override;

	virtual vatek_result createAudioCodec(obs_output_t* houtput, obs_encoder_t** encoder) = 0;

	static int32_t getAudioBitrate(ovatek_audio_bitrate bitrate)
	{
		if (bitrate == oaudio_bitrate_128)return 128;
		else if (bitrate == oaudio_bitrate_192)return 192;
		else if (bitrate == oaudio_bitrate_256)return 256;
		else VWAR("unknown bitrate : %d", bitrate);
		return 128;
	}

protected:

	ovatekParam* m_oparam;
};

class ovatekCodecStream
{
public:
	static vatek_result createStream(qtvCodec* codec, Pusbmux_stream pustream, ovatekCodecStream** ovstream);
	
	~ovatekCodecStream();

	uint32_t _tick_ms() { return m_tickms; }
	Pusbmux_stream _ustream_handle() { return m_ustream; }

	vatek_result createPesFrame(PovatekFrame* frame, encoder_packet* encpkt);
	vatek_result freePesFrame(PovatekFrame frame);

protected:
	ovatekCodecStream(int32_t memsize, qtvCodec* codec, ovatekFrameHandle* frame,Pusbmux_stream ustream);
	hcross_mutex m_lock;
	Pth_memquery m_bufquery;
	ovatekFrameHandle* m_frame;
	qtvCodec* m_codec;
	Pusbmux_stream m_ustream;
	uint32_t m_tickms;
};

#endif
