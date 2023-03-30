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

#ifndef _UI_PROPS_ENCODER_
#define _UI_PROPS_ENCODER_

#include <core/ui/ui_props_define.h>
#include <core/base/stream_encoder.h>

HALREG_S(ENCODER_MODE, 8)
HALREG_S(ENCODER_FLAGS, 32)
HALREG_S(ENCODER_TAG,32)
HALREG_S(ENCODER_PMTPID, 13)

HALREG_S(VIDEO_PID, 13)
HALREG_S(VIDEO_CODEC, 8)
HALREG_S(VIDEO_RESOLUTION, 16)
HALREG_S(VIDEO_FRAMERATE, 16)
HALREG_S(VIDEO_ASPECTRATE, 16)

HALREG_S(AUDIO_PID, 13)
HALREG_S(AUDIO_CODEC, 8)
HALREG_S(AUDIO_CHANNEL, 8)
HALREG_S(AUDIO_SAMPLERATE, 8)

/* enc : quality_param*/
HALREG_S(QUALITY_RCMODE,4)
HALREG_S(QUALITY_GOP,8)
HALREG_S(QUALITY_MINQ,8)
HALREG_S(QUALITY_MAXQ,8)
HALREG_S(QUALITY_BITRATE,32)
HALREG_S(QUALITY_LATENCY,16)

/* enc : vi_param*/
HALREG_S(VI_0_FLAGS,32)
HALREG_S(VI_0_PIXELCLOCK,32)
HALREG_S(VI_0_OFFSET_X,16)
HALREG_S(VI_0_OFFSET_Y,16)


/* media_info define */
_ui_enum_start(video_codec)
	_val(encvideo_mpeg2, mpeg2)
	_val(encvideo_h264, h264)
	_val(encvideo_hevc, hevc)
_ui_enum_end

_ui_enum_start(video_resolution)
	_val(resolution_1080p,1080p)
	_val(resolution_1080i,1080i)
	_val(resolution_720p,720p)
	_val(resolution_576p,576p)
	_val(resolution_576i,576i)
	_val(resolution_480p,480p)
	_val(resolution_480i,480i)
_ui_enum_end

_ui_enum_start(video_framerate)
	_val(framerate_23_97,23.97)
	_val(framerate_24,24)
	_val(framerate_25,25)
	_val(framerate_29_97,29.97)
	_val(framerate_30,30)
	_val(framerate_50,50)
	_val(framerate_59_94,59.94)
	_val(framerate_60,60)
_ui_enum_end

_ui_enum_start(video_aspect_rate)
	_val(aspectrate_4_3,4_3)
	_val(aspectrate_16_9,16_9)
_ui_enum_end

_ui_struct_start(media_video)
	_prop_enum(media_video,video_codec,vcodec,"video - codec", VIDEO_CODEC)
	_prop_enum(media_video, video_resolution,resolution,"video - resolution", VIDEO_RESOLUTION)
	_prop_enum(media_video,video_framerate,framerate,"video - frame rate", VIDEO_FRAMERATE)
	_prop_enum(media_video,video_aspect_rate,aspectrate,"video - aspect rate", VIDEO_ASPECTRATE)
_ui_struct_end

/* media_audio */
_ui_enum_start(audio_codec)
	_val(encaudio_mp2, mp1_l2)
	_val(encaudio_aac_lc_adts, aac_lc_adts)
	_val(encaudio_ac3, ac_3)
	_val(encaudio_aac_lc_latm, aac_lc_latm)
_ui_enum_end

_ui_enum_start(audio_samplerate)
	_val(sample_rate_32, ss_32)
	_val(sample_rate_44_1, ss_44_1)
	_val(sample_rate_48, ss_48)
_ui_enum_end

_ui_enum_start(audio_channel)
	_val(channel_mute, mute)
	_val(channel_stereo, stereo)
	_val(channel_mono_l, mono_L)
	_val(channel_mono_r, mono_R)
	_val(channel_stereo_mono_l, stereo_mono_L)
	_val(channel_stereo_mono_r, stereo_mono_R)
_ui_enum_end

_ui_struct_start(media_audio)
	_prop_enum(media_audio,audio_codec, acodec,"audio - codec type", AUDIO_CODEC)
	_prop_enum(media_audio,audio_samplerate, samplerate,"audio - sample rate", AUDIO_SAMPLERATE)
	_prop_enum(media_audio,audio_channel, channel,"audio - channel", AUDIO_CHANNEL)
_ui_struct_end

/* quality_param*/
_ui_enum_start(ratecontrol_mode)
	_val(rc_vbr, rc_vbr)
	_val(rc_fixed_q, rc_fixed_q)
	_val(rc_auto, rc_auto)
_ui_enum_end

_ui_struct_start(quality_param)
	_prop_enum(quality_param, ratecontrol_mode, rcmode, "quality - rate control mode", QUALITY_RCMODE)
	_prop_u8(quality_param, minq, "quality - min quant", QUALITY_MINQ)
	_prop_u8(quality_param, maxq, "quality - max quant", QUALITY_MAXQ)
	_prop_u16(quality_param, goplen, "quality - gop length", QUALITY_GOP)
	_prop_u32(quality_param, latency, "quality - latency (ms)", QUALITY_LATENCY)
	_prop_u32(quality_param, bitrate, "quality - bitrate (bps)", QUALITY_BITRATE)
_ui_struct_end


/* struct vi param*/
_ui_flags_start(vi_param, vi_flags)
	_val(VI_BUSWIDTH_16, en_dataline_16)
	_val(VI_SEPARATED_SYNC, en_separated_sync)
	_val(VI_CLK_INVERSE, en_clk_negative)
	_val(VI_HSYNC_INVERSE, en_hsync_negative)
	_val(VI_VSYNC_INVERSE, en_vsync_negative)
	_val(VI_EXT_HALF_FPS, en_half_fps)
	_val(VI_FIELD_INVERSE, en_field_inverse)
_ui_flags_end

/* struct vi param*/
_ui_flags_start(bridge_encoder_param, vi_flags)
	_val(VI_BUSWIDTH_16, en_dataline_16)
	_val(VI_SEPARATED_SYNC, en_separated_sync)
	_val(VI_CLK_INVERSE, en_clk_negative)
	_val(VI_HSYNC_INVERSE, en_hsync_negative)
	_val(VI_VSYNC_INVERSE, en_vsync_negative)
	_val(VI_EXT_HALF_FPS, en_half_fps)
	_val(VI_FIELD_INVERSE, en_field_inverse)
_ui_flags_end

_ui_struct_start(vi_param)
	_prop_flag(vi_param, vi_flags, "vi - function flags", VI_0_FLAGS)
	_prop_u32(vi_param, vi_pixelclk, "vi - pixel clock (KHz)", VI_0_PIXELCLOCK)
	_prop_u16(vi_param, offset_x, "vi - offset x", VI_0_OFFSET_X)
	_prop_u16(vi_param, offset_y, "vi - offset y", VI_0_OFFSET_Y)
_ui_struct_end

/* struct encoder param*/
_ui_enum_start(encoder_mode)
	_val(encoder_source_colorbar, colorbar)
	_val(encoder_source_bootlogo, bootlogo)
	//_val(encoder_source_vi_0, vi_0)
_ui_enum_end

_ui_flags_start(encoder_param, encoder_flags)
	_val(ENC_EN_DISABLE_DEINTERLACED, en_disable_deinterlaced)
	_val(ENC_EN_PROGRESSIVE_2_I,en_progressive_2_i)
	_val(ENC_EN_DISABLE_ADTS_CRC,disable_adts_crc)
	_val(ENC_DIS_LATENCY_Q, disable_latency_q)
	_val(ENC_EN_SW_I2S, en_sw_i2s)
	_val(ENC_EN_SW_CLK, en_sw_clk)
	_val(ENC_EN_DROP_FRAME, en_drop_frame)
	_val(ENC_EN_Q_COST, en_q_cost)
_ui_flags_end

_ui_struct_start(encoder_param)
	_prop_enum(encoder_param, encoder_mode, mode, "enc - encoder mode", ENCODER_MODE)
	_prop_h16(encoder_param,pmt_pid,"enc - pmt pid", ENCODER_PMTPID)
	_prop_flag(encoder_param, encoder_flags, "enc - function flag", ENCODER_FLAGS)
	_prop_h32(encoder_param,encoder_tag,"enc - resource tag",ENCODER_TAG)
	_prop_h16(encoder_param,video_pid,"enc - video pid",VIDEO_PID)
	_prop_h16(encoder_param, audio_pid, "enc - audio pid", AUDIO_PID)
_ui_struct_end


_ui_flags_start(bridge_encoder_param, en_flags)
	_val(ENC_EN_DISABLE_DEINTERLACED, en_disable_deinterlaced)
	_val(ENC_EN_PROGRESSIVE_2_I, en_progressive_2_i)
	_val(ENC_EN_DISABLE_ADTS_CRC, disable_adts_crc)
	_val(ENC_DIS_LATENCY_Q, disable_latency_q)
	_val(ENC_EN_SW_I2S, en_sw_i2s)
	_val(ENC_EN_SW_CLK, en_sw_clk)
	_val(ENC_EN_DROP_FRAME, en_drop_frame)
	_val(ENC_EN_Q_COST, en_q_cost)
		
_ui_flags_end

/* audio codec define */

_ui_enum_start(audio_rc_mode)
	_val(audio_rc_auto,auto)
	_val(audio_rc_cbr,cbr)
	_val(audio_rc_vbr,vbr)
_ui_enum_end

_ui_enum_start(audio_quality_mode)
	_val(audio_quality_normal,normal)
	_val(audio_quality_middle,middle)
	_val(audio_quality_good,good)
_ui_enum_end

_ui_flags_start(audio_param, functions)
	_val(AUDIO_EN_ADTS_NO_CRC,adts_no_crc)
	_val(AUDIO_EN_ADTS_FULLNESS,adts_en_fullness)
_ui_flags_end

/* audio_param */
_ui_struct_start(audio_param)
	_prop_enum(audio_param,audio_rc_mode, rcmode,"audio - rate control mode", NULLHAL)
	_prop_enum(audio_param, audio_quality_mode, quality, "audio - quality mode", NULLHAL)
	_prop_flag(audio_param, functions,"audio - function flags",NULLHAL)
_ui_struct_end

/* video codec define */

_ui_enum_start(qmatrix_mode)
	_val(qmatrix_en_spec, spec)
	_val(qmatrix_en_default, default)
	_val(qmatrix_en_user, user)
_ui_enum_end

/* mpeg2_param */

_ui_enum_start(mpeg2_profile)
	_val(mpeg2_profile_auto,auto)
	_val(mpeg2_profile_main,profile_main)
	_val(mpeg2_profile_high,profile_high)
	_val(mpeg2_profile_main_lmain,profile_main_lmain)
_ui_enum_end

_ui_enum_start(mpeg2_dc_precision)
	_val(dc_presision_8, dc_8)
	_val(dc_presision_9, dc_9)
	_val(dc_presision_10, dc_10)
	_val(dc_presision_11, dc_11)
_ui_enum_end

_ui_enum_start(me_mv_search)
	_val(search_all, search_all)
	_val(search_other, search_other)
	_val(search_skip, search_skip)
_ui_enum_end

_ui_flags_start(mpeg2_param, flags)
	_val(M2FLAG_QSCALE_NONELINEAR, en_qscale_nonelinear)
	_val(M2FLAG_LOW_LATENCY, en_low_latency)
	_val(M2FLAG_HALF_COARSE, en_half_coarse)
	_val(M2FLAG_EN_MV_CNTL, en_advance_mv_cntl)
	_val(M2FLAG_EN_QD_CNTL, en_advance_qd_cntl)
_ui_flags_end

_ui_flags_start(mpeg2_param, me_cntl)
	_val(ME_USED_H_MV, en_mv_hmv)
	_val(ME_USED_V_MV, en_mv_vmv)
	_val(ME_ZERO_MV, en_mv_zero)
	_val(ME_CO_LOCATED_MV, en_mv_co_located)
_ui_flags_end

_ui_flags_start(mpeg2_param,qd_cntl)
	_val(QD_INTER_VLC, en_inter_vlc)
	_val(QD_ALT_SCAN, en_alt_scan)
	_val(QD_EN_TRUNC_AFTER_Q, en_trunc_after_q)
	_val(QD_EN_TRUNC_SMALL_C, en_trunc_small_q)
_ui_flags_end

_ui_struct_start(mpeg2_param)
	_prop_enum(mpeg2_param, mpeg2_profile, profile,"mpeg2 - profile", NULLHAL)
	_prop_flag(mpeg2_param, flags, "mpeg2 - function flags", NULLHAL)
	_prop_enum(mpeg2_param, mpeg2_dc_precision, dc_precision, "mpeg2 - dc precision", NULLHAL)
	_prop_flag(mpeg2_param, me_cntl, "mpeg2 - mv me control",NULLHAL)
	_prop_flag(mpeg2_param, qd_cntl, "mpeg2 - qd control", NULLHAL)
	_prop_enum(mpeg2_param, me_mv_search, search, "mpeg2 - me search mode", NULLHAL)
	_prop_enum(mpeg2_param, qmatrix_mode, qmatrix, "mpeg2 - quant matrix mode", NULLHAL)
_ui_struct_end

/*struct h264_param*/

_ui_enum_start(h264_profile)
	_val(h264_profile_auto,auto)
	_val(h264_profile_baseline,baseline)
	_val(h264_profile_main,profile_main)
	_val(h264_profile_high, profile_high)
_ui_enum_end

_ui_enum_start(h264_level)
	_val(h264_level_auto,auto)
	_val(h264_level_4_0,level_4_0)
	_val(h264_level_4_1,level_4_1)
	_val(h264_level_4_2,level_4_2)
	_val(h264_level_5_0,level_5_0)
	_val(h264_level_5_1,level_5_1)
_ui_enum_end

_ui_enum_start(h264_entropy_code)
	_val(h264_entropy_cavlc, cavlc)
	_val(h264_entropy_cabac, cabac)
_ui_enum_end

_ui_flags_start(h264_param, flags)
	_val(H264_FLAG_EN_INTRA_8_8, en_intra_8_8)
	_val(H264_FLAG_EN_MV_8_8, en_mv_8_8)
	_val(H264_FLAG_EN_MV_16_8, en_mv_16_8)
_ui_flags_end

_ui_struct_start(h264_param)
	_prop_enum(h264_param,h264_profile,profile,"h264 - profile", NULLHAL)
	_prop_enum(h264_param,h264_level,level,"h264 - level", NULLHAL)
	_prop_flag(h264_param, flags, "h264 - function flags", NULLHAL)
	_prop_u16(h264_param, pps_id, "h264 - pps id", NULLHAL)
	_prop_u16(h264_param, sps_id, "h264 - sps id", NULLHAL)
	_prop_enum(h264_param, h264_entropy_code, entropy_code, "h264 - entropy code", NULLHAL)
	_prop_enum(h264_param, me_mv_search, search, "h264 - mv search mode", NULLHAL)
	_prop_enum(h264_param, qmatrix_mode,qmatrix,"h264 - quant matrix mode", NULLHAL)
_ui_struct_end

#endif
