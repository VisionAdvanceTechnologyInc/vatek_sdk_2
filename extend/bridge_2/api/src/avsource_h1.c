#include "internal/h1_internal.h"
#include <stm32_define.h>

typedef struct _h1_handle
{
    h1_param param;
}h1_handle,*Ph1_handle;

extern vatek_result h1_write(uint16_t reg,uint8_t val);
extern vatek_result h1_write_uint16_t(uint16_t reg,uint16_t val);
extern vatek_result h1_read(uint16_t reg,uint8_t* val);
extern vatek_result h1_check_link_on(bridge_status* bstatus);
extern void h1_set_video_info(Phdmi_video_id phdmiinfo,Pbridge_source psource,bsource_scale output,int32_t is1001);
extern vatek_result h1_write_output_timing(Phdmi_video_timing ptiming,Phdmi_video_id pvid);
extern vatek_result h1_write_output(h1_param output,video_resolution vresolution);

static uint8_t reg_val = 0;
vatek_result h1_check_support()
{
    vatek_result nres = hal_i2c_check_device(H1_I2C_ADDR);
    if(is_vatek_success(nres))
    	printf("check H1 success\r\n");
    else
    	printf("check H1 fail %d\r\n",nres);
    return nres;
}

vatek_result h1_open(hbridge_source* hsource)
{
    Ph1_handle newdrv = (Ph1_handle)malloc(sizeof(h1_handle));
    vatek_result nres = vatek_memfail;
    nres = h1_write(H1_POWER,H1_POWER_ON);
    if(!is_vatek_success(nres)){
    	printf("H1 power on fail %d",nres);
    	return nres;
    }
    if(newdrv)
    {
        memset(newdrv,0,sizeof(h1_handle));
        newdrv->param.scale = sscale_bypass;
        newdrv->param.baseclk = sbaseclk_1001;
        *hsource = newdrv;
    }
    else
    	printf("open H1 device fail");
    return nres;
}

vatek_result h1_config(hbridge_source hsource,Pbavsource_param param)
{
    return vatek_unsupport;
}

vatek_result h1_get_video_info(hbridge_source hsource,Pbridge_source psource)
{
    Ph1_handle pdrv = (Ph1_handle)hsource;
    vatek_result nres = h1_check_link_on(&psource->status);
    bsource_scale output = pdrv->param.scale;
    bsource_baseclk baseclk = pdrv->param.baseclk;
    if(is_vatek_success(nres))
    {
        if(psource->status >= bstatus_active)
        {
            uint8_t vstatus = 0;
            uint8_t vhdmi = 0;
            nres = h1_read(H1_VIN_VIC,&vstatus);
            if(is_vatek_success(nres))
                nres = h1_read(H1_HDMI_FLAG,&vhdmi);
            if(is_vatek_success(nres))
            {
                Phdmi_video_id ptime = hdmi_tool_get_video_id(vstatus);
                if(ptime)h1_set_video_info(ptime,psource,output,(vhdmi & baseclk));
                else {
                    psource->status = bstatus_invalid;
                    _HAL_LOG(h1,"unknown video code : %d",vstatus);
                }
            }
        }
    }
    return nres;
}

vatek_result h1_set_output_mode(hbridge_source hsource,h1_param output)
{
    Ph1_handle pdrv = (Ph1_handle)hsource;
    pdrv->param = output;
    return vatek_success;
}

vatek_result h1_get_status(hbridge_source hsource,Pbridge_source pbsourcesttaus)
{
	uint32_t tick_error = 0;
    vatek_result nres = vatek_success;
    Ph1_handle pdrv = (Ph1_handle)hsource;
    bsource_scale output = pdrv->param.scale;
    bsource_baseclk baseclk = pdrv->param.baseclk;

    memset(pbsourcesttaus,0,sizeof(bridge_source));

    nres = h1_check_link_on(&pbsourcesttaus->status);
    if(!is_vatek_success(nres))
    	printf("h1_check_link_on fail %d\r\n",nres);
    if(is_vatek_success(nres))
    {
        if(pbsourcesttaus->status >= bstatus_active) //remove so it can pass to next step, and can see the parameter on interface 220110
        {
            uint8_t vstatus = 0;
            uint8_t vhdmi = 0;
            nres = h1_read(H1_VIN_VIC,&vstatus);
            if(is_vatek_success(nres))
                nres = h1_read(H1_HDMI_FLAG,&vhdmi);
            if(is_vatek_success(nres))
            {
                Phdmi_video_id ptime = hdmi_tool_get_video_id(vstatus);
                h1_set_video_info(ptime,pbsourcesttaus,output,(vhdmi & baseclk));
//                if(ptime == NULL)
//                {
//                	hal_system_sleep(500);
//                	ptime = hdmi_tool_get_video_id(vstatus);
//                	printf("lock again ptime value = 0x%08x\r\n",ptime);
//                	if(ptime == NULL){
//                		hal_system_sleep(500);
//                		ptime = hdmi_tool_get_video_id(vstatus);
//                		pbsourcesttaus->status = bstatus_invalid;
//                		printf("pbsource status change value to invalid\r\n");
//                		printf("status change time = 0x%08x\r\n",vatek_get_tick_ms());
//                	}
//                    _HAL_LOG(h1,"unknown video code : %d",vstatus);
//                }
//                else h1_set_video_info(ptime,pbsourcesttaus,output,(vhdmi & baseclk));
            }
            if(is_vatek_success(nres))
            	h1_get_audio_info(hsource, pbsourcesttaus);
        }
        else{
        	nres = h1_check_link_on(&pbsourcesttaus->status);
        	if(nres != vatek_success){
				tick_error = vatek_get_tick_ms();
				printf("error occur system clock = 0x%08x\r\n",tick_error);
				hal_system_sleep(10);
				printf("H1 status(0=idle,-1=hw_fail,-2=invalid) = %d\r\n",pbsourcesttaus->status);
        	}
        }
        return nres;
    }
    return vatek_unsupport;
}

vatek_result h1_get_audio_info(hbridge_source hsource,Pbridge_source psource)
{
    #define AUDIO_SAMPLE_FREQ_32KHZ		0x00
	#define AUDIO_SAMPLE_FREQ_44_1KHZ	0x01
	#define AUDIO_SAMPLE_FREQ_48KHZ		0x02

    Ph1_handle pdrv = (Ph1_handle)hsource;
    vatek_result nres = h1_check_link_on(&psource->status);
    if(is_vatek_success(nres))
    {
        if(psource->status >= bstatus_active)
        {
            uint8_t val = 0;
            nres = h1_read(H1_AIN_STATUS,&val);
            if(is_vatek_success(nres))
            {
                val &= H1_AIN_STATUS_SAMP_FREQ;
                if(val == AUDIO_SAMPLE_FREQ_32KHZ)psource->audio_info.samplerate = sample_rate_32;
                else if(val == AUDIO_SAMPLE_FREQ_44_1KHZ)psource->audio_info.samplerate = sample_rate_44_1;
                else psource->audio_info.samplerate = sample_rate_48;
                psource->audio_info.channel = channel_stereo;
            }
        }
        else nres = vatek_busy;
    }
    return nres;
}

vatek_result h1_set_output(hbridge_source hsource, int32_t isoutput)//, Pbridge_source psource, remove psource to avoid address overwrite
{
    vatek_result nres = vatek_success;
    Ph1_handle pdrv = (Ph1_handle)hsource;
    if(isoutput)
    {
    	bridge_status status = bstatus_idle;
    	nres = h1_check_link_on(&status);
    	if(!is_vatek_success(nres))printf("h1_check_link_on fail %d\r\n",nres);
    	if(is_vatek_success(nres))
    	{
    		if(status >= bstatus_active) //remove so it can pass to next step, and can see the parameter on interface 220110
    	    {
    			uint8_t vstatus = 0;
    			uint8_t vhdmi = 0;
    	        nres = h1_read(H1_VIN_VIC,&vstatus);
    	        if(is_vatek_success(nres))
    	        	nres = h1_read(H1_HDMI_FLAG,&vhdmi);

				if(is_vatek_success(nres))
				{
					Phdmi_video_id pvid = hdmi_tool_get_video_id(vstatus);
					if(pvid == NULL)nres = vatek_badstatus;
					else
					{
						hdmi_video_timing vtime;
						hdmi_tool_get_timing(pvid,&vtime);
						printf("lock again pvid value = 0x%08x\r\n",pvid);
						nres = h1_write_output_timing(&vtime,pvid);
						if(is_vatek_success(nres))nres = h1_write(H1_OUT_CNTL,H1_OUT_CNTL_DIS_ALL); // H1 Output initial
						if(is_vatek_success(nres))nres = h1_write_output(pdrv->param,pvid->resolution);
					}
				}
    	    }
    	}
    }
    else {
    	h1_reset();
    }
    return nres;
}

void h1_close(hbridge_source hsource)
{
	Ph1_handle pep = (Ph1_handle)hsource;
    free(pep);
}

vatek_result h1_write_output(h1_param output,video_resolution vresolution)
{
    uint8_t flag = 0;
    uint8_t vifmt = 0, vout_fmt = H1_OUT_FMT_422|H1_OUT_FMT_YCC;
    vatek_result nres = vatek_success;
    uint32_t h1_ver = 0;

	if(is_vatek_success(nres)){
		switch (output.scale){
			case sscale_bypass:
				flag = H1_HDMI_FLAG_BYPASS_MODE;
				nres = h1_write(H1_HDMI_FLAG,flag);//flag
				if(is_vatek_success(nres))nres = h1_read(H1_HDMI_FLAG,&reg_val);
				printf("h1_write_output function H1_HDMI_FLAG(0x0104) = 0x%08x\r\n",reg_val);
				break;
			case sscale_i_2_p:
				nres = h1_write(H1_HDMI_FLAG,flag);//flag
				if(is_vatek_success(nres))nres = h1_read(H1_HDMI_FLAG,&reg_val);
				printf("h1_write_output function H1_HDMI_FLAG(0x0104) = 0x%08x\r\n",reg_val);
				break;
			case sscale_p_2_i:
				flag = H1_HDMI_FLAG_OUT_INTERLACED;
				nres = h1_write(H1_HDMI_FLAG,flag);//flag
				if(is_vatek_success(nres))nres = h1_read(H1_HDMI_FLAG,&reg_val);
				printf("h1_write_output function H1_HDMI_FLAG(0x0104) = 0x%08x\r\n",reg_val);
				break;
		}
	}
	if(is_vatek_success(nres)){
	    nres = h1_read(H1_VIN_FMT, &vifmt);
	    if(vifmt&H1_VIN_FMT_CS709) vout_fmt |= H1_OUT_FMT_CS709;
	    if(vresolution == resolution_480i || vresolution == resolution_576i)
	    	vout_fmt |= H1_OUT_FMT_PR2;
	    nres = h1_write(H1_OUT_FMT, vout_fmt);//vout_fmt
	    nres = h1_read(H1_OUT_FMT,&reg_val);
	    printf("h1_write_output function H1_OUT_FMT = 0x%08x\r\n",reg_val);
	}

	//h1_version();
	if(is_vatek_success(nres)){
		h1_ver = h1_version();
		if (h1_ver >= 0x1101){
			if(is_vatek_success(nres))nres = h1_write(H1_OUT_CNTL, 0x40);
			hal_system_sleep(100);
			if(is_vatek_success(nres))nres = h1_read(H1_OUT_CNTL,&reg_val);
			printf("h1_write_output function H1_OUT_CNTL(0x0300) = 0x%08x\r\n",reg_val);
			if(is_vatek_success(nres))nres = h1_write(H1_AOUT_CFG, 0x13);
	        hal_system_sleep(100); //wait to write
	        if(is_vatek_success(nres))nres = h1_read(H1_AOUT_CFG,&reg_val);
	        printf("h1_write_output function H1_AOUT_CFG(0x0301) = 0x%08x\r\n",reg_val);
	        if(is_vatek_success(nres))nres = h1_write(H1_INT_EN,0x00);
//	        if(is_vatek_success(nres))nres = h1_write(H1_OUT_FMT, 0xA4);//vout_fmt, 0xA4
//	        hal_system_sleep(100);//wait to write
//	        if(is_vatek_success(nres))nres = h1_read(H1_OUT_FMT,&reg_val);
//			printf("h1_write_output function H1_OUT_FMT(0x0201) = 0x%08x\r\n",reg_val);
			if(is_vatek_success(nres))nres = h1_write(H1_VOUT_CFG, H1_VOUT_bt656_Y_Cb_Y_Cr | H1_VOUT_CFG_FRVS); //0x1= Frvs => Set 1 for reverse the output Field signal, otherwise.
	        hal_system_sleep(100);//wait to write
	        if(is_vatek_success(nres))nres = h1_read(H1_VOUT_CFG,&reg_val);
	        printf("h1_write_output function H1_VOUT_CFG(0x0202) = 0x%08x\r\n",reg_val);
	        if(is_vatek_success(nres))nres = h1_write(H1_INT_CNTL, 0x02);
	        hal_system_sleep(100);//wait to write
	        if(is_vatek_success(nres))nres = h1_read(H1_INT_CNTL,&reg_val);
	        printf("h1_write_output function H1_INT_CNTL(0x0103) = 0x%08x\r\n",reg_val);
		}
		else{
	    	hal_system_sleep(100);
	        nres = h1_write(H1_AOUT_CFG, 0x13);
	        hal_system_sleep(100); //wait to write
	        nres = h1_read(H1_AOUT_CFG,&reg_val);
	        printf("h1_write_output function H1_AOUT_CFG(0x0301) = 0x%08x\r\n",reg_val);

	        nres = h1_write(H1_INT_EN,0x00);

	        hal_system_sleep(100);
//	        nres = h1_write(H1_OUT_FMT, 0xA4);//vout_fmt
//	        hal_system_sleep(100);//wait to write
//			nres = h1_read(H1_OUT_FMT,&reg_val);
//			printf("h1_write_output function H1_OUT_FMT(0x0201) = 0x%08x\r\n",reg_val);

	        hal_system_sleep(100);
	        nres = h1_write(H1_VOUT_CFG, 0x42);
	        hal_system_sleep(100);//wait to write
	        nres = h1_read(H1_VOUT_CFG,&reg_val);
	        printf("h1_write_output function H1_VOUT_CFG(0x0202) = 0x%08x\r\n",reg_val);

	        hal_system_sleep(100);
	        nres = h1_write(H1_INT_CNTL, 0x02);
	        hal_system_sleep(100);//wait to write
	        nres = h1_read(H1_INT_CNTL,&reg_val);
	        printf("h1_write_output function H1_INT_CNTL(0x0103) = 0x%08x\r\n",reg_val);
	        hal_system_sleep(100);
	        nres = h1_write(H1_OUT_CNTL, 0x00);
	        hal_system_sleep(100);
	        nres = h1_read(H1_OUT_CNTL,&reg_val);
	        printf("h1_write_output function H1_OUT_CNTL(0x0300) = 0x%08x\r\n",reg_val);


		}
     }
    return nres;
}
#if 1
vatek_result h1_write_output_timing(Phdmi_video_timing ptiming,Phdmi_video_id pvid)
{
    vatek_result nres = vatek_success;

    if(pvid->represent != 0)
    {
        ptiming->hbp >>= 1;
        ptiming->hfp >>= 1;
        ptiming->hpw >>= 1;
    }

    ptiming->hfp = pvid->hfp;
    ptiming->hbp = pvid->hbp;
    ptiming->hpw = pvid->hpw;
    ptiming->vfp = pvid->vfp;
    ptiming->vbp = pvid->vbp;
    ptiming->vpw = pvid->vpw;
    if(is_vatek_success(nres)) nres = h1_write(H1_VOUT_V_WIDTH,ptiming->vpw);
    if(is_vatek_success(nres)) nres = h1_write(H1_VOUT_V_DELAY,ptiming->vfp);
    if(is_vatek_success(nres)) nres = h1_write(H1_VOUT_H_WIDTH,ptiming->hpw);
    if(is_vatek_success(nres)) nres = h1_write(H1_VOUT_H_DELAY,ptiming->hfp);
    if(is_vatek_success(nres)) nres = h1_write(H1_VOUT_ACTIVE_H,ptiming->pixel_per_line);
    if(is_vatek_success(nres)) nres = h1_write(H1_VOUT_ACTIVE_V,ptiming->pixel_per_frame);
    hdmi_tool_timing_printf(ptiming);
    return nres;
}
#endif
void h1_set_video_info(Phdmi_video_id phdmiinfo,Pbridge_source psource,bsource_scale output,int32_t is1001)
{
	if (output == sscale_bypass){
	    psource->video_info.resolution = phdmiinfo->resolution;
	    psource->video_info.pixelclk = phdmiinfo->pixelclk;
	}
	else if(output == sscale_i_2_p){
        switch (phdmiinfo->resolution)
        {
            case resolution_1080i:
            	psource->video_info.resolution = resolution_1080p;
                break;
            case resolution_576i:
            	psource->video_info.resolution = resolution_576p;
                break;
             case resolution_480i:
            	psource->video_info.resolution = resolution_480p;
                break;
         }
    	psource->video_info.pixelclk = phdmiinfo->pixelclk * 2;
	}
	else if (output == sscale_p_2_i){
        switch (phdmiinfo->resolution)
        {
            case resolution_1080p:
            	psource->video_info.resolution = resolution_1080i;
                break;
            case resolution_576p:
            	psource->video_info.resolution = resolution_576i;
                break;
             case resolution_480p:
            	psource->video_info.resolution = resolution_480i;
                break;
         }
	}
    psource->video_info.framerate = phdmiinfo->framerate;
    psource->video_info.aspectrate = phdmiinfo->aspectratio;
    psource->video_info.offset_x = 0;
    psource->video_info.offset_y = 0;

    if(phdmiinfo->represent != 0)
        psource->video_info.pixelclk >>= 1;

    psource->video_info.signal_flag = VI_BUSWIDTH_16 | VI_SEPARATED_SYNC;
    Pbridge_video_info v_info = &psource->video_info;
    hdmi_tool_change_base_clock(is1001,&psource->video_info);
}

vatek_result h1_check_link_on(bridge_status* bstatus)
{
    #define H1_HDMI_VALID  (H1_SYS_STATUS2_DE | H1_SYS_STATUS2_LINK)
    uint8_t val = 0;
    vatek_result nres = h1_read(H1_SYS_STATUS2,&val);
    *bstatus = bstatus_idle;
    if(is_vatek_success(nres))
    {
        if(val & H1_SYS_STATUS2_HPD)
        {
            if((val & H1_HDMI_VALID) == H1_HDMI_VALID)
            {
                *bstatus = bstatus_active;
                /* check is hdcp enable ?? bstatus_active_protect */
            }else *bstatus = bstatus_invalid;
        }
    }
    return nres;
}

vatek_result h1_write_uint16_t(uint16_t reg,uint16_t val)
{
    vatek_result nres = h1_write(reg++,(uint8_t)val);
    if(is_vatek_success(nres))nres = h1_write(reg,(uint8_t)(val >> 8));
    return nres;
}

vatek_result h1_write(uint16_t reg,uint8_t val)
{
	uint8_t buf[2];
	vatek_result nres = hal_i2c_start(H1_I2C_WRITE,0);
	if (is_vatek_success(nres))
	{
		buf[0] = (uint8_t)(reg >> 8);
		buf[1] = (uint8_t)reg;
		nres = hal_i2c_write(&buf[0], 2);
        if(is_vatek_success(nres))nres = hal_i2c_write(&val, 1);
		if (is_vatek_success(nres))nres = hal_i2c_stop();
	}
    return nres;
}

vatek_result h1_read(uint16_t reg,uint8_t* val)
{
    vatek_result nres = hal_i2c_start(H1_I2C_WRITE,0);
    if(is_vatek_success(nres))
    {
        uint8_t buf[2];
        buf[0] = (uint8_t)(reg >> 8);
        buf[1] = (uint8_t)reg;
        nres = hal_i2c_write(&buf[0],2);
        if(is_vatek_success(nres))
        {
            nres = hal_i2c_start(H1_I2C_READ,1);
            if(is_vatek_success(nres))nres = hal_i2c_read(val,1);
            if(is_vatek_success(nres))nres = hal_i2c_stop();
        }
    }
    return nres;
}

vatek_result h1_enable(){
	vatek_result nres = vatek_unknown;
	nres = h1_write(H1_OUT_CNTL, 0x40);
	nres = h1_read(H1_OUT_CNTL,&reg_val);
	printf("h1_enable H1_OUT_CNTL = 0x%08x\r\n",reg_val);
	return nres;
}

uint32_t h1_version(){
	uint8_t ver_num = 0;
	uint8_t ver_numa = 0;
	uint32_t h1_allver = 0;

	vatek_result nres = h1_read(H1_VER1, &ver_num);
	if(nres == vatek_success)nres = h1_read(H1_VER2, &ver_numa);
	if(nres == vatek_success)h1_allver = ver_num << 8 | ver_numa;
	return h1_allver;
}

vatek_result h1_reset(void)
{
	vatek_result nres = nres = hal_gpio_set(HAL_IO_AV_RESET,0);
    if(is_vatek_success(nres))
        nres = hal_gpio_set(HAL_IO_HDMI_NIM_RESET,0);
    if(is_vatek_success(nres))
    {
        hal_system_sleep(100);
        nres = hal_gpio_set(HAL_IO_AV_RESET,1);
        if(is_vatek_success(nres))
            nres = hal_gpio_set(HAL_IO_HDMI_NIM_RESET,1);

		if(is_vatek_success(nres))
			hal_system_sleep(2000); //5000
    }
    return nres;
}
