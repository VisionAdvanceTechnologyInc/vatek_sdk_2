#include <driver/av/tools/ep9xxx_tool.h>

void ep9xxx_get_video_timing(uint8_t* vtimebuf,Phdmi_video_timing ptiming)
{
    #define TIME_APPL(a)            (((a[1]&0x0F) << 8) | a[0])		//Active Pixels Per Line
    #define TIME_ALPF(a)            (((a[9]&0x0F) << 8) | a[8])		//Active Lines Per Frame
    #define TIME_HFP(a)             (((a[3]&0x03) << 8) | a[2])		//H sync offset
    #define TIME_HBP(a)             (((a[5]&0x03) << 8) | a[4])		//H Back Porch
    #define TIME_HPW(a)             (((a[7]&0x03) << 8) | a[6])		//H sync pulse width
    #define TIME_VFP(a)             (a[10])							//V sync offset
    #define TIME_VBP(a)             (a[11])							//V Back Porch
    #define TIME_VPW(a)             (a[12] & 0x7F)					//V sync pulse width		
    #define TIME_INT(a)             (a[12] >> 7)
    
    ptiming->pixel_per_line = TIME_APPL(vtimebuf);
    ptiming->pixel_per_frame = TIME_ALPF(vtimebuf);
    ptiming->hfp = TIME_HFP(vtimebuf);
    ptiming->hbp = TIME_HFP(vtimebuf);
    ptiming->hpw = TIME_HPW(vtimebuf);
    ptiming->vfp = TIME_VFP(vtimebuf);
    ptiming->vbp = TIME_VBP(vtimebuf);
    ptiming->vpw = TIME_VPW(vtimebuf);
    ptiming->_int = TIME_INT(vtimebuf);
    
}

#define HFP_CHECK_RANGE      10
#define HFP_720P_60         (110 + HFP_CHECK_RANGE) /* 50 = 440 */
#define HFP_1080_60_30      (88 + HFP_CHECK_RANGE)  /* 1080P60 same as 1080P30 */
#define HFP_1080_50_25      (528 + HFP_CHECK_RANGE) /* 1080P50 same as 1080P25 */
#define HFP_1080P_24        (638 + HFP_CHECK_RANGE) 

vatek_result ep9xxx_get_video_from_timing(uint8_t* pvbuf,uint8_t vcode,Phdmi_video_id* pvideo)
{
    hdmi_video_timing vtime;
    int32_t vid = -1;
    vatek_result nres = vatek_badstatus;
    ep9xxx_get_video_timing(pvbuf,&vtime); 
    
    hdmi_tool_timing_printf(&vtime);
    
    if(vtime.pixel_per_line == 720)
    {
        if(!vtime._int)
        {
            if(vtime.pixel_per_frame == 480)
            {
                if(vcode == VID_480P_59_4_3)vid = VID_480P_59_4_3;
                else if(vcode == VID_480P_59_16_9)vid = VID_480P_59_16_9;
                else  _HAL_LOG(eptool,"480p unmatch vcodec : %d",vcode);
            }
            else if(vtime.pixel_per_frame == 576)
            {
                if(vcode == VID_576P_50_4_3)vid = VID_576P_50_4_3;
                else if(vcode == VID_576P_50_16_9)vid = VID_576P_50_16_9;
                else  _HAL_LOG(eptool,"576p unmatch vcodec : %d",vcode);
            }
            else _HAL_LOG(eptool,"bad frame height : %d",vtime.pixel_per_frame);
        }else _HAL_LOG(eptool,"unsupport i with %d",vtime.pixel_per_line);
    }else if(vtime.pixel_per_line == 1440)
    {
        if(vtime._int)
        {
            if(vtime.pixel_per_frame == 240)
            {
                if(vcode == VID_480I_59_4_3)vid = VID_480I_59_4_3;
                else if(vcode == VID_480I_59_16_9)vid = VID_480I_59_16_9;
                else _HAL_LOG(eptool,"480i unmatch vcodec : %d",vcode);
            }
            else if(vtime.pixel_per_frame == 288)
            {
                if(vcode == VID_576I_50_4_3)vid = VID_576I_50_4_3;
                else if(vcode == VID_576I_50_16_9) vid = VID_576I_50_16_9;
                else _HAL_LOG(eptool,"576i unmatch vcodec : %d",vcode);
            }
            else _HAL_LOG(eptool,"bad frame height : %d",vtime.pixel_per_frame);
        }else  _HAL_LOG(eptool,"1440 unsupport none i with %d",vtime.pixel_per_line);
    }
    else if(vtime.pixel_per_line == 1280)
    {
        if(!vtime._int)
        {
            if(vtime.hfp <= HFP_720P_60)vid = VID_720P_60;
            else vid = VID_720P_50;
        }else _HAL_LOG(eptool,"1280 unsupport i with %d",vtime.pixel_per_line);
    }else if(vtime.pixel_per_line == 1920)
    {
        if(vtime._int)
        {
            if(vtime.hbp < HFP_1080_60_30)vid = VID_1080I_60;
            else if(vtime.hbp < HFP_1080_50_25)vid = VID_1080I_50;
            else _HAL_LOG(eptool,"1080i unsupport hfp : %d",vtime.hfp);
            
        }else
        {
            if(vtime.hfp <= HFP_1080_60_30)
            {
				if(vcode == 0xFF)
				{
					vid = VID_1080P_60;
				}else
				{
					if(vcode == VID_1080P_60)vid = VID_1080P_60;
					else if(vcode == VID_1080P_30)vid = VID_1080P_30;
					else _HAL_LOG(eptool,"1080p 60 nmatch vcodec : %d",vcode);					
				}
            }
            else if(vtime.hfp <= HFP_1080_50_25)
            {
				if(vcode == 0xFF)
				{
					vid = VID_1080P_50;
				}else
				{
					if(vcode == VID_1080P_50)vid = VID_1080P_50;
					else if(vcode == VID_1080P_25)vid = VID_1080P_25;
					else _HAL_LOG(eptool,"1080p50 unmatch vcodec : %d",vcode);	
				}
            }
            else if(vtime.hfp <= HFP_1080P_24)vid = VID_1080P_24;
        }
    }else _HAL_LOG(eptool,"1920 unknown frame width : %d",vtime.pixel_per_line);
    
    if(vid == vcode || vcode == 0xFF)
    {
        *pvideo = hdmi_tool_get_video_id(vid & 0xFF);
        if(*pvideo == NULL)_HAL_LOG(eptool,"unsupport vid : %d",vid);
        else nres = vatek_success;
    }else _HAL_LOG(eptool,"get video format fail : %d - %d",vid,vcode);
    
    return nres;
}

audio_samplerate ep9xxx_get_audio_samplerate(uint8_t value)
{
    value &= 0x7;
    audio_samplerate asamplerate = sample_rate_unknown;
    if(value == EP9XXX_AUDIO_32KHZ)asamplerate = sample_rate_32;
    else if(value == EP9XXX_AUDIO_44_1KHZ)asamplerate = sample_rate_44_1;
    else if(value == EP9XXX_AUDIO_48KHZ)asamplerate = sample_rate_48;
    
    if(asamplerate == sample_rate_unknown)
        _HAL_LOG(eptool,"unsupport audio sample rate : %d",value);
    return asamplerate;
}
