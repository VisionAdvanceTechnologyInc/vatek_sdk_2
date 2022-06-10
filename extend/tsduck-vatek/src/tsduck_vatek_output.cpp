#include "../inc/tsduck_vatek_output.h"
#include <cross/cross_os_api.h>

using namespace ts;


extern vatek_result modparam_config_dvb_t(TSP* tsp, Args* args, Pmodulator_param pmod);
extern vatek_result modparam_config_j83a(TSP* tsp, Args* args, Pmodulator_param pmod);
extern vatek_result modparam_config_atsc(TSP* tsp, Args* args, Pmodulator_param pmod);
extern vatek_result modparam_config_j83b(TSP* tsp, Args* args, Pmodulator_param pmod);
extern vatek_result modparam_config_dtmb(TSP* tsp, Args* args, Pmodulator_param pmod);
extern vatek_result modparam_config_isdb_t(TSP* tsp, Args* args, Pmodulator_param pmod);
extern vatek_result modparam_config_j83c(TSP* tsp, Args* args, Pmodulator_param pmod);
extern vatek_result modparam_config_dvb_t2(TSP* tsp, Args* args, Pmodulator_param pmod);

const tsmod_param_config tsduck_vatek_output::m_modtables[] =
{
	{modulator_dvb_t,modparam_config_dvb_t,},
	{modulator_j83a,modparam_config_j83a,},
	{modulator_atsc,modparam_config_atsc,},
	{modulator_j83b,modparam_config_j83b,},
	{modulator_dtmb,modparam_config_dtmb,},
	{modulator_isdb_t,modparam_config_isdb_t,},
	{modulator_j83c,modparam_config_j83c,},
	{modulator_dvb_t2,modparam_config_dvb_t2,},
};

tsduck_vatek_output::tsduck_vatek_output(TSP* tsp) :
	OutputPlugin(tsp,u"Send packets to a VATEK modulator device"),
	m_hdevices(NULL),
	m_hchip(NULL),
	m_husbstream(NULL),
	m_index(-1),
	m_slicebuf(NULL)
{
	option(u"device", 'd', UNSIGNED);
	help(u"device",
		u"Device index, from 0 to N-1 (with N being the number of Vatek devices "
		u"in the system). Use the command \"tsvatel -a [-v]\" to have a "
		u"complete list of devices in the system. By default, use the first "
		u"output Vatek device.");

	option(u"frequency", 'f', UNSIGNED);
	help(u"frequency",
		u"All modulator devices: indicate the frequency, in KHz, of the output "
		u"carrier. the valid range is 50000000 Hz to 1000000000 Hz."
		u"The default is 473000000 Hz.");


	option(u"bandwidth", 0, Enumeration({
		{u"1.7", tsvatek_bw_1_7},
		{u"5",   tsvatek_bw_5},
		{u"6",   tsvatek_bw_6},
		{u"7",   tsvatek_bw_7},
		{u"8",   tsvatek_bw_8},
		{u"10",  tsvatek_bw_10},
		}));
	help(u"bandwidth",
		u"DVB-T, DVB-T2 and DMB-T(DTMB) modulators: indicate bandwidth "
		u"in MHz. The default is 8 MHz. "
		u"The bandwidth values 1.7 and 10 MHz are valid for DVB-T2 only.");

	option(u"constellation", 0, Enumeration({
		{u"QPSK",   dvb_t_qpsk},
		{u"16-QAM", dvb_t_qam16},
		{u"64-QAM", dvb_t_qam64},
		}));
	help(u"constellation",
		u"DVB-T,ISDB-T modulators: indicate the constellation type. The default is 64-QAM.");

	option(u"j83-qam", 0, Enumeration({
		{u"16-QAM",		j83a_qam16},
		{u"32-QAM",		j83a_qam32},
		{u"64-QAM",		j83a_qam64},
		{u"128-QAM",	j83a_qam128},
		{u"256-QAM",	j83a_qam256},
		}));

	help(u"constellation",
		u"The specified value and default depends on the modulation type.\n"
		u"J83A : 16-QAM, 32-QAM, 64-QAM, 128-QAM, 256-QAM. default 64-QAM.\n"
		u"J83B : 64-QAM, 256-QAM. default 256-QAM.\n"
		u"J83C : 64-QAM, 256-QAM. default 256-QAM.\n");

	option(u"convolutional-rate", 'r', Enumeration({
		{u"1/2",  coderate_1_2},
		{u"2/3",  coderate_2_3},
		{u"3/4",  coderate_3_4},
		{u"5/6",  coderate_5_6},
		{u"7/8",  coderate_7_8},
		}));
	help(u"convolutional-rate",
		u"For modulators devices only: specify the convolutional rate. "
		u"The specified value depends on the modulation type.\n"
		u"DVB-T,ISDB-T: 1/2, 2/3, 3/4, 5/6, 7/8.\n"
		u"The default is 5/6.");


	option(u"dmb-constellation", 0, Enumeration({
		{u"4-QAM-NR", dtmb_qam4_nr},
		{u"4-QAM",    dtmb_qpsk},
		{u"16-QAM",   dtmb_qam16},
		{u"32-QAM",   dtmb_qam32},
		{u"64-QAM",   dtmb_qam64},
		}));
	help(u"dmb-constellation",
		u"DMB-T(DTMB) modulators: indicate the constellation type. The default is 64-QAM.");

	option(u"dmb-fec", 0, Enumeration({
		{u"0.4", dtmb_code_rate_0_4},
		{u"0.6", dtmb_code_rate_0_6},
		{u"0.8", dtmb_code_rate_0_8},
		}));
	help(u"dmb-fec",
		u"DMB-T(DTMB) modulators: indicate the FEC code rate. The default is 0.8. "
		u"4-QAM-NR and 32-QAM can be used only with --dmb-fec 0.8.");

	option(u"dmb-carrier", 0, Enumeration({
		{u"1"		,dtmb_carrier_1},
		{u"3780"	,dtmb_carrier_3780},
		}));

	help(u"dmb-carrier",
		 u"DMB-T(DTMB) modulators: indicate the carrier mode. The default is 3780. ");

	option(u"dmb-frame-numbering");
	help(u"dmb-frame-numbering",
		u"DMB-T/H, ADTB-T modulators: indicate to use frame numbering. The default "
		u"is to use no frame numbering.");

	option(u"dmb-header", 0, Enumeration({
		{u"PN420", dtmb_framesync_420},
		{u"PN595", dtmb_framesync_595},
		{u"PN945", dtmb_framesync_945},
		}));
	help(u"dmb-header",
		u"DMB-T/H, ADTB-T modulators: indicate the FEC frame header mode. "
		u"The default is PN945.");

	option(u"dmb-interleaver", 0, Enumeration({
		{u"1", dtmb_interleaved_240},
		{u"2", dtmb_interleaved_720},
		}));
	help(u"dmb-interleaver",
		u"DMB-T(DTMB) modulators: indicate the interleaver mode. Must be one "
		u"1 (B=54, M=240) or 2 (B=54, M=720). The default is 1.");

	option(u"guard-interval", 'g', Enumeration({
			{u"1/32", guard_interval_1_32},
			{u"1/16", guard_interval_1_16},
			{u"1/8",  guard_interval_1_8},
			{u"1/4",  guard_interval_1_4},
		}));
	help(u"guard-interval",
		u"DVB-T and ISDB-T modulators: indicate the guard interval. The default is 1/16.");


	option(u"fft-mode", 0, Enumeration({
		{u"1K",  t2_fft_1k},
		{u"2K",  t2_fft_2k},
		{u"4K",  t2_fft_4k},
		{u"8K",  t2_fft_8k},
		{u"16K", t2_fft_16k},
		}));
	help(u"fft-mode",
		u"DVB-T2 modulators: indicate the FFT mode. The default is 32K.");

	option(u"pilots", 0);
	help(u"pilots", u"DVB-S2 and ADTB-T modulators: enable pilots (default: no pilot).");

	option(u"pilot-pattern", 'p', Enumeration({
		{u"1", pilot_pattern_1},
		{u"2", pilot_pattern_2},
		{u"3", pilot_pattern_3},
		{u"4", pilot_pattern_4},
		{u"5", pilot_pattern_5},
		{u"6", pilot_pattern_6},
		{u"7", pilot_pattern_7},
		{u"8", pilot_pattern_8},
		}));
	help(u"pilot-pattern",
		u"DVB-T2 modulators: indicate the pilot pattern to use, a value in the "
		u"range 1 to 8. The default is 7.");

	option(u"plp0-code-rate", 0, Enumeration({
		{u"1/2", t2_coderate_1_2},
		{u"3/5", t2_coderate_3_5},
		{u"2/3", t2_coderate_2_3},
		{u"3/4", t2_coderate_3_4},
		{u"4/5", t2_coderate_4_5},
		{u"5/6", t2_coderate_5_6},
		}));
	help(u"plp0-code-rate",
		u"DVB-T2 modulators: indicate the convolutional coding rate used by the "
		u"PLP #0. The default is 2/3.");

	option(u"plp0-fec-type", 0, Enumeration({
		{u"16K", t2_fec_16200},
		{u"64K", t2_fec_64800},
		}));
	help(u"plp0-fec-type",
		u"DVB-T2 modulators: indicate the FEC type used by the PLP #0. The default is 64K LPDC.");

	option(u"plp0-issy", 0, Enumeration({
	{u"NONE",  t2_issy_disable},
	{u"SHORT", t2_issy_short},
	{u"LONG",  t2_issy_long},
		}));
	help(u"plp0-issy",
		u"DVB-T2 modulators: type of ISSY field to compute and insert in PLP #0. "
		u"The default is NONE.");

	option(u"t2-version", 0, Enumeration({
		{u"ver131",  t2_ver_131},
		{u"ver131_lite",  t2_ver_131_lite,},
		}));
	help(u"t2-version",
		u"DVB-T2 modulators: version tag. "
		u"The default is ver131.");

	option(u"plp0-high-efficiency");
	help(u"plp0-high-efficiency",
		u"DVB-T2 modulators: indicate that the PLP #0 uses High Efficiency Mode "
		u"(HEM). Otherwise Normal Mode (NM) is used.");


	option(u"plp0-modulation", 0, Enumeration({
		{u"QPSK",    t2_plp_qpsk},
		{u"16-QAM",  t2_plp_qam16},
		{u"64-QAM",  t2_plp_qam64},
		{u"256-QAM", t2_plp_qam256},
		}));
	help(u"plp0-modulation",
		u"DVB-T2 modulators: indicate the modulation used by PLP #0. The default is 256-QAM.");

	option(u"plp0-null-packet-deletion");
	help(u"plp0-null-packet-deletion",
		u"DVB-T2 modulators: indicate that null-packet deletion is active in "
		u"PLP #0. Otherwise it is not active.");

	option(u"plp0-rotation");
	help(u"plp0-rotation",
		u"DVB-T2 modulators: indicate that constellation rotation is used for "
		u"PLP #0. Otherwise not.");

	option(u"symbol-rate", 0, POSITIVE);
	help(u"symbol-rate",
		u"J83a modulators: Specify the symbol rate in kilo-symbols/second. "
		u"The default is : 5120 Ksps");


	option(u"t2-guard-interval", 0, Enumeration({
		{u"1/128", t2_gi_1_128},
		{u"1/32", t2_gi_1_32},
		{u"1/16", t2_gi_1_16},
		{u"19/256", t2_gi_19_256},
		{u"1/8", t2_gi_1_8},
		{u"19/128", t2_gi_19_128},
		{u"1/4", t2_gi_1_4},
		}));
	help(u"t2-guard-interval",
		u"DVB-T2 modulators: indicates the guard interval. The default is 1/128.");

	option(u"t2-l1-modulation", 0, Enumeration({
		{u"BPSK",   t2_l1_bpsk},
		{u"QPSK",   t2_l1_qpsk},
		{u"16-QAM", t2_l1_qam16},
		{u"64-QAM", t2_l1_qam64},
		}));
	help(u"t2-l1-modulation",
		u"DVB-T2 modulators: indicate the modulation type used for the L1-post "
		u"signalling block. The default is 16-QAM.");

	option(u"t2-network-id", 0, UINT32);
	help(u"t2-network-id",
		u"DVB-T2 modulators: indicate the DVB-T2 network identification. "
		u"The default is 0.");

	option(u"t2-system-id", 0, UINT32);
	help(u"t2-system-id",
		u"DVB-T2 modulators: indicate the DVB-T2 system identification. "
		u"The default is 0.");

	option(u"bandwidth-extension");
	help(u"bandwidth-extension",
		u"DVB-T2 modulators: indicate that the extended carrier mode is used. "
		u"By default, use normal carrier mode.");

	option(u"transmission-mode", 't', Enumeration({
	{u"2K", fft_2k},
	{u"4K", fft_4k},
	{u"8K", fft_8k},
		}));
	help(u"transmission-mode",
		u"DVB-T and ISDB-T modulators: indicate the transmission mode. The default is 8K.");

	option(u"modulation", 'm', Enumeration({
	{u"DVB-T",         modulator_dvb_t},
	{u"DVB-T2",        modulator_dvb_t2},
	{u"J83A",          modulator_j83a},
	{u"ATSC-VSB",      modulator_atsc},
	{u"J83B",		   modulator_j83b},
	{u"ISDB-T",        modulator_isdb_t},
	{u"J83C",          modulator_j83c},
	{u"DMB-T",         modulator_dtmb},
	{u"DTMB",          modulator_dtmb},
		}));

	help(u"modulation",
		u"For modulators, indicate the modulation type. "
		u"The supported modulation types depend on the device model. "
		u"The default modulation type is: DVB-T\n");

	option(u"remux", 're', Enumeration({
	{u"remux",         ustream_remux_pcr},
	{u"passthrough",        ustream_remux_passthrough},
		}));


	memset(&m_param, 0, sizeof(usbstream_param));
	m_param.freq_khz = 473000;
	m_param.mode = ustream_mode_async;
	m_param.remux = ustream_remux_pcr;
	m_param.pcradjust = pcr_adjust;

	m_param.async.bitrate = 0;
	m_param.async.mode = uasync_mode_cbr;
	m_param.async.prepare_ms = 0;
}

tsduck_vatek_output::~tsduck_vatek_output()
{
	stop();
}

bool tsduck_vatek_output::start()
{
	vatek_result nres = vatek_badstatus;
	if (m_hdevices)tsp->error(u"bad status already broadcasting.");
	else
	{
		nres = configParam();
		if (is_vatek_success(nres))
		{
			nres = vatek_device_list_enum(DEVICE_BUS_USB, service_transform, &m_hdevices);
			if (is_vatek_success(nres))
			{
				if (nres == vatek_success)nres = vatek_nodevice;
				else if (nres > vatek_success)
				{
					nres = vatek_device_open(m_hdevices, m_index, &m_hchip);
					if (is_vatek_success(nres)) 
						nres = vatek_usbstream_open(m_hchip, &m_husbstream);

					if (!is_vatek_success(nres))
						tsp->error(u"open modulation device fail : [%d:%d]", { m_index,nres });
					else
					{
						nres = vatek_usbstream_start(m_husbstream, &m_param);

						if (is_vatek_success(nres))
						{
							tsp->info(u"modulation start - [%s:%s:%d]",
								{
								   vatek_device_get_name(m_hchip),
								   ui_enum_get_str(modulator_type,m_param.modulator.type),
								});
						}
						else tsp->error(u"start modulation device broadcasting fail : [%d]", { nres });
					}
				}
				if (!is_vatek_success(nres))stop();
			}
			else tsp->error(u"enumeration modulation device fail : [%d]", { nres });
		}
	}

	if (is_vatek_success(nres))return true;
	return false;
}

bool tsduck_vatek_output::stop()
{
	if (m_husbstream)
	{
		usbstream_status status = vatek_usbstream_get_status(m_husbstream, NULL);
		if (status == usbstream_status_running || status == usbstream_status_moredata)
			vatek_usbstream_stop(m_husbstream);
	}
	if (m_husbstream)vatek_usbstream_close(m_husbstream);
	if (m_hchip)vatek_device_close(m_hchip);
	if (m_hdevices)vatek_device_list_free(m_hdevices);
	m_hdevices = NULL;
	m_hchip = NULL;
	m_husbstream = NULL;
	modulator_param_reset(modulator_dvb_t, &m_param.modulator);
	m_index = -1;
	m_param.freq_khz = 473000;
	return true;
}

bool tsduck_vatek_output::send(const TSPacket* pkts, const TSPacketMetadata* meta, size_t packet_count)
{
	vatek_result nres = vatek_badstatus;
	usbstream_status status = vatek_usbstream_get_status(m_husbstream,NULL);

	if (status == usbstream_status_running ||
		status == usbstream_status_moredata)
	{
		char* data = reinterpret_cast<char*>(const_cast<TSPacket*>(pkts));

		int remain = packet_count;
		while (remain)
		{	
			if (!m_slicebuf)
				nres = vatek_ustream_async_get_buffer(m_husbstream, &m_slicebuf);
			else nres = (vatek_result)1;

			if (nres > vatek_success)
			{
				int32_t pktnums = m_slicebuf->packet_len - m_slicebuf->packet_pos;
				int32_t pktsize;
				if (pktnums > remain)pktnums = remain;

				pktsize = pktnums * TS_PACKET_LEN;
				memcpy(m_slicebuf->ptrbuf, data, pktsize);
				m_slicebuf->ptrbuf += pktsize;
				data += pktsize;
				m_slicebuf->packet_pos += pktnums;
				if (m_slicebuf->packet_pos == m_slicebuf->packet_len)
				{
					nres = vatek_ustream_async_commit_buffer(m_husbstream, m_slicebuf);
					m_slicebuf = NULL;
				}
				remain -= pktnums;
			}
			else if (nres == vatek_success)cross_os_sleep(1);
			if (!is_vatek_success(nres)){
				break;
			}
		}
	}

	if (is_vatek_success(nres))return true;
	tsp->error(u"send packets to modulation fail : [%d]", { nres });
	return false;
}

bool tsduck_vatek_output::isRealTime()
{
	return true;
}

BitRate tsduck_vatek_output::getBitrate()
{
	if (m_husbstream) {
		uint32_t bitrate = modulator_param_get_bitrate(&m_param.modulator);
		return (BitRate)modulator_param_get_bitrate(&m_param.modulator);
		tsp->info(u"BitRate : [%d]", { bitrate });
	}
	tsp->info(u"BitRate : [%d]", { 0 });
	return (BitRate)0;
}

vatek_result tsduck_vatek_output::configParam()
{
	#define MOD_NUMS	(sizeof(m_modtables)/sizeof(tsmod_param_config))
	vatek_result nres = vatek_badparam;
	modulator_type type = (modulator_type)intValue(u"modulation", modulator_dvb_t);
	int32_t i = 0;

	for (i = 0; i < MOD_NUMS; i++)
	{
		if (m_modtables[i].type == type)
		{
			nres = m_modtables[i].config(tsp, this, &m_param.modulator);
			break;
		}
	}

	if (is_vatek_success(nres))
	{
		m_param.freq_khz = intValue(u"frequency", m_param.freq_khz) / 1000;
		m_index = intValue(u"device", 0);
	}

	if (is_vatek_success(nres))
	{
		m_param.remux = intValue(u"remux", m_param.remux);
		m_index = intValue(u"device", 0);
	}
	return nres;
}

/* modulation param config */

#define _intVal(n,d)		args->intValue<int32_t>(n,d)
#define _errstr				tsp->error

vatek_result modparam_config_dvb_t(TSP* tsp, Args* args, Pmodulator_param pmod)
{
	vatek_result nres = modulator_param_reset(modulator_dvb_t, pmod);
	if (is_vatek_success(nres))
	{
		uint32_t val = _intVal(u"bandwidth", 8);
		nres = vatek_badparam;

		if (val == 0 || val == 10)_errstr(u"dvb-t not support bandwidth : %d", { val } );
		else
		{
			Pdvb_t_param pdvb = &pmod->mod.dvb_t;
			pmod->bandwidth_symbolrate = val;
			pdvb->constellation = (constellation_mode)_intVal(u"constellation", pdvb->constellation);
			pdvb->fft = (fft_mode)_intVal(u"transmission-mode", pdvb->fft);
			pdvb->coderate = (code_rate)_intVal(u"convolutional-rate", pdvb->coderate);
			pdvb->guardinterval = (guard_interval)_intVal(u"guard-interval", pdvb->guardinterval);
			nres = modulator_param_get_bitrate(pmod) == 0 ? vatek_badparam : vatek_success;
			if (!is_vatek_success(nres))_errstr(u"dvb-t param config fail : [%d]", { nres, });
		}
	}
	return nres;
}

vatek_result modparam_config_j83a(TSP* tsp, Args* args, Pmodulator_param pmod)
{
	vatek_result nres = modulator_param_reset(modulator_j83a, pmod);
	if (is_vatek_success(nres))
	{
		uint32_t val = _intVal(u"symbol-rate", 5120);
		nres = vatek_badparam;
		if (val < 5000 || val > 7000)_errstr(u"j83a symbol-rate must between 5000 and 7000 Ksps : [%d]", { val });
		else
		{
			Pj83a_param pj83a = &pmod->mod.j83a;
			pmod->bandwidth_symbolrate = val;
			pj83a->constellation = (constellation_mode)_intVal(u"j83-qam", j83a_qam128);
			nres = modulator_param_get_bitrate(pmod) == 0 ? vatek_badparam : vatek_success;
			if (!is_vatek_success(nres))_errstr(u"j83a param config fail : [%d]", { nres, });
		}
	}
	return nres;
}

vatek_result modparam_config_j83c(TSP* tsp, Args* args, Pmodulator_param pmod)
{
	vatek_result nres = modulator_param_reset(modulator_j83c, pmod);
	if (is_vatek_success(nres))
	{
		Pj83c_param pj83c = &pmod->mod.j83c;
		pj83c->constellation = (constellation_mode)_intVal(u"j83-qam", j83a_qam256);
		if (pj83c->constellation == j83c_qam64)pj83c->constellation = j83c_qam64;
		else if (pj83c->constellation == j83c_qam256)pj83c->constellation = j83c_qam256;
		else
		{
			_errstr(u"j83c only supported 64-QAM and 256-QAM");
			nres = vatek_badparam;
		}
	}
	return nres;
}

vatek_result modparam_config_atsc(TSP* tsp, Args* args, Pmodulator_param pmod)
{
	vatek_result nres = modulator_param_reset(modulator_atsc, pmod);
	return nres;
}

vatek_result modparam_config_j83b(TSP* tsp, Args* args, Pmodulator_param pmod)
{
	vatek_result nres = modulator_param_reset(modulator_j83b, pmod);
	if (is_vatek_success(nres))
	{
		Pj83b_param pj83b = &pmod->mod.j83b;
		pj83b->constellation = (constellation_mode)_intVal(u"j83-qam", j83a_qam256);
		if (pj83b->constellation == j83a_qam64)pj83b->constellation = j83b_qam64;
		else if (pj83b->constellation == j83a_qam256)pj83b->constellation = j83b_qam256;
		else
		{
			_errstr(u"j83b only supported 64-QAM and 256-QAM");
			nres = vatek_badparam;
		}
	}
	return nres;
}

vatek_result modparam_config_dtmb(TSP* tsp, Args* args, Pmodulator_param pmod)
{
	vatek_result nres = modulator_param_reset(modulator_dtmb, pmod);
	if (is_vatek_success(nres))
	{
		uint32_t val = _intVal(u"bandwidth", 8);
		nres = vatek_badparam;

		if (val == 0 || val == 10)_errstr(u"dtmb not support bandwidth : %d", { val });
		else
		{
			Pdtmb_param pdtmb = &pmod->mod.dtmb;
			pmod->bandwidth_symbolrate = val;
			pdtmb->constellation = (constellation_mode)_intVal(u"dmb-constellation", pdtmb->constellation);
			pdtmb->coderate = (dtmb_code_rate)_intVal(u"dmb-fec", pdtmb->coderate);
			pdtmb->framesync = (dtmb_frame_header)_intVal(u"dmb-header", pdtmb->framesync);
			pdtmb->timeinterleaved = (dtmb_time_interleaved)_intVal(u"dmb-interleaver", pdtmb->timeinterleaved);
			pdtmb->carriermode = (dtmb_carrier_mode)_intVal(u"dmb-carrier", pdtmb->carriermode);

			if ((pdtmb->constellation == dtmb_qam4_nr || pdtmb->constellation == dtmb_qam32) &&
				pdtmb->coderate != dtmb_code_rate_0_8)
				_errstr(u"dtmb qam4-nr and qam32 only support dmb-fec = 0.8.");
			else
			{
				nres = modulator_param_get_bitrate(pmod) == 0 ? vatek_badparam : vatek_success;
				if (!is_vatek_success(nres))_errstr(u"dtmb param config fail : [%d]", { nres, });
			}
		}
	}

	return nres;
}

vatek_result modparam_config_isdb_t(TSP* tsp, Args* args, Pmodulator_param pmod)
{
	vatek_result nres = modulator_param_reset(modulator_isdb_t, pmod);
	if (is_vatek_success(nres))
	{
		uint32_t val = _intVal(u"bandwidth", 6);
		nres = vatek_badparam;

		if (val == 0 || val == 10)_errstr(u"isdb-t not support bandwidth : %d", { val });
		else
		{
			Pisdb_t_param pisdbt = &pmod->mod.isdb_t;
			pmod->bandwidth_symbolrate = val;
			pisdbt->constellation = (constellation_mode)_intVal(u"constellation", dvb_t_qam64);
			if (pisdbt->constellation == dvb_t_qpsk)pisdbt->constellation = isdb_t_qpsk;
			else if (pisdbt->constellation == dvb_t_qam16)pisdbt->constellation = isdb_t_qam16;
			else pisdbt->constellation = isdb_t_qam64;

			pisdbt->fft = (fft_mode)_intVal(u"transmission-mode", pisdbt->fft);
			pisdbt->coderate = (code_rate)_intVal(u"convolutional-rate", pisdbt->coderate);
			pisdbt->guardinterval = (guard_interval)_intVal(u"guard-interval", pisdbt->guardinterval);

			if (pisdbt->fft == fft_8k)pisdbt->timeinterleaved = isdb_t_interleaved_mode3;
			else if (pisdbt->fft == fft_4k)pisdbt->timeinterleaved = isdb_t_interleaved_mode2;
			else pisdbt->timeinterleaved = isdb_t_interleaved_mode1;
			nres = modulator_param_get_bitrate(pmod) == 0 ? vatek_badparam : vatek_success;
			if (!is_vatek_success(nres))_errstr(u"isdb-t param config fail : [%d]", { nres, });
		}
	}
	return nres;
}

vatek_result modparam_config_dvb_t2(TSP* tsp, Args* args, Pmodulator_param pmod)
{
	vatek_result nres = modulator_param_reset(modulator_dvb_t2, pmod);
	if (is_vatek_success(nres))
	{
		uint32_t val = _intVal(u"bandwidth", 8);
		Pdvb_t2_param pt2 = &pmod->mod.dvb_t2;

		pmod->bandwidth_symbolrate = val;
		pt2->version = (t2_version)_intVal(u"t2-version",pt2->version);
		pt2->l1_constellation = (constellation_t2_l1)_intVal(u"t2-l1-modulation", pt2->l1_constellation);
		pt2->coderate = (t2_code_rate)_intVal(u"plp0-code-rate", pt2->coderate);
		pt2->fectype = (t2_fec_length)_intVal(u"plp0-fec-type", pt2->fectype);
		pt2->plp_constellation = (constellation_t2_plp)_intVal(u"plp0-modulation", pt2->plp_constellation);
		pt2->fft = (t2_fft_mode)_intVal(u"fft-mode", pt2->fft);
		pt2->guardinterval = (t2_guard_interval)_intVal(u"t2-guard-interval", pt2->guardinterval);
		pt2->pilotpattern = (t2_pilot_pattern)_intVal(u"pilot-pattern", pt2->pilotpattern);
		pt2->network_id = (uint16_t)_intVal(u"t2-network-id", pt2->network_id);
		pt2->system_id = (uint16_t)_intVal(u"t2-system-id", pt2->system_id);
		pt2->issy = (t2_issy)_intVal(u"plp0-issy", pt2->issy);
		pt2->t2_flags = 0;

		if (_intVal(u"bandwidth-extension", 0))
			pt2->t2_flags |= T2EN_EXTEND_CARRIER_MODE;
		if (_intVal(u"plp0-rotation", 0))
			pt2->t2_flags |= T2EN_CONSTELLATION_ROTATION;
		if (_intVal(u"plp0-null-packet-deletion", 0))
			pt2->t2_flags |= T2EN_DELETE_NULL_PACKET;
		if (_intVal(u"plp0-high-efficiency",0))
			pt2->t2_flags |= T2EN_INPUT_TS_HEM;

		pt2->fecblock_nums = 0;
		pt2->symbol_nums = 0;

		nres = modulator_param_get_bitrate(pmod) == 0 ? vatek_badparam : vatek_success;
		if(!is_vatek_success(nres))_errstr(u"dvb-t2 param config fail : [%d]", { nres, });
	}
	

	return nres;
}
