#ifndef _HDMI_TOOL_
#define _HDMI_TOOL_

#include <driver/bridge_driver.h>

static const uint8_t HDMI_EDID_DEFAULT[] = 
{
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,   // address 0x00
	0x58, 0x34, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00,   //
	0x00, 0x19, 0x01, 0x03, 0x80, 0x73, 0x41, 0x78,   // address 0x10
	0x2A, 0x7C, 0x11, 0x9E, 0x59, 0x47, 0x9B, 0x27,   //
	0x10, 0x50, 0x54, 0x00, 0x00, 0x00, 0x81, 0x80,   // address 0x20
	0x90, 0x40, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,   //
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A,   // address 0x30
	0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,   //
	0x45, 0x00, 0x10, 0x09, 0x00, 0x00, 0x00, 0x1E,   // address 0x40
	0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10,   //
	0x10, 0x3E, 0x96, 0x00, 0x04, 0x03, 0x00, 0x00,   // address 0x50
	0x00, 0x18, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x45,   //
	0x50, 0x2D, 0x56, 0x42, 0x54, 0x32, 0x30, 0x31,   // address 0x60
	0x2D, 0x52, 0x58, 0x0A, 0x00, 0x00, 0x00, 0xFD,   //
	0x00, 0x3B, 0x3D, 0x0F, 0x2E, 0x0F, 0x00, 0x0A,   // address 0x70
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x9D,   //
	0x02, 0x03, 0x22, 0x72, 0x4F, 0x10, 0x84, 0x05,   // address 0x80
	0x13, 0x14, 0x02, 0x03, 0x11, 0x12, 0x06, 0x07,   //
	0x15, 0x16, 0x1F, 0x20, 0x23, 0x09, 0x07, 0x07,   // address 0x90
	0x83, 0x01, 0x00, 0x00, 0x65, 0x03, 0x0C, 0x00,   //
	0x10, 0x00, 0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0,   // address 0xA0
	0x1E, 0x20, 0x6E, 0x28, 0x55, 0x00, 0x10, 0x09,   //
	0x00, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x80, 0x18,   // address 0xB0
	0x71, 0x1C, 0x16, 0x20, 0x58, 0x2C, 0x25, 0x00,   //
	0x10, 0x09, 0x00, 0x00, 0x00, 0x9E, 0x02, 0x3A,   // address 0xC0
	0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,   //
	0x45, 0x00, 0x10, 0x09, 0x00, 0x00, 0x00, 0x1E,   // address 0xD0
	0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10,   //
	0x10, 0x3E, 0x96, 0x00, 0x04, 0x03, 0x00, 0x00,   // address 0xE0
	0x00, 0x18, 0x8C, 0x0A, 0xD0, 0x90, 0x20, 0x40,   //
	0x31, 0x20, 0x0C, 0x40, 0x55, 0x00, 0x04, 0x03,   // address 0xF0
	0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x0C,   //	
};

#define HDMI_EDID_LEN		256

typedef struct _hdmi_video_id
{
    uint8_t vid;
    uint8_t represent;
    video_resolution resolution;
    video_framerate framerate;
    video_aspect_rate aspectratio;
    uint32_t pixelclk;
    uint16_t hfp;
    uint16_t hbp;
    uint16_t hpw;
    uint8_t vfp;
    uint8_t vbp;
    uint8_t vpw;
}hdmi_video_id,*Phdmi_video_id;

#define PIXEL_REPETITION_0  0x00
#define PIXEL_REPETITION_2  0x01
#define PIXEL_REPETITION_4  0x03

#define is_current_repetition(n)    (n == PIXEL_REPETITION_0 || n == PIXEL_REPETITION_2 || n == PIXEL_REPETITION_4)


/* CEA-861-D P65 video info */

#define Y_COLOR_RGB         0
#define Y_COLOR_YCC_422     1
#define Y_COLOR_YCC_444     2

#define M_ASPECT_4_3        1
#define M_ASPECT_16_9       2

#define C_COLORSPACE_ITU601 1
#define C_COLORSPACE_ITU709 2
#define C_COLORSPACE_EXT    3

#define Q_COLORRANGE_LIMIT  1
#define Q_COLORRANGE_FULL   2

#define EC_COLORSPACE_CC601 0
#define EC_COLORSPACE_CC709 1

/* 
CEA-861  
    3 Format Note: For 60 Hz formats, displays respond automatically to either 60 Hz or 59.94 Hz (same format with 
    slightly different clocks). The 25.2 MHz pixel frequency value is within the ± 0.5% (allowed in the VESA DMT [10]) of 
    25.175 MHz. The 480p formats and the 480i formats are typically 59.94 Hz, and the HDTV formats are typically 60 
    Hz. 
*/

#define VID_480P_59_4_3     0x02
#define VID_480P_59_16_9    0x03   
#define VID_480I_59_4_3     0x06
#define VID_480I_59_16_9    0x07

#define VID_576P_50_4_3     0x11
#define VID_576P_50_16_9    0x12
#define VID_1080I_50        0x14
#define VID_576I_50_4_3     0x15
#define VID_576I_50_16_9    0x16

#define VID_720P_60         0x04
#define VID_1080I_60        0x05
#define VID_720P_50         0x13

#define VID_1080P_60        0x10
#define VID_1080P_50        0x1F
#define VID_1080P_25        0x21
#define VID_1080P_30        0x22
#define VID_1080P_24        0x20

static const hdmi_video_id hdmi_video_table[] =
{
    {VID_480P_59_4_3    ,PIXEL_REPETITION_0,resolution_480p ,framerate_59_94,aspectrate_4_3 ,27000,16,62,60,9,20,6,},
    {VID_480P_59_16_9   ,PIXEL_REPETITION_0,resolution_480p ,framerate_59_94,aspectrate_16_9,27000,16,62,60,9,20,6,},
    {VID_720P_60        ,PIXEL_REPETITION_0,resolution_720p ,framerate_60   ,aspectrate_16_9,74250,110,40,220,5,20,5,},
    {VID_1080I_60       ,PIXEL_REPETITION_0,resolution_1080i,framerate_60   ,aspectrate_16_9,74250,88,44,148,2,15,5,},
    {VID_480I_59_4_3    ,PIXEL_REPETITION_2,resolution_480i ,framerate_60   ,aspectrate_4_3 ,27000,38,124,114,4,15,3,},
    {VID_480I_59_16_9   ,PIXEL_REPETITION_2,resolution_480i ,framerate_60   ,aspectrate_16_9,27000,38,124,114,4,15,3,},
    {VID_1080P_60       ,PIXEL_REPETITION_0,resolution_1080p,framerate_60   ,aspectrate_16_9,148500,88,44,148,4,36,5,},
    {VID_576P_50_4_3    ,PIXEL_REPETITION_0,resolution_576p ,framerate_50   ,aspectrate_4_3 ,27000,16,64,68,5,39,5,},
    {VID_576P_50_16_9   ,PIXEL_REPETITION_0,resolution_576p ,framerate_50   ,aspectrate_16_9,27000,16,64,68,5,39,5,},
    {VID_720P_50        ,PIXEL_REPETITION_0,resolution_720p ,framerate_50   ,aspectrate_16_9,74250,440,40,220,5,20,5,},
    {VID_1080I_50       ,PIXEL_REPETITION_0,resolution_1080i,framerate_50   ,aspectrate_16_9,74250,528,44,148,2,15,5,},
    {VID_576I_50_4_3    ,PIXEL_REPETITION_2,resolution_576i ,framerate_50   ,aspectrate_4_3 ,27000,24,126,138,2,19,3,},
    {VID_576I_50_16_9   ,PIXEL_REPETITION_2,resolution_576i ,framerate_50   ,aspectrate_16_9,27000,24,126,138,2,19,3,},
    {VID_1080P_50       ,PIXEL_REPETITION_0,resolution_1080p,framerate_50   ,aspectrate_16_9,148500,528,44,148,4,36,5,},
    {VID_1080P_24       ,PIXEL_REPETITION_0,resolution_1080p,framerate_24   ,aspectrate_16_9,74250,638,44,148,4,36,5,},
    {VID_1080P_25       ,PIXEL_REPETITION_0,resolution_1080p,framerate_25   ,aspectrate_16_9,74250,528,44,148,4,36,5,},
    {VID_1080P_30       ,PIXEL_REPETITION_0,resolution_1080p,framerate_30   ,aspectrate_16_9,74250,88,44,148,4,36,5,},
};

#define HDMI_VIDEO_ID_NUMS  (sizeof(hdmi_video_table)/sizeof(hdmi_video_id))
    
typedef struct _hdmi_video_timing
{
    uint16_t pixel_per_line;
    uint16_t pixel_per_frame;
    uint16_t hfp;
    uint16_t hbp;
    uint16_t hpw;
    uint8_t vfp;
    uint8_t vbp;
    uint8_t vpw;
    uint8_t _int;
}hdmi_video_timing,*Phdmi_video_timing;


#ifdef __cplusplus
extern "C" {
#endif

    Phdmi_video_id hdmi_tool_get_video_id(uint8_t vid);
    Phdmi_video_id hdmi_tool_get_video_id_by_info(video_resolution resolution,video_framerate framerate,video_aspect_rate aspect);
    void hdmi_tool_get_timing(Phdmi_video_id pvid,Phdmi_video_timing ptiming);

    void hdmi_tool_timing_printf(Phdmi_video_timing ptiming);

    void hdmi_tool_change_base_clock(bsource_baseclk bclk,Pbridge_video_info psource);

#ifdef __cplusplus
}
#endif


#endif

