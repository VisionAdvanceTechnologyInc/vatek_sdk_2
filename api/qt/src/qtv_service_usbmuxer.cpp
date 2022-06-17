//----------------------------------------------------------------------------
//
// Vision Advance Technology - Software Development Kit
// Copyright (c) 2014-2022, Vision Advance Technology Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//

#include "./internal/qinternal_usbmuxer.h"
#include "./internal/qinternal_core.h"
#include <ui/base/qtv_ui_define.h>

class usbmuxer_handle : public qi_servicehandle
{
public:
	usbmuxer_handle(hvatek_chip hchip,hvatek_usbmux hmux, qtvChartDataCaptureService* chartdata);
	~usbmuxer_handle();

protected:
	Ptransform_info m_info;
	hvatek_usbmux m_husbmux;
	qtvChartDataCaptureService* m_chartdata;

	vatek_result startService() override;
	vatek_result pollingService() override;
	void updateServiceTick() override;
	void stopService() override;
	Pbroadcast_info getServiceInfo() override;
};

/* qi_usbmuxer_stream */

qi_usbmuxer_stream::qi_usbmuxer_stream(hmux_stream hstream, qtvUSBMuxerProgram* program, qtvCodec* codec) :
	m_hstream(hstream),
	m_codec(codec),
	m_rawstream(muxstream_get(hstream)),
	m_program(program),
	m_properties(qtvPropertyCollection::createProperties(_ui_struct(qusbmuxer_stream_param),(uint8_t*)&m_param))
{
	if (m_rawstream->type == mux_stream_video)
		m_name = QString::asprintf("stream [%s]", ui_enum_get_str(video_codec, m_rawstream->_stream_info.video.vcodec));
	else if (m_rawstream->type == mux_stream_audio)
		m_name = QString::asprintf("stream [%s]", ui_enum_get_str(audio_codec, m_rawstream->_stream_info.audio.acodec));
	else m_name = QString::asprintf("stream [data-%02x]", m_rawstream->stream_type);
	memset(&m_param, 0, sizeof(qusbmuxer_stream_param));
	m_param.pid = m_rawstream->stream_pid;
}

qi_usbmuxer_stream::~qi_usbmuxer_stream()
{
	if (m_properties)delete m_properties;
	m_properties = NULL;
}

vatek_result qi_usbmuxer_stream::createStream(uint16_t pid, qi_usbmuxer_program* program, qtvCodec* codec, qtvUSBMuxerStream** stream)
{
	vatek_result nres = vatek_unsupport;
	hmux_stream hstream = NULL;

	if (codec->_mode() == qcodec_video)
	{
		qtvCodecVideo* qvideo = qobject_cast<qtvCodecVideo*>(codec);
		media_video video;
		memset(&video, 0, sizeof(media_video));
		video.framerate = framerate_unknown;
		video.resolution = resolution_unknown;
		video.aspectrate = aspectrate_unknown;
		video.vcodec = qvideo->_codec_video_mode();
		nres = muxprogram_add_video(program->_handle(), pid, &video, &hstream);
	}
	else if (codec->_mode() == qcodec_audio)
	{
		qtvCodecAudio* qaudio = qobject_cast<qtvCodecAudio*>(codec);
		media_audio audio;
		memset(&audio, 0, sizeof(media_audio));
		audio.acodec = qaudio->_codec_audio_mode();
		audio.channel = channel_unknow;
		audio.samplerate = sample_rate_unknown;
		nres = muxprogram_add_audio(program->_handle(), pid, &audio, &hstream);
	}
	
	if (is_vatek_success(nres))
		*stream = new qi_usbmuxer_stream(hstream, program, codec);
	return nres;
}

/* qi_usbmuxer_program */

qi_usbmuxer_program::qi_usbmuxer_program(hmux_program hprogram, qtvUSBMuxerChannel* channel) :
	m_hprogram(hprogram),
	m_channel(channel),
	m_properties(NULL),
	m_ptrstream(NULL)
{
	Pmux_program program = muxprogram_get(hprogram);
	m_name = QString::asprintf("program");
	memset(&m_param, 0, sizeof(qusbmuxer_program_param));
	m_param.pmt_pid = program->pmt_pid;
	m_properties = new qusbmuxer_program_properties(this);
}

qi_usbmuxer_program::~qi_usbmuxer_program()
{
	clear();
	if (m_properties)delete m_properties;
	m_properties = NULL;
}

void qi_usbmuxer_program::clear()
{
	if (m_ptrstream)delete m_ptrstream;
	while (m_streams.count())
	{
		delete m_streams.first();
		m_streams.removeFirst();
	}
	m_ptrstream = NULL;
}

vatek_result qi_usbmuxer_program::createStream(uint16_t pid, qtvCodec* codec, qtvUSBMuxerStream** stream)
{
	vatek_result nres = vatek_badstatus;
	if (!m_ptrstream)
	{
		nres = qi_usbmuxer_stream::createStream(pid, this, codec, &m_ptrstream);
		if (is_vatek_success(nres))*stream = m_ptrstream;
	}
	return nres;
}

vatek_result qi_usbmuxer_program::getStream(uint16_t pid, qtvUSBMuxerStream** stream)
{
	int32_t i = 0;
	for (i = 0; i < m_streams.count(); i++)
	{
		qtvMuxStream* ptrstream = m_streams.at(i);
		if (ptrstream->_pid() == pid)
		{
			*stream = qobject_cast<qtvUSBMuxerStream*>(ptrstream);
			return vatek_success;
		}
	}
	return vatek_badparam;
}

vatek_result qi_usbmuxer_program::commitStream(qtvUSBMuxerStream* stream)
{
	vatek_result nres = vatek_badstatus;
	if (m_ptrstream && m_ptrstream == stream)
	{
		m_streams.append(m_ptrstream);
		m_ptrstream = NULL;
		nres = vatek_success;
	}
	return nres;
}

/* qusbmuxer_program_properties */

qusbmuxer_program_properties::qusbmuxer_program_properties(qi_usbmuxer_program* uprogram) :
	m_uprogram(uprogram),
	qi_properties_program(uprogram, uprogram->_handle())
{
	refreshProperties();
}

void qusbmuxer_program_properties::refreshProperties()
{
	qi_properties_program::refreshProperties();
	qi_properties_program::insertProperty(_ui_struct(qusbmuxer_program_param), (uint8_t*)m_uprogram->_param());
}

/* qi_usbmuxer_channel */

qi_usbmuxer_channel::qi_usbmuxer_channel(hmux_channel hch, uint16_t pcrpid) :
	m_hchannel(hch),
	m_pcrpid(pcrpid),
	m_properties(NULL),
	m_ptrprogram(NULL)
{
	m_name = QString::asprintf("channel");
	m_properties = new qi_properties_channal(this, _handle());
}

qi_usbmuxer_channel::~qi_usbmuxer_channel()
{
	clear();
	if (m_properties)delete m_properties;
	m_properties = NULL;
}

void qi_usbmuxer_channel::clear()
{
	if (m_ptrprogram)delete m_ptrprogram;
	while (m_programs.count())
	{
		delete m_programs.first();
		m_programs.removeFirst();
	}
	m_ptrprogram = NULL;
}

vatek_result qi_usbmuxer_channel::createProgram(uint16_t pmtpid, qtvUSBMuxerProgram** program)
{
	vatek_result nres = vatek_badstatus;
	if (!m_ptrprogram)
	{
		qtvUSBMuxerProgram* ptrprog = NULL;
		nres = getProgram(pmtpid, &ptrprog);
		if (is_vatek_success(nres))nres = vatek_badparam;
		else
		{
			hmux_program hprogram = NULL;
			nres = muxchannel_add_program(m_hchannel, pmtpid, m_pcrpid, &hprogram);
			if (is_vatek_success(nres))
			{
				m_ptrprogram = new qi_usbmuxer_program(hprogram, this);
				*program = m_ptrprogram;
				nres = vatek_success;
			}
		}
	}
	return nres;
}

vatek_result qi_usbmuxer_channel::getProgram(uint16_t pmtpid, qtvUSBMuxerProgram** program)
{
	int32_t i = 0;
	for (i = 0; i < m_programs.count(); i++)
	{
		qtvMuxProgram* ptrprog = m_programs.at(i);
		if (ptrprog->_pmt_pid() == pmtpid)
		{
			*program = qobject_cast<qtvUSBMuxerProgram*>(ptrprog);
			if (*program)return vatek_success;
			else return vatek_unknown;
		}
	}
	return vatek_badparam;
}

vatek_result qi_usbmuxer_channel::commitProgram(qtvUSBMuxerProgram* program)
{
	vatek_result nres = vatek_badstatus;
	if (m_ptrprogram && m_ptrprogram == program)
	{
		m_programs.append(m_ptrprogram);
		m_ptrprogram = NULL;
		nres = vatek_success;
	}
	return nres;
}

/* qi_usbmuxer_service */

qi_usbmuxer_service::qi_usbmuxer_service(qtvDevice* device, hmux_core hmux) :
	m_hmux(hmux),
	m_output(device),
	m_channel(NULL),
	m_husbmux(NULL),
	m_muxproperties(_ui_struct(qtv_usbmuxer_param),(uint8_t*)&m_muxparam),
	qtvServiceUSBMuxer(qservice_usbmuxer,device)
{
	resetMuxProperties();
}

qi_usbmuxer_service::~qi_usbmuxer_service()
{
	stopBroadcast();
	resetChannel();
	mux_handle_free(m_hmux);
	m_hmux = NULL;
}

void qi_usbmuxer_service::resetChannel()
{
	if (m_channel)delete m_channel;
	mux_handle_reset(m_hmux);
	m_channel = NULL;
	changedStatus(qstatus_idle);
}

void qi_usbmuxer_service::resetOutput(uint32_t freqkhz,uint32_t bw_sb, modulator_type type)
{
	m_output.resetParam(type, bw_sb, freqkhz);
}

vatek_result qi_usbmuxer_service::createChannel(mux_spec_mode spec, mux_country_code country, qtvUSBMuxerChannel** channel)
{
	vatek_result nres = vatek_badstatus;
	if (!m_channel)
	{
		hmux_channel hchannel = NULL;
		nres = mux_open_channel_default(m_hmux, spec, country, &hchannel);
		if (is_vatek_success(nres))
		{
			m_channel = new qi_usbmuxer_channel(hchannel, 0x100);
			*channel = m_channel;
		}
	}
	return nres;
}

vatek_result qi_usbmuxer_service::getChannel(qtvUSBMuxerChannel** channel)
{
	vatek_result nres = vatek_badstatus;
	if (m_channel)
	{
		*channel = m_channel;
		nres = vatek_success;
	}
	return nres;
}

vatek_result qi_usbmuxer_service::commitChannel(qtvUSBMuxerChannel* channel)
{
	vatek_result nres = vatek_badparam;
	if (m_channel && channel == m_channel)
	{
		if (channel->_programs().count())
		{
			changedStatus(qstatus_ready);
			nres = vatek_success;
		}
	}
	return nres;
}

void qi_usbmuxer_service::resetMuxProperties()
{
	memset(&m_muxparam, 0, sizeof(qtv_usbmuxer_param));
	m_muxparam.pcr_pid = 0x100;
	m_muxparam.pcr_interval = 40;
	m_muxparam.latency_ms = 100;
}

vatek_result qi_usbmuxer_service::startBroadcast(qtvUSBMuxSource* source)
{
	vatek_result nres = vatek_badstatus;
	if (m_status == qstatus_ready)
	{
		hmux_channel hch = m_channel->_handle();
		nres = vatek_usbmux_create(m_device->_handle(), getUSBMuxParam(), &m_husbmux);
		if (is_vatek_success(nres))
		{
			hmux_channel hch = m_channel->_handle();
			Pspec_channel pch = NULL;
			nres = muxchannel_get_param(hch, &pch);
			if (is_vatek_success(nres))
			{
				nres = vatek_usbmux_open_spec(m_husbmux,pch->spec, pch->country);
				if (is_vatek_success(nres))
					nres = vatek_usbmux_replace_param(m_husbmux, pch);
			}

			if (is_vatek_success(nres))
			{
				int32_t i = 0;
				for (i = 0; i < m_channel->_programs().count(); i++)
				{
					Pusbmux_program puprog = NULL;
					hqtv_program hprogram = NULL;
					qi_usbmuxer_program* quprogram = static_cast<qi_usbmuxer_program*>(m_channel->_programs()[i]);
					nres = vatek_usbmux_add_program(m_husbmux, quprogram->_param()->pmt_pid, &puprog);
					if (is_vatek_success(nres))
					{
						nres = vatek_usbmux_program_replace_param(puprog, muxprogram_get_param(quprogram->_handle()));
						if (is_vatek_success(nres))
							nres = source->startProgram(quprogram, puprog, &hprogram);
					}
						
					if (is_vatek_success(nres))
					{
						int32_t j = 0;
						for (j = 0; j < quprogram->_streams().count(); j++)
						{
							Pusbmux_stream pstream = NULL;
							qi_usbmuxer_stream* qstream = static_cast<qi_usbmuxer_stream*>(quprogram->_streams()[j]);
							Pmux_stream praw = qstream->_raw_stream();
							if (praw->type == mux_stream_video)
								nres = vatek_usbmux_add_video_stream(puprog, qstream->_param()->pid, &praw->_stream_info.video, &pstream);
							else if (praw->type == mux_stream_audio)
								nres = vatek_usbmux_add_audio_stream(puprog, qstream->_param()->pid, &praw->_stream_info.audio, &pstream);
							else nres = vatek_usbmux_add_stream(puprog, qstream->_pid(), praw->stream_type, &pstream);
							if (!is_vatek_success(nres))break;
							else nres = source->pushStream(hprogram, qstream, pstream);
						}

						if (is_vatek_success(nres))source->stopProgram(hprogram);
					}

					if (!is_vatek_success(nres))break;
				}
			}

			if (is_vatek_success(nres))
			{
				nres = vatek_usbmux_finish(m_husbmux);
				if (is_vatek_success(nres))
					nres = vatek_usbmux_start(m_husbmux, source->getHandle());
				if (is_vatek_success(nres))
					nres = m_chartdata->startCapture();
				if (is_vatek_success(nres))
					nres = startHandle(new usbmuxer_handle(m_device->_handle(),m_husbmux, m_chartdata));
			}
			
			if (!is_vatek_success(nres))
			{
				vatek_usbmux_close(m_husbmux);
				m_husbmux = NULL;
			}
		}
	}
	return nres;
}

vatek_result qi_usbmuxer_service::sendFrame(Pusbmux_stream hstream, Pmux_pes_frame pframe)
{
	vatek_result nres = vatek_badstatus;
	if (m_status == qstatus_running)
		nres = vatek_usbmux_push_frame(m_husbmux, hstream, pframe);
	return nres;
}

vatek_result qi_usbmuxer_service::stopBroadcast()
{
	vatek_result nres = vatek_badstatus;
	if (m_status == qstatus_running)
	{
		vatek_usbmux_stop(m_husbmux);
		stopHandle();
		m_chartdata->stopCapture();
		changedStatus(qstatus_ready);
		vatek_usbmux_close(m_husbmux);
		m_husbmux = NULL;
		nres = vatek_success;
	}
	return nres;
}

Pusbmux_param qi_usbmuxer_service::getUSBMuxParam()
{
	Pqtv_output_param poutput = m_output._param();
	memset(&m_usbmuxparam, 0, sizeof(usbmux_param));
	m_usbmuxparam.pcr_pid = m_muxparam.pcr_pid;
	m_usbmuxparam.interval_ms = m_muxparam.pcr_interval;
	m_usbmuxparam.latency_ms = m_muxparam.latency_ms;
	m_usbmuxparam.bitrate = 0;
	m_usbmuxparam.r2param.freqkhz = poutput->r2param.freqkhz;
	m_usbmuxparam.r2param.mode = poutput->r2param.mode;

	memcpy(&m_usbmuxparam.modulator, &poutput->modulation, sizeof(modulator_param));
	return &m_usbmuxparam;
}

/* qtvServiceUSBMuxer */

vatek_result qtvServiceUSBMuxer::createService(qtvDevice* device, qtvServiceUSBMuxer** service)
{
	hmux_core hmux = NULL;
	vatek_result nres = mux_handle_create(&hmux);
	if (is_vatek_success(nres))
		*service = new qi_usbmuxer_service(device, hmux);
	return nres;
}

usbmuxer_handle::usbmuxer_handle(hvatek_chip hchip,hvatek_usbmux hmux, qtvChartDataCaptureService* chartdata) :
	qi_servicehandle(hchip),
	m_info(NULL),
	m_husbmux(hmux),
	m_chartdata(chartdata)
{

}

usbmuxer_handle::~usbmuxer_handle()
{

}

vatek_result usbmuxer_handle::startService()
{
	return vatek_success;
}

vatek_result usbmuxer_handle::pollingService()
{
	vatek_result nres = vatek_success;
	usbmux_status ustatus = vatek_usbmux_get_status(m_husbmux, &m_info);

	if (ustatus < umux_status_prepare)
		nres = vatek_badstatus;

	return nres;
}

void usbmuxer_handle::updateServiceTick()
{
	m_chartdata->pushBroadcast(vatek_usbmux_get_info(m_husbmux));
}

void usbmuxer_handle::stopService()
{
	m_info = NULL;
}

Pbroadcast_info usbmuxer_handle::getServiceInfo()
{
	if (m_info)return &m_info->info;
	return NULL;
}