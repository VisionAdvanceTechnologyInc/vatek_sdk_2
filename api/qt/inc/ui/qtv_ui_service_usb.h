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

#ifndef QTV_UI_SERVICE_USB_H
#define QTV_UI_SERVICE_USB_H

#include <core/qtv_service_usb.h>
#include <ui/base/qtv_ui_config.h>
#include <ui/base/qtv_ui_wizard.h>
#include <QDialog>

typedef enum _qtv_ui_usbmuxer_mode
{
	qui_usbmuxer_single = 0,
	qui_usbmuxer_mutil = 1,
}qtv_ui_usbmuxer_mode;

class QTV_DECL qtvUIMainUSBMuxer : public qtvUIMain
{
	Q_OBJECT
public:
	explicit qtvUIMainUSBMuxer(qtv_uimain_mode mainmode,qtv_ui_usbmuxer_mode uimode,qtvServiceUSBMuxer* service);
	virtual ~qtvUIMainUSBMuxer();

	virtual qtv_ui_usbmuxer_mode _usbmuxer_mode() { return m_usbmuxermode; }
	virtual const QList<qtvUIConfigBase*> _configs() { return m_configs; }
	virtual Pqtv_locale_param _local_param();
	virtual qtvCodecVideo* _video_codec() { return m_videocodec; }
	virtual qtvCodecAudio* _audio_codec() { return m_audiocodec; }

	virtual qtvCodecProvider* getVideoProvider() = 0;
	virtual qtvCodecProvider* getAudioProvider() = 0;
	virtual void setDefaultLocal(qtvDTVProperties* locale);

	virtual void attachUSBMuxer(qtvServiceUSBMuxer* usbmuxer);
	virtual void attachVideoCodec(qtvCodecVideo* vcodec);
	virtual void attachAudioCodec(qtvCodecAudio* acodec);

	virtual void refreshConfig();

Q_SIGNALS:
	void notifyServiceChanged();

protected:
	QList<qtvUIConfigBase*> m_configs;
	qtv_ui_usbmuxer_mode m_usbmuxermode;
	qtvDTVProperties* m_locale;
	qtvCodecVideo* m_videocodec;
	qtvCodecAudio* m_audiocodec;
	void clearConfig();
};

class QTV_DECL qtvUIDialogUSBMuxerConfig : public QDialog
{
	Q_OBJECT
public:
	static vatek_result createDialog(qtvServiceUSBMuxer* service, qtvUIMainUSBMuxer* main, qtvUIDialogUSBMuxerConfig** dialog, QWidget* parent = nullptr);
	static void freeDialog(qtvUIDialogUSBMuxerConfig* dialog);

protected:
	explicit qtvUIDialogUSBMuxerConfig(QSize szie, qtvServiceUSBMuxer* service, qtvUIMainUSBMuxer* main, QWidget* parent = nullptr);
	virtual ~qtvUIDialogUSBMuxerConfig();

	qtvUIMainUSBMuxer* m_main;
	qtvUITabConfigs* m_tabbase;

	void refreshConfig();
};

class QTV_DECL qtvUIWizardUSBMuxer : public qtvUIWizardBase
{
	Q_OBJECT
public:
	static vatek_result createWizard(qtvCodecProvider* video,qtvCodecProvider* audio,qtvUIWizardUSBMuxer** wizard, QWidget* parent = nullptr);
	static void freeWizard(qtvUIWizardUSBMuxer* wizard);

	/* qtvUIWizardBase */
	vatek_result startWizard() override;

	void getResult(qtvUIMainUSBMuxer** main);

	/* QWizard */
	int nextId() const override;

protected:
	explicit qtvUIWizardUSBMuxer(qtvUIMainUSBMuxer* main, QWidget* parent = nullptr);
	virtual ~qtvUIWizardUSBMuxer();
	qtvUIMainUSBMuxer* m_usbmuxermain;

protected Q_SLOTS:
	void recvServiceChanged();
};

#endif
