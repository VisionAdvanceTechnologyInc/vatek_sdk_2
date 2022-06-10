
#include <cross/cross_os_api.h>
#include <pthread.h>

#define MUTEX_EN_DBG                 1

#if MUTEX_EN_DBG
    #define _mlockd                  _cos_log
#else
    #define _mlockd(...)
#endif


#define LINUX_MUTEX_EN_TAG                  0xF8700122

typedef struct _linux_mutex_share
{
    uint32_t tag;
    pthread_mutex_t handle;
    pthread_mutexattr_t mutexattr;
}linux_mutex_share,*Plinux_mutex_share;

typedef struct _linux_mutex
{
    hcross_smem hsmem;
    Plinux_mutex_share hmutex;
    int32_t created;
    int32_t locked;
}linux_mutex,*Plinux_mutex;

vatek_result cross_os_create_mutex_name(const char* tag, hcross_mutex* hmutex)
{
    hcross_smem hsmem = NULL;
    vatek_result nres = cross_os_create_smem(tag,&hsmem,sizeof(linux_mutex_share));
    if(is_vatek_success(nres))
    {
        Plinux_mutex newm = (Plinux_mutex)malloc(sizeof(linux_mutex));
        nres = vatek_memfail;
        if(newm)
        {
            memset(newm,0,sizeof(linux_mutex));
            newm->hmutex = (Plinux_mutex_share)cross_os_get_smem(hsmem);
            newm->hsmem = hsmem;

            _mlockd("mutex map smem : %08x",newm->hmutex->tag);

            if(newm->hmutex->tag != LINUX_MUTEX_EN_TAG)
            {
                nres = (vatek_result)pthread_mutexattr_init(&newm->hmutex->mutexattr);
                _mlockd("pthread_mutexattr_init : %d",nres);
                if(is_vatek_success(nres))
                {
                    nres = (vatek_result)pthread_mutexattr_setpshared(&newm->hmutex->mutexattr, PTHREAD_PROCESS_SHARED);
                    _mlockd("pthread_mutexattr_setpshared : %d",nres);
                    if(is_vatek_success(nres))
                        nres = (vatek_result)pthread_mutex_init(&newm->hmutex->handle,&newm->hmutex->mutexattr);
                    _mlockd("pthread_mutex_init : %d",nres);

                    if(is_vatek_success(nres))
                    {
                        newm->created = 1;
                        newm->hmutex->tag = LINUX_MUTEX_EN_TAG;
                    }
                }
            }else nres = vatek_success;

            if(!is_vatek_success(nres))free(newm);
        }

        if(!is_vatek_success(nres))cross_os_close_smem(hsmem);
        else *hmutex = newm;
    }
    return nres;
}

vatek_result cross_os_open_mutex_name(const char* tag, hcross_mutex* hmuxtex)
{
    hcross_smem hsmem = NULL;
    vatek_result nres = cross_os_open_smem(tag,&hsmem,sizeof(linux_mutex_share));
    if(is_vatek_success(nres))
    {
        Plinux_mutex_share pmshare = (Plinux_mutex_share)cross_os_get_smem(hsmem);
        if(pmshare->tag == LINUX_MUTEX_EN_TAG)
        {
            Plinux_mutex newm = (Plinux_mutex)malloc(sizeof(linux_mutex));
            nres = vatek_memfail;
            if(newm)
            {
                memset(newm,0,sizeof(linux_mutex));
                newm->hmutex = (Plinux_mutex_share)cross_os_get_smem(hsmem);
                newm->hsmem = hsmem;
                newm->created = 0;
                *hmuxtex = newm;
                nres = vatek_success;
            }
        }else nres = vatek_badstatus;

        if(!is_vatek_success(nres))
            cross_os_close_smem(hsmem);
    }
    return nres;
}

vatek_result cross_os_create_mutex(hcross_mutex* hmutex)
{
    static uint32_t noname_count = 0;
    char szname[16];

    sprintf(&szname[0],"nname_%08x",noname_count++);
    return cross_os_create_mutex_name(&szname[0],hmutex);
}

void cross_os_lock_mutex(hcross_mutex hmutex)
{
    Plinux_mutex pmux = (Plinux_mutex)hmutex;
    vatek_result nres = pthread_mutex_lock(&pmux->hmutex->handle);
    if(is_vatek_success(nres))pmux->locked = 1;
}

vatek_result cross_os_lock_mutex_timeout(hcross_mutex hmutex, uint32_t ms)
{
    Plinux_mutex pmux = (Plinux_mutex)hmutex;
    vatek_result nres = vatek_badstatus;
    if(!pmux->locked)
    {
        uint32_t tick = cross_os_get_tick_ms();
        nres = vatek_timeout;
        while((cross_os_get_tick_ms() - tick) <= ms)
        {
            nres = pthread_mutex_trylock(&pmux->hmutex->handle);
            if(is_vatek_success(nres))
            {
                pmux->locked = 1;
                break;
            }
        }
    }
    if(!is_vatek_success(nres))nres = vatek_timeout;
    return nres;
}

vatek_result cross_os_trylock_mutex(hcross_mutex hmutex)
{
    Plinux_mutex pmux = (Plinux_mutex)hmutex;
    vatek_result nres = pthread_mutex_trylock(&pmux->hmutex->handle);
    if(!is_vatek_success(nres))nres = vatek_badstatus;
    return nres;
}

void cross_os_release_mutex(hcross_mutex hmutex)
{
    Plinux_mutex pmux = (Plinux_mutex)hmutex;
    if(pmux->locked)
    {
        pmux->locked = 0;
        pthread_mutex_unlock(&pmux->hmutex->handle);
    }
}

vatek_result cross_os_free_mutex(hcross_mutex hmutex)
{
    Plinux_mutex pmux = (Plinux_mutex)hmutex;
    vatek_result nres = vatek_success;
    cross_os_release_mutex(hmutex);
    
    if(pmux->created)
        nres = pthread_mutex_destroy(&pmux->hmutex->handle);
    cross_os_close_smem(pmux->hsmem);
    free(pmux);
    return nres;
}
