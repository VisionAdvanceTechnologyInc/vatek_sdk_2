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

#ifndef QTV_UI_TAB_H
#define QTV_UI_TAB_H

#include <QTabWidget>
#include <QTimer>
#include <QSpinBox>
#include <QList>
#include <QProgressBar>
#include <core/qtv_dtv.h>
#include <core/qtv_media.h>
#include <ui/base/qtv_ui_core.h>
#include <ui/base/qtv_ui_config.h>
#include <ui/base/qtv_ui_table.h>
#include <ui/base/qtv_ui_list.h>

namespace Ui {
	class qtvUITabSimple;
	class qtvUITabProperties;
	class qtvUITabDevice;
	class qtvUITabDtv;
	class qtvUITabSource;
	class qtvUITabEnum;
	class qtvUITabContext;
	class qtvUITabConfigs;
	class qtvUITabStatus;
	class qtvUITabCalibration;
}

class QTV_DECL qtvUITabBase : public QWidget
{
	Q_OBJECT
public:
	explicit qtvUITabBase(qtvUIMain* main,const char* name, const char* desc, QWidget* parent = nullptr);
	explicit qtvUITabBase(qtvUIMain* main, QString& name, QString& desc, QWidget* parent = nullptr);
	virtual ~qtvUITabBase();

	virtual QString& _name() { return m_name; }
	virtual QString& _descriptor() { return m_desc; }
	virtual bool _valid() { return m_valid; }

	virtual void tabEnter() = 0;
	virtual void tabFinish() = 0;
	virtual void tabLeave() = 0;

protected:
	qtvUIMain* m_main;
	QString m_name;
	QString m_desc;
	bool m_valid;

	virtual void setValid(bool valid);

Q_SIGNALS:
	void notifyValidChanged(bool valid);
};

class QTV_DECL qtvUITabSimple : public qtvUITabBase
{
	Q_OBJECT
public:
	explicit qtvUITabSimple(qtvUIMain* main, const char* name, const char* desc, QWidget* parent = nullptr);
	virtual ~qtvUITabSimple();

protected:
	Ui::qtvUITabSimple* ui;

	void putWidget(QWidget* widget, int stretch = 0, Qt::Alignment alignment = Qt::Alignment());
};

typedef enum _qtv_ui_tab_properties_mode
{
	qui_tab_properties_null,
	qui_tab_properties_modulation,
	qui_tab_properties_muxparam,
	qui_tab_properties_videocodec,
	qui_tab_properties_audiocodec,
}qtv_ui_tab_properties_mode;

class QTV_DECL qtvUITabProperties : public qtvUITabBase
{
	Q_OBJECT
public:
	explicit qtvUITabProperties(qtvUIMain* main, qtv_ui_tab_properties_mode mode, QWidget* parent = nullptr);
	virtual ~qtvUITabProperties();

	/* qtvUITabBase */
	void tabEnter() override;
	void tabFinish() override;
	void tabLeave() override;

	static const char* getPropertiesName(qtv_ui_tab_properties_mode mode);
	static const char* getPropertiesDesc(qtv_ui_tab_properties_mode mode);
protected:
	explicit qtvUITabProperties(qtvUIMain* main, const char* name, const char* desc, QWidget* parent = nullptr);
	explicit qtvUITabProperties(qtvUIMain* main, QString& name, QString& desc, QWidget* parent = nullptr);

	qtv_ui_tab_properties_mode m_mode;
	Ui::qtvUITabProperties* ui;
	qtvCodec* m_codec;

	void refreshProperties(qui_table_mode mode, qtvPropertyCollection* properties);
	void refreshTitle();
	void refreshCodec(qtvCodecProvider* provider);

protected Q_SLOTS:
	void recvCodecSelectChanged(int index);

};

class QTV_DECL qtvUITabStatus : public qtvUITabBase
{
	Q_OBJECT
public:
	explicit qtvUITabStatus(qtvUIMain* main, QWidget* parent = nullptr);
	virtual ~qtvUITabStatus();

	/* qtvUITabBase */
	void tabEnter() override;
	void tabFinish() override;
	void tabLeave() override;

protected:
	Ui::qtvUITabStatus* ui;

	void clearAnalysis();
	void refreshAnalysis();

protected Q_SLOTS:
	void recvAnalysusChanged(int index);
	void recvServiceChanged(qtvServiceBase* service);
};

class QTV_DECL qtvUITabNoDevice : public qtvUITabSimple
{
	Q_OBJECT
public:
	explicit qtvUITabNoDevice(qtvUIMain* main, QWidget* parent = nullptr);
	virtual ~qtvUITabNoDevice();

	/* qtvUITabBase */
	void tabEnter() override;
	void tabFinish() override;
	void tabLeave() override;

protected:
	QTimer m_timer;
	QProgressBar* m_progress;
	void startTimer();
	void stopTimer();

protected Q_SLOTS : 
	void recvTimeOut();
};

class QTV_DECL qtvUITabDevice : public qtvUITabBase
{
	Q_OBJECT
public:
	explicit qtvUITabDevice(qtvUIMain* main,QWidget* parent = nullptr);
	virtual ~qtvUITabDevice();

	/* qtvUITabBase */
	void tabEnter() override;
	void tabFinish() override;
	void tabLeave() override;

protected:
	Ui::qtvUITabDevice* ui;
	qtvUIMainTabBase* m_maintab;
	qtvPropertyCollection* m_chipinfo;
	qtvUITabStatus* m_tabstatus;

	/* qtvUITabDevice */
	void attachUIMainTab(qtvUIMainTabBase* maintab);
	void detachUIMainTab();

	void resetService();

protected Q_SLOTS:
	void recvBtnStartClick(bool bclk);
	void recvBtnCalibrationClick(bool bclk);
		
	void recvServiceChanged(qtvServiceBase* service);
	void recvStatusChanged(qtv_status status);
	void recvRunningUpdate();
};

/* qtvUITabCalibration */

class QTV_DECL qtvUITabCalibration : public qtvUITabBase
{
	Q_OBJECT
public:
	explicit qtvUITabCalibration(qtvUIMain* main, QWidget* parent = nullptr);
	virtual ~qtvUITabCalibration();

	/* qtvUITabBase */
	void tabEnter() override;
	void tabFinish() override;
	void tabLeave() override;

protected:
	QList<QSpinBox*> m_splist;
	calibration_param m_calibration;
	qtvServiceHandle* m_handle;
	QTimer m_timerupdate;

	void resetCalibration();
	void applyCalibration();
	void enableCalibration(bool isen);

protected Q_SLOTS:
	void recvSpinValueChanged(int i);
	void recvUpdateTimeout();
	void recvSaveClick(bool bclick);


private:
	Ui::qtvUITabCalibration* ui;
};

/* qtvUITabDtv */

class QTV_DECL qtvUITabDtv : public qtvUITabBase
{
	Q_OBJECT
public:
	explicit qtvUITabDtv(qtvUIMain* main, QWidget* parent = nullptr);
	virtual ~qtvUITabDtv();

	/* qtvUITabBase */
	void tabEnter() override;
	void tabFinish() override;
	void tabLeave() override;

protected:
	Ui::qtvUITabDtv* ui;
	qtvDTVProperties* m_properties;

	void resetPropertes();
	void refreshCountry();
	void refreshChannel(qtvDTVLocale* plocale);

protected Q_SLOTS:
	void recvCountrySelect(int index);
	void recvChannelSelect(int index);
};

/* qtvUITabSource */

class QTV_DECL qtvUITabSource : public qtvUITabBase
{
	Q_OBJECT
public:
	explicit qtvUITabSource(qtvUIMain* main, QWidget* parent = nullptr);
	virtual ~qtvUITabSource();

	/* qtvUITabBase */
	void tabEnter() override;
	void tabFinish() override;
	void tabLeave() override;

protected:
	qtvPropertyCollection* m_properties;
	Ui::qtvUITabSource* ui;

	void refreshSource();
	void resetProperties();
	qtvPropertyCollection* finishProperties();

protected Q_SLOTS:
	void recvSourceSelect(int index);
	void recvValidChanged(bool bvalid);
};

/* qtvUITabEnum */

class QTV_DECL qtvUITabEnum : public qtvUITabBase
{
	Q_OBJECT
public:
	explicit qtvUITabEnum(qtvUIMain* main, QWidget* parent = nullptr);
	virtual ~qtvUITabEnum();

	/* qtvUITabBase */
	void tabEnter() override;
	void tabFinish() override;
	void tabLeave() override;

protected:
	qtv_status m_status;
	Ui::qtvUITabEnum* ui;
	QTimer m_timer;
	int32_t m_stepvalue;

	void setStatus(qtv_status status);
	void setError(const char* szfmt, ...);
	void setIcon(uint32_t id);

	bool checkIsTransformEnum();
	vatek_result startTransformEnum();
	vatek_result stopTransformEnum();

protected Q_SLOTS:
	void recvTimerProcess();
	void recvEnumContextValid(bool bvalid);
};

/* qtvUITabContext */

class QTV_DECL qtvUITabContext : public qtvUITabBase
{
	Q_OBJECT
public:
	explicit qtvUITabContext(qtvUIMain* main, QWidget* parent = nullptr);
	virtual ~qtvUITabContext();

	/* qtvUITabBase */
	void tabEnter() override;
	void tabFinish() override;
	void tabLeave() override;

protected:

	explicit qtvUITabContext(qtvUIMain* main, const char* name, const char* desc, QWidget* parent = nullptr);
	explicit qtvUITabContext(qtvUIMain* main, QString& name, QString& desc, QWidget* parent = nullptr);

	Ui::qtvUITabContext* ui;
	void setChannel(qtvMuxChannel* channel);

protected Q_SLOTS:
	void recvContextSelectChanged();
	void recvStreamSelectChanged();

private:
	void initConnect();
};

/* qtvUITabConfigs */

class QTV_DECL qtvUITabConfigs : public qtvUITabBase
{
	Q_OBJECT
public:
	explicit qtvUITabConfigs(qtvUIMain* main, QWidget* parent = nullptr);
	virtual ~qtvUITabConfigs();

	/* qtvUITabBase */
	void tabEnter() override;
	void tabFinish() override;
	void tabLeave() override;

	virtual void resetConfigs();
	virtual vatek_result addConfig(qtvUIConfigBase* config);
	virtual void setCurrentSelect(int32_t index);

protected:
	Ui::qtvUITabConfigs* ui;
	qtvUIListItem* m_select;

protected Q_SLOTS:
	void recvItemSelected();

private:
	class qinternal_config
	{
	public:
		explicit qinternal_config(qtvUIConfigBase* config, qtvUITabBase* tab, qtvUIListItem* list);
		~qinternal_config();

		qtvUIConfigBase* _config() { return m_config; }
		qtvUITabBase* _tab() { return m_tab; }
		qtvUIListItem* _list_item() { return m_listitem; }
	protected:

		qtvUIConfigBase* m_config;
		qtvUITabBase* m_tab;
		qtvUIListItem* m_listitem;
	};

	QList<qinternal_config*> m_configs;
};

#endif
