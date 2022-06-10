#ifndef _ADV718X_INTERNAL_
#define _ADV718X_INTERNAL_

#include <driver/av/avsource_adv718x.h>
#include <driver/av/tools/codec_tool.h>

#define ADV718X_DEVICE_ADDR             0x42
#define ADV718X_SLAVE_WRITE             (ADV718X_DEVICE_ADDR)	//ALSB pin high
#define ADV718X_SLAVE_READ              (ADV718X_SLAVE_WRITE | 1)

#define ADV718X_IN_CTRL                 0x00 /* Input control */
#define ADV718X_VD_SEL                  0x01 /* Video selection */
#define ADV7182_IN_SEL_2                0x02
#define ADV718X_OUT_CTRL                0x03 /* Output control */
    #define ADV718X_OUT_EMBENDED        0x4C
#define ADV718X_EXT_OUT_CTRL            0x04 /* Extended output control */
    #define ADV7180_EXT_OUT_656_4       0xC5
    #define ADV7182_EXT_OUT_656_4       0xB5
#define ADV718X_AUTO_DET_EN             0x07 /* Autodetect enable */
#define ADV718X_CONTRAST                0x08 /* Contrast */
#define ADV718X_BRIGHTNESS              0x0A /* Brightness */
#define ADV718X_HUE                     0x0B /* Hue */
#define ADV718X_DEF_Y                   0x0C /* Default value Y */
#define ADV718X_DEF_C                   0x0D /* Default value C */
#define ADV718X_ADI_CTRL                0x0E /* ADI control */
#define ADV718X_POW_MANAGE              0x0F /* Power Management */
    #define ADV718X_PWR_MAN_ON          0x04
    #define ADV718X_PWR_MAN_OFF         0x24
    #define ADV718X_PWR_MAN_RES		    0x80
    
#define ADV718X_STATUS_1                0x10 /* Status 1 */
    #define ADV718X_STATUS1_IN_LOCK		0x01
    #define ADV718X_STATUS1_AUTOD_MASK	0x70
    
#define ADV718X_IDENT                   0x11 /* Ident */
    #define ADV718X_ID_7180_B           0x10 /* 0x1B [initial release] 0x1C [Improved ESD and PDC fix] 0x1D [48-lead and 32-lead devices only] */
    #define ADV718X_ID_7182_B           0x40 /* 0x40 [Prerelease silicon] 0x41 [Released silicon] */
    
#define ADV718X_STATUS_2                0x12 /* Status 2 */
#define ADV718X_STATUS_3                0x13 /* Status 3 */
    #define ADV718X_STATUS3_IS_I        0x40
    #define ADV718X_STATUS3_50FPS       0x04
#define ADV718X_ANAL_CLAMP_CTRL         0x14 /* Analog clamp control */
#define ADV718X_DIGI_CLAMP_CTRL_1       0x15 /* Digital clamp control 1 */
#define ADV718X_SHAP_FILT_CTRL          0x17 /* Shaping filter control */
#define ADV718X_SHAP_FILT_CTRL_2        0x18 /* Shaping filter control 2 */
#define ADV718X_COMB_FILT_CTRL          0x19 /* Comb filter control */
#define ADV718X_ADI_CTRL_2              0x1D /* ADI control 2 */
    #define ADV718X_TRI_LLC             0xC0
    #define ADV718X_EN_LLC              0x40
#define ADV718X_PIX_DELAY_CTRL          0x27 /* Pixel delay control */
#define ADV718X_MISC_GAIN_CTRL          0x2B /* Misc gain control */
#define ADV718X_AGC_MODE_CTRL           0x2C /* AGC mode control */
#define ADV718X_CHRO_GAIN_CTRL_1        0x2D /* Chroma gain control 1 */
#define ADV718X_CHRO_GAIN_CTRL_2        0x2E /* Chroma gain control 2 */
#define ADV718X_LUMA_GAIN_CTRL_1        0x2F /* Luma gain control 1 */
#define ADV718X_LUMA_GAIN_CTRL_2        0x30 /* Luma gain control 2 */
#define ADV718X_VS_FIELD_CTRL_1         0x31 /* Vsync field control 1 */
#define ADV718X_VS_FIELD_CTRL_2         0x32 /* Vsync field control 2 */
#define ADV718X_VS_FIELD_CTRL_3         0x33 /* Vsync field control 3 */
#define ADV718X_HS_POS_CTRL_1           0x34 /* Hsync position control 1 */
#define ADV718X_HS_POS_CTRL_2           0x35 /* Hsync position control 2 */
#define ADV718X_HS_POS_CTRL_3           0x36 /* Hsync position control 3 */
#define ADV718X_POLARITY                0x37 /* Polarity */
#define ADV718X_NTSC_COMB_CTRL          0x38 /* NTSC comb control */
#define ADV718X_PAL_COMB_CTRL           0x39 /* PAL comb control */
#define ADV718X_ADC_CTRL                0x3A /* ADC control */
#define ADV718X_MAN_WIN_CTRL            0x3D /* Manual window control */
#define ADV718X_RESAMPLE_CTRL           0x41 /* Resample control */
#define ADV718X_GEMSTAR_CTRL_1          0x48 /* Gemstar ctrl 1 */
#define ADV718X_GEMSTAR_CTRL_2          0x49 /* Gemstar ctrl 2 */
#define ADV718X_GEMSTAR_CTRL_3          0x4A /* Gemstar ctrl 3 */
#define ADV718X_GEMSTAR_CTRL_4          0x4B /* Gemstar ctrl 4 */
#define ADV718X_GEMSTAR_CTRL_5          0x4C /* Gemstar ctrl 5 */
#define ADV718X_CTI_DNR_CTRL_1          0x4D /* CTI DNR ctrl 1 */
#define ADV718X_CTI_DNR_CTRL_2          0x4E /* CTI DNR ctrl 2 */
#define ADV718X_CTI_DNR_CTRL_4          0x50 /* CTI DNR ctrl 4 */
#define ADV718X_LOCK_CNT                0x51 /* Lock count */

#define ADV7182_CVBS_TRIM               0x52
#define ADV7182_REG_CLAMP_ADJ           0x5A
#define ADV7182_REG_RES_CIR             0x5F
#define ADV7182_REG_DIFF_MODE           0x60

#define ADV718X_FREE_LINE_LEN           0x8F /* Free-Run line length 1 */
#define ADV718X_VBI_INFO                0x90 /* VBI info */
#define ADV718X_WSS_1                   0x91 /* WSS 1 */
#define ADV718X_WSS_2                   0x92 /* WSS 2 */
#define ADV718X_EDTV_1                  0x93 /* EDTV 1 */
#define ADV718X_EDTV_2                  0x94 /* EDTV 2 */
#define ADV718X_EDTV_3                  0x95 /* EDTV 3 */
#define ADV718X_CGMS_1                  0x96 /* CGMS 1 */
#define ADV718X_CGMS_2                  0x97 /* CGMS 2 */
#define ADV718X_CGMS_3                  0x98 /* CGMS 3 */
#define ADV718X_CCAP_1                  0x99 /* CCAP 1 */
#define ADV718X_CCAP_2                  0x9A /* CCAP 2 */
#define ADV718X_LETTERBOX_1             0x9B /* Letterbox 1 */
#define ADV718X_LETTERBOX_2             0x9C /* Letterbox 2 */
#define ADV718X_LETTERBOX_3             0x9D /* Letterbox 3 */
#define ADV718X_CRC_EN                  0xB2 /* CRC enable */
#define ADV718X_ADC_SWITCH_1            0xC3 /* ADC switch 1 */
#define ADV718X_ADC_SWITCH_2            0xC4 /* ADC switch 2 */
#define ADV718X_LETTERBOX_CTRL_1        0xDC /* Letterbox control 1 */
#define ADV718X_LETTERBOX_CTRL_2        0xDD /* Letterbox control 2 */
#define ADV718X_SD_OFFSET_CB            0xE1 /* SD offset Cb */
#define ADV718X_SD_OFFSET_CR            0xE2 /* SD offset Cr */
#define ADV718X_SD_SATURATION_CB        0xE3 /* SD saturation Cb */
#define ADV718X_SD_SATURATION_CR        0xE4 /* SD saturation Cr */
#define ADV718X_NTSC_V_BEGIN            0xE5 /* NTSC V bit begin */
#define ADV718X_NTSC_V_END              0xE6 /* NTSC V bit end */
#define ADV718X_NTSC_F_TOGGLE           0xE7 /* NTSC F bit toggle */
#define ADV718X_PAL_V_BEGIN             0xE8 /* PAL V bit begin */
#define ADV718X_PAL_V_END               0xE9 /* PAL V bit end */
#define ADV718X_PAL_F_TOGGLE            0xEA /* PAL F bit toggle */
#define ADV718X_DRIVE_STR               0xF4 /* Drive strength */
#define ADV718X_IF_COMP_CTRL            0xF8 /* IF comp control */
#define ADV718X_VS_MODE_CTRL            0xF9 /* VS mode control */

static inline int8_t is_adv718x_id_valid(uint8_t id)
{
	id &= 0xF0;
	return (id == ADV718X_ID_7180_B || id == ADV718X_ID_7182_B);
}

#define is_adv_7282(p)  (p->chip_id & ADV718X_ID_7182_B)

#define ADV718X_INPUT_TYPE_UNKNOWN      0xFF
#define ADV718X_INPUT_TYPE_CVBS         0
#define ADV718X_INPUT_TYPE_SVIDEO       1
#define ADV718X_INPUT_TYPE_YPBPR        2

#endif
