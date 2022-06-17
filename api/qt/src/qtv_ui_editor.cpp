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

#include "./internal/qinternal_ui.h"

/* qi_editor_unsupport */

qui_editor_unsupport::qui_editor_unsupport(QWidget* parent) :
	QLabel(parent)
{
	setText(QString("unsupport"));
}

/* qi_editor_combobox */

qui_editor_combobox::qui_editor_combobox(qi_property* prop, QWidget* parent) :
	m_prop(prop),
	m_model(new QStandardItemModel(this)),
	QComboBox(parent)
{
	refreshItems();
	setLineEdit(new QLineEdit(parent));
	lineEdit()->setReadOnly(true);
	lineEdit()->installEventFilter(this);
}

qui_editor_combobox::~qui_editor_combobox()
{
	delete m_model;
}

uint32_t qui_editor_combobox::getValue()
{
	uint32_t m_value = 0;
	if (m_prop->_type() == qproperty_flag)
	{
		for (int32_t i = 0; i < m_model->rowCount(); i++)
		{
			if (m_model->item(i)->data(Qt::CheckStateRole) == Qt::Checked)
				m_value |= m_prop->_ui_prop()->values[i].val;
		}
	}
	else
	{
		//m_value = m_prop->_ui_prop()->values[currentIndex()].val;
		m_value = m_model->item(currentIndex())->data().toUInt();
	}
	m_prop = NULL;
	return m_value;
}

void qui_editor_combobox::refreshItems()
{
	m_model->clear();
	if (m_prop->_ui_prop()->values)
	{
		int32_t i = 0;
		int32_t npos = 0;
		const Pui_prop_item pui = m_prop->_ui_prop();
		uint32_t val = m_prop->_value().toUInt();

		while (pui->values[i].name)
		{
			if (m_prop->checkSelectValid(&pui->values[i]))
			{
				QStandardItem* newitem = new QStandardItem(pui->values[i].name);
				if (m_prop->_type() == qproperty_flag)
				{
					newitem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
					if (val & pui->values[i].val)newitem->setData(Qt::Checked, Qt::CheckStateRole);
					else newitem->setData(Qt::Unchecked, Qt::CheckStateRole);
				}
				else newitem->setData(pui->values[i].val);
				m_model->setItem(npos++, 0, newitem);
			}
			i++;
		}
	}
	setModel(m_model);
}

bool qui_editor_combobox::eventFilter(QObject* _object, QEvent* _event)
{
	if (m_prop)
	{
		if (m_prop->_type() == qproperty_flag)
		{
			if (_object == lineEdit() && _event->type() == QEvent::Show)
			{
				lineEdit()->setText("");
				showPopup();
				return true;
			}
		}
	}
	return false;
}

/* qi_editor_uintedit */

qui_editor_uintedit::qui_editor_uintedit(qi_property* prop, QWidget* parent) :
	m_prop(prop),
	m_format(""),
	m_ishex(0),
	QSpinBox(parent)
{
	const QRegularExpression rx("(?:0[xX])?[0-9A-Fa-f]{1,8}");
	m_validator.setRegularExpression(rx);
	refreshItem();
}

void qui_editor_uintedit::refreshItem()
{
	const Pui_prop_item pui = m_prop->_ui_prop();
	int32_t len = pui->type & 0xF;
	if (IS_UIPROP_HEX(pui->type))
	{
		setPrefix(QStringLiteral("0x"));
		if (len == 1)
		{
			setMaximum(0xFF);
			m_format = "%02x";
		}
		else if (len == 2)
		{
			setMaximum(0xFFFF);
			m_format = "%04x";
		}
		else
		{
			setMaximum(0x7FFFFFFF);
			m_format = "%08x";
		}
		m_ishex = 1;
	}
	else
	{
		setPrefix(QString(""));
		if (len == 1)setMaximum(0xFF);
		else if (len == 2)setMaximum(0xFFFF);
		else setMaximum(0x7FFFFFFF);
		m_format = "%d";
	}
	setValue(m_prop->_value().toInt());
}

QValidator::State qui_editor_uintedit::validate(QString& text, int& pos) const
{
	if (m_ishex)
		return m_validator.validate(text, pos);
	return QValidator::State::Acceptable;
}

int qui_editor_uintedit::valueFromText(const QString& text) const
{
	int32_t val = 0;
	if (m_ishex)val = text.toUInt(0, 16);
	else val = text.toUInt();
	return val;
}

QString qui_editor_uintedit::textFromValue(int value) const
{
	if (m_ishex)return QString().sprintf(qPrintable(m_format), value);
	return QSpinBox::textFromValue(value);
}

/* qi_editor_path */

qui_editor_path::qui_editor_path(qi_property* prop, qtv_property_pathmode mode, QWidget* parent) :
	m_prop(prop),
	m_mode(mode),
	m_txtfile(NULL),
	m_txturl(NULL),
	QWidget(parent)
{
	refreshItem();
}

void qui_editor_path::refreshItem()
{
	QHBoxLayout* newlayout = new QHBoxLayout(this);
	newlayout->setMargin(0);
	if (m_mode == qpathmode_file_ts ||
		m_mode == qpathmode_passthrough)
	{
		QPushButton* newbtn = new QPushButton("...", this);
		m_txtfile = new QLabel(this);
		m_txtfile->setText(m_prop->_display_value());
		newbtn->setFixedSize(QSize(25, 25));
		connect(newbtn, SIGNAL(clicked()), this, SLOT(recvFileSelect()));

		newlayout->addWidget(m_txtfile);
		newlayout->addWidget(newbtn);
	}
	else
	{
		m_txturl = new QLineEdit(this);
		if (m_mode == qpathmode_url_udp)
			m_txturl->setInputMask("000.000.000.000:0000");
		else if (m_mode == qpathmode_url_rtp)
			m_txturl->setInputMask("000.000.000.000:0000");
		connect(m_txturl, SIGNAL(editingFinished()), this, SLOT(recvTextFinish()));
		newlayout->addWidget(m_txturl);
	}
	setLayout(newlayout);
}

void qui_editor_path::recvFileSelect()
{
	QString path = QFileDialog::getOpenFileName(this, "Open File", ".", "TS Files (*ts *.trp);;All files (*.*);;");
	if (path.length())
	{
		m_szvalue = path;
		m_txtfile->setText(m_szvalue);
	}
}

void qui_editor_path::recvTextFinish()
{
	if (m_mode == qpathmode_url_udp)m_szvalue = "udp://" + m_txturl->text();
	else m_szvalue = "rtp://" + m_txturl->text();
}

/* qi_editor_string */

qui_editor_string::qui_editor_string(qi_property_string_base* prop, QWidget* parent) :
	m_prop(prop),
	QLineEdit(parent)
{
	setText(m_prop->_display_value());
	connect(this, SIGNAL(editingFinished()), this, SLOT(recvTextFinish()));
}

qui_editor_string::~qui_editor_string()
{
	disconnect(this, SIGNAL(editingFinished()), this, SLOT(recvTextFinish()));
}

void qui_editor_string::recvTextFinish()
{

}
