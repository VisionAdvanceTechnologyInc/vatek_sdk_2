#include <obs_vatek.h>

#define OSOURCE_TAG_VIDEO		0x00000001
#define OSOURCE_TAG_AUDIO		0x00000002
#define OSOURCE_TAG_AV			(OSOURCE_TAG_VIDEO | OSOURCE_TAG_AUDIO)

#define OSOURCE_PROGRAM_NUM		1
#define OSOURCE_STREAMS_NUM		2


obsVatekOutput::obsVatekOutput(qtvUIMainUSBMuxer* main) :
	m_hmain(main),
	m_usbmuxer(NULL),
	m_houtput(NULL),
	m_videoenc(NULL),
	m_audioenc(NULL),
	m_videostream(NULL),
	m_audiostream(NULL),
	m_lock(NULL),
	m_status(qstatus_idle)
{
	m_usbmuxer = qobject_cast<qtvServiceUSBMuxer*>(m_hmain->_service());
	m_source.humsource = this;
	m_source.start = ovatekSourceStart;
	m_source.stop = ovatekSourceStop;
	m_source.freeframe = ovatekSourceFreeFrame;
	m_source.getpadding = ovatekSourceGetPadding;
	m_source.prepare = ovatekSourcePrepare;

	if (!is_vatek_success(cross_os_create_mutex(&m_lock)))VWAR("create mutex fail");
}

obsVatekOutput::~obsVatekOutput()
{
	closeOBSHandle();
	cross_os_free_mutex(m_lock);
}

vatek_result obsVatekOutput::startProgram(qtvUSBMuxerProgram* program, Pusbmux_program uprogram, hqtv_program* hprogram)
{
	*hprogram = NULL;
	return vatek_success;
}

vatek_result obsVatekOutput::pushStream(hqtv_program hprogram, qtvUSBMuxerStream* stream, Pusbmux_stream ustream)
{
	ovatekCodecStream* newstream = NULL;
	vatek_result nres = ovatekCodecStream::createStream(stream->_codec(), ustream, &newstream);
	if (is_vatek_success(nres))
	{
		qtv_codec_mode mode = stream->_codec()->_mode();
		if (mode == qcodec_video && !m_videostream)
			m_videostream = newstream;
		else if (mode == qcodec_audio && !m_audiostream)
			m_audiostream = newstream;
		else nres = vatek_badstatus;
		if (!is_vatek_success(nres))delete newstream;
	}
	return nres;
}

void obsVatekOutput::stopProgram(hqtv_program hprogram)
{

}

vatek_result obsVatekOutput::sendEncoderPacket(encoder_packet* encpkt)
{
	vatek_result nres = vatek_badstatus;

	if (m_status == qstatus_ready || m_status == qstatus_running)
	{
		PovatekFrame newframe = NULL;
		ovatekCodecStream* pstream = NULL;
		nres = vatek_unsupport;
		if (encpkt->type == OBS_ENCODER_VIDEO)
		{
			nres = vatek_badstatus;
			if (m_videostream)
			{
				pstream = m_videostream;
				cross_os_lock_mutex(m_lock);
				nres = m_videostream->createPesFrame(&newframe, encpkt);
				if (is_vatek_success(nres))
					cross_os_release_mutex(m_lock);
			}
		}
		else if (encpkt->type == OBS_ENCODER_AUDIO)
		{
			nres = vatek_badstatus;
			if (m_audiostream)
			{
				pstream = m_audiostream;
				cross_os_lock_mutex(m_lock);
				nres = m_audiostream->createPesFrame(&newframe, encpkt);
				cross_os_release_mutex(m_lock);
			}
		}

		if (is_vatek_success(nres))
			nres = m_usbmuxer->sendFrame(pstream->_ustream_handle(), &newframe->pes_frame);
		if (!is_vatek_success(nres))VWAR("send encoder packet fail : %d", nres);
	}

	return nres;
}

vatek_result obsVatekOutput::ovatekSourceStart(husbmux_source hsource)
{
	obsVatekOutput* source = (obsVatekOutput*)hsource;
	vatek_result nres = vatek_hwfail;
	cross_os_lock_mutex(source->m_lock);
	if (obs_output_can_begin_data_capture(source->m_houtput, OBS_OUTPUT_AV))
	{
		if (obs_output_initialize_encoders(source->m_houtput, OBS_OUTPUT_AV))
		{
			if (obs_output_begin_data_capture(source->m_houtput, OBS_OUTPUT_AV))
				nres = vatek_success;
		}
	}
	if (is_vatek_success(nres))source->m_status = qstatus_ready;
	cross_os_release_mutex(source->m_lock);
	blog(LOG_INFO,"ovatekSourceStart");
	return vatek_success;
}

vatek_result obsVatekOutput::ovatekSourceFreeFrame(husbmux_source hsource, Pusbmux_stream pstream, Pmux_pes_frame pframe)
{
	obsVatekOutput* source = (obsVatekOutput*)hsource;
	ovatekCodecStream* stream = (ovatekCodecStream*)vatek_usbmux_stream_get_private(pstream);
	vatek_result nres = vatek_success;

	cross_os_lock_mutex(source->m_lock);
		nres = stream->freePesFrame((PovatekFrame)pframe->private_data);
	cross_os_release_mutex(source->m_lock);
	return nres;
}

vatek_result obsVatekOutput::ovatekSourceGetPadding(husbmux_source hsource, uint8_t** packet)
{
	return vatek_success;
}

void obsVatekOutput::ovatekSourcePrepare(husbmux_source hsource, uint32_t ms)
{
	obsVatekOutput* source = (obsVatekOutput*)hsource;
	ovatekCodecStream* video = source->m_videostream;
	if (source->m_status == qstatus_ready)
	{
		int32_t qms = video->_tick_ms() - ms;
		while (qms < OVATEK_PREPARE_MS)
		{
			cross_os_sleep(1);
			qms = video->_tick_ms() - ms;
		}

		cross_os_lock_mutex(source->m_lock);
			source->m_status = qstatus_running;
		cross_os_release_mutex(source->m_lock);
		blog(LOG_INFO,"start Source Prepare : [%d:%d]", ms, video->_tick_ms());
	}

	if (source->m_status == qstatus_running)
	{
		if (ms > video->_tick_ms())
		{
			while (ms > video->_tick_ms())
			{
				cross_os_sleep(1);
				blog(LOG_INFO, "too late : [%d:%d]", ms, video->_tick_ms());
			}
		}
		else blog(LOG_INFO, "too fast : %d", video->_tick_ms() - ms);
	}
}

void obsVatekOutput::ovatekSourceStop(husbmux_source hsource)
{
	obsVatekOutput* source = (obsVatekOutput*)hsource;
	cross_os_lock_mutex(source->m_lock);
	if (source->m_status == qstatus_ready || source->m_status == qstatus_running)
	{
		obs_output_end_data_capture(source->m_houtput);
		source->m_status = qstatus_stop;
	}		
	cross_os_release_mutex(source->m_lock);
	blog(LOG_INFO,"ovatekSourceStop");
}

vatek_result obsVatekOutput::openOBSHandle()
{
	vatek_result nres = vatek_badstatus;
	if (!m_houtput)
	{
		qtvUSBMuxerChannel* pch = NULL;
		nres = m_usbmuxer->getChannel(&pch);
		if (is_vatek_success(nres))nres = checkChannelValid(pch);

		if (is_vatek_success(nres))
		{
			m_houtput = obs_output_create(OVATEK_PLUGIN_OUTPUT, "ovatek-dtv-broadcast", NULL, NULL);
			if (m_houtput)
			{
				qtvUSBMuxerStream* pstream = getChannelStream(pch, qcodec_video);
				nres = vatek_unsupport;
				if (pstream)
				{
					ovatekCodecVideo* codec = static_cast<ovatekCodecVideo*>(pstream->_codec());
					configVideoDetail(pstream->_raw_stream(), obs_output_video(m_houtput));
					nres = codec->createVideoCodec(m_houtput, &m_videoenc);
					if (is_vatek_success(nres))
						obs_output_set_video_encoder(m_houtput, m_videoenc);
				}

				if (is_vatek_success(nres))
				{
					pstream = getChannelStream(pch,qcodec_audio);
					nres = vatek_unsupport;
					if (pstream)
					{
						ovatekCodecAudio* codec = static_cast<ovatekCodecAudio*>(pstream->_codec());
						configAudioDetail(pstream->_raw_stream(), obs_output_audio(m_houtput));
						nres = codec->createAudioCodec(m_houtput, &m_audioenc);
						if (is_vatek_success(nres))
							obs_output_set_audio_encoder(m_houtput, m_audioenc, 0);
					}
				}

				if (is_vatek_success(nres))nres = m_usbmuxer->startBroadcast(this);
				if (!is_vatek_success(nres))closeOBSHandle();
			}
			else nres = vatek_unsupport;
		}
	}
	return nres;
}

vatek_result obsVatekOutput::closeOBSHandle()
{
	vatek_result nres = vatek_badstatus;
	if (m_status != qstatus_idle)
	{
		nres = m_usbmuxer->stopBroadcast();
		if (!is_vatek_success(nres))VWAR("stop usb_muxer fail : %d", nres);
		obs_output_stop(m_houtput);
	}
	else nres = vatek_success;

	if (m_videoenc)obs_encoder_release(m_videoenc);
	if (m_audioenc)obs_encoder_release(m_audioenc);
	if (m_houtput)obs_output_release(m_houtput);
	if (m_videostream)delete m_videostream;
	if (m_audiostream)delete m_audiostream;

	m_houtput = NULL;
	m_videoenc = NULL;
	m_audioenc = NULL;
	m_videostream = NULL;
	m_audiostream = NULL;
	m_status = qstatus_idle;
	return nres;
}

vatek_result obsVatekOutput::startBroadcast(qtvUIMainUSBMuxer* umain, obsVatekOutput** obsoutput)
{
	obsVatekOutput* newoutput = new obsVatekOutput(umain);
	vatek_result nres = newoutput->openOBSHandle();
	if (is_vatek_success(nres))*obsoutput = newoutput;
	return nres;
}

vatek_result obsVatekOutput::stopBroadcast(obsVatekOutput* obsoutput)
{
	vatek_result nres = obsoutput->closeOBSHandle();
	delete obsoutput;
	return nres;
}

vatek_result obsVatekOutput::checkChannelValid(qtvUSBMuxerChannel* ch)
{
	vatek_result nres = vatek_unsupport;
	if (ch->_programs().count() == OSOURCE_PROGRAM_NUM &&
		ch->_programs()[0]->_streams().count() == OSOURCE_STREAMS_NUM)
	{
		int32_t i = 0;
		uint32_t tag = 0;
		for (i = 0; i < OSOURCE_STREAMS_NUM; i++)
		{
			qtvUSBMuxerStream* pstream = qobject_cast<qtvUSBMuxerStream*>(ch->_programs()[0]->_streams()[i]);
			if (pstream->_codec()->_mode() == qcodec_video)
				tag |= OSOURCE_TAG_VIDEO;
			else if (pstream->_codec()->_mode() == qcodec_audio)
				tag |= OSOURCE_TAG_AUDIO;
		}

		if (tag == OSOURCE_TAG_AV)nres = vatek_success;
	}
	return nres;
}

qtvUSBMuxerStream* obsVatekOutput::getChannelStream(qtvUSBMuxerChannel* ch, qtv_codec_mode mode)
{
	int32_t i = 0;
	uint32_t tag = 0;
	for (i = 0; i < OSOURCE_STREAMS_NUM; i++)
	{
		qtvUSBMuxerStream* pstream = qobject_cast<qtvUSBMuxerStream*>(ch->_programs()[0]->_streams()[i]);
		if (pstream->_codec()->_mode() == mode)
			return pstream;
	}
	return NULL;
}

void obsVatekOutput::configVideoDetail(Pmux_stream pvideo, video_t* vinfo)
{
	uint32_t width = video_output_get_width(vinfo);
	uint32_t height = video_output_get_height(vinfo);
	uint32_t fps = (uint32_t)video_output_get_frame_rate(vinfo);
	Pmedia_video pinfo = &pvideo->_stream_info.video;
	pinfo->resolution = resolution_unknown;
	pinfo->framerate = framerate_unknown;
	pinfo->aspectrate = aspectrate_16_9;

	if (width == 1920 && height == 1080)pinfo->resolution = resolution_1080p;
	else if (width == 1280 && height == 720)pinfo->resolution = resolution_720p;
	else if (width == 720)
	{
		if (height == 480)pinfo->resolution = resolution_480p;
		else if (height == 576)pinfo->resolution = resolution_576p;
		else VWAR("unknown resolution height : %d", height);
	}
	else VWAR("unknown resolution width : %d", width);

	if (fps == 60)pinfo->framerate = framerate_60;
	else if (fps == 59)pinfo->framerate = framerate_59_94;
	else if (fps == 50)pinfo->framerate = framerate_50;
	else if (fps == 30)pinfo->framerate = framerate_30;
	else if (fps == 29)pinfo->framerate = framerate_29_97;
	else if (fps == 25)pinfo->framerate = framerate_25;
	else if (fps == 24)pinfo->framerate = framerate_24;
	else if (fps == 23)pinfo->framerate = framerate_23_97;
	else VWAR("unknown fps : %d", fps);
}

void obsVatekOutput::configAudioDetail(Pmux_stream paudio, audio_t* ainfo)
{
	uint32_t sb = audio_output_get_sample_rate(ainfo);
	uint32_t ch = (uint32_t)audio_output_get_channels(ainfo);
	Pmedia_audio pinfo = &paudio->_stream_info.audio;

	pinfo->samplerate = sample_rate_unknown;
	pinfo->channel = channel_mute;

	if (sb == 48000)pinfo->samplerate = sample_rate_48;
	else if (sb == 44100)pinfo->samplerate = sample_rate_44_1;
	else if (sb == 32000)pinfo->samplerate = sample_rate_32;
	else VWAR("unknown simple rate : %d", sb);

	if (ch == 2)pinfo->channel = channel_stereo;
	else if (ch == 1)pinfo->channel = channel_mono_r;
	else VWAR("unknown channel : %d", ch);
}
