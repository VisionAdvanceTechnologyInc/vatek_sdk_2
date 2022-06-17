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

#include "./internal/qinternal_ui.h"
#include <ui/base/qtv_ui_core.h>

/* qui_pixmap */

qui_resource::qui_pixmap::qui_pixmap(uint32_t id, QImage* image) :
	m_id(id),
	m_image(image)
{

}

qui_resource::qui_pixmap::~qui_pixmap()
{
	delete m_image;
}

/* qui_resource */

qui_resource& qui_resource::getInstance()
{
	static qui_resource oneInstance;
	return oneInstance;
}

qui_resource::qui_resource()
{
#if defined(LIBVATEK_QT_LIB) || defined(LIBVATEK_QT_STATIC)
	Q_INIT_RESOURCE(qtv_toolkit);
#endif
	m_images.append(new qui_pixmap(ic_module_unknown, new QImage(":/qtv/chip_unknown")));
	m_images.append(new qui_pixmap(ic_module_a1, new QImage(":/qtv/chip_a1")));
	m_images.append(new qui_pixmap(ic_module_a3, new QImage(":/qtv/chip_a3")));
	m_images.append(new qui_pixmap(ic_module_b1, new QImage(":/qtv/chip_b1")));
	m_images.append(new qui_pixmap(ic_module_b2, new QImage(":/qtv/chip_b2")));
	m_images.append(new qui_pixmap(ic_module_b2_plus, new QImage(":/qtv/chip_b2_plus")));
	m_images.append(new qui_pixmap(ic_module_b3_lite, new QImage(":/qtv/chip_b3")));
	m_images.append(new qui_pixmap(ic_module_b3_plus, new QImage(":/qtv/chip_b3_plus")));
	m_images.append(new qui_pixmap(ICON_BROADCAST, new QImage(":/qtv/broadcast")));
	m_images.append(new qui_pixmap(ICON_TRANSFORM, new QImage(":/qtv/transform")));
	m_images.append(new qui_pixmap(ICON_LOGOBIG, new QImage(":/qtv/logo_big")));
	m_images.append(new qui_pixmap(ICON_LOGOSMALL, new QImage(":/qtv/logo_small")));
	m_images.append(new qui_pixmap(ICON_NODEVICE, new QImage(":/qtv/no_device")));
	m_images.append(new qui_pixmap(ICON_ERROR, new QImage(":/qtv/error")));
	m_images.append(new qui_pixmap(ICON_VIDEO, new QImage(":/qtv/icon_video")));
	m_images.append(new qui_pixmap(ICON_AUDIO, new QImage(":/qtv/icon_audio")));
	m_images.append(new qui_pixmap(ICON_CHANNEL, new QImage(":/qtv/icon_channel")));
	m_images.append(new qui_pixmap(ICON_OUTPUT, new QImage(":/qtv/icon_output")));
	m_images.append(new qui_pixmap(ICON_PROPERTIES, new QImage(":/qtv/icon_properties")));
	m_images.append(new qui_pixmap(ICON_INFORMATION, new QImage(":/qtv/icon_information")));
}

qui_resource::~qui_resource()
{
	while (m_images.count())
	{
		delete m_images.first();
		m_images.removeFirst();
	}

#if defined(LIBVATEK_QT_LIB) || defined(LIBVATEK_QT_STATIC)
	Q_CLEANUP_RESOURCE(qtv_toolkit);
#endif
}

QImage* qui_resource::getImage(uint32_t id)
{
	int32_t i = 0;
	for (i = 0; i < m_images.count(); i++)
	{
		if (id == m_images.at(i)->_id())
			return m_images.at(i)->_image();
	}
	return NULL;
}

/* qtvUIResource */

QImage* qtvUIResource::getDeviceImage(vatek_ic_module chip)
{
	QImage* img = qui_resource::getInstance().getImage((uint32_t)chip);
	if (!img)img = qui_resource::getInstance().getImage((uint32_t)ic_module_unknown);
	return img;
}

QImage* qtvUIResource::getImage(uint32_t id)
{
	return qui_resource::getInstance().getImage(id);
}
