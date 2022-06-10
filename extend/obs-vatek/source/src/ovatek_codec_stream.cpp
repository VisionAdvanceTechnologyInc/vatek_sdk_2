#include <codec/ovatek_codec.h>

vatek_result ovatekCodecStream::createStream(qtvCodec* codec, Pusbmux_stream pustream, ovatekCodecStream** ovstream)
{
	ovatekFrameHandle* newframe = NULL;
	vatek_result nres = ovatekFrameHandle::createFrameHandle(codec, &newframe);
	if (is_vatek_success(nres))
	{
		int32_t buflen = OSTREAM_VIDEO_BUFFER_LEN;
		if (codec->_mode() == qcodec_audio)
			buflen = OSTREAM_AUDIO_BUFFER_LEN;
		*ovstream = new ovatekCodecStream(buflen, codec, newframe, pustream);
		nres = vatek_success;
	}
	return nres;
}

ovatekCodecStream::ovatekCodecStream(int32_t memsize, qtvCodec* codec, ovatekFrameHandle* frame, Pusbmux_stream ustream) :
	m_codec(codec),
	m_frame(frame),
	m_bufquery(th_memquery_create(memsize)),
	m_ustream(ustream),
	m_tickms(0)
{
	if (!is_vatek_success(cross_os_create_mutex(&m_lock)))
		VWAR("create mutex fail");
	vatek_usbmux_stream_set_private(ustream, this);
}

ovatekCodecStream::~ovatekCodecStream()
{
	if (m_bufquery)th_memquery_destory(m_bufquery);
	if (m_lock)cross_os_free_mutex(m_lock);
	m_bufquery = NULL;
	m_lock = NULL;
}

vatek_result ovatekCodecStream::createPesFrame(PovatekFrame* frame, encoder_packet* encpkt)
{
	int32_t len = sizeof(ovatekFrame) + encpkt->size;
	vatek_result nres = vatek_memfail;
	uint8_t* rawbuf = NULL;
	
	cross_os_lock_mutex(m_lock);
		rawbuf = th_memquery_malloc(m_bufquery, len);
	cross_os_release_mutex(m_lock);

	if (rawbuf)
	{
		PovatekFrame newframe = (PovatekFrame)&rawbuf[0];
		memset(newframe, 0, sizeof(ovatekFrame));
		newframe->rawbuf = rawbuf;
		if (encpkt->pts < 0)newframe->pes_frame.pts = 0;
		else if (encpkt->timebase_den == MEDIA_DEFAULT_CLOCK)
			newframe->pes_frame.pts = encpkt->pts;
		else newframe->pes_frame.pts = (encpkt->pts * MEDIA_DEFAULT_CLOCK) / encpkt->timebase_den;

		if (encpkt->dts < 0)newframe->pes_frame.dts = 0;
		else if (encpkt->timebase_den == MEDIA_DEFAULT_CLOCK)
			newframe->pes_frame.dts = encpkt->dts;
		else newframe->pes_frame.dts = (encpkt->dts * MEDIA_DEFAULT_CLOCK) / encpkt->timebase_den;

		newframe->pes_frame.lenbuf = encpkt->size;
		newframe->pes_frame.ptrbuf = &rawbuf[sizeof(ovatekFrame)];
		memcpy(&newframe->pes_frame.ptrbuf[0], &encpkt->data[0] , encpkt->size);
		newframe->pes_frame.private_data = newframe;
		newframe->stream = this;

		nres = m_frame->getHeader(newframe, encpkt);
		if (is_vatek_success(nres))
		{
			uint32_t tickms = newframe->pes_frame.dts / 90;
			if (tickms > m_tickms)m_tickms = tickms;
			cross_os_lock_mutex(m_lock);
				th_memquery_push(m_bufquery, rawbuf);
			cross_os_release_mutex(m_lock);
			*frame = newframe;
			VDBG("%04x:%d", m_ustream->pid, m_tickms);
			nres = vatek_success;
		}
	}
	return nres;
}


vatek_result ovatekCodecStream::freePesFrame(PovatekFrame frame)
{
	vatek_result nres = m_frame->freeHeader(frame);
	if (is_vatek_success(nres))
	{
		cross_os_lock_mutex(m_lock);
			nres = th_memquery_free(m_bufquery, frame->rawbuf);
		cross_os_release_mutex(m_lock);
	}
		
	return nres;
}
