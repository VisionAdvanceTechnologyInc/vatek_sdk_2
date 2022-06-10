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

#include "..\..\qt\inc\ui\base\qtv_ui_bootlogo.h"
#include "./ui_bootlogo.h"

#include <QMessagebox>
#include <QFile>
#include <QFileDialog>
#include <QImageWriter>
#include <QBitmap>
#include <QPainter>
#include <QColorDialog>
#include <math.h>
#include <bitset>
#include <iostream>
#include <string>
#include <fstream>
#include <QBuffer>
#include <QScreen>
#include <qDebug>
#include <windows.h>
#include <conio.h>

extern void ycbcr_to_color(uint32_t cycbcr, QColor* clr);
extern uint32_t color_to_ycbcr(QColor clr); 

static const int32_t frame_size[4][2] =
{
	{1920,1080,},{1280,720,},{720,576,},{720,480,}
};

bootlogo::bootlogo(qtvUIMain* main, QWidget* parent) :
	qtvUITabBase(main, "Bootlogo", "Bootlogo", parent),
	ui(new Ui::bootlogo)
{
    ui->setupUi(this);

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
	}
	else {
		setMinimumSize(screen_width / 2, screen_height / 2);
	}

	connect(ui->btn_loadimage, SIGNAL(clicked(bool)), this, SLOT(recvImage(bool)));
	connect(ui->btn_re_ok , SIGNAL(clicked(bool)), this, SLOT(OnSectionsAddres(bool)));
    connect(ui->Button_color, SIGNAL(clicked(bool)), this, SLOT(choose_color(bool)));
	connect(ui->cmb_index, SIGNAL(currentIndexChanged(int)), this, SLOT(tabEnter()));
	connect(ui->btn_re_delete, SIGNAL(clicked(bool)), this, SLOT(OnSectionsDelres(bool)));

	for (int i = 0; i < sizeof(frame_size) / sizeof(frame_size[0]); i++) {
		char str[12];
		sprintf(str, "%d", frame_size[i][1]);
		ui->cmb_resolution->addItem(str);
	}

	ui->cmd_mode->addItem("Raw Mode");
	ui->cmd_mode->addItem("Compression Mode");
	ui->btn_re_ok->setEnabled(false);
	ui->Button_color->setEnabled(false);
}

bootlogo::~bootlogo()
{
    delete ui;
	if (m_bmpbase != NULL)
		delete(m_bmpbase);
	if (m_bmpload != NULL)
		delete(m_bmpload);
	if (m_imgload != NULL)
		delete(m_imgload);
	disconnect(ui->btn_loadimage, SIGNAL(clicked(bool)), this, SLOT(recvImage(bool)));
	disconnect(ui->btn_re_ok, SIGNAL(clicked(bool)), this, SLOT(OnSectionsAddres(bool)));
	disconnect(ui->Button_color, SIGNAL(clicked(bool)), this, SLOT(choose_color(bool)));
	disconnect(ui->cmb_index, SIGNAL(currentIndexChanged(int)), this, SLOT(tabEnter()));
	disconnect(ui->btn_re_delete, SIGNAL(clicked(bool)), this, SLOT(OnSectionsDelres(bool)));

}

void bootlogo::tabEnter()
{
	QColor bkclr;
	if (qtvUIMain::is_uimain_storage(m_main->_mode()))
	{
		int resol_index = ui->cmb_resolution->currentIndex();

		qtvUIMainStorage* resource_storage = qobject_cast<qtvUIMainStorage*>(m_main);
		hvatek_storage hstorage = resource_storage->_storage_handle();
		Pstorage_resource res = NULL;
		int i = 1;
		vatek_storage_get_resource(hstorage, &res);

		// add combobox index
		if (ui->cmb_index->count() == 0) {
			int count = 0;
			while (res)
			{
				count = count + 1;
				char buffer[32];
				itoa(count, buffer, 10);
				ui->cmb_index->addItem(buffer);
				res = res->next;
			}
		}
		vatek_storage_get_resource(hstorage, &res);

		if (res != NULL) {
			if (ui->cmb_index->count() == 1) {
				load_resource(res->resource, &bkclr);
				set_resolution(frame_size[resol_index][0], frame_size[resol_index][1], bkclr, 1);
			}
			else {
				for (i = 1; i <= ui->cmb_index->count(); i++) {
					if (res->resource->index == ui->cmb_index->currentIndex()) {
						load_resource(res->resource, &bkclr);
						set_resolution(frame_size[resol_index][0], frame_size[resol_index][1], bkclr, 1);
					}
					res = res->next;
				}
			}
		}
		else {
			QPalette palette;
			palette.setColor(QPalette::Background, QColor(255, 255, 255));
			ui->color_label->setAutoFillBackground(true);
			ui->color_label->setPalette(palette);
		}
	}
}

void bootlogo::tabFinish()
{

}

void bootlogo::tabLeave()
{

}

void bootlogo::recvImage(bool bclick)
{
	if (ui->cmd_mode->currentIndex() == 0) {
		QString path = QFileDialog::getOpenFileName(this, "Open File Image", ".", "PNG BMP (*.bmp;*.png);");
		if (path != "") {
			load_image_file(path);
		}
	}
	else {
		QString path = QFileDialog::getOpenFileName(this, "Open File Image", ".", "BMP (*.bmp);");
		if (path != "") {
			load_image_file(path);
		}
	}

}

uint32_t bootlogo::get_width()
{
	if (m_imgload != NULL)return m_imgload->width();
	return 0;
}

uint32_t bootlogo::get_height()
{
	if (m_imgload != NULL)return m_imgload->height();
	return 0;
}

void bootlogo::redraw_base(int index)
{
	if (m_bmpbase != NULL)
	{
		QBrush bkbrush(QColor(240, 240, 240));
		QBrush bkimgbk(m_colorbk);

		QPainter gpaint(m_bmpbase);
		QImage* curshow = NULL;
		gpaint.fillRect(0, 0, m_bmpbase->width(), m_bmpbase->height(), bkbrush);
		gpaint.fillRect(m_rectimg, bkimgbk);

		if (index == 0) {
			curshow = m_imgload;
		}
		else {
			curshow = m_bmpload;
		}
		if (curshow != NULL)
		{
			int32_t iw = (int32_t)(curshow->width() * m_framescle);
			int32_t ih = (int32_t)(curshow->height() * m_framescle);
			QRect rect;

			if (m_rectimg.width() < iw)iw = m_rectimg.width();
			if (m_rectimg.height() < ih)ih = m_rectimg.height();

			rect.setX((m_rectimg.width() - iw) / 2 + m_rectimg.left());
			rect.setY((m_rectimg.height() - ih) / 2 + m_rectimg.top());
			rect.setWidth(iw);
			rect.setHeight(ih);

			gpaint.drawImage(rect, *curshow);

			QPixmap pixmap_n = QPixmap::fromImage(*m_bmpbase);

			ui->image_view->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
			ui->image_view->setPixmap(pixmap_n);
		}
	}

	QPainter::CompositionMode_Clear;
}

void bootlogo::load_image_file(QString filename)
{
	Pstorage_resource pres = NULL;
	m_imgload = new QImage;
	m_imgload->load(filename);

	int resol_index = ui->cmb_resolution->currentIndex();
	set_resolution(frame_size[resol_index][0], frame_size[resol_index][1], COLOR_WHITE, 0);
	ui->btn_re_ok->setEnabled(true);
	ui->Button_color->setEnabled(true);
}

void bootlogo::load_resource(Pbin_resource_section presource, QColor* bkclr)
{
	m_bmpbase = new QImage;
	m_bmpload = new QImage;
	m_imgload = new QImage;

	if (m_bmpbase != NULL)
		delete(m_bmpbase);
	if (m_bmpload != NULL)
		delete(m_bmpload);
	if (m_imgload != NULL)
		delete(m_imgload);

	ycbcr_to_color(presource->bkcolor, &m_colorbk);
	*bkclr = m_colorbk;
	m_bmpload = new QImage(presource->width, presource->height, QImage::Format_ARGB32);

	if (m_bmpload != NULL)
	{
		QBrush brbk(m_colorbk);
		QPainter gpaint(m_bmpload);
		int32_t i = 0, j = 0;

		gpaint.fillRect(0, 0, m_bmpload->width(), m_bmpload->height(), brbk);

		if (presource->table[RESIDX_TAG] == RESOURCE_RAW_TAG)
		{
			uint8_t* baseY = presource->rawpixel;
			uint8_t* baseUV = baseY + RESOURCE_BUF_Y_SIZE(m_bmpload->width(), m_bmpload->height());
			int32_t mbw = RESOURCE_W_MB(m_bmpload->width());
			int32_t mbh = RESOURCE_H_MB(m_bmpload->height());
			int32_t x, y;

			for (y = 0; y < mbh; y++)
			{
				for (x = 0; x < mbw; x++)
				{
					int32_t mbpos = (y * mbw) + x;
					uint8_t* ptrY = baseY + (mbpos * RESOURCE_Y_MB_SIZE);
					uint8_t* ptrUV = baseUV + (mbpos * RESOURCE_UV_MB_SIZE);
					int32_t i, j, posx, posy;

					posx = x * RESOURCE_MB_WIDTH;
					posy = y * RESOURCE_MB_HEIGHT;

					for (j = 0; j < RESOURCE_MB_HEIGHT; j++)
					{
						if (j + posy > (int32_t)presource->height)break;
						for (i = 0; i < RESOURCE_MB_WIDTH; i += 4)
						{
							int32_t k = 0;
							for (k = 3; k >= 0; k--)
							{
								uint32_t cycrcb = 0;
								QColor clr;
								int32_t curx = posx + i + k;
								uint8_t* ptrposuv = ptrUV + ((j >> 1) * RESOURCE_MB_WIDTH) + (((i + k) >> 1) << 1);

								if (curx > (int32_t)presource->width)break;
								cycrcb |= (*ptrY << 16);
								cycrcb |= (ptrposuv[1] << 8);
								cycrcb |= (ptrposuv[0]);
								ycbcr_to_color(cycrcb, &clr);
								m_bmpload->setPixelColor(curx, posy + j, clr);
								ptrY++;
							}
						}
					}
				}
			}
		}
		else
		{
			uint8_t* ptrpixel = presource->rawpixel;
			for (i = 0; i < (int32_t)presource->width; i++)
			{
				for (j = 0; j < (int32_t)presource->height; j++)
				{
					QColor clr;
					ycbcr_to_color(presource->table[*ptrpixel], &clr);
					ptrpixel++;
					m_bmpload->setPixelColor(i, j, clr);
				}
			}
		}
	}
}

bool bootlogo::draw_resource_raw(Pbin_resource_section presolution)
{
	int32_t mbw = RESOURCE_W_MB(m_imgload->width()) * RESOURCE_MB_WIDTH;
	int32_t mbh = RESOURCE_H_MB(m_imgload->height()) * RESOURCE_MB_HEIGHT;

	QImage* tmpbmp = new QImage(mbw, mbh, QImage::Format_ARGB32);
	QPainter gpaint(tmpbmp);
	QBrush bkclr(m_colorbk);
	if (tmpbmp == NULL)return false;
	else
	{
		int32_t x = 0, y = 0;
		gpaint.fillRect(0, 0, mbw, mbh, bkclr);
		
		int32_t ox = ((mbw - m_imgload->width()) / 2);
		int32_t oy = ((mbh - m_imgload->height()) / 2);

		if (ox < RESOURCE_MB_WIDTH / 2)ox = 0;
		if (oy < RESOURCE_MB_HEIGHT / 2)oy = 0;
		QRect rect(ox, oy, m_imgload->width(), m_imgload->height());

		gpaint.drawImage(rect, *m_imgload);
		
		gpaint.end();

		uint8_t* baseY = presolution->rawpixel;
		uint8_t* baseUV = baseY + RESOURCE_BUF_Y_SIZE(m_imgload->width(), m_imgload->height());
		presolution->bkcolor = color_to_ycbcr(m_colorbk);
		presolution->width = m_imgload->width();
		presolution->height = m_imgload->height();
		QColor test_mode;
		ycbcr_to_color(presolution->bkcolor, &test_mode);
		mbw = RESOURCE_W_MB(m_imgload->width());
		mbh = RESOURCE_H_MB(m_imgload->height());

		memset(&presolution->table[0], 0, RESOURCE_COLOR_LEN * sizeof(uint32_t));
		presolution->table[RESIDX_TAG] = RESOURCE_RAW_TAG;
		presolution->table[RESIDX_MB_W] = mbw;
		presolution->table[RESIDX_MB_H] = mbh;
		presolution->table[RESIDX_SIZE_Y] = RESOURCE_BUF_Y_SIZE(m_imgload->width(), m_imgload->height());
		presolution->table[RESIDX_SIZE_UV] = RESOURCE_BUF_UV_SIZE(m_imgload->width(), m_imgload->height());

		for (y = 0; y < mbh; y++)
		{
			for (x = 0; x < mbw; x++)
			{
				int32_t mbpos = (y * mbw) + x;
				uint8_t* ptrY = baseY + (mbpos * RESOURCE_Y_MB_SIZE);
				uint8_t* ptrUV = baseUV + (mbpos * RESOURCE_UV_MB_SIZE);
				int32_t i, j, posx, posy;

				posx = x * RESOURCE_MB_WIDTH;
				posy = y * RESOURCE_MB_HEIGHT;

				for (j = 0; j < RESOURCE_MB_HEIGHT; j++)
				{
					for (i = 0; i < RESOURCE_MB_WIDTH; i += 4)
					{
						int32_t k = 0;
						for (k = 3; k >= 0; k--)
						{
							uint32_t cycrcb;
							int32_t curx = posx + i + k;
							QColor clr = tmpbmp->pixel(curx, posy + j);
							cycrcb = color_to_ycbcr(clr);

							*ptrY = (cycrcb >> 16) & 0xFF;
							if (j % 2 == 0)
							{
								if (curx % 2 == 0)*ptrUV = (uint8_t)((cycrcb >> 8) & 0xFF);
								else *ptrUV = (uint8_t)(cycrcb & 0xFF);
								ptrUV++;
							}
							ptrY++;
						}
					}
				}
			}
		}
		delete tmpbmp;
	}
	return true;
}

bool bootlogo::draw_resource(Pbin_resource_section presolution)
{
	QImage* tmpbmp = new QImage(m_imgload->width(), m_imgload->height(), QImage::Format_ARGB32);
	QPainter gpaint(tmpbmp);
	QBrush bkclr(m_colorbk);
	QPalette* pattern = (QPalette*)malloc(sizeof(QPalette) + 255 * sizeof(QImage::Format_ARGB32));
	if (tmpbmp == NULL || pattern == NULL)return false;
	else
	{
		int32_t i = 0, j = 0, k = 0;
		QRect rect(0, 0, tmpbmp->width(), tmpbmp->height());
		gpaint.fillRect(0, 0, m_imgload->width(), m_imgload->height(), bkclr);
		gpaint.drawImage(rect, *m_imgload);
		gpaint.end();

		uint8_t* ptrpixel = presolution->rawpixel;
		presolution->bkcolor = color_to_ycbcr(m_colorbk);
		presolution->width = tmpbmp->width();
		presolution->height = tmpbmp->height();

		*tmpbmp = m_imgload->convertToFormat(QImage::Format_Indexed8,Qt::ThresholdDither | Qt::AutoColor);

		QVector<QRgb> colorTable = tmpbmp->colorTable() ;

		for (i = 0; i < 256; i++) {
			presolution->table[i] = color_to_ycbcr(colorTable[i]);
		}

		for (i = 0; i < tmpbmp->width(); i++)
		{
			for (j = 0; j < tmpbmp->height(); j++)
			{
				int32_t isfound = 0;
				QColor clr = tmpbmp->pixel(i, j);
				for (k = 0; k < 256; k++)
				{
					if (colorTable[k] == clr.rgba())
					{
						*ptrpixel = (uint8_t)k;
						ptrpixel++;
						isfound = 1;
						break;
					}
				}

				if (!isfound)
				{
					vatek_result nres = vatek_memfail;
					break;
				}
			}
		}

		delete tmpbmp;
	}
	return true;
}

uint32_t color_to_ycbcr(QColor clr)
{
	uint32_t clrYUV = 0;
	int32_t val = 0;
	uint8_t r = clr.red();
	uint8_t g = clr.green();
	uint8_t b = clr.blue();
	
	val = (((257 * r + 564 * g + 98 * b) / 1000) + 16) & 0xFF;
	clrYUV = val << 16;

	val = (((-148 * r - 291 * g + 439 * b) / 1000) + 128) & 0xFF;
	clrYUV |= val << 8;

	val = (((439 * r - 368 * g - 71 * b) / 1000) + 128) & 0xFF;
	clrYUV |= val;

	return clrYUV;

}

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
void ycbcr_to_color(uint32_t cycbcr, QColor* clr)
{
	double Y = (double)((cycbcr >> 16) & 0xFF);
	double Cb = (double)((cycbcr >> 8) & 0xFF);
	double Cr = (double)((cycbcr) & 0xFF);

	int r = (int)(Y + 1.40200 * (Cr - 0x80));
	int g = (int)(Y - 0.34414 * (Cb - 0x80) - 0.71414 * (Cr - 0x80));
	int b = (int)(Y + 1.77200 * (Cb - 0x80));

	r = MAX(0, MIN(255, r));
	g = MAX(0, MIN(255, g));
	b = MAX(0, MIN(255, b));
	clr->setRgba(0xFF000000 | (r << 16) | (g << 8) | (b ));
	//clr->setRgb((r) | (g << 8) | (b << 16));
}

bool bootlogo::get_resource_size(uint32_t* w, uint32_t* h)
{
	if (check_is_loaded())
	{
		*w = m_imgload->width();
		*h = m_imgload->height();
		return true;
	}
	return false;
}

bool bootlogo::check_is_loaded()
{
	return (m_imgload != NULL);
}

bool bootlogo::get_resource_data(Pbin_resource_section pres)
{
	if (check_is_loaded())
	{
		if (ui->cmd_mode->currentIndex() == 0) {
			put_resource_raw(pres);
			return true;
		}
		else {
			put_resource(pres);
			return true;
		}
	}
	return false;
}

void bootlogo::OnSectionsAddres(bool bclick)
{
	if (qtvUIMain::is_uimain_storage(m_main->_mode()))
	{
		qtvUIMainStorage* resource_storage = qobject_cast<qtvUIMainStorage*>(m_main);
		Papp_header papp = NULL;
		Ploader_header ploader = NULL;
		hvatek_storage hstorage = resource_storage->_storage_handle();
		if (hstorage)
		{
			uint32_t w, h;
			if (bootlogo::get_resource_size(&w, &h))
			{
				Pstorage_resource pres = NULL;
				vatek_result nres = vatek_storage_resource_create(hstorage, w, h, &pres);
				if (!is_vatek_success(nres))
					QMessageBox::warning(this, "Bootlogo", QString::asprintf("create storage resource fail"));
				else
				{
					if (!bootlogo::get_resource_data(pres->resource))
						QMessageBox::warning(this, "Bootlogo", QString::asprintf("get resource fail"));
					else
					{
						nres = vatek_storage_add_resource(hstorage, pres);
						if (!is_vatek_success(nres))
							QMessageBox::warning(this, "Bootlogo", QString::asprintf("add reource fail"));
					}
				}

				char buffer[32];
				itoa(pres->resource->index + 1, buffer, 10);

				ui->cmb_index->addItem(buffer);
				ui->cmb_index->setCurrentIndex(pres->resource->index);

				ui->btn_re_ok->setEnabled(false);
				ui->Button_color->setEnabled(false);

			}
		}
	}
}

void bootlogo::OnSectionsDelres(bool bclick)
{
	if (qtvUIMain::is_uimain_storage(m_main->_mode()))
	{
		qtvUIMainStorage* resource_storage = qobject_cast<qtvUIMainStorage*>(m_main);
		Papp_header papp = NULL;
		Ploader_header ploader = NULL;
		Pstorage_resource res = NULL;
		hvatek_storage hstorage = resource_storage->_storage_handle();

		int i = 1;
		vatek_storage_get_resource(hstorage, &res);

		if (res != NULL) {
			if (ui->cmb_index->count() == 1) {
				if (hstorage)
				{
					vatek_result nres = vatek_storage_del_resource(hstorage, res);
					if (!is_vatek_success(nres))
						QMessageBox::warning(this, "Bootlogo", QString::asprintf("delete resource fail"));
					else
					{
						nres = vatek_storage_get_resource(hstorage, &res);
						if (!is_vatek_success(nres))
							QMessageBox::warning(this, "Bootlogo", QString::asprintf("get resource fail"));
					}
					ui->cmb_index->clear();
					ui->image_view->clear();
				}

			}
			else {
				for (i = 1; i <= ui->cmb_index->count(); i++) {
					if (res->resource->index == ui->cmb_index->currentIndex()) {
						if (hstorage)
						{
							vatek_result nres = vatek_storage_del_resource(hstorage, res);
							if (!is_vatek_success(nres))
								QMessageBox::warning(this, "Bootlogo", QString::asprintf("delete resource fail"));
							else
							{
								nres = vatek_storage_get_resource(hstorage, &res);
								if (!is_vatek_success(nres))
									QMessageBox::warning(this, "Bootlogo", QString::asprintf("get resource fail"));
							}
							ui->cmb_index->removeItem(res->resource->index);

							Pstorage_resource P_res = NULL;

							nres = vatek_storage_get_resource(hstorage, &P_res);
							ui->cmb_index->clear();

							int count = 0;
							while (P_res)
							{
								P_res->resource->index = count;
								count = count + 1;
								P_res = P_res->next;
							}
						}
					}
					res = res->next;
				}
			}
		}
	}
}

bool bootlogo::put_resource_raw(Pbin_resource_section presource)
{
	if (m_imgload != NULL)
	{
		if (get_width() == presource->width && get_height() == presource->height)
		{
			return draw_resource_raw(presource);
		}
	}
	else {
		return false;
	}
}

bool bootlogo::put_resource(Pbin_resource_section presource)
{
	if (m_imgload != NULL)
	{
		if (get_width() == presource->width && get_height() == presource->height)
		{
			return draw_resource(presource);
		}
	}
	else {
		return false;
	}
}

void bootlogo::choose_color(bool bclick) {
	QColor color = QColorDialog::getColor(Qt::white, this);

	ui->color_label->clear();
	QPalette palette;

	int r = color.red();
	int g = color.green();
	int b = color.blue();

	palette.setColor(QPalette::Background, QColor(r, g, b));
	ui->color_label->setAutoFillBackground(true); 
	ui->color_label->setPalette(palette);

	QColor t_color = color.rgba(); //QColor(_CR(color.rgba()), _CG(color.rgba()), _CB(color.rgba()));

	int resol_index = ui->cmb_resolution->currentIndex();
	set_resolution(frame_size[resol_index][0], frame_size[resol_index][1], t_color, 0);
}

void bootlogo::set_resolution(uint32_t w, uint32_t h, QColor color, int index)
{
	QRect rect(0, 0, 700, 360);
	QPalette palette;
	uint32_t tw, th;
	m_framew = w;
	m_frameh = h;
	m_colorbk = color.rgba();  //QColor(_CR(color.rgba()), _CG(color.rgba()), _CB(color.rgba()));
	//m_colorbk = color;
	palette.setColor(QPalette::Background, QColor(m_colorbk.red(), m_colorbk.green(), m_colorbk.blue()));
	ui->color_label->setAutoFillBackground(true);
	ui->color_label->setPalette(palette);

	m_bmpbase = new QImage(rect.width(), rect.height(), QImage::Format_ARGB32);
	th = (w * rect.height()) / h;
	tw = (h * rect.width()) / w;
	if (tw < (uint32_t)rect.width())m_framescle = (double)rect.height() / h;
	else m_framescle = (double)rect.width() / w;

	tw = (uint32_t)(w * m_framescle);
	th = (uint32_t)(h * m_framescle);
	m_rectimg = QRect((rect.width() - tw) / 2, (rect.height() - th) / 2, tw, th);
	redraw_base(index);
}
