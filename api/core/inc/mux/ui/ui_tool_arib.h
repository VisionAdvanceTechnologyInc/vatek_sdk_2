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

#ifndef _UI_TOOL_ARIB_
#define _UI_TOOL_ARIB_

#include <mux/mux_define.h>

typedef struct _arib_jp_area
{
	const char* area;
	uint8_t region_id;
	uint16_t area_code;
	uint8_t recv;
}arib_jp_area, *Parib_jp_area;

#define JPAREA(id,name,code)	{name,id,code,0,}
#define JPAREA_NULL(id)			{NULL,id,0,0,}

static const arib_jp_area arib_jp_area_tables[] =
{
	JPAREA_NULL(0),
	JPAREA(1,"Kanto wide-area",1445),			/* 関東広域 */
	JPAREA(2,"Kinki wide-area",2261),			/* 近畿広域 */
	JPAREA(3,"Chukyo wide-area",1834),			/* 中京広域 */
	JPAREA(4,"Hokkaido wide-area",363),			/* 北海道域 */
	JPAREA(5,"Okayama,Kagawa",1363),			/* 岡山香川 */
	JPAREA(6,"Shimane,Tottori",1689),			/* 島根鳥取 */
	JPAREA_NULL(7),
	JPAREA_NULL(8),
	JPAREA_NULL(9),
	JPAREA(10,"Hokkaido(Sapporo)",363),			/* 北海道（札幌）*/
	JPAREA(11,"Hokkaido(Hakodate)",363),		/* 北海道（函館）*/
	JPAREA(12,"Hokkaido(Asahikawa)",363),		/* 北海道（旭川）*/
	JPAREA(13,"Hokkaido(Obihiro)",363),			/* 北海道（帯広）*/
	JPAREA(14,"Hokkaido(Kushiro)",363),			/* 北海道（釧路）*/
	JPAREA(15,"Hokkaido(Kitami)",363),			/* 北海道（北見）*/
	JPAREA(16,"Hokkaido(Muroran)",363),			/* 北海道（室蘭）*/
	JPAREA(17,"Miyagi",1880),					/* 宮城 */
	JPAREA(18,"Akita",2758),					/* 秋田 */
	JPAREA(19,"Yamagata",3660),					/* 山形 */
	JPAREA(20,"Iwate",1492),					/* 岩手 */
	JPAREA(21,"Fukushima",430),					/* 福島 */
	JPAREA(22,"Aomori",1127),					/* 青森 */
	JPAREA(23,"Tokyo",2732),					/* 東京 */
	JPAREA(24,"Kanagawa",1388),					/* 神奈川 */
	JPAREA(25,"Gunma",2443),					/* 群馬 */
	JPAREA(26,"Ibaraki",3177),					/* 茨城 */
	JPAREA(27,"Chiba",455),						/* 千葉 */
	JPAREA(28,"Tochigi",3640),					/* 栃木 */
	JPAREA(29,"Saitama",1611),					/* 埼玉 */
	JPAREA(30,"Nagano",2514),					/* 長野 */
	JPAREA(31,"Niigata",1230),					/* 新潟 */
	JPAREA(32,"Yamanashi",3402),				/* 山梨 */
	JPAREA(33,"Aichi",2406),					/* 愛知 */
	JPAREA(34,"Ishikawa",1702),					/* 石川 */
	JPAREA(35,"Shizuoka",2650),					/* 静岡 */
	JPAREA(36,"Fukui",2349),					/* 福井 */
	JPAREA(37,"Toyama",1337),					/* 富山 */
	JPAREA(38,"Mie",732),						/* 三重 */
	JPAREA(39,"Gifu",2661),						/* 岐阜 */
	JPAREA(40,"Osaka",3250),					/* 大阪 */
	JPAREA(41,"Kyoto",1434),					/* 京都 */
	JPAREA(42,"Hyogo",1652),					/* 兵庫 */
	JPAREA(43,"Wakayama",918),					/* 和歌山 */
	JPAREA(44,"Nara",2707),						/* 奈良 */
	JPAREA(45,"Shiga",3300),					/* 滋賀 */
	JPAREA(46,"Hiroshima",2865),				/* 広島 */
	JPAREA(47,"Okayama",693),					/* 岡山 */
	JPAREA(48,"Shimane",795),					/* 島根 */
	JPAREA(49,"Tottori",3363),					/* 鳥取 */
	JPAREA(50,"Yamaguchi",2968),				/* 山口 */
	JPAREA(51,"Ehime",413),						/* 愛媛 */
	JPAREA(52,"Kagawa",2484),					/* 香川 */
	JPAREA(53,"Tokushima",3682),				/* 徳島 */
	JPAREA(54,"Kouchi",739),					/* 高知 */
	JPAREA(55,"Fukuoka",1581),					/* 福岡 */
	JPAREA(56,"Kumamoto",2215),					/* 熊本 */
	JPAREA(57,"Nagasaki",2603),					/* 長崎 */
	JPAREA(58,"Kagoshima",3397),				/* 鹿児島 */
	JPAREA(59,"Miyazaki",3356),					/* 宮崎 */
	JPAREA(60,"Oita",3213),						/* 大分 */
	JPAREA(61,"Saga",2393),						/* 佐賀 */
	JPAREA(62,"Okinawa",882),					/* 沖縄 */
	JPAREA_NULL(63),
};

#define ARIB_JP_AREA_NUMS						(sizeof(arib_jp_area_tables) / sizeof(arib_jp_area))
#define ARIB_JP_AREA_CODE_DEFAULT				2732

#endif

