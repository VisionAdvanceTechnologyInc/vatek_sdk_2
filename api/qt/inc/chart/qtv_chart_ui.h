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

#ifndef QTV_CHART_UI_H
#define QTV_CHART_UI_H

#include <chart/qtv_chart_define.h>
#include <core/qtv_service.h>

#include <QGraphicsView>
#include <QResizeEvent>
#include <QPaintEvent>

#define QTVCHART_RANGE_OFFECT		4
#define QTVCHART_FONT_MAXSIZE		20

class QTV_DECL qtvChartPattern : public QObject
{
	Q_OBJECT
public:
	explicit qtvChartPattern(qtvChartAxisBase* x, qtvChartAxisBase* y);
	~qtvChartPattern();

	virtual void setSize(QSize& size) = 0;
	virtual qtvChartAxisBase* _x_axis() { return m_xaxis; }
	virtual qtvChartAxisBase* _y_axis() { return m_yaxis; }

Q_SIGNALS :
	void notifyViewUdpate();

protected:
	qtvChartAxisBase* m_xaxis;
	qtvChartAxisBase* m_yaxis;

	static void drawChartBase(QPainter& p, QRect& rtchart,qtvChartAxisBase* axisx,qtvChartAxisBase* axisy);
	static void setFontSizeH(QPainter& p, int32_t h, int32_t maxfont = QTVCHART_FONT_MAXSIZE);
};


class qtvChartUIBase;
class QTV_DECL qtvChartUIProvider : public QObject
{
	Q_OBJECT
public:
	explicit qtvChartUIProvider(const char* name, qtvChartPattern* pattern) : 
		m_name(name),m_pattern(pattern),m_uiattach(0) {}
	~qtvChartUIProvider() { delete m_pattern; }

	virtual QString& _name() { return m_name; }
	virtual qtvChartPattern* _pattern() { return m_pattern; }

	virtual void attachUI() { m_uiattach = 1; }
	virtual void detachUI() { m_uiattach = 0; }
	virtual void refreshSize(QSize& size) { m_pattern->setSize(size); }
	virtual void drawWidget(QPainter& p) = 0;

Q_SIGNALS : 
	void notifyDataUpdate();

protected:
	QString m_name;
	qtvChartPattern* m_pattern;
	int32_t m_uiattach;
};

class QTV_DECL qtvChartUIProviderService : public qtvChartUIProvider
{
	Q_OBJECT
public:

	virtual void connectChart(qtvServiceBase* service) = 0;
	virtual void disconnectChart() = 0;

	static qtvChartUIProviderService* createServiceProvider(qtvServiceBase* service);

protected:
	explicit qtvChartUIProviderService(const char* name, qtvChartPattern* pattern) :
		qtvChartUIProvider(name, pattern) {}
};

class QTV_DECL qtvChartUIBase : public QWidget
{
	Q_OBJECT
public:
	explicit qtvChartUIBase(QWidget* parent = nullptr);
	~qtvChartUIBase();

	void resizeEvent(QResizeEvent* event) override;
	void paintEvent(QPaintEvent* event);

	void attachProvider(qtvChartUIProvider* provider);
	void detachProvider();

protected:
	QSize m_size;
	qtvChartUIProvider* m_provider;

protected Q_SLOTS:
	void recvProviderUpdate();
};

#endif