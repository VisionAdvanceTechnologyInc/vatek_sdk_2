#ifndef _CODEC_TOOL_
#define _CODEC_TOOL_

#include <core/base/media_video.h>

typedef enum _hal_tv_encoding
{
	tv_unknown = -1,
	tv_ntsc_m = 0,
	pal_bdghi = 1,
	secam = 2,
	ntsc_4_3 = 3,
	pal_m = 4,
	pal_cn = 5,
	pal_60 = 6,
	secm_525 = 7,
}hal_tv_encoding;

typedef struct _tv_encoding_param
{
	video_resolution resolution;
	video_framerate framerate;
}tv_encoding_param,*Ptv_encoding_param;

static const tv_encoding_param tv_encoding_spec[8][2] =
{
	/*
	{ { resolution_480i,framerate_59_94 },{ resolution_480p,framerate_29_97, }, },
	{ { resolution_576i,framerate_50 },{ resolution_576p,framerate_25, }, },
	{ { resolution_576i,framerate_50 },{ resolution_576p,framerate_25, }, },
	{ { resolution_480i,framerate_60 },{ resolution_480p,framerate_30, }, },
	{ { resolution_480i,framerate_60 },{ resolution_480p,framerate_30, }, },
	{ { resolution_576i,framerate_50 },{ resolution_576p,framerate_25, }, },
	{ { resolution_480i,framerate_60 },{ resolution_480p,framerate_30, }, },
	{ { resolution_480i,framerate_60 },{ resolution_480p,framerate_30, }, },*/
	{ { resolution_480i,framerate_59_94 },{ resolution_480p,framerate_29_97, }, },	//NTSC M/NTSC J(525/59)
	{ { resolution_480i,framerate_59_94 },{ resolution_480p,framerate_29_97, }, },	//ntsc_4_3	(525/59)
	{ { resolution_480i,framerate_60 },{ resolution_480p,framerate_30, }, },		//pal_m 	(525/60)
	{ { resolution_480i,framerate_60 },{ resolution_480p,framerate_30, }, },		//pal_60	(525/60)
	{ { resolution_576i,framerate_50 },{ resolution_576p,framerate_25, }, },		//pal_bdghi	(625/50)
	{ { resolution_576i,framerate_50 },{ resolution_576p,framerate_25, }, },		//secam		(625/50)
	{ { resolution_576i,framerate_50 },{ resolution_576p,framerate_25, }, },		//pal_cn	(625/50)
	{ { resolution_480i,framerate_50 },{ resolution_480p,framerate_25, }, },		//secm_525	(525/50)
};

#endif
