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

#ifndef OPENFILEDLG_H
#define OPENFILEDLG_H

#include <QDialog>
#include "ui/base/qtv_ui_storage.h"

namespace Ui {
class openfiledlg;

}

class openfiledlg : public QDialog
{
    Q_OBJECT

public:
    explicit openfiledlg(QWidget *parent = nullptr);
    ~openfiledlg();

    QString getAppFile();
    QString getLoaderFile();
    QString getRFTableFile();

    static QString& getDefaultDir();
    static QString& getRFTableFile_tonew();
    static void setDefaultDir(QString fullpath);


protected Q_SLOTS:
    void recvClickLoader(bool checked);
    void recvClickApp(bool checked);
    void recvClickApp_2(bool checked);
    void recvClickOK(bool checked);

protected:
    static QString m_pathdir;
    static QString m_app2_path;

    bool checkValid();

private:
    Ui::openfiledlg *ui;
    Ui::qtvUIStorageR2Tune *ui_r2;
    qtvUIStorageR2Tune *inputdlg;
};


#endif // OPENFILEDLG_H

