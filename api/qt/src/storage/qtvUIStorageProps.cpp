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

#include <ui/base/qtv_ui_storage.h>
#include "ui_qtvUIStorageProps.h"
#include "../internal/qinternal_properties.h"
#include <core/ui/ui_props/ui_props_encoder.h>
#include <core/base/device_usb.h>

#define QSTR_QMATRIX_INTAR		"Intra"
#define QSTR_QMATRIX_INTER		"Inter"

#define QSTR_QMATRIX_INTAR_4_L	"Intra_4_luma"
#define QSTR_QMATRIX_INTAR_4_C	"Intra_4_chroma"
#define QSTR_QMATRIX_INTER_4_L	"Inter_4_luma"
#define QSTR_QMATRIX_INTER_4_C	"Inter_4_chroma"

#define QSTR_QMATRIX_INTAR_8	"Intra_8"
#define QSTR_QMATRIX_INTER_8	"Inter_8"

#define QSTR_RES_SD				"SD"
#define QSTR_RES_HD				"HD"
#define QSTR_RES_FULLHD			"Full-HD"

#define QSTR_ITEM_MOD			"modulation"
#define QSTR_ITEM_AUDIO_CODEC	"audio codec"
#define QSTR_ITEM_MPEG2_CODEC	"mpeg2 codec"
#define QSTR_ITEM_H264_CODEC	"h264 codec"

typedef enum _qisporp_type
{
	qisprop_unknown,
	qisprop_modulation,
	qisprop_audiocodec,
	qisprop_mpeg2codec,
	qisprop_h264codec,
	qisprop_resolution,
	qisprop_chipconfig,
	qisprop_modconfig,
	qisprop_srrc,
	qisprop_dvb_t,
	qisprop_j83a,
	qisprop_atsc,
	qisprop_j83b,
	qisprop_dtmb,
	qisprop_isdb_t,
	qisprop_dvb_t2,
	qisprop_srrctype,
	qisprop_modtype,
	qisprop_ffttype,
	qisprop_carriermode,
	qisprop_qam,
	qisprop_qmatrix,
	qisprop_chiparch,
}qisporp_type;

typedef struct _qisprop_entry
{
	const char* name;
	qisporp_type type;
	const struct _qisprop_entry* props;
	uint32_t value;
}qisprop_entry, * Pqisprop_entry;

class qisprop_property : public qi_property
{
public:
	~qisprop_property() { free(m_uiprop); }

	static qisprop_property* create(const char* name, uint32_t* prtval)
	{
		Pui_prop_item newitem = (Pui_prop_item)malloc(sizeof(ui_prop_item));
		if (newitem)
		{
			memset(newitem, 0, sizeof(ui_prop_item));
			newitem->name = name;
			newitem->desc = name;
			newitem->offset = 0;
			newitem->type = prop_type_hex32;
			return new qisprop_property(newitem, prtval);
		}
		return NULL;
	}

protected:
	qisprop_property(Pui_prop_item puiprop, uint32_t* ptrval) :
		m_uiprop(puiprop),
		qi_property(puiprop, (uint8_t*)ptrval)
	{

	}

	Pui_prop_item m_uiprop;
};

class qisprops_modconfig : public qtvPropertyCollection
{
public:
	qisprops_modconfig() {}
	void resetProperties() override {}

	static qisprops_modconfig* createDVBTMain(Pdvb_t_config pdvbt);
	static qisprops_modconfig* createDVBTMain_B2(Pdvb_t_config pdvbt);
	static qisprops_modconfig* createJ83AMain(Pj83a_config pj83a);
	static qisprops_modconfig* createATSCMain(Patsc_config patsc);
	static qisprops_modconfig* createJ83BMain(Pj83b_config pj83b);
	static qisprops_modconfig* createDTMBMain(Pdtmb_config pdtmb);
	static qisprops_modconfig* createDTMBMain_B2(Pdtmb_config pdtmb);
	static qisprops_modconfig* createISDBTMain(Pisdb_t_config pisdbt);
	static qisprops_modconfig* createISDBTMain_B2(Pisdb_t_config pisdbt);
	static qisprops_modconfig* createDVBT2Main(Pdvb_t2_config pdvbt2);

	void insertProperty(const char* name, uint32_t* ptrval)
	{
		m_properties.append(qisprop_property::create(name, ptrval));
	}

protected:
	void refreshProperties() override {}
};

class qisprops_base;
class qisprops_item : public StoragePropsItem
{
public:
	qisprops_item(const Pqisprop_entry entry, qisprops_base* base) :
		m_name(entry->name), m_base(base), m_entry(entry)
	{
		reloadItems();
	}

	~qisprops_item() { clearItems(); }

	QList<StoragePropsItem*>& _sub_items() override { return m_items; }
	QString& _name() override { return m_name; }

	const Pqisprop_entry _entry() { return m_entry; }

protected:
	QString m_name;
	qisprops_base* m_base;
	const Pqisprop_entry m_entry;
	QList<StoragePropsItem*> m_items;

	virtual void reloadItems()
	{
		if (m_entry->props)
		{
			int32_t pos = 0;
			while (m_entry->props[pos].name)
			{
				m_items.append(new qisprops_item((const Pqisprop_entry)&m_entry->props[pos++], m_base));
			}
		}
	}

	void clearItems()
	{
		while (m_items.size())
		{
			delete m_items.first();
			m_items.removeFirst();
		}
	}
};

class qisprops_base : public StoragePropsData
{
public:
	qisprops_base(const char* name, const Pqisprop_entry pentry) :
		m_name(name),
		m_datamode(sprops_data_mainsub),
		m_maindata(NULL),
		m_subdata(NULL),
		m_hexbuf(NULL),
		m_chipcfg(NULL),
		m_r2tune(NULL),
		m_hexlen(-1)
	{
		reloadItems(pentry);
	}

	~qisprops_base()
	{
		updateMain(NULL);
		updateSub(NULL);
		clearItems();
	}

	QString& _name() override { return m_name; }
	QStringList& _titles() override { return m_titles; }
	QList<StoragePropsItem*>& _main_items() override { return m_items; }
	storage_data_mode _data_mode() override { return m_datamode; }
	qtvPropertyCollection* _main_data() override { return m_maindata; }
	qtvPropertyCollection* _sub_data() override { return m_subdata; }
	Pchip_config _chip_config() override { return m_chipcfg; }
	Pr2_tune_handle _r2tune() override { return m_r2tune; }

	int32_t getBinData(uint8_t** data) override { *data = m_hexbuf; return m_hexlen; }

protected:
	QString m_name;
	QStringList m_titles;
	QList<StoragePropsItem*> m_items;
	storage_data_mode m_datamode;
	qtvPropertyCollection* m_maindata;
	qtvPropertyCollection* m_subdata;
	Pchip_config m_chipcfg;
	Pr2_tune_handle m_r2tune;
	int32_t m_hexlen;
	uint8_t* m_hexbuf;

	void updateMain(qtvPropertyCollection* maindata)
	{
		qtvPropertyCollection* prev = m_maindata;
		m_maindata = maindata;
		emit notifyDataChanged(sprops_data_main);
		if (prev)delete prev;
	}

	void updateSub(qtvPropertyCollection* subdata)
	{
		qtvPropertyCollection* prev = m_subdata;
		m_subdata = subdata;
		emit notifyDataChanged(sprops_data_sub);
		if (prev)delete prev;
	}

	void updateChipConfig(Pchip_config pchip)
	{
		m_chipcfg = pchip;
		emit notifyDataChanged(sprops_data_chipcfg);
	}

	void updateBin(uint8_t* pbuf, int32_t len)
	{
		m_hexbuf = pbuf;
		if (!m_hexbuf)m_hexlen = -1;
		else m_hexlen = len;
		emit notifyDataChanged(sprops_data_bin);
	}

	void updateTitle(const char* main, const char* sub0, const char* sub1, storage_data_mode mode)
	{
		m_titles.clear();
		if (main)m_titles.append(QString(main));
		if (sub0)m_titles.append(QString(sub0));
		if (sub1)m_titles.append(QString(sub1));
		m_datamode = mode;
		emit notifyTitlesChanged();
	}

	virtual void reloadItems(const Pqisprop_entry entry)
	{
		int32_t pos = 0;
		clearItems();
		while (entry[pos].name)
			m_items.append(new qisprops_item(&entry[pos++], this));
	}

	void clearItems()
	{
		while (m_items.size())
		{
			delete m_items.first();
			m_items.removeFirst();
		}
	}
};

extern const qisprop_entry broadcast_entries[];
extern const qisprop_entry transform_entries[];
extern const qisprop_entry config_entries[];

class qisprops_broadcast : public qisprops_base
{
public:
	qisprops_broadcast(Pstorage_broadcast pbc) :
		qisprops_base("Broadcast", (const Pqisprop_entry)&broadcast_entries[0]),
		m_broadcast(pbc),
		m_proptype(qisprop_unknown),
		m_vcodec(encvideo_unknown),
		m_resourceidx(-1),
		m_qmatrix(-1)
	{
		updateTitle("Broadcast : ", NULL, NULL, sprops_data_main);
	}
	void selectItem(StoragePropsItem* item) override;

protected:
	Pstorage_broadcast m_broadcast;
	qisporp_type m_proptype;
	video_codec m_vcodec;
	int32_t m_resourceidx;
	int32_t m_qmatrix;
};

class qisprops_transform : public qisprops_base
{
public:
	qisprops_transform(Pstorage_transform ptr) :
		qisprops_base("Transform", (const Pqisprop_entry)&transform_entries[0]),
		m_transform(ptr),
		m_proptype(qisprop_unknown)
	{
		updateTitle("Transform : ", NULL, NULL, sprops_data_main);
	}
	void selectItem(StoragePropsItem* item) override;

protected:
	Pstorage_transform m_transform;
	qisporp_type m_proptype;
};

class qisprops_config : public qisprops_base
{
public:
	qisprops_config(Pstorage_chip_config pcfg) :
		qisprops_base("Chip Configs", (const Pqisprop_entry)&config_entries[0]),
		m_chipcfg(pcfg),
		m_edittype(modulator_unknown),
		m_srrcindex(-1),
		m_chipidx(-1)
	{
		updateTitle("Chip Configs : ", NULL, NULL, sprops_data_main);
	}
	void selectItem(StoragePropsItem* item) override;

protected:
	Pstorage_chip_config m_chipcfg;
	modulator_type m_edittype;
	int32_t m_srrcindex;
	int32_t m_chipidx;

	void updateModulation();
};

/* qtvUIStorageProps */

qtvUIStorageProps::qtvUIStorageProps(qtvUIMain* main, storage_props_mode mode, QWidget* parent) :
	qtvUITabBase(main, "Storage Properties", "Storage Properties", parent),
	ui(new Ui::qtvUIStorageProps),
	m_mode(mode),
	m_data(NULL)
{
	ui->setupUi(this);
	ui->gbchip->setVisible(false);
	ui->gbhex->setVisible(false);
	ui->sbusbvendor->setEnabled(false);
	ui->sbusbproductid->setEnabled(false);
	ui->txtusbstr->setEnabled(false);
	ui->cbledstatus->setEnabled(false);
	ui->cbledstatus->addItem("idle");
	ui->cbledstatus->addItem("running");
	ui->cbledstatus->addItem("error");
	ui->cbledstatus->setCurrentIndex(0);

	connectGpio();
	setGpioEnable(false);

	connect(ui->cb_main, SIGNAL(currentIndexChanged(int)), this, SLOT(recvSelectMainChanged(int)));
	connect(ui->cb_title0, SIGNAL(currentIndexChanged(int)), this, SLOT(recvSelectSub0Changed(int)));
	connect(ui->cb_title1, SIGNAL(currentIndexChanged(int)), this, SLOT(recvSelectSub1Changed(int)));
	connect(ui->cbledstatus, SIGNAL(currentIndexChanged(int)), this, SLOT(recvGpioSelectStateChanged(int)));

	connect(ui->chdcblock, SIGNAL(stateChanged(int)), this, SLOT(recvDCBlockStateChanged(int)));
	connect(ui->chr2block, SIGNAL(stateChanged(int)), this, SLOT(recvR2BlockStateChanged(int)));
	connect(ui->cb_vsync, SIGNAL(stateChanged(int)), this, SLOT(recvVSYNCStateChanged(int)));
	connect(ui->chusbid, SIGNAL(stateChanged(int)), this, SLOT(recvUSBIDStateChanged(int)));
	connect(ui->chusbstr, SIGNAL(stateChanged(int)), this, SLOT(recvUSBStrStateChanged(int)));
	connect(ui->cbenstatusled, SIGNAL(stateChanged(int)), this, SLOT(recvStateLEDStateChanged(int)));
	setData(NULL);
}

qtvUIStorageProps::~qtvUIStorageProps()
{
	setData(NULL);
	disconnect(ui->cb_main, SIGNAL(currentIndexChanged(int)), this, SLOT(recvSelectMainChanged(int)));
	disconnect(ui->cb_title0, SIGNAL(currentIndexChanged(int)), this, SLOT(recvSelectSub0Changed(int)));
	disconnect(ui->cb_title1, SIGNAL(currentIndexChanged(int)), this, SLOT(recvSelectSub1Changed(int)));
	disconnect(ui->cbledstatus, SIGNAL(currentIndexChanged(int)), this, SLOT(recvGpioSelectStateChanged(int)));

	disconnect(ui->chdcblock, SIGNAL(stateChanged(int)), this, SLOT(recvDCBlockStateChanged(int)));
	disconnect(ui->chr2block, SIGNAL(stateChanged(int)), this, SLOT(recvR2BlockStateChanged(int)));
	disconnect(ui->cb_vsync, SIGNAL(stateChanged(int)), this, SLOT(recvVSYNCStateChanged(int)));
	disconnect(ui->chusbid, SIGNAL(stateChanged(int)), this, SLOT(recvUSBIDStateChanged(int)));
	disconnect(ui->chusbstr, SIGNAL(stateChanged(int)), this, SLOT(recvUSBStrStateChanged(int)));
	disconnect(ui->cbenstatusled, SIGNAL(stateChanged(int)), this, SLOT(recvStateLEDStateChanged(int)));
	disconnectGpio();
	delete ui;
}

void qtvUIStorageProps::tabEnter()
{
	if (qtvUIMain::is_uimain_storage(m_main->_mode()))
	{
		qtvUIMainStorage* smain = qobject_cast<qtvUIMainStorage*>(m_main);
		vatek_result nres = vatek_success;
		if (m_mode == sprops_properties)
		{
			Papp_header papp = NULL;
			nres = vatek_storage_get_app(smain->_storage_handle(), &papp);
			if (is_vatek_success(nres))
			{
				if (papp->service_mode == service_broadcast)
					setData(new qisprops_broadcast(vatek_storage_get_broadcast(smain->_storage_handle())));
				else if (papp->service_mode == service_transform)
					setData(new qisprops_transform(vatek_storage_get_transform(smain->_storage_handle())));
				else nres = vatek_unsupport;
			}
		}
		else if (m_mode == sprops_config)
			setData(new qisprops_config(vatek_storage_get_config(smain->_storage_handle())));
		else nres = vatek_unsupport;

		if (!is_vatek_success(nres))VWAR("create storage properties fail : %d", nres);
	}
}

void qtvUIStorageProps::tabFinish()
{

}

void qtvUIStorageProps::tabLeave()
{
	if (qtvUIMain::is_uimain_storage(m_main->_mode()))
	{
		setData(NULL);
	}
}

void qtvUIStorageProps::setData(StoragePropsData* data)
{
	ui->cb_main->clear();
	ui->cb_title0->clear();
	ui->cb_title1->clear();
	ui->lb_main->setText("");
	ui->lb_title0->setText("");
	ui->lb_title1->setText("");
	ui->cb_main->setEnabled(false);
	ui->cb_title0->setEnabled(false);
	ui->cb_title1->setEnabled(false);

	if (m_data)
	{
		ui->tablemain->setProperties(qui_table_normal, NULL);
		ui->tablesub->setProperties(qui_table_normal, NULL);
		disconnect(m_data, SIGNAL(notifyTitlesChanged()), this, SLOT(recvDataTitleChanged()));
		disconnect(m_data, SIGNAL(notifyDataChanged(storage_data_mode)), this, SLOT(recvDataChanged(storage_data_mode)));
		delete m_data;
	}
	m_data = data;

	if (m_data)
	{
		int32_t i = 0;
		ui->lb_main->setText(m_data->_name());
		for (i = 0; i < m_data->_main_items().size(); i++)
		{
			StoragePropsItem* item = m_data->_main_items()[i];
			ui->cb_main->addItem(item->_name(), QVariant::fromValue<StoragePropsItem*>(item));
		}

		connect(m_data, SIGNAL(notifyTitlesChanged()), this, SLOT(recvDataTitleChanged()));
		connect(m_data, SIGNAL(notifyDataChanged(storage_data_mode)), this, SLOT(recvDataChanged(storage_data_mode)));
		recvDataTitleChanged();

		if (ui->cb_main->count())
		{
			ui->cb_main->setEnabled(true);
			recvSelectMainChanged(0);
		}
		else ui->cb_main->setEnabled(false);
	}
}

void qtvUIStorageProps::connectGpio()
{
	int32_t i = 0;
	m_cbgpio[0] = ui->cbpin0;
	m_cbgpio[1] = ui->cbpin1;
	m_cbgpio[2] = ui->cbpin2;
	m_cbgpio[3] = ui->cbpin3;
	m_cbgpio[4] = ui->cbpin4;
	m_cbgpio[5] = ui->cbpin5;
	m_cbgpio[6] = ui->cbpin6;
	m_cbgpio[7] = ui->cbpin7;

	for (i = 0; i < 8; i++)
		connect(m_cbgpio[i], SIGNAL(stateChanged(int)), this, SLOT(recvGpioStateChanged(int)));
}

void qtvUIStorageProps::disconnectGpio()
{
	int32_t i = 0;
	for (i = 0; i < 8; i++)
		disconnect(m_cbgpio[i], SIGNAL(stateChanged(int)), this, SLOT(recvGpioStateChanged(int)));
}

void qtvUIStorageProps::setGpioEnable(bool benable)
{
	int32_t i = 0;
	for (i = 0; i < 8; i++)
		m_cbgpio[i]->setEnabled(false);

	m_cbgpio[0]->setEnabled(benable);
	m_cbgpio[1]->setEnabled(benable);
	m_cbgpio[2]->setEnabled(benable);
}

uint8_t qtvUIStorageProps::getGpio()
{
	int32_t i = 0;
	uint8_t tag = 0;
	uint8_t val = 0;
	for (i = 0; i < 8; i++)
	{
		tag = 1 << i;
		if (m_cbgpio[i]->checkState() == Qt::Checked)
			val |= tag;
	}
	return val;
}

void qtvUIStorageProps::setGpio(uint8_t gpio)
{
	int32_t i = 0;
	uint8_t tag = 0;
	for (i = 0; i < 8; i++)
	{
		tag = 1 << i;
		m_cbgpio[i]->setChecked(((gpio & tag) != 0));
	}
}

void qtvUIStorageProps::loadChipConfig()
{
	if (m_data)
	{
		Pchip_config pcfg = m_data->_chip_config();
		ui->chdcblock->setChecked((pcfg->functions & CHIP_EN_DAC_EXTR) != 0);
		ui->chr2block->setChecked((pcfg->functions & CHIP_EN_R2_EXTR) != 0);
		ui->cb_vsync->setChecked((pcfg->vendor_functions & VENDOR_EN_EXT_VSYNC) != 0);
		ui->chusbid->setChecked((pcfg->functions & CHIP_EN_USBID) != 0);
		ui->chusbstr->setChecked((pcfg->functions & CHIP_EN_USBSTR) != 0);
		ui->cbenstatusled->setChecked((pcfg->functions & CHIP_EN_STATUSLED) != 0);
		ui->cbledstatus->setCurrentIndex(0);
	}
	else
	{
		ui->chdcblock->setChecked(false);
		ui->chr2block->setChecked(false);
		ui->cb_vsync->setChecked(false);
		ui->chusbid->setChecked(false);
		ui->chusbstr->setChecked(false);
		ui->cbenstatusled->setChecked(false);
	}

	recvDCBlockStateChanged(ui->chdcblock->checkState());
	recvR2BlockStateChanged(ui->chr2block->checkState());
	recvVSYNCStateChanged(ui->cb_vsync->checkState());
	recvUSBIDStateChanged(ui->chusbid->checkState());
	recvUSBStrStateChanged(ui->chusbstr->checkState());
	recvStateLEDStateChanged(ui->cbenstatusled->checkState());
	recvGpioSelectStateChanged(ui->cbledstatus->currentIndex());
}

void qtvUIStorageProps::recvSelectMainChanged(int idx)
{
	ui->cb_title0->clear();
	if (ui->cb_main->currentIndex() != -1)
	{
		StoragePropsItem* pitem = qvariant_cast<StoragePropsItem*>(ui->cb_main->itemData(ui->cb_main->currentIndex()));
		m_data->selectItem(pitem);
		if (pitem->_sub_items().size())
		{
			int32_t i = 0;
			for (i = 0; i < pitem->_sub_items().count(); i++)
			{
				StoragePropsItem* subitem = pitem->_sub_items()[i];
				ui->cb_title0->addItem(subitem->_name(), QVariant::fromValue<StoragePropsItem*>(subitem));
			}
			ui->cb_title0->setEnabled(true);
			recvSelectSub0Changed(0);
		}
		else ui->cb_title0->setEnabled(false);
	}
}

void qtvUIStorageProps::import_modulation(bool checked)
{

}

void qtvUIStorageProps::recvSelectSub0Changed(int idx)
{
	ui->cb_title1->clear();
	if (ui->cb_title0->currentIndex() != -1)
	{
		StoragePropsItem* pitem = qvariant_cast<StoragePropsItem*>(ui->cb_title0->itemData(ui->cb_title0->currentIndex()));
		m_data->selectItem(pitem);
		if (pitem->_sub_items().size())
		{
			int32_t i = 0;
			for (i = 0; i < pitem->_sub_items().count(); i++)
			{
				StoragePropsItem* subitem = pitem->_sub_items()[i];
				ui->cb_title1->addItem(subitem->_name(), QVariant::fromValue<StoragePropsItem*>(subitem));
			}
			ui->cb_title1->setEnabled(true);
			recvSelectSub1Changed(0);
		}
		else ui->cb_title1->setEnabled(false);
	}
}

void qtvUIStorageProps::recvSelectSub1Changed(int idx)
{
	if (ui->cb_title1->currentIndex() != -1)
	{
		StoragePropsItem* pitem = qvariant_cast<StoragePropsItem*>(ui->cb_title1->itemData(ui->cb_title1->currentIndex()));
		m_data->selectItem(pitem);
	}
}

void qtvUIStorageProps::recvDataTitleChanged()
{
	ui->lb_main->setText("");
	ui->lb_title0->setText("");
	ui->lb_title1->setText("");

	ui->lb_main->setVisible(false);
	ui->cb_main->setVisible(false);

	ui->lb_title0->setVisible(false);
	ui->cb_title0->setVisible(false);

	ui->lb_title1->setVisible(false);
	ui->cb_title1->setVisible(false);

	ui->tablemain->setVisible(false);
	ui->tablesub->setVisible(false);
	ui->gbchip->setVisible(false);
	ui->gbhex->setVisible(false);

	if (m_data)
	{
		int32_t len = m_data->_titles().count();
		if (len > 0)ui->lb_main->setText(m_data->_titles()[0]);
		if (len > 1)ui->lb_title0->setText(m_data->_titles()[1]);
		if (len > 2)ui->lb_title1->setText(m_data->_titles()[2]);

		ui->lb_main->setVisible((len > 0));
		ui->cb_main->setVisible((len > 0));
		ui->lb_title0->setVisible((len > 1));
		ui->cb_title0->setVisible((len > 1));
		ui->lb_title1->setVisible((len > 2));
		ui->cb_title1->setVisible((len > 2));

		ui->tablemain->setVisible((m_data->_data_mode() & sprops_data_main) != 0);
		ui->tablesub->setVisible((m_data->_data_mode() & sprops_data_sub) != 0);
		ui->gbchip->setVisible((m_data->_data_mode() & sprops_data_chipcfg) != 0);
		//ui->gbhex->setVisible((m_data->_data_mode() & sprops_data_bin) != 0);
	}
}

void qtvUIStorageProps::recvDataChanged(storage_data_mode mode)
{
	if (m_data)
	{
		if (mode == sprops_data_main)
			ui->tablemain->setProperties(qui_table_normal, m_data->_main_data());
		else if (mode == sprops_data_sub)
			ui->tablesub->setProperties(qui_table_normal, m_data->_sub_data());
		else if (mode == sprops_data_chipcfg)
		{
			loadChipConfig();
		}
		else if (mode == sprops_data_bin)
		{
			uint8_t* ptrbuf = NULL;
			int32_t len = m_data->getBinData(&ptrbuf);

			ui->hexcntl->setHexBuffer(ptrbuf, len);
		}
	}
}

void qtvUIStorageProps::recvDCBlockStateChanged(int state)
{
	bool bchecked = (ui->chdcblock->checkState() == Qt::Checked);
	if (m_data)
	{
		Pchip_config pcfg = m_data->_chip_config();
		pcfg->functions &= ~CHIP_EN_DAC_EXTR;
		if (bchecked)pcfg->functions |= CHIP_EN_DAC_EXTR;
	}
}

void qtvUIStorageProps::recvR2BlockStateChanged(int state)
{
	bool bchecked = (ui->chr2block->checkState() == Qt::Checked);
	if (m_data)
	{
		Pchip_config pcfg = m_data->_chip_config();
		pcfg->functions &= ~CHIP_EN_R2_EXTR;
		if (bchecked)pcfg->functions |= CHIP_EN_R2_EXTR;
	}
}

void qtvUIStorageProps::recvVSYNCStateChanged(int state)
{
	bool bchecked = (ui->cb_vsync->checkState() == Qt::Checked);
	if (m_data)
	{
		Pchip_config pcfg = m_data->_chip_config();
		pcfg->vendor_functions &= ~VENDOR_EN_EXT_VSYNC;
		if (bchecked)pcfg->vendor_functions |= VENDOR_EN_EXT_VSYNC;
	}
}

void qtvUIStorageProps::recvUSBIDStateChanged(int state)
{
	bool bchecked = (ui->chusbid->checkState() == Qt::Checked);
	ui->sbusbvendor->setEnabled(bchecked);
	ui->sbusbproductid->setEnabled(bchecked);

	if (m_data)
	{
		Pchip_config pcfg = m_data->_chip_config();
		if (!pcfg->usb_vid)pcfg->usb_vid = USBDEV_VID;
		if (!pcfg->usb_pid)pcfg->usb_pid = 0x1FF0;
		ui->sbusbvendor->setValuePtr(&pcfg->usb_vid);
		ui->sbusbproductid->setValuePtr(&pcfg->usb_pid);
		pcfg->functions &= ~CHIP_EN_USBID;
		if (bchecked)pcfg->functions |= CHIP_EN_USBID;
	}
}

void qtvUIStorageProps::recvUSBStrStateChanged(int state)
{
	bool bchecked = (ui->chusbstr->checkState() == Qt::Checked);
	ui->txtusbstr->setEnabled(bchecked);

	if (m_data)
	{
		Pchip_config pcfg = m_data->_chip_config();
		if (!strlen(&pcfg->usb_str[0]))
			strncpy(&pcfg->usb_str[0], "VAT-DEVICE", 16);
		ui->txtusbstr->setStringPtr(&pcfg->usb_str[0], 16);
		pcfg->functions &= ~CHIP_EN_USBSTR;
		if (bchecked)pcfg->functions |= CHIP_EN_USBSTR;
	}
}

void qtvUIStorageProps::recvStateLEDStateChanged(int state)
{
	bool bchecked = (ui->cbenstatusled->checkState() == Qt::Checked);
	if (m_data)
	{
		Pchip_config pcfg = m_data->_chip_config();
		pcfg->functions &= ~CHIP_EN_STATUSLED;
		if (bchecked)pcfg->functions |= CHIP_EN_STATUSLED;
	}
	ui->cbledstatus->setEnabled(bchecked);
	setGpioEnable(bchecked);
}

void qtvUIStorageProps::recvGpioStateChanged(int state)
{
	if (m_data)
	{
		uint32_t value = getGpio();
		uint32_t make = 0xFFFFFF00;
		Pchip_config pcfg = m_data->_chip_config();

		if (ui->cbledstatus->currentIndex() == 1)
		{
			make = 0xFFFF00FF;
			value <<= 8;
		}
		else if (ui->cbledstatus->currentIndex() == 2)
		{
			make = 0xFF00FFFF;
			value <<= 16;
		}
		pcfg->status_led &= make;
		pcfg->status_led |= value;
	}
}

void qtvUIStorageProps::recvGpioSelectStateChanged(int idx)
{
	uint8_t val = 0;
	if (m_data)
	{
		Pchip_config pcfg = m_data->_chip_config();
		val = (uint8_t)(pcfg->status_led & 0xFF);
		if (ui->cbledstatus->currentIndex() == 1)
			val = (uint8_t)((pcfg->status_led >> 8) & 0xFF);
		else if (ui->cbledstatus->currentIndex() == 2)
			val = (uint8_t)((pcfg->status_led >> 16) & 0xFF);
	}
	setGpio(val);
}

/* qisprops_broadcast */

void qisprops_broadcast::selectItem(StoragePropsItem* item)
{
	qisprops_item* qitem = static_cast<qisprops_item*>(item);
	const Pqisprop_entry pentry = qitem->_entry();

	if (pentry->type == qisprop_modulation)
	{
		updateTitle("Broadcast : ", NULL, NULL, sprops_data_main);
		updateMain(new qi_modulation_properties(&m_broadcast->modulator));
	}
	else if (pentry->type == qisprop_audiocodec)
	{
		updateTitle("Broadcast : ", NULL, NULL, sprops_data_main);
		updateMain(new qi_properties_simple(_ui_struct(audio_param), (uint8_t*)&m_broadcast->audio));
	}
	else if (pentry->type == qisprop_mpeg2codec)
	{
		m_vcodec = encvideo_mpeg2;
		m_qmatrix = -1;
		m_resourceidx = -1;
		updateTitle("Broadcast : ", "Resolution : ", "Quantization Matrix : ", sprops_data_mainbin);
		updateMain(NULL);
	}
	else if (pentry->type == qisprop_h264codec)
	{
		m_vcodec = encvideo_h264;
		m_qmatrix = -1;
		m_resourceidx = -1;
		updateTitle("Broadcast : ", "Resolution : ", "Quantization Matrix : ", sprops_data_mainbin);
		updateMain(NULL);
	}
	else if (pentry->type == qisprop_resolution)
	{
		m_resourceidx = (int32_t)pentry->value;
		if (m_resourceidx != -1)
		{
			if (m_vcodec == encvideo_mpeg2)
				updateMain(new qi_properties_simple(_ui_struct(mpeg2_param), (uint8_t*)&m_broadcast->mpeg2[m_resourceidx]));
			else if (m_vcodec == encvideo_h264)
				updateMain(new qi_properties_simple(_ui_struct(h264_param), (uint8_t*)&m_broadcast->h264[m_resourceidx]));
			else updateMain(NULL);
		}
	}
	else if (pentry->type == qisprop_qmatrix)
	{
		m_qmatrix = (int32_t)pentry->value;
		updateBin(NULL, -1);
		if (m_qmatrix != -1 && m_resourceidx != -1)
		{
			if (m_vcodec == encvideo_mpeg2)
			{
				Pmpeg2_param pm2 = &m_broadcast->mpeg2[m_resourceidx];
				if (m_qmatrix == 0)updateBin(&pm2->intra_quant_matrix[0][0], MPEG2_INTRA_LEN);
				else if (m_qmatrix == 1)updateBin(&pm2->inter_quant_matrix[0][0], MPEG2_INTER_LEN);
			}
			else if (m_vcodec == encvideo_h264)
			{
				Ph264_param ph264 = &m_broadcast->h264[m_resourceidx];
				if (m_qmatrix == 0)updateBin(&ph264->intra_4x4.luma[0][0], H264_MATRIX_4_4_LEN);
				else if (m_qmatrix == 1)updateBin(&ph264->intra_4x4.chroma[0][0], H264_MATRIX_4_4_LEN);
				else if (m_qmatrix == 2)updateBin(&ph264->inter_4x4.luma[0][0], H264_MATRIX_4_4_LEN);
				else if (m_qmatrix == 3)updateBin(&ph264->inter_4x4.chroma[0][0], H264_MATRIX_4_4_LEN);
				else if (m_qmatrix == 4)updateBin(&ph264->intra_8x8.luma[0][0], H264_MATRIX_8_8_LEN);
				else if (m_qmatrix == 5)updateBin(&ph264->inter_8x8.luma[0][0], H264_MATRIX_8_8_LEN);
			}
		}
	}
}

/* qisprops_transform */

void qisprops_transform::selectItem(StoragePropsItem* item)
{
	qisprops_item* qitem = static_cast<qisprops_item*>(item);
	const Pqisprop_entry pentry = qitem->_entry();

	if (pentry->type == qisprop_modulation)
	{
		updateTitle("Transform : ", NULL, NULL, sprops_data_main);
		updateMain(new qi_modulation_properties(&m_transform->modulator));
	}
	else updateMain(NULL);
}

/* qisprops_config */

void qisprops_config::selectItem(StoragePropsItem* item)
{
	qisprops_item* qitem = static_cast<qisprops_item*>(item);
	const Pqisprop_entry pentry = qitem->_entry();

	if (pentry->type == qisprop_chipconfig)
	{
		updateTitle("Transform : ", NULL, NULL, sprops_data_chipcfg);
		updateChipConfig(&m_chipcfg->_chip.config);
	}
	else if (pentry->type == qisprop_srrc)
	{
		m_srrcindex = -1;
		updateTitle("Transform : ", "Modulation Mode", NULL, sprops_data_bin);
		updateBin(NULL, -1);
	}
	else if (pentry->type == qisprop_srrctype)
	{
		m_srrcindex = (int32_t)pentry->value;
		if (m_srrcindex != -1)
			updateBin((uint8_t*)&m_chipcfg->srrc[m_srrcindex][0], CONFIG_MOD_SRRC_LEN);
	}
	else if (pentry->type == qisprop_modconfig)
	{
		m_edittype = modulator_unknown;
		m_chipidx = -1;
		updateTitle("Transform : ", "Chip : ", "Modulation : ", sprops_data_modall);
		updateMain(NULL);
		updateSub(NULL);
	}
	else if (pentry->type == qisprop_chiparch)
	{
		m_chipidx = (int32_t)pentry->value;
		updateModulation();
	}
	else if (pentry->type == qisprop_modtype)
	{
		m_edittype = (modulator_type)pentry->value;
		updateModulation();
	}
}

void qisprops_config::updateModulation()
{
	updateMain(NULL);
	if (m_chipidx != -1 && m_edittype != modulator_unknown)
	{
		if (m_edittype == modulator_dvb_t) {
			if (m_chipidx <= 1) {
				updateMain(qisprops_modconfig::createDVBTMain_B2(&m_chipcfg->modulation[m_chipidx].dvb_t));
			}
			else {
				updateMain(qisprops_modconfig::createDVBTMain(&m_chipcfg->modulation[m_chipidx].dvb_t));
			}
		}
		else if (m_edittype == modulator_j83a)
			updateMain(qisprops_modconfig::createJ83AMain(&m_chipcfg->modulation[m_chipidx].j83a));
		else if (m_edittype == modulator_atsc)
			updateMain(qisprops_modconfig::createATSCMain(&m_chipcfg->modulation[m_chipidx].atsc));
		else if (m_edittype == modulator_j83b)
			updateMain(qisprops_modconfig::createJ83BMain(&m_chipcfg->modulation[m_chipidx].j83b));
		else if (m_edittype == modulator_dtmb) {
			if (m_chipidx <= 1) {
				updateMain(qisprops_modconfig::createDTMBMain_B2(&m_chipcfg->modulation[m_chipidx].dtmb));
			}
			else {
				updateMain(qisprops_modconfig::createDTMBMain(&m_chipcfg->modulation[m_chipidx].dtmb));
			}
		}
		else if (m_edittype == modulator_isdb_t) {
			if (m_chipidx <= 1) {
				updateMain(qisprops_modconfig::createISDBTMain_B2(&m_chipcfg->modulation[m_chipidx].isdb_t));
			}
			else {
				updateMain(qisprops_modconfig::createISDBTMain(&m_chipcfg->modulation[m_chipidx].isdb_t));
			}
		}
		else if (m_edittype == modulator_dvb_t2) {
			if (m_chipidx > 1) {
				updateMain(qisprops_modconfig::createDVBT2Main(&m_chipcfg->modulation[m_chipidx].dvb_t2));
			}
		}
	}
}

/* qisprops_modconfig */

qisprops_modconfig* qisprops_modconfig::createDVBTMain(Pdvb_t_config pdvbt)
{
	qisprops_modconfig* newmod = new qisprops_modconfig();
	newmod->insertProperty("scl_0   (0x2202)", &pdvbt->scl_0);
	newmod->insertProperty("scl_1   (0x2203)", &pdvbt->scl_1);
	newmod->insertProperty("scl_2   (0x2204)", &pdvbt->scl_2);
	newmod->insertProperty("fft_scale[2k]   (0x2212) -> 0", &pdvbt->fft_scale[fft_2k]);
	newmod->insertProperty("fft_scale[8k]   (0x2212) -> 1", &pdvbt->fft_scale[fft_8k]);
	newmod->insertProperty("fft_scale[4k]   (0x2212) -> 2", &pdvbt->fft_scale[fft_4k]);
	newmod->insertProperty("gain[2k]   (0x2201)", &pdvbt->gain[fft_2k]);
	newmod->insertProperty("gain[8k]   (0x2201)", &pdvbt->gain[fft_8k]);
	newmod->insertProperty("gain[4k]   (0x2201)", &pdvbt->gain[fft_4k]);
	return newmod;
}

qisprops_modconfig* qisprops_modconfig::createDVBTMain_B2(Pdvb_t_config pdvbt)
{
	qisprops_modconfig* newmod = new qisprops_modconfig();
	newmod->insertProperty("scl_0   (0x2202)", &pdvbt->scl_0);
	newmod->insertProperty("scl_1   (0x2203)", &pdvbt->scl_1);
	newmod->insertProperty("scl_2   (0x2204)", &pdvbt->scl_2);
	newmod->insertProperty("gain[2k]   (0x2201)", &pdvbt->gain[fft_2k]);
	newmod->insertProperty("gain[8k]   (0x2201)", &pdvbt->gain[fft_8k]);
	newmod->insertProperty("gain[4k]   (0x2201)", &pdvbt->gain[fft_4k]);
	return newmod;
}

qisprops_modconfig* qisprops_modconfig::createJ83AMain(Pj83a_config pj83a)
{
	qisprops_modconfig* newmod = new qisprops_modconfig();
	newmod->insertProperty("gain[qam16]   (0x2201) -> 0", &pj83a->gain[j83a_qam16]);
	newmod->insertProperty("gain[qam32]   (0x2201) -> 1", &pj83a->gain[j83a_qam32]);
	newmod->insertProperty("gain[qam64]   (0x2201) -> 2", &pj83a->gain[j83a_qam64]);
	newmod->insertProperty("gain[qam128]   (0x2201) -> 3", &pj83a->gain[j83a_qam128]);
	newmod->insertProperty("gain[qam256]   (0x2201) -> 4", &pj83a->gain[j83a_qam256]);
	return newmod;
}

qisprops_modconfig* qisprops_modconfig::createATSCMain(Patsc_config patsc)
{
	qisprops_modconfig* newmod = new qisprops_modconfig();
	newmod->insertProperty("dc_offect   (0x2202)", &patsc->dc_offect);
	newmod->insertProperty("gain   (0x2201)", &patsc->dac_gain);
	return newmod;
}

qisprops_modconfig* qisprops_modconfig::createJ83BMain(Pj83b_config pj83b)
{
	qisprops_modconfig* newmod = new qisprops_modconfig();
	newmod->insertProperty("gain[qam64]   (0x2201) -> 0", &pj83b->gain_q64);
	newmod->insertProperty("gain[qam256]   (0x2201) -> 1", &pj83b->gain_q256);
	return newmod;
}

qisprops_modconfig* qisprops_modconfig::createDTMBMain(Pdtmb_config pdtmb)
{
	qisprops_modconfig* newmod = new qisprops_modconfig();
	newmod->insertProperty("pn_mag[c=3780]   (0x2209)", &pdtmb->pn_mag[dtmb_carrier_3780]);
	newmod->insertProperty("pn_mag[c=1]   (0x2209)", &pdtmb->pn_mag[dtmb_carrier_1]);
	newmod->insertProperty("ofdm_mag[c=3780][qpsk]   (0x220a) -> 0", &pdtmb->ofdm_mag[dtmb_carrier_3780][dtmb_qpsk]);
	newmod->insertProperty("ofdm_mag[c=3780][qam16]   (0x220a) -> 1", &pdtmb->ofdm_mag[dtmb_carrier_3780][dtmb_qam16]);
	newmod->insertProperty("ofdm_mag[c=3780][qam64]   (0x220a) -> 2", &pdtmb->ofdm_mag[dtmb_carrier_3780][dtmb_qam64]);
	newmod->insertProperty("ofdm_mag[c=3780][qam4-nr]   (0x220a) -> 3", &pdtmb->ofdm_mag[dtmb_carrier_3780][dtmb_qam4_nr]);
	newmod->insertProperty("ofdm_mag[c=3780][qam32]   (0x220a) -> 4", &pdtmb->ofdm_mag[dtmb_carrier_3780][dtmb_qam32]);

	newmod->insertProperty("ofdm_mag[c=1][qpsk]   (0x220a) -> 0", &pdtmb->ofdm_mag[dtmb_carrier_1][dtmb_qpsk]);
	newmod->insertProperty("ofdm_mag[c=1][qam16]   (0x220a) -> 1", &pdtmb->ofdm_mag[dtmb_carrier_1][dtmb_qam16]);
	newmod->insertProperty("ofdm_mag[c=1][qam64]   (0x220a) -> 2", &pdtmb->ofdm_mag[dtmb_carrier_1][dtmb_qam64]);
	newmod->insertProperty("ofdm_mag[c=1][qam4-nr]   (0x220a) -> 3", &pdtmb->ofdm_mag[dtmb_carrier_1][dtmb_qam4_nr]);
	newmod->insertProperty("ofdm_mag[c=1][qam32]   (0x220a) -> 4", &pdtmb->ofdm_mag[dtmb_carrier_1][dtmb_qam32]);

	newmod->insertProperty("tps_scale[qpsk]   (0x220b) -> 0", &pdtmb->tps_scale[dtmb_qpsk]);
	newmod->insertProperty("tps_scale[qam16]   (0x220b) -> 1", &pdtmb->tps_scale[dtmb_qam16]);
	newmod->insertProperty("tps_scale[qam64]   (0x220b) -> 2", &pdtmb->tps_scale[dtmb_qam64]);
	newmod->insertProperty("tps_scale[qam4-nr]   (0x220b) -> 3", &pdtmb->tps_scale[dtmb_qam4_nr]);
	newmod->insertProperty("tps_scale[qam32]   (0x220b) -> 4", &pdtmb->tps_scale[dtmb_qam32]);

	newmod->insertProperty("fft_scale[qpsk]   (0x2216)", &pdtmb->fft_scale[dtmb_qpsk]);
	newmod->insertProperty("fft_scale[qam16]   (0x2216)", &pdtmb->fft_scale[dtmb_qam16]);
	newmod->insertProperty("fft_scale[qam64]   (0x2216)", &pdtmb->fft_scale[dtmb_qam64]);
	newmod->insertProperty("fft_scale[qam4-nr]   (0x2216)", &pdtmb->fft_scale[dtmb_qam4_nr]);
	newmod->insertProperty("fft_scale[qam32]   (0x2216)", &pdtmb->fft_scale[dtmb_qam32]);

	newmod->insertProperty("gain[c=1][qpsk]   (0x2213) -> 0", &pdtmb->gain[dtmb_carrier_1][dtmb_qpsk]);
	newmod->insertProperty("gain[c=1][qam16]   (0x2213) -> 1", &pdtmb->gain[dtmb_carrier_1][dtmb_qam16]);
	newmod->insertProperty("gain[c=1][qam64]   (0x2213) -> 2", &pdtmb->gain[dtmb_carrier_1][dtmb_qam64]);
	newmod->insertProperty("gain[c=1][qam4-nr]   (0x2213) -> 3", &pdtmb->gain[dtmb_carrier_1][dtmb_qam4_nr]);
	newmod->insertProperty("gain[c=1][qam32]   (0x2213) -> 4", &pdtmb->gain[dtmb_carrier_1][dtmb_qam32]);
	newmod->insertProperty("gain[c=3780][PN945]   (0x2213)", &pdtmb->gain[dtmb_carrier_3780][dtmb_framesync_945]);
	newmod->insertProperty("gain[c=3780][PN420]   (0x2213)", &pdtmb->gain[dtmb_carrier_3780][dtmb_framesync_420]);

	return newmod;
}

qisprops_modconfig* qisprops_modconfig::createDTMBMain_B2(Pdtmb_config pdtmb)
{
	qisprops_modconfig* newmod = new qisprops_modconfig();
	newmod->insertProperty("pn_mag[c=3780]   (0x2209)", &pdtmb->pn_mag[dtmb_carrier_3780]);
	newmod->insertProperty("pn_mag[c=1]   (0x2209)", &pdtmb->pn_mag[dtmb_carrier_1]);
	newmod->insertProperty("ofdm_mag[c=3780][qpsk]   (0x220a) -> 0", &pdtmb->ofdm_mag[dtmb_carrier_3780][dtmb_qpsk]);
	newmod->insertProperty("ofdm_mag[c=3780][qam16]   (0x220a) -> 1", &pdtmb->ofdm_mag[dtmb_carrier_3780][dtmb_qam16]);
	newmod->insertProperty("ofdm_mag[c=3780][qam64]   (0x220a) -> 2", &pdtmb->ofdm_mag[dtmb_carrier_3780][dtmb_qam64]);
	newmod->insertProperty("ofdm_mag[c=3780][qam4-nr]   (0x220a) -> 3", &pdtmb->ofdm_mag[dtmb_carrier_3780][dtmb_qam4_nr]);
	newmod->insertProperty("ofdm_mag[c=3780][qam32]   (0x220a) -> 4", &pdtmb->ofdm_mag[dtmb_carrier_3780][dtmb_qam32]);

	newmod->insertProperty("ofdm_mag[c=1][qpsk]   (0x220a) -> 0", &pdtmb->ofdm_mag[dtmb_carrier_1][dtmb_qpsk]);
	newmod->insertProperty("ofdm_mag[c=1][qam16]   (0x220a) -> 1", &pdtmb->ofdm_mag[dtmb_carrier_1][dtmb_qam16]);
	newmod->insertProperty("ofdm_mag[c=1][qam64]   (0x220a) -> 2", &pdtmb->ofdm_mag[dtmb_carrier_1][dtmb_qam64]);
	newmod->insertProperty("ofdm_mag[c=1][qam4-nr]   (0x220a) -> 3", &pdtmb->ofdm_mag[dtmb_carrier_1][dtmb_qam4_nr]);
	newmod->insertProperty("ofdm_mag[c=1][qam32]   (0x220a) -> 4", &pdtmb->ofdm_mag[dtmb_carrier_1][dtmb_qam32]);

	newmod->insertProperty("tps_scale[qpsk]   (0x220b) -> 0", &pdtmb->tps_scale[dtmb_qpsk]);
	newmod->insertProperty("tps_scale[qam16]   (0x220b) -> 1", &pdtmb->tps_scale[dtmb_qam16]);
	newmod->insertProperty("tps_scale[qam64]   (0x220b) -> 2", &pdtmb->tps_scale[dtmb_qam64]);
	newmod->insertProperty("tps_scale[qam4-nr]   (0x220b) -> 3", &pdtmb->tps_scale[dtmb_qam4_nr]);
	newmod->insertProperty("tps_scale[qam32]   (0x220b) -> 4", &pdtmb->tps_scale[dtmb_qam32]);

	newmod->insertProperty("gain[c=1][qpsk]   (0x2213) -> 0", &pdtmb->gain[dtmb_carrier_1][dtmb_qpsk]);
	newmod->insertProperty("gain[c=1][qam16]   (0x2213) -> 1", &pdtmb->gain[dtmb_carrier_1][dtmb_qam16]);
	newmod->insertProperty("gain[c=1][qam64]   (0x2213) -> 2", &pdtmb->gain[dtmb_carrier_1][dtmb_qam64]);
	newmod->insertProperty("gain[c=1][qam4-nr]   (0x2213) -> 3", &pdtmb->gain[dtmb_carrier_1][dtmb_qam4_nr]);
	newmod->insertProperty("gain[c=1][qam32]   (0x2213) -> 4", &pdtmb->gain[dtmb_carrier_1][dtmb_qam32]);
	newmod->insertProperty("gain[c=3780][PN945]   (0x2213)", &pdtmb->gain[dtmb_carrier_3780][dtmb_framesync_945]);
	newmod->insertProperty("gain[c=3780][PN420]   (0x2213)", &pdtmb->gain[dtmb_carrier_3780][dtmb_framesync_420]);

	return newmod;
}

qisprops_modconfig* qisprops_modconfig::createISDBTMain(Pisdb_t_config pisdbt)
{
	qisprops_modconfig* newmod = new qisprops_modconfig();
	newmod->insertProperty("scl_0   (0x220c)", &pisdbt->scl_0);
	newmod->insertProperty("scl_1   (0x220d)", &pisdbt->scl_1);
	newmod->insertProperty("scl_2   (0x2210)", &pisdbt->scl_2);
	newmod->insertProperty("fft_scale[2k]   (0x2212) -> 0", &pisdbt->fft_scale[fft_2k]);
	newmod->insertProperty("fft_scale[8k]   (0x2212) -> 1", &pisdbt->fft_scale[fft_8k]);
	newmod->insertProperty("fft_scale[4k]   (0x2212) -> 2", &pisdbt->fft_scale[fft_4k]);
	newmod->insertProperty("gain[2k]   (0x2201)", &pisdbt->gain[fft_2k]);
	newmod->insertProperty("gain[8k]   (0x2201)", &pisdbt->gain[fft_8k]);
	newmod->insertProperty("gain[4k]   (0x2201)", &pisdbt->gain[fft_4k]);
	return newmod;
}

qisprops_modconfig* qisprops_modconfig::createISDBTMain_B2(Pisdb_t_config pisdbt)
{
	qisprops_modconfig* newmod = new qisprops_modconfig();
	newmod->insertProperty("scl_0   (0x220c)", &pisdbt->scl_0);
	newmod->insertProperty("scl_1   (0x220d)", &pisdbt->scl_1);
	newmod->insertProperty("scl_2   (0x2210)", &pisdbt->scl_2);
	newmod->insertProperty("gain[2k]   (0x2201)", &pisdbt->gain[fft_2k]);
	newmod->insertProperty("gain[8k]   (0x2201)", &pisdbt->gain[fft_8k]);
	newmod->insertProperty("gain[4k]   (0x2201)", &pisdbt->gain[fft_4k]);
	return newmod;
}

qisprops_modconfig* qisprops_modconfig::createDVBT2Main(Pdvb_t2_config pdvbt2)
{
	qisprops_modconfig* newmod = new qisprops_modconfig();
	newmod->insertProperty("fftscale[1k]   (0x2212)", &pdvbt2->fftscale[t2_fft_1k]);
	newmod->insertProperty("fftscale[2k]   (0x2212)", &pdvbt2->fftscale[t2_fft_2k]);
	newmod->insertProperty("fftscale[4k]   (0x2212)", &pdvbt2->fftscale[t2_fft_4k]);
	newmod->insertProperty("fftscale[8k]   (0x2212)", &pdvbt2->fftscale[t2_fft_8k]);
	newmod->insertProperty("fftscale[16k]   (0x2212)", &pdvbt2->fftscale[t2_fft_16k]);

	newmod->insertProperty("gain[1k]   (0x2201)", &pdvbt2->gain[t2_fft_1k]);
	newmod->insertProperty("gain[2k]   (0x2201)", &pdvbt2->gain[t2_fft_2k]);
	newmod->insertProperty("gain[4k]   (0x2201)", &pdvbt2->gain[t2_fft_4k]);
	newmod->insertProperty("gain[8k]   (0x2201)", &pdvbt2->gain[t2_fft_8k]);
	newmod->insertProperty("gain[16k]   (0x2201)", &pdvbt2->gain[t2_fft_16k]);
	return newmod;
}

/* configs */

const qisprop_entry mpeg2qmatrix_entries[] =
{
	{QSTR_QMATRIX_INTAR,qisprop_qmatrix,NULL,0,},
	{QSTR_QMATRIX_INTER,qisprop_qmatrix,NULL,1,},
	{NULL,qisprop_unknown,NULL,0},
};

const qisprop_entry h264qmatrix_entries[] =
{
	{QSTR_QMATRIX_INTAR_4_L,qisprop_qmatrix,NULL,0,},
	{QSTR_QMATRIX_INTAR_4_C,qisprop_qmatrix,NULL,1,},
	{QSTR_QMATRIX_INTER_4_L,qisprop_qmatrix,NULL,2,},
	{QSTR_QMATRIX_INTER_4_C,qisprop_qmatrix,NULL,3,},
	{QSTR_QMATRIX_INTAR_8,qisprop_qmatrix,NULL,4,},
	{QSTR_QMATRIX_INTER_8,qisprop_qmatrix,NULL,5,},
	{NULL,qisprop_unknown,NULL,0,},
};

const qisprop_entry mpeg2resolution_entries[] =
{
	{QSTR_RES_SD,qisprop_resolution,(const Pqisprop_entry)&mpeg2qmatrix_entries[0],MEDIA_IDX_VIDEO_SD,},
	{QSTR_RES_HD,qisprop_resolution,(const Pqisprop_entry)&mpeg2qmatrix_entries[0],MEDIA_IDX_VIDEO_HD,},
	{QSTR_RES_FULLHD,qisprop_resolution,(const Pqisprop_entry)&mpeg2qmatrix_entries[0],MEDIA_IDX_VIDEO_FULLHD,},
	{NULL,qisprop_unknown,NULL,0,},
};

const qisprop_entry h264resolution_entries[] =
{
	{QSTR_RES_SD,qisprop_resolution,(const Pqisprop_entry)&h264qmatrix_entries[0],MEDIA_IDX_VIDEO_SD,},
	{QSTR_RES_HD,qisprop_resolution,(const Pqisprop_entry)&h264qmatrix_entries[0],MEDIA_IDX_VIDEO_HD,},
	{QSTR_RES_FULLHD,qisprop_resolution,(const Pqisprop_entry)&h264qmatrix_entries[0],MEDIA_IDX_VIDEO_FULLHD,},
	{NULL,qisprop_unknown,NULL,0,},
};

const qisprop_entry broadcast_entries[] =
{
	{QSTR_ITEM_MOD,qisprop_modulation,NULL,0,},
	{QSTR_ITEM_AUDIO_CODEC,qisprop_audiocodec,NULL,0,},
	{QSTR_ITEM_MPEG2_CODEC,qisprop_mpeg2codec,(const Pqisprop_entry)&mpeg2resolution_entries[0],encvideo_mpeg2,},
	{QSTR_ITEM_H264_CODEC,qisprop_h264codec,(const Pqisprop_entry)&h264resolution_entries[0],encvideo_h264,},
	{NULL,qisprop_unknown,NULL,0,},
};

const qisprop_entry transform_entries[] =
{
	{QSTR_ITEM_MOD,qisprop_modulation,NULL,0,},
	{NULL,qisprop_unknown,NULL,0,},
};

const qisprop_entry fft_entries[] =
{
	{"2K",qisprop_ffttype,NULL,fft_2k,},
	{"8K",qisprop_ffttype,NULL,fft_8k,},
	{"4K",qisprop_ffttype,NULL,fft_4k,},
	{NULL,qisprop_unknown,NULL,0,},
};

const qisprop_entry fft_entries_t2[] =
{
	{"1K",qisprop_ffttype,NULL,t2_fft_1k,},
	{"2K",qisprop_ffttype,NULL,t2_fft_2k},
	{"4K",qisprop_ffttype,NULL,t2_fft_4k,},
	{"8K",qisprop_ffttype,NULL,t2_fft_8k,},
	{"16K",qisprop_ffttype,NULL,t2_fft_16k,},
	{NULL,qisprop_unknown,NULL,0,},
};

const qisprop_entry qam_entries_dtmb[] =
{
	{"qpsk",qisprop_qam,NULL,dtmb_qpsk,},
	{"qam16",qisprop_qam,NULL,dtmb_qam16,},
	{"qam64",qisprop_qam,NULL,dtmb_qam64,},
	{"qam32",qisprop_qam,NULL,dtmb_qam32,},
	{"qam4-nr",qisprop_qam,NULL,dtmb_qam4_nr,},
	{NULL,qisprop_unknown,NULL,0,},
};

const qisprop_entry carrier_entries[] =
{
	{"C=0",qisprop_carriermode,(const Pqisprop_entry)&qam_entries_dtmb[0],dtmb_carrier_1,},
	{"C=3780",qisprop_carriermode,(const Pqisprop_entry)&qam_entries_dtmb[0],dtmb_carrier_3780,},
	{NULL,qisprop_unknown,NULL,0,},
};

const qisprop_entry qam_entries_j83a[] =
{
	{"qam16",qisprop_qam,NULL,j83a_qam16,},
	{"qam32",qisprop_qam,NULL,j83a_qam32,},
	{"qam64",qisprop_qam,NULL,j83a_qam64,},
	{"qam128",qisprop_qam,NULL,j83a_qam128,},
	{"qam256",qisprop_qam,NULL,j83a_qam256,},
	{NULL,qisprop_unknown,NULL,0,},
};

const qisprop_entry srrctype_entries[] =
{
	{"dvb-t",qisprop_srrctype,NULL,CONFIG_MODSRRC_DVB_T,},
	{"j83a",qisprop_srrctype,NULL,CONFIG_MODSRRC_J83A,},
	{"atsc",qisprop_srrctype,NULL,CONFIG_MODSRRC_ATSC,},
	{"j83b-qam64",qisprop_srrctype,NULL,CONFIG_MODSRRC_J83BQ64,},
	{"j83b-qam256",qisprop_srrctype,NULL,CONFIG_MODSRRC_J83BQ256,},
	{"dtmb",qisprop_srrctype,NULL,CONFIG_MODSRRC_DTMB,},
	{"isdb-t",qisprop_srrctype,NULL,CONFIG_MODSRRC_ISDB_T,},
	{"j83c",qisprop_srrctype,NULL,CONFIG_MODSRRC_J83C,},
	{"dvb-t2",qisprop_srrctype,NULL,CONFIG_MODSRRC_DVB_T2,},
	{NULL,qisprop_unknown,NULL,},
};

const qisprop_entry modcfg_entries[] =
{
	{"atsc",qisprop_modtype,NULL,modulator_atsc,},
	{"dvb-t",qisprop_modtype,(const Pqisprop_entry)&fft_entries[0],modulator_dvb_t,},
	{"dvb-t2",qisprop_modtype,(const Pqisprop_entry)&fft_entries_t2[0],modulator_dvb_t2,},
	{"isdb-t",qisprop_modtype,(const Pqisprop_entry)&fft_entries[0],modulator_isdb_t,},
	{"dtmb",qisprop_modtype,(const Pqisprop_entry)&carrier_entries[0],modulator_dtmb,},
	{"j83a",qisprop_modtype,(const Pqisprop_entry)&qam_entries_j83a[0],modulator_j83a,},
	{"j83b",qisprop_modtype,NULL,modulator_j83b,},
	{NULL,qisprop_unknown,NULL,},
};

const qisprop_entry modcfg_entries_not2[] =
{
	{"atsc",qisprop_modtype,NULL,modulator_atsc,},
	{"dvb-t",qisprop_modtype,(const Pqisprop_entry)&fft_entries[0],modulator_dvb_t,},
	{"isdb-t",qisprop_modtype,(const Pqisprop_entry)&fft_entries[0],modulator_isdb_t,},
	{"dtmb",qisprop_modtype,(const Pqisprop_entry)&carrier_entries[0],modulator_dtmb,},
	{"j83a",qisprop_modtype,(const Pqisprop_entry)&qam_entries_j83a[0],modulator_j83a,},
	{"j83b",qisprop_modtype,NULL,modulator_j83b,},
	{NULL,qisprop_unknown,NULL,},
};

const qisprop_entry chiparch_entries[] =
{
	{"B2",qisprop_chiparch,(const Pqisprop_entry)&modcfg_entries_not2[0],CONFIG_CHIPIDX_B2,},
	{"B2+",qisprop_chiparch,(const Pqisprop_entry)&modcfg_entries_not2[0],CONFIG_CHIPIDX_B2_plus,},
	{"B3 and B3+",qisprop_chiparch,(const Pqisprop_entry)&modcfg_entries[0],CONFIG_CHIPIDX_B3,},
	{"A3",qisprop_chiparch,(const Pqisprop_entry)&modcfg_entries[0],CONFIG_CHIPIDX_A3,},
	{NULL,qisprop_unknown,NULL,},
};

const qisprop_entry config_entries[] =
{
	{"chip config",qisprop_chipconfig,NULL,0,},
	//{"srrc filter",qisprop_srrc,(const Pqisprop_entry)&srrctype_entries[0],0,},
	{"modulation",qisprop_modconfig,(const Pqisprop_entry)&chiparch_entries[0],0,},
	{NULL,qisprop_unknown,NULL,},
};
