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

#ifndef QTV_DTV_H
#define QTV_DTV_H

#include <core/qtv_property.h>

class QTV_DECL qtvDTVChannel : public QObject
{
	Q_OBJECT
public:
	virtual const char* _name() = 0;
	virtual uint32_t _frequency() = 0;
};

class QTV_DECL qtvDTVLocale : public QObject
{
	Q_OBJECT
public:
	virtual const char* _name() = 0;
	virtual mux_spec_mode _spec() = 0;
	virtual mux_country_code _country() = 0;
	virtual uint32_t _bandwidth_symbolrate() = 0;
	virtual modulator_type _modulation() = 0;
	virtual video_codec _video() = 0;
	virtual audio_codec _audio() = 0;

	virtual const QList<qtvDTVChannel*> _channels() = 0;
};

class QTV_DECL qtvDTVProperties : public QObject
{
	Q_OBJECT
public:
	virtual Pqtv_locale_param _param() = 0;
	virtual void setChannel(qtvDTVChannel* channel) = 0;
	virtual void setLocale(qtvDTVLocale* locale) = 0;
	virtual qtvPropertyCollection* getPropertyCollection() = 0;
};

class QTV_DECL qtvDTVManage : public QObject
{
	Q_OBJECT
public:
	static const QList<qtvDTVLocale*> getDTVLocals();
	static qtvDTVProperties* createParam(qtvDTVLocale* locale, qtvDTVChannel* channel);
};

#endif
