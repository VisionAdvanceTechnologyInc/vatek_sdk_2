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

#ifndef DTV_CHANNEL_H
#define DTV_CHANNEL_H


#include <stdint.h>

typedef struct _qdtv_ch
{
	const char* name;
	uint32_t freqency;
}qdtv_ch, * Pqdtv_ch;

typedef struct _qdtv_ch_table
{
	const char* name;
	const Pqdtv_ch channels;
}qdtv_ch_table, * Pqdtv_ch_table;

#define _uchannel_start(tag)	static const qdtv_ch uchannel_##tag[] = {
#define _uchannel(n,f)			{#n,f,},
#define _uchannel_end			{NULL,0,}};

#define _uchannel_get(tag)			((const Pqdtv_ch)&uchannel_##tag[0])

#define _uchannel_table(tag)		static const qdtv_ch_table uchannel_table_##tag = {#tag,_uchannel_get(##tag),};
#define _uchannel_table_get(tag)	((const Pqdtv_ch_table)&uchannel_table_##tag)

_uchannel_start(uhf_6)
	_uchannel(ch_14, 473000)
	_uchannel(ch_15, 479000)
	_uchannel(ch_16, 485000)
	_uchannel(ch_17, 491000)
	_uchannel(ch_18, 497000)
	_uchannel(ch_19, 503000)
	_uchannel(ch_20, 509000)
	_uchannel(ch_21, 515000)
	_uchannel(ch_22, 521000)
	_uchannel(ch_23, 527000)
	_uchannel(ch_24, 533000)
	_uchannel(ch_25, 539000)
	_uchannel(ch_26, 545000)
	_uchannel(ch_27, 551000)
	_uchannel(ch_28, 557000)
	_uchannel(ch_29, 563000)
	_uchannel(ch_30, 569000)
	_uchannel(ch_31, 575000)
	_uchannel(ch_32, 581000)
	_uchannel(ch_33, 587000)
	_uchannel(ch_34, 593000)
	_uchannel(ch_35, 599000)
	_uchannel(ch_36, 605000)
	_uchannel(ch_37, 611000)
	_uchannel(ch_38, 617000)
	_uchannel(ch_39, 623000)
	_uchannel(ch_40, 629000)
	_uchannel(ch_41, 635000)
	_uchannel(ch_42, 641000)
	_uchannel(ch_43, 647000)
	_uchannel(ch_44, 653000)
	_uchannel(ch_45, 659000)
	_uchannel(ch_46, 665000)
	_uchannel(ch_47, 671000)
	_uchannel(ch_48, 677000)
	_uchannel(ch_49, 683000)
	_uchannel(ch_50, 689000)
	_uchannel(ch_51, 695000)
	_uchannel(ch_52, 701000)
	_uchannel(ch_53, 707000)
	_uchannel(ch_54, 713000)
	_uchannel(ch_55, 719000)
	_uchannel(ch_56, 725000)
	_uchannel(ch_57, 731000)
	_uchannel(ch_58, 737000)
	_uchannel(ch_59, 743000)
	_uchannel(ch_60, 749000)
	_uchannel(ch_61, 755000)
	_uchannel(ch_62, 761000)
_uchannel_end

_uchannel_start(jp_uhf_6)
	_uchannel(ch_14, 473143)
	_uchannel(ch_15, 479143)
	_uchannel(ch_16, 485143)
	_uchannel(ch_17, 491143)
	_uchannel(ch_18, 497143)
	_uchannel(ch_19, 503143)
	_uchannel(ch_20, 509143)
	_uchannel(ch_21, 515143)
	_uchannel(ch_22, 521143)
	_uchannel(ch_23, 527143)
	_uchannel(ch_24, 533143)
	_uchannel(ch_25, 539143)
	_uchannel(ch_26, 545143)
	_uchannel(ch_27, 551143)
	_uchannel(ch_28, 557143)
	_uchannel(ch_29, 563143)
	_uchannel(ch_30, 569143)
	_uchannel(ch_31, 575143)
	_uchannel(ch_32, 581143)
	_uchannel(ch_33, 587143)
	_uchannel(ch_34, 593143)
	_uchannel(ch_35, 599143)
	_uchannel(ch_36, 605143)
	_uchannel(ch_37, 611143)
	_uchannel(ch_38, 617143)
	_uchannel(ch_39, 623143)
	_uchannel(ch_40, 629143)
	_uchannel(ch_41, 635143)
	_uchannel(ch_42, 641143)
	_uchannel(ch_43, 647143)
	_uchannel(ch_44, 653143)
	_uchannel(ch_45, 659143)
	_uchannel(ch_46, 665143)
	_uchannel(ch_47, 671143)
	_uchannel(ch_48, 677143)
	_uchannel(ch_49, 683143)
	_uchannel(ch_50, 689143)
	_uchannel(ch_51, 695143)
	_uchannel(ch_52, 701143)
	_uchannel(ch_53, 707143)
	_uchannel(ch_54, 713143)
	_uchannel(ch_55, 719143)
	_uchannel(ch_56, 725143)
	_uchannel(ch_57, 731143)
	_uchannel(ch_58, 737143)
	_uchannel(ch_59, 743143)
	_uchannel(ch_60, 749143)
	_uchannel(ch_61, 755143)
	_uchannel(ch_62, 761143)
_uchannel_end

_uchannel_start(uhf_8)
	_uchannel(ch_21, 474000)
	_uchannel(ch_22, 482000)
	_uchannel(ch_23, 490000)
	_uchannel(ch_24, 498000)
	_uchannel(ch_25, 506000)
	_uchannel(ch_26, 514000)
	_uchannel(ch_27, 522000)
	_uchannel(ch_28, 530000)
	_uchannel(ch_29, 538000)
	_uchannel(ch_30, 546000)
	_uchannel(ch_31, 554000)
	_uchannel(ch_32, 562000)
	_uchannel(ch_33, 570000)
	_uchannel(ch_34, 578000)
	_uchannel(ch_35, 586000)
	_uchannel(ch_36, 594000)
	_uchannel(ch_37, 602000)
	_uchannel(ch_38, 610000)
	_uchannel(ch_39, 618000)
	_uchannel(ch_40, 626000)
	_uchannel(ch_41, 634000)
	_uchannel(ch_42, 642000)
	_uchannel(ch_43, 650000)
	_uchannel(ch_44, 658000)
	_uchannel(ch_45, 666000)
	_uchannel(ch_46, 674000)
	_uchannel(ch_47, 682000)
	_uchannel(ch_48, 690000)
	_uchannel(ch_49, 698000)
	_uchannel(ch_50, 706000)
	_uchannel(ch_51, 714000)
	_uchannel(ch_52, 722000)
	_uchannel(ch_53, 730000)
	_uchannel(ch_54, 738000)
	_uchannel(ch_55, 746000)
	_uchannel(ch_56, 754000)
	_uchannel(ch_57, 762000)
	_uchannel(ch_58, 770000)
	_uchannel(ch_59, 778000)
	_uchannel(ch_60, 786000)
	_uchannel(ch_61, 794000)
	_uchannel(ch_62, 802000)
	_uchannel(ch_63, 810000)
	_uchannel(ch_64, 818000)
	_uchannel(ch_65, 825000)
	_uchannel(ch_66, 834000)
	_uchannel(ch_67, 842000)
	_uchannel(ch_68, 850000)
	_uchannel(ch_69, 858000)
_uchannel_end

_uchannel_table(uhf_6)
_uchannel_table(jp_uhf_6)
_uchannel_table(uhf_8)

#endif
