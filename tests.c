#define RJD_IMPL true
#define RJD_ENABLE_ASSERT true
#define RJD_ENABLE_LOGGING true
#define RJD_ENABLE_SHORTNAMES true

#include "rjd_all.h"

////////////////////////////////////////////////////////////////////////////////
// expect utils

#define expect_true(condition) if (!(condition)) { ASSERTFAIL("Expected \"%s\" to be true, but got false\n", #condition); }
#define expect_false(condition) if (condition) { ASSERTFAIL("Expected \"%s\" to be false, but got true\n", #condition); }

void expect_str(const char* expected, const char* actual)
{
	if (expected != actual && strcmp(expected, actual)) {
		printf("Expected:\n%s\nbut got:\n%s\n", expected, actual);
	}
}

void expect_int32(int32_t expected, int32_t actual)
{
	if (expected != actual) {
		ASSERTFAIL("Expected: %d, but got: %d\n", expected, actual);
	}
}

void expect_uint32(uint32_t expected, uint32_t actual)
{
	if (expected != actual) {
		ASSERTFAIL("Expected: %u, but got: %u\n", expected, actual);
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
		"tests.c(58): test\n"
		"tests.c(59): \n"
		"tests.c(60): forma11ed!\n"
		"tests.c(69): ok1\n"
		"tests.c(74): ok2\n"
		"tests.c(75): ok2\n"
		"tests.c(79): ok3\n"
		"tests.c(80): ok3\n"
		"tests.c(81): ok3\n"
		"tests.c(93): other channel\n";

	expect_str(expected, logbuffer);
	
	// reset global logger for future tests
	rjd_log_resetglobal();
}

void test_alloc()
{
	// default allocator
	{
		struct rjd_alloc_context ctx = alloc_initdefault();
		int32_t* v = rmalloc(int32_t, &ctx);
		*v = 1337;
		expect_int32(1337, *v);

		char* p1 = rmalloc_array(char, 128, &ctx);
		strncpy(p1, "thequickbrownfoxjumpedoverthesuperdeduperlazydog!", 128);
		p1[127] = 0;

		char* p2 = rmalloc_array(char, 64, &ctx);
		strncpy(p2, "this fox wasn't as quick as the last one", 64);
		p2[63] = 0;

		rfree(v, &ctx);
		rfree(p1, &ctx);
		rfree(p2, &ctx);
	}

	// linear allocator
	{
		//char stackmem[1024 * 1024];
	}
}

void test_array()
{
	struct rjd_alloc_context context = alloc_initdefault();

	// general functionality
	{
		struct test {
			int a;
			int b;
			int c;
			int d;
		};
		struct test* a = arr_alloc(struct test, 32, &context);
		expect_uint32(arr_count(a), 0);
		expect_uint32(arr_capacity(a), 32);
		expect_true(arr_empty(a));
		expect_false(arr_full(a));

		arr_resize(a, 16);
		expect_uint32(16, arr_count(a));
		expect_uint32(32, arr_capacity(a));

		arr_resize(a, 50);
		expect_uint32(50, arr_count(a));
		expect_uint32(50, arr_capacity(a));

		for (size_t i = 0; i < arr_count(a); ++i) {
			struct test v = { (int)i };
			a[i] = v;
		}
		expect_false(arr_empty(a));
		expect_true(arr_full(a));

		arr_erase(a, 0);
		expect_int32(1, a[0].a);
		arr_erase(a, 1);
		expect_int32(3, a[1].a);

		expect_false(arr_empty(a));
		expect_false(arr_full(a));

		struct test end = arr_pop(a);
		expect_int32(49, end.a);

		arr_resize(a, 0);
		expect_true(arr_empty(a));
		expect_uint32(0, arr_count(a));
		expect_uint32(50, arr_capacity(a));

		arr_push(a, end);
		expect_int32(end.a, a[0].a);
		
		arr_clear(a);
		expect_int32(0, arr_count(a));
		expect_int32(50, arr_capacity(a));

		arr_free(a);
	}

	// first/last
	{
		int32_t* a = arr_alloc(int32_t, 16, &context);
		for (int32_t i = 0; i < (int32_t)arr_capacity(a); ++i) {
			arr_push(a, 0xD00D + i);
		}

		int32_t* b = arr_alloc(int32_t, 16, &context);

		int32_t first = arr_first(b, 1337);
		expect_int32(1337, first);
		first = arr_first(a, 0);
		expect_int32(0xD00D + 0, first);

		int32_t last = arr_last(b, 1337);
		expect_int32(1337, last);
		last = arr_last(a, 0);
		expect_int32(0xD00D + 15, last);

		arr_free(a);
		arr_free(b);
	}

	// functional-style tests
	{
		int32_t* b = arr_alloc(int32_t, 16, &context);
		for (int32_t i = 0; i < 16; ++i) {
			arr_push(b, i);
		}

		#define testfilter(element) (element < 8)
		arr_filter(b, testfilter);
		expect_uint32(8, arr_count(b));

		#define testsum(acc, element) (acc + element)
		int32_t sum1 = 0;
		arr_reduce(b, sum1, testsum);
		expect_int32(1 + 2 + 3 + 4 + 5 + 6 + 7, sum1);

		int32_t sum2 = 0;
		arr_sum(b, sum2);
		expect_int32(sum1, sum2);

		arr_free(b);
	}
}

int main(void) 
{
	test_logging();
	test_alloc();
	test_array();

	return 0;
}

