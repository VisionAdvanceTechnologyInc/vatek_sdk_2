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
#include <ui/base/qtv_ui_tree.h>
#include <ui/qtv_ui_service_base.h>
#include "ui_qtvUITabEnum.h"

#define UITAB_ENUM_NAME		"Enumerate Source"
#define UITAB_ENUM_DESC		"checked source valid and enumerate source"

#define TABENUM_ICON_SIZE	64
#define TABENUM_TIME_TICK	20

qtvUITabEnum::qtvUITabEnum(qtvUIMain* main, QWidget* parent) :
	qtvUITabBase(main, UITAB_ENUM_NAME, UITAB_ENUM_DESC, parent),
	m_status(qstatus_idle),
	m_stepvalue(0),
	ui(new Ui::qtvUITabEnum)
{
	ui->setupUi(this);
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(recvTimerProcess()));
	connect(ui->tr_content, SIGNAL(notifyValidChanged(bool)), this, SLOT(recvEnumContextValid(bool)));
}

qtvUITabEnum::~qtvUITabEnum()
{
	disconnect(&m_timer, SIGNAL(timeout()), this, SLOT(recvTimerProcess()));
	disconnect(ui->tr_content, SIGNAL(notifyValidChanged(bool)), this, SLOT(recvEnumContextValid(bool)));
	delete ui;
}

void qtvUITabEnum::tabEnter()
{
	qtvUIMainWizardBroadcast* bcmain = qobject_cast<qtvUIMainWizardBroadcast*>(m_main);
	if (bcmain)
	{
		if (bcmain->_source())
		{
			setIcon(ICON_BROADCAST);
			setStatus(qstatus_idle);
			return;
		}
	}
	setError("bad main handle %d",m_main->_mode());
}

void qtvUITabEnum::tabFinish()
{
	vatek_result nres = vatek_unsupport;
	if (m_status == qstatus_finish)
	{
		qtvUIMainWizardBroadcast* bcmain = qobject_cast<qtvUIMainWizardBroadcast*>(m_main);
		if (bcmain)
		{
			qtvDTVProperties* deflocale = bcmain->_default_locale();
			qtvMuxServiceChannel* muxchannel = NULL;
			mux_spec_mode spec = mux_spec_iso;
			mux_country_code country = mux_country_undefined;
			qtv_service_mode servicemode = m_main->_service()->_mode();

			if (deflocale)
			{
				spec = deflocale->_param()->spec;
				country = deflocale->_param()->country;
			}

			if (servicemode == qservice_transfrom)
			{
				if (checkIsTransformEnum())
				{
					nres = ui->tr_content->getMuxHandle(spec, country, &muxchannel);
					if (is_vatek_success(nres))bcmain->setMuxHandle(muxchannel);
				}
				else nres = vatek_success;
			}
			else if (servicemode == qservice_broadcast)
			{
				qtvSourceBase* source = bcmain->_source();
				if (source->_param()->source == stream_source_encoder)
				{
					qtvMuxServiceDefault* newdef = NULL;
					nres = qtvMuxServiceDefault::createBroadcastMux(spec, country, 0x100, &source->_param()->stream.encoder, &newdef);
					if (is_vatek_success(nres))bcmain->setMuxHandle(newdef);
				}
				else nres = vatek_success;
			}
		}
	}

	if (!is_vatek_success(nres))VWAR("qtvUITabEnum finish create mux handle fail : %d", nres);
}

void qtvUITabEnum::tabLeave()
{
	setError("user cancel operation");
}

void qtvUITabEnum::recvTimerProcess()
{
	int32_t step = ui->prog_status->value() + 1;
	ui->prog_status->setValue(step);
	if (step == m_stepvalue)
	{
		qtvUIMainWizardBroadcast* bcmain = qobject_cast<qtvUIMainWizardBroadcast*>(m_main);
		qtvSourceBase* source = bcmain->_source();
		m_timer.stop();
		if (m_status == qstatus_idle)setStatus(qstatus_unlock);
		else if (m_status == qstatus_unlock)
		{
			if (source->_status() == qstatus_lock)
				setStatus(qstatus_lock);
			else setStatus(qstatus_unlock);
		}
		else if (m_status == qstatus_lock)
		{
			qtvServiceTransform* trservice = qobject_cast<qtvServiceTransform*>(bcmain->_service());
			if (trservice->_status() == qstatus_running)
			{
				Pbroadcast_info pinfo = NULL;
				vatek_result nres = trservice->pollingRunning(&pinfo);
				if (is_vatek_success(nres))
				{
					if (pinfo->status == bcstatus_finish)
					{
						nres = stopTransformEnum();
						if (is_vatek_success(nres))setStatus(qstatus_ready);
						return;
					}
				}

				if (!is_vatek_success(nres))
				{
					stopTransformEnum();
					setError("enumerate context fail : [%d:%d]", nres, pinfo->status);
				}
				else setStatus(qstatus_lock);
			}
			else setError("bad status not transform enumerate context");
		}
	}
}

void qtvUITabEnum::recvEnumContextValid(bool bvalid)
{
	setValid(bvalid);
}

bool qtvUITabEnum::checkIsTransformEnum()
{
	qtvUIMainWizardBroadcast* bcmain = qobject_cast<qtvUIMainWizardBroadcast*>(m_main);
	if (bcmain && bcmain->_service()->_mode() == qservice_transfrom)
	{
		qtvSourceTransform* trsource = qobject_cast<qtvSourceTransform*>(bcmain->_source());
		if (trsource && trsource->_enable_enum_context())return true;
	}
	return false;
}

vatek_result qtvUITabEnum::startTransformEnum()
{
	vatek_result nres = vatek_unsupport;
	if (checkIsTransformEnum())
	{
		qtvUIMainWizardBroadcast* bcmain = qobject_cast<qtvUIMainWizardBroadcast*>(m_main);
		qtvServiceTransform* trservice = qobject_cast<qtvServiceTransform*>(bcmain->_service());
		qtvSourceTransform* trsource = qobject_cast<qtvSourceTransform*>(bcmain->_source());
		nres = trservice->startEnumContext(trsource, 10000);
	}
	if (!is_vatek_success(nres))
		setError("start transform enumerate fail : %d", nres);
	return nres;
}

vatek_result qtvUITabEnum::stopTransformEnum()
{
	vatek_result nres = vatek_unsupport;
	qtvServiceChipBase* service = qobject_cast<qtvServiceChipBase*>(m_main->_service());
	if (service)
	{
		if (service->_status() == qstatus_running)
			nres = service->stop();
		else nres = vatek_success;
	}
	if (!is_vatek_success(nres))setError("stop transform enumerate fail : %d", nres);
	return nres;
}

void qtvUITabEnum::setError(const char* szfmt, ...)
{
	QString szerr;
	va_list ap;
	va_start(ap, szfmt);
	szerr.vsprintf(szfmt, ap);
	va_end(ap);
	setIcon(ICON_ERROR);
	ui->txt_info->setText(szerr);
	ui->prog_status->setValue(100);
	setValid(false);
	stopTransformEnum();
	if (m_timer.isActive())m_timer.stop();
}

void qtvUITabEnum::setStatus(qtv_status status)
{
	qtvUIMainWizardBroadcast* bcmain = qobject_cast<qtvUIMainWizardBroadcast*>(m_main);
	if (status == qstatus_idle)
	{
		setValid(false);
		
		ui->txt_info->setText("check source status...");
		ui->prog_status->setValue(0);
		ui->tr_content->hide();
		m_stepvalue = 10;
		m_timer.start(TABENUM_TIME_TICK);
	}
	else if (status == qstatus_unlock)
	{
		ui->txt_info->setText("wait source lock...");
		ui->prog_status->setValue(10);
		m_stepvalue = 50;
		m_timer.start(TABENUM_TIME_TICK);
	}
	else if (status == qstatus_lock)
	{
		if (checkIsTransformEnum())
		{
			vatek_result nres = vatek_success;
			if (bcmain->_service()->_status() != qstatus_running)
				nres = startTransformEnum();

			if (is_vatek_success(nres))
			{
				ui->txt_info->setText("enumerate context wait finish...");
				ui->prog_status->setValue(50);
				m_stepvalue = 80;
				m_timer.start(TABENUM_TIME_TICK);
			}
		}
		else
		{
			ui->txt_info->setText("source context valid and read ...");
			ui->prog_status->setValue(100);
			setValid(true);
			status = qstatus_finish;
		}
	}
	else if (status == qstatus_ready)
	{
		qtvServiceTransform* trservice = qobject_cast<qtvServiceTransform*>(bcmain->_service());
		if (trservice && trservice->getEnumContext())
		{
			ui->txt_info->setText("select broadcast context");
			ui->tr_content->show();
			ui->prog_status->setValue(100);
			setValid(false);
			ui->tr_content->setEnumContext(trservice->getEnumContext());
			setValid(ui->tr_content->checkEnumContextValid());
			status = qstatus_finish;
		}
		else setError("get enumerate context fail not found");
	}
	m_status = status;
}

void qtvUITabEnum::setIcon(uint32_t id)
{
	QImage* suricon = qtvUIResource::getImage(id);
	if (suricon)
	{
		QImage m_img = suricon->scaled(QSize(TABENUM_ICON_SIZE, TABENUM_ICON_SIZE));
		ui->lb_icon->setPixmap(QPixmap::fromImage(m_img));
	}
}
