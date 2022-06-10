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

#ifndef QTV_MEDIA_H
#define QTV_MEDIA_H

#include <core/qtv_define.h>
#include <core/qtv_property.h>

typedef enum _qtv_codec_mode
{
	qcodec_video,
	qcodec_audio,
}qtv_codec_mode;

class qtvCodecFactory;

class QTV_DECL qtvCodec : public QObject
{
	Q_OBJECT
public:
	explicit qtvCodec(qtvCodecFactory* factory, QString name, qtv_codec_mode mode,qtvPropertyCollection* properties);

	virtual QString& _name() { return m_name; }
	virtual qtv_codec_mode _mode() { return m_mode; }
	virtual qtvPropertyCollection* _properties() { return m_properties; }
	virtual void resetParam() = 0;
protected:
	QString m_name;
	qtv_codec_mode m_mode;
	qtvPropertyCollection* m_properties;
	qtvCodecFactory* m_factory;
};

class QTV_DECL qtvCodecVideo : public qtvCodec
{
	Q_OBJECT
public:
	explicit qtvCodecVideo(qtvCodecFactory* factory, QString name, qtvPropertyCollection* properties);
	virtual video_codec _codec_video_mode() = 0;
};

class QTV_DECL qtvCodecAudio : public qtvCodec
{
	Q_OBJECT
public:
	explicit qtvCodecAudio(qtvCodecFactory* factory, QString name, qtvPropertyCollection* properties);
	virtual audio_codec _codec_audio_mode() = 0;
};

class QTV_DECL qtvCodecFactory : public QObject
{
	Q_OBJECT
public:

	explicit qtvCodecFactory(const char* name, qtv_codec_mode mode) : m_name(name),m_mode(mode){}

	virtual QString& _name() { return m_name; }
	virtual qtv_codec_mode _mode() { return m_mode; }
	virtual vatek_result createCodec(qtvCodec** codec) = 0;
	virtual void freeCodec(qtvCodec* codec) = 0;

protected:
	QString m_name;
	qtv_codec_mode m_mode;
};

class QTV_DECL qtvCodecProvider : public QObject
{
	Q_OBJECT
public:
	virtual qtv_codec_mode _mode() = 0;
	virtual const QList<qtvCodecFactory*> getCodecs() = 0;
};

class QTV_DECL qtvCodecProperties : public qtvPropertyCollection
{
	Q_OBJECT
public:
	static qtvCodecProperties* createCodecProperties(qtvCodecProvider* provider);

	virtual qtvCodec* getCodec() = 0;
};

class QTV_DECL qtvCodecSimpleProvider : public qtvCodecProvider
{
public:
	explicit qtvCodecSimpleProvider(qtv_codec_mode mode);
	~qtvCodecSimpleProvider();

	qtv_codec_mode _mode() override { return m_mode; }
	const QList<qtvCodecFactory*> getCodecs() override { return m_codecs; }
	virtual void addCodec(qtvCodecFactory* factory) { m_codecs.append(factory); }

protected:
	qtv_codec_mode m_mode;
	QList<qtvCodecFactory*> m_codecs;
};

#endif
