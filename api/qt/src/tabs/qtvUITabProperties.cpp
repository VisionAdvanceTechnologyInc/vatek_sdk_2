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

#include <ui/base/qtv_ui_tab.h>
#include <ui/qtv_ui_service_usb.h>

#include "ui_qtvUITabProperties.h"

qtvUITabProperties::qtvUITabProperties(qtvUIMain* main, qtv_ui_tab_properties_mode mode, QWidget* parent) :
	m_mode(mode),
	m_codec(NULL),
	qtvUITabBase(main, getPropertiesName(mode), getPropertiesDesc(mode), parent),
	ui(new Ui::qtvUITabProperties)
{
	ui->setupUi(this);
	connect(ui->cb_codecs, SIGNAL(currentIndexChanged(int)), this, SLOT(recvCodecSelectChanged(int)));
	refreshTitle();
	refreshProperties(qui_table_normal, NULL);
}

qtvUITabProperties::qtvUITabProperties(qtvUIMain* main, const char* name, const char* desc, QWidget* parent) :
	m_mode(qui_tab_properties_null),
	m_codec(NULL),
	qtvUITabBase(main,name,desc,parent),
	ui(new Ui::qtvUITabProperties)
{
	ui->setupUi(this);
	connect(ui->cb_codecs, SIGNAL(currentIndexChanged(int)), this, SLOT(recvCodecSelectChanged(int)));
	refreshTitle();
	refreshProperties(qui_table_normal, NULL);
}

qtvUITabProperties::qtvUITabProperties(qtvUIMain* main, QString& name, QString& desc, QWidget* parent) :
	m_mode(qui_tab_properties_null),
	m_codec(NULL),
	qtvUITabBase(main, name, desc, parent),
	ui(new Ui::qtvUITabProperties)
{
	ui->setupUi(this);
	connect(ui->cb_codecs, SIGNAL(currentIndexChanged(int)), this, SLOT(recvCodecSelectChanged(int)));
	refreshTitle();
	refreshProperties(qui_table_normal, NULL);
}

qtvUITabProperties::~qtvUITabProperties()
{
	disconnect(ui->cb_codecs, SIGNAL(currentIndexChanged(int)), this, SLOT(recvCodecSelectChanged(int)));
	if (m_codec)delete m_codec;
	delete ui;
	m_codec = NULL;
}

void qtvUITabProperties::tabEnter()
{
	if (m_main)
	{
		if (m_mode == qui_tab_properties_modulation)
		{
			qtvServiceBase* service = qobject_cast<qtvServiceBase*>(m_main->_service());
			if (service)
				refreshProperties(qui_table_normal, service->_output_properties());
		}
		else if (m_mode == qui_tab_properties_muxparam)
		{
			qtvServiceUSBMuxer* usbmuxer = qobject_cast<qtvServiceUSBMuxer*>(m_main->_service());
			if (usbmuxer)
				refreshProperties(qui_table_normal, usbmuxer->_mux_properties());
		}
		else if (m_mode == qui_tab_properties_videocodec ||
				 m_mode == qui_tab_properties_audiocodec)
		{
			qtvUIMainUSBMuxer* umuxmain = qobject_cast<qtvUIMainUSBMuxer*>(m_main);
			if (umuxmain)
			{
				if (m_mode == qui_tab_properties_videocodec)
					refreshCodec(umuxmain->getVideoProvider());
				else refreshCodec(umuxmain->getAudioProvider());
			}
		}
	}
}

void qtvUITabProperties::tabFinish()
{
	refreshProperties(qui_table_normal, NULL);
	if (m_mode == qui_tab_properties_videocodec ||
		m_mode == qui_tab_properties_audiocodec)
	{
		qtvUIMainUSBMuxer* umuxmain = qobject_cast<qtvUIMainUSBMuxer*>(m_main);
		if (umuxmain)
		{
			if (m_mode == qui_tab_properties_videocodec)umuxmain->attachVideoCodec(qobject_cast<qtvCodecVideo*>(m_codec));
			else umuxmain->attachAudioCodec(qobject_cast<qtvCodecAudio*>(m_codec));
		}
		m_codec = NULL;
	}
}

void qtvUITabProperties::tabLeave()
{
	refreshProperties(qui_table_normal, NULL);
	if (m_codec)delete m_codec;
	m_codec = NULL;
}

void qtvUITabProperties::refreshProperties(qui_table_mode mode, qtvPropertyCollection* properties)
{
	ui->table_properties->setProperties(mode, properties);
}

void qtvUITabProperties::refreshTitle()
{
	ui->lb_desc->setText(m_desc);
	ui->lb_title->setText(m_name);
	if (qtvUIMain::is_uimain_tabs(m_main->_mode()))
	{
		ui->lb_desc->setVisible(true);
		ui->lb_title->setVisible(true);
	}
	else
	{
		ui->lb_desc->setVisible(false);
		ui->lb_title->setVisible(false);
	}

	if (m_mode == qui_tab_properties_videocodec ||
		m_mode == qui_tab_properties_audiocodec)
	{
		ui->lb_codec->setVisible(true);
		ui->cb_codecs->setVisible(true);
		if (m_mode == qui_tab_properties_videocodec)
			ui->lb_codec->setText("video codec : ");
		else ui->lb_codec->setText("audio codec : ");
	}
	else
	{
		ui->lb_codec->setVisible(false);
		ui->cb_codecs->setVisible(false);
	}
}

void qtvUITabProperties::refreshCodec(qtvCodecProvider* provider)
{
	const QList<qtvCodecFactory*> codecs = provider->getCodecs();
	int32_t i = 0;
	ui->cb_codecs->clear();

	for (i = 0; i < codecs.count(); i++)
	{
		ui->cb_codecs->addItem(codecs[i]->_name(), QVariant::fromValue<qtvCodecFactory*>(codecs[i]));
	}

	if (codecs.count())
	{
		ui->cb_codecs->setCurrentIndex(0);
		recvCodecSelectChanged(0);
	}
		
}

void qtvUITabProperties::recvCodecSelectChanged(int index)
{
	int32_t idx = ui->cb_codecs->currentIndex();
	if (m_codec)
	{
		refreshProperties(qui_table_normal, NULL);
		delete m_codec;
	}
	m_codec = NULL;

	if (idx != -1)
	{
		qtvCodecFactory* factory = qvariant_cast<qtvCodecFactory*>(ui->cb_codecs->itemData(idx));
		if (factory)
		{
			vatek_result nres = factory->createCodec(&m_codec);
			if (is_vatek_success(nres))
				refreshProperties(qui_table_normal, m_codec->_properties());
		}
	}
}

const char* qtvUITabProperties::getPropertiesName(qtv_ui_tab_properties_mode mode)
{
	if (mode == qui_tab_properties_modulation)
		return "Modulation";
	else if (mode == qui_tab_properties_muxparam)
		return "Muxer";
	else if (mode == qui_tab_properties_videocodec)
		return "Video Encoder";
	else if (mode == qui_tab_properties_audiocodec)
		return "Audio Encoder";
	return "unknown";
}

const char* qtvUITabProperties::getPropertiesDesc(qtv_ui_tab_properties_mode mode)
{
	if (mode == qui_tab_properties_modulation)
		return "output modulation properties config";
	else if (mode == qui_tab_properties_muxparam)
		return "mpeg ts muxer properties config";
	else if (mode == qui_tab_properties_videocodec)
		return "default video encoder select and properties config";
	else if (mode == qui_tab_properties_audiocodec)
		return "default audio encoder select and properties config";
	return "unknown";
}