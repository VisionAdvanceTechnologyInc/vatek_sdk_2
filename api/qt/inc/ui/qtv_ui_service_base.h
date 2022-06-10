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

#ifndef QTV_UI_SERVICE_BASE_H
#define QTV_UI_SERVICE_BASE_H

#include <ui/base/qtv_ui_wizard.h>

class QTV_DECL qtvUIMainWizardBroadcast : public qtvUIMain
{
	Q_OBJECT
public:
	explicit qtvUIMainWizardBroadcast(qtv_uimain_mode mode, qtvServiceChipBase* service);
	virtual ~qtvUIMainWizardBroadcast();

	virtual qtvDTVProperties* _default_locale() { return m_locale; }
	virtual qtvSourceBase* _source() { return m_source; }
	virtual qtvMuxServiceBase* _mux_handle() { return m_muxhandle; }

	virtual void setDefaultLocale(qtvDTVProperties* locale);
	virtual void setMuxHandle(qtvMuxServiceBase* muxhandle);
	virtual void attachSource(qtvSourceBase* source);
	virtual void detachSource();

	virtual void getSource(qtvSourceBase** source);
	virtual void getLocale(qtvDTVProperties** locale);
	virtual void getMuxHandle(qtvMuxServiceBase** muxhandle);

protected:
	qtvDTVProperties* m_locale;
	qtvSourceBase* m_source;
	qtvMuxServiceBase* m_muxhandle;

Q_SIGNALS:
	void notifyLocaleChanged(qtvDTVProperties* locale);
	void notifyMuxHandleChenged(qtvMuxServiceBase* muxhandle);
	void notifySourceAttach(qtvSourceBase* source);
	void notifySourceDetach();
};

class QTV_DECL qtvUIWizardBroadcast : public qtvUIWizardBase
{
	Q_OBJECT
public:
	static vatek_result createWizard(qtvServiceBase* service, qtvUIWizardBroadcast** wizard,QWidget* parent = nullptr);
	static void freeWizard(qtvUIWizardBroadcast* wizard);

	/* qtvUIMainWizardBase */
	vatek_result startWizard() override;
	vatek_result getResult(qtvDTVProperties** locale, qtvSourceBase** source, qtvMuxServiceBase** muxhandle);
	/* QWizard */
	int nextId() const override;

protected:
	explicit qtvUIWizardBroadcast(qtvUIMainWizardBroadcast* main,qtvServiceChipBase* service, QWidget* parent = nullptr);
	~qtvUIWizardBroadcast();

	qtvUIMainWizardBroadcast* m_bcmain;
};

#endif
