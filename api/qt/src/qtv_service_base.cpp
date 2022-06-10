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

#include "./internal/qinternal_transform.h"
#include "./internal/qinternal_broadcast.h"
#include "./internal/qinternal_chart.h"

/* qtvServiceBase */

vatek_result qtvServiceBase::createService(qtvDevice* device, qtvServiceBase** service)
{
	vatek_result nres = vatek_unsupport;
	if (device->_chip_service() == service_broadcast)
	{
		hvatek_broadcast hbc = NULL;
		nres = vatek_broadcast_open(device->_handle(), &hbc);
		if (is_vatek_success(nres))*service = new qi_service_broadcast(device, hbc);
	}
	else if (device->_chip_service() == service_transform)
	{
		hvatek_transform htr = NULL;
		nres = vatek_transform_open(device->_handle(), &htr);
		if (is_vatek_success(nres))*service = new qi_service_transform(device, htr);
	}
	return nres;
}

qtvServiceBase::qtvServiceBase(qtv_service_mode mode, qtvDevice* device) :
	m_status(qstatus_idle),
	m_mode(mode),
	m_device(device),
	m_handle(NULL),
	m_chartdata(new qchartcapture_service())
{

}

qtvServiceBase::~qtvServiceBase()
{
	stopHandle();
	delete m_chartdata;
}

vatek_result qtvServiceBase::startHandle(qtvServiceHandle* handle)
{
	vatek_result nres = vatek_badstatus;
	if (m_status == qstatus_ready)
	{
		nres = handle->start();
		if (is_vatek_success(nres))
		{
			changedStatus(qstatus_running);
			m_handle = handle;
		}
	}
	return nres;
}

void qtvServiceBase::stopHandle()
{
	if (m_status == qstatus_running)
	{
		m_handle->stop();
		delete m_handle;
		changedStatus(qstatus_ready);
	}
	m_handle = NULL;
	m_status = qstatus_ready;
}

void qtvServiceBase::changedStatus(qtv_status status)
{
	if (m_status != status)
	{
		if (status == qstatus_running)m_chartdata->startCapture();
		else if (m_status == qstatus_running)m_chartdata->stopCapture();
		m_status = status;
		emit notifyStatusChanged(m_status);
	}
}
