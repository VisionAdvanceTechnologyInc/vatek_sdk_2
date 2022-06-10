#include <cross/cross_os_api.h>
#include <cross/cross_bridge.h>
#include "../../internal/libusb_tools.h"

#define BRIDGE_TAG                  "lbridge"

#define BRIDGE_PKTBUF_LEN           64
#define BRIDGE_TIMEOUT				1000
#define BRIDGE_BULK_WRITE_EP		0x01
#define BRIDGE_BULK_READ_EP			0x81

typedef struct _linux_bridge
{
    struct _linux_bridge* next;
    char name[32];
	struct libusb_device_handle* husb;
	struct libusb_transfer* tf_cmd;
	int32_t ref;
	hcross_mutex hlockcmd;
    uint8_t rawbuf_tx[BRIDGE_PKTBUF_LEN];
	uint8_t rawbuf_rx[BRIDGE_PKTBUF_LEN];
}linux_bridge,*Plinux_bridge;


extern vatek_result bridge_ll_write(Plinux_bridge pusb, uint8_t* pbuf, int32_t len);
extern vatek_result bridge_ll_read(Plinux_bridge pusb, uint8_t* pbuf, int32_t len);

extern uint32_t cross_os_buf_to_uint32(uint8_t* pbuf);

#if !__ANDROID__

vatek_result bridge_device_list_enum_default(hbridge_list* hblist)
{
    return bridge_device_list_enum_usb(USB_BRIDGE_VID,USB_BRIDGE_PID,hblist);
}

vatek_result bridge_device_list_enum_usb(uint16_t vid, uint16_t pid, hbridge_list* hblist)
{
	libusb_device **usbdevs;
	vatek_result nres = libusb_tool_init();
	size_t cnt = libusb_get_device_list(NULL, &usbdevs);

	if (is_vatek_success(nres) && cnt >= 0)
	{
		int32_t i = 0;
		Plinux_bridge proot = NULL;
		Plinux_bridge pnext = NULL;

		nres = vatek_success;
		for (i = 0; i < (int32_t)cnt; i++)
		{
			libusb_device* udevice = usbdevs[i];
			struct libusb_device_descriptor desc;
			int32_t r = libusb_get_device_descriptor(udevice, &desc);
			if (r >= 0)
			{
                if(vid == desc.idVendor && pid == desc.idProduct)
                {
                    struct libusb_device_handle* pdevhandle;
                    r = libusb_open(udevice, &pdevhandle);
                    if (r < 0)nres = vatek_hwfail;
                    else
                    {
                        Plinux_bridge newdevice = (Plinux_bridge)malloc(sizeof(linux_bridge));
                        if (newdevice == NULL) nres = vatek_memfail;
                        else
                        {
                            memset(newdevice, 0, sizeof(linux_bridge));
                            newdevice->husb = pdevhandle;
                            sprintf(&newdevice->name[0], 
                                    ".//%d-%d//%s", 
                                    libusb_get_bus_number(*usbdevs), 
                                    libusb_get_port_number(*usbdevs), 
                                    BRIDGE_TAG);

                            if (pnext == NULL)proot = newdevice;
                            else pnext->next = newdevice;
                            pnext = newdevice;
                            nres++;
                        }
                    }
                    if (!is_vatek_success(nres))break;
                }
			}
		}
		libusb_free_device_list(usbdevs, 1);
		if (is_vatek_success(nres))*hblist = proot;
	}
	return nres;
}

vatek_result bridge_device_list_free(hbridge_list hbridges)
{
    Plinux_bridge pusbs = (Plinux_bridge)hbridges;
	while (pusbs)
	{
		Plinux_bridge pnext = pusbs->next;
		libusb_close((libusb_device_handle*)pusbs->husb);
		free(pusbs);
		pusbs = pnext;
	}
	libusb_tool_free();
	return vatek_success;
}

vatek_result bridge_device_list_get(hbridge_list hblist, int32_t idx, hbridge_device* hbridge)
{
	Plinux_bridge proot = (Plinux_bridge)hblist;
	int32_t nums = 0;
	while (proot)
	{
		if (nums == idx)
		{
			*hbridge = proot;
			return vatek_success;
		}
		proot = proot->next;
	}
	return vatek_badparam;
}

const char* bridge_device_list_get_name(hbridge_list hblist, int32_t idx)
{
    hbridge_device husb = NULL;
	vatek_result nres = bridge_device_list_get(hblist, idx, &husb);
	if (is_vatek_success(nres))
	{
		return &((Plinux_bridge)husb)->name[0];
	}
	return NULL;
}

vatek_result bridge_device_open(hbridge_device hbridge)
{
    Plinux_bridge pusb = (Plinux_bridge)hbridge;
	vatek_result nres = vatek_badstatus;

	if (pusb->ref == 0)
	{
		hcross_mutex hlock = NULL;
		nres = cross_os_create_mutex(&hlock);
		if(is_vatek_success(nres))
		{
			libusb_device_handle* hdevice = (libusb_device_handle*)pusb->husb;
			libusb_set_auto_detach_kernel_driver(hdevice, 1);
			nres = (vatek_result)libusb_claim_interface(hdevice, 0);
			if (is_vatek_success(nres))
			{
				Phid_bridge_cmd pcmd = (Phid_bridge_cmd)&pusb->rawbuf_tx[0];
				memset(&pusb->rawbuf_tx[0],0,BRIDGE_PKTBUF_LEN);
				memset(&pusb->rawbuf_rx[0],0,BRIDGE_PKTBUF_LEN);
				memcpy(&pcmd->tag[0],&hid_bridge_tag[0],4);
				pusb->hlockcmd = hlock;
				pusb->ref++;
			}

			if(!is_vatek_success(nres))nres = vatek_hwfail;
		}
	}
	return nres;
}

#else

vatek_result bridge_device_open(hcross_ausb hausb,hbridge_device* hbridge)
{
	Plinux_bridge newdevice = (Plinux_bridge)malloc(sizeof(linux_bridge));
	vatek_result nres = vatek_memfail;
	if (newdevice)
	{
		Pandroid_usb pausb = (Pandroid_usb)hausb;
		memset(newdevice, 0, sizeof(linux_bridge));
		newdevice->husb = pausb->usb_handle;
		sprintf(&newdevice->name[0], 
				".//%d-%d//%s", 
				libusb_get_bus_number(pausb->usb_device), 
				libusb_get_port_number(pausb->usb_device), 
				BRIDGE_TAG);
		nres = vatek_success;
	}

	if(is_vatek_success(nres))
	{
		hcross_mutex hlock = NULL;
		nres = cross_os_create_mutex(&hlock);
		if(is_vatek_success(nres))
		{
			Phid_bridge_cmd pcmd = (Phid_bridge_cmd)&newdevice->rawbuf_tx[0];
			memset(&newdevice->rawbuf_tx[0],0,BRIDGE_PKTBUF_LEN);
			memset(&newdevice->rawbuf_rx[0],0,BRIDGE_PKTBUF_LEN);
			memcpy(&pcmd->tag[0],&hid_bridge_tag[0],4);
			newdevice->hlockcmd = hlock;
			newdevice->ref++;
			*hbridge = newdevice;
		}else free(newdevice);
		
	}
	return nres;
}

#endif

void bridge_device_lock_command(hbridge_device hbridge)
{
	Plinux_bridge pusbs = (Plinux_bridge)hbridge;
	cross_os_lock_mutex(pusbs->hlockcmd);
}

void bridge_device_unlock_command(hbridge_device hbridge)
{
	Plinux_bridge pusbs = (Plinux_bridge)hbridge;
	cross_os_release_mutex(pusbs->hlockcmd);
}

const char* bridge_device_get_name(hbridge_device hbridge)
{
    Plinux_bridge pusb = (Plinux_bridge)hbridge;
    return &pusb->name[0];
}

vatek_result bridge_device_close(hbridge_device hbridge)
{
	Plinux_bridge pusb = (Plinux_bridge)hbridge;
	if (pusb->ref == 1)
	{
		libusb_device_handle* hdevice = (libusb_device_handle*)pusb->husb;
		int32_t r = libusb_release_interface(hdevice, 0);
		pusb->ref = 0;
		cross_os_free_mutex(pusb->hlockcmd);
		pusb->hlockcmd = NULL;
		if (r < 0)return vatek_hwfail;
		return vatek_success;
	}
	return vatek_badstatus;
}

Phid_bridge_cmd bridge_device_get_command(hbridge_device hbridge)
{
	Plinux_bridge pusb = (Plinux_bridge)hbridge;
	return (Phid_bridge_cmd)&pusb->rawbuf_tx[0];
}

Phid_bridge_result bridge_device_get_result(hbridge_device hbridge)
{
    Plinux_bridge pusb = (Plinux_bridge)hbridge;
	return (Phid_bridge_result)&pusb->rawbuf_rx[0];
}

vatek_result bridge_device_send_bridge_command(hbridge_device hbridge)
{
	Plinux_bridge pusb = (Plinux_bridge)hbridge;
	vatek_result nres = bridge_ll_write(pusb, &pusb->rawbuf_tx[0],BRIDGE_PKTBUF_LEN);
	if (is_vatek_success(nres))
	{
		nres = bridge_ll_read(pusb, &pusb->rawbuf_rx[0],BRIDGE_PKTBUF_LEN);
		if (is_vatek_success(nres))
		{
			Phid_bridge_result presult = bridge_device_get_result(hbridge);
			presult->result = cross_os_buf_to_uint32((uint8_t*)&presult->result);
			presult->cmd = cross_os_buf_to_uint32((uint8_t*)&presult->cmd);

			if (strncmp((char*)&presult->tag[0], &hid_bridge_tag[0], 4) != 0)nres = vatek_badparam;
			else nres = (vatek_result)presult->result;
		}
	}
	return nres;
}

vatek_result bridge_ll_write(Plinux_bridge pusb, uint8_t* pbuf, int32_t len)
{
	if (pusb->ref <= 0)return vatek_badstatus;
	else if (len % BRIDGE_PKTBUF_LEN != 0)return vatek_badparam;
	else
	{
		libusb_device_handle* hdevice = (libusb_device_handle*)pusb->husb;
		int32_t rlen = 0;
		int32_t r = libusb_interrupt_transfer(hdevice, BRIDGE_BULK_WRITE_EP, pbuf, len, &rlen, BRIDGE_TIMEOUT);
		if (r >= 0)return (vatek_result)rlen;
	}
	return vatek_hwfail;
}

vatek_result bridge_ll_read(Plinux_bridge pusb, uint8_t* pbuf, int32_t len)
{
	if (pusb->ref <= 0)return vatek_badstatus;
	else if (len % BRIDGE_PKTBUF_LEN != 0)return vatek_badparam;
	else
	{
		libusb_device_handle* hdevice = (libusb_device_handle*)pusb->husb;
		int32_t rlen = 0;
		int32_t r = libusb_interrupt_transfer(hdevice, BRIDGE_BULK_READ_EP, pbuf, len, &rlen, BRIDGE_TIMEOUT);
		if (r >= 0)return (vatek_result)rlen;
	}
	return vatek_hwfail;
}

uint32_t cross_os_buf_to_uint32(uint8_t* pbuf)
{
    return (pbuf[0] << 24) | (pbuf[1] << 16) | (pbuf[2] << 8) | pbuf[3];
}
