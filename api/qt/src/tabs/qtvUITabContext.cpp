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
#include <ui/base/qtv_ui_tree.h>
#include <ui/qtv_ui_service_base.h>
#include <ui/qtv_ui_service_usb.h>
#include "ui_qtvUITabContext.h"

#define UITAB_CONTEXT_NAME	"Context"
#define UITAB_CONTEXT_DESC	"current broadcast context information and properties"

qtvUITabContext::qtvUITabContext(qtvUIMain* main, QWidget* parent) :
	qtvUITabBase(main, UITAB_CONTEXT_NAME, UITAB_CONTEXT_DESC, parent),
	ui(new Ui::qtvUITabContext)
{
	initConnect();
}

qtvUITabContext::qtvUITabContext(qtvUIMain* main, const char* name, const char* desc, QWidget* parent) :
	qtvUITabBase(main, name, desc, parent),
	ui(new Ui::qtvUITabContext)
{
	initConnect();
}

qtvUITabContext::qtvUITabContext(qtvUIMain* main, QString& name, QString& desc, QWidget* parent) :
	qtvUITabBase(main, name, desc, parent),
	ui(new Ui::qtvUITabContext)
{
	initConnect();
}

qtvUITabContext::~qtvUITabContext()
{
	disconnect(ui->tr_channel, SIGNAL(itemSelectionChanged()), this, SLOT(recvContextSelectChanged()));
	disconnect(ui->li_stream, SIGNAL(itemSelectionChanged()), this, SLOT(recvStreamSelectChanged()));
	delete ui;
}

void qtvUITabContext::initConnect()
{
	ui->setupUi(this);

	if (m_main->_mode() == qui_main_usbmuxer_wizard)
		ui->li_stream->setVisible(true);
	else ui->li_stream->setVisible(false);

	ui->table_props->setProperties(qui_table_normal, NULL);
	connect(ui->tr_channel, SIGNAL(itemSelectionChanged()), this, SLOT(recvContextSelectChanged()));
	connect(ui->li_stream, SIGNAL(itemSelectionChanged()), this, SLOT(recvStreamSelectChanged()));
}

void qtvUITabContext::tabEnter()
{
	if (m_main->_mode() == qui_main_bcwizard)
	{
		qtvUIMainWizardBroadcast* bcmain = qobject_cast<qtvUIMainWizardBroadcast*>(m_main);
		if (bcmain && bcmain->_mux_handle())
		{
			qtvMuxServiceDefault* defmux = qobject_cast<qtvMuxServiceDefault*>(bcmain->_mux_handle());
			if (defmux)
			{
				setChannel(defmux->_channel());
				return;
			}
		}
		setValid(false);
		VWAR("context not support");
	}
	else if (m_main->_mode() == qui_main_usbmuxer_wizard)
	{
		vatek_result nres = vatek_badstatus;
		qtvUIMainUSBMuxer* umuxmain = qobject_cast<qtvUIMainUSBMuxer*>(m_main);
		qtvServiceUSBMuxer* usbmuxer = NULL;
		Pqtv_locale_param plocal = NULL;
		if (umuxmain)
		{
			usbmuxer = qobject_cast<qtvServiceUSBMuxer*>(umuxmain->_service());
			if (usbmuxer && umuxmain->_local_param())nres = vatek_success;
			plocal = umuxmain->_local_param();
		}

		if (is_vatek_success(nres))
		{
			qtvUSBMuxerChannel* ch = NULL;
			qtvUSBMuxerProgram* program = NULL;

			usbmuxer->resetChannel();
			nres = usbmuxer->createChannel(plocal->spec, plocal->country, &ch);
			if (is_vatek_success(nres))
			{
				nres = ch->createProgram(0x1000, &program);
				if (is_vatek_success(nres))
				{
					qtvUSBMuxerStream* stream = NULL;
					nres = program->createStream(0x1001,umuxmain->_video_codec(),&stream);
					if (is_vatek_success(nres))nres = program->commitStream(stream);
					if (is_vatek_success(nres))
					{
						nres = program->createStream(0x1002, umuxmain->_audio_codec(), &stream);
						if (is_vatek_success(nres))nres = program->commitStream(stream);
					}
					if (is_vatek_success(nres))nres = ch->commitProgram(program);
				}
				if (is_vatek_success(nres))nres = usbmuxer->commitChannel(ch);
			}

			if (is_vatek_success(nres))setChannel(ch);
		}
	}

}

void qtvUITabContext::tabFinish()
{
#if 0
	if (m_main->_mode() == qui_main_usbmuxer_wizard)
	{
		qtvServiceUSBMuxer* usbmuxer = qobject_cast<qtvServiceUSBMuxer*>(m_main->_service());
		if (usbmuxer)
		{
			qtvUSBMuxerChannel* ch = NULL;
			vatek_result nres = usbmuxer->getChannel(&ch);
			if (is_vatek_success(nres))
			{
				nres = ch->applyProperties();
				if (is_vatek_success(nres))
				{
					int32_t i = 0;
					for (i = 0; i < ch->_programs().count(); i++)
					{
						qtvUSBMuxerProgram* program = qobject_cast<qtvUSBMuxerProgram*>(ch->_programs()[i]);
						nres = program->applyProperties();
						if (is_vatek_success(nres))
						{
							int32_t j = 0;
							for (j = 0; j < program->_streams().count(); j++)
							{
								qtvUSBMuxerStream* stream = qobject_cast<qtvUSBMuxerStream*>(program->_streams()[j]);
								nres = stream->applyProperties();
								if(!is_vatek_success(nres))break;
							}
						}
						if (!is_vatek_success(nres))break;
					}
				}
			}
			if (!is_vatek_success(nres))VWAR("usbmuxer apply context fail : %d", nres);
		}
	}
#endif
}

void qtvUITabContext::tabLeave()
{
	setChannel(NULL);
}

void qtvUITabContext::setChannel(qtvMuxChannel* channel)
{
	ui->tr_channel->setContext(channel);
	ui->table_props->setProperties(qui_table_normal, NULL);
}

void qtvUITabContext::recvContextSelectChanged()
{
	qtvMuxItem* muxitem = ui->tr_channel->getSelectMuxItem();
	QListWidget* uilist = ui->li_stream;
	ui->table_props->setProperties(qui_table_normal, NULL);

	uilist->clear();

	if (muxitem)
	{
		if (muxitem->_mode() == qmuxitem_channel)
		{
			qtvMuxChannel* muxch = qobject_cast<qtvMuxChannel*>(muxitem);
			ui->table_props->setProperties(qui_table_normal, muxch->_properties());
		}
		else if (muxitem->_mode() == qmuxitem_program)
		{
			qtvMuxProgram* muxprog = qobject_cast<qtvMuxProgram*>(muxitem);
			int32_t i = 0;
			const QList<qtvMuxStream*> streams = muxprog->_streams();
			ui->table_props->setProperties(qui_table_normal, muxprog->_properties());
			for (i = 0; i < streams.count(); i++)
			{
				QListWidgetItem* newitem = new QListWidgetItem(streams[i]->_name());
				newitem->setData(Qt::UserRole, QVariant::fromValue<qtvMuxStream*>(streams[i]));
				uilist->addItem(newitem);
			}
		}
	}
}

void qtvUITabContext::recvStreamSelectChanged()
{
	if (m_main->_mode() == qui_main_usbmuxer_wizard)
	{
		QListWidget* uilist = ui->li_stream;
		if (uilist->selectedItems().count())
		{
			QListWidgetItem* selectitem = uilist->selectedItems()[0];
			qtvMuxStream* rawstream = qvariant_cast<qtvMuxStream*>(selectitem->data(Qt::UserRole));
			if (rawstream)
			{
				qtvUSBMuxerStream* usbstream = qobject_cast<qtvUSBMuxerStream*>(rawstream);
				if (usbstream)ui->table_props->setProperties(qui_table_normal, usbstream->_properties());
			}
		}
	}
}
