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

#include <chart/qtv_chart_ui.h>
#include <QPainterPath>
#include <QtMath>

/* qtvChartUIBase */

qtvChartUIBase::qtvChartUIBase(QWidget* parent) :
	m_provider(NULL)
{

}

qtvChartUIBase::~qtvChartUIBase()
{
	detachProvider();
}

void qtvChartUIBase::resizeEvent(QResizeEvent* event)
{
	if (event->size() != m_size)
	{
		m_size = event->size();
		if (m_provider)
			m_provider->refreshSize(m_size);
	}
}

void qtvChartUIBase::paintEvent(QPaintEvent* event)
{
	QPainter g(this);
	if (m_provider)m_provider->drawWidget(g);
		//g.drawImage(0, 0, m_provider->_pattern()->_chart_base());
	else g.fillRect(0, 0, m_size.width(), m_size.height(), Qt::black);
}

void qtvChartUIBase::attachProvider(qtvChartUIProvider* provider)
{
	if (m_provider)detachProvider();

	if (provider)
	{
		provider->refreshSize(m_size);
		connect(provider->_pattern(), SIGNAL(notifyViewUdpate()), this, SLOT(recvProviderUpdate()));
		provider->attachUI();
		m_provider = provider;
		QWidget::update();
	}
}

void qtvChartUIBase::detachProvider()
{
	if (m_provider)
	{
		disconnect(m_provider->_pattern(), SIGNAL(notifyViewUdpate()), this, SLOT(recvProviderUpdate()));
		m_provider->detachUI();
		m_provider = NULL;
		QWidget::update();
	}
}

void qtvChartUIBase::recvProviderUpdate()
{
	update();
}

/* qtvChartPattern */

qtvChartPattern::qtvChartPattern(qtvChartAxisBase* x, qtvChartAxisBase* y) :
	m_xaxis(x),
	m_yaxis(y)
{

}

qtvChartPattern::~qtvChartPattern()
{
	if (m_xaxis)delete m_xaxis;
	if (m_yaxis)delete m_yaxis;
}

void qtvChartPattern::drawChartBase(QPainter& p, QRect& rtchart, qtvChartAxisBase* axisx, qtvChartAxisBase* axisy)
{
	int32_t i = 0;
	for (i = 0; i < axisx->_range()->_step_length() + 1; i++)
	{
		qreal px = rtchart.left() + axisx->_range()->convertValue(i * axisx->_range()->_step());
		p.drawLine(QPointF(px, rtchart.top()), QPointF(px, rtchart.bottom()));
	}
	for (i = 0; i < axisy->_range()->_step_length() + 1; i++)
	{
		qreal py = rtchart.top() + axisy->_range()->convertValue(i * axisy->_range()->_step());
		p.drawLine(QPointF(rtchart.left(), py), QPointF(rtchart.right(), py));
	}
}

void qtvChartPattern::setFontSizeH(QPainter& p, int32_t h, int32_t maxfont)
{
	int32_t ftsize = 0;
	QFont newfont = p.font();
	newfont.setPixelSize(maxfont);
	newfont.setFamily("Arial");
	ftsize = newfont.pixelSize();

	while (ftsize)
	{
		QFontMetrics ftm(newfont);
		if (ftm.height() > h)
		{
			ftsize--;
			newfont.setPixelSize(ftsize);
		}
		else break;
	}
	p.setFont(newfont);
}
