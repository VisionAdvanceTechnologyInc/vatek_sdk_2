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

#ifndef QINTERNAL_CORE_H
#define QINTERNAL_CORE_H

#include <core/qtv_device.h>
#include <core/qtv_service.h>
#include <vatek_sdk_device.h>
#include <cross/cross_os_api.h>
#include <core/base/calibration_define.h>
#include <core/base/output_rfmixer.h>
#include "qinternal_properties.h"

class qi_bridge : public qtvBridge
{
public:
	explicit qi_bridge(hvatek_bridge hbridge);
	~qi_bridge();

	hvatek_bridge _handle() override { return m_hbridge; }
	Pbdevice_info _info() override { return m_binfo; }
	QList<Pbridge_source>& _sources() override { return m_sources; }

	vatek_result updateSource(int32_t idx, Pbridge_source psource) override;
	vatek_result checkSource(Pbridge_source psource) override;
	vatek_result startSource(Pbridge_source psource) override;
	vatek_result stopSource(Pbridge_source psource) override;

protected:
	hvatek_bridge m_hbridge;
	Pbdevice_info m_binfo;
	QList<Pbridge_source> m_sources;
};

class qi_device : public qtvDevice
{
public:
	~qi_device();

	hvatek_chip _handle() override { return m_hchip; }
	Pchip_info _chip_info() override { return vatek_device_get_info(m_hchip); }
	uint16_t _chip_bus() override { return vatek_device_get_bus(m_hchip); }
	hal_service_mode _chip_service() override { return _chip_info()->hal_service; }
	qtvBridge* _bridge() override { return m_bridge; }
	QString& _name() override { return m_name; };

	static vatek_result rebootDevice(qi_device* pdevice);
	static vatek_result createDevice(hvatek_chip hchip,qi_device** device);

protected:
	hvatek_chip m_hchip;
	qi_bridge* m_bridge;
	QString m_name;

	explicit qi_device(hvatek_chip hchip, qi_bridge* bridge);
};

class qi_devicemanage : public qtvDeviceManage
{
public:
	static qi_devicemanage& getInstance();

	const QList<qtvDevice*> _devices() { return m_devices; }
	vatek_result enumDevices(uint32_t bus, hal_service_mode mode);
	vatek_result openDevice(int32_t idx, qtvDevice** device);
	void closeDevice(qtvDevice* device, int32_t isreboot);

protected:
	hvatek_devices m_hdevices;
	QList<qtvDevice*> m_devices;
	explicit qi_devicemanage();
	~qi_devicemanage();
	void resetDevices();
};

class qi_servicehandle : public qtvServiceHandle
{
public:
	qi_servicehandle(hvatek_chip hchip);
	~qi_servicehandle();

	vatek_result start() override;
	vatek_result calibrationGet(Pcalibration_param pcalibration) override;
	vatek_result calibrationSet(Pcalibration_param pcalibration) override;
	vatek_result calibrationSave() override;
	uint32_t calibratiobCheck() override;
	vatek_result polling(Pbroadcast_info* pinfo) override;
	void stop() override;

protected:
	hvatek_chip m_hchip;
	hcross_thread m_hthread;
	hcross_mutex m_hlock;
	qtv_status m_status;
	uint32_t m_encalibration;
	uint32_t m_calibrationcount;
	calibration_param m_calibration;
	uint32_t m_updatetick;

	void internalLock();
	void internalUnlock();

	virtual vatek_result startService() = 0;
	virtual vatek_result pollingService() = 0;
	virtual void updateServiceTick() = 0;
	virtual void stopService() = 0;
	virtual Pbroadcast_info getServiceInfo() = 0;

private:
	static void service_handler(Pcross_thread_param param);
};

#endif
