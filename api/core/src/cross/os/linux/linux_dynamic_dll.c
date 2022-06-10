
#include <cross/cross_os_api.h>
#include <dlfcn.h>

hcross_dll cross_os_dll_load(const char* dllfile)
{
    void* ptrdll = dlopen(dllfile,RTLD_LAZY);
    if(ptrdll)return ptrdll;
    return NULL;
}

void* cross_os_dll_get_function(hcross_dll hdll,const char* name)
{
    void* fpfun = dlsym(hdll,name);
    return fpfun;
}

vatek_result cross_os_dll_free(hcross_dll hdll)
{
    vatek_result nres = dlclose(hdll);
    return nres;
}

vatek_result cross_os_dll_valid(const char* name)
{
    char* sztmp = (char*)malloc(256);
    if(sztmp)
    {
        char* pch = NULL;
        strncpy(&sztmp[0],name,256);
        pch = strtok(&sztmp[0],".");
        while (pch != NULL)
        {
            if(strcmp(pch,"so") == 0)return vatek_success;
            pch = strtok (NULL, ".");
        }
        free(sztmp);
    }
    return vatek_badparam;
}
