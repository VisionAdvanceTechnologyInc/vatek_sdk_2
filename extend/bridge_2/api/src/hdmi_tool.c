#include <driver/av/tools/hdmi_tool.h>

Phdmi_video_id hdmi_tool_get_video_id(uint8_t vid)
{
    int32_t i = 0;
    for(i = 0;i < HDMI_VIDEO_ID_NUMS;i++)
    {
        if(hdmi_video_table[i].vid == vid)
            return (Phdmi_video_id)&hdmi_video_table[i];
    }
    return NULL;
}

Phdmi_video_id hdmi_tool_get_video_id_by_info(video_resolution resolution,video_framerate framerate,video_aspect_rate aspect)
{
    int32_t i = 0;
    for(i = 0;i < HDMI_VIDEO_ID_NUMS;i++)
    {
        if(hdmi_video_table[i].resolution == resolution && 
            hdmi_video_table[i].framerate == framerate &&
            hdmi_video_table[i].aspectratio == aspect)
            return (Phdmi_video_id)&hdmi_video_table[i];
    }
    return NULL;  
}

void hdmi_tool_change_base_clock(bsource_baseclk bclk,Pbridge_video_info psource)
{   
    if(bclk == sbaseclk_1001)
    {
        if(psource->framerate == framerate_24)psource->framerate = framerate_23_97;
        else if(psource->framerate == framerate_30)
        {
            psource->framerate = framerate_29_97;
            if(psource->resolution == resolution_1080p)psource->pixelclk = 74176;
            else if(psource->resolution == resolution_1080i)psource->pixelclk = 74176;
        }else if(psource->framerate  == framerate_60)
        {
            psource->framerate = framerate_59_94;
            if(psource->resolution == resolution_1080p)psource->pixelclk = 148351;
            else if(psource->resolution == resolution_1080i)psource->pixelclk = 74176;
            else if(psource->resolution == resolution_720p)psource->pixelclk = 74176;
        }
    }else
    {
        if(psource->framerate == framerate_23_97)psource->framerate = framerate_24;
        else if(psource->framerate == framerate_29_97)
        {
            psource->framerate = framerate_30;
            if(psource->resolution == resolution_1080p)psource->pixelclk = 74250;
            else if(psource->resolution == resolution_1080i)psource->pixelclk = 74250;
        }else if(psource->framerate  == framerate_59_94)
        {
            psource->framerate = framerate_60;
            if(psource->resolution == resolution_1080p)psource->pixelclk = 148500;
            else if(psource->resolution == resolution_1080i)psource->pixelclk = 74250;
            else if(psource->resolution == resolution_720p)psource->pixelclk = 74250;
        }        
    }
}

void hdmi_tool_get_timing(Phdmi_video_id pvid,Phdmi_video_timing ptiming)
{
    if(pvid->resolution == resolution_1080i || pvid->resolution == resolution_1080p)
    {
        ptiming->pixel_per_line = 1920;
        ptiming->pixel_per_frame = 1080;
    }else if(pvid->resolution == resolution_720p)
    {
        ptiming->pixel_per_line = 1280;
        ptiming->pixel_per_frame = 720;
    }else if(pvid->resolution == resolution_480i || pvid->resolution == resolution_480p)
    {
        ptiming->pixel_per_line = 720;
        ptiming->pixel_per_frame = 480;
    }else if(pvid->resolution == resolution_576i || pvid->resolution == resolution_576p)
    {
        ptiming->pixel_per_line = 720;
        ptiming->pixel_per_frame = 576;
    }else _HAL_ERR(hdmi,"unhandle resolution : %d",pvid->resolution);
    
    if(is_resolution_interlaced(pvid->resolution))
    {
        ptiming->pixel_per_frame >>= 1;
        ptiming->_int = 1;
    }
    
    ptiming->hfp = pvid->hfp;
    ptiming->hbp = pvid->hbp;
    ptiming->hpw = pvid->hpw;
    ptiming->vfp = pvid->vfp;
    ptiming->vbp = pvid->vbp;
    ptiming->vpw = pvid->vpw;
    
}

void hdmi_tool_timing_printf(Phdmi_video_timing ptiming)
{
    _HAL_LOG(hdmi,"printf hdmi timing");
    printf("    APPL : %d\r\n",ptiming->pixel_per_line);
    printf("    ALPF : %d\r\n",ptiming->pixel_per_frame);
    printf("    HFP  : %d\r\n",ptiming->hfp);
    printf("    HBP  : %d\r\n",ptiming->hbp);
    printf("    HPW  : %d\r\n",ptiming->hpw);
    printf("    VFP  : %d\r\n",ptiming->vfp);
    printf("    VPW  : %d\r\n",ptiming->vpw);
    printf("    VBP  : %d\r\n",ptiming->vbp);
    printf("    INT  : %d\r\n",ptiming->_int);
}
