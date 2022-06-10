#include <codec/ovatek_codec.h>
#include <codec/ffmpeg/ovatek_ffmpeg.h>

const uint8_t h264_nalu_aud[] = { 0x00,0x00,0x00,0x01,0x09,0x05, };
#define H264_NALU_AUD_LEN			(sizeof(h264_nalu_aud))
#define H264_NALU_AUD_BUFFER_LEN	(((H264_NALU_AUD_LEN + 3) >> 2) << 2)

#define AAC_ADTS_HDADER_LEN			7
#define AAC_ADTS_MAX_FRAME_BYTES	((1 << 13) - 1)
#define AAC_ADTS_MEMQUERY_SIZE		1024
#define AAC_ADTS_HDADER_BUFFER_LEN	(((AAC_ADTS_HDADER_LEN + 3) >> 2) << 2)

typedef enum _framehdr_mode
{
	hdrmode_bypass,
	hdrmode_h264_ffmpeg,
}framehdr_mode;

class frameHandle_h264 : public ovatekFrameHandle
{
public:
	frameHandle_h264(framehdr_mode mode);
	~frameHandle_h264();

	vatek_result getHeader(PovatekFrame pframe, encoder_packet* encpkt) override;
	vatek_result freeHeader(PovatekFrame pframe) override;

protected:
	framehdr_mode m_hdrmode;
	int32_t m_hdrlen;
	uint8_t* m_hdraud;
	uint8_t* m_hdrbuf;
};


class frameHandle_aac : public ovatekFrameHandle
{
public:
	frameHandle_aac();
	~frameHandle_aac();

	vatek_result getHeader(PovatekFrame pframe, encoder_packet* encpkt) override;
	vatek_result freeHeader(PovatekFrame pframe) override;

protected:
	uint8_t* m_hdrbuf;
	Pth_memquery m_memquery;
};

class frameHandle_bypass : public ovatekFrameHandle
{
public:
	frameHandle_bypass();
	~frameHandle_bypass();

	vatek_result getHeader(PovatekFrame pframe, encoder_packet* encpkt) override;
	vatek_result freeHeader(PovatekFrame pframe) override;
};

/* frameHandle_h264 */

frameHandle_h264::frameHandle_h264(framehdr_mode mode) :
	m_hdrmode(mode),
	m_hdrbuf(NULL),
	m_hdraud(NULL),
	m_hdrlen(0)
{

}

frameHandle_h264::~frameHandle_h264()
{
	if (m_hdrbuf)delete m_hdrbuf;
	if (m_hdraud)delete m_hdraud;
	m_hdraud = NULL;
	m_hdrbuf = NULL;
	m_hdrlen = 0;
}

vatek_result frameHandle_h264::getHeader(PovatekFrame pframe, encoder_packet* encpkt)
{
	vatek_result nres = vatek_badparam;
	if (m_hdrmode == hdrmode_bypass)nres = vatek_success;
	else if(m_hdrmode == hdrmode_h264_ffmpeg)
	{
		if (encpkt->keyframe)
		{
			if (!m_hdrbuf)
			{
				size_t hdrlen = 0;
				uint8_t* hdrptr = NULL;
				if (obs_encoder_get_extra_data(encpkt->encoder, &hdrptr, &hdrlen) && hdrlen)
				{
					int32_t len = ((hdrlen + H264_NALU_AUD_LEN + 3) >> 2) << 2;
					m_hdrbuf = (uint8_t*)malloc(len);
					if (m_hdrbuf)
					{
						memset(&m_hdrbuf[0], 0, len);
						memcpy(&m_hdrbuf[0], &h264_nalu_aud[0], H264_NALU_AUD_LEN);
						memcpy(&m_hdrbuf[H264_NALU_AUD_LEN], &hdrptr[0], hdrlen);
						m_hdrlen = H264_NALU_AUD_LEN + hdrlen;
					}
					else nres = vatek_memfail;
				}
			}

			if (m_hdrbuf)
			{
				pframe->pes_frame.lenhdr = m_hdrlen;
				pframe->pes_frame.ptrhdr = m_hdrbuf;
				nres = vatek_success;
			}
		}
		else
		{
			if (!m_hdraud)
			{
				m_hdraud = (uint8_t*)malloc(H264_NALU_AUD_BUFFER_LEN);
				if (m_hdraud)
				{
					memset(&m_hdraud[0], 0, H264_NALU_AUD_BUFFER_LEN);
					memcpy(&m_hdraud[0], &h264_nalu_aud[0], H264_NALU_AUD_LEN);
				}
			}
			else nres = vatek_memfail;

			if (m_hdraud)
			{
				pframe->pes_frame.lenhdr = H264_NALU_AUD_LEN;
				pframe->pes_frame.ptrhdr = m_hdraud;
				nres = vatek_success;
			}
		}
	}
	return nres;
}

vatek_result frameHandle_h264::freeHeader(PovatekFrame pframe)
{
	return vatek_success;
}

/* frameHandle_bypass */

frameHandle_bypass::frameHandle_bypass()
{


}

frameHandle_bypass::~frameHandle_bypass()
{

}

vatek_result frameHandle_bypass::getHeader(PovatekFrame pframe, encoder_packet* encpkt)
{
	return vatek_success;
}

vatek_result frameHandle_bypass::freeHeader(PovatekFrame pframe)
{
	return vatek_success;
}

/* frameHandle_aac */

frameHandle_aac::frameHandle_aac() :
	m_hdrbuf(NULL),
	m_memquery(th_memquery_create(AAC_ADTS_MEMQUERY_SIZE))
{

}

frameHandle_aac::~frameHandle_aac()
{
	if (m_hdrbuf)free(m_hdrbuf);
	if (m_memquery)th_memquery_destory(m_memquery);
	m_hdrbuf = NULL;
	m_memquery = NULL;
}

vatek_result frameHandle_aac::getHeader(PovatekFrame pframe, encoder_packet* encpkt)
{
	vatek_result nres = vatek_success;
	
	if (!m_hdrbuf)
	{
		size_t hdrlen = 0;
		uint8_t* hdrptr = NULL;
		if (obs_encoder_get_extra_data(encpkt->encoder, &hdrptr, &hdrlen) && hdrlen)
		{
			th_bitstream bsaac;
			th_bs_config(&bsaac, hdrptr, hdrlen);
			uint8_t objtype = th_bs_read(&bsaac, 5);
			uint8_t samplerate = th_bs_read(&bsaac, 4);
			uint8_t channel = th_bs_read(&bsaac, 4);
			m_hdrbuf = (uint8_t*)malloc(AAC_ADTS_HDADER_BUFFER_LEN);
			if (m_hdrbuf)
			{
				m_hdrbuf[0] = 0xFF;
				m_hdrbuf[1] = 0xF1;
				m_hdrbuf[2] = (uint8_t)(((objtype - 1) & 0x03) << 6) | ((samplerate & 0x0f) << 2) | ((channel & 0x07) >> 2);;
				m_hdrbuf[3] = (channel & 0x07) << 6;
				m_hdrbuf[4] = 0;
				m_hdrbuf[5] = (0x7ff >> 6) & 0x1f;
				m_hdrbuf[6] = (0x7ff & 0x3f) << 2;
			}
			else nres = vatek_memfail;
		}
	}

	if (is_vatek_success(nres) && m_hdrbuf)
	{
		uint8_t* newhdr = th_memquery_malloc(m_memquery, AAC_ADTS_HDADER_BUFFER_LEN);
		if (newhdr)
		{
			int32_t len = (int32_t)(encpkt->size + AAC_ADTS_HDADER_LEN);
			memcpy(&newhdr[0], &m_hdrbuf[0], AAC_ADTS_HDADER_BUFFER_LEN);
			newhdr[3] = (uint8_t)(newhdr[3] & 0xFC) | ((len >> 11) & 3);
			newhdr[4] = (uint8_t)((len >> 3) & 0xFF);
			newhdr[5] = (len << 5) | 0x1F;
			pframe->pes_frame.lenhdr = AAC_ADTS_HDADER_LEN;
			pframe->pes_frame.ptrhdr = newhdr;
			th_memquery_push(m_memquery, newhdr);
		}
		else nres = vatek_memfail;
	}

	return nres;
}

vatek_result frameHandle_aac::freeHeader(PovatekFrame pframe)
{
	vatek_result nres = vatek_success;
	if (pframe->pes_frame.ptrhdr)
		nres = th_memquery_free(m_memquery, pframe->pes_frame.ptrhdr);
	return nres;
}

/* ovatekFrameHandle */

vatek_result ovatekFrameHandle::createFrameHandle(qtvCodec* codec, ovatekFrameHandle** frame)
{
	vatek_result nres = vatek_success;
	if (codec->_name().compare(CODEC_NAME_INTELQSV) == 0)*frame = new frameHandle_h264(hdrmode_bypass);
	else if (codec->_name().compare(CODEC_NAME_FFMPEGNVENC) == 0)*frame = new frameHandle_h264(hdrmode_h264_ffmpeg);
	else if (codec->_name().compare(CODEC_NAME_AAC) == 0)*frame = new frameHandle_aac();
	else if (codec->_name().compare(OVATEK_FFMPEG_MPEG2VIDEO) == 0)*frame = new frameHandle_bypass();
	else if (codec->_name().compare(OVATEK_FFMPEG_MP2) == 0)*frame = new frameHandle_bypass();
	else if (codec->_name().compare(OVATEK_FFMPEG_A52) == 0)*frame = new frameHandle_bypass();
	else
	{
		VWAR("unsuppor frame handle : %s", codec->_name().toStdString().c_str());
		*frame = new frameHandle_bypass();
		//nres = vatek_unsupport;
	}
	return nres;
}
