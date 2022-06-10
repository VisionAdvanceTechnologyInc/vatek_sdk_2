
#include "../inc/tool_printf.h"
#include <stdio.h>
#include <cross/cross_os_api.h>
#include <core/ui/ui_props/ui_props_chip.h>
#include <core/ui/ui_props_api.h>
#include <vatek_sdk_bridge.h>
#include <bridge/ui/ui_bridge_base.h>
#include <bridge/ui/ui_bridge_source.h>

void printf_chip_info(Pchip_info pchip)
{
	_disp_l("-- chip information ");
	_disp_l("%-20s : %s", "status", ui_enum_get_str(chip_status,pchip->status));
	_disp_l("%-20s : %08x", "version", pchip->version);
	_disp_l("%-20s : %08x", "chip_id", pchip->chip_module);
	_disp_l("%-20s : %08x", "service", pchip->hal_service);
	_disp_l("%-20s : %08x", "input", pchip->input_support);
	_disp_l("%-20s : %08x", "output", pchip->output_support);
	_disp_l("%-20s : %08x", "peripheral", pchip->peripheral_en);
}

void printf_bridge_info(Pbdevice_info pbinfo)
{
	_disp_l("-- bridge device information ");
	_disp_l("%-20s : %08x", "bversion", pbinfo->bversion);
	_disp_l("%-20s : %08x", "source_enable", pbinfo->source_enable);
	_disp_l("%-20s : %08x", "rfmixer_enable", pbinfo->rfmixer_enable);
	_disp_l("%-20s : %08x", "demod_enable", pbinfo->demod_enable);
	_disp_l("%-20s : %08x", "source_nums", pbinfo->source_nums);
}

void printf_app_end(void)
{
	while (cross_os_get_ch_no_wait() != -1);
	_disp_l("demo finished. press any key to quit");
	while (cross_os_get_ch_no_wait() == -1);
}

void printf_bridge_avsource(const char* name,Pbridge_source psource)
{
	_disp_l("-- source : [%08x:%s] - %s",psource->source_id, name,ui_enum_get_str(bridge_status,psource->status));
	if (psource->status == bstatus_active || psource->status == bstatus_active_protect)
	{
		Pbridge_video_info pvinfo = &psource->video_info;
		Pbridge_audio_info painfo = &psource->audio_info;
		_disp_l("%-20s : %s", "resolution", ui_enum_get_str(video_resolution,pvinfo->resolution));
		_disp_l("%-20s : %s", "framerate", ui_enum_get_str(video_framerate, pvinfo->framerate));
		_disp_l("%-20s : %s", "aspectrate", ui_enum_get_str(video_aspect_rate, pvinfo->aspectrate));
		_disp_l("%-20s : %d", "offset_x", pvinfo->offset_x);
		_disp_l("%-20s : %d", "offset_y", pvinfo->offset_y);
		_disp_l("%-20s : %d", "pixelclk", pvinfo->pixelclk);
		_disp_l("%-20s : %08x", "signal_flag", pvinfo->signal_flag);

		_disp_l("%-20s : %s", "channel", ui_enum_get_str(audio_channel, painfo->channel));
		_disp_l("%-20s : %s", "samplerate", ui_enum_get_str(audio_samplerate, painfo->samplerate));
	}
}
