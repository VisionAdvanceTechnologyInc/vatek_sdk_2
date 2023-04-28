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

#ifndef _BRIDGE_SOURCE_
#define _BRIDGE_SOURCE_

#include <bridge/bridge_base.h>
#include <core/base/media_video.h>
#include <core/base/media_audio.h>

typedef enum _bsource_id
{
	bsource_ep9555e = (BBRIDGE_TYPE_BSOURCE | 0x100),
	bsource_ep9351	= (BBRIDGE_TYPE_BSOURCE | 0x200),
	bsource_adv718x = (BBRIDGE_TYPE_BSOURCE | 0x300),
	bsource_h1 = (BBRIDGE_TYPE_BSOURCE | 0x400),
}bsource_id;

#define BSOURCE_EP9555E_BIT			BBRIDGE_DRI_TO_BIT(bsource_ep9555e)
#define BSOURCE_EP9351_BIT			BBRIDGE_DRI_TO_BIT(bsource_ep9351)
#define BSOURCE_ADV718X_BIT			BBRIDGE_DRI_TO_BIT(bsource_adv718x)
#define BSOURCE_H1_BIT				BBRIDGE_DRI_TO_BIT(bsource_h1)

/* bridge source command define */

#define BRIDGE_IP_SOURCE_TAG        0x00F10000
#define IP_SOURCE_SET_CMD(cmd)		BRIDGE_SET_CMD(BRIDGE_IP_SOURCE_TAG,cmd)
#define IS_BRIDGE_SOURCE_CMD(cmd)	((cmd & 0x00FF0000) == BRIDGE_IP_SOURCE_TAG)

#define BSOURCE_CMD_CNTL			IP_SOURCE_SET_CMD(0x101)
#define BSOURCE_CMD_GETACTIVE		IP_SOURCE_SET_CMD(0x102)
#define BSOURCE_CMD_STATUS			IP_SOURCE_SET_CMD(0x200)

#define BSOURCE_CNTL_START			0xFA00A501
#define BSOURCE_CNTL_STOP			0xFA00A502

typedef struct _bridge_source_cmd
{
	uint32_t source_cntl;
	bsource_id source_id;
}bridge_source_cmd,*Pbridge_source_cmd;

_bparam_start(bridge_source_cmd)
	_bparam_uint(0, bridge_source_cmd, source_cntl)
	_bparam_enum(1, bridge_source_cmd, bsource_id, source_id)
_bparam_end

typedef struct _bridge_video_info
{
	video_resolution resolution;
	video_framerate framerate;
	video_aspect_rate aspectrate;
	uint32_t offset_x;
	uint32_t offset_y;
	uint32_t pixelclk;
	uint32_t signal_flag;
}bridge_video_info, *Pbridge_video_info;

typedef struct _bridge_audio_info
{
	audio_samplerate samplerate;
	audio_channel channel;
}bridge_audio_info, *Pbridge_audio_info;

typedef struct _bridge_source
{
	bridge_status status;
	bsource_id source_id;
	bridge_video_info video_info;
	bridge_audio_info audio_info;
}bridge_source,*Pbridge_source;

typedef enum _bsource_scale				/* output timing changed (ep9555e and sx1084a )*/
{
	sscale_bypass = 0,
	sscale_i_2_p,						/* interleave scale to progress ex. 1080i -> 1080p	*/
	sscale_p_2_i,
	sscale_fixed_480p,					/* used scale would enabled i_2_p also				*/
	sscale_fixed_576p,
	sscale_fixed_720p,
	sscale_fixed_1080p,
	sscale_end = BBRIDGE_ENUM_MAX,		/* make _keil c enum size 4 bytes					*/
}bsource_scale;

typedef enum _bsource_baseclk			/* when source can not detect current base clock used default */
{
	sbaseclk_1000 = 0,					/* fps : 24, 30, 60				*/
	sbaseclk_1001 = 1,					/* fps : 23.97, 29.97, 59.94	*/
	sbaseclk_end = BBRIDGE_ENUM_MAX,	/* make _keil c enum size 4 bytes					*/
}bsource_baseclk;

typedef struct _ep9555e_param
{
	bsource_scale scale;
	bsource_baseclk baseclk;
}ep9555e_param,*Pep9555e_param;

static const ep9555e_param def_ep9555e_param = { sscale_i_2_p ,sbaseclk_1001, };

typedef struct _h1_param
{
	bsource_scale scale;
	bsource_baseclk baseclk;
}h1_param,*Ph1_param;

static const h1_param def_h1_param = { sscale_bypass, };

typedef struct _ep9351_param
{
	bsource_baseclk baseclk;
	int32_t pin_hp;
}ep9351_param,*Pep9351_param;

static const ep9351_param def_ep9351_param = { sbaseclk_1001, -1};

#define ADV718X_BRI_DEF					0
#define ADV718X_HUE_DEF					0
#define ADV718X_CON_DEF					128
#define ADV718X_SAT_DEF					128

typedef enum _adv718x_input
{
    adv718x_in_cvbs_ain_1 = 0x00,
    adv718x_in_cvbs_ain_2 = 0x01,
    adv718x_in_cvbs_ain_3 = 0x02,
    adv718x_in_cvbs_ain_4 = 0x03,
    adv718x_in_cvbs_ain_5 = 0x04,
    adv718x_in_cvbs_ain_6 = 0x05,
    adv718x_in_svideo_ain_1_2 = 0x06,
    adv718x_in_svideo_ain_3_4 = 0x07,
    adv718x_in_svideo_ain_5_6 = 0x08,
    adv718x_in_ypbpr_ain_1_2_3 = 0x09,
    adv718x_in_ypbpr_ain_4_5_6 = 0x0a,
}adv718x_input;

typedef enum _adv718x_std
{
    adv718x_std_ad_pal_bg_ntsc_j_secam = 0x00,
    adv718x_std_ad_pal_bg_ntsc_j_secam_ped = 0x01,
    adv718x_std_ad_pal_n_ntsc_j_secam = 0x02,
    adv718x_std_ad_pal_n_ntsc_m_secam = 0x03,
    adv718x_std_ntsc_j = 0x04,
    adv718x_std_ntsc_m = 0x05,
    adv718x_std_pal_60 = 0x06,
    adv718x_std_ntsc_443 = 0x07,
    adv718x_std_pal_bg = 0x08,
    adv718x_std_pal_n = 0x09,
    adv718x_std_pal_m = 0x0a,
    adv718x_std_pal_m_ped = 0x0b,
    adv718x_std_pal_comb_n = 0x0c,
    adv718x_std_pal_comb_n_ped = 0x0d,
    adv718x_std_pal_secam = 0xe,
    adv718x_std_pal_secam_ped = 0xf,
}adv718x_std;

typedef struct _adv718x_param
{
	adv718x_input input;
	adv718x_std std;
	uint8_t brightnes;					/* +- 127	default 0	*/
	uint8_t hue;						/* +- 127	default 0	*/
	uint8_t contrast;					/* 0 - 255	default 128	*/
	uint8_t saturation;					/* 0 - 255	default 128	*/
}adv718x_param,*Padv718x_param;

static const adv718x_param def_adv718x_param = { adv718x_in_cvbs_ain_4,adv718x_std_ad_pal_bg_ntsc_j_secam,ADV718X_BRI_DEF,ADV718X_HUE_DEF,ADV718X_CON_DEF,ADV718X_SAT_DEF, };

_bparam_start(bridge_source)
	_bparam_enum(0, bridge_source, bridge_status, status)
	_bparam_enum(1, bridge_source, bsource_id, source_id)
_bparam_end

_bparam_start(bridge_video_info)
	_bparam_enum(3, bridge_video_info, video_resolution, resolution)
	_bparam_enum(4, bridge_video_info, video_framerate, framerate)
	_bparam_enum(5, bridge_video_info, video_aspect_rate, aspectrate)
	_bparam_uint(6, bridge_video_info, offset_x)
	_bparam_uint(7, bridge_video_info, offset_y)
	_bparam_uint(8, bridge_video_info, pixelclk)
	_bparam_uint(9, bridge_video_info, signal_flag)
_bparam_end

_bparam_start(bridge_audio_info)
	_bparam_enum(10, bridge_audio_info, audio_channel, channel)
	_bparam_enum(11, bridge_audio_info, audio_samplerate, samplerate)
_bparam_end

#endif
