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

#ifndef QSOURCE_BRIDGE_H
#define QSOURCE_BRIDGE_H

#include <core/qtv_source.h>
#include "../qinternal_properties.h"

/* bridge encoder implemented */

typedef enum _bridge_tag_device
{
	bsource_tag_ep9555e = bsource_ep9555e,
	bsource_tag_ep9351 = bsource_ep9351,
	bsource_tag_adv718x = bsource_adv718x,
	bsource_tag_h1 = bsource_h1,
	bdemod_tag_avl68xx = bdemod_avl68xx,
	brfmixer_tag_fintek_r2 = brfmixer_fintek_r2,
	brfmixer_tag_adrf6755 = brfmixer_adrf6755,
}bridge_tag_device;

typedef struct _bridge_encoder_param
{
	uint16_t pcrpid;
	uint16_t pmtpid;
	uint16_t video_pid;
	uint16_t audio_pid;
	video_codec vcodec;
	audio_codec acodec;
	audio_channel channel;
	bridge_tag_device source;
	uint32_t en_flags;		
	uint32_t manual_en_flags;		/*!< extend encoder function manual write */
	quality_param quality;
	uint32_t vi_flags;
	uint32_t manual_vi_flags;		/*!< extend vi function manual write */
	uint32_t mux_bitrate;

}bridge_encoder_param, * Pbridge_encoder_param;

class qbridge_avsource_property : public qi_property
{
public:
	explicit qbridge_avsource_property(const Pui_prop_item uiprop, uint8_t* raw, qtvBridge* bridge);
	bool checkSelectValid(const Pui_prop_val val) override;
protected:
	qtvBridge* m_bridge;
};

class qbridge_encoder_properties : public qi_properties_base
{
public:
	explicit qbridge_encoder_properties(qtvDevice* device);
	/* qtvPropertiesCollection */
	void resetProperties() override;
	bridge_encoder_param &_bridge_param() { return m_param; }

protected:
	qtvBridge* m_bridge;
	qtvDevice* m_device;
	bridge_encoder_param m_param;
	qtv_source_param m_param_vi;

	/* qi_properties_base */
	void refreshProperties() override;
	void insertProperty(const Pui_prop_item uiprop, uint8_t* raw) override;
};

class qbridge_encoder_factory : public qtvSourceFactory
{
public:

	qbridge_encoder_factory();
	~qbridge_encoder_factory();

	QString& _name() override { return m_name; }
	qtv_source_mode _mode() { return qsource_encoder; }

	bool checkSupported(qtvServiceBase* service) override;
	vatek_result createProperties(qtvServiceBase* service, qtvPropertyCollection** properties) override;
	vatek_result checkProperties(qtvPropertyCollection* properties) override;
	vatek_result createSource(qtvServiceBase* service, qtvPropertyCollection* properties, qtvSourceBase** source) override;

protected:
	QString m_name;
};

class qbridge_encoder_source : public qtvSourceBroadcast
{
public:
	explicit qbridge_encoder_source(qtvServiceBase* service, qbridge_encoder_properties* properties, qtvSourceFactory* factory);
	~qbridge_encoder_source();

	/* qtvSourceBase */
	qtv_source_mode _source_mode() override { return qsource_encoder; }
	qtv_status _status() override;
	qtvPropertyCollection* _properites() override { return m_properties; }
	Pqtv_source_param _param() override;
	const qtvSourceFactory* _factory() override { return m_factory; }

	vatek_result open() override;
	vatek_result start() override;
	vatek_result polling() override;
	void stop() override;
	void close() override;

protected:
	qtvDevice* m_device;
	qtvBridge* m_bridge;
	qtv_status m_status;
	int32_t m_index;
	qtvServiceBase* m_service;
	qbridge_encoder_properties* m_properties;
	qtvSourceFactory* m_factory;
	qtv_source_param m_param;
	Pbridge_source m_bsource;

	void resetParam(qtv_status qstatus);
	qtv_status checkSourceStatus();
};

#endif
