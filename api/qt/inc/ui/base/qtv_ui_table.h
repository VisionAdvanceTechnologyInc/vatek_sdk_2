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

#ifndef QTV_UI_TABLE_H
#define QTV_UI_TABLE_H

#include <ui/base/qtv_ui_core.h>
#include <QTableWidget>
#include <QResizeEvent>
#include <QStyledItemDelegate.h>

#define QTV_UI_TABLE_WIDTH_UNIT		20

typedef enum _qui_table_mode
{
	qui_table_normal,
	qui_table_readonly,
}qui_table_mode;

typedef struct _qui_table_field
{
	const char* title;
	int32_t size;
}qui_table_field, * Pqui_table_field;

static const qui_table_field qtable_normal_fields[] =
{
	{"descriptor",9,},
	{"type",3,},
	{"value",6,},
};

#define QTABLE_NORMAL_FIELDS		(sizeof(qtable_normal_fields)/sizeof(qui_table_field))

static const qui_table_field qtable_readonly_fields[] =
{
	{"descriptor",7,},
	{"value",14,},
};

#define QTABLE_READONLY_FIELDS		(sizeof(qtable_readonly_fields)/sizeof(qui_table_field))

class QTV_DECL qtvUITableBase : public QTableWidget
{
	Q_OBJECT
public:
	explicit qtvUITableBase(QWidget* parent = nullptr);
	virtual ~qtvUITableBase();

	virtual qtvPropertyCollection* _properties() { return m_properties; }
	virtual void setProperties(qui_table_mode mode,qtvPropertyCollection* properties);

	static const char* getPropertyTypeName(qtvProperty* prop);
 protected:
	qtvPropertyCollection* m_properties;
	qtvUIMain* m_main;
	qui_table_mode m_mode;
	QSize m_size;
	void insertRowNormal(int32_t idx, qtvProperty* prop);
	void insertRowReadOnly(int32_t idx, qtvProperty* prop);
	void insertRowEmpty(int32_t idx);

	void resizeColumn();

	void attachProperties(qtvPropertyCollection* properties);
	void detachProperties();

	/* QTableWidget */
	void resizeEvent(QResizeEvent* event) override;

protected Q_SLOTS:
	void refreshProperties();
};

class QTV_DECL qtvUITableDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit qtvUITableDelegate(qtvUITableBase* table);
	virtual ~qtvUITableDelegate();


	/* QStyledItemDelegate */
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

protected:
	qtvUITableBase* m_table;

	static qtvProperty* getProperty(const qtvUITableDelegate* editor,const QModelIndex& index);
	
};

#endif
