#include <vatek_bridge_2.h>
#include <cmds/cmd_mod.h>
#include <cmds/cmd_bridge.h>
#include "./internal/bridge_internal.h"


static Pbridge_handle hbboard = NULL;

extern vatek_result bridge_hw_init(void);
extern vatek_result bridge_hw_reset(void);
extern void bridge_hw_close(void);

vatek_result vatek_bridge_open(hvatek_bridge* hbridge)
{
    vatek_result nres = vatek_badstatus;
    if(!hbboard)
    {
        nres = hal_board_open();
        if(is_vatek_success(nres))
            nres = bridge_hw_init();

        if(is_vatek_success(nres))
        {
            nres = vatek_memfail;
            hbboard = (Pbridge_handle)malloc(sizeof(bridge_handle));
            if(hbboard)
            {
                memset(hbboard,0,sizeof(bridge_handle));
                nres = bridge_source_get(hbboard);
                if(is_vatek_success(nres))
                {
                    Pbsource_list ptr = hbboard->source_root;
                    hbboard->info.bversion = BRIDGE_2_VERSION;
                    hbboard->info.source_nums = hbboard->source_count;
                    while(ptr)
                    {
                        hbboard->info.source_enable |= BBRIDGE_DRI_TO_BIT(ptr->device.driver->id);
                        ptr = ptr->next;
                    }
                }

                if(is_vatek_success(nres))
                {
                    *hbridge = hbboard;
                    nres = vatek_success;
                }else free(hbboard);
            }
            if(!is_vatek_success(nres))hal_board_close();
        }
        if(!is_vatek_success(nres))hbboard = NULL;
    }
    return nres;
}

Pbdevice_info vatek_bridge_get_info(hvatek_bridge hbridge)
{
    Pbridge_handle phboard = (Pbridge_handle)hbridge;
    return &phboard->info;
}

vatek_result vatek_bridge_start(hvatek_bridge hbridge)
{
    vatek_result nres = vatek_badstatus;
    Pbridge_handle phboard = (Pbridge_handle)hbridge;
    if(!phboard->hbridgeusb)
    {
		nres = hal_bridge_usb_open(&phboard->hbridgeusb);
    }
    return nres;
}

vatek_result vatek_bridge_polling(hvatek_bridge hbridge,chip_info info)
{
    vatek_result nres = vatek_badstatus;
    Pbridge_handle phboard = (Pbridge_handle)hbridge;
    phboard->chip_module = info.chip_module;
    uint32_t run_tick = 0;
    if(phboard->hbridgeusb)
    {

		Phid_bridge_cmd pcmd = NULL;
		bridge_device_status status = hal_bridge_usb_get_status(phboard->hbridgeusb,&pcmd);
		nres = vatek_success;
		if(status == hid_status_locked && pcmd)
		{
            Phid_bridge_result presult = hal_bridge_usb_get_result(phboard->hbridgeusb);
            uint32_t usbcmd = hal_bridge_get_uint32(pcmd->cmd);
            if(IS_MODULATOR_CMD(usbcmd))nres = cmd_mod(hbridge,pcmd, presult);
            else if(IS_BOARDBRIDGE_CMD(usbcmd))
			{
				if(IS_BRIDGE_SOURCE_CMD(usbcmd)){
					hal_system_sleep(100);
					nres = cmd_bridge_source(hbridge, pcmd,presult);
					if(nres != vatek_success){
						printf("cmd_bridge_source fail %d\r\n",nres);
					}
					//printf("cmd_bridge_source polling---1\r\n");
				}
				else if(IS_BRIDGE_DEVICE_CMD(usbcmd)){
                    nres = cmd_bridge_device(hbridge,pcmd,presult);
                    if(nres != vatek_success){
						printf("cmd_bridge_device fail %d\r\n",nres);
					}
                    //printf("cmd_bridge_device polling---2\r\n");
				}
				else if(IS_BRIDGE_DEMOD_CMD(usbcmd)){
                    nres = cmd_bridge_demod(hbridge,pcmd,presult);
                    if(nres != vatek_success){
						printf("cmd_bridge_demod fail %d\r\n",nres);
					}
                    //printf("cmd_bridge_demod polling---3\r\n");
				}
				else if(IS_BRIDGE_RFMIXER_CMD(usbcmd)){
                    nres = cmd_bridge_rfmixer(hbridge,pcmd,presult);
                    if(nres != vatek_success){
						printf("cmd_bridge_rfmixer fail %d\r\n",nres);
					}
                    //printf("cmd_bridge_rfmixer polling---4\r\n");

				}
                else nres = vatek_unsupport;

			}else nres = vatek_unsupport;

            if(!is_vatek_success(nres))
            {
                presult->result = hal_bridge_get_uint32(hal_bridge_get_result(nres));
                nres = vatek_success;
            }
            nres = hal_bridge_usb_commit(phboard->hbridgeusb);

		}
    }


//    printf("tick time = %d\r\n",vatek_get_tick_ms()-run_tick);
//    run_tick = vatek_get_tick_ms();

//    cmdline_read();
    return nres;
}

vatek_result vatek_bridge_stop(hvatek_bridge hbridge)
{
    vatek_result nres = vatek_badstatus;
    Pbridge_handle phboard = (Pbridge_handle)hbridge;
    if(phboard->hbridgeusb)
    {
		hal_bridge_usb_close(phboard->hbridgeusb);
		phboard->hbridgeusb = NULL;
		nres = vatek_success;
    }
    return nres;   
}

void vatek_bridge_close(hvatek_bridge hbridge)
{
    Pbridge_handle pbridge = (Pbridge_handle)hbridge;
	vatek_bridge_stop(hbridge);
    bridge_source_reset(pbridge);
    bridge_hw_close();
    hal_board_close();
    free(pbridge);
}

vatek_result vatek_chip_open(hvatek_bridge hbridge,hvatek_chip* hchip)
{
	#define MC_BASERAM_BASE             (0x5200 >> 2)      //128 MC base register address      
	vatek_result nres = vatek_badstatus;
	Pstm32_i2c pi2c = stm32_core_get_main_i2c();
	if(pi2c)
	{
		uint32_t val = 0;
		nres = vatek_chip_read_register((hvatek_chip)pi2c,MC_BASERAM_BASE,&val);
		if(is_vatek_success(nres))*hchip = pi2c;
	}
	return nres;
}

void vatek_chip_close(hvatek_chip hchip)
{
	
}

void vatek_source_get_first(hvatek_bridge hbridge,bpointer* pos)
{
    Pbridge_handle pbridge = (Pbridge_handle)hbridge;
    *pos = pbridge->source_root;
}

Pbdevice_source vatek_source_get_next(bpointer* pos)
{
    Pbsource_list plist = (Pbsource_list)*pos;
    *pos = plist->next;
    return &plist->device;
}

vatek_result bridge_hw_init(void)
{
    vatek_result nres = hal_gpio_config(HAL_IO_CHIP_RESET,hal_gpio_output);
    if(is_vatek_success(nres))
        nres = hal_gpio_config(HAL_IO_CHIP_RESCUE,hal_gpio_output);
    if(is_vatek_success(nres))
        nres = hal_gpio_config(HAL_IO_AV_RESET,hal_gpio_output);
    if(is_vatek_success(nres))
        nres = hal_gpio_config(HAL_IO_HDMI_NIM_RESET,hal_gpio_output);

    if(is_vatek_success(nres))
    {
        nres = hal_gpio_set(HAL_IO_CHIP_RESCUE,1);
        if(is_vatek_success(nres))
            nres = bridge_hw_reset();
    }
    return nres;
}

vatek_result bridge_hw_reset(void)
{
    vatek_result nres = hal_gpio_set(HAL_IO_CHIP_RESET,0);
    if(is_vatek_success(nres))
        nres = hal_gpio_set(HAL_IO_AV_RESET,0);
    if(is_vatek_success(nres))
        nres = hal_gpio_set(HAL_IO_HDMI_NIM_RESET,0);

    if(is_vatek_success(nres))
    {
        hal_system_sleep(100);
        nres = hal_gpio_set(HAL_IO_CHIP_RESET,1);
        if(is_vatek_success(nres))
            nres = hal_gpio_set(HAL_IO_AV_RESET,1);
        if(is_vatek_success(nres))
            nres = hal_gpio_set(HAL_IO_HDMI_NIM_RESET,1);
		
		if(is_vatek_success(nres))
			hal_system_sleep(5000);
    }
    return nres;
}

void bridge_hw_close()
{
    hal_gpio_set(HAL_IO_CHIP_RESCUE,0);
    hal_gpio_set(HAL_IO_AV_RESET,0);
    hal_gpio_set(HAL_IO_HDMI_NIM_RESET,0);
}
