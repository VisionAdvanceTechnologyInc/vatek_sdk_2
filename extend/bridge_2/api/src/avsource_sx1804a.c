#include "internal/sx1804a_internal.h"
#include <stm32_define.h>

typedef struct _h1_handle
{

    sx1804_param param;
}h1_handle,*Ph1_handle;

extern vatek_result h1_write(uint16_t reg,uint8_t val);
extern vatek_result h1_write_uint16_t(uint16_t reg,uint16_t val);
extern vatek_result h1_read(uint16_t reg,uint8_t* val);
extern vatek_result h1_check_link_on(bridge_status* bstatus);
extern void h1_set_video_info(Phdmi_video_id phdmiinfo,Pbridge_source psource,h1_output_mode output,int32_t is1001);
extern vatek_result h1_write_output_timing(Phdmi_video_timing ptiming,Phdmi_video_id pvid);
extern vatek_result h1_write_output(sx1804_param output,Pbridge_source psource);
//extern void sa1804a_set_video_info(Phdmi_video_id phdmiinfo,Pbridge_video_info psource,int32_t isclk1001);

static uint8_t reg_val = 0;
vatek_result h1_check_support()
{
    return hal_i2c_check_device(H1_I2C_ADDR);
}

vatek_result h1_open(hbridge_source* hsource)
{
    Ph1_handle newdrv = (Ph1_handle)malloc(sizeof(h1_handle));
    vatek_result nres = vatek_memfail;
    nres = h1_write(H1_POWER,H1_POWER_ON);
    nres = h1_read(H1_POWER,&reg_val);
    printf("h1_open function H1_POWER = 0x%08x\r\n",reg_val);
    if(!is_vatek_success(nres)){
    	printf("H1 power on fail %d",nres);
    	return nres;
    }
    if(newdrv)
    {
        memset(newdrv,0,sizeof(h1_handle));
        //newdrv->pi2c = pi2c;
        newdrv->param.scale = sscale_bypass;
        *hsource = newdrv;
        nres = h1_set_output(*hsource,0,NULL);
    }
    else
    	printf("open H1 device fail");
    return nres;
}

vatek_result h1_config(hbridge_source hsource,Pbavsource_param param)
{
	printf("h1 configuration well");
    return vatek_unsupport;
}

vatek_result h1_get_video_info(hbridge_source hsource,Pbridge_source psource)
{
    Ph1_handle pdrv = (Ph1_handle)hsource;
    vatek_result nres = h1_check_link_on(&psource->status);
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
                if(ptime == NULL)
                {
                    psource->status = bstatus_invalid;
                    _HAL_LOG(h1,"unknown video code : %d",vstatus);
                }else h1_set_video_info(ptime,psource,h1_output_bypass,(vhdmi & H1_HDMI_FLAG_PCLK_1001));
            }
        }
    }
    return nres;
}

vatek_result h1_set_output_mode(hbridge_source hsource,sx1804_param output)
{
    Ph1_handle pdrv = (Ph1_handle)hsource;
    pdrv->param = output;
    return vatek_success;
}

//void sa1804a_set_video_info(Phdmi_video_id phdmiinfo,Pbridge_video_info psource,int32_t isclk1001)
//{
//    psource->resolution = phdmiinfo->resolution;
//    psource->aspectrate = phdmiinfo->aspectratio;
//    psource->framerate = phdmiinfo->framerate;
//    psource->offset_x = 0;
//    psource->offset_y = 0;
//    psource->pixelclk = phdmiinfo->pixelclk;
//
//    if(phdmiinfo->represent != 0)
//        psource->pixelclk >>= 1;
//
//    psource->signal_flag = VI_BUSWIDTH_16 | VI_SEPARATED_SYNC;
//    if(isclk1001)hdmi_tool_change_base_clock(sbaseclk_1001,psource);
//}

vatek_result h1_get_status(hbridge_source hsource,Pbridge_source pbsourcesttaus)
{
	uint32_t tick_error = 0;
    vatek_result nres = vatek_success;
    memset(pbsourcesttaus,0,sizeof(bridge_source));

    nres = h1_check_link_on(&pbsourcesttaus->status);
    if(is_vatek_success(nres))
    {
        if(pbsourcesttaus->status >= bstatus_active) //remove so it can pass to next step, and can see the parameter on interface 220110
        {
            uint8_t vstatus = 0;
            uint8_t vhdmi = 0x40; //0 20220310
            //hal_system_sleep(500);
            nres = h1_read(H1_VIN_VIC,&vstatus);
            //printf("h1_get_status H1_VIN_VIC(0x0204) = 0x%08x\r\n",vstatus);
            if(is_vatek_success(nres)){
                nres = h1_read(H1_HDMI_FLAG,&vhdmi);
            	//printf("h1_get_status H1_HDMI_FLAG(0x0104) = 0x%08x\r\n",vhdmi);
            }
            if(is_vatek_success(nres))
            {
            	hal_system_sleep(1000);//1000
                Phdmi_video_id ptime = hdmi_tool_get_video_id(vstatus);
                //printf("ptime = 0x%08x------\r\n",ptime);
//                while(ptime == NULL){
//                	hal_system_sleep(10);
//                	ptime = hdmi_tool_get_video_id(vstatus);
//                }
//                h1_set_video_info(ptime,pbsourcesttaus,h1_output_bypass,(vhdmi &H1_HDMI_FLAG_PCLK_1001));
                if(ptime == NULL)
                {
                	hal_system_sleep(500);
                	ptime = hdmi_tool_get_video_id(vstatus);
                	printf("lock again ptime value = 0x%08x\r\n",ptime);
                	if(ptime == NULL){
                		hal_system_sleep(500);
                		ptime = hdmi_tool_get_video_id(vstatus);
                		pbsourcesttaus->status = bstatus_invalid;
                		printf("pbsource status change value to invalid\r\n");
                		printf("status change time = 0x%08x\r\n",vatek_get_tick_ms());
                	}

                    _HAL_LOG(h1,"unknown video code : %d",vstatus);
                }else h1_set_video_info(ptime,pbsourcesttaus,h1_output_bypass,(vhdmi & H1_HDMI_FLAG_PCLK_1001));
            }

            if(is_vatek_success(nres))
            {
				#define AUDIO_SAMPLE_FREQ_32KHZ		0x00
				#define AUDIO_SAMPLE_FREQ_44_1KHZ	0x01
				#define AUDIO_SAMPLE_FREQ_48KHZ		0x02

				//Ph1_handle pdrv = (Ph1_handle)hsource;

				if(is_vatek_success(nres))
				{
					if(pbsourcesttaus->status >= bstatus_active)
					{
						uint8_t val = 0;
						nres = h1_read(H1_AIN_STATUS,&val);
						//printf("h1_get_status H1_AIN_STATUS(0x0302) = 0x%08x\r\n",val);
						if(is_vatek_success(nres))
						{
							val &= H1_AIN_STATUS_SAMP_FREQ;
							if(val == AUDIO_SAMPLE_FREQ_32KHZ)pbsourcesttaus->audio_info.samplerate = sample_rate_32;
							else if(val == AUDIO_SAMPLE_FREQ_44_1KHZ)pbsourcesttaus->audio_info.samplerate = sample_rate_44_1;
							else pbsourcesttaus->audio_info.samplerate = sample_rate_48;
							pbsourcesttaus->audio_info.channel = channel_stereo;
						}
					}else{
						hal_system_sleep(100);
						printf("bridge status = %d(idle=0,hw_fail=-1,invalid=-2)\r\n",pbsourcesttaus->status);
						//hal_system_sleep(100);
					}
				}

            }
            if(!is_vatek_success(nres))pbsourcesttaus->status = bstatus_invalid;

        }else{
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
    }
    return nres;
}

vatek_result h1_set_output(hbridge_source hsource, int32_t isoutput, Pbridge_source psource)//, Pbridge_source psource, remove psource to avoid address overwrite
{
    vatek_result nres = vatek_success;
    Ph1_handle pdrv = (Ph1_handle)hsource;
    if(isoutput)
    {

//        nres = h1_get_video_info(hsource,psource);
//        if(is_vatek_success(nres))
//        {
//            nres = vatek_badstatus;
//            if(psource->status >= bstatus_active)
//            {
#if 1
                hdmi_video_timing vtime;
                Phdmi_video_id pvid = hdmi_tool_get_video_id_by_info(psource->video_info.resolution,
                                                                     psource->video_info.framerate,
                                                                     psource->video_info.aspectrate);
                if(pvid)
                {
                    #if 0
                    if(pdrv->output == sx1804a_output_i_to_p && is_resolution_interlaced(pvid->resolution))
                    {
                        video_resolution resolutionscale = psource->info.resolution;
                        if(pvid->resolution == resolution_1080i)resolutionscale = resolution_1080p;
                        else if(pvid->resolution == resolution_480i)resolutionscale = resolution_480p;
                        else if(pvid->resolution == resolution_576i)resolutionscale = resolution_576p;
                        else _HAL_ERR(sx1804a,"unknown resolution : %d",resolutionscale);
                        pvid = hdmi_tool_get_video_id_by_info(resolutionscale,psource->info.framerate,psource->info.aspectrate);
                    }
                    #endif

                    hdmi_tool_get_timing(pvid,&vtime);
                    nres = h1_write_output_timing(&vtime,pvid);
                }
#endif

                if(is_vatek_success(nres)){
                    nres = h1_write_output(pdrv->param,psource);//NULL , psource
                }

//            }
//        }
    }else{
    	nres = h1_write(H1_OUT_CNTL,H1_OUT_CNTL_DIS_ALL); /* disable output */
    	hal_system_sleep(500);
    	nres = h1_read(H1_OUT_CNTL,&reg_val);
    	printf("h1_set_output function H1_OUT_CNTL = 0x%08x\r\n",reg_val);
    }
   
    return nres;
}

void h1_close(hbridge_source hsource)
{
    free(hsource);
}

vatek_result h1_write_output(sx1804_param output,Pbridge_source psource)
{
//    #define SX1804A_DEF_OUTPUT  (SX1804A_V_MODE_VOUT_EN | SX1804A_V_MODE_COLOR_YCRCB_422)
//    #define SX1804A_DEF_V_BYPASS (SX1804A_V_IN_DEF_V_INVERT | SX1804A_V_IN_DEF_SCALE_BYPASS)
    uint8_t flag = H1_HDMI_FLAG_BYPASS_MODE;
    vatek_result nres = vatek_success;

//    nres = h1_write(H1_OUT_CNTL,H1_OUT_CNTL_DIS_ALL); /* disable output */
    output.scale = sscale_bypass;
    if(output.scale == sscale_bypass)
    {
        if(is_resolution_interlaced(psource->video_info.resolution))
        {
            flag |= H1_HDMI_FLAG_OUT_INTERLACED;
            nres = h1_write(H1_HDMI_FLAG,0x0);//flag
            nres = h1_read(H1_HDMI_FLAG,&reg_val);
            printf("h1_write_output function H1_HDMI_FLAG(0x0104) = 0x%08x\r\n",reg_val);

            //output format

#if 0

            nres = h1_write(pi2c,SX1804A_V_IN_DEF,0x02);                               /* enable scale         */

            if(is_vatek_success(nres))
            { /* pixel cnt bypass     */
                 if(psource->info.resolution == resolution_1080i){                           /* interlaced mode      */
                     //nres = sx1804a_write(pi2c,0x30a1,0x80);
										 //psource->info.resolution = resolution_1080p;
										 nres = h1_write(pi2c,SX1804A_HDMI_FLAG,SX1804A_HDMI_FLAG_INTERLACED);

										 printf("i2p success\r\n");
								 }
                 else nres = h1_write(pi2c,SX1804A_HDMI_FLAG,0);//nres = sx1804a_write(pi2c,0x30a1,0x00);
            }

            if(is_vatek_success(nres))nres = h1_write(pi2c,0x30a6,0x08);            /* line cnt bypass      */
            if(is_vatek_success(nres))nres = h1_write(pi2c,0x30c0,0x00);
            if(is_vatek_success(nres))nres = h1_write(pi2c,0x30c1,0x00);
            if(is_vatek_success(nres))nres = h1_write(pi2c,0x30c2,0x01);
            if(is_vatek_success(nres))nres = h1_write(pi2c,0x30c3,0x00);

            if(is_vatek_success(nres))
            {
                if(psource->info.resolution == resolution_1080i)
                    nres = h1_write(pi2c,0x30c4,0x01);
                else nres = h1_write(pi2c,0x30c4,0x03);                                /* remove repetition */
            }
            if(is_vatek_success(nres))nres = h1_write(pi2c,SX1804A_V_BT656,0x22);   /* interlaced mode      */
        else nres = h1_write(pi2c,SX1804A_V_IN_DEF,SX1804A_DEF_V_BYPASS);//SX1804A_V_IN_DEF_SCALE_BYPASS
#endif
        }

        uint8_t vifmt = 0, vout_fmt = H1_OUT_FMT_422|H1_OUT_FMT_YCC;
        nres = h1_read(H1_VIN_FMT, &vifmt);
        if(vifmt&H1_VIN_FMT_CS709) vout_fmt |= H1_OUT_FMT_CS709;
        if(psource->video_info.resolution == resolution_480i || psource->video_info.resolution == resolution_576i) vout_fmt |= H1_OUT_FMT_PR2;
        nres = h1_write(H1_OUT_FMT, vout_fmt);//vout_fmt
        nres = h1_read(H1_OUT_FMT,&reg_val);
        printf("h1_write_output function H1_OUT_FMT = 0x%08x\r\n",reg_val);
    }


    if(is_vatek_success(nres))
    {

//        nres = sx1804a_write(pi2c,SX1804A_POL_CNT,SX1804A_POL_CNT_HS | SX1804A_POL_CNT_VS);
//        if(is_vatek_success(nres))
    	hal_system_sleep(100);
        nres = h1_write(H1_AOUT_CFG, 0x13);
        hal_system_sleep(100); //wait to write
        nres = h1_read(H1_AOUT_CFG,&reg_val);
        printf("h1_write_output function H1_AOUT_CFG(0x0301) = 0x%08x\r\n",reg_val);

        nres = h1_write(H1_INT_EN,0x00);

        hal_system_sleep(100);
        nres = h1_write(H1_OUT_FMT, 0xA4);//vout_fmt
        hal_system_sleep(100);//wait to write
		nres = h1_read(H1_OUT_FMT,&reg_val);
		printf("h1_write_output function H1_OUT_FMT(0x0201) = 0x%08x\r\n",reg_val);

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
    return nres;
}
#if 1
vatek_result h1_write_output_timing(Phdmi_video_timing ptiming,Phdmi_video_id pvid)
{
    //uint16_t addr = SX1804A_V_SCALE;
    uint16_t vtotal = ptiming->pixel_per_frame + ptiming->vbp + ptiming->vfp + ptiming->vpw;
    vatek_result nres = vatek_success;

    if(pvid->represent != 0)
    {
        ptiming->hbp >>= 1;
        ptiming->hfp >>= 1;
        ptiming->hpw >>= 1;
    }

//    nres = h1_write(pi2c,addr++,ptiming->vbp);
//    if(is_vatek_success(nres))
    nres = h1_write(H1_VOUT_V_WIDTH,ptiming->vpw);

    if(is_vatek_success(nres))
        nres = h1_write_uint16_t(H1_VOUT_V_DELAY,ptiming->hfp);
//    addr += 2;

    if(is_vatek_success(nres))
        nres = h1_write_uint16_t(H1_VOUT_H_WIDTH,ptiming->hpw);
//    addr += 2;

//    if(is_vatek_success(nres))
//        nres = h1_write_uint16_t(pi2c,addr,ptiming->hbp);
//    addr += 2;

    if(is_vatek_success(nres))
        nres = h1_write_uint16_t(H1_VOUT_ACTIVE_H,ptiming->pixel_per_line);
//    addr += 2;

    if(is_vatek_success(nres))
        nres = h1_write_uint16_t(H1_VOUT_ACTIVE_V,ptiming->pixel_per_frame);
//    addr += 2;

//    if(is_vatek_success(nres))
//        nres = h1_write_uint16_t(pi2c,addr,vtotal);

    return nres;
}
#endif
void h1_set_video_info(Phdmi_video_id phdmiinfo,Pbridge_source psource,h1_output_mode output,int32_t is1001)
{
    psource->video_info.resolution = phdmiinfo->resolution;
    psource->video_info.aspectrate = phdmiinfo->aspectratio;
    psource->video_info.framerate = phdmiinfo->framerate;
    psource->video_info.offset_x = 0;
    psource->video_info.offset_y = 0;
    psource->video_info.pixelclk = phdmiinfo->pixelclk;

    if(phdmiinfo->represent != 0)
        psource->video_info.pixelclk >>= 1;

    psource->video_info.signal_flag = VI_BUSWIDTH_16 | VI_SEPARATED_SYNC;
    Pbridge_video_info v_info = &psource->video_info;
    if(is1001)hdmi_tool_change_base_clock(sbaseclk_1001,&psource->video_info);
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
    if(is_vatek_success(nres))
        nres = h1_write(reg,(uint8_t)(val >> 8));
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
            if(is_vatek_success(nres))
                nres = hal_i2c_read(val,1);
            if(is_vatek_success(nres))
                nres = hal_i2c_stop();
        }
    }
    return nres;
}

vatek_result h1_enable(){
	vatek_result nres = vatek_unknown;
	nres = h1_write(H1_OUT_CNTL, 0x00);
	nres = h1_read(H1_OUT_CNTL,&reg_val);
	printf("h1_enable H1_OUT_CNTL = 0x%08x\r\n",reg_val);
	return nres;
}

