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

#ifndef PROGRESSDLG_H
#define PROGRESSDLG_H

#include <QDialog>
#include <QTimer>
#include <storage_device.h>

namespace Ui {
class progressDlg;
}

typedef enum _storage_action
{
	saction_idle,
	saction_open,
	saction_update,
	saction_writeimage,
}storage_action;

class progressDlg : public QDialog
{
    Q_OBJECT

public:
    explicit progressDlg(storageDevice* pdevice,QWidget *parent = nullptr);
    ~progressDlg();

    vatek_result runAction(storage_action action, storageDevice* device);
    vatek_result runAction(storage_action action, Promfile_handle prom, storageDevice* device);


protected:
    storage_action m_action;
    storageDevice* m_device;
    vatek_result m_result;
    Promfile_handle m_romfile;

protected Q_SLOTS:
    void recvTimeout();
    void recvStartAction();
    void recvDeviceStatus();

private:
    Ui::progressDlg *ui;
};

#endif // PROGRESSDLG_H
