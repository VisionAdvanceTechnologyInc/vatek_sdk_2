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

#ifndef ROM_DEVICE_H
#define ROM_DEVICE_H

#include <QObject>
#include <QThread>
#include <vatek_sdk_device.h>
#include <vatek_sdk_storage.h>

class storageDevice : public QObject
{
	Q_OBJECT
public:

	static storageDevice* openStorageImage(QString& szimg);
	static storageDevice* openStorageDevice(int32_t isenV0);
	static void closeStorageDevice(storageDevice* pdevice);

	vatek_result openStorage();
	vatek_result writeImage(Promfile_handle promfile);
	void closeStorage();
	QString& _information() { return m_szinfo; }
	int32_t _position() { return m_position; }
	hvatek_storage _hstorage() { return m_hstorage; }

Q_SIGNALS:
	void notifyStatusChanged();

protected:
	explicit storageDevice();
	~storageDevice();

	hvatek_storage m_hstorage;
	Pstorage_handle m_hshandle;
	int32_t m_position;
	QString m_szinfo;

	void updateStatus();

	static void storageHandler(rom_progress_msg msg, void* progressval, void* param);
};

#endif
