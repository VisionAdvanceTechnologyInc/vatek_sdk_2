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

#include <ui/base/qtv_ui_table.h>
#include "./internal/qinternal_ui.h"
#include "./internal/qinternal_properties.h"
#include <QResizeEvent>
#include <QHeaderView>

/* qtvUITableBase */

qtvUITableBase::qtvUITableBase(QWidget* parent) :
	m_main(NULL),
	m_properties(NULL),
	m_mode(qui_table_readonly),
	m_size(0,0),
	QTableWidget(parent)
{
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::SingleSelection);
	verticalHeader()->setStretchLastSection(true);
	horizontalHeader()->setStretchLastSection(true);
}

qtvUITableBase::~qtvUITableBase()
{

}

void qtvUITableBase::setProperties(qui_table_mode mode, qtvPropertyCollection* properties)
{
	if (m_mode != mode)
	{
		m_mode = mode;
		resizeColumn();
	}
	attachProperties(properties);
}

void qtvUITableBase::attachProperties(qtvPropertyCollection* properties)
{
	detachProperties();
	if (properties)
	{
		m_properties = properties;
		refreshProperties();
		connect(m_properties, SIGNAL(notifyPropertiesChanged()), this, SLOT(refreshProperties()));
	}
}

void qtvUITableBase::detachProperties()
{
	if (m_properties)
	{
		disconnect(m_properties, SIGNAL(notifyPropertiesChanged()), this, SLOT(refreshProperties()));
		clearContents();
	}
	m_properties = NULL;
}

void qtvUITableBase::resizeColumn()
{
	int32_t nw = viewport()->width() / QTV_UI_TABLE_WIDTH_UNIT;
	QStringList sztitles;

	if (m_mode == qui_table_normal)
	{
		setColumnCount(QTABLE_NORMAL_FIELDS);
		for (int32_t i = 0; i < (int32_t)QTABLE_NORMAL_FIELDS; i++)
		{
			setColumnWidth(i, nw * qtable_normal_fields[i].size);
			sztitles.append(QString(qtable_normal_fields[i].title));
		}
	}
	else
	{
		setColumnCount(QTABLE_READONLY_FIELDS);
		for (int32_t i = 0; i < (int32_t)QTABLE_READONLY_FIELDS; i++)
		{
			setColumnWidth(i, nw * qtable_readonly_fields[i].size);
			sztitles.append(QString(qtable_readonly_fields[i].title));
		}
	}

	setHorizontalHeaderLabels(sztitles);
	if(m_mode == qui_table_normal)
		setItemDelegateForColumn(2, new qtvUITableDelegate(this));
	verticalHeader()->hide();
}

void qtvUITableBase::resizeEvent(QResizeEvent* event)
{
	if (m_size != size())
	{
		resizeColumn();
		m_size = size();
	}
}

void qtvUITableBase::insertRowNormal(int32_t idx, qtvProperty* prop)
{
	QTableWidgetItem* item = new QTableWidgetItem(prop->_descriptor());
	item->setFlags(item->flags() & ~Qt::ItemIsEditable);
	setItem(idx, 0, item);
	item = new QTableWidgetItem(getPropertyTypeName(prop));
	item->setFlags(item->flags() & ~Qt::ItemIsEditable);
	setItem(idx, 1, item);
	setItem(idx, 2, new QTableWidgetItem(prop->_display_value()));
}

void qtvUITableBase::insertRowReadOnly(int32_t idx,qtvProperty* prop)
{
	QString szvalue;
	QTableWidgetItem* item = new QTableWidgetItem(prop->_descriptor());
	qi_property* quiprop = static_cast<qi_property*>(prop);
	item->setFlags(item->flags() & ~Qt::ItemIsEditable);
	setItem(idx, 0, item);

	if (quiprop)
	{
		const Pui_prop_item puiprop = quiprop->_ui_prop();
		if (puiprop->values)
		{
			if (IS_UIPROP_CHECKBOS(puiprop->type))
			{
				const Pui_prop_val pvals = puiprop->values;
				uint32_t value = prop->_value().toUInt();
				char sztmp[256];
				char* ptr = &sztmp[0];
				int32_t j = 0;
				szvalue = "";
				memset(&sztmp[0], 0, 256);
				while (pvals[j].name)
				{
					if ((pvals[j].val & value) == pvals[j].val)
					{
						sprintf(ptr, "%s;", pvals[j].name);
						ptr += strlen(ptr);
					}
					j++;
				}
				szvalue = &sztmp[0];
			}
			else szvalue = prop->_display_value();
		}
		else szvalue = prop->_display_value();
	}
	
	item = new QTableWidgetItem(szvalue);
	item->setFlags(item->flags() & ~Qt::ItemIsEditable);
	setItem(idx, 1, item);
}

void qtvUITableBase::insertRowEmpty(int32_t idx)
{
	int32_t fielsnums = QTABLE_NORMAL_FIELDS;
	if (m_mode == qui_table_readonly)
		fielsnums = QTABLE_READONLY_FIELDS;

	for (int32_t j = 0; j < fielsnums; j++)
	{
		QTableWidgetItem* pempty = new QTableWidgetItem("");
		pempty->setFlags(pempty->flags() & ~Qt::ItemIsEditable);
		setItem(idx, j, pempty);
	}
}

void qtvUITableBase::refreshProperties()
{
	if (m_properties)
	{
		const QList<qtvProperty*> props = m_properties->_properties();
		int32_t i = 0;

		setRowCount(props.count() + 1);
		for (i = 0; i < props.count(); i++)
		{
			if (m_mode == qui_table_normal)
				insertRowNormal(i, props[i]);
			else insertRowReadOnly(i, props[i]);
		}
		insertRowEmpty(i);
	}
}

const char* qtvUITableBase::getPropertyTypeName(qtvProperty* prop)
{
	qtv_property_type type = prop->_type();
	if (type == qproperty_select)return "select";
	else if (type == qproperty_flag)return "flags";
	else if (type == qproperty_string)return "string";
	else if (type == qproperty_path)return "path";
	else return "uint";
}


/* qtvUITableDelegate */

qtvUITableDelegate::qtvUITableDelegate(qtvUITableBase* table) :
	m_table(table)
{

}

qtvUITableDelegate::~qtvUITableDelegate()
{

}

qtvProperty* qtvUITableDelegate::getProperty(const qtvUITableDelegate* editor, const QModelIndex& index)
{
	if (editor->m_table)
	{
		qtvPropertyCollection* collection = editor->m_table->_properties();
		if (index.row() < collection->_properties().count())
			return collection->_properties()[index.row()];
	}
	return NULL;
}

QWidget* qtvUITableDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	qtvProperty* prop = getProperty(this, index);
	if (prop)
	{
		qi_property* iprop = static_cast<qi_property*>(prop);
		if (iprop->_type() == qproperty_path)
			m_table->item(index.row(), 2)->setText("");
		if (iprop)return iprop->createTableEditor(parent);
	}
	return QStyledItemDelegate::createEditor(parent,option,index);
}

void qtvUITableDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	qtvProperty* prop = getProperty(this, index);
	if (prop)
	{
		qi_property* iprop = static_cast<qi_property*>(prop);
		if (iprop)
		{
			if (iprop->_is_refresh())iprop->setValueEditor(editor);
			else
			{
				iprop->setValueEditor(editor);
				m_table->item(index.row(), 2)->setText(iprop->_display_value());
			}
			return;
		}
	}
	QStyledItemDelegate::setModelData(editor, model, index);
}
