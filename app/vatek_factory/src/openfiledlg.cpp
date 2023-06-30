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

#include "openfiledlg.h"
#include "ui_openfiledlg.h"
#include <windows.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QDebug>
#include "../../api/qt/ui_qtvUIStorageR2Tune.h"
#include "ui/base/qtv_ui_storage.h"

QString openfiledlg::m_pathdir;
QString openfiledlg::m_app2_path;

openfiledlg::openfiledlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::openfiledlg)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    connect(ui->btn_loader, SIGNAL(clicked(bool)), this, SLOT(recvClickLoader(bool)));
    connect(ui->btn_app, SIGNAL(clicked(bool)), this, SLOT(recvClickApp(bool)));
    connect(ui->btn_app2, SIGNAL(clicked(bool)), this, SLOT(recvClickApp_2(bool)));
    connect(ui->btn_ok, SIGNAL(clicked(bool)), this, SLOT(recvClickOK(bool)));

    ui->btn_ok->setEnabled(checkValid());
    if(m_pathdir.isEmpty())
        m_pathdir = QDir::currentPath();
}

openfiledlg::~openfiledlg()
{
    delete ui;
}

void openfiledlg::recvClickLoader(bool checked)
{
	QString fileName = QFileDialog::getOpenFileName(this, "Open Loader", m_pathdir, "Loader Files (*.v2loader *.v2lic)");
    if (!fileName.isEmpty())
    {
        m_pathdir = QFileInfo(fileName).absoluteFilePath();
        ui->txt_loader->setText(fileName);
    }
	ui->btn_ok->setEnabled(checkValid());
}

void openfiledlg::recvClickApp(bool checked)
{
	QString fileName = QFileDialog::getOpenFileName(this, "Open App_1", m_pathdir, "App Files (*.v2app)");
    if (!fileName.isEmpty())
    {
        setDefaultDir(fileName);
        ui->txt_app->setText(fileName);
    }
	ui->btn_ok->setEnabled(checkValid());
}

void openfiledlg::recvClickApp_2(bool checked)
{
	QString fileName = QFileDialog::getOpenFileName(this, "Open App_2", m_pathdir, "App Files (*.v2app)");
    if (!fileName.isEmpty())
    {
        setDefaultDir(fileName);
        ui->txt_app2->setText(fileName);
    }
	ui->btn_ok->setEnabled(checkValid());

}

void openfiledlg::recvClickOK(bool checked)
{

    if (QFileInfo::exists(ui->txt_app->text()) &&
        QFileInfo::exists(ui->txt_loader->text()))accept(); /* &&
        QFileInfo::exists(ui->txt_rf->text()))accept();*/
    m_app2_path = ui->txt_app2->text();
    qDebug() << "My Value is " << m_app2_path;

    //qtvUIStorageR2Tune::recvOpenFile_new(m_r2_path);
}

bool openfiledlg::checkValid()
{
    if (!ui->txt_app->text().isEmpty() &&
        !ui->txt_loader->text().isEmpty()) /*&&
        !ui->txt_rf->text().isEmpty())*/
    {
        return true;
    }
    return false;
}

QString openfiledlg::getAppFile()
{
    return ui->txt_app->text();
}

QString openfiledlg::getLoaderFile()
{
    return ui->txt_loader->text();
}

QString openfiledlg::getRFTableFile()
{
    return ui->txt_app2->text();
}

QString& openfiledlg::getRFTableFile_tonew()
{
    return m_app2_path;
}

QString& openfiledlg::getDefaultDir()
{
    return m_pathdir;
}

void openfiledlg::setDefaultDir(QString fullpath)
{
    m_pathdir = QFileInfo(fullpath).absoluteFilePath();
}

