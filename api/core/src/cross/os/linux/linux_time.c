
#include <cross/cross_os_api.h>
#include <time.h>
#include <unistd.h>
#include <sched.h>

#define TIME_SECOND_TO_NS		    1000000000
#define TIME_SECOND_TO_US		    1000000

void cross_os_sleep(int32_t ms)
{
	cross_os_usleep(ms * 1000);
}

void cross_os_sleep_yield()
{
	sched_yield();	/* always success */
}

uint32_t cross_os_get_tick_ms(void)
{
	struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);	
}

void cross_os_usleep(uint32_t us)
{
	struct timespec ts;
	int32_t res;
	ts.tv_sec = us / TIME_SECOND_TO_US;
	ts.tv_nsec = (us % TIME_SECOND_TO_US) * 1000;
	res = nanosleep(&ts,NULL);
	if(res < 0)sched_yield();
}

void cross_os_wait_unit(struct timespec* target)
{
	uint64_t curtick = cross_os_get_time_us();
	uint64_t timetick = cross_os_time_to_us(target);

	if (curtick > timetick)return;
	curtick = timetick - curtick;
	cross_os_usleep((uint32_t)curtick);
}

void cross_os_time_plus_ms(struct timespec* tp,int32_t ms)
{
	if (ms >= 1000)
	{
		tp->tv_sec += ms / 1000;
		ms %= 1000;
	}

	tp->tv_nsec += ms * 1000;
	if (tp->tv_nsec >= TIME_SECOND_TO_NS)
	{
		tp->tv_sec++;
		tp->tv_nsec -= TIME_SECOND_TO_NS;
	}
}

void cross_os_time_plus_time(struct timespec* tp,struct timespec* tappend)
{
	tp->tv_nsec += tappend->tv_nsec;
	tp->tv_sec += tappend->tv_sec;

	if (tp->tv_nsec > TIME_SECOND_TO_NS)
	{
		tp->tv_sec++;
		tp->tv_nsec -= TIME_SECOND_TO_NS;
	}
}

vatek_result cross_os_get_time(struct timespec * tp)
{
	struct timespec ts;
    if(clock_gettime(CLOCK_MONOTONIC, tp) >= 0)return vatek_success;
    return vatek_hwfail;
}

uint64_t cross_os_get_time_us()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ((uint64_t)ts.tv_sec * 1000000 + ts.tv_nsec / 1000);	
}

uint64_t cross_os_time_to_us(struct timespec* tp)
{
    return (tp->tv_sec * 1000000) + (tp->tv_nsec / 1000);
}

vatek_result cross_os_time_eclipse(struct timespec* st,struct timespec* eclipse)
{
	struct timespec curtime;
	vatek_result nres = cross_os_get_time(&curtime);
	if (is_vatek_success(nres))
	{
		if (curtime.tv_nsec > st->tv_nsec)
		{
			eclipse->tv_nsec = st->tv_nsec + (1000000000UL - curtime.tv_nsec);
			eclipse->tv_sec = (curtime.tv_sec - st->tv_sec) - 1;
		}
		else
		{
			eclipse->tv_nsec = curtime.tv_nsec - st->tv_nsec;
			eclipse->tv_sec = curtime.tv_sec - st->tv_sec;
		}
	}
	return nres;
}
