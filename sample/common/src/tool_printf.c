
#include "../inc/tool_printf.h"
#include <stdio.h>
#include <cross/cross_os_api.h>
#include <core/ui/ui_props/ui_props_chip.h>
#include <core/ui/ui_props_api.h>
#include <vatek_sdk_bridge.h>
#include <bridge/ui/ui_bridge_base.h>
#include <bridge/ui/ui_bridge_source.h>
#include <service/service_broadcast.h>

void printf_chip_info(Pchip_info pchip)
{
	_disp_l("-------------------------------------");
	_disp_l("	   Chip Information");
	_disp_l("-------------------------------------");
	_disp_l("%-20s : %s", "Chip Status", ui_enum_get_str(chip_status,pchip->status));
	_disp_l("%-20s : %08x", "FW Version", pchip->version);
	_disp_l("%-20s : %08x", "Chip  ID", pchip->chip_module);
	_disp_l("%-20s : %08x", "Service", pchip->hal_service);
	_disp_l("%-20s : %08x", "Input", pchip->input_support);
	_disp_l("%-20s : %08x", "Output", pchip->output_support);
	_disp_l("%-20s : %08x", "Peripheral", pchip->peripheral_en);
	_disp_l("%-20s : %d", "SDK Version", vatek_version());
	_disp_l("=====================================\r\n");
}

void printf_bridge_info(Pbdevice_info pbinfo)
{
	_disp_l("-------------------------------------");
	_disp_l("	   Bridge device");
	_disp_l("-------------------------------------");
	_disp_l("%-20s : %08x", "bversion", pbinfo->bversion);
	_disp_l("%-20s : %08x", "source_enable", pbinfo->source_enable);
	_disp_l("%-20s : %08x", "rfmixer_enable", pbinfo->rfmixer_enable);
	_disp_l("%-20s : %08x", "demod_enable", pbinfo->demod_enable);
	_disp_l("%-20s : %08x", "source_nums", pbinfo->source_nums);
	_disp_l("=====================================\r\n");
}

void printf_app_end(void)
{
	while (cross_os_get_ch_no_wait() != -1);
	_disp_l("=====================================\r\n");
	_disp_l("demo finished. press any key to quit");
	while (cross_os_get_ch_no_wait() == -1);
}

void printf_bridge_avsource(const char* name,Pbridge_source psource)
{
	//_disp_l("-- source : [%08x:%s] - %s",psource->source_id, name,ui_enum_get_str(bridge_status,psource->status));
	_disp_l("-------------------------------------");
	_disp_l("	   %s param", name);
	_disp_l("-------------------------------------");
	if (psource->status == bstatus_active || psource->status == bstatus_active_protect)
	{
		Pbridge_video_info pvinfo = &psource->video_info;
		Pbridge_audio_info painfo = &psource->audio_info;
		//_disp_l("%-20s : %s", "resolution", ui_enum_get_str(video_resolution,pvinfo->resolution));
		//_disp_l("%-20s : %s", "framerate", ui_enum_get_str(video_framerate, pvinfo->framerate));
		//_disp_l("%-20s : %s", "aspectrate", ui_enum_get_str(video_aspect_rate, pvinfo->aspectrate));
		_disp_l("%-20s : %d", "offset_x", pvinfo->offset_x);
		_disp_l("%-20s : %d", "offset_y", pvinfo->offset_y);
		_disp_l("%-20s : %d", "pixelclk", pvinfo->pixelclk);
		_disp_l("%-20s : %08x", "signal_flag", pvinfo->signal_flag);

		//_disp_l("%-20s : %s", "channel", ui_enum_get_str(audio_channel, painfo->channel));
		//_disp_l("%-20s : %s", "samplerate", ui_enum_get_str(audio_samplerate, painfo->samplerate));
	}
	_disp_l("=====================================\r\n");
}

void printf_encoder_param(encoder_param enc)
{
	_disp_l("-------------------------------------");
	_disp_l("	   Encoder param");
	_disp_l("-------------------------------------");
	_disp_l("%-20s : %s", "Mode", ui_enum_get_str(encoder_mode, enc.mode));
	_disp_l("%-20s : %08x", "PMT PID", enc.pmt_pid);
	_disp_l("%-20s : %08x", "Video PID", enc.video_pid);
	_disp_l("%-20s : %08x", "Audio PID", enc.audio_pid);
	_disp_l("%-20s : %08x", "Flags", enc.encoder_flags);
	_disp_l("%-20s : %08x", "Tags", enc.encoder_tag);
	_disp_l("%-20s : %s", "[Video] Coder", ui_enum_get_str(video_codec, enc.video.vcodec));
	_disp_l("%-20s : %s", "[Video] Resolution", ui_enum_get_str(video_resolution, enc.video.resolution));
	_disp_l("%-20s : %s", "[Video] Framerate", ui_enum_get_str(video_framerate, enc.video.framerate));
	_disp_l("%-20s : %s", "[Video] Aspectrate", ui_enum_get_str(video_aspect_rate, enc.video.aspectrate));
	_disp_l("%-20s : %s", "[Audio] Coder", ui_enum_get_str(audio_codec, enc.audio.acodec));
	_disp_l("%-20s : %s", "[Audio] Samplerate", ui_enum_get_str(audio_samplerate, enc.audio.samplerate));
	_disp_l("%-20s : %s", "[Audio] Channel", ui_enum_get_str(audio_channel, enc.audio.channel));
	_disp_l("%-20s : %s", "[Quality] RC mode", ui_enum_get_str(ratecontrol_mode, enc.quality.rcmode));
	_disp_l("%-20s : %d", "[Quality] Min Q", enc.quality.minq);
	_disp_l("%-20s : %d", "[Quality] Max Q", enc.quality.maxq);
	_disp_l("%-20s : %d", "[Quality] GOP", enc.quality.goplen);
	_disp_l("%-20s : %d", "[Quality] Latency", enc.quality.latency);
	_disp_l("%-20s : %d", "[Quality] Bitrate", enc.quality.bitrate);
	_disp_l("=====================================\r\n");
}

void printf_modulation_param(modulator_param modulator, r2_param r2param)
{
	_disp_l("-------------------------------------");
	_disp_l("	   Modulation param");
	_disp_l("-------------------------------------");
	_disp_l("%-20s : %d", "Frequency(khz)", r2param.freqkhz);
	_disp_l("%-20s : %s", "Type", ui_enum_get_str(modulator_type, modulator.type));
	_disp_l("%-20s : %d", "Bandwidth / Symbolrate", modulator.bandwidth_symbolrate);
	_disp_l("%-20s : %s", "IF mode", ui_enum_get_str(dac_ifmode, modulator.ifmode));
	_disp_l("%-20s : %d", "IF mode offset", modulator.iffreq_offset);
	if (modulator.type == modulator_atsc) {
		_disp_l("%-20s : %s", "Constellation", ui_enum_get_str(atsc_qam, modulator.mod.atsc.constellation));
	}
	else if (modulator.type == modulator_dvb_t) {
		_disp_l("%-20s : %s", "Constellation", ui_enum_get_str(dvb_t_qam, modulator.mod.dvb_t.constellation));
		_disp_l("%-20s : %s", "FFT", ui_enum_get_str(fft_mode, modulator.mod.dvb_t.fft));
		_disp_l("%-20s : %s", "Guard Interval", ui_enum_get_str(guard_interval, modulator.mod.dvb_t.guardinterval));
		_disp_l("%-20s : %s", "Code Rate", ui_enum_get_str(code_rate, modulator.mod.dvb_t.coderate));
	}
	else if (modulator.type == modulator_isdb_t) {
		_disp_l("%-20s : %s", "Constellation", ui_enum_get_str(isdb_t_qam, modulator.mod.isdb_t.constellation));
		_disp_l("%-20s : %s", "FFT", ui_enum_get_str(fft_mode, modulator.mod.isdb_t.fft));
		_disp_l("%-20s : %s", "Guard Interval", ui_enum_get_str(guard_interval, modulator.mod.isdb_t.guardinterval));
		_disp_l("%-20s : %s", "Code Tate", ui_enum_get_str(code_rate, modulator.mod.isdb_t.coderate));
		_disp_l("%-20s : %s", "Time Interleaved", ui_enum_get_str(isdb_t_time_interleaved, modulator.mod.isdb_t.timeinterleaved));
		_disp_l("%-20s : %08x", "ISDB-T Flags", modulator.mod.isdb_t.isdb_t_flags);
	}
	else if (modulator.type == modulator_dtmb) {
		_disp_l("%-20s : %s", "Constellation", ui_enum_get_str(dtmb_qam, modulator.mod.dtmb.constellation));
		_disp_l("%-20s : %s", "Time Interleaved", ui_enum_get_str(dtmb_time_interleaved, modulator.mod.dtmb.timeinterleaved));
		_disp_l("%-20s : %s", "Code Rate", ui_enum_get_str(dtmb_code_rate, modulator.mod.dtmb.coderate));
		_disp_l("%-20s : %s", "Carrier Mode", ui_enum_get_str(dtmb_carrier_mode, modulator.mod.dtmb.carriermode));
		_disp_l("%-20s : %s", "Frame Header", ui_enum_get_str(dtmb_frame_header, modulator.mod.dtmb.framesync));
	}
	else if (modulator.type == modulator_j83a) {
		_disp_l("%-20s : %s", "Constellation", ui_enum_get_str(j83a_qam, modulator.mod.j83a.constellation));
	}
	else if (modulator.type == modulator_j83b) {
		_disp_l("%-20s : %s", "Constellation", ui_enum_get_str(j83b_qam, modulator.mod.j83b.constellation));
	}
	else if (modulator.type == modulator_j83c) {
		_disp_l("%-20s : %s", "Constellation", ui_enum_get_str(j83c_qam, modulator.mod.j83c.constellation));
	}
	else if (modulator.type == modulator_dvb_t2) {
		_disp_l("%-20s : %s", "Version", ui_enum_get_str(t2_version, modulator.mod.dvb_t2.version));
		_disp_l("%-20s : %08x", "T2 Flags", modulator.mod.dvb_t2.t2_flags);
		_disp_l("%-20s : %s", "L1 Constellation", ui_enum_get_str(constellation_t2_l1, modulator.mod.dvb_t2.l1_constellation));
		_disp_l("%-20s : %s", "PLP Constellation", ui_enum_get_str(constellation_t2_plp, modulator.mod.dvb_t2.plp_constellation));
		_disp_l("%-20s : %s", "ISSY", ui_enum_get_str(t2_issy, modulator.mod.dvb_t2.issy));
		_disp_l("%-20s : %s", "FFT", ui_enum_get_str(t2_fft_mode, modulator.mod.dvb_t2.fft));
		_disp_l("%-20s : %s", "Code Rate", ui_enum_get_str(t2_code_rate, modulator.mod.dvb_t2.coderate));
		_disp_l("%-20s : %s", "Guard Interval", ui_enum_get_str(t2_guard_interval, modulator.mod.dvb_t2.guardinterval));
		_disp_l("%-20s : %s", "Pilot Pattern", ui_enum_get_str(t2_pilot_pattern, modulator.mod.dvb_t2.pilotpattern));
		_disp_l("%-20s : %s", "FEC-Block length", ui_enum_get_str(t2_fec_length, modulator.mod.dvb_t2.fectype));
		_disp_l("%-20s : %08x", "Network ID", modulator.mod.dvb_t2.network_id);
		_disp_l("%-20s : %08x", "System ID", modulator.mod.dvb_t2.system_id);
		_disp_l("%-20s : %08x", "FEC block numbers (0 mean auto)", modulator.mod.dvb_t2.fecblock_nums);
		_disp_l("%-20s : %08x", "Symbol numbers (0 mean auto)   ", modulator.mod.dvb_t2.symbol_nums);
		_disp_l("%-20s : %08x", "ti_ni", modulator.mod.dvb_t2.ti_ni);
		_disp_l("%-20s : %08x", "recv", modulator.mod.dvb_t2.recv);
	}
	_disp_l("=====================================\r\n");
}

void printf_stream_info(broadcast_info info)
{
	//_disp_l("usbstream - A3 Status:%s", ui_enum_get_str(broadcast_status, pinfo->info.status));
	
}