
#include <cross/cross_os_api.h>
#include <semaphore.h>

typedef struct _cross_sem_t
{
    sem_t hsem;
    struct timespec ts;
}cross_sem_t,*Pcross_sem_t;

vatek_result cross_os_create_event(const char* tag, hcross_event* hevent)
{
    Pcross_sem_t nesem = malloc(sizeof(cross_sem_t));
    vatek_result nres = vatek_memfail;
    if(nesem)
    {
        memset(nesem,0,sizeof(cross_sem_t));
        nres = (vatek_result)sem_init(&nesem->hsem,0,1);
        if(!is_vatek_success(nres))
            free(nesem);
        else *hevent = nesem;
    }
    return nres;
}

vatek_result cross_os_open_event(const char* tag, hcross_event* hevent)
{
    return vatek_unsupport;
}

vatek_result cross_os_wait_event_timeout(hcross_event hevent, int32_t ms)
{
    Pcross_sem_t psem = (Pcross_sem_t)hevent;
	#if 0
		clock_gettime(CLOCK_MONOTONIC, &psem->ts);
		psem->ts.tv_nsec += ms * 1000000;
		psem->ts.tv_sec += psem->ts.tv_nsec / 1000000000; 
		psem->ts.tv_nsec %= 1000000000;
		sem_timedwait(&psem->hsem, &psem->ts);
	#else
		int32_t res = sem_trywait(&psem->hsem);
		if(res < 0)cross_os_sleep(ms);
	#endif
    return vatek_success;
}

vatek_result cross_os_wait_event(hcross_event hevent)
{
    Pcross_sem_t psem = (Pcross_sem_t)hevent;
    sem_wait(&psem->hsem);
    return vatek_success;
}

vatek_result cross_os_raise_event(hcross_event hevent)
{
    Pcross_sem_t psem = (Pcross_sem_t)hevent;
    sem_post(&psem->hsem);
    return vatek_success;
}

void cross_os_free_event(hcross_event hevent)
{
    Pcross_sem_t psem = (Pcross_sem_t)hevent;
    sem_close(&psem->hsem);
    free(psem);
}
