//----------------------------------------------------------------------------
//
// Vision Advance Technology - Software Development Kit
// Copyright (c) 2014-2022, Vision Advance Technology Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//

#ifndef _HALREG_INPUT_
#define _HALREG_INPUT_

/* stream common values define */
/*--------------------------------------------------*/

#define STREAM_MODE_BYPASS				0x00000000
#define STREAM_MODE_SMOOTH				0x00000001
#define STREAM_MODE_REMUX				0x00000002
#define STREAM_MODE_PASSTHROUGH			0x00000003

#define PCR_MODE_DISABLE				0x00000000
#define PCR_MODE_ADJUST					0x00000001
#define PCR_MODE_RETAG					0x00000002

/* stream usb param									*/
/*--------------------------------------------------*/
#define HALREG_USB_INPUT_MODE			0x640
#define HALREG_USB_STREAM_FLAGS			0x641
	#define USB_EN_ASYNCBUFFER			0x00000001
#define HALREG_USB_PCR_MODE				0x642

#define HALREG_USB_ADJUST_TICK			0x643

/* stream ts param									*/
/*--------------------------------------------------*/
#define HALREG_TSIN_MODE				0x640
#define HALREG_TSIN_FLAGS				0x641
	#define TS_PIN_SERIAL               0x00000001
	#define TS_PIN_NEGATIVE_CLK         0x00000002
	#define TS_PIN_NEGATIVE_VALID       0x00000004
	#define TS_PIN_NEGATIVE_SYNC        0x00000008

#define HALREG_TSIN_PCR_MODE			0x642

/*
	mux filter only supported in a3 mux mode
	num == 0 or no current en_tag would take all packets when actived remux at a3 chip
*/

#define HALREG_MUX_FILTER_EN			0x648				
	#define MUX_FILTER_EN_TAG			0xA5F00000
	#define MUX_FILTER_INV_TAG			0x00010000

#define MUX_FILTER_MASK					0xFF
	#define MUX_FILTER_NAX_NUMS			16

/* pid_filter */
#define HALREG_MUX_FILTER_0				0x670
	#define FILTER_EN_REPLACE			0x80000000
	#define FILTER_OFFSET_SUR			16
	#define FILTER_OFFSET_NEW			0
	#define FILTER_NO_REPLACE(sur)		(sur << FILTER_OFFSET_SUR)
	#define FILTER_REPLACE(sur,new)		(FILTER_EN_REPLACE | (sur << FILTER_OFFSET_SUR) | new)

#define HALREG_MUX_FILTER_1				0x671
#define HALREG_MUX_FILTER_2				0x672
#define HALREG_MUX_FILTER_3				0x673
#define HALREG_MUX_FILTER_4				0x674
#define HALREG_MUX_FILTER_5				0x675
#define HALREG_MUX_FILTER_6				0x676
#define HALREG_MUX_FILTER_7				0x677
#define HALREG_MUX_FILTER_8				0x678
#define HALREG_MUX_FILTER_9				0x679
#define HALREG_MUX_FILTER_10			0x67A
#define HALREG_MUX_FILTER_11			0x67B
#define HALREG_MUX_FILTER_12			0x67C
#define HALREG_MUX_FILTER_13			0x67D
#define HALREG_MUX_FILTER_14			0x67E
#define HALREG_MUX_FILTER_15			0x67F

/* stream encoder param								*/
/*--------------------------------------------------*/
#define HALREG_ENCODER_MODE             0x640
	#define ENCMOD_COLORBAR             0x00000000
	#define ENCMOD_BOOTLOGO             0x00000001
	#define ENCMOD_VI_0                 0x00000002

#define HALREG_ENCODER_FLAGS            0x641
	//#define ENC_EN_INTERLACED_FRAME		0x00000002
	#define ENC_EN_DISABLE_DEINTERLACED		0x00000001
	#define ENC_EN_PROGRESSIVE_2_I			0x00000004
	#define ENC_EN_DISABLE_ADTS_CRC			0x00000008
	#define ENC_EN_DROP_FRAME				0x00000010
	#define ENC_DIS_LATENCY_Q				0x00000100
	#define ENC_EN_SW_I2S					0x00000200
	#define ENC_EN_SW_CLK					0x00001000
	#define ENC_EN_Q_COST					0x10000000
	//#define ENC_EN_H264_FULLRUN			0x00000040

#define HALREG_ENCODER_PMTPID			0x2002

#define HALREG_VIDEO_PID				0x2003
#define HALREG_VIDEO_CODEC				0x642				/* defined in hal_common_define.h */
	#define VIDEO_MPEG2					0x00000000
	#define VIDEO_H264					0x00000001
	#define VIDEO_HEVC					0x00000002

#define HALREG_VIDEO_RESOLUTION			0x643				/* defined in hal_common_define.h */
	#define RESOLUTION_480I				0x00000000
	#define RESOLUTION_480P				0x00000001
	#define RESOLUTION_576I				0x00000002
	#define RESOLUTION_576P				0x00000003
	#define RESOLUTION_720P				0x00000004
	#define RESOLUTION_1080I			0x00000005
	#define RESOLUTION_1080P			0x00000006
	#define RESOLUTION_UHD				0x00000007

#define HALREG_VIDEO_FRAMERATE			0x644				/* defined in hal_common_define.h */
	#define FRAMERATE_23_97				0x00000000
	#define FRAMERATE_24				0x00000001
	#define FRAMERATE_25				0x00000002
	#define FRAMERATE_29_97				0x00000003
	#define FRAMERATE_30				0x00000004
	#define FRAMERATE_50				0x00000005
	#define FRAMERATE_59_94				0x00000006
	#define FRAMERATE_60				0x00000007

#define HALREG_VIDEO_ASPECTRATE			0x645				/* defined in hal_common_define.h */
	#define ASPECTRATE_4_3				0x00000000
	#define ASPECTRATE_16_9				0x00000001

#define HALREG_AUDIO_PID				0x2004
#define HALREG_AUDIO_CODEC				0x646				/* defined in hal_common_define.h */
	#define AUDIO_MP1_L2				0x00000000
	#define AUDIO_AAC_LC_ADTS			0x00000001
	#define AUDIO_AC_3					0x00000002	
	#define AUDIO_AAC_LC_LATM			0x00000003
	
#define HALREG_AUDIO_CHANNEL			0x647				/* defined in hal_common_define.h */
	#define CHANNEL_MUTE				0x00000000
	#define CHANNEL_STEREO				0x00000001
	#define CHANNEL_MONO_L				0x00000002
	#define CHANNEL_MONO_R				0x00000003
	#define CHANNEL_STEREO_MONO_L		0x00000004
	#define CHANNEL_STEREO_MONO_R		0x00000005
	
#define HALREG_AUDIO_SAMPLERATE			0x648				/* defined in hal_common_define.h */
	#define SAMPLERATE_32KHZ			0x00000000
	#define SAMPLERATE_44_1KHZ			0x00000001
	#define	SAMPLERATE_48KHZ			0x00000002

#define HALREG_ENCODER_TAG				0x649

/* quality_param */

#define HALREG_QUALITY_RCMODE			0x650				/* defined in hal_common_define.h */
	#define QUALITY_RC_VBR				0x00000001
	#define QUALITY_FIXED_Q             0x00000002
	#define QUALITY_AUTO				0x00000003

#define HALREG_QUALITY_GOP				0x651
#define HALREG_QUALITY_MINQ				0x652
#define HALREG_QUALITY_MAXQ				0x653
#define HALREG_QUALITY_BITRATE			0x654
#define HALREG_QUALITY_LATENCY			0x655

/* vi_parm */

#define HALREG_VI_0_FLAGS				0x660				/* defined in hal_common_define.h */
	#define VI_FWFUN_MAP_MASK           0xFFFF0000	
	#define VI_HWREG_MAP_MASK           0x0000FFFF			/* map width vi_0 register */
	#define VI_BUSWIDTH_16              0x00000010
	#define VI_SEPARATED_SYNC           0x00000040
	#define VI_CLK_INVERSE              0x00000200
	#define VI_HSYNC_INVERSE            0x00000400
	#define VI_VSYNC_INVERSE            0x00001000
	#define VI_FIELDID_SIGNAL           0x01000000
	#define VI_EXT_HALF_FPS             0x10000000
	#define VI_FIELD_INVERSE			0x20000000
	#define VI_DISABLE_ERRCHK           0x40000000
	#define VI_PCRCLK_ENC               0x80000000

#define HALREG_VI_0_PIXELCLOCK			0x661
#define HALREG_VI_0_OFFSET_X			0x662
#define HALREG_VI_0_OFFSET_Y			0x663

#endif

