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

#include "./internal/source/qsource_base.h"
#include <core/qtv_service.h>

/* qtsin_source_factory */

qtsin_source_factory::qtsin_source_factory()
{
	m_name = "MPEG-TS Interface";
}

qtsin_source_factory::~qtsin_source_factory()
{

}

bool qtsin_source_factory::checkSupported(qtvServiceBase* sevice)
{
	return true;
}

vatek_result qtsin_source_factory::createProperties(qtvServiceBase* service, qtvPropertyCollection** properties)
{
	vatek_result nres = vatek_unsupport;
	if (service->_mode() == qservice_transfrom)
	{
		*properties = new qi_source_properties(service->_device(), stream_source_tsin, qproperties_source_filter);
		nres = vatek_success;
	}
	return nres;
}

vatek_result qtsin_source_factory::checkProperties(qtvPropertyCollection* properties)
{
	return vatek_success;
}

vatek_result qtsin_source_factory::createSource(qtvServiceBase* service, qtvPropertyCollection* properties, qtvSourceBase** source)
{
	vatek_result nres = checkProperties(properties);
	if (is_vatek_success(nres))
	{
		qi_source_properties* qsource = static_cast<qi_source_properties*>(properties);
		if (qsource)*source = new qtsin_source(service, qsource, this);
		else nres = vatek_badparam;
	}
	return nres;
}

/* qtsin_source */

qtsin_source::qtsin_source(qtvServiceBase* service, qi_source_properties* properties, qtvSourceFactory* factory) :
	m_status(qstatus_idle),
	m_service(service),
	m_properties(properties),
	m_factory(factory)
{

}

qtsin_source::~qtsin_source()
{
	delete m_properties;
}

vatek_result qtsin_source::open()
{
	vatek_result nres = vatek_success;
	if (m_status == qstatus_idle)m_status = qstatus_lock;
	else nres = vatek_badstatus;
	return nres;
}

vatek_result qtsin_source::start()
{
	vatek_result nres = vatek_success;
	if (m_status == qstatus_lock)m_status = qstatus_running;
	else nres = vatek_badstatus;
	return nres;
}

vatek_result qtsin_source::polling()
{
	if(m_status == qstatus_running)return vatek_success;
	return vatek_badstatus;
}

void qtsin_source::stop()
{
	if (m_status == qstatus_running)
		m_status = qstatus_lock;
}

void qtsin_source::close()
{
	m_status = qstatus_idle;
}
