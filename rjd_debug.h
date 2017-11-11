#pragma once

// DEPENDENCIES:
// stdio.h
// stdbool.h
// stdarg.h
// stdint.h

enum rjd_log_verbosity
{
	RJD_LOG_VERBOSITY_LOW,
	RJD_LOG_VERBOSITY_MED,
	RJD_LOG_VERBOSITY_HIGH,
};

struct rjd_logchannel
{
	enum rjd_log_verbosity verbosity;
	bool enabled;
	const char* name;
	void (*hook)(const char* formatted, size_t length);
};

#if RJD_ENABLE_LOGGING
	#define RJD_LOG_CHANNEL(channel, verbosity, ...) rjd_log_impl(__FILE__, __LINE__, channel, verbosity, __VA_ARGS__)
	#define RJD_LOG(...) RJD_LOG_CHANNEL(g_rjd_global_logchannel, RJD_LOG_VERBOSITY_MED, __VA_ARGS__)
#else
	#define RJD_LOG_CHANNEL(channel, ...)
	#define RJD_LOG(...)
#endif

#if RJD_ENABLE_ASSERT
	#define RJD_FORCECRASH() ((*(volatile int*)0) = 0xDEADDEAD)
	#define RJD_ASSERT(condition) RJD_ASSERTMSG(condition, #condition)
	#define RJD_ASSERTMSG(condition, ...) if (!(condition)) { RJD_LOG(__VA_ARGS__); RJD_FORCECRASH(); }
	#define RJD_ASSERTFAIL(...) { RJD_LOG(__VA_ARGS__); RJD_FORCECRASH(); }
#else
	#define RJD_FORCECRASH() ((*(int*)0) = 0xDEADDEAD)
	#define RJD_ASSERT(condition, ...) 
	#define RJD_ASSERTMSG(condition, ...) 
	#define RJD_ASSERTFAIL(...)
#endif

#define RJD_UNUSED_PARAM(param) ((void)param)

void rjd_log_impl(const char* file, unsigned line, const struct rjd_logchannel* channel, enum rjd_log_verbosity verbosity, const char* format, ...);
void rjd_log_resetglobal();

#if RJD_ENABLE_SHORTNAMES
	#define ASSERT RJD_ASSERT
	#define ASSERTMSG RJD_ASSERTMSG
	#define ASSERTFAIL RJD_ASSERTFAIL

	#define LOG_CHANNEL RJD_LOG_CHANNEL
	#define LOG RJD_LOG
	#define log_resetglobal rjd_log_resetglobal

	#define UNUSED_PARAM RJD_UNUSED_PARAM
#endif

extern const struct rjd_logchannel* g_rjd_global_logchannel;

#ifdef RJD_IMPL

const struct rjd_logchannel rjd_global_logchannel = {
	.verbosity = RJD_LOG_VERBOSITY_MED,
	.enabled = true,
	.name = "Default Global",
};
const struct rjd_logchannel* g_rjd_global_logchannel = &rjd_global_logchannel;

void rjd_log_impl(const char* file, unsigned line, const struct rjd_logchannel* channel, enum rjd_log_verbosity verbosity, const char* format, ...)
{
	if (!channel || !channel->enabled) {
		return;
	}

	if (verbosity > channel->verbosity) {
		return;
	}

	va_list args;
	va_start(args, format);

	char rawMessage[4096];

	int written = vsnprintf(rawMessage, sizeof(rawMessage), format, args);
	va_end(args);

	if (written < 0)
	{
		printf("Failed to format message.\n");
		RJD_FORCECRASH();
	}

	static const char* formattedLog = "%s(%u): %s\n";
	static const uint32_t logLength = sizeof("%s(%u): %s\n");
	
	char formatted[4096];

	if (sizeof(formatted) <= logLength + written)
	{
		printf("Static buffer not large enough.\n");
		RJD_FORCECRASH();
	}

	const int size = sprintf(formatted, formattedLog, file, line, rawMessage);

	if (channel->hook) {
		channel->hook(formatted, size);
	} else {
		//OutputDebugString(formatted);
		fwrite(formatted, 1, size, stdout);
		fflush(stdout);
	}
}

void rjd_log_resetglobal()
{
	g_rjd_global_logchannel = &rjd_global_logchannel;
}

#endif // RJD_IMPL

