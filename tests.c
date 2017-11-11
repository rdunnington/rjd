#define RJD_IMPL true
#define RJD_ENABLE_ASSERT true
#define RJD_ENABLE_LOGGING true
#define RJD_ENABLE_SHORTNAMES true

#include "rjd_all.h"

////////////////////////////////////////////////////////////////////////////////
// expect utils

void expect_str(const char* expected, const char* actual)
{
	if (expected != actual && strcmp(expected, actual)) {
		printf("Expected:\n%s\nbut got:\n%s\n", expected, actual);
	}
}

////////////////////////////////////////////////////////////////////////////////

char logbuffer[1024 * 128];
size_t logbuffer_pos = 0;
void test_log_hook(const char* msg, size_t length)
{
	UNUSED_PARAM(length);

	strncpy(logbuffer + logbuffer_pos, msg, sizeof(logbuffer) - logbuffer_pos);
	logbuffer_pos += length;
}

void test_logging()
{
	struct rjd_logchannel local = {
		.name = "Test Channel",
		.enabled = true,
		.hook = test_log_hook,
		.verbosity = RJD_LOG_VERBOSITY_MED,
	};
	g_rjd_global_logchannel = &local;

	// normal tests
	LOG("test");
	LOG("");
	LOG("%s%d%d%s", "forma", 1, 1, "ed!");

	// disabled
	local.enabled = false;
	LOG("shouldn't get printed");
	local.enabled = true;

	// verbosity
	local.verbosity = RJD_LOG_VERBOSITY_LOW;
	LOG_CHANNEL(&local, RJD_LOG_VERBOSITY_LOW, "ok1");
	LOG_CHANNEL(&local, RJD_LOG_VERBOSITY_MED, "nope");
	LOG_CHANNEL(&local, RJD_LOG_VERBOSITY_HIGH, "nope");

	local.verbosity = RJD_LOG_VERBOSITY_MED;
	LOG_CHANNEL(&local, RJD_LOG_VERBOSITY_LOW, "ok2");
	LOG_CHANNEL(&local, RJD_LOG_VERBOSITY_MED, "ok2");
	LOG_CHANNEL(&local, RJD_LOG_VERBOSITY_HIGH, "nope");

	local.verbosity = RJD_LOG_VERBOSITY_HIGH;
	LOG_CHANNEL(&local, RJD_LOG_VERBOSITY_LOW, "ok3");
	LOG_CHANNEL(&local, RJD_LOG_VERBOSITY_MED, "ok3");
	LOG_CHANNEL(&local, RJD_LOG_VERBOSITY_HIGH, "ok3");

	local.verbosity = RJD_LOG_VERBOSITY_MED;

	// other channel
	struct rjd_logchannel local2 = {
		.name = "Test2",
		.enabled = true,
		.hook = test_log_hook,
		.verbosity = RJD_LOG_VERBOSITY_MED,
	};

	LOG_CHANNEL(&local2, RJD_LOG_VERBOSITY_MED, "other channel");

	// expect equals
	const char* expected = 
		"tests.c(41): test\n"
		"tests.c(42): \n"
		"tests.c(43): forma11ed!\n"
		"tests.c(52): ok1\n"
		"tests.c(57): ok2\n"
		"tests.c(58): ok2\n"
		"tests.c(62): ok3\n"
		"tests.c(63): ok3\n"
		"tests.c(64): ok3\n"
		"tests.c(76): other channel\n";

	expect_str(expected, logbuffer);
	
	// reset global logger for future tests
	rjd_log_resetglobal();
}

void test_alloc()
{
	// default allocator
	{
		//struct rjd_alloc_context ctx = alloc_initdefault();
		//char* p1 = (char*)rmalloc(
	}

	// linear allocator
	{
		//char stackmem[1024 * 1024];
	}
}

void test_array()
{
//	char stackmem[1024 * 128];
//	memHeap heap = memInitHeap(stackmem, sizeof(stackmem));
//
//	int32* a = rjd_alloc(int32, 8, &heap);
//	rjd_assert(rjd_capacity(a) == 8);
//	rjd_assert(rjd_array_count(a) == 0);
//
//	for (int32 i = 0; i < 8; ++i)
//	{
//		rjd_push(a, i);
//		rjd_assert(a[i] == i);
//		rjd_assert(rjd_array_count(a) == (uint32)i + 1);
//	}
//
//	for (int32 i = 0; rjd_array_count(a) > 0; ++i)
//	{
//		rjd_erase(a, 0);
//		if (rjd_array_count(a) > 0)
//		{
//			rjd_assert(a[0] == i + 1);
//		}
//	}
}

int main(void) 
{
	test_logging();
	test_alloc();
	test_array();

	return 0;
}

