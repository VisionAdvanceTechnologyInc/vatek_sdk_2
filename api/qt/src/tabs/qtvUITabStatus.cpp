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
#include "ui_qtvUITabStatus.h"

#define UITAB_STATUS_NAME	"Status"
#define UITAB_STATUS_DESC	"show service status"

qtvUITabStatus::qtvUITabStatus(qtvUIMain* main, QWidget* parent) :
	qtvUITabBase(main, UITAB_STATUS_NAME, UITAB_STATUS_DESC),
	ui(new Ui::qtvUITabStatus)
{
	qtvUIMainTabBase* tabmain = qobject_cast<qtvUIMainTabBase*>(m_main);
	ui->setupUi(this);
	connect(ui->cb_analysis, SIGNAL(currentIndexChanged(int)), this, SLOT(recvAnalysusChanged(int)));
	if(tabmain)
		connect(tabmain, SIGNAL(notifyServiceChanged(qtvServiceBase*)), this, SLOT(recvServiceChanged(qtvServiceBase*)));
}

qtvUITabStatus::~qtvUITabStatus()
{
#if 0
	qtvUIMainTabBase* tabmain = qobject_cast<qtvUIMainTabBase*>(m_main);
	if(tabmain)
		disconnect(tabmain, SIGNAL(notifyServiceChanged(qtvServiceBase*)), this, SLOT(recvServiceChanged(qtvServiceBase*)));
#endif
	disconnect(ui->cb_analysis, SIGNAL(currentIndexChanged(int)), this, SLOT(recvAnalysusChanged(int)));
	delete ui;
}

void qtvUITabStatus::tabEnter()
{
	refreshAnalysis();
}

void qtvUITabStatus::tabFinish()
{

}

void qtvUITabStatus::tabLeave()
{
	clearAnalysis();
}

void qtvUITabStatus::clearAnalysis()
{
	int32_t i = 0;
	qtvUIMainTabBase* tabmain = qobject_cast<qtvUIMainTabBase*>(m_main);

	ui->chart_ui->detachProvider();
	for (i = 0; i < ui->cb_analysis->count(); i++)
	{
		qtvChartUIProvider* provider = qvariant_cast<qtvChartUIProvider*>(ui->cb_analysis->itemData(i));
		delete provider;
	}
	ui->cb_analysis->clear();
}

void qtvUITabStatus::refreshAnalysis()
{
	clearAnalysis();
	qtvChartUIProviderService* newprovider = qtvChartUIProviderService::createServiceProvider(NULL);
	if (newprovider)
		ui->cb_analysis->addItem(newprovider->_name(), QVariant::fromValue<qtvChartUIProvider*>(newprovider));

	if (m_main)
	{
		if(m_main->_service())
			newprovider->connectChart(m_main->_service());
	}
}

void qtvUITabStatus::recvAnalysusChanged(int index)
{
	if (index != -1)
	{
		qtvChartUIProvider* provider = qvariant_cast<qtvChartUIProvider*>(ui->cb_analysis->itemData(index));
		if (provider)ui->chart_ui->attachProvider(provider);
		else ui->chart_ui->detachProvider();
	}
}

void qtvUITabStatus::recvServiceChanged(qtvServiceBase* service)
{
	int32_t i = 0;
	for (i = 0; i < ui->cb_analysis->count(); i++)
	{
		qtvChartUIProvider* provider = qvariant_cast<qtvChartUIProvider*>(ui->cb_analysis->itemData(i));
		qtvChartUIProviderService* pservice = qobject_cast<qtvChartUIProviderService*>(provider);
		if (pservice)
		{
			if (service)pservice->connectChart(service);
			else pservice->disconnectChart();
		}
	}
}