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
#include <QLabel>
#include <QProgressBar>
#include <ui/qtv_ui_service_usb.h>

#define UITAB_NODEVICE_NAME		"Enumerate"
#define UITAB_NODEVICE_DESC		"detect supported devices"

#define NODEVOCE_SIZE		200
#define NODEVICE_STEP_MS	200
#define NODEVICE_STEP		5

qtvUITabNoDevice::qtvUITabNoDevice(qtvUIMain* main, QWidget* parent) :
	qtvUITabSimple(main, UITAB_NODEVICE_NAME, UITAB_NODEVICE_DESC, parent),
	m_progress(NULL)
{
	QLabel* newimg = new QLabel(this);
	m_progress = new QProgressBar(this);
	QImage* imgsur = qtvUIResource::getImage(ICON_NODEVICE);
	QImage imglogo = imgsur->scaled(QSize(NODEVOCE_SIZE, NODEVOCE_SIZE), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	
	newimg->setFixedWidth(NODEVOCE_SIZE);
	newimg->setFixedHeight(NODEVOCE_SIZE);
	newimg->setPixmap(QPixmap::fromImage(imglogo));

	m_progress->setFixedWidth(newimg->width());
	m_progress->setTextVisible(false);
	putWidget(newimg);
	putWidget(m_progress);
	setValid(false);
}

qtvUITabNoDevice::~qtvUITabNoDevice()
{
	stopTimer();
	m_progress = nullptr;
}

void qtvUITabNoDevice::tabEnter()
{
	startTimer();
}

void qtvUITabNoDevice::tabFinish()
{
	stopTimer();
}

void qtvUITabNoDevice::tabLeave()
{
	stopTimer();
}

void qtvUITabNoDevice::startTimer()
{
	if (!m_timer.isActive())
	{
		m_progress->setValue(0);
		connect(&m_timer, SIGNAL(timeout()), this, SLOT(recvTimeOut()));
		m_timer.start(NODEVICE_STEP_MS);
	}
}

void qtvUITabNoDevice::stopTimer()
{
	if (m_timer.isActive())
	{
		m_timer.stop();
		disconnect(&m_timer, SIGNAL(timeout()), this, SLOT(recvTimeOut()));
	}
}

void qtvUITabNoDevice::recvTimeOut()
{
	int32_t value = m_progress->value() + NODEVICE_STEP;
	if (value >= 100)value = 0;

	if (!_valid())
	{
		qtvUIMainUSBMuxer* umuxmain = qobject_cast<qtvUIMainUSBMuxer*>(m_main);
		if (umuxmain)
		{
			vatek_result nres = qtvDeviceManage::enumDevices(DEVICE_BUS_USB, service_transform);
			if (nres > vatek_success)
			{
				qtvDevice* device = NULL;
				nres = qtvDeviceManage::openDevice(0, &device);
				if (is_vatek_success(nres))
				{
					qtvServiceUSBMuxer* newservice = NULL;
					nres = qtvServiceUSBMuxer::createService(device, &newservice);
					if (is_vatek_success(nres))umuxmain->attachUSBMuxer(newservice);
				}
			}
		}
		if (m_main->_service())setValid(true);
	}
	m_progress->setValue(value);
}
