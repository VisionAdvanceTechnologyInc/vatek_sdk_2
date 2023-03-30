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

#include <vatek_factory.h>
#include <openfiledlg.h>
#include <QMessagebox>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QDate>
#include <QStackedWidget>
#include <QScreen>
#include <qDebug>
#include <windows.h>
#include <conio.h>
const char* vatek_factory::imageTempFile = "~tmpimage.vimage";

QString image_path = "";

vatek_factory::vatek_factory(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::vatek_factory),
    m_hstorage(NULL),
    m_istempimage(false)
{
    ui->setupUi(this);
    ui->lisections->attachStacked(ui->stpages);

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
        setMinimumSize(screen_width/1.5, screen_height /1.3);
    }

	setFixedSize(size());
	statusBar()->setSizeGripEnabled(false);
    refreshUIStatus();
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(recvOpenFile()));
    connect(ui->actionOpenImage, SIGNAL(triggered()), this, SLOT(recvOpenImage()));
    connect(ui->actionBuildImage, SIGNAL(triggered()), this, SLOT(recvBuildImage()));
    connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(recvCloseFile()));
}

vatek_factory::~vatek_factory()
{
    closeRom();
	disconnect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(recvOpenFile()));
    disconnect(ui->actionOpenImage, SIGNAL(triggered()), this, SLOT(recvOpenImage()));
    disconnect(ui->actionBuildImage, SIGNAL(triggered()), this, SLOT(recvBuildImage()));
    disconnect(ui->actionClose, SIGNAL(triggered()), this, SLOT(recvCloseFile()));
    delete ui;
}

vatek_result vatek_factory::openRomImage(QString& fimg)
{
    //std::string szimg = fimg.toStdString();
    Pstorage_handle pstorage = NULL;
    vatek_result nres = vatek_storage_open_file_handle(fimg.toLocal8Bit(), &pstorage, storage_handler, this);
    if (is_vatek_success(nres))
    {
        hvatek_storage hstorage = NULL;
        nres = vatek_storage_open(pstorage, &hstorage,1);
        if (is_vatek_success(nres))
        {
            closeRom();
            m_hstorage = hstorage;
            m_istempimage = false;
            
            refreshUIStatus();
        }

        if (!is_vatek_success(nres))
            vatek_storage_free_handle(pstorage);
    }

	if (!is_vatek_success(nres))
		QMessageBox::warning(this, windowTitle(), QString::asprintf("open image fail : %d", nres));
    return nres;
}

vatek_result vatek_factory::openRom(QString& floader, QString& fapp, QString& fr2_tune)
{
    //std::string szloader = floader.toStdString();
    //std::string szapp = fapp.toStdString();
    //std::string szr2_tune = fr2_tune.toStdString();

    Pstorage_handle pstorage = NULL;

    vatek_result nres = vatek_storage_create_file_handle(imageTempFile, floader.toLocal8Bit(), fapp.toLocal8Bit(), &pstorage, storage_handler, this);
    if (is_vatek_success(nres))
    {
        hvatek_storage hstorage = NULL;
        nres = vatek_storage_open(pstorage, &hstorage,1);
        if (is_vatek_success(nres))
        {
            closeRom();
            m_hstorage = hstorage;
            m_istempimage = false;
            refreshUIStatus();
        }
        if(!is_vatek_success(nres))
            vatek_storage_free_handle(pstorage);
    }

    if (!is_vatek_success(nres))
        QMessageBox::warning(this, windowTitle(), QString::asprintf("open file fail : %d", nres));
    return nres;
}

void vatek_factory::closeRom()
{
    if (m_hstorage)vatek_storage_close(m_hstorage);
    QFile::remove(imageTempFile);

    openfiledlg::setDefaultDir(imageTempFile);
    qtvUIStorageR2Tune::setpathInit();
    m_hstorage = NULL;
    m_istempimage = false;
    refreshUIStatus();
}

void vatek_factory::recvOpenFile()
{
    openfiledlg* filedlg = new openfiledlg(this);
    int32_t nres = filedlg->exec();
    if (nres == QDialog::DialogCode::Accepted)
        openRom(filedlg->getLoaderFile(), filedlg->getAppFile(), filedlg->getRFTableFile());
    delete filedlg;
}

void vatek_factory::recvOpenImage()
{
    QString szimgfile = QFileDialog::getOpenFileName(this, "Open Image", openfiledlg::getDefaultDir(), "v2Image (*.v2img)");
    if (!szimgfile.isEmpty())
    {
        openfiledlg::setDefaultDir(szimgfile);
        openRomImage(szimgfile);
        vatek_factory::getimg(szimgfile);
    }
}

void vatek_factory::recvCloseFile()
{

    closeRom();
}

void vatek_factory::recvBuildImage()
{
    if (m_hstorage)
    {
        QString szfilename;
        Papp_header papp = NULL;
        vatek_result nres = vatek_storage_get_app(m_hstorage, &papp);
        if (is_vatek_success(nres))
        {
            QDateTime t = QDateTime::currentDateTime();
            const char* szapp = "app2_broadcast";
            if (papp->service_mode == service_transform)
                szapp = "app2_transform";
            szfilename = QString::asprintf("%s_%02d%02d%02d_%02d%02d_[%04x]", 
                                           szapp, 
                                           t.date().year(),
                                           t.date().month(),
                                           t.date().day(),
                                           t.time().hour(),
                                           t.time().minute(),
                                           papp->major_version);

            szfilename = QDir::cleanPath(openfiledlg::getDefaultDir() + QDir::separator() + szfilename);
            szfilename = QFileDialog::getSaveFileName(this, "Build Image", szfilename, "v2Image (*.v2img)");
            if (!szfilename.isEmpty())
            {
                std::string szimg = szfilename.toLocal8Bit();
                nres = vatek_storage_save(m_hstorage, szimg.c_str());
            }
            else return;

            if (!is_vatek_success(nres))
                QMessageBox::warning(this, windowTitle(), QString::asprintf("Build image fail : %d", nres));
            else QMessageBox::information(this, windowTitle(), QString::asprintf("Build image success!"));
        }
        else QMessageBox::warning(this,windowTitle(), "can not found current application and loader");
    }
}

void vatek_factory::storage_handler(rom_progress_msg msg, void* progressval, void* param)
{
    vatek_factory* pfactory = (vatek_factory*)param;
}

bool vatek_factory::isValid()
{
    return (m_hstorage != NULL);
}

void vatek_factory::refreshUIStatus()
{
	ui->actionBuildImage->setEnabled(isValid());
	ui->actionOpen->setEnabled(!isValid());
	ui->actionClose->setEnabled(isValid());
    ui->actionOpenImage->setEnabled(!isValid());

    ui->lisections->refreshStorage(m_hstorage);

}


void vatek_factory::getimg(QString img) {
    image_path = img;
}
