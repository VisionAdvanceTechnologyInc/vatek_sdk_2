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

#include <storage_device.h>
#include <cross/cross_usb_device.h>
#include <QApplication>

/* storageChip */

class storageChip : public storageDevice
{
public:
	storageChip(hvatek_devices hdevices, hvatek_chip hchip) :
		m_hdevices(hdevices), 
		m_hchip(hchip) 
	{
	
	}

	~storageChip()
	{
		vatek_device_close_reboot(m_hchip);
		vatek_device_list_free(m_hdevices);
		m_hchip = NULL;
		m_hdevices = NULL;
	}
protected:
	hvatek_chip m_hchip;
	hvatek_devices m_hdevices;
};

/* stiorageImage */

class storageImage : public storageDevice
{
public:
};


/* storageDevice */

storageDevice::storageDevice() :
	m_hshandle(NULL),m_hstorage(NULL),m_position(0),m_szinfo("")
{

}

vatek_result storageDevice::openStorage()
{
	vatek_result nres = vatek_badstatus;
	if (m_hshandle && !m_hstorage)
	{
		nres = vatek_storage_open(m_hshandle, &m_hstorage,1);
	}
	return nres;
}

vatek_result storageDevice::writeImage(Promfile_handle promfile)
{
	vatek_result nres = vatek_badstatus;
	if (m_hshandle && m_hstorage)
	{
		nres = vatek_storage_write_image(m_hstorage, promfile);
	}
	return nres;
}

void storageDevice::closeStorage()
{
	if (m_hstorage)
		vatek_storage_close(m_hstorage);
	m_hstorage = NULL;
}

storageDevice::~storageDevice()
{
	if (m_hshandle)vatek_storage_free_handle(m_hshandle);
	m_hshandle = NULL;
}

storageDevice* storageDevice::openStorageImage(QString& szimg)
{
	QByteArray byteArray = szimg.toLocal8Bit();
	char* cimg = byteArray.data();
	storageImage* newimg = new storageImage();
	vatek_result nres = vatek_storage_open_file_handle(cimg, &newimg->m_hshandle, storageDevice::storageHandler, newimg);
	if (!is_vatek_success(nres))delete newimg;
	return newimg;
}

storageDevice* storageDevice::openStorageDevice(int32_t isenV0)
{
	int32_t i = 0;
	int32_t isfound = 0;
	hvatek_devices hdevices = NULL;
	hvatek_chip hchip = NULL;
	vatek_result nres = vatek_nodevice;

	for (i = 0; i < USBDEVICE_ID_NUMS; i++)
	{
		nres = vatek_device_list_enum_by_usbid(_usb_table[i].vid, _usb_table[i].pid, &hdevices);
		if (nres == vatek_success && isenV0)
			nres = vatek_device_list_enum_by_usbid(USBDEV_VID_OLD, _usb_table[i].pid, &hdevices);
		if (!is_vatek_success(nres))break;
		else if (nres > vatek_success)break;
	}

	if(nres == vatek_success)
		nres = vatek_device_list_enum(DEVICE_BUS_BRIDGE, service_unknown, &hdevices);

	if (nres > vatek_success)
	{
		hvatek_chip hchip = NULL;
		nres = vatek_device_open(hdevices, 0, &hchip);
		if (is_vatek_success(nres))
		{
			storageChip* newchip = new storageChip(hdevices, hchip);
			nres = vatek_storage_create_chip_handle(hchip, &newchip->m_hshandle, storageDevice::storageHandler, newchip);
			if (!is_vatek_success(nres))
			{
				delete newchip;
				hdevices = NULL;
				hchip = NULL;
			}
			else return newchip;
		}
	}
		

	if (!is_vatek_success(nres))
	{
		if (hchip)vatek_device_close(hchip);
		if (hdevices)vatek_device_list_free(hdevices);
	}
	return NULL;
}

void storageDevice::closeStorageDevice(storageDevice* pdevice)
{
	pdevice->closeStorage();
	delete pdevice;
}

void storageDevice::updateStatus()
{
	emit notifyStatusChanged();
	QApplication::processEvents();
}

void storageDevice::storageHandler(rom_progress_msg msg, void* progressval, void* param)
{
	storageDevice* sdevice = (storageDevice*)param;
	if (msg == rom_msg_set_pos)
		sdevice->m_position = (int32_t)(intptr_t)progressval;
	else sdevice->m_szinfo = (char*)progressval;
	sdevice->updateStatus();
}
