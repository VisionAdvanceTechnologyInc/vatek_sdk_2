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

#include <ui/base/qtv_ui_hex.h>
#include <QPainter>
#include <QPoint>

/* qtvUIHexWidget */

#define UIHEX_ADDRESS_LEN	8
#define UIHEX_APPEND		1

qtvUIHexWidget::qtvUIHexWidget(QWidget* parent) :
	QAbstractScrollArea(parent),
	m_prebytes(0),
	m_ptrbuf(NULL),
	m_buflen(0),
	m_view(qhex_view_raw)
{
	setFont(QFont("Courier", 10));
	m_cWidth = fontMetrics().horizontalAdvance(QLatin1Char('9')) + (UIHEX_APPEND << 1);
	m_cHeight = fontMetrics().height() + (UIHEX_APPEND << 1);
	setFocusPolicy(Qt::StrongFocus);
	setHexView(qhex_view_raw_addr, 16);
}

qtvUIHexWidget::~qtvUIHexWidget()
{

}

void qtvUIHexWidget::setHexView(qtv_hex_view view, int32_t perlinelen)
{
	m_view = view;
	m_prebytes = perlinelen;
	redrawBase();
}

void qtvUIHexWidget::setHexBuffer(uint8_t* pbuf, int32_t len)
{
	m_ptrbuf = NULL;
	if (pbuf)
	{
		m_ptrbuf = pbuf;
		m_buflen = len;
	}
	else m_buflen = 0;
	redrawBase();
}

void qtvUIHexWidget::resizeEvent(QResizeEvent* event)
{
	redrawBase();
}

void qtvUIHexWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(viewport());
	if (!m_baseimage.isNull())
	{
		painter.setBrush(QBrush(Qt::lightGray));
		painter.drawRect(QRect(0, 0, viewport()->width(), viewport()->height()));
		painter.drawImage(QPoint(0, 0), m_baseimage);
	}
}

void qtvUIHexWidget::keyPressEvent(QKeyEvent* event)
{

}

void qtvUIHexWidget::mouseMoveEvent(QMouseEvent* event)
{

}

void qtvUIHexWidget::mousePressEvent(QMouseEvent* event)
{

}

void qtvUIHexWidget::redrawBase()
{
	if (m_buflen)
	{
		int32_t nh = ((m_buflen + (m_prebytes - 1)) / m_prebytes) * m_cHeight;
		int32_t nw = (m_cWidth * m_prebytes) + (m_prebytes << 1);

		if (m_view == qhex_view_raw_addr)
			nw += (m_cWidth * (UIHEX_ADDRESS_LEN + 1));
		m_baseimage = QImage(QSize(nw, nh), QImage::Format_ARGB32);
	}
	else m_baseimage = QImage(size(), QImage::Format_ARGB32);

	if (!m_baseimage.isNull())
	{
		QPainter pt(&m_baseimage);
		pt.setFont(QFont("Courier", 10));

		pt.setBrush(QBrush(Qt::white));
		pt.drawRect(QRect(0, 0, m_baseimage.width(), m_baseimage.height()));
		pt.setBrush(QBrush(QColor(0xF2D036)));
		pt.drawRect(QRect(0, 0, m_cWidth * (UIHEX_ADDRESS_LEN + 1), m_baseimage.height()));
	}
	update();
}
