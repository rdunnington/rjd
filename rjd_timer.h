#pragma once

#define RJD_TIMER_H 1

struct rjd_timer
{
	double timestamp;
};

struct rjd_timer rjd_timer_init(void);
void rjd_timer_reset(struct rjd_timer* timer);
double rjd_timer_elapsed(const struct rjd_timer* timer);
double rjd_timer_global(void);

#define RJD_TIMER_SCOPE_BEGIN(name) struct rjd_timer timer_##name = rjd_timer_init();
#define RJD_TIMER_SCOPE_END_WITHLOG(name, log_function) log_function("Elapsed %s: %.4fms", #name, rjd_timer_elapsed(&timer_##name));
#define RJD_TIMER_SCOPE_END(name)	RJD_TIMER_SCOPE_END_WITHLOG(name, RJD_LOG)

#if RJD_IMPL

struct rjd_timer rjd_timer_init(void)
{
	struct rjd_timer timer;
	rjd_timer_reset(&timer);
	return timer;
}

void rjd_timer_reset(struct rjd_timer* timer)
{
	timer->timestamp = rjd_timer_global();
}

double rjd_timer_elapsed(const struct rjd_timer* timer)
{
	return rjd_timer_global() - timer->timestamp;
}
#if RJD_PLATFORM_WINDOWS
	static double RJD_QPC_FREQUENCY = 0;
	
	double rjd_timer_global(void)
	{
		if (RJD_QPC_FREQUENCY == 0) {
			LARGE_INTEGER frequency = {.QuadPart = 1};
			if (!QueryPerformanceFrequency(&frequency))
			{
				RJD_LOG("Failed to get QueryPerformanceFrequency: %d", GetLastError());
			}
			RJD_QPC_FREQUENCY = (double)frequency.QuadPart;
		}

		LARGE_INTEGER time = { .QuadPart = 0 };
		if (!QueryPerformanceCounter(&time))
		{
			RJD_LOG("Failed to get QueryPerformanceCounter. Time will be incorrect. Error: %d", GetLastError());
		}

		return (time.QuadPart * 1000LL) / RJD_QPC_FREQUENCY;
	}
#endif //RJD_PLATFORM_WINDOWS

#if RJD_PLATFORM_OSX
	#include <mach/mach.h>
	#include <mach/mach_time.h>

	static mach_timebase_info_data_t RJD_MACH_TIMEBASE_INFO;
	double rjd_timer_global(void)
	{
		if (RJD_MACH_TIMEBASE_INFO.denom == 0) {
			int ok = mach_timebase_info(&RJD_MACH_TIMEBASE_INFO);
			if (ok != KERN_SUCCESS) {
				RJD_LOG("Failed to get mach timebase info: %d", ok);
			}
		}

		double time = (double)mach_absolute_time();
		return time * ((double)RJD_MACH_TIMEBASE_INFO.numer / ((double)RJD_MACH_TIMEBASE_INFO.denom)) / 1000000;
	}
#endif // 

#endif // RJD_IMPL

