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
#include <QStyledItemDelegate>
#include <QSpinBox>
#include <QComboBox>
#include <QStandardItemModel>
#include <QLineEdit>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>
#include "ui_qtvUIStorageR2Tune.h"
#include <core/storage/storage_section.h>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QDate>
#include <QStackedWidget>

#define R2TABLE_IDX_FREQ		(0)
#define R2TABLE_IDX_NORM_PA		(1)
#define R2TABLE_IDX_NORM_GPIO	(2)

#define R2_REGDEF_GAIN			0x3003

#define R2TABLE_IDX_I			(R2_RANGE_I + 1)
#define R2TABLE_IDX_Q			(R2_RANGE_Q + 1)
#define R2TABLE_IDX_IMG			(R2_RANGE_IMG + 1)
#define R2TABLE_IDX_PHASE		(R2_RANGE_PHASE + 1)
#define R2TABLE_IDX_PA			(R2_RANGE_PAGAIN + 1)
#define R2TABLE_IDX_ADV_GPIO	(R2_RANGE_PAGAIN + 2)

QString all_path = "";

/* qi_r2tune_editor */

class qi_r2tune_editor : public QSpinBox
{
public:
	qi_r2tune_editor(int32_t index, uint32_t val, QWidget* parent = nullptr) :
		QSpinBox(parent)
	{
		setRange(r2_value_ranges[index][R2_VALIDX_MIN], r2_value_ranges[index][R2_VALIDX_MAX]);
		setValue(val);
	}
protected:
	int valueFromText(const QString& text)const override
	{
		QByteArray ba = text.toLocal8Bit();
		const char* c_str2 = ba.data();
		int num = (int)strtol(c_str2, NULL, 16);       // number base 16
		return num;
	}

	QString textFromValue(int value)const override
	{
		return QString::asprintf("%04x", value);
	}
};

class qi_r2tune_pa_editor : public QLineEdit
{
public:
	qi_r2tune_pa_editor(int32_t index, uint32_t val, QWidget* parent = nullptr) :
		QLineEdit(parent)
	{
		setText(QString(val));
	}
protected:

};

class qi_r2gpio_editor : public QComboBox
{
public:
	qi_r2gpio_editor(uint32_t gpio,QWidget* parent = nullptr) :
		QComboBox(parent),
		m_model(new QStandardItemModel(this))
	{
		QStandardItem* newitem = NULL;
		int32_t i = 0;
		setLineEdit(new QLineEdit(parent));
		lineEdit()->setReadOnly(true);
		lineEdit()->installEventFilter(this);

		for (i = 0; i < 6; i++)
		{
			newitem = new QStandardItem(QString::asprintf("gpio_%d",i));
			newitem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			if(gpio & (1 << i))newitem->setData(Qt::Checked, Qt::CheckStateRole);
			else newitem->setData(Qt::Unchecked, Qt::CheckStateRole);
			m_model->setItem(i, 0, newitem);
		}
		setModel(m_model);
	}

	uint32_t getValue()
	{
		uint32_t m_value = 0;
		for (int32_t i = 0; i < m_model->rowCount(); i++)
		{
			if (m_model->item(i)->data(Qt::CheckStateRole) == Qt::Checked)
				m_value |= (1 << i);
		}
		return m_value;
	}

	~qi_r2gpio_editor()
	{
		delete m_model;
	}

protected:
	QStandardItemModel* m_model;

	bool eventFilter(QObject* _object, QEvent* _event)
	{
		lineEdit()->setText("");
		if (_object == lineEdit() && _event->type() == QEvent::Show)
		{
			showPopup();
			return true;
		}
		return false;
	}
};

/* qtvUITableDelegate */

class qi_r2tune_delegate : public QStyledItemDelegate
{
public:
	qi_r2tune_delegate(qtvUITableR2Tune* r2table) :m_r2table(r2table){}

	/* QStyledItemDelegate */
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override
	{
		Pr2_tune_rule prule = m_r2table->getRule(&index);
		if (m_r2table->_mode() == qr2tune_advance)
		{
			if (index.column() == R2TABLE_IDX_I)return new qi_r2tune_editor(R2_RANGE_I,prule->tune.ioffset, parent);
			else if (index.column() == R2TABLE_IDX_Q)return new qi_r2tune_editor(R2_RANGE_Q,prule->tune.qoffset, parent);
			else if (index.column() == R2TABLE_IDX_IMG)return new qi_r2tune_editor(R2_RANGE_IMG,prule->tune.imgoffset, parent);
			else if (index.column() == R2TABLE_IDX_PHASE)return new qi_r2tune_editor(R2_RANGE_PHASE,prule->tune.phaseoffset, parent);
			else if (index.column() == R2TABLE_IDX_ADV_GPIO)return new qi_r2gpio_editor(prule->gpiocntl, parent);
			else if (index.column() == R2TABLE_IDX_PA)return new qi_r2tune_pa_editor(R2_RANGE_POWER,prule->pagain,parent);
		}
		else
		{
			if (index.column() == R2TABLE_IDX_NORM_GPIO)return new qi_r2gpio_editor(prule->gpiocntl, parent);
			else if (index.column() == R2TABLE_IDX_NORM_PA)return new qi_r2tune_pa_editor(R2_RANGE_POWER, prule->pagain, parent);
		}
		return QStyledItemDelegate::createEditor(parent, option, index);
	}

	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override
	{
		Pr2_tune_rule prule = m_r2table->getRule(&index);
		if (m_r2table->_mode() == qr2tune_advance)
		{
			if (index.column() == R2TABLE_IDX_ADV_GPIO)
			{
				qi_r2gpio_editor* pgpio = static_cast<qi_r2gpio_editor*>(editor);
				prule->gpiocntl = (uint16_t)pgpio->getValue();
			}
			else
			{
				qi_r2tune_editor* pedit = static_cast<qi_r2tune_editor*>(editor);
				qi_r2tune_pa_editor* pedit_pa = static_cast<qi_r2tune_pa_editor*>(editor);

				if (index.column() == R2TABLE_IDX_I)prule->tune.ioffset = rfmixer_r2_int2tune(pedit->value(), R2_RANGE_I);
				else if (index.column() == R2TABLE_IDX_Q)prule->tune.qoffset = rfmixer_r2_int2tune(pedit->value(), R2_RANGE_Q);
				else if (index.column() == R2TABLE_IDX_IMG)prule->tune.imgoffset = rfmixer_r2_int2tune(pedit->value(), R2_RANGE_IMG);
				else if (index.column() == R2TABLE_IDX_PHASE)prule->tune.phaseoffset = rfmixer_r2_int2tune(pedit->value(), R2_RANGE_PHASE);
				else if (index.column() == R2TABLE_IDX_PA)
				{
					QByteArray qArr = pedit_pa->text().toLatin1();
					const char* cStr2 = qArr.data();
					int num = (int)strtol(cStr2, NULL, 16);        // String to hex
					prule->pagain = num;
				}
			}
		}
		else
		{
			if (index.column() == R2TABLE_IDX_NORM_GPIO)
			{
				qi_r2gpio_editor* pgpio = static_cast<qi_r2gpio_editor*>(editor);
				prule->gpiocntl = (uint16_t)pgpio->getValue();
			}
			else if (index.column() == R2TABLE_IDX_NORM_PA)
			{
				qi_r2tune_pa_editor* pedit_pa = static_cast<qi_r2tune_pa_editor*>(editor);
	
				QByteArray qArr = pedit_pa->text().toLatin1();
				const char* cStr2 = qArr.data();
				int num = (int)strtol(cStr2, NULL, 16);       // String to hex
				prule->pagain = num;
			}
		}
		m_r2table->updateRule(&index);
	}

private:
	qtvUITableR2Tune* m_r2table;
};


/* qtvUITableR2Tune */

qtvUITableR2Tune::qtvUITableR2Tune(QWidget* parent) :
	QTableWidget(parent),
	m_r2tune(NULL),
	m_mode(qr2tune_normal),
	m_r2menu(parent),
	m_menuadd("add",&m_r2menu),
	m_menudel("delete",&m_r2menu),
	m_menuidx(-1)
{
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::SingleSelection);
	verticalHeader()->setStretchLastSection(true);
	horizontalHeader()->setStretchLastSection(true);
	setContextMenuPolicy(Qt::CustomContextMenu);

	m_r2menu.addAction(&m_menuadd);
	m_r2menu.addAction(&m_menudel);
	connect(&m_menuadd, SIGNAL(triggered(bool)), this, SLOT(recvMenuAdded(bool)));
	connect(&m_menudel, SIGNAL(triggered(bool)), this, SLOT(recvMenuDelete(bool)));
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(recvContextMenuRequested(const QPoint&)));
}

qtvUITableR2Tune::~qtvUITableR2Tune()
{
	disconnect(&m_menuadd, SIGNAL(triggered(bool)), this, SLOT(recvMenuAdded(bool)));
	disconnect(&m_menudel, SIGNAL(triggered(bool)), this, SLOT(recvMenuDelete(bool)));
	disconnect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(recvContextMenuRequested(const QPoint&)));
}

void qtvUITableR2Tune::setR2Tune(qtv_r2tune_mode mode, int32_t ruleidx, Pr2_tune_handle pr2tune)
{
	int32_t i = 0;
	Pr2_tune_handle pr2_backup = pr2tune;
	m_mode = mode;
	clear();
	resizeTable();
	setRowCount(pr2tune->items_num + 1);
	m_ruleindex = ruleidx;
	m_r2tune = pr2tune;

	if (m_r2tune)
	{
		for (i = 0; i < pr2tune->items_num; i++)
		{
			Pr2_tune_item pr2;
			insertItem(i, &pr2tune->rules[i]);
		}
	}
}

void qtvUITableR2Tune::insertItem(int32_t row, Pr2_tune_item pr2)
{
	QTableWidgetItem* nitem = new QTableWidgetItem(QString::asprintf("%d", pr2->freq));
	int32_t pos = 0;
	nitem->setFlags(nitem->flags() & ~Qt::ItemIsEditable);
	setItem(row, R2TABLE_IDX_FREQ, nitem);
	if (m_mode == qr2tune_advance)
	{
		nitem = new QTableWidgetItem(QString::asprintf("%d", rfmixer_r2_tune2int(pr2->rule[m_ruleindex].tune.ioffset, R2_RANGE_I)));
		setItem(row, R2TABLE_IDX_I, nitem);
		nitem = new QTableWidgetItem(QString::asprintf("%d", rfmixer_r2_tune2int(pr2->rule[m_ruleindex].tune.qoffset, R2_RANGE_Q)));
		setItem(row, R2TABLE_IDX_Q, nitem);
		nitem = new QTableWidgetItem(QString::asprintf("%d", rfmixer_r2_tune2int(pr2->rule[m_ruleindex].tune.imgoffset, R2_RANGE_IMG)));
		setItem(row, R2TABLE_IDX_IMG, nitem);
		nitem = new QTableWidgetItem(QString::asprintf("%d", rfmixer_r2_tune2int(pr2->rule[m_ruleindex].tune.phaseoffset, R2_RANGE_PHASE)));
		setItem(row, R2TABLE_IDX_PHASE, nitem);
		nitem = new QTableWidgetItem(QString::asprintf("0x%04x", pr2->rule[m_ruleindex].pagain));
		setItem(row, R2TABLE_IDX_PA, nitem);
		nitem = new QTableWidgetItem(QString::asprintf("0x%02x", pr2->rule[m_ruleindex].gpiocntl));
		setItem(row, R2TABLE_IDX_ADV_GPIO, nitem);
	}
	else
	{
		nitem = new QTableWidgetItem(QString::asprintf("0x%04x", pr2->rule[m_ruleindex].pagain));
		setItem(row, R2TABLE_IDX_NORM_PA, nitem);
		nitem = new QTableWidgetItem(QString::asprintf("0x%02x", pr2->rule[m_ruleindex].gpiocntl));
		setItem(row, R2TABLE_IDX_NORM_GPIO, nitem);
	}
}

void qtvUITableR2Tune::resizeTable()
{
	int32_t nw = viewport()->width() / QTV_UI_TABLE_WIDTH_UNIT;
	QStringList sztitles;

	if (m_mode == qr2tune_normal)
	{
		setColumnCount(R2TUNE_FIELD_NORMAL_NUMS);
		for (int32_t i = 0; i < (int32_t)R2TUNE_FIELD_NORMAL_NUMS; i++)
		{
			setColumnWidth(i, nw * r2tune_fields_normal[i].size);
			sztitles.append(QString(r2tune_fields_normal[i].title));
		}
	}
	else
	{
		setColumnCount(R2TUNE_FIELD_ADVANCE_NUMS);
		for (int32_t i = 0; i < (int32_t)R2TUNE_FIELD_ADVANCE_NUMS; i++)
		{
			setColumnWidth(i, nw * r2tune_fields_advance[i].size);
			sztitles.append(QString(r2tune_fields_advance[i].title));
		}
	}
	setHorizontalHeaderLabels(sztitles);
	setItemDelegate(new qi_r2tune_delegate(this));
	verticalHeader()->hide();
}

void qtvUITableR2Tune::resizeEvent(QResizeEvent* event)
{
	if (m_size != size())
	{
		resizeTable();
		m_size = size();
	}
}

Pr2_tune_rule qtvUITableR2Tune::getRule(const QModelIndex* idx)
{
	return &m_r2tune->rules[idx->row()].rule[m_ruleindex];
}

void qtvUITableR2Tune::updateRule(const QModelIndex* idx)
{
	QTableWidgetItem* item = NULL;
	Pr2_tune_rule prule = getRule(idx);
	if (m_mode == qr2tune_advance)
	{
		if (idx->column() == R2TABLE_IDX_ADV_GPIO)
			item = new QTableWidgetItem(QString::asprintf("0x%02x", prule->gpiocntl));
		else if (idx->column() == R2TABLE_IDX_PA)
			item = new QTableWidgetItem(QString::asprintf("0x%04x", prule->pagain));
		else if (idx->column() == R2TABLE_IDX_I)
			item = new QTableWidgetItem(QString::asprintf("%d", rfmixer_r2_tune2int(prule->tune.ioffset,R2_RANGE_I)));
		else if (idx->column() == R2TABLE_IDX_Q)
			item = new QTableWidgetItem(QString::asprintf("%d", rfmixer_r2_tune2int(prule->tune.qoffset, R2_RANGE_Q)));
		else if(idx->column() == R2TABLE_IDX_IMG)
			item = new QTableWidgetItem(QString::asprintf("%d", rfmixer_r2_tune2int(prule->tune.imgoffset, R2_RANGE_IMG)));
		else if (idx->column() == R2TABLE_IDX_PHASE)
			item = new QTableWidgetItem(QString::asprintf("%d", rfmixer_r2_tune2int(prule->tune.phaseoffset, R2_RANGE_PHASE)));
	}
	else
	{
		if(idx->column() == R2TABLE_IDX_NORM_GPIO)
			item = new QTableWidgetItem(QString::asprintf("0x%02x", prule->gpiocntl));
		else if(idx->column() == R2TABLE_IDX_NORM_PA)
			item = new QTableWidgetItem(QString::asprintf("0x%04x", prule->pagain));
	}

	setItem(idx->row(), idx->column(), item);
}

void qtvUITableR2Tune::recvContextMenuRequested(const QPoint& pos)
{
	m_menuidx = -1;
	if (m_r2tune)
	{
		QModelIndex idx = indexAt(pos);
		m_menuidx = idx.row();
		if (m_menuidx >= m_r2tune->items_num)m_menuidx = - 1;
		m_menudel.setEnabled(m_menuidx != -1);
		m_menuadd.setEnabled(m_r2tune->items_num < R2_TUNE_NUMS);
		m_r2menu.exec(mapToGlobal(pos));
	}
}

void qtvUITableR2Tune::recvMenuAdded(bool checked)
{
	bool bok = false;
	Qt::WindowFlags flags = Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint;
#ifdef _MSC_VER
	flags |= Qt::MSWindowsFixedSizeDialogHint;
#endif
	int32_t nfreq = QInputDialog::getInt(this, "R2 Tune Rule","Frequency(KHz)",5000,50000,1000000,1, &bok, flags);
	if (bok)
	{
		Pr2_tune_item pr2 = NULL;
		vatek_result nres = rfmixer_r2_add_tune_table(m_r2tune, (uint32_t)nfreq, &pr2);
		if (is_vatek_success(nres))
			setR2Tune(m_mode, m_ruleindex, m_r2tune);
		else QMessageBox::warning(this, "vatek_toolkit", QString::asprintf("add r2 rule fail : %d", nres));
	}
}

void qtvUITableR2Tune::recvMenuDelete(bool checked)
{
	if (m_menuidx != -1)
	{
		Pr2_tune_item pr2 = &m_r2tune->rules[m_menuidx];
		if (QMessageBox::question(this, "R2 Tune Rule", "Are you sure to delete?") == QMessageBox::Yes)
		{
			vatek_result nres = rfmixer_r2_del_tune_table(m_r2tune, pr2->freq);
			if (!is_vatek_success(nres))
				QMessageBox::warning(this,"R2 Tune Rule", QString::asprintf("Delete R2 rule fail : %d", nres));
			else setR2Tune(m_mode, m_ruleindex, m_r2tune);
		}
	}
}

/* qtvUIStorageR2Tune */

qtvUIStorageR2Tune::qtvUIStorageR2Tune(qtvUIMain* main, QWidget* parent) :
	qtvUITabBase(main, "R2 Tune", "R2 Tune", parent),
	ui(new Ui::qtvUIStorageR2Tune),
	m_r2tune(NULL),
	m_mode(qr2tune_normal)
{
	ui->setupUi(this);
	//ui->cbpath->setEnabled(false);
	ui->cbpath->addItem("path_0");
	ui->cbpath->addItem("path_1");
	ui->cbpath->setCurrentIndex(0);
	ui->cbentune->setVisible(false);
	ui->btnreset->setVisible(false);
	connect(ui->cbentune, SIGNAL(stateChanged(int)), this, SLOT(recvEnCalibrationStateChanged(int)));
	connect(ui->cbpath, SIGNAL(currentIndexChanged(int)), this, SLOT(recvPathSelectedChanged(int)));
	connect(ui->btnreset, SIGNAL(clicked(bool)), this, SLOT(recvBtnResetClick(bool)));
	connect(ui->btnimport, SIGNAL(clicked(bool)), this, SLOT(recvBtnImportClick(bool)));
	connect(ui->btnexport, SIGNAL(clicked(bool)), this, SLOT(recvBtnExportClick(bool)));
}

qtvUIStorageR2Tune::~qtvUIStorageR2Tune()
{
	disconnect(ui->cbentune, SIGNAL(stateChanged(int)), this, SLOT(recvEnCalibrationStateChanged(int)));
	disconnect(ui->cbpath, SIGNAL(currentIndexChanged(int)), this, SLOT(recvPathSelectedChanged(int)));
	disconnect(ui->btnreset, SIGNAL(clicked(bool)), this, SLOT(recvBtnResetClick(bool)));
	disconnect(ui->btnimport, SIGNAL(clicked(bool)), this, SLOT(recvBtnImportClick(bool)));
	disconnect(ui->btnexport, SIGNAL(clicked(bool)), this, SLOT(recvBtnExportClick(bool)));
	delete ui;
}

void qtvUIStorageR2Tune::tabEnter()
{
	if (qtvUIMain::is_uimain_storage(m_main->_mode()))
	{
		qtvUIMainStorage* smain = qobject_cast<qtvUIMainStorage*>(m_main);
		Pr2_tune_handle pr2 = vatek_storage_get_r2tune(smain->_storage_handle());
		m_r2tune = pr2;
		m_mode = qr2tune_normal;
		if (ui->cbentune->checkState() == Qt::Checked)
			m_mode = qr2tune_advance;
		ui->tabler2tune->setR2Tune(m_mode, ui->cbpath->currentIndex(), pr2);
	}
}

void qtvUIStorageR2Tune::tabFinish()
{

}

void qtvUIStorageR2Tune::tabLeave()
{

}


void qtvUIStorageR2Tune::recvEnCalibrationStateChanged(int state)
{
	if (m_r2tune)
	{
		m_r2tune->r2_flags_0 &= ~R2FLAG_EN_CALIBRATION;
		m_r2tune->r2_flags_1 &= ~R2FLAG_EN_CALIBRATION;
		m_mode = qr2tune_normal;
		
		if (ui->cbentune->checkState() == Qt::Checked)
		{
			m_r2tune->r2_flags_0 |= R2FLAG_EN_CALIBRATION;
			m_r2tune->r2_flags_1 |= R2FLAG_EN_CALIBRATION;
			m_mode = qr2tune_advance;
		}
	}
	ui->cbpath->setEnabled((ui->cbentune->checkState() == Qt::Checked));
	ui->cbpath->setCurrentIndex(0);
	recvPathSelectedChanged(0);
}

void qtvUIStorageR2Tune::recvBtnResetClick(bool bclick)
{
	if (!all_path.isEmpty())
	{
		QFile frule(all_path);

		if (frule.open(QIODevice::ReadOnly))
		{
			if (frule.read((char*)m_r2tune, sizeof(r2_tune_handle)) <= sizeof(r2_tune_handle))
			{
				if (m_r2tune->tag != R2_TUNE_HDR_TAG)
				{
					QMessageBox::warning(this, "vatek_factory", QString::asprintf("Bad R2 Rules file format : [%08x]", m_r2tune->tag));
					recvBtnResetClick(false);
				}
				ui->tabler2tune->setR2Tune(m_mode, ui->cbpath->currentIndex(), m_r2tune);
				return;
			}
		}
	}
}

void qtvUIStorageR2Tune::recvPathSelectedChanged(int index)
{
	if(m_r2tune)
		ui->tabler2tune->setR2Tune(m_mode, ui->cbpath->currentIndex(), m_r2tune);
}

void qtvUIStorageR2Tune::recvBtnExportClick(bool bclick)
{
	QString path = QFileDialog::getSaveFileName(this, "Save File", ".", "R2 Rules (*.r2tune);");
	if (!path.isEmpty())
	{
		QFile frule(path);
		if (frule.open(QIODevice::ReadWrite))
		{
			if (frule.write((char*)m_r2tune, sizeof(r2_tune_handle)) == sizeof(r2_tune_handle))
				return;
		}
		QMessageBox::warning(this, "vatek_factory", QString::asprintf("Save R2 Rules file fail"));
	}
}

void qtvUIStorageR2Tune::recvBtnImportClick(bool bclick)
{
	QMessageBox msgbox;
	QString path = QFileDialog::getOpenFileName(this, "Open File", ".", "R2 Rules (*.r2tune);");

	if (!path.isEmpty())
	{
		QFile frule(path);

		if (frule.open(QIODevice::ReadOnly))
		{
			if (frule.read((char*)m_r2tune, sizeof(r2_tune_handle)) <= sizeof(r2_tune_handle))
			{
				if (m_r2tune->tag != R2_TUNE_HDR_TAG)
				{
					QMessageBox::warning(this, "vatek_factory", QString::asprintf("Bad R2 Rules file format : [%08x]", m_r2tune->tag));
					recvBtnResetClick(false);
				}
				all_path = path;
				ui->tabler2tune->setR2Tune(m_mode, ui->cbpath->currentIndex(), m_r2tune);
				return;
			}
		}

		QMessageBox::warning(this, "vatek_factory", QString::asprintf("Open R2 Rules file fail"));
	}

}

void qtvUIStorageR2Tune::setpathInit(){
	all_path = "";
}
