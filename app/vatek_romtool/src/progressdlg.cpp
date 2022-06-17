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

#include "progressdlg.h"
#include "ui_progressdlg.h"

progressDlg::progressDlg(storageDevice* pdevice,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::progressDlg),
    m_action(saction_idle),
    m_device(pdevice),
    m_romfile(NULL),
    m_result(vatek_success)
{
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    ui->setupUi(this);
    if(m_device)
        connect(m_device, SIGNAL(notifyStatusChanged()), this, SLOT(recvDeviceStatus()));
}

progressDlg::~progressDlg()
{
    if (m_device)
        disconnect(m_device, SIGNAL(notifyStatusChanged()), this, SLOT(recvDeviceStatus()));
    delete ui;
}

vatek_result progressDlg::runAction(storage_action action, storageDevice* device)
{
    vatek_result nres = vatek_badstatus;
    if (m_action == saction_idle)
    {
		m_action = action;
		m_device = device;
        QTimer::singleShot(0, this, &progressDlg::recvStartAction);
        exec();
        nres = m_result;
    }
    return nres;
}
vatek_result progressDlg::runAction(storage_action action, Promfile_handle prom, storageDevice* device)
{
	vatek_result nres = vatek_badstatus;
	if (m_action == saction_idle)
	{
        nres = vatek_badparam;
        if (action == saction_writeimage)
        {
			m_action = action;
            m_romfile = prom;
			m_device = device;
			QTimer::singleShot(0, this, &progressDlg::recvStartAction);
			exec();
			nres = m_result;
        }
	}
	return nres;
}


void progressDlg::recvStartAction()
{
    vatek_result nres = vatek_badparam;
    if (m_action == saction_open)
    {
        ui->txt_info->setText("open storage device...");
        ui->prog_pos->setValue(0);
        m_result = m_device->openStorage();
    }
    else if (m_action == saction_writeimage)
    {
		ui->txt_info->setText("write image ...");
		ui->prog_pos->setValue(0);
        m_result = m_device->writeImage(m_romfile);
    }

    QThread::msleep(100);
    close();
}

void progressDlg::recvTimeout()
{
    ui->txt_info->setText(m_device->_information());
    ui->prog_pos->setValue(m_device->_position());
    QApplication::processEvents();
}

void progressDlg::recvDeviceStatus()
{
    QTimer::singleShot(0, this, &progressDlg::recvTimeout);
}