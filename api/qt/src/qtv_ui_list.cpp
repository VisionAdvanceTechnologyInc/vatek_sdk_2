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

#include <ui/base/qtv_ui_list.h>
#include <QStyledItemDelegate>
#include <QSize>
#include <QPainter>
#include <QStyleOptionViewItem>

/* qtvUIListDelegate define */

class qtvUIListDelegate : public QStyledItemDelegate
{
public:
	explicit qtvUIListDelegate(qtv_ui_list_mode mode,int32_t logosize);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

protected:
	qtv_ui_list_mode m_mode;
	int32_t m_logosize;
};

/* qtvUIListItem */

qtvUIListItem::qtvUIListItem(uint32_t logo, int32_t sizelogo, QString& title0, QString& title1) :
	m_title0(title0),
	m_title1(title1),
	m_logo(logo),
	m_sizelogo(sizelogo),
	m_widget(NULL)
{
	refreshLogo();
}

qtvUIListItem::qtvUIListItem(uint32_t logo, int32_t sizelogo, QString& title0) :
	m_title0(title0),
	m_logo(logo),
	m_sizelogo(sizelogo),
	m_widget(NULL)
{
	refreshLogo();
}

qtvUIListItem::qtvUIListItem(uint32_t logo, int32_t sizelogo, const char* title0, const char* title1) :
	m_title0(title0),
	m_title1(title1),
	m_logo(logo),
	m_sizelogo(sizelogo),
	m_widget(NULL)
{
	refreshLogo();
}

void qtvUIListItem::refreshLogo()
{
	QImage* img = qtvUIResource::getImage(m_logo);
	m_pixmap = QPixmap::fromImage(img->scaled(QSize(m_sizelogo, m_sizelogo),Qt::KeepAspectRatio,Qt::SmoothTransformation));
}

void qtvUIListItem::setWidget(QWidget* widget)
{
	m_widget = widget;
}

/* qtvUIListBase */

qtvUIListBase::qtvUIListBase(QWidget* parent) :
	QListWidget(parent)
{
	
}

qtvUIListConfig::qtvUIListConfig(QWidget* parent) :
	qtvUIListBase(parent)
{

}

qtvUIListConfig::~qtvUIListConfig()
{
	resetList(qui_list_normal,0);
}

void qtvUIListConfig::resetList(qtv_ui_list_mode mode, int32_t iconsize)
{
	while (m_items.count())
	{
		delete m_items.first();
		m_items.removeFirst();
	}
	clear();
	setItemDelegate(new qtvUIListDelegate(mode, iconsize));
}

void qtvUIListConfig::insertItem(qtvUIListItem* item)
{
	QListWidgetItem* newitem = new QListWidgetItem(item->_title_0());
	newitem->setData(Qt::UserRole, QVariant::fromValue<qtvUIListItem*>(item));
	m_items.append(item);
	addItem(newitem);
}

qtvUIListItem* qtvUIListConfig::getSelectConfig()
{
	if (m_items.size())
	{
		QListWidgetItem* selectitem = selectedItems()[0];
		if (selectitem)
		{
			return qvariant_cast<qtvUIListItem*>(selectitem->data(Qt::UserRole));
		}
	}
	return NULL;
}

void qtvUIListConfig::setSelectConfig(int32_t idx)
{
	if (count() > idx)
	{
		clearSelection();
		setItemSelected(item(idx), true);
	}
}

/* qtvUIListDelegate */

#define ICON_OFFSET		2

qtvUIListDelegate::qtvUIListDelegate(qtv_ui_list_mode mode,int32_t logosize) :
	m_mode(mode),
	m_logosize(logosize),
	QStyledItemDelegate()
{

}

void qtvUIListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (m_mode == qui_list_normal)
		QStyledItemDelegate::paint(painter, option, index);
	else
	{
		qtvUIListItem* qitem = qvariant_cast<qtvUIListItem*>(index.data(Qt::UserRole));
		if (qitem)
		{
			QRect rect = option.rect;
			QRect recticon = rect;
			QFont fttext(painter->font());
			fttext.setFamily("Arial");
			rect.adjust(ICON_OFFSET, ICON_OFFSET, -ICON_OFFSET, -ICON_OFFSET);
			//recticon.setRight(recticon.left() + m_logosize);

			painter->save();

			if (option.state & QStyle::State_Selected)
			{
				painter->setBrush(QColor(0xE5, 0xE8, 0xE8));
				//painter->drawRoundedRect(rect, 2, 2);
				painter->drawRect(rect);
				painter->setBrush(Qt::NoBrush);
			}
			//painter->drawPixmap(recticon, qitem->_logo_pixmap());
			painter->drawPixmap(recticon.x() + ICON_OFFSET, recticon.y() + ICON_OFFSET, qitem->_logo_pixmap());
			rect.setLeft(rect.left() + m_logosize + (ICON_OFFSET << 2));

			if (option.state & QStyle::State_Selected)fttext.setBold(true);
			else fttext.setBold(false);
			painter->setFont(fttext);

			if (m_mode == qui_list_logo_title_1)
				painter->drawText(rect,Qt::AlignLeft | Qt::AlignVCenter, qitem->_title_0());
			else if (m_mode == qui_list_logo_title_2)
			{
				rect.setBottom(rect.top() + m_logosize / 2);
				painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, qitem->_title_0());
				rect.setTop(rect.bottom());
				rect.setBottom(rect.top() + m_logosize / 2);
				painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, qitem->_title_1());
			}

			painter->restore();

		}else QStyledItemDelegate::paint(painter, option, index);
	}
}

QSize qtvUIListDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QSize size = QStyledItemDelegate::sizeHint(option, index);
	if (m_mode != qui_list_normal)
		size.setHeight(m_logosize + (ICON_OFFSET << 2));
	return size;
}
