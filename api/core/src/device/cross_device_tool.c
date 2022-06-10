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

#include "./internal/cross_device_tool.h"

typedef struct _cross_handle
{
	int32_t reference;
	Pcross_device root;
	Pcross_device last;
	hbridge_list bridges;
	husb_device_list usbdevices;
}cross_handle, * Pcross_handle;

static cross_handle m_cdevices = { 0,NULL,NULL,NULL,NULL, };

vatek_result cross_devices_create(Pcross_device* pcross)
{
	vatek_result nres = vatek_success;
	if (!m_cdevices.root)
	{
		int32_t i = 0;
		int32_t nums = 0;
		Pcross_device newcross = NULL;

		nres = bridge_device_list_enum_default(&m_cdevices.bridges);
		if (nres > vatek_success)
		{
			nums = nres;
			for (i = 0; i < nums; i++)
			{
				hbridge_device hbridge = NULL;
				nres = bridge_device_list_get(m_cdevices.bridges, i, &hbridge);
				if (is_vatek_success(nres))
				{
					nres = cross_bridge_open(hbridge, &newcross);
					if (is_vatek_success(nres))
					{
						if (!m_cdevices.root)m_cdevices.root = newcross;
						else m_cdevices.last->next = newcross;
						m_cdevices.last = newcross;
					}
				}
				if (!is_vatek_success(nres))VWAR("cross_devices_create - bridge fail [%d:%d]", i, nres);
			}
		}

		nres = usb_api_ll_enum(usb_type_all, &m_cdevices.usbdevices);
		if (nres > vatek_success)
		{
			nums = nres;
			for (i = 0; i < nums; i++)
			{
				husb_device husb = NULL;
				nres = usb_api_ll_list_get_device(m_cdevices.usbdevices, i, &husb);
				if (is_vatek_success(nres))
				{
					nres = cross_usb_device_open(husb, &newcross);
					if (is_vatek_success(nres))
					{
						if (!m_cdevices.root)m_cdevices.root = newcross;
						else m_cdevices.last->next = newcross;
						m_cdevices.last = newcross;
					}
				}
			}
		}
	}

	if (m_cdevices.root)
	{
		m_cdevices.reference++;
		*pcross = m_cdevices.root;
		nres = (vatek_result)cross_devices_get_size(m_cdevices.root);
	}
	else nres = vatek_success;
	return nres;
}

vatek_result cross_devices_create_by_usbid(uint16_t vid, uint16_t pid, Pcross_device* pcross)
{
	vatek_result nres = vatek_success;
	if (!m_cdevices.root)
	{
		m_cdevices.bridges = NULL;
		nres = usb_api_ll_enum_by_id(vid, pid, &m_cdevices.usbdevices);
		if (nres > vatek_success)
		{
			int32_t nums = nres;
			int32_t i = 0;
			Pcross_device newcross = NULL;
			for (i = 0; i < nums; i++)
			{
				husb_device husb = NULL;
				nres = usb_api_ll_list_get_device(m_cdevices.usbdevices, i, &husb);
				if (is_vatek_success(nres))
				{
					nres = cross_usb_device_open(husb, &newcross);
					if (is_vatek_success(nres))
					{
						if (!m_cdevices.root)m_cdevices.root = newcross;
						else m_cdevices.last->next = newcross;
						m_cdevices.last = newcross;
					}
				}
			}
		}
	}

	if (m_cdevices.root)
	{
		m_cdevices.reference++;
		*pcross = m_cdevices.root;
		nres = (vatek_result)cross_devices_get_size(m_cdevices.root);
	}
	else nres = vatek_success;
	return nres;
}

vatek_result cross_devices_free(Pcross_device pcross)
{
	vatek_result nres = vatek_badstatus;
	if (m_cdevices.reference)
	{
		m_cdevices.reference--;
		nres = vatek_success;
		if (!m_cdevices.reference)
		{
			Pcross_device ptrdev = m_cdevices.root;
			while (ptrdev)
			{
				Pcross_device pnext = ptrdev->next;
				if (ptrdev->driver == cdriver_bridge)
					cross_bridge_close(ptrdev);
				else if (ptrdev->driver == cdriver_usb)
					cross_usb_device_close(ptrdev);
				else VWAR("bad memory unknown cross device - %08x", ptrdev->driver);
				ptrdev = pnext;
			}

			if (m_cdevices.usbdevices)
				usb_api_ll_free_list(m_cdevices.usbdevices);
			if (m_cdevices.bridges)
				bridge_device_list_free(m_cdevices.bridges);
			m_cdevices.usbdevices = NULL;
			m_cdevices.bridges = NULL;
			m_cdevices.root = NULL;
			m_cdevices.last = NULL;
		}
	}
	else VWAR("cross_devices_free ref underflow");
	return nres;
}

vatek_result cross_devices_get_size(Pcross_device pcross)
{
	int32_t nums = 0;
	Pcross_device ptrdev = pcross;
	while (ptrdev)
	{
		nums++;
		ptrdev = ptrdev->next;
	}
	return (vatek_result)nums;
}

const char* cdevice_get_name(Pcross_device pcross)
{
	if (pcross->driver == cdriver_bridge)return bridge_device_get_name((hbridge_device)pcross->hcross);
	else if (pcross->driver == cdriver_usb)return usb_api_ll_get_name((husb_device)pcross->hcross);
	return "_unknown";
}

vatek_result cdevice_malloc(Pcross_device* pcross, hal_service_mode hal)
{
	vatek_result nres = vatek_unsupport;
	if (hal == service_rescue || hal == service_broadcast || hal == service_transform)
	{
		Pcross_device newdev = (Pcross_device)malloc(sizeof(cross_device));
		nres = vatek_memfail;
		if (newdev)
		{
			memset(newdev, 0, sizeof(cross_device));
			nres = vatek_success;
			*pcross = newdev;
		}
	}
	return nres;
}

void cdevice_free(Pcross_device pcross)
{
	free(pcross);
}
