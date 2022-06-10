#include <codec/ffmpeg/ovatek_ffmpeg.h>
#include <codec/ffmpeg/ovatek_ffmpeg_nvenc.h>
#include <codec/ffmpeg/ovatek_ffmpeg_qsv.h>
#include <codec/ffmpeg/ovatek_ffmpeg_sw.h>
#include <obs_vatek.h>
#include <obs-avc.h>

#define _ff_video(tag,id,hw)	{OVATEK_FFMPEG_##tag,AV_CODEC_ID_##id,OBS_ENCODER_VIDEO,AV_HWDEVICE_TYPE_##hw,ovatekFactory##tag::create},
#define _ff_audio(tag,id)		{OVATEK_FFMPEG_##tag,AV_CODEC_ID_##id,OBS_ENCODER_AUDIO,AV_HWDEVICE_TYPE_NONE,ovatekFactoryFFmpegAudio::create},

static const offmpeg_codec ovatek_ffcodec_table[] =
{
	_ff_video(MPEG2VIDEO	,MPEG2VIDEO	,NONE)
//	_ff_video(QSV_H264		,H264		,QSV)
//	_ff_video(QSV_HEVC		,HEVC		,QSV)
//	_ff_video(NVENC_H264	,H264		,CUDA)
//	_ff_video(NVENC_HEVC	,HEVC		,CUDA)
	_ff_audio(MP2			,MP2)
	_ff_audio(A52			,AC3)
//	_ff_audio(AAC			,AAC)
};

#define OFFMPEG_CODEC_NUMS	(sizeof(ovatek_ffcodec_table)/sizeof(offmpeg_codec))


static inline void copy_data(AVFrame* pic, const struct encoder_frame* frame,
	int height, enum AVPixelFormat format)
{
	int h_chroma_shift, v_chroma_shift;
	av_pix_fmt_get_chroma_sub_sample(format, &h_chroma_shift,
		&v_chroma_shift);
	for (int plane = 0; plane < MAX_AV_PLANES; plane++) {
		if (!frame->data[plane])
			continue;

		int frame_rowsize = (int)frame->linesize[plane];
		int pic_rowsize = pic->linesize[plane];
		int bytes = frame_rowsize < pic_rowsize ? frame_rowsize
			: pic_rowsize;
		int plane_height = height >> (plane ? v_chroma_shift : 0);

		for (int y = 0; y < plane_height; y++) {
			int pos_frame = y * frame_rowsize;
			int pos_pic = y * pic_rowsize;

			memcpy(pic->data[plane] + pos_pic,
				frame->data[plane] + pos_frame, bytes);
		}
	}
}


/* ovatekFFmpegBase */


ovatekFFmpegHandle::ovatekFFmpegHandle(ovatekFFmpegFactory* factory) :
	m_codec(factory->_codec()),
	m_ffcodec(factory->_ff_codec()),
	m_context(NULL),
	m_frame(NULL),
	m_factory(factory),
	m_framemode(ff_frame_bypass),
	m_framecount(0),
	m_isfirstpkt(0),
	m_obsencoder(NULL),
	m_hdrbuf(NULL),
	m_hdrlen(0),
	m_ffhandle(NULL)
{
	
}

ovatekFFmpegHandle::~ovatekFFmpegHandle()
{
	closeCodec();
}

vatek_result ovatekFFmpegHandle::openCodec(obs_data_t* settings, obs_encoder_t* encoder)
{
	vatek_result nres = vatek_badstatus;
	if (!m_context)
	{
		obs_encoder_type type = obs_encoder_get_type(encoder);
		if (type == OBS_ENCODER_VIDEO)
			nres = openVideoCodec(settings, encoder);
		else if (type == OBS_ENCODER_AUDIO)
			nres = openAudioCodec(settings, encoder);
		else blog(LOG_ERROR, "unknown obs codec type : %d", type);
	}

	if (!is_vatek_success(nres))blog(LOG_ERROR, "open ovatekFFmpeg fail : [%d:%s]", nres, m_ffcodec->name);
	else
	{
		m_isfirstpkt = 1;	/* process sei only first packet */
		m_obsencoder = encoder;
	}
	return nres;
}

void ovatekFFmpegHandle::closeCodec()
{
	if (m_obsencoder)
	{
		obs_encoder_type type = obs_encoder_get_type(m_obsencoder);
		if (type == OBS_ENCODER_VIDEO)closeVideoCodec();
		else if (type == OBS_ENCODER_AUDIO)closeAudioCodec();
		m_obsencoder = NULL;
	}
}

vatek_result ovatekFFmpegHandle::openVideoCodec(obs_data_t* settings, obs_encoder_t* encoder)
{
	vatek_result nres = vatek_memfail;
	m_context = avcodec_alloc_context3(m_codec);
	if (m_context)
	{
		m_ffhandle = malloc_ffhandle(OBS_ENCODER_VIDEO);
		if (m_ffhandle)
		{
			Poffmpeg_video_handle pffvideo = &m_ffhandle->_handle.video;
			m_framemode = setVideoContext(m_context, encoder, m_factory, &pffvideo->pixelfmt);
			nres = m_factory->configFFmpegContext(settings, m_context);
			if (is_vatek_success(nres))
			{
				nres = openVideoFrame();
				if (is_vatek_success(nres))
				{
					nres = openVideoFFmpeg();
					if (!is_vatek_success(nres))closeVideoFrame();
				}
			}
		}
		else avcodec_free_context(&m_context);
	}
	return nres;
}

void ovatekFFmpegHandle::closeVideoCodec()
{
	if (m_context)
	{
		if (m_framecount)
		{
			AVPacket pkt = { 0 };
			for (;;)
			{
				if (avcodec_receive_packet(m_context, &pkt) < 0)
					break;
			}
			m_framecount = 0;
		}

		closeVideoFrame();
		closeVideoFFmpeg();

		if (m_ffhandle)
		{
			free_ffhandle(m_ffhandle);
			m_ffhandle = NULL;
		}
	}
}

vatek_result ovatekFFmpegHandle::openVideoFFmpeg()
{
	vatek_result nres = vatek_success;
	if (m_ffcodec->hw != AV_HWDEVICE_TYPE_NONE)
	{

	}
	
	if (is_vatek_success(nres))
	{
		nres = (vatek_result)avcodec_open2(m_context, m_codec, NULL);
		if (!is_vatek_success(nres))blog(LOG_ERROR, "open ffmpeg codec fail : [%d:%s]", nres, m_ffcodec->name);
	}
	return nres;
}

void ovatekFFmpegHandle::closeVideoFFmpeg()
{
	if (m_context)
	{
		if (m_ffcodec->hw != AV_HWDEVICE_TYPE_NONE)
		{

		}
		avcodec_close(m_context);
		avcodec_free_context(&m_context);
	}
}

vatek_result ovatekFFmpegHandle::openVideoFrame()
{
	vatek_result nres = vatek_memfail;
	m_frame = av_frame_alloc();
	if (m_frame)
	{
		m_frame->format = m_context->pix_fmt;
		m_frame->width = m_context->width;
		m_frame->height = m_context->height;
		m_frame->colorspace = m_context->colorspace;
		m_frame->color_range = m_context->color_range;
		nres = (vatek_result)av_frame_get_buffer(m_frame, base_get_alignment());
		if (is_vatek_success(nres))da_init(m_dynbuffer);
		else
		{
			m_isfirstpkt = 1;
			av_frame_free(&m_frame);
		}
	}
	if (!is_vatek_success(nres))blog(LOG_ERROR, "create video frame fail : %d", nres);
	return nres;
}

void ovatekFFmpegHandle::closeVideoFrame()
{
	if (m_frame)
	{
		av_frame_unref(m_frame);
		av_frame_free(&m_frame);
		bfree(m_hdrbuf);
		da_free(m_dynbuffer);
		m_isfirstpkt = 0;
		m_hdrbuf = NULL;
		m_hdrlen = 0;
	}
}

vatek_result ovatekFFmpegHandle::openAudioCodec(obs_data_t* settings, obs_encoder_t* encoder)
{
	vatek_result nres = vatek_memfail;
	m_context = avcodec_alloc_context3(m_codec);
	if (m_context)
	{
		m_ffhandle = malloc_ffhandle(OBS_ENCODER_AUDIO);
		if (m_ffhandle)
		{
			Poffmpeg_audio_handle pffaudio = &m_ffhandle->_handle.audio;
			m_framemode = setAudioContext(m_context, m_codec, pffaudio,encoder);
			nres = m_factory->configFFmpegContext(settings, m_context);
			if (is_vatek_success(nres))
			{
				m_frame = av_frame_alloc();
				if (m_frame)
				{
					nres = (vatek_result)avcodec_open2(m_context, m_codec, NULL);
					if (is_vatek_success(nres))
					{
						m_frame->format = m_context->sample_fmt;
						m_frame->channels = m_context->channels;
						m_frame->channel_layout = m_context->channel_layout;
						m_frame->sample_rate = m_context->sample_rate;
						pffaudio->aframesize = m_context->frame_size;
						if (!pffaudio->aframesize)pffaudio->aframesize = 1024;
						pffaudio->aframesizebytes = pffaudio->aframesize * (int)pffaudio->asize;
						nres = (vatek_result)av_samples_alloc(pffaudio->asamples, NULL, m_context->channels,
															  pffaudio->aframesize, m_context->sample_fmt, 0);
						if (!is_vatek_success(nres))avcodec_close(m_context);
					}
					if (!is_vatek_success(nres))av_frame_free(&m_frame);
					else da_init(m_dynbuffer);
				}
			}
			if (!is_vatek_success(nres))
			{
				free_ffhandle(m_ffhandle);
				m_ffhandle = NULL;
			}
		}
		if (!is_vatek_success(nres))avcodec_free_context(&m_context);
	}
	return nres;
}

void ovatekFFmpegHandle::closeAudioCodec()
{
	if (m_context)
	{
		Poffmpeg_audio_handle pffaudio = &m_ffhandle->_handle.audio;
		avcodec_close(m_context);
		av_frame_free(&m_frame);
		da_free(m_dynbuffer);
		free_ffhandle(m_ffhandle);
		avcodec_free_context(&m_context);
		m_ffhandle = NULL;
	}
}

void* ovatekFFmpegHandle::obsCreate(const char* name, obs_data_t* settings, obs_encoder_t* encoder)
{
	ovatekFFmpegFactory* factory = static_cast<ovatekFFmpegFactory*>(obsVatekHandle::getVideoCodecByName(name));
	if(!factory)
		factory = static_cast<ovatekFFmpegFactory*>(obsVatekHandle::getAudioCodecByName(name));
	if (factory)
	{
		ovatekFFmpegHandle* newh = new ovatekFFmpegHandle(factory);
		vatek_result nres = vatek_memfail;
		if (newh)
		{
			nres = newh->openCodec(settings, encoder);
			if (!is_vatek_success(nres))delete newh;
		}

		if (!is_vatek_success(nres))blog(LOG_ERROR, "create obs handle fail : %d", nres);
		else return newh;
	}
	else blog(LOG_ERROR, "create %s not found", name);
	return NULL;
}

void ovatekFFmpegHandle::obsDestroy(void* data)
{
	ovatekFFmpegHandle* fhandle = static_cast<ovatekFFmpegHandle*>(data);
	delete fhandle;
}

bool ovatekFFmpegHandle::obsEncodeVideo(void* data, struct encoder_frame* frame, struct encoder_packet* packet, bool* received_packet)
{
	ovatekFFmpegHandle* hff = static_cast<ovatekFFmpegHandle*>(data);
	AVPacket pkt = { 0 };
	int32_t ispkt = 0;
	vatek_result nres = vatek_success;
	AVCodecContext* context = hff->m_context;
	AVFrame* fframe = hff->m_frame;
	Poffmpeg_video_handle pffvideo = &hff->m_ffhandle->_handle.video;

	av_init_packet(&pkt);

	copy_data(fframe, frame, context->height, context->pix_fmt);

	fframe->pts = frame->pts;
	nres = (vatek_result)avcodec_send_frame(context, fframe);
	if (nres == 0)
		nres = (vatek_result)avcodec_receive_packet(context, &pkt);

	if (is_vatek_success(nres))ispkt = 1;
	else
	{
		if (nres == AVERROR_EOF || nres == AVERROR(EAGAIN))nres = vatek_success;
		else
		{
			blog(LOG_WARNING, "ffmpeg encoder fail : [%d:%s]", nres, hff->m_codec->name);
			return false;
		}
	}

	if (ispkt && pkt.size) 
	{
		if (hff->m_isfirstpkt) 
		{
			uint8_t* new_packet;
			size_t size;

			hff->m_isfirstpkt = 0;

			if (context->codec->id != AV_CODEC_ID_MPEG2VIDEO)
			{
				obs_extract_avc_headers(pkt.data, pkt.size,
										&new_packet, &size,
										&hff->m_hdrbuf, &hff->m_hdrlen,
										&pffvideo->seibuf, &pffvideo->seilen);

				da_copy_array(hff->m_dynbuffer, new_packet, size);
				bfree(new_packet);
			}else da_copy_array(hff->m_dynbuffer, pkt.data, pkt.size);
		}
		else da_copy_array(hff->m_dynbuffer, pkt.data, pkt.size);

		packet->pts = pkt.pts;
		packet->dts = pkt.dts;
		packet->data = hff->m_dynbuffer.array;
		packet->size = hff->m_dynbuffer.num;
		packet->type = OBS_ENCODER_VIDEO;
		if (context->codec->id != AV_CODEC_ID_MPEG2VIDEO)
		{
			packet->keyframe = obs_avc_keyframe(packet->data, packet->size);
		}
		else
		{
			if (pkt.flags & AV_PKT_FLAG_KEY)
				packet->keyframe = true;
			else packet->keyframe = false;
		}
		*received_packet = true;
	}
	else *received_packet = false;
	av_packet_unref(&pkt);
	return true;
}

bool ovatekFFmpegHandle::obsExtraData(void* data, uint8_t** extra_data, size_t* size)
{
	ovatekFFmpegHandle* hff = static_cast<ovatekFFmpegHandle*>(data);
	*extra_data = hff->m_hdrbuf;
	*size = hff->m_hdrlen;
	return true;
}

bool ovatekFFmpegHandle::obsSeiData(void* data, uint8_t** extra_data, size_t* size)
{
	ovatekFFmpegHandle* hff = static_cast<ovatekFFmpegHandle*>(data);
	Poffmpeg_video_handle pffvideo = &hff->m_ffhandle->_handle.video;
	*extra_data = pffvideo->seibuf;
	*size = pffvideo->seilen;

	return true;
}

void ovatekFFmpegHandle::obsGetVideoInfo(void* data, struct video_scale_info* info)
{
	ovatekFFmpegHandle* hff = static_cast<ovatekFFmpegHandle*>(data);
	Poffmpeg_video_handle pffvideo = &hff->m_ffhandle->_handle.video;
	info->format = pffvideo->pixelfmt;
}

bool ovatekFFmpegHandle::obsEncodeAudio(void* data, struct encoder_frame* frame, struct encoder_packet* packet, bool* received_packet)
{
	ovatekFFmpegHandle* hff = static_cast<ovatekFFmpegHandle*>(data);
	AVCodecContext* context = hff->m_context;
	AVFrame* aframe = hff->m_frame;
	Poffmpeg_audio_handle pffaudio = &hff->m_ffhandle->_handle.audio;
	AVRational time_base = { 1, context->sample_rate };
	AVPacket avpacket = { 0 };
	int32_t i = 0;
	vatek_result nres = vatek_success;
	int32_t got_packet = 0;

	for (i = 0; i < pffaudio->aplanes; i++)
		memcpy(pffaudio->asamples[i], frame->data[i], pffaudio->aframesizebytes);


	aframe->nb_samples = pffaudio->aframesize;
	aframe->pts = av_rescale_q(pffaudio->atotalsamples, time_base,context->time_base);

	nres = (vatek_result)avcodec_fill_audio_frame(aframe,context->channels,
												  context->sample_fmt,
												  pffaudio->asamples[0], 
												  pffaudio->aframesizebytes * context->channels,
												  1);
	if (is_vatek_success(nres))
	{
		pffaudio->atotalsamples += pffaudio->aframesize;
		nres = (vatek_result)avcodec_send_frame(context, aframe);
		if (is_vatek_success(nres))
		{
			nres = (vatek_result)avcodec_receive_packet(context, &avpacket);
			if (is_vatek_success(nres))got_packet = 1;
			else if (nres == AVERROR_EOF || nres == AVERROR(EAGAIN))
				nres = vatek_success;
			else blog(LOG_ERROR, "avcodec_encode_audio fail : %d", nres);
		}

		if (is_vatek_success(nres))
		{
			*received_packet = (got_packet != 0);
			if (got_packet)
			{
				AVRounding round = (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
				da_resize(hff->m_dynbuffer, 0);
				da_push_back_array(hff->m_dynbuffer, avpacket.data, avpacket.size);
				packet->pts = av_rescale_q_rnd(avpacket.pts, context->time_base, time_base, round);
				packet->dts = av_rescale_q_rnd(avpacket.dts, context->time_base, time_base, round);
				packet->data = hff->m_dynbuffer.array;
				packet->size = avpacket.size;
				packet->type = OBS_ENCODER_AUDIO;
				packet->timebase_num = 1;
				packet->timebase_den = (int32_t)context->sample_rate;
				av_free_packet(&avpacket);
			}
		}
	}
	else blog(LOG_ERROR, "avcodec_fill_audio_frame fail : %d", nres);

	if (is_vatek_success(nres))return true;
	return false;
}
void ovatekFFmpegHandle::obsGetAudioInfo(void* data, struct audio_convert_info* info)
{
	ovatekFFmpegHandle* hff = static_cast<ovatekFFmpegHandle*>(data);
	Poffmpeg_audio_handle pffaudio = &hff->m_ffhandle->_handle.audio;
	info->format = getOBSAudioFormatFromFFmpeg(hff->m_context->sample_fmt);
	info->samples_per_sec = (uint32_t)hff->m_context->sample_rate;
	info->speakers = getOBSSpeakerLayoutFromFFmpeg(hff->m_context->channel_layout);
}

size_t ovatekFFmpegHandle::obsGetFrameSize(void* data)
{
	ovatekFFmpegHandle* hff = static_cast<ovatekFFmpegHandle*>(data);
	Poffmpeg_audio_handle pffaudio = &hff->m_ffhandle->_handle.audio;
	return pffaudio->aframesize;
}

void ovatekFFmpegHandle::getOBSDefault(obs_data_t* settings, const Pui_prop_item uiprops, const Pui_prop_item advui, uint8_t* param, int32_t advoffect)
{
	ovatekParamFactory::setObsDataWithUIPropsDefault(settings, uiprops, param);
	if(advui)
		ovatekParamFactory::setObsDataWithUIPropsDefault(settings, advui, &param[advoffect]);
}

void ovatekFFmpegHandle::setVideoContextCodec(AVCodecContext* context, ovatek_rcmode rcmode, uint32_t bitrate, uint32_t maxbitrate, uint8_t bframe, int32_t keysec)
{
	if (rcmode == ovatek_rc_cbr)
	{
		context->bit_rate = bitrate * 1000;
		context->rc_max_rate = bitrate * 1000;
	}
	else
	{
		context->bit_rate = bitrate * 1000;
		context->rc_max_rate = bitrate * 1000;
		context->rc_min_rate = bitrate * 1000;
	}

	if (!bframe)
	{
		context->has_b_frames = 0;
		context->max_b_frames = 0;
	}
	else context->max_b_frames = bframe;

	if (!keysec)
	{
		if (context->codec->id == AV_CODEC_ID_MPEG2VIDEO)
			context->gop_size = 16;
		else context->gop_size = 60;
	}
	else context->gop_size = keysec * (context->time_base.num / context->time_base.den);
}

void ovatekFFmpegHandle::setAudioContextCodec(AVCodecContext* context, uint32_t bitrate)
{
	context->bit_rate = bitrate * 1000;
}

uint64_t ovatekFFmpegHandle::getOBSChannelLayoutToFFmpeg(enum speaker_layout layout)
{
	switch (layout) {
	case SPEAKERS_UNKNOWN:
		return 0;
	case SPEAKERS_MONO:
		return AV_CH_LAYOUT_MONO;
	case SPEAKERS_STEREO:
		return AV_CH_LAYOUT_STEREO;
	case SPEAKERS_2POINT1:
		return AV_CH_LAYOUT_SURROUND;
	case SPEAKERS_4POINT0:
		return AV_CH_LAYOUT_4POINT0;
	case SPEAKERS_4POINT1:
		return AV_CH_LAYOUT_4POINT1;
	case SPEAKERS_5POINT1:
		return AV_CH_LAYOUT_5POINT1_BACK;
	case SPEAKERS_7POINT1:
		return AV_CH_LAYOUT_7POINT1;
	}
	/* shouldn't get here */
	return 0;
}

enum speaker_layout ovatekFFmpegHandle::getOBSSpeakerLayoutFromFFmpeg(uint64_t nchannel)
{
	switch (nchannel) {
	case AV_CH_LAYOUT_MONO:
		return SPEAKERS_MONO;
	case AV_CH_LAYOUT_STEREO:
		return SPEAKERS_STEREO;
	case AV_CH_LAYOUT_SURROUND:
		return SPEAKERS_2POINT1;
	case AV_CH_LAYOUT_4POINT0:
		return SPEAKERS_4POINT0;
	case AV_CH_LAYOUT_4POINT1:
		return SPEAKERS_4POINT1;
	case AV_CH_LAYOUT_5POINT1_BACK:
		return SPEAKERS_5POINT1;
	case AV_CH_LAYOUT_7POINT1:
		return SPEAKERS_7POINT1;
	}

	/* shouldn't get here */
	return SPEAKERS_UNKNOWN;
}

enum audio_format ovatekFFmpegHandle::getOBSAudioFormatFromFFmpeg(enum AVSampleFormat format)
{
	switch ((uint32_t)format) {
	case AV_SAMPLE_FMT_U8:
		return AUDIO_FORMAT_U8BIT;
	case AV_SAMPLE_FMT_S16:
		return AUDIO_FORMAT_16BIT;
	case AV_SAMPLE_FMT_S32:
		return AUDIO_FORMAT_32BIT;
	case AV_SAMPLE_FMT_FLT:
		return AUDIO_FORMAT_FLOAT;
	case AV_SAMPLE_FMT_U8P:
		return AUDIO_FORMAT_U8BIT_PLANAR;
	case AV_SAMPLE_FMT_S16P:
		return AUDIO_FORMAT_16BIT_PLANAR;
	case AV_SAMPLE_FMT_S32P:
		return AUDIO_FORMAT_32BIT_PLANAR;
	case AV_SAMPLE_FMT_FLTP:
		return AUDIO_FORMAT_FLOAT_PLANAR;
	}

	/* shouldn't get here */
	return AUDIO_FORMAT_16BIT;
}

enum AVPixelFormat ovatekFFmpegHandle::getOBSPixelFormatToFFmpeg(enum video_format pixfmt)
{
	switch (pixfmt) 
	{
		case VIDEO_FORMAT_I444:
			return AV_PIX_FMT_YUV444P;
		case VIDEO_FORMAT_I420:
			return AV_PIX_FMT_YUV420P;
		case VIDEO_FORMAT_NV12:
			return AV_PIX_FMT_NV12;
		case VIDEO_FORMAT_YUY2:
			return AV_PIX_FMT_YUYV422;
		case VIDEO_FORMAT_UYVY:
			return AV_PIX_FMT_UYVY422;
		case VIDEO_FORMAT_RGBA:
			return AV_PIX_FMT_RGBA;
		case VIDEO_FORMAT_BGRA:
			return AV_PIX_FMT_BGRA;
		case VIDEO_FORMAT_BGRX:
			return AV_PIX_FMT_BGRA;
		case VIDEO_FORMAT_Y800:
			return AV_PIX_FMT_GRAY8;
		case VIDEO_FORMAT_BGR3:
			return AV_PIX_FMT_BGR24;
		case VIDEO_FORMAT_I422:
			return AV_PIX_FMT_YUV422P;
		case VIDEO_FORMAT_I40A:
			return AV_PIX_FMT_YUVA420P;
		case VIDEO_FORMAT_I42A:
			return AV_PIX_FMT_YUVA422P;
		case VIDEO_FORMAT_YUVA:
			return AV_PIX_FMT_YUVA444P;
		case VIDEO_FORMAT_NONE:
		case VIDEO_FORMAT_YVYU:
		case VIDEO_FORMAT_AYUV:
			/* not supported by FFmpeg */
			return AV_PIX_FMT_NONE;
	}

	return AV_PIX_FMT_NONE;
}

obs_encoder_info* ovatekFFmpegHandle::mallocOBSPlugin(obs_encoder_type type)
{
	obs_encoder_info* newinfo = (obs_encoder_info*)malloc(sizeof(obs_encoder_info));
	if (newinfo)
	{
		memset(newinfo, 0, sizeof(obs_encoder_info));
		newinfo->id = NULL;
		newinfo->type = type;
		newinfo->codec = NULL;
		newinfo->get_name = NULL;
		newinfo->create = NULL;
		newinfo->get_defaults = NULL;
		newinfo->get_properties = NULL;
		newinfo->destroy = ovatekFFmpegHandle::obsDestroy;
		newinfo->get_extra_data = ovatekFFmpegHandle::obsExtraData;

		if (type == OBS_ENCODER_VIDEO)
		{
			newinfo->encode = ovatekFFmpegHandle::obsEncodeVideo;
			newinfo->get_sei_data = ovatekFFmpegHandle::obsSeiData;
			newinfo->get_video_info = ovatekFFmpegHandle::obsGetVideoInfo;
		}
		else if (type == OBS_ENCODER_AUDIO)
		{
			newinfo->get_audio_info = ovatekFFmpegHandle::obsGetAudioInfo;
			newinfo->encode = ovatekFFmpegHandle::obsEncodeAudio;
			newinfo->get_frame_size = ovatekFFmpegHandle::obsGetFrameSize;
		}
		else
		{
			free(newinfo);
			newinfo = NULL;
		}
	}
	return newinfo;
}

offmpeg_frame_mode ovatekFFmpegHandle::setAudioContext(AVCodecContext* context, AVCodec* codec, Poffmpeg_audio_handle pffaudio, obs_encoder_t* enc)
{
	audio_t* ainfo = obs_encoder_audio(enc);
	const struct audio_output_info* aoi = audio_output_get_info(ainfo);

	context->channels = (int)audio_output_get_channels(ainfo);
	context->channel_layout = getOBSChannelLayoutToFFmpeg(aoi->speakers);
	context->sample_rate = audio_output_get_sample_rate(ainfo);
	context->sample_fmt = codec->sample_fmts ? codec->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
	context->strict_std_compliance = -2;
	context->flags = AV_CODEC_FLAG_GLOBAL_HEADER;

	pffaudio->aformat = getOBSAudioFormatFromFFmpeg(context->sample_fmt);
	pffaudio->aplanes = get_audio_planes(pffaudio->aformat, aoi->speakers);
	pffaudio->asize = get_audio_size(pffaudio->aformat, aoi->speakers, 1);
	return ff_frame_audio;
}

offmpeg_frame_mode ovatekFFmpegHandle::setVideoContext(AVCodecContext* context, obs_encoder_t* enc, ovatekFFmpegFactory* factory, video_format* pixelfmt)
{
	video_t* vinfo = obs_encoder_video(enc);
	const struct video_output_info* voi = video_output_get_info(vinfo);
	video_format surfmt = voi->format;
	video_format destfmt = factory->getVideoFormat(voi->format);

	context->width = obs_encoder_get_width(enc);
	context->height = obs_encoder_get_height(enc);
	context->time_base.num = voi->fps_den;
	context->time_base.den = voi->fps_num;
	context->pix_fmt = getOBSPixelFormatToFFmpeg(destfmt);
	context->color_range = voi->range == VIDEO_RANGE_FULL ? AVCOL_RANGE_JPEG : AVCOL_RANGE_MPEG;

	switch (voi->colorspace)
	{
	case VIDEO_CS_601:
		context->color_trc = AVCOL_TRC_SMPTE170M;
		context->color_primaries = AVCOL_PRI_SMPTE170M;
		context->colorspace = AVCOL_SPC_SMPTE170M;
		break;
	case VIDEO_CS_DEFAULT:
	case VIDEO_CS_709:
		context->color_trc = AVCOL_TRC_BT709;
		context->color_primaries = AVCOL_PRI_BT709;
		context->colorspace = AVCOL_SPC_BT709;
		break;
	case VIDEO_CS_SRGB:
		context->color_trc = AVCOL_TRC_IEC61966_2_1;
		context->color_primaries = AVCOL_PRI_BT709;
		context->colorspace = AVCOL_SPC_BT709;
		break;
	}

	*pixelfmt = destfmt;
	return ff_frame_bypass;
}

obs_properties_t* ovatekFFmpegHandle::createOBSProperties(const Pui_prop_item uiprops, const Pui_prop_item uiadv)
{
	obs_properties_t* newprops = obs_properties_create();
	if (newprops)
	{
		ovatekParamFactory::getOBSProperties(newprops, uiprops);
		if(uiadv)ovatekParamFactory::getOBSProperties(newprops,uiadv);
	}
	return newprops;
}

Poffmpeg_handle ovatekFFmpegHandle::malloc_ffhandle(obs_encoder_type type)
{
	Poffmpeg_handle newh = (Poffmpeg_handle)bzalloc(sizeof(offmpeg_handle));
	newh->type = type;
	return newh;
}

void ovatekFFmpegHandle::free_ffhandle(Poffmpeg_handle pffhandle)
{
	if (pffhandle)
	{
		if (pffhandle->type == OBS_ENCODER_VIDEO)
		{
			Poffmpeg_video_handle pvideo = &pffhandle->_handle.video;
			if (pvideo->seibuf)bfree(pvideo->seibuf);
		}
		else
		{
			Poffmpeg_audio_handle pffaudio = &pffhandle->_handle.audio;
			av_freep(&pffaudio->asamples[0]);
		}
		bfree(pffhandle);
	}
}

/* ovatekFFMpegFactory */

vatek_result ovatekFFmpegFactory::registerCodecs(qtvCodecSimpleProvider* vprovider, qtvCodecSimpleProvider* aprovider)
{
	vatek_result nres = vatek_success;
	int32_t i = 0;
	for (i = 0; i < OFFMPEG_CODEC_NUMS; i++)
	{
		AVCodec* pavcodec = avcodec_find_encoder_by_name(ovatek_ffcodec_table[i].name);
		if (pavcodec)
		{
			ovatekFFmpegFactory* factory = NULL;
			nres = ovatek_ffcodec_table[i].create(pavcodec,&ovatek_ffcodec_table[i], &factory);
			if (is_vatek_success(nres))
			{
				if (ovatek_ffcodec_table[i].type == OBS_ENCODER_VIDEO)
					vprovider->addCodec(factory);
				else if (ovatek_ffcodec_table[i].type == OBS_ENCODER_AUDIO)
					aprovider->addCodec(factory);
				else nres = vatek_badparam;

				if (!is_vatek_success(nres))delete factory;
				else obs_register_encoder(factory->_obs_plugin());
			}

			if (!is_vatek_success(nres))
				blog(LOG_WARNING, "ovatekFFmpeg add codec fail : [%s:%d]", ovatek_ffcodec_table[i].name, nres);
		}
		else blog(LOG_WARNING, "ovatekFFmpeg not support : %s", ovatek_ffcodec_table[i].name);
	}
	return vatek_success;
}

ovatekFFmpegFactory::ovatekFFmpegFactory(const Poffmpeg_codec ffcodec,
										 const char* name,
										 qtv_codec_mode mode,
										 AVCodec* codec,
										 obs_encoder_info* obsinfo,
										 ovatekParamFactory* factory) :
	m_obsplugin(obsinfo),
	m_ffcodec(ffcodec),
	m_codec(codec),
	m_paramfactory(factory),
	qtvCodecFactory(name, mode)
{

}

vatek_result ovatekFFmpegFactory::createCodec(qtvCodec** codec)
{
	vatek_result nres = vatek_success;
	if (m_codec->type == AVMEDIA_TYPE_VIDEO)
		*codec = new ovatekFFmpegVideo(this, m_codec);
	else if (m_codec->type == AVMEDIA_TYPE_AUDIO)
		*codec = new ovatekFFmpegAudio(this, m_codec);
	else nres = vatek_unsupport;

	return nres;
}

void ovatekFFmpegFactory::freeCodec(qtvCodec* codec)
{
	delete codec;
}

