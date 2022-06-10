#include "internal/ep9555e_internal.h"

#define EP9555E_CMD_DELAY   2
#define EP9555E_OUTFMT_DEFAULT      (EP9555E_MANUAL_VFMT_OUT422 | EP9555E_MANUAL_VFMT_OUT_YCC)

/* ep9555e read/write */
extern vatek_result ep9555e_write(uint8_t dev, uint16_t reg, uint8_t* pbuf, int32_t len);
extern vatek_result ep9555e_read(uint8_t dev, uint16_t reg, uint8_t* pbuf, int32_t len);

#define EP9555E_CHIP_WR(reg,buf,len)    ep9555e_write(EP9555E_CHIP_ADDR,reg,buf,len)
#define EP9555E_CHIP_RD(reg,buf,len)	ep9555e_read(EP9555E_CHIP_ADDR,reg,buf,len)

#define EP9555E_SYS_WR(reg,buf,len)     ep9555e_write(EP9555E_SYS_ADDR,reg,buf,len)
#define EP9555E_SYS_RD(reg,buf,len)     ep9555e_read(EP9555E_SYS_ADDR,reg,buf,len)

#define EP9555E_ISP_WR(reg,buf,len)     ep9555e_write(EP9555E_ISP_ADDR,reg,buf,len)
#define EP9555E_ISP_RD(reg,buf,len)     ep9555e_read(EP9555E_ISP_ADDR,reg,buf,len)

typedef struct _ep9555e_handle
{   
    ep9555e_param param;
}ep9555e_handle,*Pep9555e_handle;

extern vatek_result ep9555e_write_init(Pep9555e_handle pep);
extern vatek_result ep9555e_write_chip_uint16(uint16_t addr,uint16_t val);
extern vatek_result ep9555e_read_timing(uint8_t* pbuf);
/* ep9555e tool */
extern vatek_result ep9555e_write_output_format(Pep9555e_handle pep9555e);
extern vatek_result ep9555e_write_output_timing(Pep9555e_handle pep9555e);
extern vatek_result ep9555e_check_source_valid(bridge_status* status);
extern vatek_result ep9555e_get_current_resolution(Pep9555e_handle pep9555e,video_resolution* resolution);
extern vatek_result ep9555e_get_scale_timing(video_resolution resolution,video_framerate fps,Phdmi_video_timing ptiming);

vatek_result ep9555e_check_support(void)
{
    vatek_result nres = hal_i2c_check_device(EP9555E_SYS_ADDR);
	if(is_vatek_success(nres))
		nres = hal_i2c_check_device(EP9555E_CHIP_ADDR);
	return nres;
}

vatek_result ep9555e_open(hbridge_source* hsource)
{
    uint8_t val = 0;
    vatek_result nres = EP9555E_CHIP_RD(EP9555E_POWER_STATUS,&val,1);
    if(is_vatek_success(nres))
    {
    	Pep9555e_handle newep = (Pep9555e_handle)malloc(sizeof(ep9555e_handle));
    	nres = vatek_memfail;
    	if(newep)
    	{
            memset(newep,0,sizeof(ep9555e_handle));
            newep->param.scale = sscale_i_2_p;
            newep->param.baseclk = sbaseclk_1001;
            nres = ep9555e_write_init(newep);
            if(!is_vatek_success(nres))free(newep);
    		else *hsource = newep;
    	}
    }
	return nres;
}

vatek_result ep9555e_config(hbridge_source hsource,Pbavsource_param param)
{
	return vatek_unsupport;
}

vatek_result ep9555e_get_status(hbridge_source hsource,Pbridge_source pbsourcesttaus)
{
    vatek_result nres = vatek_success;
    Pep9555e_handle pep = (Pep9555e_handle)hsource;
    
    //memset(&pbsourcesttaus,0,sizeof(bridge_source));
    nres = ep9555e_check_source_valid(&pbsourcesttaus->status);
    if(is_vatek_success(nres))
    {   
        if(pbsourcesttaus->status >= bstatus_active)
        {
            uint8_t val = 0;
            Phdmi_video_id pvideo = NULL;
            nres = EP9555E_CHIP_RD(EP9555E_V_TIMING,&val,1);
            if(is_vatek_success(nres))
            {
                pvideo = hdmi_tool_get_video_id(val);
                nres = vatek_unsupport;
                if(pvideo == NULL)_HAL_LOG(ep9555e,"unsupported video code : %d",val);
                else 
                {
                    uint32_t signal = VI_SEPARATED_SYNC | VI_BUSWIDTH_16;
                    uint32_t pixelclk = pvideo->pixelclk;
                    video_resolution resolution = pvideo->resolution;
                    nres = ep9555e_get_current_resolution(pep,&resolution);
        
                    if(is_vatek_success(nres))
                    {
                        if(pep->param.scale == sscale_i_2_p)
                        {
                            if(pvideo->resolution == resolution_1080i)pixelclk <<= 1;
                            if(is_resolution_interlaced(pvideo->resolution))signal |= VI_EXT_HALF_FPS;
                        }else if(pep->param.scale == sscale_fixed_720p)
                        {
                            if(pvideo->resolution > resolution_720p)pixelclk = 74250;
                            if(is_resolution_interlaced(pvideo->resolution))signal |= VI_EXT_HALF_FPS;
                        }

                        if(is_vatek_success(nres))
                        {
                            pbsourcesttaus->video_info.resolution = resolution;
                            pbsourcesttaus->video_info.framerate = pvideo->framerate;  
                            pbsourcesttaus->video_info.pixelclk = pixelclk;        
                            hdmi_tool_change_base_clock(pep->param.baseclk,&pbsourcesttaus->video_info);
                            pbsourcesttaus->video_info.aspectrate = pvideo->aspectratio;
                            pbsourcesttaus->video_info.signal_flag = signal;
                            pbsourcesttaus->video_info.offset_x = 0;
                            pbsourcesttaus->video_info.offset_y = 0;   
                        }   
                    }
                }
            }

            if(is_vatek_success(nres))
            {
                uint8_t reg_status = 0;
                nres = EP9555E_CHIP_RD(EP9555E_SYS_STATUS1,&reg_status,1);
                if(is_vatek_success(nres))
                {
                    pbsourcesttaus->audio_info.channel = channel_mute;
                    if(is_ep9555e_source_valid(reg_status))
                    {
                        uint8_t val = 0;
                        nres = EP9555E_CHIP_RD(EP9555E_A_STATUS0,&val,1);
                        if(is_vatek_success(nres))
                        {
                            pbsourcesttaus->audio_info.samplerate = ep9xxx_get_audio_samplerate(val);
                            if(pbsourcesttaus->audio_info.samplerate == sample_rate_unknown)nres = vatek_unsupport;
                            else pbsourcesttaus->audio_info.channel = channel_stereo;
                        }
                    }
                }
            }
            if(!is_vatek_success(nres))pbsourcesttaus->status = bstatus_invalid;
        }
    }
    return nres;
}

vatek_result ep9555e_set_output(hbridge_source hsource,int32_t isoutput)
{
	#define EP9555E_EN_OUTPUT		(OUTPUT_DISABLE_CNTL_I2S_PCM)
	#define EP9555E_DISABLE_OUTPUT	(OUTPUT_DISABLE_CNTL_SPDIF_DIS | \
									 OUTPUT_DISABLE_CNTL_IIS_DIS | \
									 OUTPUT_DISABLE_CNTL_VOUT_DIS | \
									 OUTPUT_DISABLE_CNTL_I2S_PCM)

	uint8_t val = EP9555E_DISABLE_OUTPUT;
	vatek_result nres = vatek_success;
	if (isoutput)
    {
        Pep9555e_handle pep = (Pep9555e_handle)hsource;
        bridge_status bstatus = bstatus_idle;
        nres = ep9555e_check_source_valid(&bstatus);
        if(is_vatek_success(nres))
        {
            if(bstatus >= bstatus_active)
            {
                nres = ep9555e_write_output_format(pep);
                if(is_vatek_success(nres))
                    nres = ep9555e_write_output_timing(pep);
                val = EP9555E_EN_OUTPUT;      
            }else 
            {
                nres = vatek_badstatus;
                _HAL_LOG(ep9555e,"set output no valid source");
            }
        }
    }
    if(is_vatek_success(nres))
        nres = EP9555E_CHIP_WR(EP9555E_OUTPUT_DISABLE_CNTL,&val,1);      
	return nres;
}

void ep9555e_close(hbridge_source hsource)
{
    Pep9555e_handle pep = (Pep9555e_handle)hsource;
    free(pep);
}

vatek_result ep9555e_check_source_valid(bridge_status* status)
{
    uint8_t status0 = 0,status1 = 0;
    vatek_result nres = EP9555E_CHIP_RD(EP9555E_SYS_STATUS0,&status0,1);
    if(is_vatek_success(nres))
        nres = EP9555E_CHIP_RD(EP9555E_SYS_STATUS1,&status1,1);
    
    *status = bstatus_hw_fail;
    
    if(is_vatek_success(nres))
    {
        *status = bstatus_idle;
        if(status1 & EP9555E_SYS_STATUS1_LINK_ON)
        {
            *status = bstatus_invalid;
            if(status1 & EP9555E_SYS_STATUS1_DE_VALID)
            {
                if(status0 & EP9555E_SYS_STATUS0_HDMI)
                {
                    if((status0 & EP9555E_SYS_STATUS0_DECRYPT) &&
                        (status0 & EP9555E_SYS_STATUS0_AUTHEN))
                    {
                        //_HAL_LOG(ep9555e,"source is hdcp descypt");
                        *status = bstatus_active_protect;
                    }else *status = bstatus_active;
                }
                else _HAL_LOG(ep9555e,"linked on but not hdmi");
            }else _HAL_LOG(ep9555e,"linked on but de invalid");
        }
    }
    
    if(!is_vatek_success(nres))_HAL_ERR(ep9555e,"check source fail : %d",nres);
    return nres;
}

vatek_result ep9555e_write_output_format(Pep9555e_handle pep9555e)
{
    uint8_t val = 0;
    uint8_t infopr = 0;
    uint8_t ncfg = EP9555E_OUTFMT_DEFAULT;
    
    vatek_result nres = EP9555E_CHIP_RD(EP9555E_V_STATUS0,&val,1);
    if(is_vatek_success(nres))
        nres = EP9555E_CHIP_RD(EP9555E_AVI_INFO + 5,&infopr,1);
    
    if(is_vatek_success(nres))
    {
        bsource_scale outmode = pep9555e->param.scale;
        if(val & EP9555E_V_STATUS0_VIN_FMT_YCC)
        {
            ncfg |= EP9555E_MANUAL_VFMT_IN_YCC;
            if(val & EP9555E_V_STATUS0_VIN_FMT_422)
                ncfg |= EP9555E_MANUAL_VFMT_IN422;
        }
        
        if(val & EP9555E_V_STATUS0_VIN_FMT_Full_Range)
            ncfg |= EP9555E_MANUAL_VFMT_RGB_range;
        if(val & EP9555E_V_STATUS0_VIN_FMT_ITU709)
            ncfg |= EP9555E_MANUAL_VFMT_CS;
        
        if(outmode == sscale_bypass)ncfg |= (infopr & 0xF);
        nres = EP9555E_CHIP_WR(EP9555E_MANUAL_VFMT_Control,&ncfg,1);
    }
    
    if(!is_vatek_success(nres))
        _HAL_ERR(ep9555e,"write output format fail : %d",nres);
    return nres;
}

vatek_result ep9555e_write_output_timing(Pep9555e_handle pep9555e)
{
    uint8_t val = 0;
    Phdmi_video_id pvid = NULL;
    hdmi_video_timing vtime;
    uint8_t video_timing[EP9XXX_VTIME_SIZE];
    vatek_result nres = EP9555E_CHIP_RD(EP9555E_V_TIMING,&val,1);
    if(is_vatek_success(nres))
    {
        nres = vatek_unsupport;
        pvid = hdmi_tool_get_video_id(val);
        if(pvid == NULL)_HAL_LOG(ep9555e,"unsupport video code : %d",val);
        else
        {
            nres = ep9555e_read_timing(&video_timing[0]);
            if(is_vatek_success(nres))
                ep9xxx_get_video_timing(&video_timing[0],&vtime);
        }
    }
    
    if(is_vatek_success(nres))
    {
        if(pep9555e->param.scale != sscale_bypass)
        {
            video_resolution resolution = pvid->resolution;
            nres = ep9555e_get_current_resolution(pep9555e,&resolution);
            if(is_vatek_success(nres))
                nres = ep9555e_get_scale_timing(resolution,pvid->framerate,&vtime);
        }
    }
    
    if(is_vatek_success(nres))
    {
        nres = ep9555e_write_chip_uint16(EP9555E_SCALE_ACTIVE_H,vtime.pixel_per_line);
        if(is_vatek_success(nres))nres = ep9555e_write_chip_uint16(EP9555E_SCALE_ACTIVE_V,vtime.pixel_per_frame);
        if(is_vatek_success(nres))nres = ep9555e_write_chip_uint16(EP9555E_SCALE_H_DELAY,vtime.hfp);
        if(is_vatek_success(nres))nres = ep9555e_write_chip_uint16(EP9555E_SCALE_H_WIDTH,vtime.hpw);
        if(is_vatek_success(nres))nres = EP9555E_CHIP_WR(EP9555E_SCALE_V_DELAY,&vtime.vfp,1);
        if(is_vatek_success(nres))nres = EP9555E_CHIP_WR(EP9555E_SCALE_V_WIDTH,&vtime.vpw,1);
        if(!is_vatek_success(nres))_HAL_LOG(ep9555e,"write output timing fail : %d",nres);   
    }
    return nres;    
}

vatek_result ep9555e_write_init(Pep9555e_handle pep)
{
    typedef struct _ep9555e_reg
    {
        uint16_t addr;
        uint8_t val;
        uint8_t recv;
    }ep9555e_reg,*Pep9555e_reg;

    static const ep9555e_reg ep9555e_init_regs[] =
    {
        { EP9555E_INT_CNTL              ,0x00 },
        { EP9555E_POL_CNTL              ,0x2A },    /* VSYNC_LOW */
        { EP9555E_VOUT_CNTL             ,0x00 },
        { EP9555E_AOUT_CNTL             ,0x03 },
        { EP9555E_AOUT_CHMAP_CNTL       ,0x00 },
        { EP9555E_SYS_CNTL              ,0x10 },
        { EP9555E_MUTE_CNTL             ,0x08 },
        { EP9555E_OUTFMT_CNTL           ,0x49 },	//Disable AUTO video ouput format
        { EP9555E_MANUAL_VFMT_Control   ,0xA4 },	//YCC422 output, ITU-R BT709
        { EP9555E_OUTPUT_DISABLE_CNTL   ,0xD1 },    //SPDIF, IIS, DE, SYNC, DCLK, D0-2 in tri-state
    };
        
    #define INIT_REGS_NUM   (sizeof(ep9555e_init_regs)/sizeof(ep9555e_reg))

    int32_t i = 0;
    vatek_result nres = vatek_success;

    for(i = 0;i < INIT_REGS_NUM;i++)
    {
        uint8_t val = ep9555e_init_regs[i].val;
        nres = EP9555E_CHIP_WR(ep9555e_init_regs[i].addr,&val,1);
        if(!is_vatek_success(nres))break;	
    }
    return nres;
}

vatek_result ep9555e_get_current_resolution(Pep9555e_handle pep9555e,video_resolution* resolution)
{
    vatek_result nres = vatek_badparam;
    if(pep9555e->param.scale == sscale_i_2_p)
    {
        if(is_current_resolution(*resolution))
        {
            if(*resolution == resolution_1080i)*resolution = resolution_1080p;
            else if(*resolution == resolution_480i)*resolution = resolution_480p;
            else if(*resolution == resolution_576i)*resolution = resolution_576p;
        }
        nres = vatek_success;
    }else if(pep9555e->param.scale == sscale_fixed_720p)
    {
        if(*resolution > resolution_720p)
        {
            if(*resolution == resolution_480i)*resolution = resolution_480p;
            else if(*resolution == resolution_576i)*resolution = resolution_576p;            
        }else *resolution = resolution_720p;
        nres = vatek_success;
    }else if(pep9555e->param.scale != sscale_bypass)
        _HAL_ERR(ep9555e,"unhandle output mode : %d",pep9555e->param.scale);
    return nres;
}

vatek_result ep9555e_get_scale_timing(video_resolution resolution,video_framerate fps,Phdmi_video_timing ptiming)
{
    /* 
        follow CEA-861D Table 2 Video Format Timings—Detailed Timing Information 
        hblank = hfp + hfb + hfw 
        vblank = vfp + vfb + vfw
    */
    Phdmi_video_id pvid = hdmi_tool_get_video_id_by_info(resolution,fps,aspectrate_16_9);
    if(pvid == NULL)return vatek_badparam;
    else
    {
        hdmi_tool_get_timing(pvid,ptiming);
        return vatek_success;
    }
}

void swip_byte(uint8_t* pbuf)
{
    uint8_t tmp = pbuf[0];
    pbuf[0] = pbuf[1];
    pbuf[1] = tmp;
}

vatek_result ep9555e_read_timing(uint8_t* pbuf)
{
    #define EP9555E_SYNC_H_LEN  8
    #define EP9555E_SYNC_V_LEN  5
    
    vatek_result nres = EP9555E_CHIP_RD(EP9555E_SYNC_TIMING_H,&pbuf[0],EP9555E_SYNC_H_LEN);
    if(is_vatek_success(nres))nres = EP9555E_CHIP_RD(EP9555E_SYNC_TIMING_V,&pbuf[EP9555E_SYNC_H_LEN],EP9555E_SYNC_V_LEN);
    
    if(is_vatek_success(nres))
    {
        swip_byte(&pbuf[0]);
        swip_byte(&pbuf[2]);
        swip_byte(&pbuf[4]);
        swip_byte(&pbuf[6]);
        swip_byte(&pbuf[8]);
    }
    return nres;
}

vatek_result ep9555e_write(uint8_t dev, uint16_t reg, uint8_t* pbuf, int32_t len)
{
	uint8_t buf[2];
	vatek_result nres = hal_i2c_start(EP9555E_ADDR_W(dev),0);
	if (is_vatek_success(nres))
	{		
		buf[0] = (uint8_t)(reg >> 8);
		buf[1] = (uint8_t)reg;
		nres = hal_i2c_write(&buf[0], 2);
        if(is_vatek_success(nres))nres = hal_i2c_write(pbuf, len);
		if (is_vatek_success(nres))nres = hal_i2c_stop();
	}
	hal_system_sleep(EP9555E_CMD_DELAY);
    return nres;
}

vatek_result ep9555e_write_chip_uint16(uint16_t addr,uint16_t val)
{
    uint8_t buf = (uint8_t)(val >> 8);
    vatek_result nres = EP9555E_CHIP_WR(addr,&buf,1);
    if(is_vatek_success(nres))
    {
        addr++;
        buf = (uint8_t)val & 0xFF;
        nres = EP9555E_CHIP_WR(addr,&buf,1);
    }
    return nres;
}


vatek_result ep9555e_read(uint8_t dev, uint16_t reg, uint8_t* pbuf, int32_t len)
{
	uint8_t buf[2];
	vatek_result nres = vatek_success;

    buf[0] = (uint8_t)(reg >> 8);
    buf[1] = (uint8_t)reg;
    
    nres = hal_i2c_start(EP9555E_ADDR_W(dev),0);
	if (is_vatek_success(nres))nres = hal_i2c_write(&buf[0], 2);
    if (is_vatek_success(nres))nres = hal_i2c_stop();
    if (is_vatek_success(nres))
    {
        nres = hal_i2c_start(EP9555E_ADDR_R(dev),0);
        if (is_vatek_success(nres))
        {
            nres = hal_i2c_read(pbuf, len);
            if (is_vatek_success(nres))nres = hal_i2c_stop();
        }
    }
    return nres;
}
