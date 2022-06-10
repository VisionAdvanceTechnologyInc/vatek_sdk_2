#include <obs_vatek.h>

OBS_DECLARE_MODULE()
OBS_MODULE_AUTHOR("Wu Chen Hsui (JS_WU)")
OBS_MODULE_USE_DEFAULT_LOCALE("obs-vatek", "en-US")

bool obs_module_load(void)
{
    vatek_result nres = obsVatekHandle::connectOBS();
    blog(LOG_INFO,"connect obs-studio : [%d]", nres);
    if (!is_vatek_success(nres))return false;
    return true;
}

void obs_module_unload()
{
    obsVatekHandle::disconnectOBS();
    blog(LOG_INFO, "disconnect obs-studio");
}
