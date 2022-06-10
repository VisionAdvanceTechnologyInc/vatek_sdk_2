#include <stm32_usb_bridge.h>
#include <usbd_customhid.h>
#include <usbd_custom_hid_if.h>

#include "./inc/stm32_internal.h"

#define STM32_HID_PACKET_LEN        0x40
#define STM32_HID_INTERVAL_MS       0x01

#define BRIDGE_REPORT_DESC_SIZE     33
#define BRIDGE_DEVICE_DESC_SIZE     41

__ALIGN_BEGIN static uint8_t stm32_bridge_hid_report[BRIDGE_REPORT_DESC_SIZE] __ALIGN_END =
{
  /* USER CODE BEGIN 0 */
    0x05, 0x8c, /* USAGE_PAGE (ST Page) */
    0x09, 0x01, /* USAGE (Demo Kit) */
    0xa1, 0x01, /* COLLECTION (Application) */

    // The Input report
    0x09,0x03, // USAGE ID - Vendor defined
    0x15,0x00, // LOGICAL_MINIMUM (0)
    0x26,0x00, 0xFF, // LOGICAL_MAXIMUM (255)
    0x75,0x08, // REPORT_SIZE (8bit)
    0x95,0x40, // REPORT_COUNT (64Byte)
    0x81,0x02, // INPUT (Data,Var,Abs)

    // The Output report
    0x09,0x04, // USAGE ID - Vendor defined
    0x15,0x00, // LOGICAL_MINIMUM (0)
    0x26,0x00,0xFF, // LOGICAL_MAXIMUM (255)
    0x75,0x08, // REPORT_SIZE (8bit)
    0x95,0x40, // REPORT_COUNT (64Byte)
    0x91,0x02, // OUTPUT (Data,Var,Abs)
    /* USER CODE END 0 */
    0xC0    /*     END_COLLECTION	             */
};

extern vatek_result hooked_stm32_custom_hid_class(void);

extern vatek_result stm32_usb_set_recv(hstm32_usb hbridge,fpbridge_recv fprecv);
extern vatek_result stm32_usb_out_ready(hstm32_usb hbridge);
extern vatek_result stm32_usb_send(hstm32_usb hbridge,uint8_t* pbuf, int32_t len);
extern void stm32_usb_disable_pin(Pstm32_pin pin);

static Pstm32_hw_ip bridge_usb_ip = NULL;
static int32_t bridge_usb_start = 0;
static USBD_HandleTypeDef* bridge_usb_handle = NULL;

vatek_result stm32_hal_bridge_start(void)
{
    vatek_result nres = vatek_unsupport;
    if(bridge_usb_ip == NULL)_S_ERR("stm32 usb not support.");
    else
    {
        nres = vatek_badstatus;
        if(bridge_usb_start)_S_ERR("usb bridge alredy start.");
        else{
            nres = vatek_success;
            bridge_usb_ip->init();   
            bridge_usb_handle = (USBD_HandleTypeDef*)bridge_usb_ip->handle;
            bridge_usb_start = 1;
        }
    }
    return nres;
}

Pstm32_bridge_usb stm32_usb_bridge_get(void)
{
    static stm32_bridge_usb bridge_usb = 
    {
        .hbridge = NULL,
        .setrecv = stm32_usb_set_recv,
        .setready = stm32_usb_out_ready,
        .send = stm32_usb_send,
    };
    
    if(bridge_usb_ip == NULL)
    {
        vatek_result nres = vatek_unsupport;
        Pstm32_hw_ip phwip = stm32_core_get_hwip(stm32_ip_bridge_usb);
        if(phwip == NULL)_S_ERR("bridge usb not supported");
        else
        {
            nres = hooked_stm32_custom_hid_class();
            if(!is_vatek_success(nres))_S_ERR("modify bridge usb hid class fail : %d",nres);
            else 
            {
                bridge_usb_ip = phwip;
                bridge_usb.hbridge = phwip->handle;
                stm32_usb_disable_pin(&phwip->pins[STM32_USB_IO_DM]);
                stm32_usb_disable_pin(&phwip->pins[STM32_USB_IO_DP]);
                bridge_usb_start = 0;
            }
        }     
    }
    
    if(bridge_usb_ip == NULL)return NULL;
    else return &bridge_usb;
}

extern vatek_result usb_hid_modify_cfg_desc(void);
extern uint8_t hooked_hid_dataout(USBD_HandleTypeDef *pdev, uint8_t epnum);
extern uint8_t hooked_custom_hid_init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);

vatek_result hooked_stm32_custom_hid_class(void)
{
    vatek_result nres = vatek_badstatus;
    if(USBD_CUSTOM_HID_REPORT_DESC_SIZE != BRIDGE_REPORT_DESC_SIZE)_S_ERR("bad hid report desc len");
    else if(USB_CUSTOM_HID_CONFIG_DESC_SIZ != BRIDGE_DEVICE_DESC_SIZE)_S_ERR("bad device desc len");
    else if(USBD_CUSTOMHID_OUTREPORT_BUF_SIZE != STM32_HID_PACKET_LEN)_S_ERR("bad hid report buffer size");
    else 
    {
        nres = usb_hid_modify_cfg_desc();
        if(is_vatek_success(nres))
        {
            USBD_CustomHID_fops_FS.pReport = &stm32_bridge_hid_report[0];
            USBD_CUSTOM_HID.DataOut = hooked_hid_dataout;
            USBD_CUSTOM_HID.Init = hooked_custom_hid_init;
        }
    }
    return nres;
}


#if (__CM_CMSIS_VERSION_MAIN == 5)

extern vatek_result hid_cfg_desc_modified(uint8_t* pbuf,uint16_t len);

vatek_result usb_hid_modify_cfg_desc(void)
{
    uint16_t nlen = 0;
    uint8_t* pbuf = USBD_CUSTOM_HID.GetHSConfigDescriptor(&nlen);
    vatek_result nres = hid_cfg_desc_modified(pbuf,nlen);
    if(is_vatek_success(nres))
    {
        pbuf = USBD_CUSTOM_HID.GetFSConfigDescriptor(&nlen);
        nres = hid_cfg_desc_modified(pbuf,nlen);
    }
    return nres;
}

vatek_result hid_cfg_desc_modified(uint8_t* pbuf,uint16_t nlen)
{
    int32_t npos = 0;
    int32_t bfound = 0;
    vatek_result nres = vatek_success;
    
    while(npos < nlen)
    {
        uint8_t nlen = pbuf[npos];
        uint8_t ntype = pbuf[npos + 1];
        
        if(ntype == USB_DESC_TYPE_ENDPOINT)
        {
            pbuf[npos + 4] = STM32_HID_PACKET_LEN;
            pbuf[npos + 6] = STM32_HID_INTERVAL_MS;
            bfound++;
        }
        
        npos += nlen;
    }
    
    if(bfound != 2)
    {
        nres = vatek_badstatus;
        _S_ERR("usb endpoint desc fail : %d",bfound);
    }
    return nres;
}

uint8_t hooked_custom_hid_init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    uint8_t ret = 0U;
    USBD_CUSTOM_HID_HandleTypeDef *hhid;

    /* Open EP IN */
    USBD_LL_OpenEP(pdev, CUSTOM_HID_EPIN_ADDR, USBD_EP_TYPE_INTR,STM32_HID_PACKET_LEN);
    pdev->ep_in[CUSTOM_HID_EPIN_ADDR & 0xFU].is_used = 1U;
    /* Open EP OUT */
    USBD_LL_OpenEP(pdev, CUSTOM_HID_EPOUT_ADDR, USBD_EP_TYPE_INTR,STM32_HID_PACKET_LEN);
    pdev->ep_out[CUSTOM_HID_EPOUT_ADDR & 0xFU].is_used = 1U;
    pdev->pClassData = USBD_malloc(sizeof (USBD_CUSTOM_HID_HandleTypeDef));

    if(pdev->pClassData == NULL)ret = 1U;
    else
    {
        hhid = (USBD_CUSTOM_HID_HandleTypeDef*) pdev->pClassData;
        hhid->state = CUSTOM_HID_IDLE;
        ((USBD_CUSTOM_HID_ItfTypeDef *)pdev->pUserData)->Init();
        /* Prepare Out endpoint to receive 1st packet */
        USBD_LL_PrepareReceive(pdev, CUSTOM_HID_EPOUT_ADDR, hhid->Report_buf,USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
    }
    return ret;
}

#else

vatek_result usb_hid_modify_cfg_desc(void)
{
    uint16_t nlen = 0;
    int32_t npos = 0;
    int32_t bfound = 0;
    uint8_t* pbuf = USBD_CUSTOM_HID.GetHSConfigDescriptor(&nlen);
    vatek_result nres = hal_lite_success;
    
    while(npos < nlen)
    {
        uint8_t nlen = pbuf[npos];
        uint8_t ntype = pbuf[npos + 1];
        
        if(ntype == USB_DESC_TYPE_ENDPOINT)
        {
            pbuf[npos + 4] = STM32_HID_PACKET_LEN;
            pbuf[npos + 6] = STM32_HID_INTERVAL_MS;
            bfound++;
        }
        
        npos += nlen;
    }
    
    if(bfound != 2)
    {
        nres = vatek_badstatus;
        _S_ERR("usb endpoint desc fail : %d",bfound);
    }
    return nres;
}

uint8_t hooked_custom_hid_init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  uint8_t ret = 0;
  USBD_CUSTOM_HID_HandleTypeDef *hhid;
  /* Open EP IN */
  USBD_LL_OpenEP(pdev,CUSTOM_HID_EPIN_ADDR,USBD_EP_TYPE_INTR,STM32_HID_PACKET_LEN);  
  
  /* Open EP OUT */
  USBD_LL_OpenEP(pdev,CUSTOM_HID_EPOUT_ADDR,USBD_EP_TYPE_INTR,STM32_HID_PACKET_LEN);
  
  pdev->pClassData = USBD_malloc(sizeof (USBD_CUSTOM_HID_HandleTypeDef));
  
  if(pdev->pClassData == NULL){ret = 1; }
  else
  {
    hhid = (USBD_CUSTOM_HID_HandleTypeDef*) pdev->pClassData;
    hhid->state = CUSTOM_HID_IDLE;
    ((USBD_CUSTOM_HID_ItfTypeDef *)pdev->pUserData)->Init();
          /* Prepare Out endpoint to receive 1st packet */ 
    USBD_LL_PrepareReceive(pdev, CUSTOM_HID_EPOUT_ADDR, hhid->Report_buf, 
                           USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
  }
    
  return ret; 
}

#endif

static fpbridge_recv bridge_usb_handler = NULL;
static int32_t handle_locked = 0;
static USBD_CUSTOM_HID_HandleTypeDef *bridge_hid = NULL;

uint8_t hooked_hid_dataout(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
    bridge_hid = (USBD_CUSTOM_HID_HandleTypeDef*)pdev->pClassData;
    handle_locked = 1;
    if(bridge_usb_handler != NULL)
        bridge_usb_handler(&bridge_hid->Report_buf[0],USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
    handle_locked = 0;
    return USBD_OK;
}

vatek_result stm32_usb_set_recv(hstm32_usb hbridge,fpbridge_recv fprecv)
{
    vatek_result res = vatek_badstatus;
    uint32_t tick = HAL_GetTick();
    while(handle_locked && ((HAL_GetTick() - tick)< 1000));
    bridge_usb_handler = fprecv;
    
    if((HAL_GetTick() - tick) > 1000)_S_ERR("set recv handle timeout.");
    else res = vatek_success;
    return res;
}

vatek_result stm32_usb_out_ready(hstm32_usb hbridge)
{
    vatek_result nres = vatek_badstatus;
    if(!bridge_usb_start)_S_ERR("usb bridged not start.");
    else{
        if(bridge_usb_handle != NULL)
        {
            USBD_StatusTypeDef res = USBD_LL_PrepareReceive(bridge_usb_handle, 
                                                            CUSTOM_HID_EPOUT_ADDR ,
                                                            bridge_hid->Report_buf, 
                                                            USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
            if(res != USBD_OK)_S_ERR("prepare recv fail : %d",res);
            else nres = vatek_success;  
        }else _S_ERR("usb handle NULL"); 
    }
    return nres;
}

vatek_result stm32_usb_send(hstm32_usb hbridge,uint8_t* pbuf, int32_t len)
{
    vatek_result nres = vatek_badstatus;
    if(!bridge_usb_start)_S_ERR("usb bridged not start.");
    else
    {
       if(bridge_usb_handle != NULL)
        {
            uint8_t res = USBD_CUSTOM_HID_SendReport(bridge_usb_handle,pbuf,len);
            if(res == USBD_OK)nres = vatek_success;
            else _S_ERR("usb send report fail : %d",res);
        }else _S_ERR("usb handle NULL");     
    }


    return nres;
}

void stm32_usb_disable_pin(Pstm32_pin pin)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = pin->index;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    HAL_GPIO_WritePin(pin->port,pin->index,GPIO_PIN_RESET);    
}

