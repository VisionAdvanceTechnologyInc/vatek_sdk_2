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

#include "vatek_toolkit.h"
#include <ui/qtv_ui_service_base.h>
#include <core/qtv_device.h>
#include <QScreen>
#include <qDebug>
#include <windows.h>
#include <conio.h>
#define TOOLKIT_TIMEOUT_MS		1000

/* vatek_toolkit */

vatek_toolkit::vatek_toolkit(QWidget* parent) :
	m_uimain(this),
	m_timemode(tktimer_null),
	m_source(NULL),
	m_chipservice(NULL),
	m_muxhandle(NULL),
	QMainWindow(parent)
{
	ui.setupUi(this);
	ui.deviceTabs->attachUIMainTab(&m_uimain);
	startTimer(tktimer_waitsource);
	statusBar()->setSizeGripEnabled(false);
	
	QScreen* screen = QGuiApplication::primaryScreen();     //螢幕大小
	QRect mm = screen->availableGeometry();
	int screen_width = mm.width();
	int screen_height = mm.height();
	qDebug() << screen_width << screen_height;

	HDC hd = GetDC(NULL);                                           // 獲取DPI
	int horDPI = GetDeviceCaps(hd, LOGPIXELSX);
	int verticalDPI = GetDeviceCaps(hd, LOGPIXELSY);
	double objectRate = horDPI / 96.0;

	setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
	
	if (screen_width >= 1920) {
		if (objectRate >= 1.5) {
			setMinimumSize(screen_width, screen_height-40);
		}
		else {
			setMinimumSize(screen_width / 1.5, screen_height / 1.3);
		}
	}
	else {
		setMinimumSize(screen_width, screen_height-40);
	}

	//int32_t is_stop = 0;

	//while (!is_stop) {
	//	static uint32_t run_tick = 0;
	//	uint32_t stop_tick = vatek_get_tick_ms();

	//	if (stop_tick - run_tick >= 100000) {
	//		recvTimerCheckDevice();
	//		run_tick = stop_tick;

	//	}
	//}
		
}

vatek_toolkit::~vatek_toolkit()
{
	stopTimer();
	if (m_chipservice)
	{
		if (m_chipservice->_status() == qstatus_running)
			m_chipservice->stop();
		closeService();
		qtvDeviceManage::closeDevice(m_device,0);
	}
	ui.deviceTabs->detachUIMainTab();
}

void vatek_toolkit::startTimer(toolkit_timer tktimer)
{
	vatek_result nres = vatek_success;
	if (m_timemode != tktimer_null)stopTimer();
	if (tktimer == tktimer_waitsource)
		connect(&m_timer, SIGNAL(timeout()), this, SLOT(recvTimerCheckDevice()));
	else if (tktimer == tktimer_update)
		connect(&m_timer, SIGNAL(timeout), this, SLOT(recvTimerUpdate()));
	else nres = vatek_badparam;

	if (is_vatek_success(nres))
	{
		m_timemode = tktimer;
		m_timer.start(TOOLKIT_TIMEOUT_MS);
	}
}

void vatek_toolkit::stopTimer()
{
	if (m_timemode != tktimer_null)
	{
		m_timer.stop();
		if(m_timemode == tktimer_waitsource)
			disconnect(&m_timer, SIGNAL(timeout()), this, SLOT(recvTimerCheckDevice()));
		else if(m_timemode == tktimer_update)
			disconnect(&m_timer, SIGNAL(timeout), this, SLOT(recvTimerUpdate()));
	}
	m_timemode = tktimer_null;
}

vatek_result vatek_toolkit::openService(qtvServiceBase* service)
{
	vatek_result nres = vatek_success;
	if (m_chipservice)closeService();
	{
		m_chipservice = qobject_cast<qtvServiceChipBase*>(service);
		if (m_chipservice)
		{
			m_uimain.setService(m_chipservice);
			nres = vatek_success;
		}
	}
	return nres;
}

void vatek_toolkit::closeService()
{
	if (m_chipservice)
	{
		m_uimain.setService(NULL);
		delete m_chipservice;
	}
	m_chipservice = NULL;
}

vatek_result vatek_toolkit::startBroadcast(qtvSourceBase* source, qtvMuxServiceBase* muxhandle)
{
	vatek_result nres = vatek_badstatus;
	if (m_chipservice)
	{
		m_source = source;
		m_muxhandle = muxhandle;
		nres = m_chipservice->startBroadcast(m_source, m_muxhandle);
	}
	return nres;
}

void vatek_toolkit::stopBroadcast()
{
	vatek_result nres = vatek_success;
	if (m_chipservice)
	{
		if (m_chipservice->_status() == qstatus_running)
			nres = m_chipservice->stop();
	}
	closeService();
	qtvDeviceManage::closeDevice(m_device, 1);
	if (m_muxhandle)delete m_muxhandle;
	if (m_source)delete m_source;
	m_muxhandle = NULL;
	m_source = NULL;
	m_device = NULL;

	startTimer(tktimer_waitsource);
}

void vatek_toolkit::recvTimerCheckDevice()
{
	vatek_result nres = qtvDeviceManage::enumDevices(DEVICE_BUS_ALL, service_unknown);
	if (nres > vatek_success)
	{
		qtvDevice* device = NULL;
		nres = qtvDeviceManage::openDevice(0, &device);
		if (is_vatek_success(nres))
		{
			qtvServiceBase* service;
			nres = qtvServiceBase::createService(device, &service);
			if (is_vatek_success(nres))
			{
				nres = openService(service);
				if (!is_vatek_success(nres))delete service;
			}
			if (!is_vatek_success(nres))qtvDeviceManage::closeDevice(device, 0);
			else m_device = device;
		}

		if (is_vatek_success(nres))stopTimer();
		else VWAR("open device and create service fail : %d", nres);
	}
}

void vatek_toolkit::recvTimerUpdate()
{

}

/* UIMainToolKit */

vatek_toolkit::UIMainToolKit::UIMainToolKit(vatek_toolkit* toolkit) :
	m_toolkit(toolkit)
{

}

bool vatek_toolkit::UIMainToolKit::startBroadcast()
{
	qtvUIWizardBroadcast* bcwizard = NULL;
	vatek_result nres = qtvUIWizardBroadcast::createWizard(m_toolkit->_service(), &bcwizard, m_toolkit);
	if (is_vatek_success(nres))
	{
		nres = bcwizard->startWizard();
		if (is_vatek_success(nres))
		{
			qtvSourceBase* source = NULL;
			qtvMuxServiceBase* muxhandle = NULL;
			bcwizard->getResult(NULL, &source, &muxhandle);
			if (source)nres = m_toolkit->startBroadcast(source, muxhandle);
			else nres = vatek_unsupport;
		}


		qtvUIWizardBroadcast::freeWizard(bcwizard);
	}
	if (is_vatek_success(nres))return true;
	return false;
}

void vatek_toolkit::UIMainToolKit::startCalibration()
{
	calibrationDialig newdlg(this,m_toolkit);
	newdlg.exec();
}

bool vatek_toolkit::UIMainToolKit::startUpdate()
{
	return false;
}

void vatek_toolkit::UIMainToolKit::stop()
{
	m_toolkit->stopBroadcast();
}

/* calibrationDialig */

calibrationDialig::calibrationDialig(qtvUIMain* main, QWidget* parent) :
	QDialog(parent)
{
	setFixedSize(QSize(600,400));
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setWindowTitle("calibration");
	m_tabbase = new qtvUITabCalibration(main, this);
	setLayout(m_tabbase->layout());
}

calibrationDialig::~calibrationDialig()
{
	delete m_tabbase;
}
