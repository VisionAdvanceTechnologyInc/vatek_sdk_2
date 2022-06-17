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

#ifndef QINTERNAL_TRANSFORM_H
#define QINTERNAL_TRANSFORM_H

#include <core/qtv_service.h>
#include "qinternal_properties.h"

#include <vatek_sdk_transform.h>
#include <vatek_sdk_usbstream.h>
#include <vatek_sdk_usbmux.h>

#include <cross/cross_os_api.h>

typedef struct _qi_transform_param
{
	transform_mode mode;
	//uint32_t frequency;
	r2_param r2param;
	union _param
	{
		transform_enum trenum;
		transform_capture trcapture;
		transform_broadcast trbc;
	}param;
}qi_transform_param,*Pqi_transform_param;

class qi_service_transform : public qtvServiceTransform
{
public:
	explicit qi_service_transform(qtvDevice* device, hvatek_transform htr);
	~qi_service_transform();

	/* qtvServiceBase */
	qtvPropertyCollection* _output_properties() override { return &m_output; }
	Pqtv_output_param _output_param() override { return m_output._param(); }
	Pbroadcast_info _broadcast_detail() override;

	/* qtvServiceTransform */
	vatek_result startEnumContext(qtvSourceBase* source, int32_t timeout) override;
	qtvMuxChannel* getEnumContext() override;
	/* qtvServiceChipBase */
	vatek_result setLocalDefault(qtvDTVProperties* locale) override;
	vatek_result startBroadcast(qtvSourceBase* source, qtvMuxServiceBase* muxservice) override;
	vatek_result pollingRunning(Pbroadcast_info* pinfo) override;
	vatek_result stop() override;

protected:
	qi_output_properties m_output;
	hvatek_transform m_htr;
	qi_transform_param m_param;
	qtvMuxChannel* m_enumcontext;

	void resetEnumContext();
	void resetParam();
	vatek_result configFilter(Ptransform_broadcast pbc, qtvMuxServiceBase* muxhandle);
	vatek_result setParamSource(qtvSourceBase* source);
};

class qi_transform_sources
{
public:
	explicit qi_transform_sources();
	~qi_transform_sources();

	QList<qtvSourceFactory*>& _sources() { return m_sources; }
	static qi_transform_sources& getInstance();

protected:
	QList<qtvSourceFactory*> m_sources;
};

#endif
