#ifndef _EP9XXXX_TOOL_
#define _EP9XXXX_TOOL_

#include <driver/av/tools/hdmi_tool.h>

#define EP9XXX_VTIME_SIZE           13
#define EP9XXX_VINFO_SIZE           15
#define EP9XXX_AINFO_SIZE           7


#define EP9XXX_AUDIO_32KHZ          0
#define EP9XXX_AUDIO_44_1KHZ        1
#define EP9XXX_AUDIO_48KHZ          2

/* CEA-861-D P65 */
#define VINFO_COLORSPACE(a)         ((a[2] >> 5) & 0x03)
#define VINFO_SCAN_MODE(a)          (a[2] & 0x03)
#define VINFO_IS_ASPECT(a)          ((a[2] >> 4) & 0x01)
#define VINFO_IS_VBAR(a)            ((a[2] >> 3) & 0x01)
#define VINFO_IS_HBAR(a)            ((a[2] >> 2) & 0x01)
#define VINFO_COLORIMETRY(a)        ((a[3] >> 6) & 0x03)
#define VINFO_PASPECT(a)            ((a[3] >> 4) & 0x03)
#define VINFO_AASPECT(a)            (a[3] & 0x0F)
#define VINFO_EX_COLORIMETRY(a)	    ((a[4] >> 4) & 0x07)
#define VINFO_QUANT_RANGE(a)        ((a[4] >> 2) & 0x03)
#define VINFO_NUPS(a)               (a[4] & 0x03)
#define VINFO_ITC(a)                ((a[4] >> 7) & 0x01)
#define VINFO_VIDEOCODE(a)          (a[5] & 0x7F)
#define VINFO_YCC_QUANT_RANGE(a)    ((a[6] >> 6) & 0x03)
#define VINFO_CONTENT_TYPE(a)       ((a[6] >> 4) & 0x03)
#define VINFO_PIXEL_REPEAT(a)       (a[6] & 0x0F)
#define VINFO_TOPBAR(a)			    (a[6] | (a[7] << 8))
#define VINFO_BOTTOMBAR(a)          (a[8] | (a[9] << 8))
#define VINFO_LEFTBAR(a)            (a[10] | (a[11] << 8))
#define VINFO_RIGHTBAR(a)           (a[11] | (a[12] << 8))

#ifdef __cplusplus
extern "C" {
#endif

    void ep9xxx_get_video_timing(uint8_t* vtimebuf,Phdmi_video_timing ptiming);
    vatek_result ep9xxx_get_video_from_timing(uint8_t* pvbuf,uint8_t vcode,Phdmi_video_id* pvideo);
    audio_samplerate ep9xxx_get_audio_samplerate(uint8_t value);
    
#ifdef __cplusplus
}
#endif

#endif
