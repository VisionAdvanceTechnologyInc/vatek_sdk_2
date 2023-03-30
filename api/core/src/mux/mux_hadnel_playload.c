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

#include <mux/mux_handle.h>
#include <core/hal/halservice_base.h>
#include <core/hal/halreg_playload.h>
#include "./internal/mux_core.h"

extern vatek_result muxplayload_reset(hvatek_chip hchip);

extern vatek_result muxplayload_set_raw(hvatek_chip hchip,Ppsi_table_raw praw);
extern vatek_result muxplayload_get_raw(hmux_core hmux,hvatek_chip hchip);

extern vatek_result muxplayload_set_broadcast_default(hvatek_chip hchip, Phandle_channel pbc);
extern vatek_result muxplayload_get_broadcast_default(hmux_core hmux, hvatek_chip hchip,uint16_t pcrpid,Pencoder_param penc);

extern vatek_result muxplayload_set_broadcast_rule(hvatek_chip hchip,Pmux_rule_block prule, Phandle_channel pbc);
extern vatek_result muxplayload_get_broadcast_rule(hmux_core hmux, hvatek_chip hchip, uint16_t pcrpid, Pencoder_param penc);

extern vatek_result muxplayload_set_channel_default(hvatek_chip hchip,Phandle_channel pch);
extern vatek_result muxplayload_get_channel_default(hmux_core hmux, hvatek_chip hchip);
extern vatek_result muxplayload_set_channel_rule(hvatek_chip hchip,Pmux_rule_block prule,Phandle_channel pch);
extern vatek_result muxplayload_get_channel_rule(hmux_core hmux, hvatek_chip hchip);

extern vatek_result muxplayload_set_channel(hvatek_chip hchip,Phandle_channel pch,uint32_t* addr);
extern vatek_result muxplayload_get_channel(hmux_core hmux, hvatek_chip hchip, hmux_channel hch, uint32_t* addr);
extern vatek_result muxplayload_set_program(hvatek_chip hchip,Phandle_channel pch,Pmux_program program,uint32_t* addr);
extern vatek_result muxplayload_get_program(hmux_core hmux, hvatek_chip hchip,hmux_channel hch, uint32_t* addr);
extern vatek_result muxplayload_set_stream(hvatek_chip hchip,Phandle_channel pch,Pmux_stream pstream,uint32_t* addr);
extern vatek_result muxplayload_get_stream(hmux_core hmux, hvatek_chip hchip,hmux_program hprog, uint32_t* addr);

extern vatek_result muxplayload_set_rule(hvatek_chip hchip,Pmux_rule_block prule,uint32_t* addr);
extern vatek_result muxplayload_get_rule(hmux_core hmux, hvatek_chip hchip,Pmux_rule_block* prule,uint32_t* addr);
extern vatek_result muxplayload_set_default(hvatek_chip hchip,mux_spec_mode spec,mux_country_code country,Ppsiparam_buffer pbuf,uint32_t* addr);

extern vatek_result muxplayload_get_default(hvatek_chip hchip, Ppsiparam_buffer pbuf, uint32_t* addr);

extern vatek_result muxplayload_set_epg(hvatek_chip hchip,Pmux_epg_param pepg,uint32_t* addr);
extern vatek_result muxplayload_get_epg(hmux_core hmux, hvatek_chip hchip, Pmux_epg_param* pepg, uint32_t* addr);

extern vatek_result muxplayload_set_epg_section(hvatek_chip hchip, Pmux_epg_section psection, uint32_t* addr);
extern vatek_result muxplayload_get_epg_section(hmux_core hmux, hvatek_chip hchip, Pmux_epg_section* psection, uint32_t* addr);
extern int32_t muxplayload_get_epg_section_nums(Pmux_epg_param pepg);


extern vatek_result muxplayload_wr_buf(hvatek_chip hchip, uint8_t* pbuf, int32_t len, uint32_t* pos);
extern vatek_result muxplayload_rd_buf(hvatek_chip hchip, uint8_t* pbuf, int32_t len, uint32_t* pos);

#define _wrhal(p,v)			vatek_chip_write_memory(hchip,p++,v)
#define _wrbuf(p,buf,len)	muxplayload_wr_buf(hchip,buf,len,&p)
#define _rdbuf(p,buf,len)	muxplayload_rd_buf(hchip,buf,len,&p)
#define _wlen(l)			((l + 3) >> 2)

#define _rdhal(p,v)			vatek_chip_read_memory(hchip,p++,&v)

vatek_result mux_handle_set_hal(hmux_core hmux, hvatek_chip hchip)
{
	vatek_result nres = vatek_badstatus;
	Phandle_mux pmux = (Phandle_mux)hmux;
	if(!mux_check_idle(pmux))
	{
		if(pmux->hmode == hmux_mode_broadcast)
		{
			if(pmux->mode == mux_psi_specdefault)
				nres = muxplayload_set_broadcast_default(hchip,pmux->_h.channel);
			else if(pmux->mode == mux_psi_specrule)
				nres = muxplayload_set_broadcast_rule(hchip,pmux->rule,pmux->_h.channel);
			else nres = vatek_badparam;
		}
		else if(pmux->hmode == hmux_mode_channel)
		{
			if(pmux->mode == mux_psi_specdefault)
				nres = muxplayload_set_channel_default(hchip,pmux->_h.channel);
			else if(pmux->mode == mux_psi_specrule)
				nres = muxplayload_set_channel_rule(hchip,pmux->rule,pmux->_h.channel);
			else nres = vatek_badparam;
		}
		else if(pmux->hmode == hmux_mode_raw)
			nres = muxplayload_set_raw(hchip,pmux->_h.raw->rawtables.rawtables);
		else nres = vatek_badparam;
	}

	if(!is_vatek_success(nres))muxplayload_reset(hchip);
	return nres;
}

vatek_result mux_handle_get_hal_broadcast(hmux_core hmux, hvatek_chip hchip, uint16_t pcrpid, Pencoder_param penc)
{
	vatek_result nres = vatek_badstatus;
	Phandle_mux pmux = (Phandle_mux)hmux;
	uint32_t pos = HALRANGE_PLAYLOAD_START;
	uint32_t val = 0;
	if (mux_check_idle(pmux))
	{
		nres = _rdhal(pos, val);
		if (is_vatek_success(nres))
		{
			if (val == RAWPSI_EN_TAG)nres = muxplayload_get_raw(hmux, hchip);
			else if (val == LICENSED_DEFSPEC_V1_EN_START)
				nres = muxplayload_get_broadcast_default(hmux, hchip, pcrpid, penc);
			else if (val == LICENSED_EN_START)
				nres = muxplayload_get_broadcast_rule(hmux, hchip, pcrpid, penc);
			else 
			{
				hmux_rawtable hraw = NULL;
				nres = mux_open_raw(hmux,&hraw);
				VWAR("mux_handle_get_hal_broadcast unhandle used raw : [%08x]",val);
			}
			muxplayload_reset(hchip);
			if (!is_vatek_success(nres))mux_handle_reset(hmux);
		}else VWAR("mux_check_idle not empty");
	}
	return nres;
}

vatek_result mux_handle_get_hal_channel(hmux_core hmux, hvatek_chip hchip)
{
	vatek_result nres = vatek_badstatus;
	Phandle_mux pmux = (Phandle_mux)hmux;
	uint32_t pos = HALRANGE_PLAYLOAD_START;
	uint32_t val = 0;
	if(mux_check_idle(pmux))
	{
		nres = _rdhal(pos,val);
		if(is_vatek_success(nres))
		{
			if (val == RAWPSI_EN_TAG)nres = muxplayload_get_raw(hmux, hchip);
			else if (val == LICENSED_DEFSPEC_V2_EN_START)
				nres = muxplayload_get_channel_default(hmux, hchip);
			else if (val == LICENSED_V2_EN_START)
				nres = muxplayload_get_channel_rule(hmux, hchip);
			else 
			{
				hmux_rawtable hraw = NULL;
				nres = mux_open_raw(hmux,&hraw);
			}
			muxplayload_reset(hchip);
			if (!is_vatek_success(nres))mux_handle_reset(hmux);
		}
	}else VWAR("mux_check_idle not empty");
	return nres;
}

vatek_result muxplayload_set_broadcast_default(hvatek_chip hchip, Phandle_channel pbc)
{
	vatek_result nres = muxplayload_reset(hchip);
	uint32_t pos = HALRANGE_PLAYLOAD_START;

	if(is_vatek_success(nres))
	{
		nres = muxrule_set_channel_rawbuf(pbc->context.specchannel,pbc->buffer.ch_buf);
		if (is_vatek_success(nres))
		{
			Ppsispec_program ppsiprog = (Ppsispec_program)pbc->context.programs;
			nres = muxrule_set_program_rawbuf(ppsiprog->specprogram, pbc->buffer.prog_buf);
		}

		if(is_vatek_success(nres))
		{
			nres = _wrhal(pos, LICENSED_DEFSPEC_V1_EN_START);
			if (is_vatek_success(nres))
			{
				nres = muxplayload_set_default(hchip,pbc->spec,pbc->country,&pbc->buffer,&pos);
				if (is_vatek_success(nres))
				{
					Pmux_stream pstream = pbc->context.programs->streams;
					while (pstream)
					{
						if(pstream->type == mux_stream_data)
							nres = muxplayload_set_stream(hchip, pbc, pstream, &pos);
						if(is_vatek_success(nres))
							pstream = pstream->next;
						else break;
					}
				}
				if (is_vatek_success(nres))nres = _wrhal(pos, LICENSED_DEFSPEC_EN_END);
			}
		}
	}
	return nres;
}

vatek_result muxplayload_get_broadcast_default(hmux_core hmux, hvatek_chip hchip, uint16_t pcrpid, Pencoder_param penc)
{
	uint32_t pos = HALRANGE_PLAYLOAD_START;
	uint32_t val = 0;
	vatek_result nres = _rdhal(pos, val);
	if (is_vatek_success(nres))
	{
		if (val == LICENSED_DEFSPEC_V1_EN_START)
		{
			mux_spec_mode spec = mux_spec_undefine;
			mux_country_code country = mux_country_undefined;
			nres = _rdhal(pos, val);
			if (is_vatek_success(nres))
			{
				spec = (mux_spec_mode)val;
				nres = _rdhal(pos, val);
				if (is_vatek_success(nres))country = (mux_country_code)val;
			}

			if (is_vatek_success(nres))
			{
				hmux_broadcast hbc = NULL;
				nres = mux_open_broadcast_default(hmux, pcrpid, penc, spec, country, &hbc);
				if (is_vatek_success(nres))
				{
					Phandle_mux pmux = (Phandle_mux)hmux;
					Ppsiparam_buffer pbuf = &pmux->_h.channel->buffer;
					nres = muxplayload_get_default(hchip, pbuf, &pos);
					if (is_vatek_success(nres))
					{
						hmux_program hprog = (hmux_program)pmux->_h.channel->context.programs;
						for (;;)
						{
							nres = _rdhal(pos, val);
							if (is_vatek_success(nres))
							{
								if (val == STREAM_EN_START)
								{
									pos--;
									nres = muxplayload_get_stream(hmux, hchip, hprog, &pos);
								}
								else break;
							}
							if (!is_vatek_success(nres))break;
						}
					}

					if (is_vatek_success(nres))
					{
						Ppsispec_program ppsiprog = (Ppsispec_program)pmux->_h.channel->context.programs;
						nres = muxrule_get_channel_rawbuf(pmux->_h.channel->context.specchannel, &pbuf->ch_buf[0]);
						if (is_vatek_success(nres))
							nres = muxrule_get_program_rawbuf(ppsiprog->specprogram, &pbuf->prog_buf[0]);
					}
				}
			}
		}
		else nres = vatek_format;
	}
	return nres;
}

vatek_result muxplayload_set_broadcast_rule(hvatek_chip hchip,Pmux_rule_block prule, Phandle_channel pbc)
{
	uint32_t pos = HALRANGE_PLAYLOAD_START;
	vatek_result nres = muxplayload_reset(hchip);
	if(is_vatek_success(nres))
	{
		Ppsispec_program ppsiprog = (Ppsispec_program)pbc->context.programs;
		nres= muxrule_set_channel_param(prule, pbc->context.specchannel);
		if (is_vatek_success(nres))
			nres = muxrule_set_program_param(prule, ppsiprog->specprogram);
		
		if(is_vatek_success(nres))
		{
			nres = _wrhal(pos, LICENSED_EN_START);
			if(is_vatek_success(nres))
				nres = muxplayload_set_rule(hchip,prule,&pos);

			if (is_vatek_success(nres))
			{
				if (is_mux_rule_en_eit(prule) && ppsiprog->pepg)
					nres = muxplayload_set_epg(hchip, ppsiprog->pepg, &pos);
				if (is_vatek_success(nres))
				{
					Pmux_stream pstream = pbc->context.programs->streams;
					while (pstream)
					{
						if (pstream->type == mux_stream_data)
							nres = muxplayload_set_stream(hchip, pbc, pstream, &pos);
						if (is_vatek_success(nres))
							pstream = pstream->next;
						else break;
					}
				}
			}
		}
		if (is_vatek_success(nres))nres = _wrhal(pos, LICENSED_EN_END);
	}
	return nres;
}

vatek_result muxplayload_get_broadcast_rule(hmux_core hmux, hvatek_chip hchip, uint16_t pcrpid, Pencoder_param penc)
{
	uint32_t pos = HALRANGE_PLAYLOAD_START;
	uint32_t val = 0;
	vatek_result nres = _rdhal(pos, val);
	if (is_vatek_success(nres))
	{
		if (val == LICENSED_EN_START)
		{
			Pmux_rule_block pblock = NULL;
			nres = muxplayload_get_rule(hmux, hchip, &pblock, &pos);
			if (is_vatek_success(nres))
			{
				Pmux_epg_param newepg = NULL;
				hmux_broadcast hbc = NULL;

				nres = muxplayload_get_epg(hmux, hchip, &newepg, &pos);
				if (nres == vatek_unsupport)nres = vatek_success;

				if (is_vatek_success(nres))
				{
					Phandle_mux pmux = (Phandle_mux)hmux;
					hmux_program hprog = (Pmux_program)pmux->_h.channel->context.programs;
					for (;;)
					{
						nres = _rdhal(pos, val);
						if (is_vatek_success(nres))
						{
							if (val == STREAM_EN_START)
							{
								pos--;
								nres = muxplayload_get_stream(hmux, hchip, hprog, &pos);
							}
							else break;
						}
						if (!is_vatek_success(nres))break;
					}
				}

				if (is_vatek_success(nres))
				{
					nres = _rdhal(pos, val);
					if (is_vatek_success(nres))
					{
						if (val != LICENSED_EN_END)nres = vatek_format;
					}
				}

				if (is_vatek_success(nres))
				{
					nres = mux_open_broadcast_rule_raw(hmux, pcrpid, penc, pblock, &hbc);
					if (is_vatek_success(nres))
					{
						Phandle_mux pmux = (Phandle_mux)hmux;
						Ppsispec_program pprog = (Ppsispec_program)pmux->_h.channel->context.programs;
						pprog->pepg = newepg;
					}
				}
			}
		}
	}
	return nres;
}

vatek_result muxplayload_set_channel_default(hvatek_chip hchip,Phandle_channel pch)
{
	uint32_t pos = HALRANGE_PLAYLOAD_START;
	vatek_result nres = muxplayload_reset(hchip);
	if(is_vatek_success(nres))
	{
		nres = muxrule_set_channel_rawbuf(pch->context.specchannel,pch->buffer.ch_buf);
		if(is_vatek_success(nres))
		{
			nres = _wrhal(pos, LICENSED_DEFSPEC_V2_EN_START);
			if (is_vatek_success(nres))nres = _wrhal(pos, RULE_EN_START);
			if (is_vatek_success(nres))nres = _wrhal(pos, pch->spec);
			if (is_vatek_success(nres))nres = _wrhal(pos, pch->country);
			if (is_vatek_success(nres))nres = _wrhal(pos, pch->buffer.ch_len);
			if (is_vatek_success(nres))nres = _wrbuf(pos, pch->buffer.ch_buf, pch->buffer.ch_len);
			if (is_vatek_success(nres))nres = _wrhal(pos, RULE_EN_END);

			if(is_vatek_success(nres))
				nres = muxplayload_set_channel(hchip,pch,&pos);
			if(is_vatek_success(nres))
				nres = _wrhal(pos,LICENSED_DEFSPEC_EN_END);
		}

	}
	return nres;
}

vatek_result muxplayload_get_channel_default(hmux_core hmux, hvatek_chip hchip)
{
	uint32_t pos = HALRANGE_PLAYLOAD_START;
	uint32_t val = 0;
	vatek_result nres = _rdhal(pos, val);
	if (is_vatek_success(nres))
	{
		if (val == LICENSED_DEFSPEC_V2_EN_START)
		{
			nres = _rdhal(pos, val);
			if (is_vatek_success(nres))
			{
				if (val != RULE_EN_START)nres = vatek_format;
			}

			if (is_vatek_success(nres))
			{
				mux_spec_mode spec = mux_spec_undefine;
				mux_country_code country = mux_country_undefined;
				nres = _rdhal(pos, val);
				if (is_vatek_success(nres))
				{
					spec = (mux_spec_mode)val;
					nres = _rdhal(pos, val);
					if (is_vatek_success(nres))country = (mux_country_code)val;
					if (is_vatek_success(nres))
					{
						hmux_channel hch = NULL;
						nres = mux_open_channel_default(hmux, spec, country, &hch);
						if (is_vatek_success(nres))
						{
							Phandle_channel pch = ((Phandle_mux)hmux)->_h.channel;
							nres = _rdhal(pos, val);
							if (is_vatek_success(nres))
							{
								if (val == pch->buffer.ch_len)
								{
									nres = _rdbuf(pos, &pch->buffer.ch_buf[0], pch->buffer.ch_len);
									if(is_vatek_success(nres))
										nres = muxrule_get_channel_rawbuf(pch->context.specchannel, pch->buffer.ch_buf);
									if (is_vatek_success(nres))nres = _rdhal(pos, val);
								}
							}
							if (is_vatek_success(nres))
							{
								if (val != RULE_EN_END)nres = vatek_format;
							}
						}

						if (is_vatek_success(nres))
						{
							nres = muxplayload_get_channel(hmux, hchip, hch, &pos);
						}
					}
				}
			}

			if (is_vatek_success(nres))
			{
				nres = _rdhal(pos, val);
				if (is_vatek_success(nres) && val != LICENSED_DEFSPEC_EN_END)nres = vatek_format;
			}
		}
		else nres = vatek_format;
	}
	return nres;
}

vatek_result muxplayload_set_channel_rule(hvatek_chip hchip,Pmux_rule_block prule,Phandle_channel pch)
{
	uint32_t pos = HALRANGE_PLAYLOAD_START;
	vatek_result nres = muxplayload_reset(hchip);
	if(is_vatek_success(nres))
	{
		nres = muxrule_set_channel_param(prule,pch->context.specchannel);
		if(is_vatek_success(nres))
		{
			nres = _wrhal(pos, LICENSED_V2_EN_START);
			if (is_vatek_success(nres))
				nres = muxplayload_set_rule(hchip,prule,&pos);
		}

		if(is_vatek_success(nres))
			nres = muxplayload_set_channel(hchip,pch,&pos);
		if(is_vatek_success(nres))
			nres = _wrhal(pos,LICENSED_V2_EN_END);
	}
	return nres;
}

vatek_result muxplayload_get_channel_rule(hmux_core hmux, hvatek_chip hchip)
{
	uint32_t pos = HALRANGE_PLAYLOAD_START;
	uint32_t val = 0;
	vatek_result nres = _rdhal(pos, val);
	if (is_vatek_success(nres) && val != LICENSED_V2_EN_START)nres = vatek_format;
	if (is_vatek_success(nres))
	{
		Pmux_rule_block pblock = NULL;
		nres = muxplayload_get_rule(hmux, hchip, &pblock, &pos);
		if (is_vatek_success(nres))
		{
			hmux_channel hch = NULL;
			nres = mux_open_channel_rule_raw(hmux, pblock, &hch);
			if (is_vatek_success(nres))
			{
				nres = muxplayload_get_channel(hmux, hchip, hch, &pos);
				if (is_vatek_success(nres))
				{
					nres = _rdhal(pos, val);
					if (is_vatek_success(nres) && val != LICENSED_V2_EN_END)nres = vatek_format;
				}
			}
		}
	}
	return nres;
}

vatek_result muxplayload_set_raw(hvatek_chip hchip, Ppsi_table_raw praw)
{
	vatek_result nres = muxplayload_reset(hchip);
	uint32_t pos = HALRANGE_PLAYLOAD_START;
	if (is_vatek_success(nres))
	{
		while (praw)
		{
			nres = _wrhal(pos, RAWPSI_EN_TAG);
			if (is_vatek_success(nres))
				nres = _wrhal(pos, praw->interval_ms);
			if (is_vatek_success(nres))
				nres = _wrhal(pos, praw->packet_nums);
			if (is_vatek_success(nres))
				nres = _wrbuf(pos, &praw->packet_buf[0], praw->packet_nums * TS_PACKET_LEN);

			if (is_vatek_success(nres) && pos > HALRANGE_PLAYLOAD_END)
				nres = vatek_memfail;
			if (!is_vatek_success(nres))break;
			praw = praw->next;
		}

	}
	return nres;
}

vatek_result muxplayload_get_raw(hmux_core hcore,hvatek_chip hchip)
{
	uint32_t pos = HALRANGE_PLAYLOAD_START;
	uint32_t val = 0;
	hmux_rawtable hraw = NULL;
	vatek_result nres = mux_open_raw(hcore, &hraw);
	if (is_vatek_success(nres))
	{
		nres = _rdhal(pos, val);

		while (is_vatek_success(nres))
		{
			if (val == RAWPSI_EN_TAG)
			{
				uint32_t intervalms = 0;
				uint32_t pkgnums = 0;
				nres = _rdhal(pos, intervalms);
				if (is_vatek_success(nres))
					nres = _rdhal(pos, pkgnums);
				if (is_vatek_success(nres))
				{
					Ppsi_table_raw newpsi = NULL;
					nres = muxraw_create_table(hraw, (int32_t)pkgnums, (int32_t)intervalms, &newpsi);
					if (is_vatek_success(nres))
					{
						nres = _rdbuf(pos, &newpsi->packet_buf[0], pkgnums * TS_PACKET_LEN);
						if (is_vatek_success(nres))
							nres = muxraw_push_table(hraw, newpsi);
					}
				}

				if (is_vatek_success(nres))nres = _rdhal(pos, val);
			}
			else if (val == RAWPSI_EN_ENDTAG)break;
			else
			{
				Ppsi_table_raw ptables = NULL;
				nres = muxraw_get_table(hraw,&ptables);
				if (is_vatek_success(nres))
				{
					if (!ptables)nres = vatek_format;
				}
				break;
			}
		}
	}
	return nres;
}

vatek_result muxplayload_reset(hvatek_chip hchip)
{
	return writehal(HALRANGE_PLAYLOAD_START,0);
}

vatek_result muxplayload_wr_buf(hvatek_chip hchip, uint8_t* pbuf, int32_t len, uint32_t* pos)
{
	uint32_t licensed_pos = *pos;
	int32_t wlen = _wlen(len);
	vatek_result nres = vatek_chip_write_buffer(hchip, licensed_pos,&pbuf[0],wlen);
	if (is_vatek_success(nres))
		*pos = licensed_pos + wlen;
	return nres;
}

vatek_result muxplayload_rd_buf(hvatek_chip hchip, uint8_t* pbuf, int32_t len, uint32_t* pos)
{
	uint32_t licensed_pos = *pos;
	int32_t wlen = _wlen(len);
	vatek_result nres = vatek_chip_read_buffer(hchip, licensed_pos, &pbuf[0], wlen);
	if (is_vatek_success(nres))
		*pos = licensed_pos + wlen;
	return nres;
}

vatek_result muxplayload_set_channel(hvatek_chip hchip,Phandle_channel pch,uint32_t* addr)
{
	uint32_t pos = *addr;
	Pmux_program ptrprog = pch->context.programs;
	vatek_result nres = vatek_success;
	while(ptrprog)
	{
		nres = muxplayload_set_program(hchip,pch,ptrprog,&pos);
		if(!is_vatek_success(nres))break;
		ptrprog = ptrprog->next;
	}

	if(is_vatek_success(nres))*addr = pos;
	return nres;
}

vatek_result muxplayload_get_channel(hmux_core hmux, hvatek_chip hchip, hmux_channel hch, uint32_t* addr)
{
	vatek_result nres = vatek_success;
	uint32_t pos = *addr;
	while (is_vatek_success(nres))
	{
		nres = muxplayload_get_program(hmux, hchip, hch, &pos);
	}
	if (nres == vatek_unsupport)nres = vatek_success;
	if (is_vatek_success(nres))*addr = pos;
	return nres;
}

vatek_result muxplayload_set_program(hvatek_chip hchip,Phandle_channel pch,Pmux_program program,uint32_t* addr)
{
	uint32_t pos = *addr;
	Ppsispec_program progpsi = (Ppsispec_program)program;
	vatek_result nres = _wrhal(pos, PROGRAM_EN_START);
	if (is_vatek_success(nres))
	{
		nres = _wrhal(pos, program->pcr_pid);
		if (is_vatek_success(nres))nres = _wrhal(pos, program->pmt_pid);
	}

	if(is_vatek_success(nres))
	{
		nres = muxrule_set_program_rawbuf(progpsi->specprogram,pch->buffer.prog_buf);
		if(is_vatek_success(nres))
		{
			nres = _wrhal(pos,pch->buffer.prog_len);
			if(is_vatek_success(nres))nres = _wrbuf(pos,pch->buffer.prog_buf,pch->buffer.prog_len);

			if(progpsi->pepg)
				nres = muxplayload_set_epg(hchip,progpsi->pepg,&pos);
		}
	}

	if(is_vatek_success(nres))
	{
		Pmux_stream ptrstream = program->streams;
		while(ptrstream)
		{
			nres = muxplayload_set_stream(hchip,pch,ptrstream,&pos);
			if(!is_vatek_success(nres))break;
			ptrstream = ptrstream->next;
		}
	}

	if(is_vatek_success(nres))
		nres = _wrhal(pos,PROGRAM_EN_END);

	if(is_vatek_success(nres))*addr = pos;
	return nres;
}

vatek_result muxplayload_get_program(hmux_core hmux, hvatek_chip hchip, hmux_channel hch, uint32_t* addr)
{
	uint32_t pos = *addr;
	uint32_t val = 0;
	Phandle_channel pch = ((Phandle_mux)hmux)->_h.channel;
	vatek_result nres = _rdhal(pos, val);
	if (is_vatek_success(nres) && val != PROGRAM_EN_START)nres = vatek_unsupport;
	if (is_vatek_success(nres))
	{
		uint32_t pcrpid = 0;
		uint32_t pmtpid = 0;
		nres = _rdhal(pos, pcrpid);
		if (is_vatek_success(nres))nres = _rdhal(pos, pmtpid);
		if (is_vatek_success(nres))
		{
			hmux_program hprog = NULL;
			nres = muxchannel_add_program(hch, (uint16_t)pmtpid, (uint16_t)pcrpid, &hprog);
			if (is_vatek_success(nres))
			{
				nres = _rdhal(pos, val);
				if (is_vatek_success(nres) && val != pch->buffer.prog_len)nres = vatek_format;
				if (is_vatek_success(nres))
				{
					nres = _rdbuf(pos, &pch->buffer.prog_buf[0], pch->buffer.prog_len);
					if (is_vatek_success(nres))
					{
						Pspec_program pprog = muxprogram_get_param(hprog);
						nres = muxrule_get_program_rawbuf(pprog, &pch->buffer.prog_buf[0]);
						if (is_vatek_success(nres))
						{
							Pmux_epg_param pepg = NULL;
							nres = muxplayload_get_epg(hmux, hchip, &pepg, &pos);
							if (is_vatek_success(nres))
								nres = muxprogram_set_epg(hprog, pepg);
							else if (nres == vatek_unsupport)nres = vatek_success;
						}
					}
				}

				while (is_vatek_success(nres))
				{
					nres = muxplayload_get_stream(hmux, hchip, hprog, &pos);
				}
				if (nres == vatek_unsupport)nres = vatek_success;
			}

			if (is_vatek_success(nres))
			{
				nres = _rdhal(pos, val);
				if (is_vatek_success(nres) && val != PROGRAM_EN_END)nres = vatek_format;
			}
		}
	}
	if (is_vatek_success(nres))*addr = pos;
	return nres;
}

vatek_result muxplayload_set_stream(hvatek_chip hchip,Phandle_channel pch,Pmux_stream pstream,uint32_t* addr)
{
	uint32_t pos = *addr;
	vatek_result nres = _wrhal(pos, STREAM_EN_START);
	if (is_vatek_success(nres))
	{
		nres = _wrhal(pos, pstream->type);
		if (is_vatek_success(nres))
			nres = _wrhal(pos, pstream->stream_pid);
		if (is_vatek_success(nres))
			nres = _wrhal(pos, pstream->stream_type);
		if (is_vatek_success(nres))
			nres = _wrhal(pos, pstream->es_info_len);

		if (is_vatek_success(nres))
		{
			uint32_t prvpos = pos;
			if (pstream->type == mux_stream_video)
			{
				Pmedia_video pvideo = &pstream->_stream_info.video;
				nres = _wrhal(pos, pvideo->vcodec);
				if (is_vatek_success(nres))
					nres = _wrhal(pos, pvideo->resolution);
				if (is_vatek_success(nres))
					nres = _wrhal(pos, pvideo->framerate);
				if (is_vatek_success(nres))
					nres = _wrhal(pos, pvideo->aspectrate);
			}
			else if (pstream->type == mux_stream_audio)
			{
				Pmedia_audio paudio = &pstream->_stream_info.audio;
				nres = _wrhal(pos, paudio->acodec);
				if (is_vatek_success(nres))
					nres = _wrhal(pos, paudio->samplerate);
				if (is_vatek_success(nres))
					nres = _wrhal(pos, paudio->channel);
			}
			pos = prvpos + 12;
			if (is_vatek_success(nres) && pstream->es_info_len)
				nres = _wrbuf(pos, &pstream->es_info_buf[0], pstream->es_info_len);
			if (is_vatek_success(nres))
				nres = _wrhal(pos, STREAM_EN_END);
		}
	}

	if(is_vatek_success(nres))*addr = pos;
	return nres;
}

vatek_result muxplayload_get_stream(hmux_core hmux, hvatek_chip hchip, hmux_program hprog, uint32_t* addr)
{
	uint32_t pos = *addr;
	uint32_t val = 0;
	vatek_result nres = _rdhal(pos, val);
	if (is_vatek_success(nres) && val != STREAM_EN_START)nres = vatek_unsupport;
	if (is_vatek_success(nres))
	{
		uint32_t type, stream_pid, es_info_len,stream_type;
		hmux_stream hstream = NULL;
		nres = _rdhal(pos, type);
		if (is_vatek_success(nres))nres = _rdhal(pos, stream_pid);
		if (is_vatek_success(nres))nres = _rdhal(pos, stream_type); //stream_type
		if (is_vatek_success(nres))nres = _rdhal(pos, es_info_len);
		if (is_vatek_success(nres))
		{
			mux_stream_type stype = (mux_stream_type)type;
			uint32_t alignpos = pos;
			if (stype == mux_stream_video)
			{
				media_video video;
				nres = _rdhal(pos, val);
				if (is_vatek_success(nres))
				{
					video.vcodec = (video_codec)val;
					nres = _rdhal(pos, val);
					if (is_vatek_success(nres))
					{
						video.resolution = (video_resolution)val;
						nres = _rdhal(pos, val);
						if (is_vatek_success(nres))
						{
							video.framerate = (video_framerate)val;
							nres = _rdhal(pos, val);
							if (is_vatek_success(nres))video.aspectrate = (video_aspect_rate)val;
						}
					}
				}
				if (is_vatek_success(nres))
					nres = muxprogram_add_video(hprog, (uint16_t)stream_pid, &video, &hstream);
			}
			else if (stype == mux_stream_audio)
			{
				media_audio audio;
				nres = _rdhal(pos, val);
				if (is_vatek_success(nres))
				{
					audio.acodec = (audio_codec)val;
					nres = _rdhal(pos, val);
					if (is_vatek_success(nres))
					{
						audio.samplerate = (audio_samplerate)val;
						nres = _rdhal(pos, val);
						if (is_vatek_success(nres))audio.channel = (audio_channel)val;
					}
				}
				if (is_vatek_success(nres))
					nres = muxprogram_add_audio(hprog, (uint16_t)stream_pid, &audio, &hstream);
			}
			else if (stype == mux_stream_data)
			{
				nres = muxprogram_add_stream(hprog, (uint16_t)stream_pid, (uint8_t)stream_type ,&hstream);
			}
			else
			{
				VWAR("unknown mux_stream type : %d", stype);
				nres = vatek_format;
			}

			pos = alignpos + 12;
			if (is_vatek_success(nres) && es_info_len)
			{
				Pmux_stream pstream = muxstream_get(hstream);
				pstream->es_info_len = (uint8_t)es_info_len;
				pstream->es_info_buf = mux_handle_malloc_buffer(hmux, PES_STREAM_ES_INFO_LEN);
				if (pstream->es_info_buf)
					nres = _rdbuf(pos, &pstream->es_info_buf[0], es_info_len);
				else nres = vatek_memfail;
			}

			if (is_vatek_success(nres))
			{
				nres = _rdhal(pos, val);
				if (is_vatek_success(nres) && val != STREAM_EN_END)
					nres = vatek_format;
			}
		}

	}
	if (is_vatek_success(nres))*addr = pos;
	return nres;
}

vatek_result muxplayload_set_rule(hvatek_chip hchip,Pmux_rule_block prule,uint32_t* addr)
{
	uint32_t pos = *addr;
	vatek_result nres = _wrhal(pos, RULE_EN_START);
	if (is_vatek_success(nres))nres = _wrhal(pos, prule->block_len);
	if (is_vatek_success(nres))nres = _wrbuf(pos, &prule->rawblock[0],prule->block_len);
	if (is_vatek_success(nres))nres = _wrhal(pos, RULE_EN_END);

	if(is_vatek_success(nres))
		*addr = pos;
	return nres;
}

vatek_result muxplayload_get_rule(hmux_core hmux, hvatek_chip hchip, Pmux_rule_block* prule, uint32_t* addr)
{
	uint32_t pos = *addr;
	uint32_t val = 0;
	vatek_result nres = _rdhal(pos, val);
	if (is_vatek_success(nres))
	{
		if (val == RULE_EN_START)
		{
			nres = _rdhal(pos, val);
			if (is_vatek_success(nres))
			{
				uint8_t* pblock = mux_handle_malloc_buffer(hmux, (int32_t)val);
				nres = vatek_memfail;
				if (pblock)nres = _rdbuf(pos, pblock, (int32_t)val);
				if (is_vatek_success(nres))
				{
					nres = muxrule_load_raw(mux_handle_get_mempool(hmux), pblock, (int32_t)val, prule);
					if (is_vatek_success(nres))
					{
						nres = _rdhal(pos, val);
						if (is_vatek_success(nres))
						{
							if (val != RULE_EN_END)nres = vatek_format;
						}
					}
				}
			}
		}
	}
	if (is_vatek_success(nres))*addr = pos;
	return nres;
}

vatek_result muxplayload_set_default(hvatek_chip hchip,mux_spec_mode spec,mux_country_code country,Ppsiparam_buffer pbuf,uint32_t* addr)
{
	uint32_t pos = *addr;
	vatek_result nres = _wrhal(pos, spec);
	if (is_vatek_success(nres))nres = _wrhal(pos, country);
	if (is_vatek_success(nres))nres = _wrhal(pos, pbuf->ch_len);
	if (is_vatek_success(nres))nres = _wrbuf(pos, pbuf->ch_buf, pbuf->ch_len);
	if (is_vatek_success(nres))nres = _wrhal(pos, pbuf->prog_len);
	if (is_vatek_success(nres))nres = _wrbuf(pos, pbuf->prog_buf, pbuf->prog_len);

	if(is_vatek_success(nres))*addr = pos;
	return nres;
}

vatek_result muxplayload_get_default(hvatek_chip hchip, Ppsiparam_buffer pbuf, uint32_t* addr)
{
	uint32_t pos = *addr;
	uint32_t val = 0;
	vatek_result nres = _rdhal(pos, val);
	if (is_vatek_success(nres))
	{
		if (val == pbuf->ch_len)
		{
			nres = _rdbuf(pos, &pbuf->ch_buf[0], pbuf->ch_len);
			if (is_vatek_success(nres))
				nres = _rdhal(pos, val);
		}
		else nres = vatek_format;
	}

	if (is_vatek_success(nres))
	{
		if (val == pbuf->prog_len)
		{
			nres = _rdbuf(pos, &pbuf->prog_buf[0], pbuf->prog_len);
		}
	}
	if (is_vatek_success(nres))*addr = pos;
	return nres;
}

vatek_result muxplayload_set_epg(hvatek_chip hchip,Pmux_epg_param pepg,uint32_t* addr)
{
	uint32_t pos = *addr;
	vatek_result nres = _wrhal(pos, LIC_EPG_EN_START);
	if (is_vatek_success(nres))nres = _wrhal(pos, pepg->mode);
	if (is_vatek_success(nres))nres = _wrhal(pos, 0);
	if (is_vatek_success(nres))nres = _wrhal(pos, MUXTIME_DATA_TO_UINT(pepg->start_time));
	if (is_vatek_success(nres))nres = _wrhal(pos, MUXTIME_TIME_TO_UINT(pepg->start_time));
	if (is_vatek_success(nres))nres = _wrhal(pos, pepg->event_id);
	if (is_vatek_success(nres))nres = _wrhal(pos, 0);
	if (is_vatek_success(nres))nres = _wrhal(pos, 0);

	if (is_vatek_success(nres))
	{
		nres = _wrhal(pos, muxplayload_get_epg_section_nums(pepg));
		if (is_vatek_success(nres))nres = _wrhal(pos, 0);
		if (is_vatek_success(nres))
		{
			Pmux_epg_section psection = pepg->sections;
			while (psection)
			{
				nres = muxplayload_set_epg_section(hchip, psection, &pos);
				if (!is_vatek_success(nres))break;
				psection = psection->next;
			}
		}

		if (is_vatek_success(nres))nres = _wrhal(pos, LIC_EPG_EN_END);
	}

	if(is_vatek_success(nres))*addr = pos;
	return nres;
}

vatek_result muxplayload_get_epg(hmux_core hmux, hvatek_chip hchip, Pmux_epg_param* pepg, uint32_t* addr)
{
	uint32_t pos = *addr;
	uint32_t val = 0;
	vatek_result nres = _rdhal(pos, val);
	if (is_vatek_success(nres))
	{
		if (val == LIC_EPG_EN_START)
		{
			Pmux_epg_param newepg = (Pmux_epg_param)mux_handle_malloc_buffer(hmux,sizeof(mux_epg_param));
			nres = vatek_memfail;
			if (newepg)nres = _rdhal(pos, val);
			
			if (is_vatek_success(nres))
			{
				newepg->mode = (mux_epg_mode)val;
				nres = _rdhal(pos, val);
				if (is_vatek_success(nres))
				{
					nres = _rdhal(pos, val);
					if (is_vatek_success(nres))
					{
						muxtime_date_from_reg(&newepg->start_time, val);
						nres = _rdhal(pos, val);
						if (is_vatek_success(nres))muxtime_clock_from_reg(&newepg->start_time, val);
					}

					if (is_vatek_success(nres))
					{
						nres = _rdhal(pos, val);
						if (is_vatek_success(nres))
						{
							newepg->event_id = (uint16_t)val;
							nres = _rdhal(pos, val);
							if (is_vatek_success(nres))
							{
								//newepg->days = (uint8_t)val;
								nres = _rdhal(pos, val);
								//if (is_vatek_success(nres))newepg->loop_ms = val;
							}
						}
					}
				}
			}

			if (is_vatek_success(nres))
			{
				uint32_t sectionnums = 0;
				uint32_t descnums = 0;
				nres = _rdhal(pos, sectionnums);
				if (is_vatek_success(nres))
					nres = _rdhal(pos, descnums);
				if (is_vatek_success(nres))
				{
					Pmux_epg_section newsection = NULL;
					Pmux_epg_section lastsection = NULL;
					while (sectionnums)
					{
						nres = muxplayload_get_epg_section(hmux, hchip, &newsection, &pos);
						if (is_vatek_success(nres))
						{
							if (!newepg->sections)newepg->sections = newsection;
							else lastsection->next = newsection;
							lastsection = newsection;
						}else break;
						sectionnums--;
					}
				}

				if (is_vatek_success(nres))
				{
					nres = _rdhal(pos, val);
					if (is_vatek_success(nres))
					{
						if (val != LIC_EPG_EN_END)nres = vatek_format;
					}
				}

			}
			if (is_vatek_success(nres))*pepg = newepg;
		}
		else nres = vatek_unsupport;
	}

	if (is_vatek_success(nres))*addr = pos;
	return nres;
}

int32_t muxplayload_get_epg_section_nums(Pmux_epg_param pepg)
{
	Pmux_epg_section psection = pepg->sections;
	int32_t nums = 0;
	while (psection)
	{
		nums++;
		psection = psection->next;
	}
	return nums;

}

vatek_result muxplayload_set_epg_section(hvatek_chip hchip, Pmux_epg_section psection, uint32_t* addr)
{
	uint32_t pos = *addr;
	vatek_result nres = _wrhal(pos, psection->duration);
	if (is_vatek_success(nres))nres = _wrhal(pos, psection->type);
	if (is_vatek_success(nres))nres = _wrhal(pos, psection->parental_rating);
	if (is_vatek_success(nres))nres = _wrhal(pos, psection->title.maxlen);
	if (is_vatek_success(nres))nres = _wrhal(pos, psection->title.len);
	if (is_vatek_success(nres))nres = _wrbuf(pos, &psection->title.text[0],psection->title.maxlen);

	if (is_vatek_success(nres))
	{
		if (is_vatek_success(nres))nres = _wrhal(pos, psection->content.maxlen);
		if (is_vatek_success(nres))nres = _wrhal(pos, psection->content.len);
		if (is_vatek_success(nres))nres = _wrbuf(pos, &psection->content.text[0], psection->content.maxlen);
	}
	if (is_vatek_success(nres))*addr = pos;
	return nres;
}

vatek_result muxplayload_get_epg_section(hmux_core hmux, hvatek_chip hchip, Pmux_epg_section* psection, uint32_t* addr)
{
	uint32_t pos = *addr;
	Pmux_epg_section newepg = (Pmux_epg_section)mux_handle_malloc_buffer(hmux, sizeof(mux_epg_section));
	vatek_result nres = vatek_memfail;
	if (newepg)
	{
		uint32_t val = 0;
		nres = _rdhal(pos, val);
		if (is_vatek_success(nres))
		{
			newepg->duration = (uint8_t)val;
			nres = _rdhal(pos, val);
		}

		if (is_vatek_success(nres))
		{
			newepg->type = (mux_content_type)val;
			nres = _rdhal(pos, val);
		}

		if (is_vatek_success(nres))
		{
			newepg->parental_rating = (mux_parental_rating)val;
			nres = _rdhal(pos, newepg->title.maxlen);
		}

		if (is_vatek_success(nres))
		{
			newepg->title.text = mux_handle_malloc_buffer(hmux, (int32_t)newepg->title.maxlen);
			nres = vatek_memfail;
			if (newepg->title.text)
			{
				nres = _rdhal(pos, newepg->title.len);
				if (is_vatek_success(nres))
					nres = _rdbuf(pos, &newepg->title.text[0], newepg->title.maxlen);
				if (is_vatek_success(nres))nres = _rdhal(pos, newepg->content.maxlen);
			}

			if (is_vatek_success(nres))
			{
				newepg->content.text = mux_handle_malloc_buffer(hmux, (int32_t)newepg->content.maxlen);
				nres = vatek_memfail;
				if (newepg->content.text)
				{
					nres = _rdhal(pos, newepg->content.len);
					if (is_vatek_success(nres))
						nres = _rdbuf(pos, &newepg->content.text[0], newepg->content.maxlen);
				}
			}
		}
		if (is_vatek_success(nres))*psection = newepg;
	}
	if (is_vatek_success(nres))*addr = pos;
	return nres;
}
