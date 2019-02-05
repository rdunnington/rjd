#pragma once

#define RJD_DEBUG_H 1

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


#define RJD_NAMEGEN2(a, b) a##b
#define RJD_NAMEGEN(a, b) RJD_NAMEGEN2(a, b)

#if RJD_COMPILER_MSVC
	#define RJD_STATIC_ASSERTMSG(condition, message) typedef int RJD_NAMEGEN(rjd_staticassert_fail, __COUNTER__)[(condition) ? 1 : -1]
#else
	#define RJD_STATIC_ASSERTMSG(condition, message) _Static_assert(condition, message)
#endif
#define RJD_STATIC_ASSERT(condition) RJD_STATIC_ASSERTMSG(condition, #condition)

#if RJD_COMPILER_CLANG || RJD_COMPILER_GCC
	#define RJD_SAME_TYPE_TEST(a, b) (__builtin_types_compatible_p(__typeof__(a), __typeof__(b)))
#else
	#define RJD_SAME_TYPE_TEST(a, b) (1)
#endif
#define RJD_STATIC_TEST(a) (sizeof(int[(a)?1:-1]) * 0)
#define RJD_MUST_BE_SAME_TYPE_TEST(a,b) RJD_STATIC_TEST(RJD_SAME_TYPE_TEST(a,b))

#if RJD_COMPILER_MSVC
	#define RJD_TRAP() __debugbreak()
#elif RJD_COMPILER_GCC || RJD_COMPILER_CLANG
	#define RJD_TRAP() __builtin_trap()
#endif

#if RJD_ENABLE_ASSERT
	#define RJD_ASSERT(condition) RJD_ASSERTMSG(condition, #condition)
	#define RJD_ASSERTMSG(condition, ...) if (!(condition)) { RJD_LOG(__VA_ARGS__); RJD_TRAP(); }
	#define RJD_ASSERTFAIL(...) { RJD_LOG(__VA_ARGS__); RJD_TRAP(); }
#else
	#define RJD_ASSERT(condition, ...) 
	#define RJD_ASSERTMSG(condition, ...) 
	#define RJD_ASSERTFAIL(...)
#endif

#define RJD_UNUSED_PARAM(param) ((void)(param))

void rjd_log_impl(const char* file, unsigned line, const struct rjd_logchannel* channel, enum rjd_log_verbosity verbosity, const char* format, ...);
void rjd_log_resetglobal(void);

extern const struct rjd_logchannel* g_rjd_global_logchannel;

#if RJD_IMPL

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
		RJD_TRAP();
	}

	static const char* formattedLog = "%s(%u): %s\n";
	static const uint32_t logLength = sizeof("%s(%u): %s\n");
	
	char formatted[4096];

	if (sizeof(formatted) <= logLength + written)
	{
		printf("Static buffer not large enough.\n");
		RJD_TRAP();
	}

	const int size = sprintf(formatted, formattedLog, file, line, rawMessage);

	if (channel->hook) {
		channel->hook(formatted, size);
	} else {
		RJD_COMPILER_MSVC_ONLY(OutputDebugString(formatted));
		fwrite(formatted, 1, size, stdout);
		fflush(stdout);
	}
}

void rjd_log_resetglobal()
{
	g_rjd_global_logchannel = &rjd_global_logchannel;
}

#endif // RJD_IMPL

