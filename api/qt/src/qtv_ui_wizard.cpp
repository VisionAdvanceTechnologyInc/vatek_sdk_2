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

#include <ui/base/qtv_ui_wizard.h>
#include <QAbstractButton>
#include <QtWidgets/QApplication>
#include <QShowEvent>
#include <QHideEvent>
#include <QScreen>
#include <qDebug>
#include <windows.h>
#include <conio.h>
#define TOOLKIT_TIMEOUT_MS		1000

/* qtvUIWizardPage */

qtvUIWizardPage::qtvUIWizardPage(qtvUIMain* main, uint32_t id, QWidget* parent) :
	m_main(main),
	m_tab(NULL),
	m_id(id),
	QWizardPage(parent)
{
	if (id == BCWIZARD_ID_LOCALE)m_tab = new qtvUITabDtv(main, this);
	else if (id == BCWIZARD_ID_SOURCE)m_tab = new qtvUITabSource(main, this);
	else if (id == BCWIZARD_ID_SOURCEVALID)m_tab = new qtvUITabEnum(main, this);
	else if (id == BCWIZARD_ID_CONTEXT)m_tab = new qtvUITabContext(main, this);
	else if (id == BCWIZARD_ID_MODULATION)m_tab = new qtvUITabProperties(main, qui_tab_properties_modulation, this);
	else if (id == BCWIZARD_ID_MUXPARAM)m_tab = new qtvUITabProperties(main, qui_tab_properties_muxparam, this);
	else if (id == BCWIZARD_ID_VIDEOCODEC)m_tab = new qtvUITabProperties(main, qui_tab_properties_videocodec, this);
	else if (id == BCWIZARD_ID_AUDIOCODEC)m_tab = new qtvUITabProperties(main, qui_tab_properties_audiocodec, this);
	else if (id == BCWIZARD_ID_NODEVICE)m_tab = new qtvUITabNoDevice(main, this);
	if (m_tab)setTab(m_tab);
}

qtvUIWizardPage::~qtvUIWizardPage()
{
	if (m_tab)
	{
		disconnect(m_tab, SIGNAL(notifyValidChanged(bool)), this, SLOT(recvValidChanged(bool)));
		delete m_tab;
	}
}

void qtvUIWizardPage::setTab(qtvUITabBase* tab)
{
	m_tab = tab;
	if (m_tab)
	{
		setTitle(m_tab->_name());
		setSubTitle(m_tab->_descriptor());
		setLayout(m_tab->layout());
		connect(m_tab, SIGNAL(notifyValidChanged(bool)), this, SLOT(recvValidChanged(bool)));
	}
}

void qtvUIWizardPage::setWizardButtonEnable(QWizard::WizardButton button, bool isenable)
{
	wizard()->button(button)->setEnabled(isenable);
}

void qtvUIWizardPage::initializePage()
{
	
}

void qtvUIWizardPage::cleanupPage()
{
	
}

bool qtvUIWizardPage::isComplete() const
{
	return m_tab->_valid();
}

bool qtvUIWizardPage::validatePage()
{
	bool bok = m_tab->_valid();
	if (bok)m_tab->tabFinish();
	return bok;
}

void qtvUIWizardPage::tabEnter()
{
	m_tab->tabEnter();
}

void qtvUIWizardPage::tabLeave()
{
	m_tab->tabLeave();
}

void qtvUIWizardPage::recvValidChanged(bool bvalid)
{
	emit completeChanged();
}

/* qtvUIWizardBase */

qtvUIWizardBase::qtvUIWizardBase(qtvUIMain* main, QWidget* parent) :
	m_result(vatek_badstatus),
	m_currentpage(NULL),
	m_isdone(0),
	m_main(main),
	QWizard(parent)
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

	if (screen_width >= 1920) {
		if (objectRate >= 1.5) {
			setMinimumSize(screen_width / 1.5, screen_height / 1.2);
		}
		else {
			setMinimumSize(screen_width /1.8, screen_height /1.5);
		}
	}
	else {
		setMinimumSize(screen_width / 1.5, screen_height / 1.2);
	}


	setWizardStyle(QWizard::ClassicStyle);
	setMaximumSize(size());
	setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));
	setWindowFlags(windowFlags() | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
	setOption(WizardOption::IndependentPages, false);
	connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(recvPageChanged(int)));
}

qtvUIWizardBase::~qtvUIWizardBase()
{
	if (m_currentpage)m_currentpage->tabLeave();
	m_currentpage = NULL;
	disconnect(this, SIGNAL(currentIdChanged(int)), this, SLOT(recvPageChanged(int)));
}

void qtvUIWizardBase::done(int result)
{
	QWizard::done(result);
	if (result == QDialog::Accepted)
		m_result = vatek_success;
	else if (result == QDialog::Rejected)
		m_result = vatek_badparam;
	else m_result = vatek_unsupport;
	m_isdone = 1;
}

void qtvUIWizardBase::pushPage(uint32_t id)
{
	qtvUIWizardPage* page = new qtvUIWizardPage(m_main,id,this);
	m_pages.append(page);
	setPage(id, page);
}

qtvUIWizardPage* qtvUIWizardBase::getPage(uint32_t id)
{
	for (int32_t i = 0; i < m_pages.count(); i++)
	{
		if (m_pages.at(i)->_id() == id)
			return m_pages.at(i);
	}
	return NULL;
}

void qtvUIWizardBase::recvPageChanged(int id)
{
	//vatek_result nres = qtvDeviceManage::enumDevices(DEVICE_BUS_ALL, service_unknown);
	//if (nres < 0) {
	//	startTimer(tktimer_waitsource_new);
	//}

	if (m_currentpage)m_currentpage->tabLeave();
	m_currentpage = getPage(id);
	if (m_currentpage)m_currentpage->tabEnter();
}

void qtvUIWizardBase::startTimer(toolkit_timer_new tktimer)
{
	vatek_result nres = vatek_success;
	if (m_timemode != tktimer_null_new)stopTimer();
	if (tktimer == tktimer_waitsource_new)
		connect(&m_timer, SIGNAL(timeout()), this, SLOT(recvTimerCheckDevice()));
	else if (tktimer == tktimer_update_new)
		connect(&m_timer, SIGNAL(timeout), this, SLOT(recvTimerUpdate()));
	else nres = vatek_badparam;

	if (is_vatek_success(nres))
	{
		m_timemode = tktimer;
		m_timer.start(TOOLKIT_TIMEOUT_MS);
	}
}

void qtvUIWizardBase::stopTimer()
{
	if (m_timemode != tktimer_null_new)
	{
		m_timer.stop();
		if (m_timemode == tktimer_waitsource_new)
			disconnect(&m_timer, SIGNAL(timeout()), this, SLOT(recvTimerCheckDevice()));
		else if (m_timemode == tktimer_update_new)
			disconnect(&m_timer, SIGNAL(timeout), this, SLOT(recvTimerUpdate()));
	}
	m_timemode = tktimer_null_new;
}

void qtvUIWizardBase::recvTimerCheckDevice()
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

vatek_result qtvUIWizardBase::openService(qtvServiceBase* service)
{
	vatek_result nres = vatek_success;
	if (m_chipservice);
	{
		m_chipservice = qobject_cast<qtvServiceChipBase*>(service);
		if (m_chipservice)
		{
			nres = vatek_success;
		}
	}
	return nres;
}
