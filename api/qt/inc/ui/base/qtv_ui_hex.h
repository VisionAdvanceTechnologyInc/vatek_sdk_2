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

#ifndef QTV_UI_HEX_H
#define QTV_UI_HEX_H

#include <QAbstractScrollArea>
#include <QSize>
#include <QImage>

typedef enum _qtv_hex_view
{
	qhex_view_raw,
	qhex_view_raw_addr,
}qtv_hex_view;

class qtvUIHexWidget : public QAbstractScrollArea
{
	Q_OBJECT
public:
	explicit qtvUIHexWidget(QWidget* parent = nullptr);
	virtual ~qtvUIHexWidget();

	void setHexView(qtv_hex_view view, int32_t perlinelen);
	void setHexBuffer(uint8_t* pbuf, int32_t len);

protected:
	int32_t m_cWidth;
	int32_t m_cHeight;
	qtv_hex_view m_view;
	int32_t m_prebytes;
	QImage m_baseimage;
	uint8_t* m_ptrbuf;
	int32_t m_buflen;

	void redrawBase();
	void resizeEvent(QResizeEvent* event);

	void paintEvent(QPaintEvent* event);
	void keyPressEvent(QKeyEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);

};

#endif
