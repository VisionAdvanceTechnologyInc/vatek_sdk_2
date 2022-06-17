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

#ifndef VATEK_FACTORY_H
#define VATEK_FACTORY_H

#include <QMainWindow>
#include <vatek_sdk_storage.h>
#include "./ui_vatek_factory.h"
#include <../../../api/qt/inc/ui/base/qtv_ui_bootlogo.h>

#include <ui/base/qtv_ui_storage.h>

QT_BEGIN_NAMESPACE
namespace Ui { class vatek_factory; }
QT_END_NAMESPACE

class vatek_factory : public QMainWindow
{
    Q_OBJECT

public:
    vatek_factory(QWidget *parent = nullptr);
    ~vatek_factory();
    void closeRom();
    void refreshUIStatus();
    bool isValid();
    static void getimg(QString img);
    

private:
    hvatek_storage m_hstorage;
    bool m_istempimage;

    vatek_result openRom(QString& floader,QString& fapp, QString& fr2_tune);
    vatek_result openRomImage(QString& fimg);

    static void storage_handler(rom_progress_msg msg, void* progressval, void* param);

    static const char* imageTempFile;

protected Q_SLOTS:
    void recvOpenFile();
    void recvOpenImage();
    void recvCloseFile();
    void recvBuildImage();

private:
    Ui::vatek_factory *ui;
};


extern QString image_path;

#endif // VATEK_FACTORY_H
