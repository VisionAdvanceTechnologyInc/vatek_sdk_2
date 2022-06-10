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

#include "./internal/qinternal_dtv.h"
#include <core/ui/ui_props_define.h>
#include <mux/ui/ui_props_mux.h>
#include <core/ui/ui_props/ui_props_modulator.h>

_ui_struct_start(qtv_locale_param)
	_prop_enum(qtv_locale_param, mux_spec_mode, spec, "dtv - psi service spec", NULLHAL)
	_prop_enum(qtv_locale_param, mux_country_code, country, "dtv - country", NULLHAL)
	_prop_enum(qtv_locale_param, modulator_type, modulation, "dtv - modulation mode", NULLHAL)
	_prop_u32(qtv_locale_param, bw_sb, "dtv - modulatrion bandwidth or symbol rate", NULLHAL)
	_prop_u32(qtv_locale_param, frequency, "dtv - output rf frequency KHz", NULLHAL)
_ui_struct_end

/* qi_dtvchannel_table_manage */

qi_dtvchannel_table_manage& qi_dtvchannel_table_manage::getInstance()
{
	static qi_dtvchannel_table_manage oneInstance;
	return oneInstance;
}

qi_dtvchannel_table_manage::qi_dtvchannel_table_manage()
{

}

qi_dtvchannel_table_manage::~qi_dtvchannel_table_manage()
{
	while (m_tables.count())
	{
		qi_dtvchannel_table* pch = m_tables.first();
		delete pch;
		m_tables.removeFirst();
	}
}

qi_dtvchannel_table* qi_dtvchannel_table_manage::getChannels(Pqdtv_ch_table table)
{
	qi_dtvchannel_table* qtable = nullptr;
	int32_t i = 0;
	for (i = 0; i < m_tables.count(); i++)
	{
		qi_dtvchannel_table* ptr = static_cast<qi_dtvchannel_table*>(m_tables.at(i));
		if (ptr->_table() == table)qtable = ptr;
		if (qtable)break;
	}

	if (!qtable)
	{
		qtable = new qi_dtvchannel_table(table);
		m_tables.append(qtable);
	}
	return qtable;
}

/* qi_dtvchannel_table */

qi_dtvchannel_table::qi_dtvchannel_table(Pqdtv_ch_table table) :
	m_table(table)
{
	int32_t i = 0;
	while (table->channels[i].name)
	{
		m_channels.append(new qi_dtvchannel(&table->channels[i]));
		i++;
	}
}

qi_dtvchannel_table::~qi_dtvchannel_table()
{
	while (m_channels.count())
	{
		qtvDTVChannel* pch = m_channels.first();
		delete pch;
		m_channels.removeFirst();
	}
}

/* qi_dtvlocal */

qi_dtvlocale::qi_dtvlocale(Pqdtv_country locale) : 
	m_locale(locale),
	m_channels(NULL)
{
	m_channels = qi_dtvchannel_table_manage::getInstance().getChannels(locale->channels);
}

qi_dtvlocale::~qi_dtvlocale()
{

}

/* qi_dtvlocale_param */

qi_dtvlocale_param::qi_dtvlocale_properties::qi_dtvlocale_properties(qtvDTVLocale* locale, qtvDTVChannel* channel) :
	m_locale(locale),
	m_channel(channel)
{
	resetProperties();
	refreshProperties();
}

void qi_dtvlocale_param::qi_dtvlocale_properties::setLocale(qtvDTVLocale* locale)
{
	m_param.spec = locale->_spec();
	m_param.country = locale->_country();
	m_param.modulation = locale->_modulation();
	m_param.bw_sb = locale->_bandwidth_symbolrate();
	m_param.video = locale->_video();
	m_param.audio = locale->_audio();
	m_locale = locale;
}

void qi_dtvlocale_param::qi_dtvlocale_properties::setChannel(qtvDTVChannel* channel)
{
	m_param.frequency = channel->_frequency();
	m_channel = channel;
}

void qi_dtvlocale_param::qi_dtvlocale_properties::resetProperties()
{
	memset(&m_param, 0, sizeof(qtv_locale_param));
	setLocale(m_locale);
	setChannel(m_channel);
}

void qi_dtvlocale_param::qi_dtvlocale_properties::refreshProperties()
{
	clearProperties();
	insertProperties(_ui_struct(qtv_locale_param), (uint8_t*)&m_param);
}

/* qtvDTVManage */

qi_dtvlocale_manage& qi_dtvlocale_manage::getInstance()
{
	static qi_dtvlocale_manage oneInstance;
	return oneInstance;
}

qi_dtvlocale_manage::qi_dtvlocale_manage()
{
	int32_t i = 0;
	while (_ucountry_table[i].name)
	{
		m_locales.append(new qi_dtvlocale(&_ucountry_table[i]));
		i++;
	}
}

qi_dtvlocale_manage::~qi_dtvlocale_manage()
{
	while (m_locales.count())
	{
		qtvDTVLocale* plocale = m_locales.first();
		delete plocale;
		m_locales.removeFirst();
	}
}

/* qtvDTVManage */

const QList<qtvDTVLocale*> qtvDTVManage::getDTVLocals()
{
	return qi_dtvlocale_manage::getInstance().getLocale();
}

qtvDTVProperties* qtvDTVManage::createParam(qtvDTVLocale* locale, qtvDTVChannel* channel)
{
	return new qi_dtvlocale_param(locale, channel);
}
