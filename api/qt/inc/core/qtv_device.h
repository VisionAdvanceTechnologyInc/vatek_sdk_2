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

#ifndef QTV_DEVICE_H
#define QTV_DEVICE_H

#include <core/qtv_define.h>
#include <core/base/chip_define.h>
#include <core/base/calibration_define.h>
#include <vatek_sdk_bridge.h>

class QTV_DECL qtvBridge : public QObject
{
	Q_OBJECT
public:
	virtual hvatek_bridge _handle() = 0;
	virtual Pbdevice_info _info() = 0;
	virtual QList<Pbridge_source> &_sources() = 0;

	virtual vatek_result updateSource(int32_t idx, Pbridge_source psource) = 0;

	/* check current active source status */
	virtual vatek_result startSource(Pbridge_source psource) = 0;
	virtual vatek_result checkSource(Pbridge_source psource) = 0;
	virtual vatek_result stopSource(Pbridge_source psource) = 0;
};

class QTV_DECL qtvDevice : public QObject
{
	Q_OBJECT
public:
	virtual hvatek_chip _handle() = 0;
	virtual Pchip_info _chip_info() = 0;
	virtual uint16_t _chip_bus() = 0;
	virtual hal_service_mode _chip_service() = 0;
	virtual qtvBridge* _bridge() = 0;
	virtual QString& _name() = 0;

	static const char* getServiceName(qtvDevice* device);
};

class QTV_DECL qtvDeviceManage : public QObject
{
	Q_OBJECT
public:
	static vatek_result enumDevices(uint32_t bus, hal_service_mode mode);
	static vatek_result openDevice(int32_t idx, qtvDevice** device);
	static void closeDevice(qtvDevice* device, int32_t isreboot);
};

#endif
