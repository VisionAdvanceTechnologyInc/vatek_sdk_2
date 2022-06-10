#ifndef OVATEK_UI_CODEC_H
#define OVATEK_UI_CODEC_H

#include <codec/ovatek_codec.h>
#include <codec/ovatek_codec_aac.h>
#include <codec/ovatek_codec_nvenc.h>
#include <codec/ovatek_codec_qsv.h>

_ui_enum_start(ovatek_rcmode)
	_val(ovatek_rc_cbr,cbr)
	_val(ovatek_rc_vbr,vbr)
_ui_enum_end

_ui_enum_start(ovatek_audio_bitrate)
	_val(oaudio_bitrate_128, 128)
	_val(oaudio_bitrate_192, 192)
	_val(oaudio_bitrate_256, 256)
_ui_enum_end

_ui_enum_start(ovatek_aac_mode)
	_val(ovatek_aac_adts, adts)
	_val(ovatek_aac_latm, latm)
_ui_enum_end

_ui_struct_start(ovatek_aac_param)
	_prop_enum(ovatek_aac_param, ovatek_aac_mode, mode, "aac - format mode", NULLHAL)
	_prop_enum(ovatek_aac_param, ovatek_audio_bitrate, bitrate, "aac - target bitrate", NULLHAL)
_ui_struct_end

_ui_struct_start(ovatek_h264_param)
	_prop_enum(ovatek_h264_param,h264_profile,profile,"h264 codec profile",NULLHAL)
	_prop_enum(ovatek_h264_param,h264_level,level,"h264 codec level",NULLHAL)
	_prop_enum(ovatek_h264_param,ovatek_rcmode,rcmode,"h264 rate controll mode",NULLHAL)
	_prop_u8(ovatek_h264_param, bframe,"h264 enable bframe numbers",NULLHAL)
	_prop_u8(ovatek_h264_param, keysec,"h264 IDR replace time second",NULLHAL)
	_prop_u32(ovatek_h264_param, bitrate,"encode target bitrate",NULLHAL)
	_prop_u32(ovatek_h264_param,maxbitrate,"encode rate controll bitrate threshold",NULLHAL)
_ui_struct_end

_ui_struct_start(ovatek_mpeg2_param)
	_prop_enum(ovatek_mpeg2_param, mpeg2_profile, profile, "mpeg2 codec profile", NULLHAL)
	_prop_u8(ovatek_mpeg2_param, bframe, "mpeg2 enable bframe numbers [0-3]", NULLHAL)
	_prop_u32(ovatek_mpeg2_param, bitrate, "encode target bitrate", NULLHAL)
	_prop_u32(ovatek_mpeg2_param, maxbitrate, "encode rate controll bitrate threshold", NULLHAL)
_ui_struct_end

#endif
