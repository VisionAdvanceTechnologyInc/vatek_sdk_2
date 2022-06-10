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

#ifndef _STREAM_ENCODER_
#define _STREAM_ENCODER_

#include <core/vatek_base.h>
#include <core/base/media_video.h>
#include <core/base/media_audio.h>

/**
 * @ingroup encoder_props
 * encoder av source mode 
 */
typedef enum _encoder_mode{
	encoder_source_unknown = -1,
    encoder_source_colorbar = ENCMOD_COLORBAR,		/*!< colorbar */
    encoder_source_bootlogo = ENCMOD_BOOTLOGO,		/*!< boot logo (firmware storage)*/
    encoder_source_vi_0 = ENCMOD_VI_0,				/*!< bt656|1120 and I2S */
}encoder_mode;

#define is_current_encmode(val)					(val >= encoder_source_colorbar && val <= encoder_source_vi_0)
#define is_current_encmode_used_vi(val)			(val == encoder_source_vi_0)

/**
 * @ingroup encoder_props
 * vi parameters
 */
typedef struct _vi_param{
    uint32_t vi_flags;			/*!< bt656|1120 single flags and function select*/
    uint32_t vi_pixelclk;		/*!< input pixel clock */
    uint16_t offset_x;			/*!< video source offset x (pixel) */
    uint16_t offset_y;			/*!< video source offset y (pixel) */
}vi_param;

typedef vi_param* Pvi_param;

#define is_vi_param_en_half_fps(vi)				(((vi)-> vi_flags& VI_EXT_HALF_FPS) != 0)
#define is_vi_param_en_field_signal(vi)			(((vi)-> vi_flags& VI_FIELDID_SIGNAL) != 0)
#define is_vi_param_en_pcrclk_enc(vi)			(((vi)-> vi_flags& VI_PCRCLK_ENC) != 0)
#define is_vi_param_en_field_id_inv(vi)			(((vi)-> vi_flags& VI_FIELD_INVERSE) != 0)
#define is_vi_param_en_disable_errchk(vi)		(((vi)-> vi_flags& VI_DISABLE_ERRCHK) != 0)

/**
 * @ingroup encoder_props
 * encoder parameters - config b-serial encoder function 
 */
typedef struct _encoder_param{
    encoder_mode mode;			/*!< video source mode */
	uint16_t pmt_pid;			/*!< output ts stream pmt-pid used when mux used iso-13818-1 mode*/
	uint16_t recv;
    uint32_t encoder_flags;		/*!< extend encoder function select */
	uint32_t encoder_tag;		/*!< mutil-function :
								 *video source is bootlogo used to select resource index */
	media_video video;			/*!< video source format */
	quality_param quality;		/*!< video encoder quality control */
	vi_param viparam;			/*!< control vi when source used vi */
	media_audio audio;			/*!< audio source format */
	uint16_t video_pid;			/*!< output ts stream video pid */
	uint16_t audio_pid;			/*!< output ts stream audio pid */
}encoder_param;

typedef encoder_param* Pencoder_param;

#define is_encoder_en_field_picture(flags)		(flags & ENC_EN_INTERLACED_FRAME)
#define is_encoder_en_progressive_2_i(flags)	(flags & ENC_EN_PROGRESSIVE_2_I)
#define is_encoder_en_disable_adts_crc(flags)	(flags & ENC_EN_DISABLE_ADTS_CRC)

#define EP9555E_VI_FLAG							(VI_SEPARATED_SYNC | VI_BUSWIDTH_16)

static const encoder_param default_encoder_param =
{
	encoder_source_colorbar,
	0x1000,
	0,
	0,
	0,
	{ encvideo_mpeg2,resolution_1080i,framerate_60,aspectrate_16_9 },
	{ rc_vbr,3,10,16,500,19000000, },
	{ EP9555E_VI_FLAG,148500,0,0, },
	{ encaudio_mp2,sample_rate_48,channel_stereo, },
	0x1002,
	0x1003,
};

#ifdef __cplusplus
extern "C" {
#endif

	HAL_API vatek_result encoder_param_reset(video_codec vcodec, Pencoder_param penc);
	HAL_API vatek_result encoder_param_set(hvatek_chip hchip,Pencoder_param penc);
	HAL_API vatek_result encoder_param_get(hvatek_chip hchip,Pencoder_param penc);

#ifdef __cplusplus
}
#endif

#endif
