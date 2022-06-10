#ifndef _SX1804A_INTERNAL_
#define _SX1804A_INTERNAL_

#include <driver/av/avsource_sx1804a.h>
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

	typedef enum _h1_output_mode
	{
		h1_output_bypass = 0,
		h1_output_i_to_p = 1,
	}h1_output_mode;

#endif

//#define SX1804A_I2C_ADDR                        0xE0
//#define SX1804A_I2C_WRITE                       (SX1804A_I2C_ADDR)
//#define SX1804A_I2C_READ                        (SX1804A_I2C_WRITE | 0x01)
//
//
//#define SX1804A_VENDER_ID 	                    0x422C
//#define SX1804A_CHIP_ID		                    0x0418
///* SX1804A Register Map */
////		REGISTER
//#define SX1804A_HDMI_FLAG			            0x0180
//	#define SX1804A_HDMI_FLAG_A_CH				0x08
//	#define SX1804A_HDMI_FLAG_V_CH				0x04
//	#define SX1804A_HDMI_FLAG_PCLK_1001			0x01
//
//#define SX1804A_SYS_STATUS			            0x0200
//	#define SX1804A_SYS_STATUS_AUTH				0x08
//	#define SX1804A_SYS_STATUS_DECRYPT			0x04
//
//#define SX1804A_SYS_STATUS2			            0x0201
//	#define SX1804A_SYS_STATUS2_LINK			0x80
//	#define SX1804A_SYS_STATUS2_DE				0x40
//	#define SX1804A_SYS_STATUS2_PWR5V			0x10
//
//#define SX1804A_V_STATUS			            0x0301
//	#define SX1804A_V_STATUS_VTIME				0xFF
//
//#define SX1804A_A_STATUS			            0x0400
//	#define SX1804A_A_STATUS_DST_ADO			0x40
//	#define SX1804A_A_STATUS_HBR_ADO			0x20
//	#define SX1804A_A_STATUS_DSD_ADO			0x10
//	#define SX1804A_A_STATUS_STD_ADO			0x08
//	#define SX1804A_A_STATUS_SAMP_FREQ			0x07
//
//#define SX1804A_AVI_INFO			            0x0804
//	#define SX1804A_AVI_INFO_VIC				0xFF
//
//#define SX1804A_POL_CNT				            0x2001
//	#define SX1804A_POL_CNT_VS					0x02
//	#define SX1804A_POL_CNT_HS					0x01
//
//#define SX1804A_V_OUT				            0x2002
//	#define SX1804A_V_OUT_DCLK_POL				0x02
//
//#define SX1804A_MAN_VFMT			            0x2301
//	#define SX1804A_MAN_VFMT_OUT422				0x80
//	#define SX1804A_MAN_VFMT_IN422				0x40
//	#define SX1804A_MAN_VFMT_OUT_YCC			0x20
//	#define SX1804A_MAN_VFMT_IN_YCC				0x10
//	#define SX1804A_MAN_VFMT_CS					0x04
//
//#define SX1804A_SCALE_H_OUT_H		            0x2500
//	#define SX1804A_SCALE_HOUT_H				0x1F
//
//#define SX1804A_SCALE_H_OUT_L		            0x2501
//#define SX1804A_SCALE_V_OUT_H		            0x2502
//	#define SX1804A_SCALE_VOUT_H				0x1F
//
//#define SX1804A_SCALE_V_OUT_L		            0x2503
//
//#define SX1804A_SCALE_H_DELAY_L		            0x2504
//	#define SX1804A_SCALE_HDELAY_L				0x03
//
//#define SX1804A_SCALE_H_DELAY_H		            0x2505
//
//
//#define SX1804A_V_MODE				            0x3080
//	#define SX1804A_V_MODE_VOUT_EN				0x80
//    #define SX1804A_V_MODE_COLOR_RGB            0x00
//    #define SX1804A_V_MODE_COLOR_YCRCB_422      0x01
//    #define SX1804A_V_MODE_COLOR_YCRCB_444      0x02
//
//#define SX1804A_V_IN_DEF			            0x30A0
//	#define SX1804A_V_IN_DEF_SCALE_BYPASS		0x04
//	#define SX1804A_V_IN_DEF_V_INVERT			0x02
//	#define SX1804A_V_IN_DEF_H_INVERT			0x01
//
//#define SX1804A_V_CNT_NUM			            0x30A5
//#define SX1804A_V_SCALE				            0x30AA
//#define SX1804A_V_VTOTAL_L			            0x30B6
//#define SX1804A_V_VTOTAL_H			            0x30B7
//#define SX1804A_V_BT656				            0x30B8
//#define SX1804A_V_CNT_BLANK			            0x30B9
//
//#define SX1804A_A_INTERFACE			            0x3040
//#define SX1804A_A_MUTE1				            0x3041
//#define SX1804A_A_MUTE2				            0x3042
//#define SX1804A_A_CNT1				            0x3044
//#define SX1804A_A_CNT2				            0x3045
//#define SX1804A_A_ACR_CTS_L			            0x30C6
//#define SX1804A_A_ACR_CTS_H			            0x30C7
//#define SX1804A_A_ACR_N_L			            0x30C8
//#define SX1804A_A_ACR_N_H			            0x30C9
//#define SX1804A_A_CLK_DIV			            0x30CA
//#define SX1804A_A_FREQ_CNT			            0x30CC
