#include "./internal/libusb_tools.h"

#include <cross/cross_usb_device.h>
#include <cross/cross_os_api.h>
#include <stdio.h>
#include <core/base/chip_define.h>

#define VATEK_USB_DEVICE_TAG        "vatek-usb"
#define VATEK_USB_RESCUE_TAG        "vatek-rescue"

static int usb_token = 0;

typedef struct _usb_handle
{
	struct _usb_handle* next;
	char name[32];
	struct libusb_device_handle* husb;
	struct libusb_device* usb_dev;
	hcross_mutex lock;
	int32_t ref;
	int32_t is_dma;
	int32_t epsize;
	int32_t bulksize;
	uint8_t* none_dmabuf;
	uint16_t vid;
	uint16_t pid;
	uint8_t device_address;
	uint8_t bus_number;
	int index;
}usb_handle, *Pusb_handle;

extern Pusbdevice_id usb_ll_list_get_id(uint16_t vid, uint16_t pid);
extern void usb_ll_convert_bufffer(uint8_t* psrc, uint8_t* pdest, int32_t len);
typedef int32_t(*fpenum_check)(struct libusb_device_descriptor* pdesc,usbdevice_type* type, uint32_t checkparam);
extern vatek_result usb_api_ll_enum_common(fpenum_check fpcheck, husb_device_list* hlist, uint32_t checkparam);
extern vatek_result usb_api_ll_enum_usb_multiple(fpenum_check fpcheck, husb_device_list* hlist, uint32_t checkparam, int index);

extern int32_t usb_enum_check_normal(struct libusb_device_descriptor* pdesc, usbdevice_type* type, uint32_t checkparam);
extern int32_t usb_enum_check_id(struct libusb_device_descriptor* pdesc, usbdevice_type* type, uint32_t checkparam);

vatek_result usb_api_ll_enum(usbdevice_type type, husb_device_list* hlist)
{
	return usb_api_ll_enum_common(usb_enum_check_normal, hlist, (uint32_t)type);
}

vatek_result usb_api_ll_usb_multiple(usbdevice_type type, husb_device_list* hlist, int index)
{
	return usb_api_ll_enum_usb_multiple(usb_enum_check_normal, hlist, (uint32_t)type, index);
}

vatek_result usb_api_ll_enum_by_id(uint16_t vid, uint16_t pid, husb_device_list* hlist)
{
	return usb_api_ll_enum_common(usb_enum_check_id, hlist, ((vid << 16) | pid));
}

vatek_result usb_api_ll_enum_usb_multiple(fpenum_check fpcheck, husb_device_list* hlist, uint32_t checkparam, int index)
{
	libusb_device** usbdevs;
	libusb_context* ctx = NULL;
	unsigned char string[256];

	int level = 1;
	int nums = 0;
	vatek_result nres = libusb_tool_init();
	libusb_set_debug(ctx, level);
	if (is_vatek_success(nres))
	{
		size_t cnt = libusb_get_device_list(NULL, &usbdevs);
		if (cnt >= 0)
		{
			int32_t i = 0;
			Pusb_handle proot = NULL;
			Pusb_handle pnext = NULL;
			int32_t enumnums = 0;

			for (i = 0; i < (int32_t)cnt; i++) {
				libusb_device* udevice = usbdevs[i];
				struct libusb_device_descriptor desc;
				nres = (vatek_result)libusb_get_device_descriptor(udevice, &desc);
				if (is_vatek_success(nres)) {
					usbdevice_type devtype = usb_type_unknown;
					if (fpcheck(&desc, &devtype, checkparam)) {
						Pusb_handle newdevice = (Pusb_handle)malloc(sizeof(usb_handle));
						if (newdevice != NULL)
						{
							const char* name = VATEK_USB_DEVICE_TAG;
							if (devtype == usb_type_rescure)name = VATEK_USB_RESCUE_TAG;
							memset(newdevice, 0, sizeof(usb_handle));
							newdevice->pid = desc.idProduct;
							newdevice->vid = desc.idVendor;
							newdevice->bus_number = libusb_get_bus_number(udevice);
							newdevice->device_address = libusb_get_device_address(udevice);
							newdevice->usb_dev = udevice;
							//newdevice->husb = pdevhandle[nums];
							//newdevice->lock = hlock;
							newdevice->is_dma = 0;
							newdevice->index = enumnums;
							newdevice->none_dmabuf = malloc(CHIP_STREAM_SLICE_LEN);
							newdevice->bulksize = libusb_get_max_packet_size(newdevice->usb_dev, USBDEV_BULK_WRITE_EP);
							sprintf(&newdevice->name[0], "Device[%d]//%d-%d//%s", enumnums, libusb_get_bus_number(newdevice->usb_dev), libusb_get_device_address(newdevice->usb_dev), name);
							if (index == vatek_alldevice)
								printf("%s\n", newdevice->name);

							if (proot == NULL) {
								proot = newdevice;
							}
							else {
								if (!proot->next)
									pnext = proot;
								while (pnext->next != NULL) {
									pnext = pnext->next;
								}
								pnext->next = newdevice;
							}
							enumnums++;
						}
						else nres = vatek_memfail;
					}
				}
			}
			//libusb_free_device_list(usbdevs, 1);
			if (is_vatek_success(nres))nres = (vatek_result)enumnums;
			else nres = vatek_hwfail;
			*hlist = proot;
		}
	}
	return nres;
}

vatek_result usb_api_ll_enum_common(fpenum_check fpcheck, husb_device_list* hlist, uint32_t checkparam)
{
	libusb_device** usbdevs;
	libusb_context* ctx = NULL;
	unsigned char string[256];

	int level = 1;
	int nums = 0;
	vatek_result nres = libusb_tool_init();
	libusb_set_debug(ctx, level);
	if (is_vatek_success(nres))
	{
		size_t cnt = libusb_get_device_list(NULL, &usbdevs);
		if (cnt >= 0)
		{
			int32_t i = 0;
			Pusb_handle proot = NULL;
			Pusb_handle pnext = NULL;
			int32_t enumnums = 0;

			for (i = 0; i < (int32_t)cnt; i++)
			{
				libusb_device* udevice = usbdevs[i];
				struct libusb_device_descriptor desc;
				nres = (vatek_result)libusb_get_device_descriptor(udevice, &desc);
				if (is_vatek_success(nres))
				{
					usbdevice_type devtype = usb_type_unknown;

					if (fpcheck(&desc, &devtype, checkparam))
					{
						hcross_mutex hlock = NULL;
						libusb_device_handle* pdevhandle[64] = { NULL };
						if (!usb_token) {
							if ((desc.idProduct == 0x1031) ||
								(desc.idProduct == 0x2021) ||
								(desc.idProduct == 0x2031) 
								)
							{
								if (!pdevhandle[nums])
									nres = (vatek_result)libusb_open(udevice, &pdevhandle[nums]);
								usb_token = 1;
								//nres = (vatek_result)libusb_set_configuration(pdevhandle[nums], 1);
							}
							if (!is_vatek_success(nres)){
								libusb_close(pdevhandle[nums]);
								pdevhandle[nums] = NULL;
							}

							// libusb_get_string_descriptor_ascii(pdevhandle[nums], desc.iProduct, string, sizeof(string));
							// if (usb_index == 0 && strcmp(string, "VA-DEVICE") == 0) {
							// 	printf("NAME : %s\n", (char*)string);
							// }
							// else if (usb_index == 1 && strcmp(string, "VAT-Device-103") == 0) {
							// 	printf("NAME : %s\n", (char*)string);
							// }
							// else {
							// 	libusb_close(pdevhandle[nums]);
							// 	pdevhandle[nums] = NULL;
							// }

							if (pdevhandle[nums]) {
								if (is_vatek_success(nres))nres = cross_os_create_mutex(&hlock);
								else {}

								if (is_vatek_success(nres))
								{
									Pusb_handle newdevice = (Pusb_handle)malloc(sizeof(usb_handle));
									if (newdevice != NULL)
									{
										const char* name = VATEK_USB_DEVICE_TAG;
										if (devtype == usb_type_rescure)name = VATEK_USB_RESCUE_TAG;
										memset(newdevice, 0, sizeof(usb_handle));
										newdevice->pid = desc.idProduct;
										newdevice->vid = desc.idVendor;
										newdevice->husb = pdevhandle[nums];
										newdevice->lock = hlock;
										newdevice->is_dma = 0;
										newdevice->none_dmabuf = malloc(CHIP_STREAM_SLICE_LEN);
										newdevice->bulksize = libusb_get_max_packet_size(udevice, USBDEV_BULK_WRITE_EP);
										sprintf(&newdevice->name[0], ".//%d-%d//%s", libusb_get_bus_number(udevice), libusb_get_port_number(udevice), name);


										if (pnext == NULL)proot = newdevice;
										else proot->next = newdevice;
										pnext = newdevice;
										enumnums++;
									}
									else nres = vatek_memfail;
									if (!is_vatek_success(nres))cross_os_free_mutex(hlock);
								}
							}
							nums++;
						}
						usb_token = 0;
					}
				}
			}
			libusb_free_device_list(usbdevs, 1);
			usb_token = 0;
			if (is_vatek_success(nres))nres = (vatek_result)enumnums;
			else nres = vatek_hwfail;
			*hlist = proot;
		}
	}
	return nres;
}

int32_t usb_enum_check_normal(struct libusb_device_descriptor* pdesc, usbdevice_type* type, uint32_t checkparam)
{
	usbdevice_type utype = (usbdevice_type)checkparam;
	Pusbdevice_id puid = usb_ll_list_get_id(pdesc->idVendor, pdesc->idProduct);
	if (puid && (puid->type == utype || utype == usb_type_all))
	{
		*type = puid->type;
		return 1;
	}
	return 0;
}

int32_t usb_enum_check_id(struct libusb_device_descriptor* pdesc, usbdevice_type* type, uint32_t checkparam)
{
	uint16_t vid = (checkparam >> 16) & 0xFFFF;
	uint16_t pid = (checkparam & 0xFFFF);

	if (pdesc->idVendor == vid && pdesc->idProduct == pid)
	{
		*type = usb_type_broadcast;
		return 1;
	}
	return 0;
}

vatek_result usb_api_ll_list_get_device(husb_device_list hlist, int32_t idx, husb_device* husb)
{
	Pusb_handle pusbs = (Pusb_handle)hlist;
	int32_t nums = 0;
	while (pusbs)
	{
		if (nums == idx)
		{
			*husb = pusbs;
			return vatek_success;
		}
		nums++;
		pusbs = pusbs->next;
	}
	return vatek_badparam;
}

const char* usb_api_ll_list_get_name(husb_device_list hlist, int32_t idx)
{
	husb_device husb = NULL;
	vatek_result nres = usb_api_ll_list_get_device(hlist, idx, &husb);
	if (is_vatek_success(nres))
	{
		return &((Pusb_handle)husb)->name[0];
	}
	return NULL;
}

vatek_result usb_api_ll_free_list(husb_device_list hlist)
{
	Pusb_handle pusbs = (Pusb_handle)hlist;
	while (pusbs)
	{
		Pusb_handle pnext = pusbs->next;
		libusb_close((libusb_device_handle*)pusbs->husb);
		cross_os_free_mutex(pusbs->lock);
		free(pusbs->none_dmabuf);
		free(pusbs);
		pusbs = pnext;
	}
	libusb_tool_free();
	return vatek_success;
}

vatek_result usb_api_ll_open(husb_device husb)
{
	Pusb_handle pusb = (Pusb_handle)husb;
	vatek_result nres = vatek_badstatus;
	if (pusb->ref == 0)
	{
		libusb_device_handle* hdevice = (libusb_device_handle*)pusb->husb;
		
		libusb_set_auto_detach_kernel_driver(hdevice, 1);
		nres = (vatek_result)libusb_set_configuration(hdevice, 1);
		if(is_vatek_success(nres))
			nres = (vatek_result)libusb_claim_interface(hdevice, 0);
		if (!is_vatek_success(nres))nres = vatek_hwfail;
		pusb->ref++;
	}
	return nres;
}

vatek_result usb_api_ll_open_usb(husb_device husb)
{
	Pusb_handle pusb = (Pusb_handle)husb;
	libusb_device_handle* pdevhandle = NULL;
	hcross_mutex hlock = NULL;
	libusb_device* hdevice_descr = pusb->usb_dev;
	vatek_result nres = vatek_badstatus;
	if (pusb->ref == 0)
	{
		if (!pdevhandle)
			nres = (vatek_result)libusb_open(hdevice_descr, &pdevhandle);

		if (pdevhandle) {
			if (is_vatek_success(nres))	nres = cross_os_create_mutex(&hlock);

			pusb->husb = pdevhandle;
			pusb->lock = hlock;

			libusb_device_handle* hdevice = (libusb_device_handle*)pusb->husb;

			libusb_set_auto_detach_kernel_driver(hdevice, 1);
			nres = (vatek_result)libusb_set_configuration(hdevice, 1);
			if (is_vatek_success(nres))
				nres = (vatek_result)libusb_claim_interface(hdevice, 0);
			if (!is_vatek_success(nres))nres = vatek_hwfail;
			pusb->ref++;
		}
	}
	return nres;
}

Pusbdevice_id usb_ll_list_get_id(uint16_t vid, uint16_t pid)
{
	int32_t pos = 0;

	while (usb_device_ids[pos].type != usb_type_unknown)
	{
		if (usb_device_ids[pos].vid == vid &&
			usb_device_ids[pos].pid == pid)
			return (Pusbdevice_id)&usb_device_ids[pos];
		pos++;
	}
	return NULL;
}

vatek_result usb_api_ll_close(husb_device husb)
{
	Pusb_handle pusb = (Pusb_handle)husb;
	if (pusb->ref == 1)
	{
		libusb_device_handle* hdevice = (libusb_device_handle*)pusb->husb;
		int32_t r = libusb_release_interface(hdevice, 0);
		pusb->ref = 0;
		if (r < 0)return vatek_hwfail;
		return vatek_success;
	}
	return vatek_badstatus;
}

const char* usb_api_ll_get_name(husb_device husb)
{
	Pusb_handle pusb = (Pusb_handle)husb;
	return &pusb->name[0];
}

vatek_result usb_api_ll_set_dma(husb_device husb, int32_t isdma)
{
	Pusb_handle pusb = (Pusb_handle)husb;
	vatek_result nres = vatek_success;
	
	if (isdma)
		nres = usb_api_ll_command(husb, VATCMD_CLASSV2_SET_MODE, CLASSV2_MODE_HWDMA, NULL);
	else nres = usb_api_ll_command(husb, VATCMD_CLASSV2_SET_MODE, CLASSV2_MODE_NORMAL, NULL);

	if(is_vatek_success(nres))
		pusb->is_dma = isdma;
	else pusb->is_dma = 0;
	return nres;
}

void usb_api_ll_lock(husb_device husb) {
	Pusb_handle pusb = (Pusb_handle)husb;
	cross_os_lock_mutex(pusb->lock);
}

void usb_api_ll_unlock(husb_device husb) {
	Pusb_handle pusb = (Pusb_handle)husb;
	cross_os_release_mutex(pusb->lock);
}
vatek_result usb_api_ll_write(husb_device husb, uint8_t* pbuf, int32_t len)
{
	Pusb_handle pusb = (Pusb_handle)husb;
	vatek_result nres = vatek_badparam;
	if (len <= CHIP_STREAM_SLICE_LEN && (len % 64) == 0)
	{
		libusb_device_handle* hdevice = (libusb_device_handle*)pusb->husb;
		int32_t rlen = 0;
		if (!pusb->is_dma)  // none_dma
		{
			usb_ll_convert_bufffer(pbuf, pusb->none_dmabuf,len);
			pbuf = pusb->none_dmabuf;
		}
		nres = (vatek_result)libusb_bulk_transfer(hdevice, USBDEV_BULK_WRITE_EP, pbuf, len, &rlen, USBDEV_TIMEOUT);
		if (is_vatek_success(nres))nres = (vatek_result)rlen;
	}
	return nres;
}

vatek_result usb_api_ll_read(husb_device husb, uint8_t* pbuf, int32_t len)
{
	Pusb_handle pusb = (Pusb_handle)husb;
	vatek_result nres = vatek_success;

	if (len > CHIP_STREAM_SLICE_LEN || (len % 64) != 0)nres = vatek_badparam;
	else
	{
		libusb_device_handle* hdevice = (libusb_device_handle*)pusb->husb;
		int32_t rlen = 0;
		nres = (vatek_result)libusb_bulk_transfer(hdevice, USBDEV_BULK_READ_EP, pbuf, len, &rlen, USBDEV_TIMEOUT);
		if (is_vatek_success(nres))nres = (vatek_result)rlen;
	}

	return nres;
}

vatek_result usb_api_ll_command(husb_device husb, uint8_t cmd, uint32_t param0, uint8_t* rxbuf)
{
	vatek_result nres = vatek_success;
	uint16_t wval = ((param0 >> 16) << 8) | ((param0 >> 24) & 0xFF);
	uint16_t widx = ((param0 & 0xFF) << 8) | ((param0 >> 8) & 0xFF);
	Pusb_handle pusb = (Pusb_handle)husb;
	libusb_device_handle* hdevice = (libusb_device_handle*)pusb->husb;

	if (rxbuf != NULL)
		nres = (vatek_result)libusb_control_transfer(hdevice, 0x80 | 0x40, cmd, wval, widx, rxbuf, 8, USBDEV_TIMEOUT);
	else nres = (vatek_result)libusb_control_transfer(hdevice, 0x40, cmd, wval, widx, NULL, 0, USBDEV_TIMEOUT);

	if (!is_vatek_success(nres))nres = vatek_hwfail;
	return nres;
}

vatek_result usb_api_ll_bulk_get_size(husb_device husb)
{
	Pusb_handle pusb = (Pusb_handle)husb;
	return pusb->bulksize;
}

vatek_result usb_api_ll_bulk_send_command(husb_device husb, Pusbbulk_command pcmd)
{
	Pusb_handle pusb = (Pusb_handle)husb;
	vatek_result nres = vatek_badstatus;
	if (!pusb->is_dma)
	{
		nres = usbbulk_command_set(pcmd, pusb->none_dmabuf);
		if (is_vatek_success(nres))
		{
			libusb_device_handle* hdevice = (libusb_device_handle*)pusb->husb;
			int32_t rlen = 0;
			uint8_t* prawbuf = &pusb->none_dmabuf[pusb->bulksize];
			usb_ll_convert_bufffer(pusb->none_dmabuf, prawbuf, pusb->bulksize);

			nres = (vatek_result)libusb_bulk_transfer(hdevice, USBDEV_BULK_WRITE_EP, prawbuf, pusb->bulksize, &rlen, USBDEV_TIMEOUT);
			if (is_vatek_success(nres))nres = (vatek_result)rlen;
		}
	}
	return nres;
}

vatek_result usb_api_ll_bulk_get_result(husb_device husb, Pusbbulk_result presult)
{
	Pusb_handle pusb = (Pusb_handle)husb;
	vatek_result nres = vatek_badstatus;
	if (!pusb->is_dma)
	{
		libusb_device_handle* hdevice = (libusb_device_handle*)pusb->husb;
		int32_t rlen = 0;
		uint8_t* prawbuf = &pusb->none_dmabuf[pusb->bulksize];
		nres = (vatek_result)libusb_bulk_transfer(hdevice, USBDEV_BULK_READ_EP, pusb->none_dmabuf, pusb->bulksize, &rlen, USBDEV_TIMEOUT);
		if (is_vatek_success(nres))
		{
			usb_ll_convert_bufffer(pusb->none_dmabuf, prawbuf, pusb->bulksize);
			nres = usbbulk_result_get(presult, prawbuf);
		}
	}
	return nres;
}

#define _align_bulk(p,n)	(((n + (p->bulksize - 1))/p->bulksize) * p->bulksize)

vatek_result usb_api_ll_bulk_write(husb_device husb, uint8_t* pbuf, int32_t len)
{
	Pusb_handle pusb = (Pusb_handle)husb;
	vatek_result nres = vatek_badstatus;
	if (!pusb->is_dma)
	{
		libusb_device_handle* hdevice = (libusb_device_handle*)pusb->husb;
		int32_t rlen = 0;
		uint8_t* ptrbuf = pbuf;
		uint8_t* prawbuf = pusb->none_dmabuf;
		int32_t pos = 0;
		while (len > pos)
		{
			int32_t nrecv = len - pos;
			int32_t neach = nrecv;
			if (neach >= CHIP_STREAM_SLICE_LEN)neach = CHIP_STREAM_SLICE_LEN;
			else neach = _align_bulk(pusb, neach);
			usb_ll_convert_bufffer(&pbuf[pos], prawbuf, nrecv);

			nres = (vatek_result)libusb_bulk_transfer(hdevice, USBDEV_BULK_WRITE_EP, prawbuf, neach, &rlen, USBDEV_TIMEOUT);
			if (!is_vatek_success(nres))
			{
				break;
			}
			else pos += neach;
		}
	}
	return nres;
}

vatek_result usb_api_ll_bulk_read(husb_device husb, uint8_t* pbuf, int32_t len)
{
	Pusb_handle pusb = (Pusb_handle)husb;
	vatek_result nres = vatek_badstatus;
	if (!pusb->is_dma)
	{
		libusb_device_handle* hdevice = (libusb_device_handle*)pusb->husb;
		int32_t rlen = 0;
		uint8_t* ptrbuf = pbuf;
		uint8_t* prawbuf = pusb->none_dmabuf;
		int32_t pos = 0;
		while (len > pos)
		{
			int32_t nrecv = len - pos;
			int32_t neach = nrecv;
			if (neach >= CHIP_STREAM_SLICE_LEN)neach = CHIP_STREAM_SLICE_LEN;
			else neach = _align_bulk(pusb, neach);
			nres = (vatek_result)libusb_bulk_transfer(hdevice, USBDEV_BULK_READ_EP, prawbuf, neach, &rlen, USBDEV_TIMEOUT);
			if (!is_vatek_success(nres))break;
			else
			{
				usb_ll_convert_bufffer(prawbuf, &pbuf[pos], nrecv);
				pos += neach;
			}
		}
	}
	return nres;
}

vatek_result usb_api_ll_command_buffer(husb_device husb, uint8_t cmd, uint8_t* pbuf, uint8_t* rxbuf)
{
	vatek_result nres = vatek_success;
	uint16_t wval = (pbuf[1] << 8) | pbuf[0];
	uint16_t widx = (pbuf[3] << 8) | pbuf[2];
	Pusb_handle pusb = (Pusb_handle)husb;
	libusb_device_handle* hdevice = (libusb_device_handle*)pusb->husb;


	if (rxbuf != NULL)
		nres = (vatek_result)libusb_control_transfer(hdevice, 0x80 | 0x40, cmd, wval, widx, rxbuf, 8, USBDEV_TIMEOUT);
	else nres = (vatek_result)libusb_control_transfer(hdevice, 0x40, cmd, wval, widx, NULL, 0, USBDEV_TIMEOUT);


	if (!is_vatek_success(nres))nres = vatek_hwfail;
	return nres;
}

void usb_ll_convert_bufffer(uint8_t* psrc, uint8_t* pdest, int32_t len)
{
	int32_t pos = 0;
	while (len > pos)
	{
		pdest[0] = psrc[3];
		pdest[1] = psrc[2];
		pdest[2] = psrc[1];
		pdest[3] = psrc[0];

		pos += 4;
		psrc += 4;
		pdest += 4;
	}
}
