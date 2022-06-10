#include "internal/ep9351_internal.h"

#define EP9351_CHECK_EDID           0
#define EP9351_EN_NOTHDMI			1

#define EP9351_WRITE_ADDR   (EP9351_ADDRESS)
#define EP9351_READ_ADDR    (EP9351_WRITE_ADDR | 1)

#define EP9351_GC_1_IO_DISABLE      0x0F
#define EP9351_SR_1_LINKED_ON       (STATUS_REGISTER_1_LINK_ON | STATUS_REGISTER_1_DE_VALID)
#define EP9351_GC_4_OUTPUT          (GENERAL_CONTROL_2_OUTFORMAT_422 | GENERAL_CONTROL_2_OUTCOLOR_YCC)

typedef struct _ep9351_handle
{
    ep9351_param param;
}ep9351_handle,*Pep9351_handle;

extern vatek_result ep9351_write_one(uint8_t reg,uint8_t val);
extern vatek_result ep9351_write(uint8_t reg,const uint8_t* pbuf,int32_t len);
extern vatek_result ep9351_read(uint8_t reg,uint8_t* pbuf,int32_t len);

extern vatek_result ep9351_source_valid(Pep9351_handle pep,bridge_status* status);
extern vatek_result ep9351_source_output_format(Pep9351_handle pep);

extern vatek_result ep9351_hotplug_in_config(Pep9351_handle pep);
extern vatek_result ep9351_hotplug_in_set(Pep9351_handle pep,int32_t status);

extern vatek_result ep9351_phy_start(Pep9351_handle pep,const uint8_t* edid);

vatek_result ep9351_check_support()
{
	#define EP9555E_SYS_ADDR					        0x68
	
    vatek_result nres = hal_i2c_check_device(EP9555E_SYS_ADDR);
	if(!is_vatek_success(nres))
		nres = hal_i2c_check_device(EP9351_ADDRESS);
	else nres = vatek_unsupport; //ep9555e
	return nres;
}

vatek_result ep9351_open(hbridge_source* hsource)
{
    Pep9351_handle newep = (Pep9351_handle)malloc(sizeof(ep9351_handle));
    vatek_result nres = vatek_memfail;
    if(newep)
	{
		memset(newep,0,sizeof(ep9351_handle));
		newep->param.baseclk = sbaseclk_1001;
		newep->param.pin_hp = -1;
		nres = ep9351_phy_start(newep,&HDMI_EDID_DEFAULT[0]);
	}
    return nres;
}

vatek_result ep9351_config(hbridge_source hsource,Pbavsource_param param)
{
	return vatek_unsupport;
}

vatek_result ep9351_get_status(hbridge_source hsource,Pbridge_source pbsourcesttaus)
{
    vatek_result nres = vatek_badstatus;
    Pep9351_handle pep = (Pep9351_handle)hsource;
    memset(pbsourcesttaus,0,sizeof(bridge_source));

    nres = ep9351_source_valid(pep,&pbsourcesttaus->status);
    if(is_vatek_success(nres))
    {
        if(pbsourcesttaus->status >= bstatus_active)
        {
            uint8_t vbuf[EP9XXX_VINFO_SIZE];
            #if !EP9351_EN_NOTHDMI
                uint8_t vcode = 0;
                nres = ep9351_read(EP9351_AVI_INFOFRAME,&vbuf[0],EP9XXX_VINFO_SIZE);
                if(is_hal_lite_success(nres))
                {
                    vcode = VINFO_VIDEOCODE(vbuf);
                    nres = ep9351_read(EP9351_TIMING_REGISTERS,&vbuf[0],EP9XXX_VTIME_SIZE);
                }
            #else
                nres = ep9351_read(EP9351_TIMING_REGISTERS,&vbuf[0],EP9XXX_VTIME_SIZE);
            #endif

            if(is_vatek_success(nres))
            {
                Phdmi_video_id pvideo = NULL;
                #if EP9351_EN_NOTHDMI
                    nres = ep9xxx_get_video_from_timing(&vbuf[0],0xFF,&pvideo);
                #else 
                    nres = ep9xxx_get_video_from_timing(&vbuf[0],vcode,&pvideo);
                #endif
                if(is_vatek_success(nres))
                {        
                    pbsourcesttaus->video_info.resolution = pvideo->resolution;
                    pbsourcesttaus->video_info.framerate = pvideo->framerate;
                    pbsourcesttaus->video_info.aspectrate = pvideo->aspectratio;
                    pbsourcesttaus->video_info.offset_x = 0;
                    pbsourcesttaus->video_info.offset_y = 0;
                    pbsourcesttaus->video_info.pixelclk = pvideo->pixelclk;
                    pbsourcesttaus->video_info.signal_flag = VI_SEPARATED_SYNC | VI_BUSWIDTH_16;
                    hdmi_tool_change_base_clock(pep->param.baseclk,&pbsourcesttaus->video_info);
                }
            }

            if(is_vatek_success(nres))
            {
                uint8_t val = 0;
                nres = ep9351_read(EP9351_GENERAL_CONTROL_4,&val,1);
                if(is_vatek_success(nres))
                {  
                    val &= 0x7;
                    pbsourcesttaus->audio_info.samplerate = ep9xxx_get_audio_samplerate(val);
                    if(pbsourcesttaus->audio_info.samplerate != sample_rate_unknown)
                        pbsourcesttaus->audio_info.channel = channel_stereo;
                    else pbsourcesttaus->audio_info.channel = channel_mute;
                }
                if(!is_vatek_success(nres))pbsourcesttaus->status = bstatus_invalid;
            }

            if(!is_vatek_success(nres))pbsourcesttaus->status = bstatus_invalid;
        }
    }
    return nres;
}

vatek_result ep9351_set_output(hbridge_source hsource,int32_t isoutput)
{
    vatek_result nres = vatek_success;
    if(!isoutput)nres = ep9351_write_one(EP9351_GENERAL_CONTROL_1,EP9351_GC_1_IO_DISABLE);
    else
    {
        bridge_status status = bstatus_idle;
        Pep9351_handle pep = (Pep9351_handle)hsource;
        nres = ep9351_source_valid(pep,&status);
        if(is_vatek_success(nres))
        {
            nres = vatek_badstatus;
            if(is_bstatus_active(status))
            {
                nres = ep9351_source_output_format(pep);
                if(is_vatek_success(nres))
                {
                    uint8_t val = 0;
                    uint8_t vcfg = 0;
                    nres = ep9351_read(EP9351_GENERAL_CONTROL_1,&val,1);
                    if(is_vatek_success(nres))
                    {
                        if((val & GENERAL_CONTROL_1_V_POL_RW) == 0)vcfg |= GENERAL_CONTROL_1_V_POL_RW;
                        if(val & GENERAL_CONTROL_1_H_POL_RW)vcfg |= GENERAL_CONTROL_1_H_POL_RW;
                        nres = ep9351_write_one(EP9351_GENERAL_CONTROL_1,vcfg);
                    }
                }
            }
        } 
    }
    
    if(!is_vatek_success(nres))
        _HAL_ERR(ep9351,"set output fail : %d - %d",nres,isoutput);
    return nres;
}

void ep9351_close(hbridge_source hsource)
{
    Pep9351_handle pep = (Pep9351_handle)hsource;
    free(pep);
}

vatek_result ep9351_phy_start(Pep9351_handle pep,const uint8_t* edid)
{
    #define GC_0_NORMAL         (GENEROL_CONTROL_0_DDC_DIS)
    #define GC_0_RESET          (GENEROL_CONTROL_0_HDCP_RST | GENEROL_CONTROL_0_DDC_DIS | GENEROL_CONTROL_0_RST)
    #define GC_0_POWERDOWN      (GC_0_NORMAL | GENEROL_CONTROL_0_PWR_DWN | GENEROL_CONTROL_0_DE_RST)
    
    vatek_result nres = ep9351_write_one(EP9351_GENERAL_CONTROL_0,GC_0_RESET);
    if(is_vatek_success(nres))nres = ep9351_write_one(EP9351_GENERAL_CONTROL_0,GC_0_NORMAL);
    if(is_vatek_success(nres))
    {
        nres = ep9351_write_one(EP9351_GENERAL_CONTROL_0,GC_0_POWERDOWN);
        if(is_vatek_success(nres))
        {
            nres = ep9351_write(EP9351_EDID_DATA_REGISTER,edid,HDMI_EDID_LEN);
            #if EP9351_CHECK_EDID
                if(is_vatek_success(nres))
                {
                    uint8_t checkedit[HDMI_EDID_LEN];
                    nres = ep9351_read(EP9351_EDID_DATA_REGISTER,&checkedit[0],HDMI_EDID_LEN);
                    if(is_vatek_success(nres))
                    {
                        int32_t i = 0;
                        for(i = 0;i < HDMI_EDID_LEN;i++)
                        {
                            if(edid[i] != checkedit[i])nres = hal_lite_hwfail;
                            if(!is_vatek_success(nres))break;
                            printf("%02x-%02x\r\n",edid[i],checkedit[i]);
                        }
                        if(!is_vatek_success(nres))_HAL_ERR(ep9351,"check edid fail : %d",i);
                    }else _HAL_ERR(ep9351,"read edid fail : %d",nres);
                }
            #endif
            if(is_vatek_success(nres))
            {
                nres = ep9351_write_one(EP9351_GENERAL_CONTROL_0,GENEROL_CONTROL_0_ON_CHIP_EDID);
                if(is_vatek_success(nres))ep9351_write_one(EP9351_GENERAL_CONTROL_4,GENERAL_CONTROL_4_LNK_ON_RST);
                if(is_vatek_success(nres))nres = ep9351_hotplug_in_set(pep,1);
                if(is_vatek_success(nres))nres = ep9351_set_output(pep,0);
            }
        }
        
        
        if(!is_vatek_success(nres))_HAL_ERR(ep9351,"download edid fail : %d",nres);
        else nres = ep9351_write_one(EP9351_GENERAL_CONTROL_0,GENEROL_CONTROL_0_ON_CHIP_EDID);
    }
    
    if(!is_vatek_success(nres))_HAL_ERR(ep9351,"reset chip fail : %d",nres);
    
    return nres;
}

vatek_result ep9351_source_valid(Pep9351_handle pep,bridge_status* status)
{
    uint8_t status0,status1;
    static uint32_t reset_invalid_source = 0;
    vatek_result nres = ep9351_read(EP9351_STATUS_REGISTER_1,&status1,1);
    if(is_vatek_success(nres))nres = ep9351_read(EP9351_STATUS_REGISTER_0,&status0,1);
    
    *status = bstatus_hw_fail;
    if(is_vatek_success(nres))
    {
        *status = bstatus_idle;
        if(status1 & STATUS_REGISTER_1_LINK_ON)
        {
            if((status1 & EP9351_SR_1_LINKED_ON) == EP9351_SR_1_LINKED_ON)
            {
                *status = bstatus_invalid;
				#if EP9351_EN_NOTHDMI
					if(status0 & STATUS_REGISTER_0_HDMI)_HAL_LOG(ep9351,"linked-on hdmi");
					else _HAL_LOG(ep9351,"linked-on not hdmi");
					*status = bstatus_active;
					reset_invalid_source = 0;
					nres = vatek_success;
				#else
					if(status0 & STATUS_REGISTER_0_HDMI)
					{
						*status = bstatus_active;
						reset_invalid_source = 0;
						nres = vatek_success;
					}
					else _HAL_LOG(ep9351,"linked but not hdmi");
				#endif
            }else _HAL_LOG(ep9351,"linked but invalid");
            
            if(*status == bstatus_invalid)
            {
                reset_invalid_source++;
                if(EP9351_INVALID_RESET_COUNT != 0 && 
                   reset_invalid_source > EP9351_INVALID_RESET_COUNT)
                {
                    _HAL_LOG(ep9351,"reset hot-plug ep9351");
                    ep9351_hotplug_in_set(pep,0);
                    hal_system_sleep(1000);
                    ep9351_hotplug_in_set(pep,1);
                    reset_invalid_source = 0;
                }
                
            }
        }else reset_invalid_source = 0;
    }
    return nres;
}

vatek_result ep9351_source_output_format(Pep9351_handle pep)
{
    uint8_t vinfo[EP9XXX_VINFO_SIZE];
    vatek_result nres = ep9351_read(EP9351_AVI_INFOFRAME,&vinfo[0],EP9XXX_VINFO_SIZE);
    if(is_vatek_success(nres))
    {
        uint8_t val = EP9351_GC_4_OUTPUT;
        uint8_t flag = VINFO_COLORSPACE(vinfo);
                    
        if(flag != Y_COLOR_RGB)
        {
            val |= GENERAL_CONTROL_2_INCOLOR_YCC;
            if(flag == Y_COLOR_YCC_422)val |= GENERAL_CONTROL_2_INFORMAT_422;
        }
                    
        flag = VINFO_PIXEL_REPEAT(vinfo) & 0xF;
        if(flag != 0)val |= ((flag - 1) & 0x3);
        flag = VINFO_COLORIMETRY(vinfo);
        if(flag == C_COLORSPACE_ITU709)val |= GENERAL_CONTROL_2_ITU709;
        else if(flag == C_COLORSPACE_EXT)
        {
            flag = VINFO_EX_COLORIMETRY(vinfo);
            if(flag == EC_COLORSPACE_CC709)val |= GENERAL_CONTROL_2_ITU709;
        }
                    
        flag = VINFO_QUANT_RANGE(vinfo);
        if(flag == Q_COLORRANGE_FULL)val |= GENERAL_CONTROL_2_FULLRANGE;         
        nres = ep9351_write_one(EP9351_GENERAL_CONTROL_2,val);
    }
    
    return nres;
}

vatek_result ep9351_hotplug_in_config(Pep9351_handle pep)
{
    vatek_result nres = vatek_success;
    if(pep->param.pin_hp != -1)
        nres = hal_gpio_config((hhal_pin)pep->param.pin_hp,hal_gpio_output);
    return nres;
}

vatek_result ep9351_hotplug_in_set(Pep9351_handle pep,int32_t status)
{
    vatek_result nres = vatek_success;
    if(pep->param.pin_hp != -1)
        nres = hal_gpio_set((hhal_pin)pep->param.pin_hp,status);
    return nres;
}

vatek_result ep9351_write_one(uint8_t reg,uint8_t val)
{
    uint8_t tmp = val;
    return ep9351_write(reg,&tmp,1);
}

vatek_result ep9351_write(uint8_t reg,const uint8_t* pbuf,int32_t len)
{
    vatek_result nres = hal_i2c_start(EP9351_WRITE_ADDR,0);
    if(is_vatek_success(nres))
    {
        nres = hal_i2c_write(&reg,1);
        if(is_vatek_success(nres))nres = hal_i2c_write(pbuf,len);
        if(is_vatek_success(nres))nres = hal_i2c_stop();
    }
    return nres;
}

vatek_result ep9351_read(uint8_t reg,uint8_t* pbuf,int32_t len)
{
    vatek_result nres = hal_i2c_start(EP9351_WRITE_ADDR,0);
    if(is_vatek_success(nres))
    {
        nres = hal_i2c_write(&reg,1);
        if(is_vatek_success(nres))nres = hal_i2c_stop();
    }
    
    if(is_vatek_success(nres))
    {
        nres = hal_i2c_start(EP9351_READ_ADDR,0);
        if(is_vatek_success(nres))nres = hal_i2c_read(pbuf,len);
        if(is_vatek_success(nres))nres = hal_i2c_stop();
    }
    return nres;
}
