#include <hal/hal_bridge_usb.h>
#include <stm32_usb_bridge.h>

#define HALBRIDGE_CMDBUF_LEN				64
extern vatek_result stm32_bridge_recv_handler(uint8_t* pbuf, int32_t len);

typedef struct _stm32_bridge_handle
{
	int32_t recv_cmd;
	bridge_device_status status;
	Pstm32_bridge_usb pusb;
	uint8_t tx_buf[HALBRIDGE_CMDBUF_LEN];
	uint8_t rx_buf[HALBRIDGE_CMDBUF_LEN];
}stm32_bridge_handle,*Pstm32_bridge_handle;

static Pstm32_bridge_handle hstm32usb = NULL;

vatek_result hal_bridge_usb_open(hhal_bridge* hbridge)
{
	vatek_result nres = vatek_badstatus;
	if(!hstm32usb)
	{
		Pstm32_bridge_usb pstmusb = stm32_usb_bridge_get();
		nres = vatek_unsupport;
		
		if(pstmusb)
			nres = pstmusb->setrecv(pstmusb->hbridge,stm32_bridge_recv_handler);
		
		if(is_vatek_success(nres))
		{
			hstm32usb = (Pstm32_bridge_handle)malloc(sizeof(stm32_bridge_handle));
			nres = vatek_memfail;
			if(hstm32usb)
			{
				memset(hstm32usb,0,sizeof(stm32_bridge_handle));
				memcpy(&hstm32usb->tx_buf[0],&hid_bridge_tag[0],4);
				hstm32usb->pusb = pstmusb;
				hstm32usb->status = hid_status_idle;
				nres = vatek_success;
			}
		}
		
		if(is_vatek_success(nres))
		{
			nres = stm32_hal_bridge_start();
			if(is_vatek_success(nres))*hbridge = hstm32usb;
		}
	}
	return nres;

}

bridge_device_status hal_bridge_usb_get_status(hhal_bridge hbridge,Phid_bridge_cmd* pcmd)
{
	Pstm32_bridge_handle pbridge = (Pstm32_bridge_handle)hbridge;
	if(pcmd && pbridge->recv_cmd)
	{
		*pcmd = hal_bridge_usb_get_command(hbridge);
	}
	return pbridge->status;
}

Phid_bridge_cmd hal_bridge_usb_get_command(hhal_bridge hbridge)
{
	Pstm32_bridge_handle pbridge = (Pstm32_bridge_handle)hbridge;
	if(pbridge->recv_cmd)
		return (Phid_bridge_cmd)&pbridge->rx_buf[0];
	return NULL;
}

Phid_bridge_result hal_bridge_usb_get_result(hhal_bridge hbridge)
{
	Pstm32_bridge_handle pbridge = (Pstm32_bridge_handle)hbridge;
	if(pbridge->recv_cmd)
		return (Phid_bridge_result)&pbridge->tx_buf[0];
	return NULL;	
}

vatek_result hal_bridge_usb_commit(hhal_bridge hbridge)
{
	Pstm32_bridge_handle pbridge = (Pstm32_bridge_handle)hbridge;
	vatek_result nres = vatek_badstatus;
	if(pbridge->recv_cmd)
	{
		Pstm32_bridge_usb pusb = pbridge->pusb;
		nres = pusb->send(pusb->hbridge, &pbridge->tx_buf[0], HALBRIDGE_CMDBUF_LEN);
		if (is_vatek_success(nres))nres = pusb->setready(pusb->hbridge);
		pbridge->recv_cmd = 0;
	}
	return nres;
}

void hal_bridge_usb_close(hhal_bridge hbridge)
{
	
}

vatek_result stm32_bridge_recv_handler(uint8_t* pbuf, int32_t len)
{
	vatek_result res = vatek_badparam;
	if (len != HALBRIDGE_CMDBUF_LEN)return res;
	else if(!hstm32usb)res = vatek_badstatus;
	else
	{
		Phid_bridge_cmd pcmd = (Phid_bridge_cmd)&pbuf[0];
		Pstm32_bridge_usb pusb = hstm32usb->pusb;
		Phid_bridge_result presult = (Phid_bridge_result)&hstm32usb->tx_buf[0];
		uint32_t usbcmd = vatek_buffer_2_uint32((uint8_t*)&pcmd->cmd);
		if (strncmp((const char*)&pcmd->tag[0], hid_bridge_tag, 4) != 0)return res;
		else 
		{
            res = vatek_success;
			if (IS_USB_CMD(usbcmd))
			{
				if (usbcmd == USB_CMD_LOCK)hstm32usb->status = hid_status_locked;
				else if(usbcmd == USB_CMD_UNLOCK)hstm32usb->status = hid_status_idle;
				presult->result = res;
				presult->cmd = usbcmd;

				vatek_uint32_2_buffer((uint8_t*)&presult->result,(uint32_t)hstm32usb->status);
				res = pusb->send(pusb->hbridge, &hstm32usb->tx_buf[0], HALBRIDGE_CMDBUF_LEN);
				if (is_vatek_success(res))
					res = pusb->setready(pusb->hbridge);
			}
			else
			{
				memcpy(&hstm32usb->rx_buf[0], pcmd, sizeof(hid_bridge_cmd));
				hstm32usb->recv_cmd = 1;
			}
		}
	}

	return res;
}
