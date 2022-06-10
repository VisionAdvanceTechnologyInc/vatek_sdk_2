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

#ifndef _MEDIA_VIDEO_
#define _MEDIA_VIDEO_

#include <stdint.h>
#include <core/hal/halreg_stream.h>

#define MPEG2_QUANT_MAX					31
#define MPEG2_QUANT_MIN					3

#define H264_QUANT_MAX					51
#define H264_QUANT_MIN					5

#define MEDIA_IDX_VIDEO_SD				0
#define MEDIA_IDX_VIDEO_HD				1
#define MEDIA_IDX_VIDEO_FULLHD			2
#define MEDIA_IDX_VIDEO_NUMS			3

#define is_current_resolution_idx(idx)	(idx >= MEDIA_IDX_VIDEO_SD && idx <= MEDIA_IDX_VIDEO_FULLHD)

#define QMATRIX_EN_SPEC					0x00000000
#define QMATRIX_EN_DEFAULT				0x00000001
#define QMATRIX_EN_USER					0x00000002

#define MVME_SEARCH_ALL					0x00000000
#define MVME_SEARCH_OTHER				0x00000001
#define MVME_SEARCH_SKIP				0x00000002

#define H264_FLAG_EN_INTRA_8_8			0x00000002
#define H264_FLAG_EN_MV_8_8				0x00010000
#define H264_FLAG_EN_MV_16_8			0x00020000

#define H264_CODE_CAVLC					0x00000000
#define H264_CODE_CABAC					0x00000001

#define MPEG2_PROFILE_AUTO				0x00000000
#define MPEG2_PROFILE_HIGH				0x00000001
#define MPEG2_PROFILE_MAIN				0x00000004
#define MPEG2_PROFILE_MAIN_LMAIN		0x00000008

#define M2FLAG_QSCALE_NONELINEAR    	0x00000001
#define M2FLAG_LOW_LATENCY          	0x00000002
#define M2FLAG_HALF_COARSE				0x00010000
#define M2FLAG_EN_MV_CNTL				0x00020000
#define M2FLAG_EN_QD_CNTL				0x00040000

#define M2DC_PRESISION_8            	0x00000000
#define M2DC_PRESISION_9            	0x00000001
#define M2DC_PRESISION_10           	0x00000002
#define M2DC_PRESISION_11           	0x00000003

#define QD_INTER_VLC					0x00000010
#define QD_ALT_SCAN						0x00000040
#define QD_EN_TRUNC_AFTER_Q				0x00000080
#define QD_EN_TRUNC_SMALL_C				0x00000100

#define ME_USED_H_MV					0x00000001
#define ME_USED_V_MV					0x00000002
#define ME_ZERO_MV						0x00000008
#define ME_CO_LOCATED_MV				0x00000010

/**
 * @ingroup media_props
 * Video Codec Define
 */
typedef enum _video_codec{
	encvideo_unknown = -1,
	encvideo_mpeg2 = VIDEO_MPEG2,	/** MPEG2 (ISO-13818-2)	*/
	encvideo_h264 = VIDEO_H264,		/** H264  (IEC_14496-XX) */
	encvideo_hevc = VIDEO_HEVC,		/** HEVC	*/
}video_codec;

#define is_current_vcodec(val)	(val >= encvideo_mpeg2 && val <= encvideo_hevc)

/**
 * @ingroup media_props
 * Video Resolution Define
 */
typedef enum _video_resolution{
	resolution_unknown = -1,
	resolution_480i = RESOLUTION_480I,		/*!< 480i (720x480)*/
	resolution_480p = RESOLUTION_480P,		/*!< 480p (720x480)*/
	resolution_576i = RESOLUTION_576I,		/*!< 576i (720x576)*/
	resolution_576p = RESOLUTION_576P,		/*!< 576p (720x576)*/
	resolution_720p = RESOLUTION_720P,		/*!< 720p (1280x720)*/
	resolution_1080i = RESOLUTION_1080I,	/*!< 1080i (1920x1080)*/
	resolution_1080p = RESOLUTION_1080P,	/*!< 1080p (1920x1080)*/
	resolution_uhd = RESOLUTION_UHD, 		/*!< UHD (4K:3840x2160) */
}video_resolution;

static const uint16_t resolution_size[][2] =
{
	{ 720,240, },{ 720,480, },{ 720,288, },{ 720,576, },{ 1280,720, },{ 1920,540, },{ 1920,1080, },{ 3840,2160, },
};

#define is_current_resolution(res)	(res >= resolution_480i && res <= resolution_1080p)
#define is_resolution_interlaced(res)	(res == resolution_480i || res == resolution_576i || res == resolution_1080i)

/**
 * @ingroup media_props
 * Video Framerate Define
 */
typedef enum _video_framerate{
	framerate_unknown = -1,
	framerate_23_97 = FRAMERATE_23_97,	/*!< 23.97 */
	framerate_24 = FRAMERATE_24,		/*!< 24 */
	framerate_25 = FRAMERATE_25,		/*!< 25 */
	framerate_29_97 = FRAMERATE_29_97,	/*!< 29.97 */
	framerate_30 = FRAMERATE_30,		/*!< 30 */
	framerate_50 = FRAMERATE_50,		/*!< 50 */
	framerate_59_94 = FRAMERATE_59_94,	/*!< 59.94 */
	framerate_60 = FRAMERATE_60,		/*!< 60 */
}video_framerate;

#define is_current_framerate(fps)	(fps >= framerate_23_97 && fps <= framerate_60)

/**
 * @ingroup media_props
 * Video Aspect Rate Define
 */
typedef enum _video_aspect_rate{
	aspectrate_unknown = -1,
	aspectrate_4_3 = ASPECTRATE_4_3,	/*!< 4:3 */
	aspectrate_16_9 = ASPECTRATE_16_9,	/*!< 16:9 */
}video_aspect_rate;

#define is_current_aspectrate(aspect)	(aspect >= aspectrate_4_3 && aspect <= aspectrate_16_9)

/**
 * @ingroup media_props
 * video Parameters
 */
typedef struct _media_video{
	video_codec vcodec;				/*!< video codec */
	video_resolution resolution;	/*!< video resolution */
	video_framerate framerate;		/*!< video framerate */
	video_aspect_rate aspectrate;	/*!< video aspect rate */
}media_video;

typedef media_video* Pmedia_video;

/**
 * @ingroup encoder_props 
 * encoder rate control mode 
 */
typedef enum _ratecontrol_mode{
	rc_vbr = QUALITY_RC_VBR,		/*!< vbr */
	rc_fixed_q = QUALITY_FIXED_Q,	/*!< fixed q (debug) */
	rc_auto = QUALITY_AUTO,			/*!< auto */
}ratecontrol_mode;

#define is_current_rcmode(mode)	(rcmode >= rc_vbr && rcmode <= rc_fixed_q)

/**
 * @ingroup encoder_props
 * encoder quality parameters
 */
typedef struct _quality_param{
	ratecontrol_mode rcmode;		/*!< rate control mode */
	uint8_t minq;					/*!< video codec quantization min value [MPEG2 : 1-32] [H264 : 1-51] */
	uint8_t maxq;					/*!< video codec quantization max value [MPEG2 : 1-32] [H264 : 1-51] */
	uint16_t goplen;				/*!< group of pictures length normal used 16 with mpeg2 and 30 with h264*/
	uint32_t latency;				/*!< ts stream pcr time offset with pts and dts in ms */
	uint32_t bitrate;				/*!< target video encoder output bitrate*/
}quality_param;

typedef quality_param* Pquality_param;

static const quality_param def_quality_mpeg2 = { rc_vbr,3,10,16,500,19000000, };
static const quality_param def_quality_h264 = { rc_vbr,10,25,16,500,19000000, };


typedef enum _qmatrix_mode{
	qmatrix_en_spec = QMATRIX_EN_SPEC,
	qmatrix_en_default = QMATRIX_EN_DEFAULT,
	qmatrix_en_user = QMATRIX_EN_USER,
}qmatrix_mode;

#define is_current_qmatrix(qm)	(qm >= qmatrix_en_spec && qm <= qmatrix_en_user)

typedef enum _me_mv_search{
	search_all = MVME_SEARCH_ALL,
	search_other = MVME_SEARCH_OTHER,
	search_skip = MVME_SEARCH_SKIP,
}me_mv_search;

#define is_current_memv_search(s)	(s >= search_all && s <= search_skip)

typedef enum _mpeg2_dc_precision{
	dc_presision_8 = M2DC_PRESISION_8,
	dc_presision_9 = M2DC_PRESISION_9,
	dc_presision_10 = M2DC_PRESISION_10,
	dc_presision_11 = M2DC_PRESISION_11,
}mpeg2_dc_precision;

#define is_current_mpeg2_dc(dc)	(dc >= dc_presision_8 && dc <= dc_presision_11)

typedef enum _mpeg2_profile{
	mpeg2_profile_auto = MPEG2_PROFILE_AUTO,
	mpeg2_profile_high = MPEG2_PROFILE_HIGH,
	mpeg2_profile_main = MPEG2_PROFILE_MAIN,
	mpeg2_profile_main_lmain = MPEG2_PROFILE_MAIN_LMAIN,
}mpeg2_profile;

#define is_current_mpeg2_profile(pf)	(pf >= mpeg2_profile_auto && pf <= mpeg2_profile_main)

typedef struct _mpeg2_param{
	uint32_t flags;
	mpeg2_profile profile;
	mpeg2_dc_precision dc_precision;
	me_mv_search search;
	qmatrix_mode qmatrix;
	uint32_t qd_cntl;
	uint32_t me_cntl;
	uint32_t recvs[2];
	uint8_t intra_quant_matrix[8][8];	/* can not changed order*/
	uint8_t inter_quant_matrix[8][8];
}mpeg2_param; /* 160 bytes*/

typedef mpeg2_param* Pmpeg2_param;

#define MPEG2_INTRA_LEN		(8*8)
#define MPEG2_INTER_LEN		(8*8)
#define MPEG2_QUANT_LEN		(8*8*2)

/* h264_codec param */

typedef enum _h264_profile{
	h264_profile_auto = 0,
	h264_profile_baseline = 66,
	h264_profile_main = 77,
	h264_profile_high = 100,
}h264_profile;

typedef enum _h264_level{
	h264_level_auto = 0,
	h264_level_3_0 = 30,
	h264_level_3_1 = 31,
	h264_level_3_2 = 32,
	h264_level_4_0 = 40,
	h264_level_4_1 = 41,
	h264_level_4_2 = 42,
	h264_level_5_0 = 50,
	h264_level_5_1 = 51,
}h264_level;

typedef enum _h264_entropy_code{
	h264_entropy_cavlc = H264_CODE_CAVLC,
	h264_entropy_cabac = H264_CODE_CABAC,
}h264_entropy_code;

#define is_current_entropy_code(ec)	(ec >= h264_entropy_cavlc && ec <= h264_entropy_cabac)

typedef struct _h264_matrix_4x4{
	uint8_t luma[4][4];
	uint8_t chroma[4][4];
}h264_matrix_4x4;

typedef h264_matrix_4x4* Ph264_matrix_4x4;

typedef struct _h264_martix_8x8{
	uint8_t luma[8][8];
}h264_martix_8x8;

typedef h264_martix_8x8* Ph264_martix_8x8;

#define H264_MATRIX_4_4_LEN		(4*4)
#define H264_MATRIX_8_8_LEN		(8*8)

typedef struct _h264_param{
	h264_profile profile;
	h264_level level;
	uint32_t flags;
	uint16_t sps_id;
	uint16_t pps_id;
	h264_entropy_code entropy_code;
	me_mv_search search;
	qmatrix_mode qmatrix;
	uint8_t recvs[8];
	h264_matrix_4x4 intra_4x4; /* can not changed order*/
	h264_matrix_4x4 inter_4x4;
	h264_martix_8x8 intra_8x8;
	h264_martix_8x8 inter_8x8;
}h264_param;

typedef h264_param* Ph264_param;

#define H264_QUANT_LEN  ((sizeof(h264_matrix_4x4) * 2) + (sizeof(h264_martix_8x8) * 2))

#endif
