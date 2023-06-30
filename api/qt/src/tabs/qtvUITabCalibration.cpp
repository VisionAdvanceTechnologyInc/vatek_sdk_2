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
#include <QMessageBox>

#include "ui_qtvUITabCalibration.h"

#define UISUB_TAB_CALIBRATION_NAME		"Calibration"
#define UISUB_TAB_CALIBRATION_DESC		"calibration modulation and output performance"

qtvUITabCalibration::qtvUITabCalibration(qtvUIMain* main, QWidget* parent) :
    qtvUITabBase(main, UISUB_TAB_CALIBRATION_NAME, UISUB_TAB_CALIBRATION_DESC, parent),
    ui(new Ui::qtvUITabCalibration),
    m_handle(NULL)
{
    int32_t i = 0;
    ui->setupUi(this);

    m_splist.append(ui->sb_clock);
    m_splist.append(ui->sb_ioffect_0);
    m_splist.append(ui->sb_qoffect_0);
    m_splist.append(ui->sb_igain_0);
    m_splist.append(ui->sb_qgain_0);
    m_splist.append(ui->sb_ioffect_1);
    m_splist.append(ui->sb_qoffect_1);
    m_splist.append(ui->sb_image_1);
    m_splist.append(ui->sb_phase_1);
    m_splist.append(ui->sb_dac);
    m_splist.append(ui->sb_r2);
    ui->sb_dac->setRange(-60 , 60);
    ui->sb_r2->setRange(0, 15);

    for (i = 0; i < m_splist.size(); i++)
        connect(m_splist[i], SIGNAL(valueChanged(int)), this, SLOT(recvSpinValueChanged(int)));
    connect(&m_timerupdate, SIGNAL(timeout()), this, SLOT(recvUpdateTimeout()));
    connect(ui->btn_save, SIGNAL(clicked(bool)), this, SLOT(recvSaveClick(bool)));

    memset(&m_calibration, 0, sizeof(calibration_param));
	if (m_main && m_main->_service())
		m_handle = m_main->_service()->_sevice_handle();

    if (m_handle)
    {
        if (m_handle->calibratiobCheck() != 0)
        {
            resetCalibration();
            m_timerupdate.stop();
        }
        else enableCalibration(false);
    }
    else enableCalibration(false);
}

qtvUITabCalibration::~qtvUITabCalibration()
{
    int32_t i = 0;
	for (i = 0; i < m_splist.size(); i++)
		disconnect(m_splist[i], SIGNAL(valueChanged(int)), this, SLOT(recvSpinValueChanged(int)));
    disconnect(&m_timerupdate, SIGNAL(timeout()), this, SLOT(recvUpdateTimeout()));
    disconnect(ui->btn_save, SIGNAL(clicked(bool)), this, SLOT(recvSaveClick(bool)));

    m_splist.clear();
    delete ui;
}

void qtvUITabCalibration::tabEnter()
{

}

void qtvUITabCalibration::tabFinish()
{

}

void qtvUITabCalibration::tabLeave()
{

}

void qtvUITabCalibration::resetCalibration()
{
	vatek_result nres = m_handle->calibrationGet(&m_calibration);
	if (!is_vatek_success(nres))memset(&m_calibration, 0, sizeof(calibration_param));


	ui->sb_clock->setValue(m_calibration.clock);
	ui->sb_ioffect_0->setValue(m_calibration.dac.ioffect);
	ui->sb_qoffect_0->setValue(m_calibration.dac.qoffect);
	ui->sb_igain_0->setValue(m_calibration.dac.igain);
	ui->sb_qgain_0->setValue(m_calibration.dac.qgain);
	ui->sb_ioffect_1->setValue(rfmixer_r2_tune2int(m_calibration.r2.ioffset, R2_RANGE_I));
	ui->sb_qoffect_1->setValue(rfmixer_r2_tune2int(m_calibration.r2.qoffset, R2_RANGE_Q));
	ui->sb_image_1->setValue(rfmixer_r2_tune2int(m_calibration.r2.imgoffset, R2_RANGE_IMG));
	ui->sb_phase_1->setValue(rfmixer_r2_tune2int(m_calibration.r2.phaseoffset, R2_RANGE_PHASE));
    ui->sb_r2->setValue(m_calibration.r2_power);

    if (!is_vatek_success(nres))QMessageBox::warning(this, "calibration", "get calibration param fail");
}

void qtvUITabCalibration::applyCalibration()
{
    vatek_result nres = vatek_unsupport;
    enableCalibration(false);

    m_calibration.clock = ui->sb_clock->value();
    m_calibration.dac.ioffect = ui->sb_ioffect_0->value();
    m_calibration.dac.qoffect = ui->sb_qoffect_0->value();
    m_calibration.dac.igain = ui->sb_igain_0->value();
    m_calibration.dac.qgain = ui->sb_qgain_0->value();
    m_calibration.r2.ioffset = rfmixer_r2_int2tune(ui->sb_ioffect_1->value(), R2_RANGE_I);
    m_calibration.r2.qoffset = rfmixer_r2_int2tune(ui->sb_qoffect_1->value(), R2_RANGE_Q);
    m_calibration.r2.imgoffset = rfmixer_r2_int2tune(ui->sb_image_1->value(), R2_RANGE_IMG);
    m_calibration.r2.phaseoffset = rfmixer_r2_int2tune(ui->sb_phase_1->value(), R2_RANGE_PHASE);
    m_calibration.r2_power = ui->sb_r2->value();
    m_calibration.dac_power = ui->sb_dac->value();
    nres = m_handle->calibrationSet(&m_calibration);
    if(!is_vatek_success(nres))
        QMessageBox::warning(this, "calibration", "apply calibration param fail");
    enableCalibration(true);
}

void qtvUITabCalibration::enableCalibration(bool isen)
{
    ui->sb_clock->setEnabled(isen);
	ui->sb_ioffect_0->setEnabled(isen);
	ui->sb_qoffect_0->setEnabled(isen);
	ui->sb_igain_0->setEnabled(isen);
	ui->sb_qgain_0->setEnabled(isen);
	ui->sb_ioffect_1->setEnabled(isen);
	ui->sb_qoffect_1->setEnabled(isen);
	ui->sb_image_1->setEnabled(isen);
	ui->sb_phase_1->setEnabled(isen);
}

void qtvUITabCalibration::recvSpinValueChanged(int i)
{
    if (!m_timerupdate.isActive())
        m_timerupdate.start(1000);
}

void qtvUITabCalibration::recvUpdateTimeout()
{
    applyCalibration();
    m_timerupdate.stop();
}

void qtvUITabCalibration::recvSaveClick(bool bclick)
{
    vatek_result nres = vatek_success;
    enableCalibration(false);
    ui->btn_save->setEnabled(false);

    nres = m_handle->calibrationSave();
    if(!is_vatek_success(nres))
        QMessageBox::warning(this, "calibration", "save calibration param fail");

    ui->btn_save->setEnabled(true);
    enableCalibration(true);
}