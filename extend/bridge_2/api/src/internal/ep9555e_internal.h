#ifndef _EP9555E_INTERNAL_
#define _EP9555E_INTERNAL_

#include <driver/av/avsource_ep9555e.h>
#include <driver/av/tools/ep9xxx_tool.h>

#if 0
#define EP9555E_CHIP_ADDR					        0x3C
#define EP9555E_SYS_ADDR					        0x34
#define EP9555E_ISP_ADDR					        0x28
#endif

#define EP9555E_CHIP_ADDR					        0x78
#define EP9555E_SYS_ADDR					        0x68
#define EP9555E_ISP_ADDR					        0x50

#define EP9555E_POWER_STATUS						0x0000

/* chip function */
#define EP9555E_INT1						        0x0100
	#define EP9555E_INT1_TIM_CH				        0x80
	#define EP9555E_INT1_AVMC				        0x40
	#define EP9555E_INT1_AVMS				        0x20
	#define EP9555E_INT1_ACP_CH				        0x10
	#define EP9555E_INT1_SEL1_CH			        0x08
	#define EP9555E_INT1_ADO_CH				        0x02
	#define EP9555E_INT1_AVI_CH				        0x01

#define EP9555E_SYS_STATUS0					        0x0200
	#define EP9555E_SYS_STATUS0_AVMUTE_F	        0x20
	#define EP9555E_SYS_STATUS0_HDMI		        0x10
	#define EP9555E_SYS_STATUS0_AUTHEN		        0x08
	#define EP9555E_SYS_STATUS0_DECRYPT		        0x04
	#define EP9555E_SYS_STATUS0_DST_double	        0x02
	#define EP9555E_SYS_STATUS0_LAYOUT		        0x01

#define EP9555E_SYS_STATUS1					        0x0201
	#define EP9555E_SYS_STATUS1_LINK_ON		        0x80
	#define EP9555E_SYS_STATUS1_DE_VALID	        0x40
	#define EP9555E_SYS_STATUS1_PWR5V_P2	        0x20
	#define EP9555E_SYS_STATUS1_PWR5V_P1	        0x10
	#define EP9555E_SYS_STATUS1_PWR5V_P0	        0x08
	#define EP9555E_SYS_STATUS1_CD			        0x07
    
#define is_ep9555e_source_valid(status)             ((status & EP9555E_SYS_STATUS1_LINK_ON) && \
                                                     (status & EP9555E_SYS_STATUS1_DE_VALID))

#define EP9555E_V_STATUS0					        0x0300
	#define EP9555E_V_STATUS0_VIN_FREQ				0x30
	#define EP9555E_V_STATUS0_VIN_FMT				0x0F
	#define EP9555E_V_STATUS0_VIN_FMT_YCC			0x08
	#define EP9555E_V_STATUS0_VIN_FMT_ITU709		0x04
	#define EP9555E_V_STATUS0_VIN_FMT_Extended		0x02
	#define EP9555E_V_STATUS0_VIN_FMT_Full_Range	0x02
	#define EP9555E_V_STATUS0_VIN_FMT_422			0x01
	#define EP9555E_V_STATUS0_VIN_FMT_RGB			0x00
    
#define EP9555E_V_TIMING                            0x0301

#define EP9555E_A_STATUS0					        0x0400
	#define EP9555E_A_STATUS0_DST_AUDIO		        0x40
	#define EP9555E_A_STATUS0_HBR_AUDIO		        0x20
	#define EP9555E_A_STATUS0_DSD_AUDIO		        0x10
	#define EP9555E_A_STATUS0_STD_AUDIO		        0x08
	#define EP9555E_A_STATUS0_SAMP_FREQ		        0x07

#define EP9555E_SYNC_TIMING_H                       0x0600
#define EP9555E_SYNC_TIMING_V                       0x0700
#define EP9555E_AVI_INFO					        0x0800
#define EP9555E_AVI_INFO_VIC					    0x0804

#define EP9555E_INT_CNTL                            0x2000
#define EP9555E_POL_CNTL					        0x2001
	#define EP9555E_POL_CNTL_MUTE_POL_Low		    0x80
	#define EP9555E_POL_CNTL_HOTPLUG_POL_High	    0x40
	#define EP9555E_POL_CNTL_OCD				    0x30
	#define EP9555E_POL_CNTL_INT_MODE_Level		    0x08
	#define EP9555E_POL_CNTL_INT_POL_High		    0x04
	#define EP9555E_POL_CNTL_VS_POL_Low			    0x02
	#define EP9555E_POL_CNTL_HS_POL_Low			    0x01
    
#define EP9555E_VOUT_CNTL                           0x2002
#define EP9555E_AOUT_CNTL                           0x2003
#define EP9555E_AOUT_CHMAP_CNTL                     0x2004 
    

#define EP9555E_SYS_CNTL					        0x2100
	#define	SYS_CNTL_DDC_DIS				        0x80
	#define SYS_CNTL_LOCK_SLOW				        0x40
	#define SYS_CNTL_PORT_SEL				        0x30
	#define SYS_CNTL_PORT_SEL__P1			        0x00
	#define SYS_CNTL_PORT_SEL__P2			        0x10
	#define SYS_CNTL_PORT_SEL__P3			        0x20
	#define SYS_CNTL_CEC_EN					        0x08
	#define SYS_CNTL_EQ_BOOST				        0x04
	#define SYS_CNTL_PD_HDMI				        0x02
	#define SYS_CNTL_PD_TOT					        0x01
    
#define EP9555E_MUTE_CNTL                           0x2200

#define EP9555E_OUTFMT_CNTL                         0x2300
#define EP9555E_MANUAL_VFMT_Control			        0x2301
	#define EP9555E_MANUAL_VFMT_OUT422		        0x80
	#define EP9555E_MANUAL_VFMT_IN422		        0x40
	#define EP9555E_MANUAL_VFMT_OUT_YCC		        0x20
	#define EP9555E_MANUAL_VFMT_IN_YCC		        0x10
	#define EP9555E_MANUAL_VFMT_RGB_range	        0x08
	#define EP9555E_MANUAL_VFMT_CS			        0x04
	#define EP9555E_MANUAL_VFMT_PR                  0x03
	#define EP9555E_MANUAL_VFMT_PR__repeat0	        0x00
	#define EP9555E_MANUAL_VFMT_PR__repeat1	        0x01
	#define EP9555E_MANUAL_VFMT_PR__repeat2	        0x02
	#define EP9555E_MANUAL_VFMT_PR__repeat3	        0x03

#define EP9555E_OUTPUT_DISABLE_CNTL			        0x2400
	#define OUTPUT_DISABLE_CNTL_SPDIF_DIS	        0x80
	#define OUTPUT_DISABLE_CNTL_IIS_DIS		        0x40
	#define OUTPUT_DISABLE_CNTL_VOUT_DIS	        0x10
	#define OUTPUT_DISABLE_CNTL_DAC_EN		        0x04 // Not avaliable in PJ2
	#define OUTPUT_DISABLE_CNTL_SOG_EN		        0x02 // Not avaliable in PJ2
	#define OUTPUT_DISABLE_CNTL_I2S_PCM		        0x01 // Enable I2S only when PCM

#define EP9555E_SCALE_ACTIVE_H				        0x2500
#define EP9555E_SCALE_ACTIVE_V 				        0x2502
#define EP9555E_SCALE_H_DELAY				        0x2504
#define EP9555E_SCALE_H_WIDTH				        0x2506
#define EP9555E_SCALE_V_DELAY				        0x2508
#define EP9555E_SCALE_V_WIDTH				        0x2509

#define EP9555E_ADDR_W(type)	                    (type)
#define EP9555E_ADDR_R(type)	                    (EP9555E_ADDR_W(type) | 1)

/* system function*/
#define EP9555E_EDID_DATAREG		                0x0000		//0x00~0xFF	(256 bytes)

#endif
