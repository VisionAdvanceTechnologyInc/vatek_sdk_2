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

#include "./internal/qinternal_media.h"

#define CODEC_PROPERTIES_MEMSIZE	1024

/* qtvCodec */

qtvCodec::qtvCodec(qtvCodecFactory* factory, QString name, qtv_codec_mode mode, qtvPropertyCollection* properties) :
	m_name(name),
	m_mode(mode),
	m_factory(factory),
	m_properties(properties)
{

}

/* qtvCodecVideo */

qtvCodecVideo::qtvCodecVideo(qtvCodecFactory* factory, QString name, qtvPropertyCollection* properties) :
	qtvCodec(factory, name, qcodec_video,properties)
{

}

/* qtvCodecAudio */

qtvCodecAudio::qtvCodecAudio(qtvCodecFactory* factory, QString name, qtvPropertyCollection* properties) :
	qtvCodec(factory, name, qcodec_audio, properties)
{

}

/* qtvCodecSimpleProvider */

qtvCodecSimpleProvider::qtvCodecSimpleProvider(qtv_codec_mode mode) :
	m_mode(mode)
{

}

qtvCodecSimpleProvider::~qtvCodecSimpleProvider()
{
	while (m_codecs.count())
	{
		delete m_codecs.first();
		m_codecs.removeFirst();
	}
}

/* qi_codec_properties */

qi_codec_properties::qi_codec_properties(qtvCodecProvider* provider) :
	m_codec(NULL),
	m_provider(provider)
{

}

void qi_codec_properties::resetProperties()
{

}

void qi_codec_properties::refreshProperties()
{

}

void qi_codec_properties::insertProperty(const Pui_prop_item uiprop, uint8_t* raw)
{
	qtvPropertyCollection::insertProperty(uiprop, raw);
}

void qi_codec_properties::raisePropertiesChanged()
{
	resetProperties();
	refreshProperties();
	emit notifyPropertiesChanged();
}

void qi_codec_properties::raisePropertyEdited(qtvProperty* prop)
{
	setValid(checkPropertiesValid());
}

qtvCodecProperties* qtvCodecProperties::createCodecProperties(qtvCodecProvider* provider)
{
	return new qi_codec_properties(provider);
}