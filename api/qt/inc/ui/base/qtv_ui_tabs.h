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

#ifndef QTV_UI_TABS_H
#define QTV_UI_TABS_H

#include <ui/base/qtv_ui_tab.h>

typedef enum _qtv_ui_tabs_mode
{
	qui_tabs_normal,
	qui_tabs_mainleft,
}qtv_ui_tabs_mode;

class QTV_DECL qtvUITabsBase : public QTabWidget
{
	Q_OBJECT
public:
	explicit qtvUITabsBase(qtv_ui_tabs_mode mode = qui_tabs_normal, QWidget* parent = nullptr);
	virtual ~qtvUITabsBase();

	qtvUIMainTabBase* _tabs_main() { return m_tabmain; }

	virtual void attachUIMainTab(qtvUIMainTabBase* main);
	virtual void detachUIMainTab();

protected:
	qtv_ui_tabs_mode m_mode;
	QImage m_logo;
	qtvUIMainTabBase* m_tabmain;
	qtvUITabBase* m_curtab;

	virtual void refreshTabs() = 0;
	void paintEvent(QPaintEvent* paint) override;
	void pushTab(qtvUITabBase* tab);

protected Q_SLOTS:
	void recvTabChanged(int index);
};


#endif
