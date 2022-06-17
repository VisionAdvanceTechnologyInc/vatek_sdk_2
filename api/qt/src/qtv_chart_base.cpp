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

/* qchart_draw_pingping */

qchartdraw_pingpong::qchartdraw_pingpong() :
	m_hthread(NULL),
	m_hevent(NULL),
	m_hlock(NULL),
	m_isrunning(0),
	m_imgptr(-1)
{
	cross_os_create_mutex(&m_hlock);
	cross_os_create_event(NULL, &m_hevent);
}

qchartdraw_pingpong::~qchartdraw_pingpong()
{
	cross_os_free_mutex(m_hlock);
	cross_os_free_event(m_hevent);
}

QImage& qchartdraw_pingpong::lockImage()
{
	cross_os_lock_mutex(m_hlock);
	if (m_imgptr == -1)return m_imagebase;
	else return m_imagepp[m_imgptr];
}

void qchartdraw_pingpong::unlockImage()
{
	cross_os_release_mutex(m_hlock);
}

vatek_result qchartdraw_pingpong::startAsyncDraw()
{
	vatek_result nres = vatek_badstatus;
	if (!m_hthread)
	{
		m_isrunning = 1;
		cross_os_lock_mutex(m_hlock);
		drawPingPong(0);
		cross_os_release_mutex(m_hlock);
		m_hthread = cross_os_create_thread(asyncDrawHandler, this);
		if (!m_hthread)nres = vatek_unknown;
		else nres = vatek_success;
	}
	return nres;
}

void qchartdraw_pingpong::stopAsyncDraw()
{
	if (m_hthread)
	{
		if (m_isrunning == 1)
		{
			m_isrunning = 2;
			while (m_isrunning == 2)
			{
				cross_os_raise_event(m_hevent);
				cross_os_sleep(10);
			}
			cross_os_free_thread(m_hthread);
		}
	}

	m_hthread = NULL;
	m_isrunning = 0;
}

void qchartdraw_pingpong::asyncDrawHandler(Pcross_thread_param param)
{
	qchartdraw_pingpong* pp = (qchartdraw_pingpong*)param->userparam;
	while (pp->m_isrunning == 1)
	{
		cross_os_wait_event(pp->m_hevent);
		if (pp->m_isrunning == 1)
		{
			cross_os_lock_mutex(pp->m_hlock);
			{
				if (pp->m_imgptr == -1)pp->drawPingPong(0);
				else pp->drawPingPong(!pp->m_imgptr);
			}
			cross_os_release_mutex(pp->m_hlock);
		}
	}
	pp->m_isrunning = 3;
}

void qchartdraw_pingpong::drawPingPong(int32_t idx)
{
	QPainter p(&m_imagepp[idx]);
	p.drawImage(0, 0, m_imagebase);
	drawFull(p);
	m_imgptr = idx;
}

/* qchart_pattern_base */

qchartpattern_base::qchartpattern_base(qtvChartAxisBase* x, qtvChartAxisBase* y) :
	m_uiattach(0),
	qtvChartPattern(x,y)
{

}

qchartpattern_base::~qchartpattern_base()
{

}

void qchartpattern_base::setUIAttach(int32_t attach)
{
	if (m_uiattach != attach)
	{
		m_uiattach = attach;
		if (!m_imagebase.isNull())redrawBase();
	}
}

void qchartpattern_base::setSize(QSize& size)
{
	if (!size.isEmpty() && m_imagebase.size() != size)
	{
		m_imagebase = QImage(size, QImage::Format_ARGB32);
		redrawBase();
	}
}

void qchartpattern_base::redrawBase()
{
	if (!m_imagebase.isNull())
	{
		cross_os_lock_mutex(m_hlock);
		drawBase(QPainter(&m_imagebase), m_imagebase.size());
		m_imagepp[0] = m_imagebase.copy();
		m_imagepp[1] = m_imagebase.copy();
		m_imgptr = -1;
		cross_os_release_mutex(m_hlock);
	}
}

void qchartpattern_base::setAxisX(qtvChartAxisBase* x)
{
	if (m_xaxis)delete m_xaxis;
	m_xaxis = x;
}

void qchartpattern_base::setAxisY(qtvChartAxisBase* y)
{
	if (m_yaxis)delete m_yaxis;
	m_yaxis = y;
}

void qchartpattern_base::drawTitleX(QPainter& p, qtvChartAxisBase* x, QRect& rt, const char* strmax,QColor clr, int32_t flags)
{
	int32_t i = 2;
	int32_t step = rt.width() / x->_range()->_step_length();
	QFontMetrics ftmet(p.font());
	int32_t fw = (((ftmet.width(strmax) + 1) >> 1) << 1);
	int32_t fh = ftmet.height();
	int32_t noffect = (fw + step - 1) / step;
	QRect rttitle(0, rt.bottom() + 1, fw, fh);

	p.setPen(clr);
	while (i < x->_range()->_step_length())
	{
		int32_t value = i * x->_range()->_step();
		rttitle.moveLeft(rt.left() + x->_range()->convertValue(value) - (fw >> 1));
		//p.fillRect(rttitle, Qt::lightGray);
		p.drawText(rttitle, flags, x->getTitleString(i));
		i += noffect;
	}
}

void qchartpattern_base::drawTitleY(QPainter& p, qtvChartAxisBase* y, QRect& rt, const char* strmax,QColor clr, int32_t flags)
{
	int32_t i = 1;
	int32_t step = rt.height() / y->_range()->_step_length();
	QFontMetrics ftmet(p.font());
	int32_t fw = ftmet.width(strmax);
	int32_t fh = ftmet.height();
	int32_t noffect = (fh + (step - 1)) / step;
	QRect rttitle(rt.left() - fw, 0, fw, fh);

	p.setPen(Qt::yellow);
	while (i < y->_range()->_step_length())
	{
		int32_t value = i * y->_range()->_step();
		rttitle.moveTop(y->_range()->convertValue(value) + (fh >> 1));
		//p.fillRect(rttitle, Qt::lightGray);
		p.drawText(rttitle, flags, y->getTitleString(i));
		i += noffect;
	}
}

/* qtvChartRange */

void qtvChartRange::setConvert(uint32_t length, int64_t base)
{
	m_length = length;
	m_base = base;
}

void qtvChartRange::setRange(int32_t min, int32_t max, uint32_t step)
{
	m_min = min;
	m_max = max;
	m_step = step;
	setConvert(m_length, m_base);
}

qtvChartRange* qtvChartRange::createFixedAxisH(int32_t min, int32_t max, int32_t step)
{
	return new qchartrange_fixed_H(min, max, step);
}

qtvChartRange* qtvChartRange::createFixedAxisV(int32_t min, int32_t max, int32_t step)
{
	return new qchartrange_fixed_V(min, max, step);
}

/* qchart_data_linear_u32 */

qchartdata_linear_u32::qchartdata_linear_u32(QColor color, const char* name, int32_t size) :
	m_lock(NULL),
	m_ptrr(0),
	m_ptrw(0),
	m_len(size),
	m_datalen(size << 1),
	m_count(0),
	m_data(NULL),
	qtvCharDataLinearU32(color,name)
{
	if (!is_vatek_success(cross_os_create_mutex(&m_lock)))
		VWAR("create data mutex fail");
	m_data = (uint32_t*)malloc(sizeof(uint32_t) * m_datalen);
	if(m_data)
		memset(m_data, 0, sizeof(uint32_t) * m_datalen);
}

qchartdata_linear_u32::~qchartdata_linear_u32()
{
	cross_os_free_mutex(m_lock);
	free(m_data);
	m_lock = NULL;
}

int32_t qchartdata_linear_u32::getDataPosition(int32_t* len)
{
	cross_os_lock_mutex(m_lock);
	if (m_count > m_len)
	{
		*len = m_len;
		m_ptrr = m_ptrw - m_len;
		if (m_ptrr < 0)m_ptrr = m_datalen + m_ptrr;
	}
	else
	{
		*len = (int32_t)m_count;
		m_ptrr = 0;
	}
	cross_os_release_mutex(m_lock);
	return m_ptrr;
}

uint32_t qchartdata_linear_u32::getData()
{
	uint32_t val = 0;
	if (m_ptrr >= 0)
	{
		val = m_data[m_ptrr++];
		if (m_ptrr >= m_datalen)
			m_ptrr = 0;
	}
	return val;
}

void qchartdata_linear_u32::putData(uint32_t val)
{
	m_data[m_ptrw] = val;
	m_ptrw++;
	cross_os_lock_mutex(m_lock);
	if (m_ptrw >= m_datalen)m_ptrw = 0;
	m_count++;
	cross_os_release_mutex(m_lock);
}

void qchartdata_linear_u32::resetData()
{
	cross_os_lock_mutex(m_lock);
	m_ptrw = 0;
	m_ptrr = -1;
	m_count = 0;
	cross_os_release_mutex(m_lock);
}