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

#include <ui/qtv_ui_app.h>

/* qtvUIMainDeviceTabs */

qtvUIMainDeviceTabs::qtvUIMainDeviceTabs() :
	qtvUIMainTabBase(qui_main_tabsdevice)
{

}


/* qtvUITabsDevice */

qtvUITabsDevice::qtvUITabsDevice(QWidget* parent) :
	qtvUITabsBase(qui_tabs_mainleft, parent)
{

}

qtvUITabsDevice::~qtvUITabsDevice()
{
	detachUIMainTab();
}

void qtvUITabsDevice::recvServiceChanged(qtvServiceBase* service)
{
	refreshTabs();
}

void qtvUITabsDevice::attachUIMainTab(qtvUIMainTabBase* main)
{
	qtvUITabsBase::attachUIMainTab(main);
	if (main)
		connect(m_tabmain, SIGNAL(notifyServiceChanged(qtvServiceBase*)), this, SLOT(recvServiceChanged(qtvServiceBase*)));
}

void qtvUITabsDevice::detachUIMainTab()
{
	if (m_tabmain)
	{
		disconnect(m_tabmain, SIGNAL(notifyServiceChanged(qtvServiceBase*)), this, SLOT(recvServiceChanged(qtvServiceBase*)));
		qtvUITabsBase::detachUIMainTab();
		m_tabmain = NULL;
		refreshTabs();
	}
}

void qtvUITabsDevice::refreshTabs()
{
	clear();
	if (m_tabmain)
	{
		qtvServiceBase* service = m_tabmain->_service();
		if (!service)pushTab(new qtvUITabNoDevice(m_tabmain, this));
		else pushTab(new qtvUITabDevice(m_tabmain, this));
	}
}