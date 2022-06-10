
#include <cross/cross_os_api.h>
#include <pthread.h>
#include <signal.h>

typedef struct _linux_thread
{
    pthread_t hthread;
    cross_thread_param c_param;
}linux_thread,*Plinux_thread;

typedef void* (*fppthread_handler)(void*);

hcross_thread cross_os_create_thread(fpcross_thread_function fpfun,void* userparam)
{
    Plinux_thread newh = (Plinux_thread)malloc(sizeof(linux_thread));
    if(newh)
    {
        memset(newh,0,sizeof(linux_thread));
        newh->c_param.result = vatek_success;
        newh->c_param.userparam = userparam;
        if(pthread_create(&newh->hthread,NULL,(fppthread_handler)fpfun,&newh->c_param) >= 0)
            return (hcross_thread)newh;
    }
    return NULL;
}

vatek_result cross_os_join_thread(hcross_thread hthread)
{
    Plinux_thread plthread = (Plinux_thread)hthread;
    vatek_result nres = vatek_success;
    nres = (vatek_result)pthread_kill(plthread->hthread,0);
    if(is_vatek_success(nres))
        nres = pthread_join(plthread->hthread,NULL);
    else nres = vatek_badstatus;
    return nres;
}

vatek_result cross_os_free_thread(hcross_thread hthread)
{
    Plinux_thread plthread = (Plinux_thread)hthread;
    vatek_result nres = cross_os_join_thread(hthread);
    nres = plthread->c_param.result;
    free(plthread);
    return nres;
}

vatek_result cross_os_get_thread_result(hcross_thread hthread)
{
    Plinux_thread plthread = (Plinux_thread)hthread;
    return plthread->c_param.result;
}
