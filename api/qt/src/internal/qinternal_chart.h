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

#ifndef QINTERNAL_CHART_H
#define QINTERNAL_CHART_H

#include <chart/qtv_chart_ui.h>
#include <cross/cross_os_api.h>

class qchartaxis_time_h : public qtvChartAxisBase
{
public:
	explicit qchartaxis_time_h(QString name, QString unit, qtv_chartaxis_mode mode, qtvChartRange* range) :
		qtvChartAxisBase(name,unit,mode,range){}

	QString getTitleString(int32_t nstep) override
	{
		return QString::asprintf("%d", nstep * _range()->_step());
	}
};

class qchartaxis_bitrate_v : public qtvChartAxisBase
{
public:
	explicit qchartaxis_bitrate_v(QString name, QString unit, qtv_chartaxis_mode mode, qtvChartRange* range) :
		qtvChartAxisBase(name, unit, mode, range) {}

	QString getTitleString(int32_t nstep) override
	{
		return QString::asprintf("%d", (nstep * _range()->_step())/1000000);
	}
};


class qchartrange_fixed_base : public qtvChartRange
{
public:
	qchartrange_fixed_base(int32_t min, int32_t max, int32_t step) : qtvChartRange(min, max, step) {}

	void setConvert(uint32_t length, int64_t base)
	{
		qtvChartRange::setConvert(length, base);
		m_valuerange = m_max - m_min;
	}

protected:
	int32_t m_valuerange;
};

class qchartrange_fixed_H : public qchartrange_fixed_base
{
public:
	qchartrange_fixed_H(int32_t min, int32_t max, int32_t step) : qchartrange_fixed_base(min, max, step) {}

	qreal convertValue(int64_t value)
	{
		int64_t nstep = value - m_base;
		return (qreal)(m_length * nstep) / m_valuerange;
	}
};

class  qchartrange_fixed_V : public qchartrange_fixed_base
{
public:
	qchartrange_fixed_V(int32_t min, int32_t max, int32_t step) : qchartrange_fixed_base(min, max, step) {}

	qreal convertValue(int64_t value)
	{
		int64_t nstep = value - m_base;
		return (qreal)m_length - (qreal)(m_length * nstep) / m_valuerange;
	}
};

class qchartdata_linear_u32 : public qtvCharDataLinearU32
{
public:
	explicit qchartdata_linear_u32(QColor color, const char* name, int32_t size);
	~qchartdata_linear_u32();

	int32_t getDataPosition(int32_t* len) override;
	uint32_t getData() override;
	void putData(uint32_t val) override;
	void resetData() override;

protected:
	hcross_mutex m_lock;
	int32_t m_ptrw;
	int32_t m_ptrr;
	int32_t m_len;
	int32_t m_datalen;
	uint32_t m_count;
	uint32_t* m_data;
};

class qchartdraw_pingpong
{
public:
	explicit qchartdraw_pingpong();
	~qchartdraw_pingpong();

	QImage& lockImage();
	void unlockImage();

protected:
	hcross_mutex m_hlock;
	hcross_thread m_hthread;
	hcross_event m_hevent;
	int32_t m_isrunning;
	QImage m_imagepp[2];
	int32_t m_imgptr;
	QImage m_imagebase;

	virtual vatek_result startAsyncDraw();
	virtual void stopAsyncDraw();

	virtual void drawBase(QPainter& p, QSize& size) = 0;
	virtual void drawFull(QPainter& p) = 0;

	void drawPingPong(int32_t idx);

	static void asyncDrawHandler(Pcross_thread_param param);
};

class qchartpattern_base : public qtvChartPattern, public qchartdraw_pingpong
{
public:
	explicit qchartpattern_base(qtvChartAxisBase* x, qtvChartAxisBase* y);
	~qchartpattern_base();
	void setSize(QSize& size) override;

	void setAxisX(qtvChartAxisBase* x);
	void setAxisY(qtvChartAxisBase* y);
	void setUIAttach(int32_t attach);

protected:
	int32_t m_uiattach;

	void redrawBase();

	static void drawTitleX(QPainter& p, qtvChartAxisBase* x, QRect& rt, const char* strmax,QColor clr,int32_t flags);
	static void drawTitleY(QPainter& p, qtvChartAxisBase* y, QRect& rt, const char* strmax,QColor clr, int32_t flags);
};

class qchartcapture_service : public qtvChartDataCaptureService
{
public:
	explicit qchartcapture_service();
	~qchartcapture_service();

	qchartdata_linear_u32* _mux_data() { return m_datamux; }
	qchartdata_linear_u32* _source_data() { return m_datasource; }
	qchartdata_linear_u32* _current_data() { return m_datacur; }

	vatek_result startCapture() override;
	void stopCapture() override;
	vatek_result pushBroadcast(Pbroadcast_info pinfo) override;

protected:
	qchartdata_linear_u32* m_datamux;
	qchartdata_linear_u32* m_datasource;
	qchartdata_linear_u32* m_datacur;
};

class qchartpattern_service : public qchartpattern_base
{
	Q_OBJECT
public:
	explicit qchartpattern_service();
	~qchartpattern_service();

	void attachService(qtvServiceBase* service);
	void drawFull(QPainter& p) override;

protected:
	qtvServiceBase* m_service;
	qtvChartDataCaptureService* m_data;
	QRect m_rtchart;
	QRect m_rtclient;
	QRect m_rtitems;

	void checkAsyncStatus();

	void drawBase(QPainter& p, QSize& size) override;
	void refreshRect(QSize& size);
	void reDrawTitleX(QPainter& p);
	void reDrawTitleY(QPainter& p);

	void drawdata(QPainter& p,qchartdata_linear_u32* data, QPoint& pt, int32_t phitem,uint32_t brate);

	static qtvChartAxisBase* getAxisX(int32_t timems);
	static qtvChartAxisBase* getAxisY(int32_t maxbitrate, int32_t stepbitrate);

protected Q_SLOTS:
	void recvStatusChanged(qtv_status status);
	void recvCaptureDataChanged();
};

class qchartprovider_service : public qtvChartUIProviderService
{
public:
	explicit qchartprovider_service();
	~qchartprovider_service();

	void attachUI() override;
	void detachUI() override;

	/* qtvChartUIProvderService */
	void connectChart(qtvServiceBase* service) override;
	void disconnectChart() override;
	void drawWidget(QPainter& p) override;

protected:
	qchartpattern_service* m_spattern;
};

#endif
