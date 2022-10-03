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

#ifndef QTV_UI_STORAGE_H
#define QTV_UI_STORAGE_H

#include <vatek_sdk_storage.h>
#include <ui/base/qtv_ui_tab.h>
#include <ui/base/qtv_ui_table.h>
#include <QStackedWidget>
#include <QCheckBox>
#include <QModelIndex>
#include <QSpinBox>
#include <QLineEdit>
#include <QMenu>
#include <QRegularExpressionValidator>

namespace Ui {
	class qtvUIStorageInfo;
	class qtvUIStorageProps;
	class qtvUIStorageR2Tune;
}

class QTV_DECL qtvUIMainStorage : public qtvUIMain
{
	Q_OBJECT
public:
	explicit qtvUIMainStorage();

	void setStorage(hvatek_storage hstorage);
	hvatek_storage& _storage_handle() { return m_hstorage; }
protected:
	hvatek_storage m_hstorage;
};

class QTV_DECL qtvUIListStorage : public qtvUIListConfig
{
	Q_OBJECT
public:
	explicit qtvUIListStorage(QWidget* parent = nullptr);
	virtual ~qtvUIListStorage();

	void attachStacked(QStackedWidget* stacked);
	void refreshStorage(hvatek_storage hstorage);

protected:
	qtvUIMainStorage m_main;
	QStackedWidget* m_stacked;
	QWidget* m_empty;
	hvatek_storage m_hstorage;

	static qtvUIListItem* createInformation(qtvUIMainStorage* main,QWidget* parent);
	static qtvUIListItem* createServiceProperties(qtvUIMainStorage* main, QWidget* parent);
	static qtvUIListItem* createR2Tune(qtvUIMainStorage* main, QWidget* parent);
	static qtvUIListItem* createChipConfig(qtvUIMainStorage* main, QWidget* parent);
	static qtvUIListItem* createBootlogo(qtvUIMainStorage* main, QWidget* parent);
protected Q_SLOTS:
	void recvItemSelectChanged();
};

class QTV_DECL qtvUIUsbIDEditor : public QSpinBox
{
	Q_OBJECT
public:
	qtvUIUsbIDEditor(QWidget* parent = nullptr);
	virtual ~qtvUIUsbIDEditor();

	void setValuePtr(uint32_t* id);

protected:
	QRegularExpressionValidator m_validator;
	QString m_format;
	uint32_t* m_value;

	QValidator::State validate(QString& text, int& pos)const override;
	int valueFromText(const QString& text)const override;
	QString textFromValue(int value)const override;

protected Q_SLOTS:
	void recvValueChanged(int val);
};

class QTV_DECL qtvUIUSBStringEditor : public QLineEdit
{
	Q_OBJECT
public:
	qtvUIUSBStringEditor(QWidget* parent = nullptr);
	virtual ~qtvUIUSBStringEditor();
	void setStringPtr(char* ptrstr, int32_t len);

protected:
	char* m_ptrstr;
	int32_t m_len;

protected Q_SLOTS:
	void recvEditFinish();
};

class QTV_DECL qtvUIStorageInfo : public qtvUITabBase
{
	Q_OBJECT
public:
	explicit qtvUIStorageInfo(qtvUIMain* main, QWidget* parent = nullptr);
	virtual ~qtvUIStorageInfo();

	void tabEnter() override;
	void tabFinish() override;
	void tabLeave() override;

	void device_service_get();

protected:

	typedef enum _sinfo_mode
	{
		smode_hex,
		smode_arch,
		smode_service,
		smode_build,
	}sinfo_mode;

	typedef struct _sinfo_item
	{
		const char* name;
		sinfo_mode mode;
		int32_t offect;
	}sinfo_item, * Psinfo_item;

	Ui::qtvUIStorageInfo* ui;

	void insertItem(int32_t pos, const Psinfo_item sitem, uint8_t* raw);
	static void resetTable(QTableWidget* table);
};

typedef enum _storage_props_mode
{
	sprops_properties,
	sprops_config,
}storage_props_mode;

typedef enum _storage_data_mode
{
	sprops_data_main = 0x00000001,
	sprops_data_modall = sprops_data_main,
	sprops_data_sub = 0x00000002,
	sprops_data_mainsub = (sprops_data_main | sprops_data_sub),
	sprops_data_chipcfg = 0x00000004,
	sprops_data_bin	= 0x00000008,
	sprops_data_mainbin = (sprops_data_main | sprops_data_bin),
}storage_data_mode;

class QTV_DECL StoragePropsItem : public QObject
{
	Q_OBJECT
public:
	virtual QList<StoragePropsItem*>& _sub_items() = 0;
	virtual QString& _name() = 0;
};

class QTV_DECL StoragePropsData : public QObject
{
	Q_OBJECT
public:
	virtual QList<StoragePropsItem*>& _main_items() = 0;
	virtual QString& _name() = 0;
	virtual QStringList& _titles() = 0;
	virtual storage_data_mode _data_mode() = 0;
	virtual qtvPropertyCollection* _main_data() = 0;
	virtual qtvPropertyCollection* _sub_data() = 0;
	virtual Pchip_config _chip_config() = 0;
	virtual Pr2_tune_handle _r2tune() = 0;

	virtual void selectItem(StoragePropsItem* item) = 0;
	virtual int32_t getBinData(uint8_t** data) = 0;

Q_SIGNALS:
	void notifyTitlesChanged();
	void notifyDataChanged(storage_data_mode mode);
};

class QTV_DECL qtvUIStorageProps : public qtvUITabBase
{
	Q_OBJECT
public:
	explicit qtvUIStorageProps(qtvUIMain* main, storage_props_mode mode, QWidget* parent = nullptr);
	virtual ~qtvUIStorageProps();

	void tabEnter() override;
	void tabFinish() override;
	void tabLeave() override;

protected:
	Ui::qtvUIStorageProps* ui;
	storage_props_mode m_mode;
	StoragePropsData* m_data;
	QCheckBox* m_cbgpio[8];

	void setData(StoragePropsData* data);

	void connectGpio();
	void disconnectGpio();
	void setGpioEnable(bool benable);
	uint8_t getGpio();
	void setGpio(uint8_t gpio);

	void loadChipConfig();

protected Q_SLOTS:
	void recvSelectMainChanged(int idx);
	void recvSelectSub0Changed(int idx);
	void recvSelectSub1Changed(int idx);
	void recvGpioSelectStateChanged(int idx);

	void recvDCBlockStateChanged(int state);
	void recvR2BlockStateChanged(int state);
	void recvVSYNCStateChanged(int state);
	void recvUSBIDStateChanged(int state);
	void recvUSBStrStateChanged(int state);
	void recvStateLEDStateChanged(int state);
	void recvGpioStateChanged(int state);

	void recvDataTitleChanged();
	void recvDataChanged(storage_data_mode mode);

	void import_modulation(bool checked);
};

static const qui_table_field r2tune_fields_normal[] =
{
	{"Frequency",5,},
	{"PA",4,},
	{"Filter",10,},
};

#define R2TUNE_FIELD_NORMAL_NUMS	(sizeof(r2tune_fields_normal)/sizeof(qui_table_field))

static const qui_table_field r2tune_fields_advance[] =
{
	{"Frequency",4,},
	{"I",2,},
	{"Q",2,},
	{"Image",2,},
	{"Phase",2,},
	{"PA",4,},
	{"Filter",3},
};

#define R2TUNE_FIELD_ADVANCE_NUMS	(sizeof(r2tune_fields_advance)/sizeof(qui_table_field))

typedef enum _qtv_r2tune_mode
{
	qr2tune_normal,
	qr2tune_advance,
}qtv_r2tune_mode;

class QTV_DECL qtvUITableR2Tune : public QTableWidget
{
	Q_OBJECT
public:
	explicit qtvUITableR2Tune(QWidget* parent = nullptr);
	virtual ~qtvUITableR2Tune();

	qtv_r2tune_mode _mode() { return m_mode; }
	void setR2Tune(qtv_r2tune_mode mode,int32_t ruleidx,Pr2_tune_handle pr2tune);

	Pr2_tune_rule getRule(const QModelIndex* idx);
	void updateRule(const QModelIndex* idx);

protected:
	Pr2_tune_handle m_r2tune;
	qtv_r2tune_mode m_mode;
	QSize m_size;
	int32_t m_ruleindex;
	QMenu m_r2menu;
	QAction m_menuadd;
	QAction m_menudel;
	int32_t m_menuidx;

	void resizeTable();
	void resizeEvent(QResizeEvent* event) override;
	void insertItem(int32_t row, Pr2_tune_item pr2);

protected Q_SLOTS:
	void recvContextMenuRequested(const QPoint& pos);
	void recvMenuAdded(bool checked);
	void recvMenuDelete(bool checked);
};

class QTV_DECL qtvUIStorageR2Tune : public qtvUITabBase
{
	Q_OBJECT
public:
	explicit qtvUIStorageR2Tune(qtvUIMain* main, QWidget* parent = nullptr);
	virtual ~qtvUIStorageR2Tune();

	void tabEnter() override;
	void tabFinish() override;
	void tabLeave() override;
	static void setpathInit();
	//void recvOpenFile_new(QString path);

	public slots:
		void recvBtnImportClick(bool bclick);


protected:
	Pr2_tune_handle m_r2tune;

	qtv_r2tune_mode m_mode;
	Ui::qtvUIStorageR2Tune* ui;

protected Q_SLOTS:
	void recvEnCalibrationStateChanged(int state);
	void recvPathSelectedChanged(int);
	void recvBtnResetClick(bool bclick);
	void recvBtnExportClick(bool bclick);


};

extern QString all_path;

#endif
