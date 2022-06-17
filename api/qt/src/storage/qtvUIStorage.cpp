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

#include <ui/base/qtv_ui_storage.h>
#include "..\..\qt\inc\ui\base\qtv_ui_bootlogo.h"
#include <QRegularExpression>

#define UISTORAGE_ICON_SIZE		32

/* qtvUIMainStorage */

qtvUIMainStorage::qtvUIMainStorage() :
	qtvUIMain(qui_main_storage, NULL),
	m_hstorage(NULL)
{
	
}

void qtvUIMainStorage::setStorage(hvatek_storage hstorage)
{
	m_hstorage = hstorage;
}

/* qtvUIListStorage */

qtvUIListStorage::qtvUIListStorage(QWidget* parent) :
	qtvUIListConfig(parent),
	m_hstorage(NULL),
	m_stacked(NULL),
	m_empty(NULL)
{
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(recvItemSelectChanged()));
}

qtvUIListStorage::~qtvUIListStorage()
{
	if (m_empty)delete m_empty;
	m_empty = NULL;
}

void qtvUIListStorage::attachStacked(QStackedWidget* stacked)
{
	m_stacked = stacked;
	if (m_stacked)
	{
		if (m_empty)delete m_empty;
		m_empty = new qtvUIStorageInfo(&m_main, m_stacked);
	}
	refreshStorage(m_hstorage);
}

void qtvUIListStorage::refreshStorage(hvatek_storage hstorage)
{
	int32_t i = 0;
	if (m_stacked)
	{
		if (!m_hstorage)m_stacked->removeWidget(m_empty);
		else
		{
			for (i = 0; i < m_items.size(); i++)
			{
				qtvUIListItem* item = m_items[i];
				if (item->_widget())
					m_stacked->removeWidget(item->_widget());
			}
		}
	}
	resetList(qui_list_logo_title_1, UISTORAGE_ICON_SIZE);
	m_main.setStorage(hstorage);
	if (hstorage)
	{
		insertItem(createInformation(&m_main, m_stacked));
		insertItem(createServiceProperties(&m_main, m_stacked));
		insertItem(createR2Tune(&m_main, m_stacked));
		insertItem(createChipConfig(&m_main, m_stacked));
		insertItem(createBootlogo(&m_main, m_stacked));

		if (m_stacked)
		{
			if (m_items.size())
			{
				for (i = 0; i < m_items.size(); i++)
				{
					qtvUIListItem* item = m_items[i];
					if (item->_widget())
						m_stacked->addWidget(item->_widget());
				}
				setSelectConfig(0);
			}
		}
	}
	m_hstorage = hstorage;

	if (!m_hstorage)
		m_stacked->addWidget(m_empty);
}

void qtvUIListStorage::recvItemSelectChanged()
{
	if (m_stacked)
	{
		qtvUIListItem* item = getSelectConfig();
		if (item && item->_widget())
		{
			qtvUITabBase* prev = qobject_cast<qtvUITabBase*>(m_stacked->currentWidget());
			if (prev)prev->tabLeave();
			prev = qobject_cast<qtvUITabBase*>(item->_widget());
			if(prev)prev->tabEnter();
			m_stacked->setCurrentWidget(item->_widget());
		}
	}
}

qtvUIListItem* qtvUIListStorage::createInformation(qtvUIMainStorage* main, QWidget* parent)
{
	qtvUIListItem* newlist = new qtvUIListItem(ICON_INFORMATION, UISTORAGE_ICON_SIZE, "Information",NULL);
	newlist->setWidget(new qtvUIStorageInfo(main, parent));
	return newlist;
}

qtvUIListItem* qtvUIListStorage::createServiceProperties(qtvUIMainStorage* main, QWidget* parent)
{
	qtvUIListItem* newlist = new qtvUIListItem(ICON_PROPERTIES, UISTORAGE_ICON_SIZE, "Properties", NULL);
	newlist->setWidget(new qtvUIStorageProps(main, sprops_properties,parent));
	return newlist;
}

qtvUIListItem* qtvUIListStorage::createR2Tune(qtvUIMainStorage* main, QWidget* parent)
{
	qtvUIListItem* newlist = new qtvUIListItem(ICON_OUTPUT, UISTORAGE_ICON_SIZE, "RF Table", NULL);
	newlist->setWidget(new qtvUIStorageR2Tune(main, parent));
	return newlist;
}

qtvUIListItem* qtvUIListStorage::createChipConfig(qtvUIMainStorage* main, QWidget* parent)
{
	qtvUIListItem* newlist = new qtvUIListItem(ICON_CHANNEL, UISTORAGE_ICON_SIZE, "Configs", NULL);
	newlist->setWidget(new qtvUIStorageProps(main, sprops_config, parent));
	return newlist;
}

qtvUIListItem* qtvUIListStorage::createBootlogo(qtvUIMainStorage* main, QWidget* parent)
{
	qtvUIListItem* newlist = new qtvUIListItem(ICON_VIDEO, UISTORAGE_ICON_SIZE, "Bootlogo", NULL);
	newlist->setWidget(new bootlogo(main, parent));
	return newlist;
}

/* qtvUIUsbIDEditor */

qtvUIUsbIDEditor::qtvUIUsbIDEditor(QWidget* parent) :
	QSpinBox(parent),
	m_format("0x%04x"),
	m_value(NULL)
{
	setRange(0x0001, 0xFFFF);
	const QRegularExpression rx("(?:0[xX])?[0-9A-Fa-f]{1,8}");
	m_validator.setRegularExpression(rx);
	connect(this, SIGNAL(valueChanged(int)), this, SLOT(recvValueChanged(int)));
}

qtvUIUsbIDEditor::~qtvUIUsbIDEditor()
{
	disconnect(this, SIGNAL(valueChanged(int)), this, SLOT(recvValueChanged(int)));
}

QValidator::State qtvUIUsbIDEditor::validate(QString& text, int& pos) const
{
	return m_validator.validate(text, pos);
}

int qtvUIUsbIDEditor::valueFromText(const QString& text) const
{
	return text.toUInt(0, 16);
}

QString qtvUIUsbIDEditor::textFromValue(int value) const
{
	return QString().sprintf(qPrintable(m_format), value);
}

void qtvUIUsbIDEditor::setValuePtr(uint32_t* id)
{
	m_value = id;
	setValue(*m_value);
}

void qtvUIUsbIDEditor::recvValueChanged(int val)
{
	if (m_value)*m_value = val;
}

/* qtvUIUSBStringEditor */

qtvUIUSBStringEditor::qtvUIUSBStringEditor(QWidget* parent) :
	QLineEdit(parent),
	m_ptrstr(NULL),
	m_len(0)
{
	connect(this, SIGNAL(editingFinished()), this, SLOT(recvEditFinish()));
}

qtvUIUSBStringEditor::~qtvUIUSBStringEditor()
{
	disconnect(this, SIGNAL(editingFinished()), this, SLOT(recvEditFinish()));
}

void qtvUIUSBStringEditor::recvEditFinish()
{
	memset(&m_ptrstr[0], 0, m_len);
	if (text().size() > 0 && text().size() <= m_len)
	{
		QByteArray ba = text().toLocal8Bit();
		const char* c_str2 = ba.data();
		strncpy(&m_ptrstr[0], c_str2, m_len);
	}
	else
	{
		strncpy(&m_ptrstr[0],"VAT-Device", m_len);
	}
}

void qtvUIUSBStringEditor::setStringPtr(char* ptrstr, int32_t len)
{
	m_ptrstr = ptrstr;
	m_len = len;
	if (m_ptrstr)
	{
		setText(m_ptrstr);
	}
}
