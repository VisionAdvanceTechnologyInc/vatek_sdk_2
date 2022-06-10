#ifndef OVATEK_UI_FFMPEG_H
#define OVATEK_UI_FFMPEG_H

#include <ui/ovatek_ui_codec.h>
#include <codec/ffmpeg/ovatek_ffmpeg_sw.h>
#include <codec/ffmpeg/ovatek_ffmpeg_qsv.h>
#include <codec/ffmpeg/ovatek_ffmpeg_nvenc.h>

_ui_flags_start(ompeg2_param, functions)
	_val(OMPEG2_EN_SEQ_DISP_EXT, en_seq_disp_ext)
	_val(OMPEG2_EN_A52CC, en_a52_cc)
_ui_flags_end

_ui_struct_start(ompeg2_param)
	_prop_flag(ompeg2_param, functions, "mpeg2 - extend function", NULLHAL)
_ui_struct_end

_ui_struct_start(omp2_param)
	_prop_enum(omp2_param, ovatek_audio_bitrate,bitrate,"mp2 - bitrate",NULLHAL)
_ui_struct_end

_ui_struct_start(oa52_param)
	_prop_enum(omp2_param, ovatek_audio_bitrate, bitrate, "mp2 - bitrate", NULLHAL)
_ui_struct_end

#endif
