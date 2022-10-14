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

#include <vatek_romtool.h>

#include <ui/base/qtv_ui_storage.h>
#include "ui_vatek_romtool.h"
#include "..\..\..\api\qt\inc\ui\base\qtv_ui_storage.h"

#include <QTextCodec>
#include <QMessageBox>
#include <QFileDialog>
#include "progressdlg.h"
#include <QScreen>
#include <qDebug>
#include <windows.h>
#include <conio.h>
const char* vatek_romtool::imageTempFile = "~tmpimage.vimage";
vatek_romtool::vatek_romtool(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::vatek_romtool),
    m_rstatus(romtool_idle),
    m_device(NULL)
{
    ui->setupUi(this);
    ui->listorage->attachStacked(ui->stcontext);

    QScreen* screen = QGuiApplication::primaryScreen();     //螢幕大小
    QRect mm = screen->availableGeometry();
    int screen_width = mm.width();
    int screen_height = mm.height();
    qDebug() << screen_width << screen_height;

    HDC hd = GetDC(NULL);                                           // 獲取DPI
    int horDPI = GetDeviceCaps(hd, LOGPIXELSX);
    int verticalDPI = GetDeviceCaps(hd, LOGPIXELSY);
    double objectRate = horDPI / 96.0;

    setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);

    if (screen_width >= 1920) {
        if (objectRate >= 1.5) {
            setMinimumSize(screen_width, screen_height - 40);
        }
        else {
            setMinimumSize(screen_width / 1.7, screen_height / 1.5);
        }
    }
    else {
        setMinimumSize(screen_width / 1.5, screen_height / 1.3);
    }

	statusBar()->setSizeGripEnabled(false);
    connect(ui->actionDevice, SIGNAL(triggered()), this, SLOT(recvOpenDevice()));
    connect(ui->actionImage, SIGNAL(triggered()), this, SLOT(recvOpenImage()));
    connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(recvCloseHandle()));
    connect(ui->actionUpdate, SIGNAL(triggered()), this, SLOT(recvUpdateResource()));
    connect(ui->actionExport, SIGNAL(triggered()), this, SLOT(recvExport()));
    connect(ui->actionUpdateDevice, SIGNAL(triggered()), this, SLOT(recvWriteImage()));
    setStatus(m_rstatus);
}

vatek_romtool::~vatek_romtool()
{
    recvCloseHandle();
    disconnect(ui->actionDevice, SIGNAL(triggered()), this, SLOT(recvOpenDevice()));
    disconnect(ui->actionImage, SIGNAL(triggered()), this, SLOT(recvOpenImage()));
    disconnect(ui->actionClose, SIGNAL(triggered()), this, SLOT(recvCloseHandle()));
    disconnect(ui->actionUpdate, SIGNAL(triggered()), this, SLOT(recvUpdateResource()));
    disconnect(ui->actionExport, SIGNAL(triggered()), this, SLOT(recvExport()));
    disconnect(ui->actionUpdateDevice, SIGNAL(triggered()), this, SLOT(recvWriteImage()));
    delete ui;
}

void vatek_romtool::recvOpenDevice()
{
    if (!m_device)
    {
        m_device = storageDevice::openStorageDevice(1);
        if (!m_device)QMessageBox::warning(this, "vatek_romtool", "can not found valid device.");
        else
        {
            progressDlg newprog(m_device, this);
            vatek_result nres = newprog.runAction(saction_open, m_device);
            if (is_vatek_success(nres))
            {
                ui->listorage->refreshStorage(m_device->_hstorage());
                setStatus(romtool_device);
            }
            else
            {
                QMessageBox::warning(this, "vatek_romtool", QString::asprintf("open device fail : %d", nres));
                storageDevice::closeStorageDevice(m_device);
                m_device = NULL;
            }
        }
    }
}



void vatek_romtool::recvOpenImage()
{

}

void vatek_romtool::recvUpdateResource()
{

}

void vatek_romtool::recvCloseHandle()
{
    if (m_device)
    {
        ui->listorage->refreshStorage(NULL);
        storageDevice::closeStorageDevice(m_device);
        m_device = NULL;
        setStatus(romtool_idle);
    }
}

void vatek_romtool::recvExport()
{

}

void vatek_romtool::recvservice(QString& fimg) {
    hvatek_devices hdevlist = NULL;
    hvatek_chip hchip = NULL;
    vatek_result nres = vatek_success;
    QMessageBox msg(this);

    if (m_hstorage)
    {
        QString szfilename;
        Papp_header papp = NULL;
        vatek_result nres = vatek_storage_get_app(m_hstorage, &papp);
        if (is_vatek_success(nres))
        {
            const char* szapp = "app2_broadcast";
            if (papp->service_mode == service_transform) {
                szapp = "app2_transform";
                if (nres == vatek_success)
                    nres = vatek_device_list_enum(DEVICE_BUS_ALL, service_transform, &hdevlist);
                //if (nres == vatek_success)
                //    nres = vatek_device_list_enum(DEVICE_BUS_USB, service_transform, &hdevlist);
                if (hdevlist) {
                    if (is_vatek_success(nres))
                    {
                        nres = vatek_device_open(hdevlist, 0, &hchip);
                        Pchip_info pinfo = vatek_device_get_info(hchip);

                        if (pinfo->hal_service == papp->service_mode) {
                            if (!fimg.isEmpty())
                            {
                                QByteArray ba = fimg.toLocal8Bit();
                                const char* c_str2 = ba.data();
                                Promfile_handle prom = NULL;
                                vatek_result nres = vatek_storage_romfile_create(ba.data(), &prom);
                                //2.設置消息對話框屬性
                                msg.setWindowTitle("Information");
                                msg.setText("Sure write image file to device?");
                                msg.setIcon(QMessageBox::Information);
                                msg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

                                switch (msg.exec()) {
                                case (QMessageBox::Ok):
                                {
                                    if (is_vatek_success(nres))
                                    {
                                        progressDlg newprog(m_device, this);
                                        nres = newprog.runAction(saction_writeimage, prom, m_device);
                                        vatek_storage_romfile_free(prom);
                                    }
                                    if (!is_vatek_success(nres))
                                        QMessageBox::warning(this, "vatek_tomtool", QString::asprintf("write image fail : %d", nres));
                                    else {
                                        if (hdevlist)vatek_device_list_free(hdevlist);
                                        recvCloseHandle();
                                    }
                                }

                                case (QMessageBox::Cancel):
                                {
                                    break;
                                }
                                }
                            }
                        }
                    }
                }
                else {
                    QMessageBox::warning(this, "Romtool", QString::asprintf("Error file format"));
                }
            }
            else {

                //if (nres == vatek_success)
                //    nres = vatek_device_list_enum(DEVICE_BUS_BRIDGE, service_broadcast, &hdevlist);
                if (nres == vatek_success)
                    nres = vatek_device_list_enum(DEVICE_BUS_ALL, service_broadcast, &hdevlist);
                if (hdevlist) {
                    if (is_vatek_success(nres))
                    {
                        nres = vatek_device_open(hdevlist, 0, &hchip);
                        Pchip_info pinfo = vatek_device_get_info(hchip);                     
                        if (pinfo->hal_service == papp->service_mode) {
                            if (!fimg.isEmpty())
                            {
                                QByteArray ba = fimg.toLocal8Bit();
                                const char* c_str2 = ba.data();
                                Promfile_handle prom = NULL;
                                vatek_result nres = vatek_storage_romfile_create(ba.data(), &prom);
                                //2.設置消息對話框屬性
                                msg.setWindowTitle("Information");
                                msg.setText("Sure write image file to device?");
                                msg.setIcon(QMessageBox::Information);
                                msg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

                                switch (msg.exec()) {
                                case (QMessageBox::Ok):
                                {
                                    if (is_vatek_success(nres))
                                    {
                                        progressDlg newprog(m_device, this);
                                        nres = newprog.runAction(saction_writeimage, prom, m_device);
                                        vatek_storage_romfile_free(prom);
                                    }
                                    if (!is_vatek_success(nres))
                                        QMessageBox::warning(this, "vatek_tomtool", QString::asprintf("write image fail : %d", nres));
                                    else {
                                        if (hdevlist)vatek_device_list_free(hdevlist);
                                        recvCloseHandle();
                                    }
                                }

                                case (QMessageBox::Cancel):
                                {
                                    break;
                                }
                                }
                            }
                        }
                    }
                }
                else {
                    QMessageBox::warning(this, "Romtool", QString::asprintf("Error file format"));
                }
            }
        }
        else QMessageBox::warning(this, windowTitle(), "can not found current application and loader");
    }
}

void vatek_romtool::recvWriteImage()
{

    QString szimg = QFileDialog::getOpenFileName(this, "Open Image File", ".", "v2Image (*.v2img);");
    vatek_romtool::openRomImage(szimg);
   
}

void vatek_romtool::setStatus(romtool_status status)
{
	ui->actionClose->setEnabled(status != romtool_idle);
	ui->actionImage->setEnabled(status == romtool_idle);
	ui->actionDevice->setEnabled(status == romtool_idle);
	ui->actionUpdate->setEnabled(status != romtool_idle);
	ui->actionExport->setEnabled(status != romtool_idle);
	ui->actionUpdateDevice->setEnabled(status == romtool_device);
}

vatek_result vatek_romtool::openRomImage(QString& fimg)
{
    Pstorage_handle pstorage = NULL;

    // 20220803 - fix chinese path
    vatek_result nres = vatek_storage_open_file_handle(fimg.toLocal8Bit(), &pstorage, storage_handler, this);
    
    if (is_vatek_success(nres))
    {
        hvatek_storage hstorage = NULL;
        nres = vatek_storage_open(pstorage, &hstorage, 1);
        if (is_vatek_success(nres))
        {
            m_hstorage = hstorage;
            recvservice(fimg);
        }


        if (!is_vatek_success(nres))
            vatek_storage_free_handle(pstorage);
    }

    if (!is_vatek_success(nres))
        QMessageBox::warning(this, windowTitle(), QString::asprintf("open image fail : %d", nres));
    return nres;
}

//void vatek_romtool::closeRom()
//{
//    if (m_hstorage)vatek_storage_close(m_hstorage);
//    QFile::remove(imageTempFile);
//
//    openfiledlg::setDefaultDir(imageTempFile);
//    qtvUIStorageR2Tune::setpathInit();
//    m_hstorage = NULL;
//    m_istempimage = false;
//    refreshUIStatus();
//}

void vatek_romtool::storage_handler(rom_progress_msg msg, void* progressval, void* param)
{
    vatek_romtool* promtool = (vatek_romtool*)param;
}