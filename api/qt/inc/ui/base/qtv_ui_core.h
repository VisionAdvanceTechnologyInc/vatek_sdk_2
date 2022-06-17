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

#ifndef QTV_UI_CORE_H
#define QTV_UI_CORE_H

#include <QWidget>
#include <QPixmap>
#include <core/qtv_service.h>
#include <core/qtv_property.h>
#include <core/qtv_mux.h>
#include <core/qtv_dtv.h>

#include <chart/qtv_chart_ui.h>

#define ICON_BROADCAST		0x80000000
#define ICON_TRANSFORM		0x80000001
#define ICON_LOGOBIG		0x80000010
#define ICON_LOGOSMALL		0x80000011
#define ICON_NODEVICE		0x80000012
#define ICON_ERROR			0x80000013
#define ICON_VIDEO			0x80000100
#define ICON_AUDIO			0x80000101
#define ICON_CHANNEL		0x80000102
#define ICON_OUTPUT			0x80000103
#define ICON_PROPERTIES		0x80000104
#define ICON_INFORMATION	0x80000105

typedef enum _qtv_uimain_mode
{
	qui_main_maintabs = 0x100,
	qui_main_tabsdevice,
	qui_main_usbmuxer_config,
	qui_main_bcwizard = 0x200,
	qui_main_usbmuxer_wizard,
	qui_main_storage = 0x300,
	qui_main_mask = 0x0FFFFF00,
}qtv_uimain_mode;

class QTV_DECL qtvUIMain : public QObject
{
	Q_OBJECT
public:
	explicit qtvUIMain(qtv_uimain_mode mode, qtvServiceBase* service);
	virtual qtv_uimain_mode _mode() { return m_mode; }
	virtual qtvServiceBase* _service() { return m_service; }

	static bool is_uimain_tabs(qtv_uimain_mode mode);
	static bool is_uimain_wizard(qtv_uimain_mode mode);
	static bool is_uimain_storage(qtv_uimain_mode mode);

protected:
	qtv_uimain_mode m_mode;
	qtvServiceBase* m_service;
};

class QTV_DECL qtvUIMainTabBase : public qtvUIMain
{
	Q_OBJECT
public:
	explicit qtvUIMainTabBase(qtv_uimain_mode mode);
	virtual ~qtvUIMainTabBase();

	virtual qtv_status _status() { return m_status; }
	virtual void setService(qtvServiceBase* service);

protected:
	qtv_status m_status;
	qtvChartDataCapture* m_chartdata;

	virtual void setStatus(qtv_status status);
	virtual void attachService(qtvServiceBase* service);
	virtual void detachService();

protected Q_SLOTS:
	void recvServiceStatusChanged(qtv_status status);

Q_SIGNALS:
	void notifyServiceChanged(qtvServiceBase* service);
	void notifyStatusChanged(qtv_status status);
	void notifyRunningUpdate();
};

class QTV_DECL qtvUIResource : public QObject
{
	Q_OBJECT
public:
	static QImage* getDeviceImage(vatek_ic_module chip);
	static QImage* getImage(uint32_t id);
};

#endif
