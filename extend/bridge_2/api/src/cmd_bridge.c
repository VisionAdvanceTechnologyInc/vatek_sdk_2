#include <cmds/cmd_bridge.h>
#include "internal/bridge_internal.h"
#include <stdio.h>
#include <string.h>

extern Pbdevice_source cmdsource_get_source(hvatek_bridge hbridge,int32_t idx);
extern vatek_result cmdsource_map_bsource(Pbridge_source psource,uint8_t* buf);
extern vatek_result cmdsource_start(hvatek_bridge hbridge,bsource_id source);
extern vatek_result cmdsource_stop(hvatek_bridge hbridge);

static uint32_t tick_start,tick_stop,count_once=0;
vatek_result cmd_bridge_source(hvatek_bridge hbridge, Phid_bridge_cmd pcmd, Phid_bridge_result pres)
{
    vatek_result nres = vatek_unsupport;
    uint32_t cmd = hal_bridge_get_uint32(pcmd->cmd);
    uint16_t pin_val = 0;

    if(count_once == 0){
		tick_start = vatek_get_tick_ms();
		printf("start system time = 0x%08x\r\n",tick_start);
		count_once = 1;
    }
    //printf("cmd value = 0x%08x\r\n",cmd);
    //hal_system_sleep(100);

    if((cmd & 0xFFFFFF00) == BSOURCE_CMD_STATUS)
    {
        Pbdevice_source psource = cmdsource_get_source(hbridge,cmd & 0xFF);
        nres = vatek_badparam;
        if(psource)
        {
            bridge_source binfo;
            memset(&binfo,0,sizeof(bridge_source));
            nres = psource->driver->get_status(psource->hsource,&binfo);
            if(is_vatek_success(nres))
            {
            	binfo.source_id = (bsource_id)psource->driver->id;
            	nres = cmdsource_map_bsource(&binfo,&pres->data.raw[0]);
            }

        }
    }else if(cmd == BSOURCE_CMD_CNTL)
    {
        bridge_source_cmd cmd;
        nres = map_bridge_to_buffer(&pcmd->param.raw[0],(uint8_t*)&cmd,_bparam_struct_get(bridge_source_cmd));
        if(is_vatek_success(nres))
        {
            if(cmd.source_cntl == BSOURCE_CNTL_START)
            {
				if(cmd.source_id == bsource_h1){
					hal_system_sleep(100);
					nres = hal_gpio_config(HAL_IO_AUDIO_SWITCH,hal_gpio_output);
				}
				else if(cmd.source_id == bsource_ep9555e || cmd.source_id == bsource_ep9351) // set ep9555e
				{
					hal_system_sleep(100);
					nres = hal_gpio_config(HAL_IO_AUDIO_SWITCH,hal_gpio_output); //audio switch enable
				}
				nres = cmdsource_start(hbridge,cmd.source_id);
        	}
            else if(cmd.source_cntl == BSOURCE_CNTL_STOP)
            {
            	tick_stop = vatek_get_tick_ms();
            	printf("stop time = 0x%08x\r\n",tick_stop);
            	printf("bridge during time = 0x%08x\r\n",(tick_stop-tick_start));
            	nres = cmdsource_stop(hbridge);
            }
            else nres = vatek_badparam;
        }

    }
    else if(cmd == BSOURCE_CMD_GETACTIVE)
    {
        Pbridge_handle pbridge = (Pbridge_handle)hbridge;
        nres = vatek_badstatus;
        if(pbridge->source_active)
        {
            Pbdevice_source pdevice = pbridge->source_active;
            bridge_source binfo;
            memset(&binfo,0,sizeof(bridge_source));
            nres = pdevice->driver->get_status(pdevice->hsource,&binfo);
            if(is_vatek_success(nres))
            {
            	binfo.source_id = (bsource_id)pdevice->driver->id;
            	nres = cmdsource_map_bsource(&binfo,&pres->data.raw[0]);
            }


        }
    }else{
    	printf("cmd value = 0x%08x\r\n",cmd);
    }
    return nres;
}

vatek_result cmd_bridge_device(hvatek_bridge hbridge, Phid_bridge_cmd pcmd, Phid_bridge_result pres)
{
	vatek_result nres = vatek_success;
    Pbridge_handle pbridge = (Pbridge_handle)hbridge;
    uint32_t cmd =  hal_bridge_get_uint32(pcmd->cmd);
    Pbstorage_handle pstorage = &pbridge->storage;
	Pbflash_command pfcmd = &pstorage->cmd;
	
	if(cmd == BDEVICE_CMD_GET_INFO)
		nres = map_buffer_to_bridge(&pres->data.raw[0],(uint8_t*)&pbridge->info,_bparam_struct_get(bdevice_info));
	else if(cmd == BDEVICE_CMD_BFLASH_START)
	{
		bflash_command fcmd;
        nres = map_bridge_to_buffer(&pcmd->param.raw[0],(uint8_t*)&fcmd,_bparam_struct_get(bflash_command));
		if(is_vatek_success(nres))
			nres = bstorage_start(pbridge,&fcmd);
	}else if(cmd == BDEVICE_CMD_BFLASH_POS)
	{
		int32_t rlen = BSTORAGE_SECTION_LEN - pstorage->pos;
        nres = vatek_badparam;
		if(pfcmd->bcmd == BFLASH_CNTL_WRITE || pfcmd->bcmd == BFLASH_CNTL_READ)
		{
			if(rlen > BRIDGE_PARAM_MAX_LEN)rlen = BRIDGE_PARAM_MAX_LEN;
			if(pfcmd->bcmd == BFLASH_CNTL_WRITE)
				nres = bstorage_put(pbridge,&pcmd->param.raw[0],rlen);
			else 
			{
				uint8_t* ptrbuf = NULL;
				nres = bstorage_get(pbridge,&ptrbuf,rlen);
				if(is_vatek_success(nres)){
					memcpy(&pres->data.raw[0],ptrbuf,rlen);

				}
			}
		}
	}else if(cmd == BDEVICE_CMD_BFLASH_END)
	{
		nres = vatek_badparam;
		if(pfcmd->bcmd == BFLASH_CNTL_WRITE || pfcmd->bcmd == BFLASH_CNTL_READ)
			nres = bstorage_stop(pbridge);
	}
	else nres = vatek_unsupport;
    return nres;
}

vatek_result cmd_bridge_demod(hvatek_bridge hbridge, Phid_bridge_cmd pcmd, Phid_bridge_result pres)
{
    vatek_result nres = vatek_unsupport;
    uint32_t cmd = hal_bridge_get_uint32(pcmd->cmd);

    if(cmd == BDEMOD_CMD_GET_MODE)
    {

    }else if(cmd == BDEMOD_CMD_GETINFO)
    {

    }
    else if(cmd == BDEMOD_CMD_SET_MODE)
    {

    }
    else if(cmd == BDEMOD_CMD_START)
    {

    }
    else if(cmd == BDEMOD_CMD_STATUS)
    {

    }else if(cmd == BDEMOD_CMD_STOP)
    {

    }

    return nres;
}

vatek_result cmd_bridge_rfmixer(hvatek_bridge hbridge, Phid_bridge_cmd pcmd, Phid_bridge_result pres)
{
    vatek_result nres = vatek_unsupport;
    uint32_t cmd = hal_bridge_get_uint32(pcmd->cmd);
    return nres;
}

Pbdevice_source cmdsource_get_source(hvatek_bridge hbridge,int32_t idx)
{
    bpointer bptr = NULL;
    vatek_source_get_first(hbridge,&bptr);
    int32_t pos = 0;
    while(bptr)
    {
        Pbdevice_source pdevice = vatek_source_get_next(&bptr);
        if(pos == idx)return pdevice;
        pos++;
    }
    return NULL;
}

vatek_result cmdsource_start(hvatek_bridge hbridge,bsource_id source)
{
    Pbridge_handle pbridge = (Pbridge_handle)hbridge;
    vatek_result nres = vatek_badparam;
    if(!pbridge->source_active)
    {
        bpointer bptr = NULL;
        vatek_source_get_first(hbridge,&bptr);

        while(bptr)
        {
            Pbdevice_source pdevice = vatek_source_get_next(&bptr);
            if(pdevice->driver->id == source)
            {
                nres = pdevice->driver->set_open(pdevice->hsource, 1, pbridge->chip_module);
                if(is_vatek_success(nres))
                    pbridge->source_active = pdevice;
            }
            else nres = pdevice->driver->set_open(pdevice->hsource,0, pbridge->chip_module);
            if(!is_vatek_success(nres))break;
        }
    }
    return nres;
}

vatek_result cmdsource_stop(hvatek_bridge hbridge)
{
    Pbridge_handle pbridge = (Pbridge_handle)hbridge;
    vatek_result nres = vatek_badparam;
    if(pbridge->source_active)
    {
        Pbdevice_source pdevice = pbridge->source_active;
        nres = pdevice->driver->set_open(pdevice->hsource,0, 0);
        pbridge->source_active = NULL;
    }
    return nres;  
}

vatek_result cmdsource_map_bsource(Pbridge_source psource,uint8_t* buf)
{
    vatek_result nres = map_buffer_to_bridge(&buf[0],(uint8_t*)&psource->video_info,_bparam_struct_get(bridge_video_info));
    if(is_vatek_success(nres))
        nres = map_buffer_to_bridge(&buf[0],(uint8_t*)&psource->audio_info,_bparam_struct_get(bridge_audio_info));
    if(is_vatek_success(nres))
        nres = map_buffer_to_bridge(&buf[0],(uint8_t*)psource,_bparam_struct_get(bridge_source));
    return nres;
}
