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

#ifndef BOOTLOGO_H
#define BOOTLOGO_H

#include <vatek_sdk_storage.h>
#include <ui/base/qtv_ui_tab.h>
#include <ui/base/qtv_ui_table.h>
#include <ui/base/qtv_ui_storage.h>
#include <QWidget>
#include <QBitmap>
#include <QPainter>
#include <core/storage/storage_section.h>

QT_BEGIN_NAMESPACE
namespace Ui { class bootlogo; }
QT_END_NAMESPACE

class QTV_DECL bootlogo : public qtvUITabBase
{
    Q_OBJECT
#define _CR(c)	((uint8_t)(c))
#define _CG(c)	((uint8_t)(c >> 8))
#define _CB(c)	((uint8_t)(c >> 16))

#define RGB_2_C(r,g,b)	((r ) | (g << 8) | (b << 16))
#define COLOR_WHITE	RGB_2_C(0xFF,0xFF,0xFF)

public:
    explicit bootlogo(qtvUIMain* main, QWidget* parent = nullptr);
    virtual ~bootlogo();

    void tabFinish() override;
    void tabLeave() override;
    static void setpathInit();

    void set_resolution(uint32_t w, uint32_t h, QColor color, int index);
    void reset_preview(QPalette);
    uint32_t get_height();
    bool check_is_loaded();
    bool put_resource(Pbin_resource_section presource);
    bool put_resource_raw(Pbin_resource_section presource);
    void load_resource(Pbin_resource_section presource, QColor* bkclr);
    bool get_resource_size(uint32_t* w, uint32_t* h);
    bool get_resource_data(Pbin_resource_section pres);
    bool convert32to8(QImage& rgbImage, QImage& grayImage);

    uint32_t bootlogo::get_width();

protected Q_SLOTS:
    void tabEnter() override;

    void recvImage(bool bclick);
    void load_image_file(QString filename);
    void OnSectionsAddres(bool bclick);
    void OnSectionsDelres(bool bclick);
    void choose_color(bool bclick);

protected:
    bool draw_resource_raw(Pbin_resource_section presolution);
    bool draw_resource(Pbin_resource_section presolution);
    uint32_t m_framew;
    uint32_t m_frameh;
    double m_framescle;
    QRect m_rectimg;
    double m_scale;
    QImage* m_bmpbase;
    QImage* m_bmpload;
    QImage* m_imgload;
    QRectF m_rectrange;
    QColor m_colorbk;
    void redraw_base(int);
    void redraw_base_test(void);
    Pbin_resource_section m_resbin;
    QImage Image;

private:
    Ui::bootlogo *ui;
};

#endif // BOOTLOGO_H
