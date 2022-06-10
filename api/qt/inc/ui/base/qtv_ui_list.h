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

#ifndef QTV_UI_LIST_H
#define QTV_UI_LIST_H

#include <QListWidget>
#include <ui/base/qtv_ui_core.h>

typedef enum _qtv_ui_list_mode
{
	qui_list_normal,
	qui_list_logo,
	qui_list_logo_title_1,
	qui_list_logo_title_2,
}qtv_ui_list_mode;

class QTV_DECL qtvUIListBase : public QListWidget
{
	Q_OBJECT
public:
	explicit qtvUIListBase(QWidget* parent = nullptr);
};

class QTV_DECL qtvUIListItem : public QObject
{
	Q_OBJECT
public:
	explicit qtvUIListItem(uint32_t logo, int32_t sizelogo, const char* title0, const char* title1);
	explicit qtvUIListItem(uint32_t logo, int32_t sizelogo, QString& title0, QString& title1);
	explicit qtvUIListItem(uint32_t logo, int32_t sizelogo, QString& title0);

	QString& _title_0() { return m_title0; }
	QString& _title_1() { return m_title1; }
	uint32_t _logo() { return m_logo; }
	QPixmap& _logo_pixmap() { return m_pixmap; }
	QWidget* _widget() { return m_widget; }

	void setWidget(QWidget* widget);

protected:
	uint32_t m_logo;
	int32_t m_sizelogo;
	QPixmap m_pixmap;
	QString m_title0;
	QString m_title1;
	QWidget* m_widget;

	void refreshLogo();
};

class QTV_DECL qtvUIListConfig : public qtvUIListBase
{
	Q_OBJECT
public:
	explicit qtvUIListConfig(QWidget* parent = nullptr);
	virtual ~qtvUIListConfig();

	virtual void resetList(qtv_ui_list_mode mode,int32_t iconsize);
	virtual void insertItem(qtvUIListItem* item);
	virtual qtvUIListItem* getSelectConfig();
	virtual void setSelectConfig(int32_t idx);

//Q_SIGNALS:
//	void notifyItemsChanged();

protected:
	QList<qtvUIListItem*> m_items;
};


#endif
