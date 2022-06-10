#ifndef OBS_VATEK_H
#define OBS_VATEK_H

#include <obs-module.h>
#include <obs-frontend-api.h>
#include <core/vatek_base.h>

#include <QMainWindow>
#include <QAction>
#include <QMessageBox>
#include <QDockWidget>
#include <QMenu>
#include <QMenuBar>

#include <core/qtv_service_usb.h>
#include <ui/qtv_ui_service_usb.h>
#include <ui/base/qtv_ui_tab.h>

#include <cross/cross_os_api.h>

#include <codec/ovatek_codec.h>

#define blog(level, msg, ...) blog(level, "[obs-vatek] " msg, ##__VA_ARGS__)


#define OVATEK_NAME						"obs_vatek"
#define OVATEK_UISTR_MENUSTART			"obs_vatek.menu.start"
#define OVATEK_UISTR_MENUSTOP			"obs_vatek.menu.stop"
#define _OV_STR(n)						obs_module_text(OVATEK_UISTR_##n)

#define OVATEK_PLUGIN_OUTPUT			"obs_vatek_dtv"
#define OVATEK_PREPARE_MS				200

typedef enum _obs_vatek_status
{
	ovatek_status_idle,
	ovatek_status_running,
}obs_vatek_status;

class obsVatekStatusDock : public QDockWidget,public qtvUIMainTabBase
{
public:
	explicit obsVatekStatusDock(QWidget* parent = nullptr);
	~obsVatekStatusDock();

private:
	qtvUITabStatus* m_tabstatus;
};

class obsVatekOutput : protected qtvUSBMuxSource
{
public:
	static vatek_result startBroadcast(qtvUIMainUSBMuxer* umain, obsVatekOutput** obsoutput);
	static vatek_result stopBroadcast(obsVatekOutput* obsoutput);

	~obsVatekOutput();
	Pusbmux_source getHandle() override { return &m_source; }
	virtual vatek_result sendEncoderPacket(encoder_packet* encpkt);

	/* qtvUSBMuxSource */
	vatek_result startProgram(qtvUSBMuxerProgram* program, Pusbmux_program uprogram, hqtv_program* hprogram) override;
	vatek_result pushStream(hqtv_program hprogram, qtvUSBMuxerStream* stream, Pusbmux_stream ustream) override;
	void stopProgram(hqtv_program hprogram) override;

protected:
	explicit obsVatekOutput(qtvUIMainUSBMuxer* main);

	usbmux_source m_source;
	qtvUIMainUSBMuxer* m_hmain;
	qtvServiceUSBMuxer* m_usbmuxer;
	obs_output_t* m_houtput;
	obs_encoder_t* m_videoenc;
	obs_encoder_t* m_audioenc;
	hcross_mutex m_lock;
	qtv_status m_status;
	ovatekCodecStream* m_videostream;
	ovatekCodecStream* m_audiostream;

	vatek_result openOBSHandle();
	vatek_result closeOBSHandle();

	static vatek_result checkChannelValid(qtvUSBMuxerChannel* ch);
	static qtvUSBMuxerStream* getChannelStream(qtvUSBMuxerChannel* ch, qtv_codec_mode mode);
	static void configVideoDetail(Pmux_stream pvideo, video_t* vinfo);
	static void configAudioDetail(Pmux_stream paudio, audio_t* ainfo);

private:
	static vatek_result ovatekSourceStart(husbmux_source hsource);
	static vatek_result  ovatekSourceFreeFrame(husbmux_source hsource, Pusbmux_stream pstream, Pmux_pes_frame pframe);
	static vatek_result  ovatekSourceGetPadding(husbmux_source hsource, uint8_t** packet);
	static void  ovatekSourcePrepare(husbmux_source hsource, uint32_t ms);
	static void ovatekSourceStop(husbmux_source hsource);
};

class obsVatekHandle
{
public:
	static vatek_result connectOBS();
	static obsVatekHandle* getInstance();
	static void disconnectOBS();

	static qtvCodecProvider* _video_codecs() { return m_videocodecs; }
	static qtvCodecProvider* _audio_codecs() { return m_aduiocodecs; }

	static obs_output_info* getPluginOutput();
	static obsVatekOutput* getOutputHandle() { return m_output; }

	static qtvCodecFactory* getVideoCodecByName(const char* name);
	static qtvCodecFactory* getAudioCodecByName(const char* name);

protected:
	obsVatekHandle();
	~obsVatekHandle();

	obs_vatek_status m_status;
	QMainWindow* m_main;
	QMenu* m_menudtv;
	qtvUIMainUSBMuxer* m_uimain;
	obsVatekStatusDock* m_dockstatus;

	void obsMenuClick();

	vatek_result startBroadcast();
	void stopBroadcast();

private:
	static obsVatekHandle* m_instance;
	static QAction* m_menubtn;
	static qtvCodecSimpleProvider* m_videocodecs;
	static qtvCodecSimpleProvider* m_aduiocodecs;
	static obsVatekOutput* m_output;

	static void obsEventCb(enum obs_frontend_event oevent, void* private_data);
	static void obsMenuClickCb();
};

#endif
