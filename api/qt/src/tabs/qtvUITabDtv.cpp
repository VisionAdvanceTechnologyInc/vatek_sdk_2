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
#include <ui/base/qtv_ui_table.h>
#include <ui/qtv_ui_service_base.h>
#include <ui/qtv_ui_service_usb.h>

#include "ui_qtvUITabDtv.h"
#include "../internal/qinternal_ui.h"

#include <QLabel>
#include <QComboBox>
#include <QScreen>
#include <qDebug>
#include <windows.h>
#include <conio.h>

#define UITAB_DTV_NAME		"DTV Config"
#define UITAB_DTV_DESC		"select DTV default properties"

qtvUITabDtv::qtvUITabDtv(qtvUIMain* main, QWidget* parent) :
	m_properties(NULL),
	qtvUITabBase(main, UITAB_DTV_NAME, UITAB_DTV_DESC, parent),
	ui(new Ui::qtvUITabDtv)
{
	ui->setupUi(this);
	ui->lb_country->setText("Default");
	ui->lb_channel->setText("Channel");
	connect(ui->cb_country, SIGNAL(currentIndexChanged(int)), this, SLOT(recvCountrySelect(int)));
	connect(ui->ch_channel, SIGNAL(currentIndexChanged(int)), this, SLOT(recvChannelSelect(int)));
	
	QScreen* screen = QGuiApplication::primaryScreen();     //螢幕大小
	QRect mm = screen->availableGeometry();
	int screen_width = mm.width();
	int screen_height = mm.height();
	qDebug() << screen_width << screen_height;

	HDC hd = GetDC(NULL);                                           // 獲取DPI
	int horDPI = GetDeviceCaps(hd, LOGPIXELSX);
	int verticalDPI = GetDeviceCaps(hd, LOGPIXELSY);
	double objectRate = horDPI / 96.0;

	if (screen_width >= 1920) {
		if (objectRate >= 1.5) {
			ui->table_detail->setMinimumSize(screen_width / 1.8, screen_height / 2.4);
		}
		else {
			ui->table_detail->setMinimumSize(screen_width / 2.1, screen_height / 2.4);
		}
	}
	else {
		ui->table_detail->setMinimumSize(screen_width / 1.8, screen_height / 2.4);
	}

}

qtvUITabDtv::~qtvUITabDtv()
{
	disconnect(ui->cb_country, SIGNAL(currentIndexChanged(int)), this, SLOT(recvCountrySelect(int)));
	disconnect(ui->ch_channel, SIGNAL(currentIndexChanged(int)), this, SLOT(recvChannelSelect(int)));
	resetPropertes();
	delete ui;
}

void qtvUITabDtv::tabEnter()
{
	refreshCountry();
}

void qtvUITabDtv::tabFinish()
{
	if (m_properties)
	{
		if (m_main->_mode() == qui_main_bcwizard)
		{
			qtvUIMainWizardBroadcast* bcmain = qobject_cast<qtvUIMainWizardBroadcast*>(m_main);
			if (bcmain)
			{
				bcmain->setDefaultLocale(m_properties);
				m_properties = NULL; /* take locale properties */
			}
		}
		else if (m_main->_mode() == qui_main_usbmuxer_wizard)
		{
			qtvUIMainUSBMuxer* usbmux = qobject_cast<qtvUIMainUSBMuxer*>(m_main);
			if (usbmux)
			{
				usbmux->setDefaultLocal(m_properties);
				m_properties = NULL;
			}
		}
	}
}

void qtvUITabDtv::tabLeave()
{
	resetPropertes();
}

void qtvUITabDtv::resetPropertes()
{
	ui->table_detail->setProperties(qui_table_normal, NULL);
	if (m_properties)delete m_properties;
	m_properties = NULL;
}

void qtvUITabDtv::refreshCountry()
{
	const QList<qtvDTVLocale*> locales = qtvDTVManage::getDTVLocals();
	int32_t i = 0;

	resetPropertes();
	ui->cb_country->clear();
	ui->ch_channel->clear();

	for (i = 0; i < locales.count(); i++)
	{
		qtvDTVLocale* plocale = locales.at(i);
		ui->cb_country->addItem(plocale->_name(), QVariant::fromValue<qtvDTVLocale*>(plocale));
	}
	ui->cb_country->setCurrentIndex(0);
}

void qtvUITabDtv::refreshChannel(qtvDTVLocale* plocale)
{
	int32_t i = 0;
	const QList<qtvDTVChannel*> channels = plocale->_channels();
	for (i = 0; i < channels.count(); i++)
	{
		qtvDTVChannel* prf = channels.at(i);
		QString sztitle;
		sztitle.sprintf("[%s] : %d", prf->_name(), prf->_frequency());
		ui->ch_channel->addItem(sztitle, QVariant::fromValue<qtvDTVChannel*>(prf));
	}
	ui->ch_channel->setCurrentIndex(0);
}

void qtvUITabDtv::recvCountrySelect(int index)
{
	qtvDTVLocale* plocale = qvariant_cast<qtvDTVLocale*>(ui->cb_country->itemData(index));
	resetPropertes();
	ui->ch_channel->clear();
	if (plocale)refreshChannel(plocale);
}

void qtvUITabDtv::recvChannelSelect(int index)
{
	qtvDTVChannel* pchannel = qvariant_cast<qtvDTVChannel*>(ui->ch_channel->itemData(index));
	if (pchannel)
	{
		if (m_properties)m_properties->setChannel(pchannel);
		else
		{
			index = ui->cb_country->currentIndex();
			qtvDTVLocale* plocale = qvariant_cast<qtvDTVLocale*>(ui->cb_country->itemData(index));
			m_properties = qtvDTVManage::createParam(plocale, pchannel);
		}
		ui->table_detail->setProperties(qui_table_normal, m_properties->getPropertyCollection());
	}
}
