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
#include "ui_qtvUITabSource.h"
#include <QScreen>
#include <qDebug>
#include <windows.h>
#include <conio.h>
#define UITAB_SOURCE_NAME	"Source"
#define UITAB_SOURCE_DESC	"select service source"

qtvUITabSource::qtvUITabSource(qtvUIMain* main, QWidget* parent) :
	qtvUITabBase(main, UITAB_SOURCE_NAME, UITAB_SOURCE_DESC, parent),
	m_properties(NULL),
	ui(new Ui::qtvUITabSource)
{
	ui->setupUi(this);
	ui->lb_source->setText("Sources : ");

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
			ui->table_source->setMinimumSize(screen_width / 1.8, screen_height / 2.2);
		}
		else {
			ui->table_source->setMinimumSize(screen_width / 2.1, screen_height / 2.4);
		}
	}
	else {
		ui->table_source->setMinimumSize(screen_width / 1.8, screen_height / 2.2);
	}
	connect(ui->cb_source, SIGNAL(currentIndexChanged(int)), this, SLOT(recvSourceSelect(int)));
}

qtvUITabSource::~qtvUITabSource()
{
	disconnect(ui->cb_source, SIGNAL(currentIndexChanged(int)), this, SLOT(recvSourceSelect(int)));
	resetProperties();
	delete ui;
}

void qtvUITabSource::tabEnter()
{
	qtvUIMainWizardBroadcast* bcmain = qobject_cast<qtvUIMainWizardBroadcast*>(m_main);
	if (bcmain)bcmain->detachSource();
	refreshSource();
}

void qtvUITabSource::tabFinish()
{
	qtvUIMainWizardBroadcast* bcmain = qobject_cast<qtvUIMainWizardBroadcast*>(m_main);
	if (bcmain)
	{
		qtvSourceFactory* factory = qvariant_cast<qtvSourceFactory*>(ui->cb_source->itemData(ui->cb_source->currentIndex()));
		if (factory)
		{
			vatek_result nres = factory->checkProperties(m_properties);
			if (is_vatek_success(nres))
			{
				qtvSourceBase* source = NULL;
				nres = factory->createSource(m_main->_service(), finishProperties(), &source);
				if (is_vatek_success(nres))
				{
					nres = source->open();
					if (is_vatek_success(nres))bcmain->attachSource(source);
					else
					{
						VWAR("start source fail : [%s:%d]", factory->_name(), nres);
						delete source;
					}
					return;
				}
			}
			if (!is_vatek_success(nres))VWAR("create source fail : %d", nres);
			return;
		}
	}
	VWAR("unsupport main handle : %d",m_main->_mode());
}

void qtvUITabSource::tabLeave()
{
	resetProperties();
}

qtvPropertyCollection* qtvUITabSource::finishProperties()
{
	qtvPropertyCollection* rproperties = m_properties;
	disconnect(m_properties, SIGNAL(notifyValidChanged(bool)), this, SLOT(recvValidChanged(bool)));
	ui->table_source->setProperties(qui_table_normal, NULL);
	m_properties = NULL;	/* taked properties */
	return rproperties;
}

void qtvUITabSource::refreshSource()
{
	ui->cb_source->clear();
	if (m_main->_service())
	{
		QList<qtvSourceFactory*> m_sources = qtvServiceBroadcast::getSupportedSources();
		if (m_main->_service()->_mode() == qservice_transfrom)
			m_sources = qtvServiceTransform::getSupportedSources();

		for (int32_t i = 0; i < m_sources.count(); i++)
			ui->cb_source->insertItem(ui->cb_source->count(), m_sources[i]->_name(), QVariant::fromValue<qtvSourceFactory*>(m_sources[i]));
	}
}

void qtvUITabSource::resetProperties()
{
	if (m_properties)delete finishProperties();
	setValid(false);
}

void qtvUITabSource::recvSourceSelect(int index)
{
	if (index != -1)
	{
		vatek_result nres = vatek_badparam;
		qtvSourceFactory* factory = qvariant_cast<qtvSourceFactory*>(ui->cb_source->itemData(index));
		if (factory)
		{
			resetProperties();
			nres = factory->createProperties(m_main->_service(), &m_properties);
			if (is_vatek_success(nres))
			{
				ui->table_source->setProperties(qui_table_normal, m_properties);
				setValid(m_properties->_valid());
				connect(m_properties, SIGNAL(notifyValidChanged(bool)), this, SLOT(recvValidChanged(bool)));
			}
		}
		if (!is_vatek_success(nres))VWAR("qtvUITabSource create source fail : [%d:%d]", index, nres);
	}
}

void qtvUITabSource::recvValidChanged(bool bvalid)
{
	setValid(bvalid);
}
