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

#ifndef VATEK_ROMTOOL_H
#define VATEK_ROMTOOL_H

#include <QMainWindow>
#include <vatek_sdk_storage.h>
#include <vatek_sdk_device.h>
#include <storage_device.h>

namespace Ui {
class vatek_romtool;
}

typedef enum _romtool_status
{
    romtool_idle,
    romtool_device,
    romtool_image,
}romtool_status;

class vatek_romtool : public QMainWindow
{
    Q_OBJECT

public:
    explicit vatek_romtool(QWidget *parent = nullptr);
    ~vatek_romtool();

    void setStatus(romtool_status status);
    void closeRom();

protected:
    romtool_status m_rstatus;
    storageDevice* m_device;

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

private:
    Ui::vatek_romtool *ui;
    hvatek_storage m_hstorage;
    vatek_result openRomImage(QString& fimg);

    static void storage_handler(rom_progress_msg msg, void* progressval, void* param);
    static const char* imageTempFile;

private Q_SLOTS:
    void recvOpenDevice();
    void recvOpenImage();
    void recvWriteImage();
    void recvUpdateResource();
    void recvExport();
    void recvCloseHandle();
    void recvservice(QString& fimg);
};

#endif // VATEK_ROMTOOL_H
