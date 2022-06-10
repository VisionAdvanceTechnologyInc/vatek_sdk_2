#include <obs_vatek.h>
#include <codec/ovatek_codec.h>
#include <codec/ffmpeg/ovatek_ffmpeg.h>

/* obs_vatek_ui */

obsVatekHandle* obsVatekHandle::m_instance = NULL;
QAction* obsVatekHandle::m_menubtn = NULL;
qtvCodecSimpleProvider* obsVatekHandle::m_videocodecs = NULL;
qtvCodecSimpleProvider* obsVatekHandle::m_aduiocodecs = NULL;
obsVatekOutput* obsVatekHandle::m_output = NULL;

/* obs_vatek_status*/

obsVatekHandle* obsVatekHandle::getInstance()
{
	return obsVatekHandle::m_instance;
}

vatek_result obsVatekHandle::connectOBS()
{
	vatek_result nres = vatek_badstatus;
	if (!m_instance)
	{
		obsVatekHandle* newhandle = new obsVatekHandle();
		QMainWindow* mwnd = (QMainWindow*)obs_frontend_get_main_window();
		m_menubtn = (QAction*)obs_frontend_add_tools_menu_qaction(_OV_STR(MENUSTART));
		m_menubtn->connect(m_menubtn, &QAction::triggered, obsVatekHandle::obsMenuClickCb);

		nres = ovatekCodec::createVideoProvider(&m_videocodecs);
		if (is_vatek_success(nres))
		{
			nres = ovatekCodec::createAudioProvider(&m_aduiocodecs);
			if (!is_vatek_success(nres))delete m_videocodecs;
		}

		if (is_vatek_success(nres))
			obs_frontend_add_dock(newhandle->m_dockstatus);

		if (!is_vatek_success(nres))delete newhandle;
		else
		{
			obs_register_output(obsVatekHandle::getPluginOutput());
			ovatekFFmpegFactory::registerCodecs(m_videocodecs, m_aduiocodecs);
			m_instance = newhandle;
		}
	}
	return nres;
}

qtvCodecFactory* obsVatekHandle::getVideoCodecByName(const char* name)
{
	int32_t i = 0;
	const QList<qtvCodecFactory*> codecs = m_videocodecs->getCodecs();
	for (i = 0; i < codecs.count(); i++)
	{
		if (codecs[i]->_name().compare(name) == 0)
			return codecs[i];
	}
	return 0;
}

qtvCodecFactory* obsVatekHandle::getAudioCodecByName(const char* name)
{
	int32_t i = 0;
	const QList<qtvCodecFactory*> codecs = m_aduiocodecs->getCodecs();
	for (i = 0; i < codecs.count(); i++)
	{
		if (codecs[i]->_name().compare(name) == 0)
			return codecs[i];
	}
	return 0;
}

void obsVatekHandle::disconnectOBS()
{
	if (m_instance)
		delete m_instance;
	if (m_videocodecs)delete m_videocodecs;
	if (m_aduiocodecs)delete m_aduiocodecs;

	m_instance = NULL;
	m_videocodecs = NULL;
	m_aduiocodecs = NULL;
}

obsVatekHandle::obsVatekHandle() :
	m_status(ovatek_status_idle),
	m_main((QMainWindow*)obs_frontend_get_main_window()),
	m_uimain(NULL),
	m_dockstatus(NULL)
{
	m_dockstatus = new obsVatekStatusDock(m_main);
	m_main->addDockWidget(Qt::BottomDockWidgetArea, m_dockstatus);
}

obsVatekHandle::~obsVatekHandle()
{
	stopBroadcast();
}

vatek_result obsVatekHandle::startBroadcast()
{
	vatek_result nres = vatek_badstatus;
	if (!m_uimain)
	{
		qtvUIWizardUSBMuxer* newwizard = NULL;
		nres = qtvUIWizardUSBMuxer::createWizard(m_videocodecs, m_aduiocodecs, &newwizard, m_main);
		if (is_vatek_success(nres))
		{
			nres = newwizard->startWizard();
			if (is_vatek_success(nres))
			{
				newwizard->getResult(&m_uimain);
				nres = obsVatekOutput::startBroadcast(m_uimain, &m_output);
				if (is_vatek_success(nres))
				{
					m_status = ovatek_status_running;
					m_dockstatus->setService(m_uimain->_service());
				}
				else
				{
					delete m_uimain;
					m_uimain = NULL;
				}
			}
			qtvUIWizardUSBMuxer::freeWizard(newwizard);
		}
	}
	return nres;
}

void obsVatekHandle::stopBroadcast()
{
	if (m_uimain)
	{
		vatek_result nres = obsVatekOutput::stopBroadcast(m_output);
		m_dockstatus->setService(NULL);
		if (m_uimain)delete m_uimain;
	}
	m_uimain = NULL;
	m_output = NULL;
	m_status = ovatek_status_idle;
}

void obsVatekHandle::obsMenuClick()
{
	if (m_status == ovatek_status_idle)
	{
		vatek_result nres = startBroadcast();
		if (is_vatek_success(nres))
			m_menubtn->setText(_OV_STR(MENUSTOP));
	}
	else if (m_status == ovatek_status_running)
	{
		stopBroadcast();	
		m_menubtn->setText(_OV_STR(MENUSTART));
	}
}

void obsVatekHandle::obsEventCb(enum obs_frontend_event oevent, void* private_data)
{

}

void obsVatekHandle::obsMenuClickCb()
{
	if(obsVatekHandle::m_instance)
		obsVatekHandle::m_instance->obsMenuClick();
}

obsVatekStatusDock::obsVatekStatusDock(QWidget* parent) :
	m_tabstatus(NULL),
	qtvUIMainTabBase(qui_main_maintabs)
{
	m_tabstatus = new qtvUITabStatus(this, this);
	setWindowTitle("DTV Status");
	QDockWidget::setWidget(m_tabstatus);
	setAllowedAreas(Qt::BottomDockWidgetArea);
	setMinimumSize(QSize(400, 200));
	setFeatures(features() & ~QDockWidget::DockWidgetClosable);
	m_tabstatus->tabEnter();
}

obsVatekStatusDock::~obsVatekStatusDock()
{
	m_tabstatus->tabLeave();
	if (m_tabstatus)delete m_tabstatus;
}