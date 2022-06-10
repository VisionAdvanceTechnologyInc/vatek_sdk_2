#include "internal/adv718x_internal.h"

/* 
    https://github.com/analogdevicesinc/linux/blob/master/drivers/media/i2c/adv7180.c 
*/

/* hiddle reg : 0x80 page */
#define ADV7180_REG_RST_CLAMP	        	0x9c
#define ADV7180_REG_AGC_ADJ1	        	0xb6
#define ADV7180_REG_AGC_ADJ2	        	0xc0

/* hiddle reg : 0x40 page */
#define ADV7180_REG_ACE_CTRL1		    	0x80
#define ADV7180_REG_ACE_CTRL5		    	0x84
#define ADV7180_REG_FLCONTROL		    	0xe0
    #define ADV7180_FLCONTROL_FL_ENABLE 	0x1

typedef struct _adv718x_handle
{
    uint8_t chip_id;
    uint8_t reg_out;
    uint8_t reg_ext_out;
    uint8_t input_std;
    adv718x_param param;
}adv718x_handle,*Padv718x_handle;

extern vatek_result adv718x_write_reg(uint8_t reg, uint8_t val);
extern vatek_result adv718x_read_req(uint8_t reg, uint8_t* val);

extern vatek_result adv718x_chip_reset(Padv718x_handle padv);
extern vatek_result adv718x_chip_open(Padv718x_handle padv);
extern uint8_t adv718x_chip_get_input_type(Padv718x_handle padv);
extern vatek_result adv718x_get_check_status(Padv718x_handle padv,bridge_status* bstatus,Ptv_encoding_param* pvparam);

vatek_result adv718x_check_support()
{
    return hal_i2c_check_device(ADV718X_DEVICE_ADDR);
}

vatek_result adv718x_open(hbridge_source* hsource)
{
    vatek_result nres = vatek_memfail;
    Padv718x_handle newadv = (Padv718x_handle)malloc(sizeof(adv718x_handle));
    if(newadv)
    {
		memset(newadv,0,sizeof(adv718x_handle));
		memcpy(&newadv->param,&def_adv718x_param,sizeof(adv718x_param));
        newadv->input_std = newadv->param.input | (newadv->param.std  << 4);
        nres = adv718x_chip_open(newadv);
        if(is_vatek_success(nres))*hsource = newadv;
    }
    return nres;
}

vatek_result adv718x_config(hbridge_source hsource,Pbavsource_param param)
{
	return vatek_unsupport;
}

vatek_result adv718x_get_status(hbridge_source hsource,Pbridge_source pbsourcesttaus)
{
    vatek_result nres = vatek_success;
    Padv718x_handle padv = (Padv718x_handle)hsource;
    Ptv_encoding_param pstd = NULL;

    memset(pbsourcesttaus,0,sizeof(bridge_source));
    nres = adv718x_get_check_status(padv,&pbsourcesttaus->status,&pstd);
    if(is_vatek_success(nres))
    {
        if(pbsourcesttaus->status >= bstatus_active)
        {
            pbsourcesttaus->video_info.offset_x = 0;
            pbsourcesttaus->video_info.offset_y = 0;
            pbsourcesttaus->video_info.pixelclk = 27000;
            pbsourcesttaus->video_info.resolution = resolution_unknown;
            pbsourcesttaus->video_info.aspectrate = aspectrate_16_9;
            pbsourcesttaus->video_info.framerate = framerate_unknown;
            pbsourcesttaus->video_info.signal_flag = 0; /* embended_sync */
            pbsourcesttaus->video_info.framerate = pstd->framerate;
            pbsourcesttaus->video_info.resolution = pstd->resolution;
            pbsourcesttaus->audio_info.channel = channel_stereo;
            pbsourcesttaus->audio_info.samplerate = sample_rate_48;
        }
    }
    return nres;
}

vatek_result adv718x_set_output(hbridge_source hsource,int32_t isoutput)
{
    vatek_result nres = vatek_success;
    Padv718x_handle padv = (Padv718x_handle)hsource;
    if(isoutput)
    {
        bridge_status bstatus = bstatus_idle;
        nres = adv718x_get_check_status(padv,&bstatus,NULL);
        if(is_vatek_success(nres))
        {
            if(bstatus < bstatus_active)nres = vatek_badstatus;
            else 
            {
                nres = adv718x_write_reg(ADV718X_BRIGHTNESS,padv->param.brightnes);
                if(is_vatek_success(nres))
                    nres = adv718x_write_reg(ADV718X_HUE,-padv->param.hue);
                if(is_vatek_success(nres))
                    nres = adv718x_write_reg(ADV718X_CONTRAST,padv->param.contrast);
                if(is_vatek_success(nres))
                    nres = adv718x_write_reg(ADV718X_CONTRAST,padv->param.contrast);
                if(is_vatek_success(nres))
                    nres = adv718x_write_reg(ADV718X_SD_SATURATION_CB,padv->param.saturation);
                if(is_vatek_success(nres))
                    nres = adv718x_write_reg(ADV718X_SD_SATURATION_CR,padv->param.saturation);
                
                if(is_vatek_success(nres))
                    nres = adv718x_write_reg(ADV718X_ADI_CTRL_2,ADV718X_EN_LLC);
                if(is_vatek_success(nres))
                    nres = adv718x_write_reg(ADV718X_OUT_CTRL,padv->reg_out & 0x0F);
            }
        }
    }else 
    {
        nres = adv718x_write_reg(ADV718X_ADI_CTRL_2,ADV718X_TRI_LLC);
        if(is_vatek_success(nres))
            nres = adv718x_write_reg(ADV718X_OUT_CTRL,padv->reg_out);
    }
    return nres;
}

void adv718x_close(hbridge_source hsource)
{
    Padv718x_handle padv = (Padv718x_handle)hsource;
    free(padv);
}

/* Reset clamp circuitry - ADI recommended writes */
extern vatek_result adv7182_select_input(Padv718x_handle padv);
extern vatek_result adv7182_reset_clamp(void);
extern vatek_result adv7182_adj_clamp(uint8_t v0,uint8_t v1);

vatek_result adv718x_get_check_status(Padv718x_handle padv,bridge_status* bstatus,Ptv_encoding_param* pvparam)
{
    uint8_t status0 = 0;
    vatek_result nres = adv718x_read_req(ADV718X_STATUS_1,&status0);
    if(is_vatek_success(nres))
    {
        *bstatus = bstatus_idle;
        if(status0 & ADV718X_STATUS1_IN_LOCK)
        {
            uint8_t status3 = 0;
            nres = adv718x_read_req(ADV718X_STATUS_3,&status3);
            status0 = (status0 & ADV718X_STATUS1_AUTOD_MASK) >> 4;
            
            if(is_vatek_success(nres))
            {
                Ptv_encoding_param pstd = (Ptv_encoding_param)&tv_encoding_spec[status0][0];
                int8_t is_interlanced = ((status3 & ADV718X_STATUS3_IS_I) != 0);
                int8_t is_f50 = ((status3 & ADV718X_STATUS3_50FPS) != 0);
                if(pstd->framerate == framerate_50 && !is_f50)*bstatus = bstatus_invalid;
                else if(pstd->framerate != framerate_50 && is_f50)*bstatus = bstatus_invalid;
                else 
                {
                    if(pvparam)*pvparam = (Ptv_encoding_param)&tv_encoding_spec[status0][!is_interlanced];
                    *bstatus = bstatus_active;
                }
            }  
        }
    }
    return nres;    
}

vatek_result adv718x_write_reg(uint8_t reg, uint8_t val)
{
	uint8_t buf[2];
	vatek_result nres = vatek_success;
    
    buf[0] = reg;
    buf[1] = val;
    
    nres = hal_i2c_start(ADV718X_SLAVE_WRITE,0);
	if (is_vatek_success(nres))
	{
		nres = hal_i2c_write(&buf[0], 2);
		if (is_vatek_success(nres))nres = hal_i2c_stop();
	}
    return nres;
}

vatek_result adv718x_read_req(uint8_t reg, uint8_t* val)
{
    vatek_result nres = hal_i2c_start(ADV718X_SLAVE_WRITE,0);
	if (is_vatek_success(nres))nres = hal_i2c_write(&reg, 1);
	if (is_vatek_success(nres))
	{
		nres = hal_i2c_start(ADV718X_SLAVE_READ,1); /* re-start */
		if (is_vatek_success(nres))nres = hal_i2c_read(val, 1);
        if (is_vatek_success(nres))nres = hal_i2c_stop();
	}
    return nres;
}

vatek_result adv718x_chip_reset(Padv718x_handle padv)
{
    vatek_result nres = adv718x_write_reg(ADV718X_POW_MANAGE,ADV718X_PWR_MAN_RES);
    if(is_vatek_success(nres))
    {
        hal_system_sleep(10);
        nres = adv718x_read_req(ADV718X_IDENT,&padv->chip_id);
        if(is_vatek_success(nres))
        {
            nres = vatek_unsupport;
            if(is_adv718x_id_valid(padv->chip_id))
            {
                padv->reg_out = ADV718X_OUT_EMBENDED;
                padv->reg_ext_out = ADV7180_EXT_OUT_656_4;
                /* default output bt656-4 pin */
                if(is_adv_7282(padv))padv->reg_ext_out = ADV7182_EXT_OUT_656_4;
                nres = adv718x_write_reg(ADV718X_POW_MANAGE,ADV718X_PWR_MAN_ON);
            }
        }
    }
    return nres;
}

vatek_result adv718x_chip_open(Padv718x_handle padv)
{
    vatek_result nres = adv718x_chip_reset(padv);
    if(is_vatek_success(nres))
    {
        if(is_adv_7282(padv))
        {
            nres = adv7182_select_input(padv);
        }else nres = adv718x_write_reg(ADV718X_IN_CTRL,padv->input_std);
    }

    if(is_vatek_success(nres))
    {
        nres = adv718x_write_reg(ADV718X_OUT_CTRL,padv->reg_out);
        if(is_vatek_success(nres))nres = adv718x_write_reg(ADV718X_EXT_OUT_CTRL,padv->reg_ext_out);
        //if(is_hal_lite_success(nres))nres = adv718x_write_reg(padv->i2c,ADV718X_VS_FIELD_CTRL_1,0x02);
        if(is_vatek_success(nres))nres = adv718x_write_reg(ADV718X_ADI_CTRL_2,ADV718X_TRI_LLC);
            
            
        if(is_vatek_success(nres))
        {
            if(is_adv_7282(padv))nres = adv718x_write_reg(ADV718X_STATUS_3,0x00);
            else
            {
                nres = adv718x_write_reg(ADV718X_NTSC_V_END,0x4F);
                if(is_vatek_success(nres))
                nres = adv718x_write_reg(ADV718X_IN_CTRL,padv->input_std);
            }
        }
    }
    return nres;
}

uint8_t adv718x_chip_get_input_type(Padv718x_handle padv)
{
    uint8_t val = padv->input_std & 0x0F;
    if(val >= adv718x_in_svideo_ain_1_2 && val <= adv718x_in_svideo_ain_5_6)
        return ADV718X_INPUT_TYPE_SVIDEO;
    else if(val >= adv718x_in_ypbpr_ain_1_2_3 && val <= adv718x_in_ypbpr_ain_4_5_6)
        return ADV718X_INPUT_TYPE_YPBPR;
    else if(val > adv718x_in_cvbs_ain_6)
		return ADV718X_INPUT_TYPE_UNKNOWN;
    return ADV718X_INPUT_TYPE_CVBS;
}

vatek_result adv7182_select_input(Padv718x_handle padv)
{
    static const uint8_t adv7182_lbias_regs[][3] = 
    {
        {0xCB, 0x4E, 0x80,},
        {0x0B, 0xCE, 0x80,},
        {0x0B, 0x4E, 0xC0,},
    };
    
    uint8_t type = adv718x_chip_get_input_type(padv);
	vatek_result nres = vatek_unsupport;
	if(type != ADV718X_INPUT_TYPE_UNKNOWN)
	{
		nres = adv718x_write_reg(ADV718X_IN_CTRL,padv->input_std & 0xF);
		if(is_vatek_success(nres))
			nres = adv718x_write_reg(ADV7182_IN_SEL_2,(padv->input_std & 0xF0) | 0x04);
		if(is_vatek_success(nres))
			nres = adv7182_reset_clamp();
						
		if(is_vatek_success(nres))
		{
			uint8_t* plbias = (uint8_t*)&adv7182_lbias_regs[type];                        
			for(type = 0;type < 3;type++)
			{
				nres = adv718x_write_reg(ADV7182_CVBS_TRIM + type,plbias[type]);
				if(!is_vatek_success(nres))break;
			}
							
			if(type == ADV718X_INPUT_TYPE_CVBS)
				nres = adv718x_write_reg(ADV718X_SHAP_FILT_CTRL,0x41);
			else nres = adv718x_write_reg(ADV718X_SHAP_FILT_CTRL,0x01);
			
			#if 0
			if(is_hal_lite_success(nres))
			{
				nres = adv718x_write_reg(padv->i2c, ADV7182_REG_RES_CIR, 0xf0);
				if(is_hal_lite_success(nres))
					nres = adv718x_write_reg(padv->i2c, ADV7182_REG_CLAMP_ADJ, 0xd0);
				if(is_hal_lite_success(nres))
					nres = adv718x_write_reg(padv->i2c, ADV7182_REG_DIFF_MODE, 0x10);
				if(is_hal_lite_success(nres))
					nres = adv7182_adj_clamp(padv->i2c,0x9c,0x00);
			}
			#endif
		} 
	}
    return nres;
}

vatek_result adv7182_reset_clamp(void)
{
    vatek_result nres = adv718x_write_reg(ADV718X_ADI_CTRL,0x80);
    if(is_vatek_success(nres))
    {
        if(is_vatek_success(nres))nres = adv718x_write_reg(ADV7180_REG_RST_CLAMP,0x00);
        if(is_vatek_success(nres))nres = adv718x_write_reg(ADV7180_REG_RST_CLAMP,0xFF);
    }
    
    /* 
        Fast switch mode : 
        allows the ADV7182/728x video decoders to lock to single-ended CVBS or differential CVBS video in under 250 ms.
    */
    #if 0
    if(is_hal_lite_success(nres))
    {
        nres =  adv718x_write_reg(pi2c,0xD9,0x44);
        if(is_hal_lite_success(nres))nres = adv718x_write_reg(pi2c,ADV718X_ADI_CTRL,0x40);
        if(is_hal_lite_success(nres))nres = adv718x_write_reg(pi2c,ADV7180_REG_FLCONTROL,0x01);
    }
    #endif
    
    if(is_vatek_success(nres))nres = adv718x_write_reg(ADV718X_ADI_CTRL,0x00);
    return nres;
}

vatek_result adv7182_adj_clamp(uint8_t v0,uint8_t v1)
{
    vatek_result nres = adv718x_write_reg(ADV718X_ADI_CTRL,0x80);
    if(is_vatek_success(nres))
    {
        if(is_vatek_success(nres))nres = adv718x_write_reg(ADV7180_REG_AGC_ADJ1,v0);
        if(is_vatek_success(nres))nres = adv718x_write_reg(ADV7180_REG_AGC_ADJ2,v1);
        if(is_vatek_success(nres))nres = adv718x_write_reg(ADV718X_ADI_CTRL,0x00);
    }
    return nres;  
}
