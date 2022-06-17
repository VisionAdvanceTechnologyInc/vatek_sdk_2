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

#include <ui/base/qtv_ui_tab.h>
#include <QStackedWidget>

#include "ui_qtvUITabConfigs.h"

#define UITAB_CONFIG_NAME			"Configs"
#define UITAB_CONFIG_DESC			"Config and list all supported properties"
#define UITAB_CONFIG_ICONSIZE		32

#define UISUB_TAB_VIDEO_NAME		"Video Codec"
#define UISUB_TAB_VIDEO_DESC		"config video default codec and properties"
#define UISUB_TAB_AUDIO_NAME		"Audio Codec"
#define UISUB_TAB_AUDIO_DESC		"config audio default codec and properties"
#define UISUB_TAB_CHANNEL_NAME		"Channel"
#define UISUB_TAB_CHANNEL_DESC		"config DTV channel and program properties"
#define UISUB_TAB_OUTPUT_NAME		"Output"
#define UISUB_TAB_OUTPUT_DESC		"config output frequency and modulation"
#define UISUB_TAB_MUXPARAM_NAME		"Muxer"
#define UISUB_TAB_MUXPARAM_DESC		"config mpeg-ts muxer properties"

/* qinternal_tab_properties */

class qinternal_tab_properties : public qtvUITabProperties
{
public:
	explicit qinternal_tab_properties(qtvUIMain* main,qtvUIListItem* list, qtvUIConfigBase* config,QWidget* parent = nullptr);
	void tabEnter() override;
	void tabLeave() override;
	void tabFinish() override;

protected:
	qtvUIListItem* m_listitem;
	qtvUIConfigBase* m_config;
};

class qinternal_tab_context : public qtvUITabContext
{
public:
	explicit qinternal_tab_context(qtvUIMain* main, qtvUIListItem* list, qtvUIConfigBase* config, QWidget* parent = nullptr);
	void tabEnter() override;
	void tabLeave() override;
	void tabFinish() override;

protected:
	qtvUIListItem* m_listitem;
	qtvUIConfigBase* m_config;
};

/* qtvUITabConfigs */

qtvUITabConfigs::qtvUITabConfigs(qtvUIMain* main, QWidget* parent) :
	qtvUITabBase(main, UITAB_CONFIG_NAME, UITAB_CONFIG_DESC, parent),
	ui(new Ui::qtvUITabConfigs),
	m_select(NULL)
{
	ui->setupUi(this);
	resetConfigs();
	connect(ui->listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(recvItemSelected()));
}

qtvUITabConfigs::~qtvUITabConfigs()
{
	disconnect(ui->listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(recvItemSelected()));
	resetConfigs();
}

void qtvUITabConfigs::tabEnter()
{

}

void qtvUITabConfigs::tabFinish()
{

}

void qtvUITabConfigs::tabLeave()
{

}

void qtvUITabConfigs::resetConfigs()
{
	while (ui->st_context->count())
	{
		QWidget* widget = ui->st_context->widget(0);
		qtvUITabBase* tab = qobject_cast<qtvUITabBase*>(widget);
		if(tab)tab->tabLeave();
		ui->st_context->removeWidget(widget);
	}

	ui->listWidget->resetList(qui_list_logo_title_1, UITAB_CONFIG_ICONSIZE);

	while (m_configs.count())
	{
		delete m_configs.first();
		m_configs.removeFirst();
	}
}

vatek_result qtvUITabConfigs::addConfig(qtvUIConfigBase* config)
{
	vatek_result nres = vatek_unsupport;
	qtvUITabBase* tab = NULL;
	qtvUIListItem* listitem = NULL;

	if (config->_mode() == qui_config_videocodec)
	{
		listitem = new qtvUIListItem(ICON_VIDEO, UITAB_CONFIG_ICONSIZE, UISUB_TAB_VIDEO_NAME, UISUB_TAB_VIDEO_DESC);
		tab = new qinternal_tab_properties(m_main, listitem, config, this);
	}
	else if (config->_mode() == qui_config_audiocodec)
	{
		listitem = new qtvUIListItem(ICON_AUDIO, UITAB_CONFIG_ICONSIZE, UISUB_TAB_AUDIO_NAME, UISUB_TAB_AUDIO_DESC);
		tab = new qinternal_tab_properties(m_main, listitem, config, this);
	}
	else if (config->_mode() == qui_config_muxchannel)
	{
		listitem = new qtvUIListItem(ICON_CHANNEL, UITAB_CONFIG_ICONSIZE, UISUB_TAB_CHANNEL_NAME, UISUB_TAB_CHANNEL_DESC);
		tab = new qinternal_tab_context(m_main, listitem, config, this);
	}
	else if (config->_mode() == qui_config_output)
	{
		listitem = new qtvUIListItem(ICON_OUTPUT, UITAB_CONFIG_ICONSIZE, UISUB_TAB_OUTPUT_NAME, UISUB_TAB_OUTPUT_DESC);
		tab = new qinternal_tab_properties(m_main, listitem, config, this);
	}
	else if (config->_mode() == qui_config_muxparam)
	{
		listitem = new qtvUIListItem(ICON_PROPERTIES, UITAB_CONFIG_ICONSIZE, UISUB_TAB_MUXPARAM_NAME, UISUB_TAB_MUXPARAM_DESC);
		tab = new qinternal_tab_properties(m_main, listitem, config, this);
	}

	if (listitem && tab)
	{
		listitem->setWidget(tab);
		ui->listWidget->insertItem(listitem);
		ui->st_context->addWidget(tab);
		tab->tabEnter();
		nres = vatek_success;
	}
	return nres;
}

void qtvUITabConfigs::setCurrentSelect(int32_t index)
{
	ui->listWidget->setSelectConfig(index);
}

void qtvUITabConfigs::recvItemSelected()
{
	qtvUIListItem* config = ui->listWidget->getSelectConfig();
	if (config)
	{
		m_select = config;
		ui->st_context->setCurrentWidget(m_select->_widget());
	}
}

/* qinternal_config */

qtvUITabConfigs::qinternal_config::qinternal_config(qtvUIConfigBase* config, qtvUITabBase* tab, qtvUIListItem* list) :
	m_config(config),
	m_tab(tab),
	m_listitem(list)
{

}

qtvUITabConfigs::qinternal_config::~qinternal_config()
{
	delete m_tab;
}

/* qinternal_tab_properties */

qinternal_tab_properties::qinternal_tab_properties(qtvUIMain* main, qtvUIListItem* list, qtvUIConfigBase* config, QWidget* parent) :
	m_listitem(list),
	m_config(config),
	qtvUITabProperties(main, list->_title_0(), list->_title_1(), parent)
{

}

void qinternal_tab_properties::tabEnter()
{
	if (m_config->_mode() == qui_config_videocodec ||
		m_config->_mode() == qui_config_audiocodec)
	{
		qtvUIConfigCodec* codeccfg = qobject_cast<qtvUIConfigCodec*>(m_config);
		refreshProperties(qui_table_normal, codeccfg->_properties());
	}
	else
	{
		qtvUIConfigProperties* propcfg = qobject_cast<qtvUIConfigProperties*>(m_config);
		refreshProperties(qui_table_normal, propcfg->_properties());
	}
}

void qinternal_tab_properties::tabLeave()
{

}

void qinternal_tab_properties::tabFinish()
{

}

/* qinternal_tab_context */

qinternal_tab_context::qinternal_tab_context(qtvUIMain* main, qtvUIListItem* list, qtvUIConfigBase* config, QWidget* parent) :
	m_listitem(list),
	m_config(config),
	qtvUITabContext(main, list->_title_0(), list->_title_1(), parent)
{

}

void qinternal_tab_context::tabEnter()
{
	
}

void qinternal_tab_context::tabLeave()
{

}

void qinternal_tab_context::tabFinish()
{

}