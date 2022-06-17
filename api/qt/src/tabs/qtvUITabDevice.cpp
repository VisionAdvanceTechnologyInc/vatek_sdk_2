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

#include <ui/base/qtv_ui_tab.h>
#include <ui/base/qtv_ui_table.h>
#include <ui/qtv_ui_app.h>
#include <QScreen>
#include <QDebug>
#include <windows.h>
#include <conio.h>

#include "ui_qtvUITabDevice.h"

#define TABDEVICE_NAEM		"Device"
#define TABDEVICE_DESC		"device detail information"

#define DEVICE_ICON_SIZE    64

qtvUITabDevice::qtvUITabDevice(qtvUIMain* main,QWidget* parent) :
	qtvUITabBase(main,TABDEVICE_NAEM, TABDEVICE_DESC, parent),
	m_maintab(NULL),
	m_chipinfo(NULL),
	m_tabstatus(NULL),
	ui(new Ui::qtvUITabDevice)
{
	ui->setupUi(this);

	connect(ui->btn_start, SIGNAL(clicked(bool)), this, SLOT(recvBtnStartClick(bool)));
	connect(ui->btn_calibration, SIGNAL(clicked(bool)), this, SLOT(recvBtnCalibrationClick(bool)));
	m_tabstatus = new qtvUITabStatus(main, ui->device_content);
	ui->device_content->addWidget(m_tabstatus);
	ui->btn_calibration->setEnabled(false);
	m_tabstatus->tabEnter();




}

qtvUITabDevice::~qtvUITabDevice()
{
	disconnect(ui->btn_start, SIGNAL(clicked(bool)), this, SLOT(recvBtnStartClick(bool)));
	resetService();
	detachUIMainTab();
	delete m_tabstatus;
	delete ui;
}

void qtvUITabDevice::tabEnter()
{
	if (qtvUIMain::is_uimain_tabs(m_main->_mode()))
	{
		attachUIMainTab(qobject_cast<qtvUIMainTabBase*>(m_main));
		return;
	}
	VWAR("qtvUITabDevice unsupport qtvUIMain");
}

void qtvUITabDevice::tabFinish()
{
	detachUIMainTab();
}

void qtvUITabDevice::tabLeave()
{
	detachUIMainTab();
}

void qtvUITabDevice::attachUIMainTab(qtvUIMainTabBase* maintab)
{
	detachUIMainTab();
	if (maintab)
	{
		m_maintab = maintab;
		connect(m_maintab, SIGNAL(notifyServiceChanged(qtvServiceBase*)), this, SLOT(recvServiceChanged(qtvServiceBase*)));
		connect(m_maintab, SIGNAL(notifyStatusChanged(qtv_status)), this, SLOT(recvStatusChanged(qtv_status)));
		connect(m_maintab, SIGNAL(notifyRunningUpdate()), this, SLOT(recvRunningUpdate()));
		if (m_maintab->_service())
			recvServiceChanged(m_maintab->_service());
		recvStatusChanged(m_maintab->_status());
	}
}

void qtvUITabDevice::detachUIMainTab()
{
	if (m_maintab)
	{
		m_tabstatus->tabLeave();
		disconnect(m_maintab, SIGNAL(notifyServiceChanged(qtvServiceBase*)), this, SLOT(recvServiceChanged(qtvServiceBase*)));
		disconnect(m_maintab, SIGNAL(notifyStatusChanged(qtv_status)), this, SLOT(recvStatusChanged(qtv_status)));
		disconnect(m_maintab, SIGNAL(notifyRunningUpdate()), this, SLOT(recvRunningUpdate()));
	}
	m_maintab = NULL;
}

void qtvUITabDevice::recvBtnCalibrationClick(bool bclk)
{
	ui->btn_calibration->setEnabled(false);
	if (m_maintab)
	{
		if (m_maintab->_mode() == qui_main_tabsdevice)
		{
			qtvUIMainDeviceTabs* tabdevice = qobject_cast<qtvUIMainDeviceTabs*>(m_maintab);
			if (tabdevice)
			{
				tabdevice->startCalibration();
				goto end_start;
			}
		}
	}
	VWAR("qtvUITabDevice recvBtnCalibrationClick not support main");

end_start:
	ui->btn_calibration->setEnabled(true);
}

void qtvUITabDevice::recvBtnStartClick(bool bclk)
{
	ui->btn_start->setEnabled(false);
	if (m_maintab)
	{
		if (m_maintab->_mode() == qui_main_tabsdevice)
		{
			qtvUIMainDeviceTabs* tabdevice = qobject_cast<qtvUIMainDeviceTabs*>(m_maintab);
			if (tabdevice)
			{
				if (tabdevice->_status() == qstatus_idle)
					tabdevice->startBroadcast();
				else tabdevice->stop();
				goto end_start;
			}
		}
	}
	VWAR("qtvUITabDevice recvBtnStartClick not support main");

end_start:
	ui->btn_start->setEnabled(true);
}

void qtvUITabDevice::resetService()
{
	QScreen* screen = QGuiApplication::primaryScreen();     //螢幕大小
	QRect mm = screen->availableGeometry();
	int screen_width = mm.width();
	int screen_height = mm.height();
	qDebug() << screen_width << screen_height;

	HDC hd = GetDC(NULL);                                           // 獲取DPI
	int horDPI = GetDeviceCaps(hd, LOGPIXELSX);
	int verticalDPI = GetDeviceCaps(hd, LOGPIXELSY);
	double objectRate = horDPI / 96.0;

	ui->tableinfo->setProperties(qui_table_readonly, NULL);

	if (screen_width >= 1920) {
		if (objectRate >= 1.3) {
			ui->tableinfo->setMinimumSize(QSize(500, 100));
		}
		else {
			ui->tableinfo->setMinimumSize(QSize(750, 100));
		}
	}
	else if (screen_width < 1000) {
		ui->tableinfo->setMinimumSize(QSize(500, 100));
	}
	else {
		ui->tableinfo->setMinimumSize(QSize(800, 100));
	}

	ui->txt_devicename->setText("");
	ui->txt_deviceservice->setText("");
	if (m_chipinfo)delete m_chipinfo;
	m_chipinfo = NULL;
}

void qtvUITabDevice::recvServiceChanged(qtvServiceBase* service)
{
	resetService();
	if (service)
	{
		qtvDevice* device = service->_device();
		QImage* icondev = qtvUIResource::getDeviceImage(service->_device()->_chip_info()->chip_module);
		m_chipinfo = qtvPropertyCollection::createChipInformation(device->_chip_info());
		ui->lab_deviceicon->setPixmap(QPixmap::fromImage(icondev->scaled(QSize(DEVICE_ICON_SIZE, DEVICE_ICON_SIZE))));
		ui->txt_devicename->setText(device->_name());
		ui->txt_deviceservice->setText(qtvDevice::getServiceName(device));
		ui->tableinfo->setProperties(qui_table_readonly, m_chipinfo);
	}
	
}

void qtvUITabDevice::recvStatusChanged(qtv_status status)
{
	if (status == qstatus_idle)ui->btn_start->setText("Start");
	else ui->btn_start->setText("Stop");
	ui->btn_calibration->setEnabled(!(status == qstatus_idle));
	ui->tableinfo->setProperties(qui_table_readonly, m_chipinfo);
}

void qtvUITabDevice::recvRunningUpdate()
{

}
