#pragma once

struct rjd_timer
{
	double timestamp;
};

struct rjd_timer rjd_timer_init(void);
void rjd_timer_reset(struct rjd_timer* timer);
double rjd_timer_elapsed(const struct rjd_timer* timer);
double rjd_timer_global(void);

#define RJD_PROFILE_SCOPE(name, scope) {									\
		struct rjd_timer _timer##name = rjd_timer_init(); 					\
		{scope}																\
		LOG("Elapsed %s: %.2fms", #name, rjd_timer_elapsed(&_timer##name));	\
	}

#if RJD_ENABLE_SHORTNAMES
	#define timer_init		rjd_timer_init
	#define timer_reset		rjd_timer_reset
	#define timer_elapsed	rjd_timer_elapsed
	#define timer_global	rjd_timer_global

	#define PROFILE_SCOPE	RJD_PROFILE_SCOPE
#endif

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

// TODO support osx/linux
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define WIN32_EXTRA_LEANA
	#define NOMINMAX
	#include <windows.h>

	static double RJD_QPC_FREQUENCY = 0;
	
	double rjd_timer_global(void)
	{
		if (RJD_QPC_FREQUENCY == 0) {
			LARGE_INTEGER frequency = {.QuadPart = 1};
			if (!QueryPerformanceFrequency(&frequency))
			{
				LOG("Failed to get QueryPerformanceFrequency: %d", GetLastError());
			}
			RJD_QPC_FREQUENCY = (double)frequency.QuadPart;
		}

		LARGE_INTEGER time = { .QuadPart = 0 };
		if (!QueryPerformanceCounter(&time))
		{
			LOG("Failed to get QueryPerformanceCounter. Time will be incorrect. Error: %d", GetLastError());
		}

		return (time.QuadPart * 1000LL) / RJD_QPC_FREQUENCY;
	}
#endif //_WIN32
#endif // RJD_IMPL

