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

#pragma once

#include <QtWidgets/QMainWindow>
#include <QDialog>
#include "ui_vatek_toolkit.h"

#include <ui/base/qtv_ui_tab.h>
#include <ui/qtv_ui_app.h>

typedef enum _toolkit_timer
{
	tktimer_null,
	tktimer_waitsource,
	tktimer_update,
}toolkit_timer;

class calibrationDialig : public QDialog
{
	Q_OBJECT
public:
	explicit calibrationDialig(qtvUIMain* main,QWidget* parent = nullptr);
	~calibrationDialig();

protected:
	qtvUITabBase* m_tabbase;
	qtvServiceHandle* m_service;
};

class vatek_toolkit : public QMainWindow
{
    Q_OBJECT
public:
    vatek_toolkit(QWidget *parent = Q_NULLPTR);
    ~vatek_toolkit();

	qtvServiceBase* _service() { return m_uimain._service(); }

protected:

	class UIMainToolKit : public qtvUIMainDeviceTabs
	{
	public:
		explicit UIMainToolKit(vatek_toolkit* toolkit);
		/* qtvUIMainDeviceTabs */
		bool startBroadcast() override;
		bool startUpdate() override;
		void startCalibration() override;
		void stop() override;

	protected:
		vatek_toolkit* m_toolkit;
		QTimer m_timer;
	};

	UIMainToolKit m_uimain;
	QTimer m_timer;
	toolkit_timer m_timemode;
	qtvDevice* m_device;
	qtvSourceBase* m_source;
	qtvServiceChipBase* m_chipservice;
	qtvMuxServiceBase* m_muxhandle;

	void startTimer(toolkit_timer tktimer);
	void stopTimer();

	vatek_result startBroadcast(qtvSourceBase* source,qtvMuxServiceBase* muxhandle);
	void stopBroadcast();

	vatek_result openService(qtvServiceBase* service);
	void closeService();

private Q_SLOTS:
	void recvTimerCheckDevice();
	void recvTimerUpdate();

private:
    Ui::vatek_toolkitClass ui;
};
