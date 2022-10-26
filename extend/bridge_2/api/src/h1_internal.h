#ifndef _H1_INTERNAL_
#define _H1_INTERNAL_

#include <driver/av/avsource_h1.h>
#include <driver/av/tools/hdmi_tool.h>

    #define H1_I2C_ADDR                         0xE0
    #define H1_I2C_READ                         (H1_I2C_ADDR | 0x01)
    #define H1_I2C_WRITE                        (H1_I2C_ADDR)

    /* H1 Register Map */
    #define H1_POWER                            0x0100
        #define H1_POWER_ON                         0x01

    #define H1_INT_EN                           0x0101
    #define H1_INT_STATUS                       0x0102
    #define H1_INT_CNTL                         0x0103
        #define H1_INT_CNTL_INT_MODE                0x08
        #define H1_INT_CNTL_VLP                     0x02
        #define H1_INT_CNTL_HLP                     0x01
    
    #define H1_HDMI_FLAG                        0x0104
        #define H1_HDMI_FLAG_OUT_INTERLACED         0x80
        #define H1_HDMI_FLAG_BYPASS_MODE            0x40
        #define H1_HDMI_FLAG_PCLK_1001              0x01          /** only read */

    #define H1_SYS_STATUS1                      0x0105
        #define H1_SYS_STATUS1_AUTH                 0x08
        #define H1_SYS_STATUS1_DECRYPT              0x04
    #define H1_SYS_STATUS2                      0x0106
        #define H1_SYS_STATUS2_LINK                 0x80
        #define H1_SYS_STATUS2_DE                   0x40
        #define H1_SYS_STATUS2_HPD                  0x10

    #define H1_MUTE_CNTL                        0x0107
        #define H1_MUTE_CNTL_AUDIO                  0x02
        #define H1_MUTE_CNTL_VIDEO                  0x01

    #define H1_OUT_FMT                          0x0201
        #define H1_OUT_FMT_422                      0x80
        #define H1_OUT_FMT_YCC                      0x20
        #define H1_OUT_FMT_CS709                    0x04
        #define H1_OUT_FMT_PR2                      0x01
    #define H1_VOUT_CFG                         0x0202
        #define H1_VOUT_CFG_16BITS                  0x80
        #define H1_VOUT_CFG_EMBEDDED                0x20
		#define V_CLK_OUT_INV						0x02
        #define H1_VOUT_CFG_FRVS                    0x01

    #define H1_VIN_FMT                          0x0203
        #define H1_VIN_FMT_YUV                      0x08
        #define H1_VIN_FMT_CS709                    0x04
        #define H1_VIN_FMT_FR                       0x02
        #define H1_VIN_FMT_422                      0x01
    #define H1_VIN_VIC                          0x0204

    #define H1_VOUT_ACTIVE_H				    0x0205      /** 13-bits */
    #define H1_VOUT_ACTIVE_V 				    0x0207      /** 13-bits */
    #define H1_VOUT_H_DELAY				        0x0209      /** 10-bits */
    #define H1_VOUT_H_WIDTH				        0x020b      /** 12-bits */
    #define H1_VOUT_V_DELAY				        0x020d
    #define H1_VOUT_V_WIDTH				        0x020e

    #define H1_OUT_CNTL                         0x0300
        #define H1_OUT_CNTL_DIS_SPDIF               0x80
        #define H1_OUT_CNTL_DIS_IIS                 0x40
        #define H1_OUT_CNTL_DIS_VOUT                0x10

        #define H1_OUT_CNTL_DIS_ALL                 (H1_OUT_CNTL_DIS_SPDIF | H1_OUT_CNTL_DIS_IIS | H1_OUT_CNTL_DIS_VOUT)

    #define H1_AOUT_CFG                         0x0301

    #define H1_AIN_STATUS                       0x0302
        #define H1_AIN_STATUS_DST_DET               0x40
        #define H1_AIN_STATUS_HBR_DET               0x20
        #define H1_AIN_STATUS_DSD_DET               0x10
        #define H1_AIN_STATUS_STD_DET               0x08
        #define H1_AIN_STATUS_SAMP_FREQ             0x07

    #define H1_VIN_ACTIVE_H				        0x0400      /** 13-bits */
    #define H1_VIN_HFB       				    0x0402      /** 12-bits */
    #define H1_VIN_HBB  				        0x0404      /** 12-bits */
    #define H1_VIN_HPLUSE				        0x0406      /** 10-bits */
    #define H1_VIN_ACTIVE_V				        0x0410      /** 13-bits */
    #define H1_VIN_VFB       				    0x0412      /** 8-bits */
    #define H1_VIN_VBB  				        0x0413      /** 8-bits */
    #define H1_VIN_VPLUSE				        0x0414      /** 8-bits */
        #define H1_VIN_VPLUSE_PLUS                  0x7F    /** 7-bits */
        #define H1_VIN_VPLUSE_INT                   0x80    /** 1-bit  */
    
    #define H1_AVI_INFO                         0x0500
    #define H1_AUDIO_INFO                       0x0600

	#define H1_VER1								0x0f00
	#define H1_VER2								0x0f01

	typedef enum _h1_output_mode
	{
		h1_output_bypass = 0,
		h1_output_i_to_p = 1,
	}h1_output_mode;

#endif
