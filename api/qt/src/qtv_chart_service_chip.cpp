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

#include "internal/qinternal_chart.h"
#include <core/ui/ui_props/ui_props_modulator.h>
#include <QPainterPath>

/* qtvChartUIProviderService */

qchartprovider_service::qchartprovider_service() :
	m_spattern(NULL),
	qtvChartUIProviderService("Device", new qchartpattern_service())
{
	m_spattern = static_cast<qchartpattern_service*>(m_pattern);
}

qchartprovider_service::~qchartprovider_service()
{
	disconnectChart();
}

void qchartprovider_service::connectChart(qtvServiceBase* service)
{
	m_spattern->attachService(service);
}

void qchartprovider_service::disconnectChart()
{
	m_spattern->attachService(NULL);
}

void qchartprovider_service::attachUI()
{
	qtvChartUIProvider::attachUI();
	m_spattern->setUIAttach(1);
}

void qchartprovider_service::detachUI()
{
	qtvChartUIProvider::detachUI();
	m_spattern->setUIAttach(0);
}


void qchartprovider_service::drawWidget(QPainter& p)
{
	QImage img = m_spattern->lockImage();
	p.drawImage(QPoint(0, 0), img);
	m_spattern->unlockImage();
}

/* qchart_pattern_service */

#define QCHART_DEF_MAXBITRATE		50000000
#define QCHART_DEF_MAXSTEP			5000000
#define QCHART_DEF_BASEBITRATE		10000000
#define QCHART_DEF_BASESTEP			1000000

#define QCHART_MIN_TITLELEN			90

qchartpattern_service::qchartpattern_service() :
	m_service(NULL),
	m_data(NULL),
	qchartpattern_base(getAxisX(QTVCHART_DATASERVICE_TIME), getAxisY(QCHART_DEF_MAXBITRATE, QCHART_DEF_MAXSTEP))
{

}

qchartpattern_service::~qchartpattern_service()
{

}

void qchartpattern_service::checkAsyncStatus()
{
	int32_t isactive = (m_service && m_service->_status() == qstatus_running && m_uiattach);
	if (isactive)
	{
		if (!m_isrunning)startAsyncDraw();
	}
	else
	{
		if (m_isrunning)stopAsyncDraw();
	}
	emit notifyViewUdpate();
}

void qchartpattern_service::attachService(qtvServiceBase* service)
{
	if (service != m_service)
	{
		if (m_service)
		{
			if (m_data)
				disconnect(m_data, SIGNAL(notifyDataUpdate(void)), this, SLOT(recvCaptureDataChanged()));
			disconnect(m_service, SIGNAL(notifyStatusChanged(qtv_status)), this, SLOT(recvStatusChanged(qtv_status)));
			m_service = NULL;
			m_data = NULL;
		}

		if (service)
		{
			connect(service, SIGNAL(notifyStatusChanged(qtv_status)), this, SLOT(recvStatusChanged(qtv_status)));
			m_service = service;
			m_data = m_service->_chart_data();
			if(m_data)
				connect(m_data, SIGNAL(notifyDataUpdate(void)), this, SLOT(recvCaptureDataChanged()));

		}
		redrawBase();
		checkAsyncStatus();
	}
}

void qchartpattern_service::recvStatusChanged(qtv_status status)
{
	redrawBase();
	checkAsyncStatus();
}

void qchartpattern_service::recvCaptureDataChanged()
{
	if (m_isrunning)
		cross_os_raise_event(m_hevent);
}

void qchartpattern_service::drawFull(QPainter& p)
{
	if (m_service && m_service->_status() == qstatus_running)
	{
		Pbroadcast_info pinfo = m_service->_broadcast_detail();
		Pqtv_output_param pout = m_service->_output_param();
		qchartcapture_service* datas = static_cast<qchartcapture_service*>(m_data);
		int32_t phitem = m_rtitems.height() / 6;
		int32_t positemy = m_rtitems.top();
		QRect rtitem;
		qreal pty = m_yaxis->_range()->convertValue(pinfo->mod_bitrate) + m_rtclient.top();
		QString szout = QString::asprintf("%s - %d - %d",
										  ui_enum_get_str(modulator_type, pout->modulation.type),
										  pout->r2param.freqkhz,
										  pinfo->mod_bitrate);

		setFontSizeH(p, phitem, 12);
		phitem = p.fontMetrics().height();
		p.setPen(Qt::green);
		p.drawLine(QPointF(m_rtclient.left(), pty), QPointF(m_rtclient.right(), pty));

		drawdata(p, datas->_mux_data(), QPoint(m_rtitems.left(), positemy), phitem, pinfo->mux_bitrate);
		positemy += phitem << 1;
		drawdata(p, datas->_current_data(), QPoint(m_rtitems.left(), positemy), phitem, pinfo->cur_bitrate);
		positemy += phitem << 1;
		drawdata(p, datas->_source_data(), QPoint(m_rtitems.left(), positemy), phitem, pinfo->data_bitrate);

		setFontSizeH(p, pty - m_rtchart.top(), 12);
		p.setPen(Qt::green);
		//p.drawText(QPointF(m_rtclient.left(), pty - 2), szout);
		p.drawText(QPointF(m_rtclient.left(),m_rtclient.top() + p.fontMetrics().height()), szout);

		emit notifyViewUdpate();
	}
}

void qchartpattern_service::drawdata(QPainter& p, qchartdata_linear_u32* data, QPoint& pt, int32_t phitem, uint32_t brate)
{
	int32_t ns = phitem;
	int32_t of = 0;
	int32_t py = 0;
	QPainterPath path;
	int32_t nline = QTVCHART_DATASERVICE_TIME;
	int32_t i = 0;

	if (ns > 5)ns = 5;
	of = (phitem - ns) >> 1;
	py = pt.y() + p.fontMetrics().height();

	p.setBrush(data->_color());
	p.setPen(data->_color());
	p.drawRect(QRect(pt.x(), py, ns, ns));
	p.drawText(QPoint(pt.x() + phitem, py), data->_name());
	p.drawText(QPoint(pt.x() + phitem, py + p.fontMetrics().height()), QString::asprintf("%d", brate));

	i = data->getDataPosition(&nline);

	if (nline)
	{
		qreal px = 0;
		qreal py = 0;
		for (i = 0; i < nline; i++)
		{
			px = m_rtclient.left() + m_xaxis->_range()->convertValue(i);
			py = m_rtclient.top() + m_yaxis->_range()->convertValue(data->getData());
			if (i == 0)path.moveTo(QPointF(px, py));
			else
			{
				QPointF pt(px, py);
				path.lineTo(pt);
				path.moveTo(pt);
			}
		}

		if (nline == QTVCHART_DATASERVICE_TIME)
			path.lineTo(QPointF(m_rtclient.right(), py));
		p.drawPath(path);
	}
}

void qchartpattern_service::drawBase(QPainter& p, QSize& size)
{
	if (m_service)
	{
		Pbroadcast_info pinfo = m_service->_broadcast_detail();
		if (m_service->_status() == qstatus_running)
		{
			uint32_t nbitrate = (pinfo->mod_bitrate / QCHART_DEF_BASESTEP);
			uint32_t nstep = QCHART_DEF_BASESTEP;
			nbitrate = (nbitrate + 5) * QCHART_DEF_BASESTEP;
			while ((nbitrate / nstep) > 10)nstep += QCHART_DEF_BASESTEP;

			nbitrate = ((nbitrate / nstep) + 1) * nstep;
			setAxisY(getAxisY(nbitrate, nstep));
		}
		else setAxisY(getAxisY(QCHART_DEF_MAXBITRATE, QCHART_DEF_MAXSTEP));
	}
	else setAxisY(getAxisY(QCHART_DEF_MAXBITRATE, QCHART_DEF_MAXSTEP));

	refreshRect(size);
	p.fillRect(m_imagebase.rect(), Qt::black);
#if 0
	p.fillRect(m_rtchart, Qt::lightGray);
	p.fillRect(m_rtclient, Qt::red);
	p.fillRect(m_rtitems, Qt::blue);
#endif
	p.setPen(Qt::darkGray);
	drawChartBase(p, m_rtclient, m_xaxis, m_yaxis);
	setFontSizeH(p, m_rtchart.bottom() - m_rtclient.bottom(), 10);
	reDrawTitleX(p);
	reDrawTitleY(p);
}

void qchartpattern_service::refreshRect(QSize& size)
{
	int32_t w = size.width() - QCHART_MIN_TITLELEN;
	int32_t h = (size.height() * 95) / 100;
	int32_t stepw = m_xaxis->_range()->_step_length() + 2;
	int32_t steph = m_yaxis->_range()->_step_length() + 1;
	int32_t chartw = (w / stepw) * stepw;
	int32_t charth = (h / steph) * steph;

	m_rtitems = QRect(4, m_rtclient.top(), QCHART_MIN_TITLELEN, m_rtclient.height());
#if 0
	if (m_rtitems.width() < QCHART_MIN_TITLELEN)
	{
		m_rtitems = QRect(4, m_rtclient.top(), QCHART_MIN_TITLELEN, m_rtclient.height());
		w = size.width() - QCHART_MIN_TITLELEN - 4;
		chartw = (w / stepw) * stepw;
	}
#endif

	m_rtchart = QRect(size.width() - w, size.height() - charth >> 1, size.width() - (size.width() - w) - 4, charth);
	
	stepw = (chartw / stepw);
	steph = charth / steph;
	m_rtclient = QRect(m_rtchart.right() - chartw, m_rtchart.top() + steph/2, chartw - stepw, charth - (steph <<1) );
	

	m_xaxis->_range()->setConvert(m_rtclient.width(), 0);
	m_yaxis->_range()->setConvert(m_rtclient.height(), 0);
}

void qchartpattern_service::reDrawTitleX(QPainter& p)
{
	drawTitleX(p, m_xaxis, m_rtclient, "0000",Qt::yellow, Qt::AlignHCenter | Qt::AlignVCenter);
}

void qchartpattern_service::reDrawTitleY(QPainter& p)
{
	drawTitleY(p, m_yaxis, m_rtclient, "000", Qt::yellow, Qt::AlignHCenter | Qt::AlignVCenter);
}

qtvChartAxisBase* qchartpattern_service::getAxisX(int32_t timems)
{
	return new qchartaxis_time_h("time", "ms", qchart_axis_hide, qtvChartRange::createFixedAxisH(0, timems, 1));
}

qtvChartAxisBase* qchartpattern_service::getAxisY(int32_t maxbitrate, int32_t stepbitrate)
{
	return new qchartaxis_bitrate_v("bitrate", "Mbps", qchart_axis_static, qtvChartRange::createFixedAxisV(0, maxbitrate, stepbitrate));
}

/* qcharcapture_service */

qchartcapture_service::qchartcapture_service() :
	m_datamux(NULL),
	m_datasource(NULL),
	m_datacur(NULL)
{

}

qchartcapture_service::~qchartcapture_service()
{
	stopCapture();
}

vatek_result qchartcapture_service::startCapture()
{
	vatek_result nres = vatek_badstatus;
	if (!m_datas.count())
	{
		m_datamux = new qchartdata_linear_u32(Qt::cyan, "mux", QTVCHART_DATASERVICE_TIME);
		m_datasource = new qchartdata_linear_u32(Qt::magenta, "data", QTVCHART_DATASERVICE_TIME);
		m_datacur = new qchartdata_linear_u32(Qt::red, "current", QTVCHART_DATASERVICE_TIME);
		pushData(m_datamux);
		pushData(m_datasource);
		pushData(m_datacur);
	}
	return vatek_success;
}

void qchartcapture_service::stopCapture()
{
	if (m_datas.count())clearDatas();
	m_datamux = NULL;
	m_datasource = NULL;
	m_datacur = NULL;
}

vatek_result qchartcapture_service::pushBroadcast(Pbroadcast_info pinfo)
{
	vatek_result nres = vatek_badstatus;
	if (m_datas.count())
	{
		m_datamux->putData(pinfo->mux_bitrate);
		m_datasource->putData(pinfo->data_bitrate);
		m_datacur->putData(pinfo->cur_bitrate);
		emit notifyDataUpdate();
		nres = vatek_success;
	}
	return nres;
}

/*  qtvChartUIProviderService */

qtvChartUIProviderService* qtvChartUIProviderService::createServiceProvider(qtvServiceBase* service)
{
	qchartprovider_service* newprovider = new qchartprovider_service();
	if (service)newprovider->connectChart(service);
	return newprovider;
}