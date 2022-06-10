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


/* qencoder_source_factory */

qencoder_source_factory::qencoder_source_factory()
{
	m_name = "Raw-Encoder";
}

qencoder_source_factory::~qencoder_source_factory()
{

}

bool qencoder_source_factory::checkSupported(qtvServiceBase* sevice)
{
	return true;
}

vatek_result qencoder_source_factory::createProperties(qtvServiceBase* service, qtvPropertyCollection** properties)
{
	vatek_result nres = vatek_unsupport;
	if (service->_mode() == qservice_broadcast)
	{
		*properties = new qi_source_properties(service->_device(), stream_source_encoder, qproperties_source);
		nres = vatek_success;
	}
	return nres;
}

vatek_result qencoder_source_factory::checkProperties(qtvPropertyCollection* properties)
{
	return vatek_success;
}

vatek_result qencoder_source_factory::createSource(qtvServiceBase* service, qtvPropertyCollection* properties, qtvSourceBase** source)
{
	vatek_result nres = checkProperties(properties);
	if (is_vatek_success(nres))
	{
		qi_source_properties* qsource = static_cast<qi_source_properties*>(properties);
		if (qsource)*source = new qencoder_source(service, qsource, this);
		else nres = vatek_badparam;
	}
	return nres;
}

/* qencoder_source */

qencoder_source::qencoder_source(qtvServiceBase* service, qi_source_properties* properties, qtvSourceFactory* factory) :
	m_status(qstatus_idle),
	m_service(service),
	m_properties(properties),
	m_factory(factory)
{

}

qencoder_source::~qencoder_source()
{
	delete m_properties;
}

vatek_result qencoder_source::open()
{
	vatek_result nres = vatek_success;
	if (m_status == qstatus_idle)m_status = qstatus_lock;
	else nres = vatek_badstatus;
	return nres;
}

vatek_result qencoder_source::start()
{
	vatek_result nres = vatek_success;
	if (m_status == qstatus_lock)m_status = qstatus_running;
	else nres = vatek_badstatus;
	return nres;
}

vatek_result qencoder_source::polling()
{
	if (m_status == qstatus_running)return vatek_success;
	return vatek_badstatus;
}

void qencoder_source::stop()
{
	if (m_status == qstatus_running)
		m_status = qstatus_lock;
}

void qencoder_source::close()
{
	m_status = qstatus_idle;
}