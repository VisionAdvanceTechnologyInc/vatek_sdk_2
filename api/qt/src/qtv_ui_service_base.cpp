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

#include <ui/qtv_ui_service_base.h>


/* qtvUIMainWizardBroadcast */

qtvUIMainWizardBroadcast::qtvUIMainWizardBroadcast(qtv_uimain_mode mode, qtvServiceChipBase* service) :
	qtvUIMain(mode, service),
	m_source(NULL),
	m_muxhandle(NULL),
	m_locale(NULL)
{

}

qtvUIMainWizardBroadcast::~qtvUIMainWizardBroadcast()
{
	if (m_locale)delete m_locale;
	if (m_source)delete m_source;
	if (m_muxhandle)delete m_muxhandle;
}

void qtvUIMainWizardBroadcast::setDefaultLocale(qtvDTVProperties* locale)
{
	qtvServiceChipBase* service = qobject_cast<qtvServiceChipBase*>(m_service);
	if (m_locale)delete m_locale;
	m_locale = locale;

	if (service)
	{
		service->setLocalDefault(locale);
		emit notifyLocaleChanged(m_locale);
	}
}

void qtvUIMainWizardBroadcast::setMuxHandle(qtvMuxServiceBase* muxhandle)
{
	if (m_muxhandle)delete m_muxhandle;
	m_muxhandle = muxhandle;
	emit notifyMuxHandleChenged(m_muxhandle);
}

void qtvUIMainWizardBroadcast::attachSource(qtvSourceBase* source)
{
	if (m_source)
		detachSource();
	if (source)
	{
		m_source = source;
		emit notifySourceAttach(m_source);
	}
}

void qtvUIMainWizardBroadcast::detachSource()
{
	if (m_source)
	{
		delete m_source;
		emit notifySourceDetach();
	}
	m_source = NULL;
}

void qtvUIMainWizardBroadcast::getSource(qtvSourceBase** source)
{
	if (m_source && source)
	{
		*source = m_source;
		m_source = NULL;
	}
}

void qtvUIMainWizardBroadcast::getLocale(qtvDTVProperties** locale)
{
	if (m_locale && locale)
	{
		*locale = m_locale;
		m_locale = NULL;
	}
}

void qtvUIMainWizardBroadcast::getMuxHandle(qtvMuxServiceBase** muxhandle)
{
	if (m_muxhandle && muxhandle)
	{
		*muxhandle = m_muxhandle;
		m_muxhandle = NULL;
	}
}

/* qtvUIWizardBroadcast */

vatek_result qtvUIWizardBroadcast::createWizard(qtvServiceBase* service, qtvUIWizardBroadcast** wizard, QWidget* parent)
{
	vatek_result nres = vatek_unsupport;
	qtvServiceChipBase* chipservice = qobject_cast<qtvServiceChipBase*>(service);
	if (chipservice)
	{
		qtvUIMainWizardBroadcast* newmain = new qtvUIMainWizardBroadcast(qui_main_bcwizard, chipservice);
		*wizard = new qtvUIWizardBroadcast(newmain,chipservice, parent);
		nres = vatek_success;
	}
	return nres;
}

void qtvUIWizardBroadcast::freeWizard(qtvUIWizardBroadcast* wizard)
{
	delete wizard;
}

qtvUIWizardBroadcast::qtvUIWizardBroadcast(qtvUIMainWizardBroadcast* main, qtvServiceChipBase* service, QWidget* parent) :
	qtvUIWizardBase(main, parent),
	m_bcmain(main)
{
	setWindowTitle("broadcast wizard");
	pushPage(BCWIZARD_ID_LOCALE);
	pushPage(BCWIZARD_ID_SOURCE);
	pushPage(BCWIZARD_ID_SOURCEVALID);
	pushPage(BCWIZARD_ID_CONTEXT);
	pushPage(BCWIZARD_ID_MODULATION);
	setStartId(BCWIZARD_ID_LOCALE);
#if 0
	if (m_bcmain->_service()->_mode() == qservice_transfrom)
		setStartId(BCWIZARD_ID_SOURCE);
	else {
		setStartId(BCWIZARD_ID_LOCALE);
	}
#endif
}

qtvUIWizardBroadcast::~qtvUIWizardBroadcast()
{
	delete m_bcmain;
}

vatek_result qtvUIWizardBroadcast::startWizard()
{
	exec();
	return m_result;
}

int qtvUIWizardBroadcast::nextId() const
{
	int32_t curid = currentId();
	if (curid == BCWIZARD_ID_LOCALE)
		return BCWIZARD_ID_SOURCE;
	else if (curid == BCWIZARD_ID_SOURCE)
		return BCWIZARD_ID_SOURCEVALID;
	else if (curid == BCWIZARD_ID_SOURCEVALID)
	{
		if (m_bcmain->_service()->_mode() == qservice_broadcast)
			return BCWIZARD_ID_CONTEXT;
		else
		{
			qtvSourceTransform* trsource = qobject_cast<qtvSourceTransform*>(m_bcmain->_source());
			if (trsource->_enable_enum_context())
				return BCWIZARD_ID_CONTEXT;
			else return BCWIZARD_ID_MODULATION;
		}
	}
	else if (curid == BCWIZARD_ID_CONTEXT)
		return BCWIZARD_ID_MODULATION;
	return -1;
}

vatek_result qtvUIWizardBroadcast::getResult(qtvDTVProperties** locale, qtvSourceBase** source, qtvMuxServiceBase** muxhandle)
{
	vatek_result nres = vatek_badstatus;
	if (m_isdone)
	{
		if (!is_vatek_success(m_result))nres = m_result;
		else
		{
			if (locale)m_bcmain->getLocale(locale);
			if (source)m_bcmain->getSource(source);
			if (muxhandle)m_bcmain->getMuxHandle(muxhandle);
			nres = vatek_success;
		}
	}
	return nres;
}
