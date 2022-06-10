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

#ifndef QTV_CHART_DEFINE_H
#define QTV_CHART_DEFINE_H

#include <core/qtv_define.h>
#include <QColor>
#include <service/service_base.h>

#define QTVCHART_DATASERVICE_TIME		60

typedef enum _qtv_chart_mode
{
	qchart_mode_line,
	qchart_mode_bar
}qtv_chart_mode;

typedef enum _qtv_chartaxis_mode
{
	qchart_axis_hide,
	qchart_axis_static,
	qchart_axis_dynamic,
}qtv_chartaxis_mode;

typedef enum _qtv_chart_data
{
	qchart_data_linear_u32,
}qtv_chart_data;

class QTV_DECL qtvChartRange : public QObject
{
	Q_OBJECT
public:
	explicit qtvChartRange(int32_t min, int32_t max, uint32_t step) :
		m_min(min),m_max(max),m_step(step){}

	virtual int32_t _mix() { return m_min; }
	virtual int32_t _max() { return m_max; }
	virtual uint32_t _step() { return m_step; }
	virtual int32_t _length() { return m_max - m_min; }
	virtual int32_t _step_length() { return _length() / _step(); }
	
	virtual void setRange(int32_t min, int32_t max, uint32_t step);
	virtual void setConvert(uint32_t length, int64_t base);
	virtual qreal convertValue(int64_t value) = 0;

	static qtvChartRange* createFixedAxisH(int32_t min, int32_t max, int32_t step);
	static qtvChartRange* createFixedAxisV(int32_t min, int32_t max, int32_t step);

protected:
	int32_t m_min;
	int32_t m_max;
	uint32_t m_step;
	int64_t m_base;
	int32_t m_length;
};

class QTV_DECL qtvChartAxisBase : public QObject
{
	Q_OBJECT
public:
	explicit qtvChartAxisBase(QString name,QString unit, qtv_chartaxis_mode mode, qtvChartRange* range) :
		m_name(name), m_unit(unit), m_mode(mode), m_range(range) {}

	virtual QString& _name() { return m_name; }
	virtual QString& _unit() { return m_unit; }
	virtual qtv_chartaxis_mode _mode() { return m_mode; }
	virtual qtvChartRange* _range() { return m_range; }

	virtual QString getTitleString(int32_t nstep) = 0;

protected:
	QString m_name;
	QString m_unit;
	qtv_chartaxis_mode m_mode;
	qtvChartRange* m_range;
};

class QTV_DECL qtvChartData : public QObject
{
	Q_OBJECT
public:
	explicit qtvChartData(QColor color,const char* name, qtv_chart_data mode) :
		m_color(color),m_name(name),m_mode(mode){}

	QColor& _color() { return m_color; }
	QString& _name() { return m_name; }
	qtv_chart_data _mode() { return m_mode; }

protected:
	QColor m_color;
	QString m_name;
	qtv_chart_data m_mode;
};

class QTV_DECL qtvCharDataLinearU32 : public qtvChartData
{
	Q_OBJECT
public:
	explicit qtvCharDataLinearU32(QColor color, const char* name) :
		qtvChartData(color, name, qchart_data_linear_u32){}

	virtual int32_t getDataPosition(int32_t* len) = 0;
	virtual uint32_t getData() = 0;
	virtual void putData(uint32_t val) = 0;
	virtual void resetData() = 0;
};

class QTV_DECL qtvChartDataCapture : public QObject
{
	Q_OBJECT
public:
	explicit qtvChartDataCapture() {}
	~qtvChartDataCapture() { clearDatas(); }

	const QList<qtvChartData*>& _datas() { return m_datas; }
	virtual vatek_result startCapture() = 0;
	virtual void stopCapture() = 0;

Q_SIGNALS:
	void notifyDataUpdate();

protected:
	QList<qtvChartData*> m_datas;

	void pushData(qtvChartData* data) { m_datas.append(data); }
	void clearDatas()
	{
		while (m_datas.count())
		{
			delete m_datas.first();
			m_datas.removeFirst();
		}
	}
};

class QTV_DECL qtvChartDataCaptureService : public qtvChartDataCapture
{
	Q_OBJECT
public:
	virtual vatek_result pushBroadcast(Pbroadcast_info pinfo) = 0;
};

#endif
