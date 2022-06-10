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

#ifndef QTV_UI_WIZARD_H
#define QTV_UI_WIZARD_H

#include <QWizard>
#include <QWizardPage>
#include <QShowEvent>
#include <QHideEvent>
#include <ui/base/qtv_ui_tab.h>

typedef enum _toolkit_timer_new
{
	tktimer_null_new,
	tktimer_waitsource_new,
	tktimer_update_new,
}toolkit_timer_new;

#define BCWIZARD_ID_LOCALE			0
#define BCWIZARD_ID_SOURCE			1
#define BCWIZARD_ID_SOURCEVALID		2
#define BCWIZARD_ID_CONTEXT			3
#define BCWIZARD_ID_MODULATION		4

#define BCWIZARD_ID_MUXPARAM		5
#define BCWIZARD_ID_VIDEOCODEC		6
#define BCWIZARD_ID_AUDIOCODEC		7

#define BCWIZARD_ID_NODEVICE		8

class QTV_DECL qtvUIWizardPage : public QWizardPage
{
	Q_OBJECT
public:
	explicit qtvUIWizardPage(qtvUIMain* main,uint32_t id, QWidget* parent = nullptr);
	virtual ~qtvUIWizardPage();

	uint32_t _id() { return m_id; }
	void setWizardButtonEnable(QWizard::WizardButton button, bool isenable);

	/* QWizardPage */
	void initializePage() override;
	void cleanupPage() override;
	bool isComplete()const override;
	bool validatePage() override;

	void tabEnter();
	void tabLeave();

protected:
	qtvUIMain* m_main;
	qtvUITabBase* m_tab;
	uint32_t m_id;

	void setTab(qtvUITabBase* tab);


protected Q_SLOTS:
	void recvValidChanged(bool bvalid);
};

class QTV_DECL qtvUIWizardBase : public QWizard
{
	Q_OBJECT
public:
	explicit qtvUIWizardBase(qtvUIMain* main, QWidget* parent = nullptr);
	virtual ~qtvUIWizardBase();

	virtual vatek_result startWizard() = 0;
	void done(int result) override;

	void startTimer(toolkit_timer_new tktimer);
	void stopTimer();
	void recvTimerCheckDevice();

	vatek_result openService(qtvServiceBase* service);

protected:
	vatek_result m_result;
	QList<qtvUIWizardPage*> m_pages;
	qtvUIWizardPage* m_currentpage;
	int32_t m_isdone;
	qtvUIMain* m_main;


	QTimer m_timer;
	toolkit_timer_new m_timemode;
	qtvDevice* m_device;
	qtvServiceChipBase* m_chipservice;

	virtual void pushPage(uint32_t id);
	qtvUIWizardPage* getPage(uint32_t id);

protected Q_SLOTS:
	void recvPageChanged(int id);
};

#endif
