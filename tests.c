#include <math.h>
#include <limits.h>
#include "tests_rjd_wrapped.h"

////////////////////////////////////////////////////////////////////////////////
// expect utils

#define expect_true(condition) if (!(condition)) { RJD_ASSERTFAIL("Expected \"%s\" to be true, but got false\n", #condition); }
#define expect_false(condition) if (condition) { RJD_ASSERTFAIL("Expected \"%s\" to be false, but got true\n", #condition); }

// #define expect_str(expected, actual) 
// 	if (expected != actual && (expected == NULL || actual == NULL || strcmp(expected, actual))) { 
// 		RJD_ASSERTFAIL("Expected:\n%s\nbut got:\n%s\n", expected ? expected : "", actual ? actual : ""); }

void expect_str(const char* expected, const char* actual)
{
	if (expected != actual && (expected == NULL || actual == NULL || strcmp(expected, actual))) {
		RJD_ASSERTFAIL("Expected:\n%s\nbut got:\n%s\n", expected ? expected : "", actual ? actual : "");
	}
}

void expect_float(double expected, double actual)
{
	if (fabs(expected - actual) > 0.00001) {
		RJD_ASSERTFAIL("Expected: %f, but got: %f", expected, actual);
	}
}

void expect_bool(bool expected, bool actual)
{
	if (expected != actual) {
		RJD_ASSERTFAIL("Expected: %d, but got: %d", expected, actual);
	}
}

void expect_int64(int64_t expected, int64_t actual) {
	if (expected != actual) {
		RJD_ASSERTFAIL("Expected: %lld, but got: %lld\n", expected, actual);
	}
}

void expect_int32(int32_t expected, int32_t actual) {
	if (expected != actual) {
		RJD_ASSERTFAIL("Expected: %d, but got: %d\n", expected, actual);
	}
}

void expect_int16(int16_t expected, int16_t actual) {
	if (expected != actual) {
		RJD_ASSERTFAIL("Expected: %d, but got: %d\n", expected, actual);
	}
}

void expect_int8(int8_t expected, int8_t actual) {
	if (expected != actual) {
		RJD_ASSERTFAIL("Expected: %d, but got: %d\n", expected, actual);
	}
}

void expect_uint64(uint64_t expected, uint64_t actual) {
	if (expected != actual) {
		RJD_ASSERTFAIL("Expected: %lld, but got: %lld\n", expected, actual);
	}
}

void expect_uint32(uint32_t expected, uint32_t actual) {
	if (expected != actual) {
		RJD_ASSERTFAIL("Expected: %d, but got: %d\n", expected, actual);
	}
}

void expect_uint16(uint16_t expected, uint16_t actual) {
	if (expected != actual) {
		RJD_ASSERTFAIL("Expected: %d, but got: %d\n", expected, actual);
	}
}

void expect_uint8(uint8_t expected, uint8_t actual) {
	if (expected != actual) {
		RJD_ASSERTFAIL("Expected: %d, but got: %d\n", expected, actual);
	}
}
	

void expect_pointer(const void* expected, const void* actual) {
	if (expected != actual) {
		RJD_ASSERTFAIL("Expected: %p, but got: %p\n", expected, actual);
	}
}

void expect_path(const char* expected, struct rjd_path path)
{
	expect_str(expected, rjd_path_get(&path));
}

#define expect_result_ok(actual) RJD_ASSERTMSG(actual.error == NULL, "Expected OK result, but got error '%s'", actual.error)

#define expect_result_notok(actual) RJD_ASSERTMSG(actual.error != NULL, "Expected bad result, but got OK")
// void expect_result_ok(struct rjd_result actual)
// {
// 	RJD_ASSERTMSG(actual.error == NULL, "Expected OK result, but got error '%s'", actual.error);
// }

// void expect_result_notok(struct rjd_result actual)
// {
// 	RJD_ASSERTMSG(actual.error != NULL, "Expected bad result, but got OK");
// }

void expect_no_leaks(const struct rjd_mem_allocator* allocator)
{
	struct rjd_mem_allocator_stats stats = rjd_mem_allocator_getstats(allocator);
	RJD_ASSERTMSG(rjd_atomic_uint64_get(&stats.current.used) == 0, "Found some leaks");
}

////////////////////////////////////////////////////////////////////////////////

static int RJD_COMPILER_MSVC_ONLY(__cdecl) compare_int32(const void* a, const void* b)
{
	int32_t aa = *(int32_t*)a;
	int32_t bb = *(int32_t*)b;

	if (aa == bb) {
		return 0;
	} else if (aa < bb) {
		return -1;
	} else {
		return 1;
	}
}

static int RJD_COMPILER_MSVC_ONLY(__cdecl) compare_int32_c(void* context, const void* a, const void* b)
{
	RJD_ASSERT(context);
	return compare_int32(a, b);
}

////////////////////////////////////////////////////////////////////////////////

RJD_STATIC_ASSERT(true);
RJD_STATIC_ASSERT(1 == 1);
RJD_STATIC_ASSERT(sizeof(uint32_t) == sizeof(char) * 4);

////////////////////////////////////////////////////////////////////////////////

char g_logbuffer[1024 * 128];
size_t g_logbuffer_pos = 0;
void test_log_hook(const char* msg, size_t length)
{
	strncpy(g_logbuffer + g_logbuffer_pos, msg, sizeof(g_logbuffer) - g_logbuffer_pos);
	g_logbuffer_pos += length;
}

void test_logging_redirect_to_logbuffer(void)
{
	static struct rjd_logchannel local = {
		.name = "Test Channel",
		.enabled = true,
		.hook = test_log_hook,
		.verbosity = RJD_LOG_VERBOSITY_MED,
	};
	g_rjd_global_logchannel = &local;
}

void test_logging_reset(void)
{
	g_logbuffer_pos = 0;
	g_logbuffer[0] = 0;
	rjd_log_resetglobal();
}

////////////////////////////////////////////////////////////////////////////////

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
	const int line_test_begin = __LINE__;
	RJD_LOG("test");
	RJD_LOG("");
	RJD_LOG("%s%d%d%s", "forma", 1, 1, "ed!");

	// disabled
	local.enabled = false;
	RJD_LOG("shouldn't get printed");
	local.enabled = true;

	// verbosity
	local.verbosity = RJD_LOG_VERBOSITY_LOW;
	RJD_LOG_CHANNEL(&local, RJD_LOG_VERBOSITY_LOW, "ok1");
	RJD_LOG_CHANNEL(&local, RJD_LOG_VERBOSITY_MED, "nope");
	RJD_LOG_CHANNEL(&local, RJD_LOG_VERBOSITY_HIGH, "nope");

	local.verbosity = RJD_LOG_VERBOSITY_MED;
	RJD_LOG_CHANNEL(&local, RJD_LOG_VERBOSITY_LOW, "ok2");
	RJD_LOG_CHANNEL(&local, RJD_LOG_VERBOSITY_MED, "ok2");
	RJD_LOG_CHANNEL(&local, RJD_LOG_VERBOSITY_HIGH, "nope");

	local.verbosity = RJD_LOG_VERBOSITY_HIGH;
	RJD_LOG_CHANNEL(&local, RJD_LOG_VERBOSITY_LOW, "ok3");
	RJD_LOG_CHANNEL(&local, RJD_LOG_VERBOSITY_MED, "ok3");
	RJD_LOG_CHANNEL(&local, RJD_LOG_VERBOSITY_HIGH, "ok3");

	local.verbosity = RJD_LOG_VERBOSITY_MED;

	// other channel
	struct rjd_logchannel local2 = {
		.name = "Test2",
		.enabled = true,
		.hook = test_log_hook,
		.verbosity = RJD_LOG_VERBOSITY_MED,
	};

	RJD_LOG_CHANNEL(&local2, RJD_LOG_VERBOSITY_MED, "other channel");
	
	// on windows this is the filename, but on osx this is the fully-qualified path
	const char* filename = __FILE__;

	// expect equals
	char expected[1024];
	snprintf(expected, sizeof(expected),
		"%s(%d): test\n"
		"%s(%d): \n"
		"%s(%d): forma11ed!\n"
		"%s(%d): ok1\n"
		"%s(%d): ok2\n"
		"%s(%d): ok2\n",
		filename, line_test_begin + 1,
		filename, line_test_begin + 2,
		filename, line_test_begin + 3,
		filename, line_test_begin + 12,
		filename, line_test_begin + 17,
		filename, line_test_begin + 18);

	snprintf(expected + strlen(expected), sizeof(expected) - strlen(expected),
		"%s(%d): ok3\n"
		"%s(%d): ok3\n"
		"%s(%d): ok3\n"
		"%s(%d): other channel\n",
		filename, line_test_begin + 22,
		filename, line_test_begin + 23,
		filename, line_test_begin + 24,
		filename, line_test_begin + 36);

	expect_str(expected, g_logbuffer);
	
	test_logging_reset();
}

struct rjd_result check_result(bool condition) {
	RJD_RESULT_CHECK(condition, "not ok");
	return RJD_RESULT_OK();
}

struct rjd_result promote_result(struct rjd_result result) {
	RJD_RESULT_PROMOTE(result);
	return RJD_RESULT_OK();
}

void test_result()
{
	struct rjd_result r1 = RJD_RESULT("not ok");
	struct rjd_result r2 = RJD_RESULT_OK();

	expect_false(rjd_result_isok(r1));
	expect_true(rjd_result_isok(r2));

	expect_true(rjd_result_isok(check_result(true)));
	expect_false(rjd_result_isok(check_result(false)));

	expect_true(promote_result(r1).error == r1.error);
	expect_true(rjd_result_isok(promote_result(r2)));
}

#define TEST_ENUM1_LIST(macro)	\
	macro(e1_a)					\
	macro(e1_b)					\
	macro(e1_c)					\
	macro(e1_d)

#define TEST_ENUM2_LIST(macro)	\
	macro(e2_ok, "OK")			\
	macro(e2_notok, "NOTOK")

RJD_ENUM_DECLARE(e1, TEST_ENUM1_LIST);
RJD_ENUM_DEFINE(e1, TEST_ENUM1_LIST);

RJD_ENUM_DECLARE_WITH_STRINGS(e2, TEST_ENUM2_LIST);
RJD_ENUM_DEFINE_WITH_STRINGS(e2, TEST_ENUM2_LIST);

void test_enum()
{
	// enum 1
	expect_str("e1_a", s_e1_strings[0]);
	expect_str("e1_b", s_e1_strings[1]);
	expect_str("e1_c", s_e1_strings[2]);
	expect_str("e1_d", s_e1_strings[3]);

	expect_uint32(4, k_e1_count);

	expect_str("e1_a", e1_tostring(e1_a));
	expect_str("e1_b", e1_tostring(e1_b));
	expect_str("e1_c", e1_tostring(e1_c));
	expect_str("e1_d", e1_tostring(e1_d));

	enum e1 e1_value;
	expect_true(e1_parse("e1_a", &e1_value) && e1_value == e1_a);
	expect_true(e1_parse("e1_b", &e1_value) && e1_value == e1_b);
	expect_true(e1_parse("e1_c", &e1_value) && e1_value == e1_c);
	expect_true(e1_parse("e1_d", &e1_value) && e1_value == e1_d);
	expect_false(e1_parse("e1_e", &e1_value));

	// enum 2
	expect_str("OK", s_e2_strings[0]);
	expect_str("NOTOK", s_e2_strings[1]);

	char test[k_e2_count];
	expect_uint32(2, rjd_countof(test));

	expect_str("OK", e2_tostring(e2_ok));
	expect_str("NOTOK", e2_tostring(e2_notok));

	enum e2 e2_value;
	expect_true(e2_parse("OK", &e2_value) && e2_value == e2_ok);
	expect_true(e2_parse("NOTOK", &e2_value) && e2_value == e2_notok);
}

void test_hash()
{
	const char* str1 = "test1";
	const char* str2 = "a longer string that has a bunch of characters in it!!$%^&*(";
	const uint8_t* data1 = (const uint8_t*)str1;
	const uint8_t* data2 = (const uint8_t*)str2;
	const uint8_t* data3 = NULL;

	expect_uint32(rjd_hash32_data(data1, (uint32_t)strlen((const char*)data1)).value, rjd_hash32_data(data1, -1).value);
	expect_uint32(rjd_hash32_data(data2, (uint32_t)strlen((const char*)data2)).value, rjd_hash32_data(data2, -1).value);
	expect_uint32(rjd_hash32_data(data3, 0).value, rjd_hash32_data(data3, -1).value);
	expect_uint32(rjd_hash32_str(str1).value, rjd_hash32_data(data1, -1).value);
	expect_uint32(rjd_hash32_str(str2).value, rjd_hash32_data(data2, -1).value);

	expect_uint64(rjd_hash64_data(data1, (uint32_t)strlen((const char*)data1)).value, rjd_hash64_data(data1, -1).value);
	expect_uint64(rjd_hash64_data(data2, (uint32_t)strlen((const char*)data2)).value, rjd_hash64_data(data2, -1).value);
	expect_uint64(rjd_hash64_data(data3, 0).value, rjd_hash64_data(data3, -1).value);
	expect_uint64(rjd_hash64_str(str1).value, rjd_hash64_data(data1, -1).value);
	expect_uint64(rjd_hash64_str(str2).value, rjd_hash64_data(data2, -1).value);
}

void test_mem()
{
	//macros
	{
		#if RJD_COMPILER_MSVC
			#pragma warning(push)
			#pragma warning(disable:4127) // conditional expression is constant (yes we know, that's the point of this test)
		#endif

		expect_true(RJD_MEM_ISALIGNED(0, 4));
		expect_true(RJD_MEM_ISALIGNED(4, 4));
		expect_true(RJD_MEM_ISALIGNED(8, 4));
		expect_true(RJD_MEM_ISALIGNED(8, 8));
		expect_true(RJD_MEM_ISALIGNED(16, 8));
		expect_true(RJD_MEM_ISALIGNED(16, 16));
		expect_true(RJD_MEM_ISALIGNED(32, 16));
		expect_true(RJD_MEM_ISALIGNED(32, 32));

		expect_false(RJD_MEM_ISALIGNED(1, 4));
		expect_false(RJD_MEM_ISALIGNED(2, 4));
		expect_false(RJD_MEM_ISALIGNED(3, 4));

		expect_int32(0, RJD_MEM_ALIGN(0, 4));
		expect_int32(4, RJD_MEM_ALIGN(1, 4));
		expect_int32(4, RJD_MEM_ALIGN(2, 4));
		expect_int32(4, RJD_MEM_ALIGN(3, 4));
		expect_int32(4, RJD_MEM_ALIGN(4, 4));
		expect_int32(8, RJD_MEM_ALIGN(5, 4));
		expect_int32(8, RJD_MEM_ALIGN(6, 4));
		expect_int32(8, RJD_MEM_ALIGN(7, 4));
		expect_int32(8, RJD_MEM_ALIGN(8, 4));
		expect_int32(12, RJD_MEM_ALIGN(9, 4));
		expect_int32(12, RJD_MEM_ALIGN(10, 4));
		expect_int32(12, RJD_MEM_ALIGN(11, 4));
		expect_int32(12, RJD_MEM_ALIGN(12, 4));

		expect_int32(0, RJD_MEM_ALIGN(0, 8));
		expect_int32(8, RJD_MEM_ALIGN(2, 8));
		expect_int32(8, RJD_MEM_ALIGN(4, 8));
		expect_int32(8, RJD_MEM_ALIGN(6, 8));
		expect_int32(8, RJD_MEM_ALIGN(8, 8));
		expect_int32(16, RJD_MEM_ALIGN(10, 8));
		expect_int32(16, RJD_MEM_ALIGN(12, 8));
		expect_int32(16, RJD_MEM_ALIGN(14, 8));
		expect_int32(16, RJD_MEM_ALIGN(16, 8));
		expect_int32(24, RJD_MEM_ALIGN(18, 8));

		#if RJD_COMPILER_MSVC
			#pragma warning(pop)
		#endif
	}

	// default allocator
	{
		struct rjd_mem_allocator allocator = rjd_mem_allocator_init_default();
		int32_t* p0 = rjd_mem_alloc(int32_t, &allocator);
		expect_true(p0 != NULL);
		*p0 = 1337;

		char* p1 = rjd_mem_alloc_array(char, 128, &allocator);
		expect_true(p1 != NULL);
		strncpy(p1, "thequickbrownfoxjumpedoverthesuperdeduperlazydog!", 128);
		p1[127] = 0;

		char* p2 = rjd_mem_alloc_array(char, 64, &allocator);
		expect_true(p2 != NULL);
		strncpy(p2, "this fox wasn't as quick as the last one", 64);
		p2[63] = 0;

		char* p3 = rjd_mem_alloc_aligned(char, &allocator, 64);
		expect_true(p3 != NULL);
		expect_uint64(RJD_MEM_ALIGN((uint64_t)p3, 64), (uint64_t)p3);

		struct aligned_struct {
			double allocator;
			double b;
			double c;
			double e;
		};
		char* p4 = rjd_mem_alloc_array_aligned(char, 8, &allocator, 32);
		expect_true(p4 != NULL);
		expect_uint64(RJD_MEM_ALIGN((uint64_t)p4, 32), (uint64_t)p4);

		rjd_mem_free(p0);
		rjd_mem_free(p1);
		rjd_mem_free(p2);
		rjd_mem_free(p3);
		rjd_mem_free(p4);

		expect_no_leaks(&allocator);
		expect_false(rjd_mem_allocator_reset(&allocator));
	}

	// linear allocator
	{
		char stackmem[1024];
		struct rjd_mem_allocator allocator = rjd_mem_allocator_init_linear(stackmem, sizeof(stackmem));

		expect_true(rjd_mem_allocator_type(&allocator) != NULL);

		expect_true(rjd_mem_alloc_array(char, 256, &allocator) != NULL);
		expect_true(rjd_mem_alloc_array(char, 256, &allocator) != NULL);
		expect_true(rjd_mem_alloc_array(char, 256, &allocator) != NULL);

		expect_true(rjd_atomic_uint64_get(&allocator.stats.total_size) <= sizeof(stackmem));
		{
			const uint64_t total = rjd_atomic_uint64_get(&allocator.stats.total_size);
			expect_uint64(rjd_atomic_uint64_get(&allocator.stats.current.used) - rjd_atomic_uint64_get(&allocator.stats.current.overhead), 256 * 3);
			expect_uint64(rjd_atomic_uint64_get(&allocator.stats.current.peak), rjd_atomic_uint64_get(&allocator.stats.current.used));
			expect_uint64(rjd_atomic_uint64_get(&allocator.stats.current.unused), total - ((256 * 3) + rjd_atomic_uint64_get(&allocator.stats.current.overhead)));
			expect_uint64(rjd_atomic_uint64_get(&allocator.stats.current.allocs), 3);
			expect_uint64(rjd_atomic_uint64_get(&allocator.stats.current.frees), 0);

			expect_uint64(rjd_atomic_uint64_get(&allocator.stats.lifetime.peak), rjd_atomic_uint64_get(&allocator.stats.current.peak));
			expect_uint64(rjd_atomic_uint64_get(&allocator.stats.lifetime.allocs), 3);
			expect_uint64(rjd_atomic_uint64_get(&allocator.stats.lifetime.frees), 0);
			expect_uint64(rjd_atomic_uint64_get(&allocator.stats.lifetime.resets), 0);
		}

        const uint64_t old_peak = rjd_atomic_uint64_get(&allocator.stats.lifetime.peak);
		expect_true(rjd_mem_allocator_reset(&allocator));

		expect_true(rjd_mem_alloc_array(char, 512, &allocator) != NULL);
		{
			const uint64_t total = rjd_atomic_uint64_get(&allocator.stats.total_size);
			expect_uint64(rjd_atomic_uint64_get(&allocator.stats.current.used) - rjd_atomic_uint64_get(&allocator.stats.current.overhead), 512);
			expect_uint64(rjd_atomic_uint64_get(&allocator.stats.current.peak), rjd_atomic_uint64_get(&allocator.stats.current.used));
			expect_uint64(rjd_atomic_uint64_get(&allocator.stats.current.unused), total - (512 + rjd_atomic_uint64_get(&allocator.stats.current.overhead)));
			expect_uint64(rjd_atomic_uint64_get(&allocator.stats.current.allocs), 1);
			expect_uint64(rjd_atomic_uint64_get(&allocator.stats.current.frees), 0);

			expect_uint64(rjd_atomic_uint64_get(&allocator.stats.lifetime.peak), old_peak);
			expect_uint64(rjd_atomic_uint64_get(&allocator.stats.lifetime.allocs), 4);
			expect_uint64(rjd_atomic_uint64_get(&allocator.stats.lifetime.frees), 0);
			expect_uint64(rjd_atomic_uint64_get(&allocator.stats.lifetime.resets), 1);
		}

		expect_true(rjd_mem_allocator_reset(&allocator));
		expect_uint64(rjd_atomic_uint64_get(&allocator.stats.lifetime.resets), 2);
	}

	// stack allocations
	{
		const uint32_t SENTINEL = 0xFFFFFFFF;
		{
			uint32_t before = SENTINEL;
			uint32_t* single = rjd_mem_alloc_stack_noclear(uint32_t);
			uint32_t after = SENTINEL;

			*single = 0;
			expect_uint32(SENTINEL, before);
			expect_uint32(SENTINEL, after);
		}

		{
			uint32_t before = SENTINEL;
			const size_t count = 9;
			uint32_t* array = rjd_mem_alloc_stack_array_noclear(uint32_t, 9);
			uint32_t after = SENTINEL;

			for (size_t i = 0; i < count; ++i) {
				array[i] = 0;
			}
			expect_uint32(SENTINEL, before);
			expect_uint32(SENTINEL, after);
		}

	}

	// mem_swap
	{
		char test1[] = { 'm','y','t','e','s','t','\0' };
		char test2[] = { 'o','h','n','o','e','s','\0' };
		RJD_STATIC_ASSERT(sizeof(test1) == sizeof(test2));
		rjd_mem_swap(test1, test2, sizeof(test1));
		expect_str("ohnoes", test1);
		expect_str("mytest", test2);
	}
}

void test_array()
{
	struct rjd_mem_allocator allocator = rjd_mem_allocator_init_default();

	// rjd_countof
	{
		int a[3];
		expect_uint32(3, rjd_countof(a));

		unsigned long long b[64];
		expect_uint32(64, rjd_countof(b));

		struct { uint32_t a[4]; const char* b; unsigned c[20]; } c[20];
		expect_uint32(20, rjd_countof(c));
	}

	// general functionality
	{
		struct test {
			int a;
			int b;
			int c;
			int d;
		};
		struct test* a = rjd_array_alloc(struct test, 32, &allocator);
		expect_uint32(0, rjd_array_count(a));
		expect_uint32(32, rjd_array_capacity(a));
		expect_true(rjd_array_empty(a));
		expect_false(rjd_array_full(a));
		expect_uint32(0, rjd_array_count(NULL));
		expect_uint32(0, rjd_array_capacity(NULL));

		rjd_array_resize(a, 16);
		expect_uint32(16, rjd_array_count(a));
		expect_uint32(32, rjd_array_capacity(a));
		for (size_t i = 0; i < rjd_array_count(a); ++i) {
			expect_int32(0, a[i].a);
			expect_int32(0, a[i].b);
			expect_int32(0, a[i].c);
			expect_int32(0, a[i].d);
		}

		rjd_array_resize(a, 50);
		expect_uint32(50, rjd_array_count(a));
		expect_uint32(50, rjd_array_capacity(a));
		for (size_t i = 0; i < rjd_array_count(a); ++i) {
			expect_int32(0, a[i].a);
			expect_int32(0, a[i].b);
			expect_int32(0, a[i].c);
			expect_int32(0, a[i].d);
		}

		rjd_array_resize(a, 51);
		expect_uint32(51, rjd_array_count(a));
		expect_uint32(51, rjd_array_capacity(a));
		expect_int32(0, a[50].a);
		expect_int32(0, a[50].b);
		expect_int32(0, a[50].c);
		expect_int32(0, a[50].d);

		rjd_array_resize(a, 50);
		expect_uint32(50, rjd_array_count(a));
		expect_uint32(51, rjd_array_capacity(a));

		rjd_array_trim(a);
		expect_uint32(50, rjd_array_capacity(a));

		for (size_t i = 0; i < rjd_array_count(a); ++i) {
			struct test v = { (int)i, 0, 0, 0 };
			a[i] = v;
		}
		expect_false(rjd_array_empty(a));
		expect_true(rjd_array_full(a));

		for (uint32_t i = 0; i < rjd_array_count(a); ++i) {
			expect_uint32(i, rjd_array_get(a, i)->a);
		}

		rjd_array_erase(a, 0);
		expect_int32(1, a[0].a);
		rjd_array_erase(a, 1);
		expect_int32(3, a[1].a);

		expect_false(rjd_array_empty(a));
		expect_false(rjd_array_full(a));

		struct test end = rjd_array_pop(a);
		expect_int32(49, end.a);

		rjd_array_resize(a, 0);
		expect_true(rjd_array_empty(a));
		expect_uint32(0, rjd_array_count(a));
		expect_uint32(50, rjd_array_capacity(a));

		rjd_array_push(a, end);
		expect_int32(end.a, a[0].a);
		
		rjd_array_clear(a);
		expect_int32(0, rjd_array_count(a));
		expect_int32(50, rjd_array_capacity(a));

		rjd_array_free(a);
	}
    
    expect_no_leaks(&allocator);

	// clone
	{
		uint32_t* a = rjd_array_alloc(uint32_t, 128, &allocator);
		for (uint32_t i = 0; i < 128; ++i) {
			rjd_array_push(a, i * 2);
		}

		uint32_t* b = rjd_array_clone(a, &allocator);
		expect_uint32(rjd_array_count(a), rjd_array_count(b));
		for (uint32_t i = 0; i < 128; ++i) {
			expect_uint32(a[i], b[i]);
		}

		rjd_array_free(a);
		rjd_array_free(b);
	}
    
    expect_no_leaks(&allocator);

	// growing from push test
	{
		uint32_t* a = rjd_array_alloc(uint32_t, 3, &allocator);

		for (uint32_t i = 0; i < 15; ++i) {
			rjd_array_push(a, i * 2);
		}

		for (uint32_t i = 0; i < rjd_array_count(a); ++i) {
			expect_uint32(a[i], i * 2);
		}

		expect_uint32(24, rjd_array_capacity(a));

		rjd_array_free(a);
	}
    
    expect_no_leaks(&allocator);

	// reserve
	{
		uint32_t* a = rjd_array_alloc(uint32_t, 3, &allocator);
		rjd_array_reserve(a, 20);
		expect_uint32(20, rjd_array_capacity(a));

		for (uint32_t i = 0; i < 20; ++i) {
			rjd_array_push(a, i * 2);
		}
		expect_uint32(20, rjd_array_capacity(a));

		for (uint32_t i = 0; i < rjd_array_count(a); ++i) {
			expect_uint32(a[i], i * 2);
		}

		expect_uint32(20, rjd_array_capacity(a));

		rjd_array_free(a);
	}
    
    expect_no_leaks(&allocator);

	// insert
	{
		int32_t* a = rjd_array_alloc(int32_t, 20, &allocator);
		for (int32_t i = 0; i < 20; ++i) {
			rjd_array_push(a, 22);
		}

		int32_t insert_value = 77;
		rjd_array_insert(a, &insert_value, 0);
		rjd_array_insert(a, &insert_value, 5);
		rjd_array_insert(a, &insert_value, 10);
		rjd_array_insert(a, &insert_value, 15);
		rjd_array_insert(a, &insert_value, 20);
		expect_uint32(25, rjd_array_count(a));
		rjd_array_insert(a, &insert_value, 25);// this is the array count + 1
		expect_uint32(26, rjd_array_count(a));

		for (uint32_t i = 0; i < rjd_array_count(a); ++i) {
			if (i % 5 == 0) {
				expect_int32(insert_value, a[i]);
			} else {
				expect_int32(22, a[i]);
			}
		}

		rjd_array_free(a);
	}

	// first/last
	{
		int32_t* a = rjd_array_alloc(int32_t, 16, &allocator);
		for (int32_t i = 0; i < (int32_t)rjd_array_capacity(a); ++i) {
			rjd_array_push(a, 0xD00D + i);
		}

		const int32_t first = rjd_array_first(a);
		expect_int32(0xD00D + 0, first);

		const int32_t last = rjd_array_last(a);
		expect_int32(0xD00D + 15, last);

		rjd_array_free(a);
	}
    
    expect_no_leaks(&allocator);

	// find and sort tests
	{
		int32_t* shuffled = rjd_array_alloc(int32_t, 8, &allocator);
		rjd_array_push(shuffled, 5);
		rjd_array_push(shuffled, 0);
		rjd_array_push(shuffled, 3);
		rjd_array_push(shuffled, 4);
		rjd_array_push(shuffled, 7);
		rjd_array_push(shuffled, 6);
		rjd_array_push(shuffled, 2);
		rjd_array_push(shuffled, 1);

		int32_t* sorted = rjd_array_alloc(int32_t, 256, &allocator);
		for (uint32_t i = 0; i < rjd_array_count(sorted); ++i)
		{
			rjd_array_push(sorted, i);
		}

		int32_t* holes = rjd_array_alloc(int32_t, 8, &allocator);
		rjd_array_push(holes, 0);
		rjd_array_push(holes, 3);
		rjd_array_push(holes, 7);
		rjd_array_push(holes, 11);
		rjd_array_push(holes, 21);
		rjd_array_push(holes, 37);
		rjd_array_push(holes, 50);

		// find linear
		{
			int32_t needle = 0;
			expect_int32(1, rjd_array_find(shuffled, &needle));
			needle = 100;
			expect_int32(RJD_ARRAY_NOT_FOUND, rjd_array_find(shuffled, &needle));
			needle = 1;
			expect_int32(7, rjd_array_find(shuffled, &needle));
		}

		// lowerbound
		{
			int32_t needle = -1;
			expect_int32(0, rjd_array_lowerbound(holes, &needle, compare_int32));
			needle = 0;
			expect_int32(0, rjd_array_lowerbound(holes, &needle, compare_int32));
			needle = 1;
			expect_int32(1, rjd_array_lowerbound(holes, &needle, compare_int32));
			needle = 2;
			expect_int32(1, rjd_array_lowerbound(holes, &needle, compare_int32));
			needle = 3;
			expect_int32(1, rjd_array_lowerbound(holes, &needle, compare_int32));
			needle = 4;
			expect_int32(2, rjd_array_lowerbound(holes, &needle, compare_int32));
			needle = 15;
			expect_int32(4, rjd_array_lowerbound(holes, &needle, compare_int32));
			needle = 21;
			expect_int32(4, rjd_array_lowerbound(holes, &needle, compare_int32));
			needle = 50;
			expect_int32(6, rjd_array_lowerbound(holes, &needle, compare_int32));
			needle = 51;
			expect_int32(7, rjd_array_lowerbound(holes, &needle, compare_int32)); // note this is after the last element
		}

		// find sorted
		for (int32_t i = 0; i < (int32_t)rjd_array_count(sorted); ++i)
		{
			expect_int32(i, rjd_array_find_sorted(sorted, &i, compare_int32));
		}

		// contains
		{
			int32_t two = 2;
			bool has2 = rjd_array_contains(shuffled, &two);
			expect_true(has2);

			int32_t twenty = 20;
			bool has10 = rjd_array_contains(shuffled, &twenty);
			expect_false(has10);
		}

		// sort
		int32_t* shuffled2 = rjd_array_clone(shuffled, &allocator);

		rjd_array_sort(shuffled, compare_int32);
		for (uint32_t i = 0; i < rjd_array_count(shuffled); ++i) {
			expect_int32(i, shuffled[i]);
		}

		// context-versions
		{
			int32_t context = 0;
			int32_t needle = -1;
			expect_int32(0, rjd_array_lowerbound_c(holes, &needle, compare_int32_c, &context));
			needle = 0;
			expect_int32(0, rjd_array_lowerbound_c(holes, &needle, compare_int32_c, &context));
			needle = 1;
			expect_int32(1, rjd_array_lowerbound_c(holes, &needle, compare_int32_c, &context));
			needle = 2;
			expect_int32(1, rjd_array_lowerbound_c(holes, &needle, compare_int32_c, &context));
			needle = 3;
			expect_int32(1, rjd_array_lowerbound_c(holes, &needle, compare_int32_c, &context));
			needle = 4;
			expect_int32(2, rjd_array_lowerbound_c(holes, &needle, compare_int32_c, &context));
			needle = 15;
			expect_int32(4, rjd_array_lowerbound_c(holes, &needle, compare_int32_c, &context));
			needle = 21;
			expect_int32(4, rjd_array_lowerbound_c(holes, &needle, compare_int32_c, &context));
			needle = 50;
			expect_int32(6, rjd_array_lowerbound_c(holes, &needle, compare_int32_c, &context));
			needle = 51;
			expect_int32(7, rjd_array_lowerbound_c(holes, &needle, compare_int32_c, &context)); // note this is after the last element

			// find sorted
			for (int32_t i = 0; i < (int32_t)rjd_array_count(sorted); ++i)
			{
				expect_int32(i, rjd_array_find_sorted_c(sorted, &i, compare_int32_c, &context));
			}

			rjd_array_sort_c(shuffled2, compare_int32_c, &context);
			for (uint32_t i = 0; i < rjd_array_count(shuffled2); ++i) {
				expect_int32(i, shuffled2[i]);
			}
		}

		rjd_array_free(holes);
		rjd_array_free(sorted);
		rjd_array_free(shuffled);
		rjd_array_free(shuffled2);
	}
    
    expect_no_leaks(&allocator);

	// functional-style tests
	{
		int32_t* b = rjd_array_alloc(int32_t, 16, &allocator);
		for (int32_t i = 0; i < 16; ++i) {
			rjd_array_push(b, i);
		}

		// filter
		#define testfilter(element) (element < 8)
		rjd_array_filter(b, testfilter, NULL);
		expect_uint32(8, rjd_array_count(b));
		#undef testfilter

		// reduce / sum
		int32_t sum1 = 0;
		rjd_array_reduce(b, sum1, rjd_array_sum_predicate);
		expect_int32(1 + 2 + 3 + 4 + 5 + 6 + 7, sum1);

		int32_t sum2 = 0;
		rjd_array_sum(b, sum2);
		expect_int32(sum1, sum2);

		// map
		rjd_array_clear(b);
		rjd_array_push(b, 0);
		rjd_array_push(b, 1);
		rjd_array_push(b, 2);
		rjd_array_push(b, 3);
		rjd_array_push(b, 4);

		#define TEST_PRED(v) (v * 2)
		int32_t* mapped = rjd_array_alloc(int32_t, 16, &allocator);
		rjd_array_map(b, mapped, TEST_PRED);
		#undef TEST_PRED
		expect_int32(0, mapped[0]);
		expect_int32(2, mapped[1]);
		expect_int32(4, mapped[2]);
		expect_int32(6, mapped[3]);
		expect_int32(8, mapped[4]);
	
		// reverse
		rjd_array_reverse(b);
		expect_int32(4, b[0]);
		expect_int32(3, b[1]);
		expect_int32(2, b[2]);
		expect_int32(1, b[3]);
		expect_int32(0, b[4]);

		rjd_array_free(b);
        rjd_array_free(mapped);
	}

    expect_no_leaks(&allocator);
    
	// rng tests
	{
		struct rjd_rng rng = rjd_rng_init(0x1337C0DE);

		int32_t* a = rjd_array_alloc(int32_t, 8, &allocator);
		for (int32_t i = 0; i < 8; ++i) {
			rjd_array_push(a, i);
		}

		expect_int32(5, rjd_array_sample(a, &rng));

		rjd_array_shuffle(a, &rng);
		expect_int32(3, a[0]);
		expect_int32(6, a[1]);
		expect_int32(0, a[2]);
		expect_int32(7, a[3]);
		expect_int32(2, a[4]);
		expect_int32(4, a[5]);
		expect_int32(1, a[6]);
		expect_int32(5, a[7]);
        
        rjd_array_free(a);
	}

	expect_no_leaks(&allocator);
}

void expect_vec4(rjd_math_vec4 expected, rjd_math_vec4 actual) 
{
	if (!rjd_math_vec4_eq(expected, actual)) {
		RJD_ASSERTFAIL("Expected (%.2f, %.2f, %.2f, %.2f), but got: (%.2f, %.2f, %.2f, %.2f)", 
			rjd_math_vec4_x(expected), rjd_math_vec4_y(expected), rjd_math_vec4_z(expected), rjd_math_vec4_w(expected),
			rjd_math_vec4_x(actual), rjd_math_vec4_y(actual), rjd_math_vec4_z(actual), rjd_math_vec4_w(actual));
	}
}

void expect_vec3(rjd_math_vec3 expected, rjd_math_vec3 actual) 
{
	if (!rjd_math_vec3_eq(expected, actual)) {
		RJD_ASSERTFAIL("Expected (%.2f, %.2f, %.2f), but got: (%.2f, %.2f, %.2f)", 
			rjd_math_vec3_x(expected), rjd_math_vec3_y(expected), rjd_math_vec3_z(expected), 
			rjd_math_vec3_x(actual), rjd_math_vec3_y(actual), rjd_math_vec3_z(actual));
	}
}

void test_math(void)
{
	// helper functions
	expect_uint32(0, rjd_math_next_pow2(0));
	expect_uint32(1, rjd_math_next_pow2(1));
	expect_uint32(2, rjd_math_next_pow2(2));
	expect_uint32(4, rjd_math_next_pow2(3));
	expect_uint32(4, rjd_math_next_pow2(4));
	expect_uint32(8, rjd_math_next_pow2(5));
	expect_uint32(8, rjd_math_next_pow2(6));
	expect_uint32(8, rjd_math_next_pow2(7));
	expect_uint32(8, rjd_math_next_pow2(8));

	expect_int32(1,		rjd_math_pow32(2, 0));
	expect_int32(2,		rjd_math_pow32(2, 1));
	expect_int32(64,	rjd_math_pow32(2, 6));
	expect_int32(-32,	rjd_math_pow32(-2, 5));
	expect_int32(1,		rjd_math_pow32(23987, 0));
	expect_int32(23987, rjd_math_pow32(23987, 1));

	expect_int32(1,		rjd_math_sign32(0));
	expect_int32(1,		rjd_math_sign32(67));
	expect_int32(-1,	rjd_math_sign32(-231));
	expect_float(1,		rjd_math_sign(0.0));
	expect_float(1,		rjd_math_sign(-0.0));
	expect_float(1,		rjd_math_sign(234.0));
	expect_float(-1,	rjd_math_sign(-234.0));
	expect_float(1,		(double)rjd_math_signf(234.0f));
	expect_float(-1,	(double)rjd_math_signf(-234.0f));

	expect_true(rjd_math_isequal(0, 0));
	expect_true(rjd_math_isequal(505, 505));
	expect_true(rjd_math_isequal(0.00000001, 0.00000002));
	expect_true(rjd_math_isequal(0.00000001f, 0.00000002f));
	expect_false(rjd_math_isequal(1, 2));
	expect_false(rjd_math_isequal(1/3.0, 1/3.1));

	expect_float(0, rjd_math_remap(0, 0, 1, 0, 2));
	expect_float(1, rjd_math_remap(.5, 0, 1, 0, 2));
	expect_float(2, rjd_math_remap(1, 0, 1, 0, 2));
	
	expect_int32(1,		rjd_math_min32(2, 1));
	expect_int32(-1,	rjd_math_min32(-1, 1));
	expect_int32(10,	rjd_math_minu32(10, 1100));
	expect_int32(1,		rjd_math_max32(-1, 1));
	expect_int32(2,		rjd_math_max32(2, 1));
	expect_int32(1100,	rjd_math_maxu32(10, 1100));

	// vec and matrix alignment
	{
		char a1;
		rjd_math_vec3 v1 = rjd_math_vec3_xyz(1,2,3);
		char a2;
		rjd_math_vec4 v2 = rjd_math_vec4_xyzw(1,2,3,4);
		char a3, a4;
		rjd_math_mat4 m1 = rjd_math_mat4_identity();;

		expect_uint64(0, (uint64_t)(&v1) & 0xF);
		expect_uint64(0, (uint64_t)(&v2) & 0xF);
		expect_uint64(0, (uint64_t)(&m1) & 0xF);

		RJD_UNUSED_PARAM(a1);
		RJD_UNUSED_PARAM(a2);
		RJD_UNUSED_PARAM(a3);
		RJD_UNUSED_PARAM(a4);

		float* f1 = (float*)&v1;
		expect_float(1, f1[0]);
		expect_float(2, f1[1]);
		expect_float(3, f1[2]);

		f1[0] = 10;
		f1[1] = 20;
		f1[2] = 30;
		expect_float(10, f1[0]);
		expect_float(20, f1[1]);
		expect_float(30, f1[2]);

		const float* f2 = (float*)&v2;
		expect_float(1, f2[0]);
		expect_float(2, f2[1]);
		expect_float(3, f2[2]);
		expect_float(4, f2[3]);

		const float* f3 = (float*)&m1;
		expect_float(1, f3[0]); expect_float(0, f3[1]); expect_float(0, f3[2]); expect_float(0, f3[3]);
		expect_float(0, f3[4]); expect_float(1, f3[5]); expect_float(0, f3[6]); expect_float(0, f3[7]);
		expect_float(0, f3[8]); expect_float(0, f3[9]); expect_float(1, f3[10]); expect_float(0, f3[11]);
		expect_float(0, f3[12]); expect_float(0, f3[13]); expect_float(0, f3[14]); expect_float(1, f3[15]);
	}

	// vec4
	expect_true(rjd_math_vec4_eq(rjd_math_vec4_zero(), rjd_math_vec4_zero()));
	expect_true(rjd_math_vec4_eq(rjd_math_vec4_xyzw(1,2,3,4), rjd_math_vec4_xyzw(1,2,3,4)));
	expect_vec4(rjd_math_vec4_zero(), rjd_math_vec4_xyzw(0,0,0,0));
	expect_vec4(rjd_math_vec4_xyzw(1,1,0,1), rjd_math_vec4_add(rjd_math_vec4_xyzw(1,0,0,0), rjd_math_vec4_xyzw(0,1,0,1)));
	expect_float(32, rjd_math_vec4_x(rjd_math_vec4_splat(32)));
	expect_float(128, rjd_math_vec4_y(rjd_math_vec4_splat(128)));
	expect_float(777, rjd_math_vec4_z(rjd_math_vec4_splat(777)));
	expect_float(987234, rjd_math_vec4_w(rjd_math_vec4_splat(987234)));
	expect_vec4(rjd_math_vec4_xyzw(7, 7, 9, 20), rjd_math_vec4_shuffle(rjd_math_vec4_xyzw(9,0,7,20), 2, 2, 0, 3));
	expect_float(8, rjd_math_vec4_x(rjd_math_vec4_setx(rjd_math_vec4_zero(), 8)));
	expect_float(8, rjd_math_vec4_y(rjd_math_vec4_sety(rjd_math_vec4_zero(), 8)));
	expect_float(8, rjd_math_vec4_z(rjd_math_vec4_setz(rjd_math_vec4_zero(), 8)));
	expect_float(8, rjd_math_vec4_w(rjd_math_vec4_setw(rjd_math_vec4_zero(), 8)));
	expect_float(8, rjd_math_vec4_sum(rjd_math_vec4_xyzw(2, 2, 2, 2)));
	expect_float(2, rjd_math_vec4_dot(rjd_math_vec4_xyzw(0,1,0,0), rjd_math_vec4_xyzw(0,2,0,0)));
	expect_float(0, rjd_math_vec4_dot(rjd_math_vec4_xyzw(0,1,0,0), rjd_math_vec4_xyzw(1,0,0,0)));
	expect_float(16, rjd_math_vec4_lengthsq(rjd_math_vec4_xyzw(4,0,0,0)));
	expect_float(4, rjd_math_vec4_length(rjd_math_vec4_xyzw(4,0,0,0)));
	expect_float(5, rjd_math_vec4_length(rjd_math_vec4_xyzw(3,0,0,4)));
	expect_vec4(rjd_math_vec4_xyzw(1,0,0,0), rjd_math_vec4_normalize(rjd_math_vec4_xyzw(7368,0,0,0)));
	expect_vec4(rjd_math_vec4_xyzw(1,1,0,FLT_MAX), rjd_math_vec4_abs(rjd_math_vec4_xyzw(1, -1, -0, -FLT_MAX)));
	expect_vec4(rjd_math_vec4_xyzw(-1,1,-0,FLT_MAX), rjd_math_vec4_neg(rjd_math_vec4_xyzw(1, -1, 0, -FLT_MAX)));
	expect_vec4(rjd_math_vec4_xyzw(0,0,0,-1), rjd_math_vec4_floor(rjd_math_vec4_xyzw(0, .5f, .99f, -0.1f)));
	expect_vec4(rjd_math_vec4_xyzw(0,1,1,-.0), rjd_math_vec4_ceil(rjd_math_vec4_xyzw(0, .5f, .01f, -0.9f)));
	expect_vec4(rjd_math_vec4_xyzw(0,0,0,1), rjd_math_vec4_round(rjd_math_vec4_xyzw(0, .1f, .5f, .9f)));
	expect_vec4(rjd_math_vec4_xyzw(1,0,0,-1), rjd_math_vec4_round(rjd_math_vec4_xyzw(1, -.1f, -.5f, -.9f)));
	expect_vec4(rjd_math_vec4_xyzw(-1,-2,2,0), rjd_math_vec4_round(rjd_math_vec4_xyzw(-1, -1.5f, 1.5f, 0)));
	expect_vec4(rjd_math_vec4_xyzw(26, 60, 44, 6), rjd_math_vec4_scale(rjd_math_vec4_xyzw(13, 30, 22, 3), 2));
	expect_vec4(rjd_math_vec4_xyzw(3,3,3,3), rjd_math_vec4_add(rjd_math_vec4_xyzw(1,1,1,1), rjd_math_vec4_xyzw(2,2,2,2)));
	expect_vec4(rjd_math_vec4_xyzw(-1,-1,-1,-1), rjd_math_vec4_sub(rjd_math_vec4_xyzw(1,1,1,1), rjd_math_vec4_xyzw(2,2,2,2)));
	expect_vec4(rjd_math_vec4_xyzw(2,2,2,2), rjd_math_vec4_mul(rjd_math_vec4_xyzw(1,1,1,1), rjd_math_vec4_xyzw(2,2,2,2)));
	expect_vec4(rjd_math_vec4_xyzw(.5,.5,.5,.5), rjd_math_vec4_div(rjd_math_vec4_xyzw(1,1,1,1), rjd_math_vec4_xyzw(2,2,2,2)));
	expect_vec4(rjd_math_vec4_xyzw(23,45,21,5), rjd_math_vec4_min(rjd_math_vec4_xyzw(23,45,72,5), rjd_math_vec4_xyzw(43,75,21,6)));
	expect_vec4(rjd_math_vec4_xyzw(43,75,72,6), rjd_math_vec4_max(rjd_math_vec4_xyzw(23,45,72,5), rjd_math_vec4_xyzw(43,75,21,6)));
	expect_vec4(rjd_math_vec4_xyzw(1,0,0,0), rjd_math_vec4_project(rjd_math_vec4_xyzw(1,1,1,1), rjd_math_vec4_xyzw(1,0,0,0)));
	expect_vec4(rjd_math_vec4_xyzw(1,2,4,8), rjd_math_vec4_lerp(rjd_math_vec4_zero(), rjd_math_vec4_xyzw(2, 4, 8, 16), .5f));
	expect_vec4(rjd_math_vec4_xyzw(1,2,4,8), rjd_math_vec4_lerp(rjd_math_vec4_zero(), rjd_math_vec4_xyzw(2, 4, 8, 16), .5f));

	// vec3
	expect_true(rjd_math_vec3_eq(rjd_math_vec3_left(), rjd_math_vec3_left()));
	expect_vec3(rjd_math_vec3_zero(), rjd_math_vec3_xyz(0,0,0));
	expect_vec3(rjd_math_vec3_up(), rjd_math_vec3_xyz(0,1,0));
	expect_vec3(rjd_math_vec3_back(), rjd_math_vec3_xyz(0,0,-1));
	expect_vec3(rjd_math_vec3_xyz(1, 1, 0), rjd_math_vec3_add(rjd_math_vec3_up(), rjd_math_vec3_right()));
	expect_float(32, rjd_math_vec3_x(rjd_math_vec3_splat(32)));
	expect_float(128, rjd_math_vec3_y(rjd_math_vec3_splat(128)));
	expect_float(777, rjd_math_vec3_z(rjd_math_vec3_splat(777)));
	expect_vec3(rjd_math_vec3_xyz(7, 7, 9), rjd_math_vec3_shuffle(rjd_math_vec3_xyz(9,0,7), 2, 2, 0));
	expect_float(8, rjd_math_vec3_x(rjd_math_vec3_setx(rjd_math_vec3_zero(), 8)));
	expect_float(8, rjd_math_vec3_y(rjd_math_vec3_sety(rjd_math_vec3_zero(), 8)));
	expect_float(8, rjd_math_vec3_z(rjd_math_vec3_setz(rjd_math_vec3_zero(), 8)));
	expect_float(6, rjd_math_vec3_sum(rjd_math_vec3_xyz(2, 2, 2)));
	expect_float(2, rjd_math_vec3_dot(rjd_math_vec3_up(), rjd_math_vec3_xyz(0,2,0)));
	expect_float(0, rjd_math_vec3_dot(rjd_math_vec3_up(), rjd_math_vec3_xyz(1,0,0)));
	expect_float(16, rjd_math_vec3_lengthsq(rjd_math_vec3_xyz(4,0,0)));
	expect_float(4, rjd_math_vec3_length(rjd_math_vec3_xyz(4,0,0)));
	expect_float(5, rjd_math_vec3_length(rjd_math_vec3_xyz(3,4,0)));
	expect_vec3(rjd_math_vec3_xyz(1,0,0), rjd_math_vec3_normalize(rjd_math_vec3_xyz(7368, 0, 0)));
	expect_float(RJD_MATH_PI/4, rjd_math_vec3_angle(rjd_math_vec3_up(), rjd_math_vec3_xyz(1,1,0)));
	expect_vec3(rjd_math_vec3_xyz(26, 60, 44), rjd_math_vec3_scale(rjd_math_vec3_xyz(13, 30, 22), 2));
	expect_vec3(rjd_math_vec3_xyz(3,3,3), rjd_math_vec3_add(rjd_math_vec3_xyz(1,1,1), rjd_math_vec3_xyz(2,2,2)));
	expect_vec3(rjd_math_vec3_xyz(-1,-1,-1), rjd_math_vec3_sub(rjd_math_vec3_xyz(1,1,1), rjd_math_vec3_xyz(2,2,2)));
	expect_vec3(rjd_math_vec3_xyz(2,2,2), rjd_math_vec3_mul(rjd_math_vec3_xyz(1,1,1), rjd_math_vec3_xyz(2,2,2)));
	expect_vec3(rjd_math_vec3_xyz(.5,.5,.5), rjd_math_vec3_div(rjd_math_vec3_xyz(1,1,1), rjd_math_vec3_xyz(2,2,2)));
	expect_vec3(rjd_math_vec3_left(), rjd_math_vec3_cross(rjd_math_vec3_forward(), rjd_math_vec3_up()));
	expect_vec3(rjd_math_vec3_right(), rjd_math_vec3_cross(rjd_math_vec3_up(), rjd_math_vec3_forward()));
	expect_vec3(rjd_math_vec3_xyz(23, 45, 21), rjd_math_vec3_min(rjd_math_vec3_xyz(23,45,72), rjd_math_vec3_xyz(43, 75, 21)));
	expect_vec3(rjd_math_vec3_xyz(43, 75, 72), rjd_math_vec3_max(rjd_math_vec3_xyz(23,45,72), rjd_math_vec3_xyz(43, 75, 21)));
	expect_vec3(rjd_math_vec3_xyz(1,0,0), rjd_math_vec3_project(rjd_math_vec3_xyz(1,1,1), rjd_math_vec3_xyz(1,0,0)));
	expect_vec3(rjd_math_vec3_xyz(1,1,0), rjd_math_vec3_reflect(rjd_math_vec3_xyz(1,-1,0), rjd_math_vec3_up()));
	expect_vec3(rjd_math_vec3_xyz(1,2,4), rjd_math_vec3_lerp(rjd_math_vec3_zero(), rjd_math_vec3_xyz(2, 4, 8), .5));

	// matrix
	// TODO
}

void test_procgeo()
{
	struct rjd_mem_allocator allocator = rjd_mem_allocator_init_default();

	for (enum rjd_procgeo_type type = 0; type < RJD_PROCGEO_TYPE_COUNT; ++type)
	{
		for (uint32_t tesselation = 0; tesselation < 32; ++tesselation)
		{
			uint32_t verts = rjd_procgeo_calc_num_verts(type, tesselation);
			float* data = rjd_mem_alloc_array(float, verts * 3, &allocator);
			float* data_strided = rjd_mem_alloc_array(float, verts * 5, &allocator);
			
			float* data_end = rjd_procgeo(type, tesselation, 1.0f, 1.0f, 1.0f, data, verts * 3, 0);
			float* data_strided_end = rjd_procgeo(type, tesselation, 1.0f, 1.0f, 1.0f, data_strided, verts * 5, 2);

			RJD_ASSERT(data_end != NULL);
			RJD_ASSERT(data_strided_end != NULL);

			for (uint32_t vert = 0; vert < verts; ++vert)
			{
				expect_float(data[vert * 3 + 0], data_strided[vert * 5 + 0]);
                expect_float(data[vert * 3 + 1], data_strided[vert * 5 + 1]);
                expect_float(data[vert * 3 + 2], data_strided[vert * 5 + 2]);
			}

			rjd_mem_free(data);
			rjd_mem_free(data_strided);
		}

		expect_no_leaks(&allocator);
	}
}

void test_geo()
{
	// point-rect
	{
		rjd_math_vec3 p1 = rjd_math_vec3_xyz(0,0,0);
		rjd_math_vec3 p2 = rjd_math_vec3_xyz(10,1,0);
		rjd_math_vec3 p3 = rjd_math_vec3_xyz(2,2,10);

		rjd_geo_rect r1 = rjd_geo_rect_minmax(-1,-1,4,4);
		rjd_geo_rect r2 = rjd_geo_rect_minmax(0,0,20,1);
		rjd_geo_rect r3 = rjd_geo_rect_minmax(1,1,2,2);

		expect_true(rjd_geo_point_rect(p1, r1));
		expect_true(rjd_geo_point_rect(p1, r2));
		expect_false(rjd_geo_point_rect(p1, r3));

		expect_false(rjd_geo_point_rect(p2, r1));
		expect_true(rjd_geo_point_rect(p2, r2));
		expect_false(rjd_geo_point_rect(p2, r3));

		expect_true(rjd_geo_point_rect(p3, r1));
		expect_false(rjd_geo_point_rect(p3, r2));
		expect_true(rjd_geo_point_rect(p3, r3));
	}

	// point-box
	{
		rjd_math_vec3 p1 = rjd_math_vec3_xyz(0,0,0);
		rjd_math_vec3 p2 = rjd_math_vec3_xyz(-1,1,0);
		rjd_math_vec3 p3 = rjd_math_vec3_xyz(0,1,-1);
		rjd_math_vec3 p4 = rjd_math_vec3_xyz(1,0,1);

		rjd_geo_box b1 = rjd_geo_box_minmax(rjd_math_vec3_xyz(-1,-1,-1), rjd_math_vec3_xyz(1,1,1));
		rjd_geo_box b2 = rjd_geo_box_minmax(rjd_math_vec3_xyz(0,0,0), rjd_math_vec3_xyz(1,1,1));
		rjd_geo_box b3 = rjd_geo_box_minmax(rjd_math_vec3_xyz(-1,-1,-1), rjd_math_vec3_xyz(0,0,0));
		rjd_geo_box b4 = rjd_geo_box_minmax(rjd_math_vec3_xyz(-1,0,-1), rjd_math_vec3_xyz(1,1,0));

		expect_true(rjd_geo_point_box(p1, b1));
		expect_true(rjd_geo_point_box(p1, b2));
		expect_true(rjd_geo_point_box(p1, b3));
		expect_true(rjd_geo_point_box(p1, b4));

		expect_true(rjd_geo_point_box(p2, b1));
		expect_false(rjd_geo_point_box(p2, b2));
		expect_false(rjd_geo_point_box(p2, b3));
		expect_true(rjd_geo_point_box(p2, b4));

		expect_true(rjd_geo_point_box(p3, b1));
		expect_false(rjd_geo_point_box(p3, b2));
		expect_false(rjd_geo_point_box(p3, b3));
		expect_true(rjd_geo_point_box(p3, b4));

		expect_true(rjd_geo_point_box(p4, b1));
		expect_true(rjd_geo_point_box(p4, b2));
		expect_false(rjd_geo_point_box(p4, b3));
		expect_false(rjd_geo_point_box(p4, b4));
	}

	// point-circle
	{
		rjd_math_vec3 p1 = rjd_math_vec3_xyz(0,0,0);
		rjd_math_vec3 p2 = rjd_math_vec3_xyz(5,1,0);
		rjd_math_vec3 p3 = rjd_math_vec3_xyz(-3,2,0);
		rjd_math_vec3 p4 = rjd_math_vec3_xyz(-2,-2,0);

		rjd_geo_circle c1 = rjd_geo_circle_xyr(0,0,1);
		rjd_geo_circle c2 = rjd_geo_circle_xyr(4,0,4);
		rjd_geo_circle c3 = rjd_geo_circle_xyr(-1,-1,2);

		expect_true(rjd_geo_point_circle(p1, c1));
		expect_true(rjd_geo_point_circle(p1, c2));
		expect_true(rjd_geo_point_circle(p1, c3));

		expect_false(rjd_geo_point_circle(p2, c1));
		expect_true(rjd_geo_point_circle(p2, c2));
		expect_false(rjd_geo_point_circle(p2, c3));

		expect_false(rjd_geo_point_circle(p3, c1));
		expect_false(rjd_geo_point_circle(p3, c2));
		expect_false(rjd_geo_point_circle(p3, c3));

		expect_false(rjd_geo_point_circle(p4, c1));
		expect_false(rjd_geo_point_circle(p4, c2));
		expect_true(rjd_geo_point_circle(p4, c3));
	}

	// point-sphere
	{
		rjd_math_vec3 p1 = rjd_math_vec3_xyz(0,0,0);
		rjd_math_vec3 p2 = rjd_math_vec3_xyz(1,2,3);
		rjd_math_vec3 p3 = rjd_math_vec3_xyz(-2,2,7);
		rjd_math_vec3 p4 = rjd_math_vec3_xyz(1,-2,-3);

		rjd_geo_sphere s1 = rjd_geo_sphere_xyzr(0,0,0,4);
		rjd_geo_sphere s2 = rjd_geo_sphere_xyzr(2,2,4,5);
		rjd_geo_sphere s3 = rjd_geo_sphere_xyzr(-1,-4,0,3);

		expect_true(rjd_geo_point_sphere(p1, s1));
		expect_true(rjd_geo_point_sphere(p1, s2));
		expect_false(rjd_geo_point_sphere(p1, s3));

		expect_true(rjd_geo_point_sphere(p2, s1));
		expect_true(rjd_geo_point_sphere(p2, s2));
		expect_false(rjd_geo_point_sphere(p2, s3));

		expect_false(rjd_geo_point_sphere(p3, s1));
		expect_true(rjd_geo_point_sphere(p3, s2));
		expect_false(rjd_geo_point_sphere(p3, s3));

		expect_true(rjd_geo_point_sphere(p4, s1));
		expect_false(rjd_geo_point_sphere(p4, s2));
		expect_false(rjd_geo_point_sphere(p4, s3));
	}

	// circle-circle
	{
		rjd_geo_circle c1 = rjd_geo_circle_xyr(0,0,1);
		rjd_geo_circle c2 = rjd_geo_circle_xyr(-2,-1,2);
		rjd_geo_circle c3 = rjd_geo_circle_xyr(3,0,5);
		rjd_geo_circle c4 = rjd_geo_circle_xyr(-4,-1,1);

		expect_true(rjd_geo_circle_circle(c1, c1));
		expect_true(rjd_geo_circle_circle(c1, c2));
		expect_true(rjd_geo_circle_circle(c1, c3));
		expect_false(rjd_geo_circle_circle(c1, c4));

		expect_true(rjd_geo_circle_circle(c2, c1));
		expect_true(rjd_geo_circle_circle(c2, c2));
		expect_true(rjd_geo_circle_circle(c2, c3));
		expect_true(rjd_geo_circle_circle(c2, c4));

		expect_true(rjd_geo_circle_circle(c3, c1));
		expect_true(rjd_geo_circle_circle(c3, c2));
		expect_true(rjd_geo_circle_circle(c3, c3));
		expect_false(rjd_geo_circle_circle(c3, c4));

		expect_false(rjd_geo_circle_circle(c4, c1));
		expect_true(rjd_geo_circle_circle(c4, c2));
		expect_false(rjd_geo_circle_circle(c4, c3));
		expect_true(rjd_geo_circle_circle(c4, c4));
	}

	// circle-rect
	{
		rjd_geo_circle c1 = rjd_geo_circle_xyr(0,0,1);
		rjd_geo_circle c2 = rjd_geo_circle_xyr(2,1,.5);
		rjd_geo_circle c3 = rjd_geo_circle_xyr(3,-3,2);

		rjd_geo_rect r1 = rjd_geo_rect_minmax(-1,-1,1,1);
		rjd_geo_rect r2 = rjd_geo_rect_minmax(-2,-2,2,2);
		rjd_geo_rect r3 = rjd_geo_rect_minmax(0,1,2,3);
		rjd_geo_rect r4 = rjd_geo_rect_minmax(-2, 1,-1,2);

		expect_true(rjd_geo_circle_rect(c1, r1));
		expect_true(rjd_geo_circle_rect(c1, r2));
		expect_false(rjd_geo_circle_rect(c1, r3));
		expect_false(rjd_geo_circle_rect(c1, r4));

		expect_false(rjd_geo_circle_rect(c2, r1));
		expect_true(rjd_geo_circle_rect(c2, r2));
		expect_true(rjd_geo_circle_rect(c2, r3));
		expect_false(rjd_geo_circle_rect(c2, r4));

		expect_false(rjd_geo_circle_rect(c3, r1));
		expect_true(rjd_geo_circle_rect(c3, r2));
		expect_false(rjd_geo_circle_rect(c3, r3));
		expect_false(rjd_geo_circle_rect(c3, r4));
	}

	// rect-rect
	{
		rjd_geo_rect r1 = rjd_geo_rect_minmax(-3,-1,3,1);
		rjd_geo_rect r2 = rjd_geo_rect_minmax(1,-3,2,2);
		rjd_geo_rect r3 = rjd_geo_rect_minmax(-1,-4,1,-2);
		rjd_geo_rect r4 = rjd_geo_rect_minmax(.5,-2.5,1.5,-.5);
		rjd_geo_rect r5 = rjd_geo_rect_minmax(-2,0,-1,2);

		expect_true(rjd_geo_rect_rect(r1, r1));
		expect_true(rjd_geo_rect_rect(r1, r2));
		expect_false(rjd_geo_rect_rect(r1, r3));
		expect_true(rjd_geo_rect_rect(r1, r4));
		expect_true(rjd_geo_rect_rect(r1, r5));

		expect_true(rjd_geo_rect_rect(r2, r1));
		expect_true(rjd_geo_rect_rect(r2, r2));
		expect_false(rjd_geo_rect_rect(r2, r3));
		expect_true(rjd_geo_rect_rect(r2, r4));
		expect_false(rjd_geo_rect_rect(r2, r5));

		expect_false(rjd_geo_rect_rect(r3, r1));
		expect_false(rjd_geo_rect_rect(r3, r2));
		expect_true(rjd_geo_rect_rect(r3, r3));
		expect_true(rjd_geo_rect_rect(r3, r4));
		expect_false(rjd_geo_rect_rect(r3, r5));

		expect_true(rjd_geo_rect_rect(r4, r1));
		expect_true(rjd_geo_rect_rect(r4, r2));
		expect_true(rjd_geo_rect_rect(r4, r3));
		expect_true(rjd_geo_rect_rect(r4, r4));
		expect_false(rjd_geo_rect_rect(r4, r5));

		expect_true(rjd_geo_rect_rect(r5, r1));
		expect_false(rjd_geo_rect_rect(r5, r2));
		expect_false(rjd_geo_rect_rect(r5, r3));
		expect_false(rjd_geo_rect_rect(r5, r4));
		expect_true(rjd_geo_rect_rect(r5, r5));	
	}

	// sphere-sphere
	{
		// rjd_geo_sphere_sphere
	}

	// sphere-box
	{
		// rjd_geo_sphere_box
	}

	// box-box
	{
		// rjd_geo_box_box
	}

	// ray-point
	{
		// rjd_geo_ray_point
	}

	// ray-sphere
	{
		// rjd_geo_ray_sphere
	}

	// ray-box
	{
		// rjd_geo_ray_box
		// rjd_geo_ray_boxfast
	}
}

void expect_ease(rjd_ease_func f, float f1, float f2, float f3)
{
	expect_float(f1, f(0.25f));
	expect_float(f2, f(0.5f));
	expect_float(f3, f(0.75f));
}

void test_easing()
{
	expect_ease(rjd_ease_line,        0.250000f,  0.500000f,  0.750000f);
	expect_ease(rjd_ease_in_sine,     0.076120f,  0.292893f,  0.617317f);
	expect_ease(rjd_ease_in_quad,     0.062500f,  0.250000f,  0.562500f);
	expect_ease(rjd_ease_in_cube,     0.015625f,  0.125000f,  0.421875f);
	expect_ease(rjd_ease_in_quar,     0.003906f,  0.062500f,  0.316406f);
	expect_ease(rjd_ease_in_quin,     0.000977f,  0.031250f,  0.237305f);
	expect_ease(rjd_ease_in_expo,     0.005524f,  0.031250f,  0.176777f);
	expect_ease(rjd_ease_in_circ,     0.031754f,  0.133975f,  0.338562f);
	expect_ease(rjd_ease_in_back,    -0.161152f, -0.375000f, -0.108455f);
	expect_ease(rjd_ease_in_elas,    -0.005104f, -0.022097f,  0.067650f);
	expect_ease(rjd_ease_in_boun,     0.041136f,  0.281250f,  0.527344f);
	expect_ease(rjd_ease_out_sine,    0.382683f,  0.707107f,  0.923880f);
	expect_ease(rjd_ease_out_quad,    0.437500f,  0.750000f,  0.937500f);
	expect_ease(rjd_ease_out_cube,    0.578125f,  0.875000f,  0.984375f);
	expect_ease(rjd_ease_out_quar,    0.683594f,  0.937500f,  0.996094f);
	expect_ease(rjd_ease_out_quin,    0.762695f,  0.968750f,  0.999023f);
	expect_ease(rjd_ease_out_expo,    0.823223f,  0.968750f,  0.994476f);
	expect_ease(rjd_ease_out_circ,    0.661438f,  0.866025f,  0.968246f);
	expect_ease(rjd_ease_out_back,    1.108455f,  1.375000f,  1.161152f);
	expect_ease(rjd_ease_out_elas,    0.932351f,  1.022097f,  1.005104f);
	expect_ease(rjd_ease_out_boun,    0.472656f,  0.718750f,  0.958864f);
	expect_ease(rjd_ease_inout_sine,  0.146447f,  0.500000f,  0.853553f);
	expect_ease(rjd_ease_inout_quad,  0.125000f,  0.500000f,  0.875000f);
	expect_ease(rjd_ease_inout_cube,  0.062500f,  0.500000f,  0.937500f);
	expect_ease(rjd_ease_inout_quar,  0.031250f,  0.500000f,  0.968750f);
	expect_ease(rjd_ease_inout_quin,  0.015625f,  0.500000f,  0.984375f);
	expect_ease(rjd_ease_inout_expo,  0.015625f,  0.500000f,  0.984375f);
	expect_ease(rjd_ease_inout_circ,  0.066987f,  0.500000f,  0.933013f);
	expect_ease(rjd_ease_inout_back, -0.187500f,  0.500000f,  1.187500f);
	expect_ease(rjd_ease_inout_elas, -0.011049f,  0.500000f,  1.011049f);
	expect_ease(rjd_ease_inout_boun,  0.140625f,  0.500000f,  0.859375f);
}

void test_strbuf(void)
{	
	struct rjd_mem_allocator allocator = rjd_mem_allocator_init_default();

	struct rjd_strbuf builder = rjd_strbuf_init(&allocator);

	// regular append
	rjd_strbuf_append(&builder, "test");
	expect_str("test", rjd_strbuf_str(&builder));

	rjd_strbuf_append(&builder, "123");
	rjd_strbuf_append(&builder, "123");
	expect_str("test123123", rjd_strbuf_str(&builder));

	rjd_strbuf_free(&builder);

	// formatting
	rjd_strbuf_append(&builder, "forma%d%ded", 1, 1);
	expect_str("forma11ed", rjd_strbuf_str(&builder));

	rjd_strbuf_append(&builder, "1234567890123456789012345678901234567890");

	expect_str("forma11ed1234567890123456789012345678901234567890", rjd_strbuf_str(&builder));

	rjd_strbuf_free(&builder);

	// stack to heap switch
	rjd_strbuf_append(&builder, "123456789012345678901234567890");
	rjd_strbuf_append(&builder, "1");
	rjd_strbuf_append(&builder, "2");
	rjd_strbuf_append(&builder, "3");
	expect_str("123456789012345678901234567890123", rjd_strbuf_str(&builder));

	rjd_strbuf_free(&builder);

	// append substring
	rjd_strbuf_appendl(&builder, "only see this, no comma", (uint32_t)strlen("only see this"));
	expect_str("only see this", rjd_strbuf_str(&builder));

	expect_no_leaks(&allocator);
}

void test_profiler(void)
{
	test_logging_redirect_to_logbuffer();

	{
		RJD_TIMER_SCOPE_BEGIN(test1);

		double d = 1;
		for (size_t i = 0; i < 1000; ++i) {
			d += d;
		}
		expect_true(d > 0);

		RJD_TIMER_SCOPE_END(test1);
	}

	expect_true(strstr(g_logbuffer, "Elapsed test1") != NULL);

	test_logging_reset();
}

void test_cmd()
{
	test_logging_redirect_to_logbuffer();

	struct rjd_mem_allocator allocator = rjd_mem_allocator_init_default();
	
	const char* argv0[] = { "test.exe", NULL };
	struct rjd_cmd cmd = rjd_cmd_init(rjd_countof(argv0), argv0, &allocator);

	expect_true(rjd_cmd_ok(&cmd));

	rjd_cmd_add_req(&cmd, "PATTERN", "The regex pattern to search for.");
	rjd_cmd_add_req(&cmd, "FILE", "File in which to search for the pattern.");
	rjd_cmd_add_opt(&cmd, "-c", "--color", NULL, "Enable colorized output");
	rjd_cmd_add_opt(&cmd, "-w", "--wordsonly", NULL, "Force matching of whole words only.");
	rjd_cmd_add_opt(&cmd, "-z", "--zfile", "ZFILE", "Metafile.");

	expect_false(rjd_cmd_ok(&cmd));

	const char* argv1[] = { "a.exe", "mypattern", "file.txt", NULL };
	cmd.argc = rjd_countof(argv1);
	cmd.argv = argv1;
	expect_true(rjd_cmd_ok(&cmd));
	expect_str("mypattern", rjd_cmd_str(&cmd, "PATTERN"));
	expect_str("file.txt", rjd_cmd_str(&cmd, "FILE"));

	const char* argv2[] = { "a.exe", "-c", "-w", "mypattern", "file.txt", NULL };
	cmd.argc = rjd_countof(argv2);
	cmd.argv = argv2;
	expect_true(rjd_cmd_ok(&cmd));
	expect_true(rjd_cmd_bool(&cmd, "-c"));
	expect_true(rjd_cmd_bool(&cmd, "-w"));
	expect_false(rjd_cmd_bool(&cmd, "-z"));
	expect_str("mypattern", rjd_cmd_str(&cmd, "PATTERN"));
	expect_str("file.txt", rjd_cmd_str(&cmd, "FILE"));

	const char* argv3[] = { "a.exe", "-z", "meta.txt", "-w", "--color", "mypattern", "file.txt", NULL };
	cmd.argc = rjd_countof(argv3);
	cmd.argv = argv3;
	expect_true(rjd_cmd_ok(&cmd));
	expect_true(rjd_cmd_bool(&cmd, "-z"));
	expect_true(rjd_cmd_bool(&cmd, "-w"));
	expect_true(rjd_cmd_bool(&cmd, "-c"));
	expect_str("meta.txt", rjd_cmd_str(&cmd, "-z"));
	expect_str("mypattern", rjd_cmd_str(&cmd, "PATTERN"));
	expect_str("file.txt", rjd_cmd_str(&cmd, "FILE"));

	const char* argv4[] = { "a.exe", "-z", "1337", "mypattern", "file.txt", NULL };
	cmd.argc = rjd_countof(argv4);
	cmd.argv = argv4;
	expect_true(rjd_cmd_ok(&cmd));
	expect_int32(1337, rjd_cmd_int(&cmd, "-z", 0));
	expect_float(1337, rjd_cmd_int(&cmd, "-z", 0));
	expect_str("mypattern", rjd_cmd_str(&cmd, "PATTERN"));
	expect_str("file.txt", rjd_cmd_str(&cmd, "FILE"));

	rjd_cmd_usage(&cmd);
	expect_true(strstr(g_logbuffer, "Usage: a.exe [-h -c -w -z] PATTERN FILE") != NULL);
	g_logbuffer_pos = 0;

	rjd_cmd_help(&cmd);
	expect_true(strstr(g_logbuffer, "Usage: a.exe [-h -c -w -z] PATTERN FILE") != NULL);
	expect_true(strstr(g_logbuffer, "The regex pattern to search for.") != NULL);

	rjd_cmd_free(&cmd);

	expect_no_leaks(&allocator);

	test_logging_reset();
}

void test_rng()
{
	struct rjd_rng rng = rjd_rng_init(0xF7A290B49);
	expect_int64(11675382715271363104ull, rjd_rng_next(&rng));
	expect_int64(15127709716525989220ull, rjd_rng_next(&rng));
	expect_int64(6059634119111723720ull, rjd_rng_next(&rng));
	expect_int32(41, rjd_rng_range32(&rng, 0, 100));
	expect_int32(67, rjd_rng_range32(&rng, 0, 100));
	expect_int32(87, rjd_rng_range32(&rng, 0, 100));
	expect_int32(80, rjd_rng_range32(&rng, 0, 100));
	expect_float(0.619404, rjd_rng_float(&rng));
	expect_float(0.349893, rjd_rng_float(&rng));
	expect_float(0.877108, rjd_rng_float(&rng));
	expect_float(0.933046, rjd_rng_float(&rng));

	rng = rjd_rng_init(0xF7A290B49);
	expect_int64(11675382715271363104ull, rjd_rng_next(&rng));
	expect_int64(15127709716525989220ull, rjd_rng_next(&rng));
}

void test_dict()
{
	struct rjd_mem_allocator allocator = rjd_mem_allocator_init_default();
	
	struct rjd_dict dict = rjd_dict_init(&allocator, 0);
	expect_str(NULL, (char*)rjd_dict_get_hashstr(&dict, "key"));
	expect_str(NULL, (char*)rjd_dict_erase_hashstr(&dict, "key"));

	rjd_dict_insert_hashstr(&dict, "key1", "ok1");
	rjd_dict_insert_hashstr(&dict, "key2", "ok2");
	rjd_dict_insert_hashstr(&dict, "key3", "ok3");
	rjd_dict_insert_hashstr(&dict, "key4", "ok4");
	rjd_dict_insert_hashstr(&dict, "key5", NULL);

	expect_str("ok1", (const char*)rjd_dict_get_hashstr(&dict, "key1"));
	expect_str("ok2", (const char*)rjd_dict_get_hashstr(&dict, "key2"));
	expect_str("ok3", (const char*)rjd_dict_get_hashstr(&dict, "key3"));
	expect_str("ok4", (const char*)rjd_dict_get_hashstr(&dict, "key4"));
	expect_str(NULL,  (const char*)rjd_dict_get_hashstr(&dict, "key5"));

	expect_true(rjd_dict_has_hashstr(&dict, "key1"));
	expect_true(rjd_dict_has_hashstr(&dict, "key2"));
	expect_true(rjd_dict_has_hashstr(&dict, "key3"));
	expect_true(rjd_dict_has_hashstr(&dict, "key4"));
	expect_true(rjd_dict_has_hashstr(&dict, "key5"));
	expect_false(rjd_dict_has_hashstr(&dict, "does_not_exist"));

	expect_str("ok2", (const char*)rjd_dict_erase_hashstr(&dict, "key2"));
	expect_str(NULL,  (const char*)rjd_dict_get_hashstr(&dict, "key2"));

	int32_t data[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};

	for (size_t i = 0; i < rjd_countof(data); ++i) {
		char key[32] = {0};
		sprintf(key, "k%zu", i);
		rjd_dict_insert_hashstr(&dict, key, data + i);
	}

	for (int32_t i = 0; i < (int32_t)rjd_countof(data); ++i) {
		char key[32] = {0};
		sprintf(key, "k%d", i);
		expect_true(rjd_dict_has_hashstr(&dict, key));
		expect_int32(i, *(int32_t*)rjd_dict_get_hashstr(&dict, key));
	}

	rjd_dict_free(&dict);

	expect_no_leaks(&allocator);
}

void expect_fio_ok(bool expected_ok, struct rjd_result actual)
{
	if (expected_ok != rjd_result_isok(actual)) {
		const char* expected_str = expected_ok ? "OK" : "not ok";
		const char* actual_str = actual.error ? actual.error : "OK";
		RJD_ASSERTFAIL("fio: expected %s, but got %s\n", expected_str, actual_str);
	}
}

void test_fio()
{
	struct rjd_mem_allocator allocator = rjd_mem_allocator_init_default();

	const char expected_contents[] = "this is a test file that has ä utf-8 character!";

	struct rjd_result result;

	result = rjd_fio_write("test.txt", expected_contents, sizeof(expected_contents), RJD_FIO_WRITEMODE_REPLACE);
	expect_fio_ok(true, result);

	char* buffer;
    result = rjd_fio_read("does_not_exist.txt", &buffer, &allocator);
    expect_fio_ok(false, result);
    result = rjd_fio_read("test.txt", &buffer, &allocator);
    expect_fio_ok(true, result);
    expect_uint32(sizeof(expected_contents), rjd_array_count(buffer));
    expect_true(!memcmp(buffer, expected_contents, sizeof(expected_contents)));
    rjd_array_free(buffer);

	result = rjd_fio_write("test2.txt", expected_contents, sizeof(expected_contents), RJD_FIO_WRITEMODE_REPLACE);
	expect_fio_ok(true, result);
	result = rjd_fio_write("test2.txt", expected_contents, sizeof(expected_contents), RJD_FIO_WRITEMODE_APPEND);
	char* buffer2;
	result = rjd_fio_read("test2.txt", &buffer2, &allocator);
	expect_fio_ok(true, result);
	expect_uint32(sizeof(expected_contents)*2, rjd_array_count(buffer2));
    rjd_array_free(buffer2);

	size_t filesize;
	result = rjd_fio_size("does_not_exist.txt", &filesize);
	expect_fio_ok(false, result);
	result = rjd_fio_size("test.txt", &filesize);
	expect_fio_ok(true, result);
	expect_uint32(sizeof(expected_contents), (uint32_t)filesize);

	result = rjd_fio_delete("does_not_exist.txt");
	expect_fio_ok(false, result);
	result = rjd_fio_delete("test.txt");
	expect_fio_ok(true, result);
	result = rjd_fio_delete("test2.txt");
	expect_fio_ok(true, result);

    rjd_fio_delete("test_folder");
	result = rjd_fio_mkdir("test_folder");
	expect_result_ok(result);
    expect_true(rjd_fio_exists("test_folder"));
	result = rjd_fio_mkdir("test_folder");
	expect_result_notok(result);
	result = rjd_fio_mkdir("test_folder/nested/deepnest");
	expect_result_ok(result);
    expect_true(rjd_fio_exists("test_folder/nested"));
    expect_true(rjd_fio_exists("test_folder/nested/deepnest"));
    result = rjd_fio_mkdir("test_folder/nested/deepnest");
    expect_result_notok(result);
    result = rjd_fio_delete("test_folder");
    expect_result_ok(result);
    expect_false(rjd_fio_exists("test_folder"));

	expect_no_leaks(&allocator);
}

enum test_thread_stage
{
	TEST_THREAD_STAGE_PENDING,
	TEST_THREAD_STAGE_STARTED,
	TEST_THREAD_STAGE_ACQUIRE_LOCK,
    TEST_THREAD_STAGE_ACQUIRE_READ_LOCK,
    TEST_THREAD_STAGE_ACQUIRE_WRITE_LOCK,
	TEST_THREAD_STAGE_FINISHED,
};

struct test_thread_data
{
	struct rjd_condvar goto_next_main;
	struct rjd_condvar goto_next_thread;
	struct rjd_lock lock;
	struct rjd_rwlock rwlock;
	enum test_thread_stage stage;
};

void step_thread_main(struct test_thread_data* data)
{
	struct rjd_result result = rjd_condvar_lock(&data->goto_next_main);
	expect_result_ok(result);
	result = rjd_condvar_signal_all(&data->goto_next_main);
	expect_result_ok(result);
	result = rjd_condvar_unlock(&data->goto_next_main);
	expect_result_ok(result);

	result = rjd_condvar_wait(&data->goto_next_thread);
	expect_result_ok(result);
}

void step_thread_test(struct test_thread_data* data)
{
	const uint32_t TIMEOUT = 1;

	struct rjd_result result = rjd_condvar_lock(&data->goto_next_thread);
	expect_result_ok(result);
	result = rjd_condvar_signal_single(&data->goto_next_thread);
	expect_result_ok(result);
	result = rjd_condvar_unlock(&data->goto_next_thread);
	expect_result_ok(result);

	result = rjd_condvar_wait_timed(&data->goto_next_main, TIMEOUT);
	expect_result_ok(result);
}

void test_thread_entrypoint(void* userdata)
{
	struct test_thread_data* data = (struct test_thread_data*)userdata;
	struct rjd_result result = RJD_RESULT_OK();

	result = rjd_condvar_lock(&data->goto_next_thread);
	expect_result_ok(result);

	data->stage = TEST_THREAD_STAGE_STARTED;
	step_thread_main(data);
    
    data->stage = TEST_THREAD_STAGE_ACQUIRE_LOCK;
	result = rjd_lock_acquire(&data->lock);
	expect_result_ok(result);
	step_thread_main(data);
    
    data->stage = TEST_THREAD_STAGE_ACQUIRE_READ_LOCK;
	result = rjd_lock_release(&data->lock);
	expect_result_ok(result);
    result = rjd_rwlock_acquire_reader(&data->rwlock);
    expect_result_ok(result);
	step_thread_main(data);
    
    // promote reader to writer lock
    data->stage = TEST_THREAD_STAGE_ACQUIRE_WRITE_LOCK;
    result = rjd_rwlock_try_acquire_writer(&data->rwlock);
    expect_result_notok(result);
    
    #if RJD_PLATFORM_WINDOWS
        // unfortunately the windows and posix rwlock functionality for unlocking is different.
        // Windows will fail on attempting to unlock a writer when the thread already has a
        // read lock. But there is only 1 posix unlock function so it will always succeed.
        // Not sure the best way to unify behavior across the 2 platforms yet so I'll settle for
        // this for now.
        result = rjd_rwlock_release_writer(&data->rwlock);
        expect_result_notok(result);
    #endif

	result = rjd_rwlock_release_reader(&data->rwlock);
	expect_result_ok(result);
	result = rjd_rwlock_try_acquire_writer(&data->rwlock);
	expect_result_ok(result);
	step_thread_main(data);

	data->stage = TEST_THREAD_STAGE_FINISHED;
    result = rjd_rwlock_release_writer(&data->rwlock);
    expect_result_ok(result);

	result = rjd_condvar_unlock(&data->goto_next_thread);
	expect_result_ok(result);

	result = rjd_condvar_lock(&data->goto_next_main);
	expect_result_ok(result);
	result = rjd_condvar_signal_single(&data->goto_next_main);
	expect_result_ok(result);
	result = rjd_condvar_unlock(&data->goto_next_main);
	expect_result_ok(result);
}

void test_thread()
{
    {
        struct rjd_thread_id id1 = rjd_thread_id_current();
        struct rjd_thread_id id2 = rjd_thread_id_current();
        expect_true(rjd_thread_id_equals(id1, id1));
        expect_true(rjd_thread_id_equals(id1, id2));
    }
    
	struct rjd_mem_allocator allocator = rjd_mem_allocator_init_default();

    struct rjd_result result = RJD_RESULT_OK();
	struct test_thread_data thread_data = {0};
	struct rjd_thread_desc desc =
	{
		.entrypoint_func = test_thread_entrypoint,
		.allocator = &allocator,
		.userdata = &thread_data,
        .optional_name = "my_cool_thread!" // 16 bytes including null terminator
	};

	result = rjd_condvar_create(&thread_data.goto_next_main);
	expect_result_ok(result);
	result = rjd_condvar_create(&thread_data.goto_next_thread);
	expect_result_ok(result);
	result = rjd_lock_create(&thread_data.lock);
	expect_result_ok(result);
	result = rjd_rwlock_create(&thread_data.rwlock);
	expect_result_ok(result);
    
    result = rjd_condvar_lock(&thread_data.goto_next_main);
    expect_result_ok(result);

	struct rjd_thread thread = {0};
	result = rjd_thread_create(&thread, desc);
	expect_result_ok(result);
    
    const uint32_t TIMEOUT = 1;

	result = rjd_condvar_wait_timed(&thread_data.goto_next_main, TIMEOUT);
	expect_result_ok(result);
    
	expect_uint32(TEST_THREAD_STAGE_STARTED, thread_data.stage); // now the main and child threads are in lockstep
    {
        char outname[32] = {0};
        result = rjd_thread_getname(&thread, sizeof(outname), outname);
        expect_result_ok(result);
        expect_str("my_cool_thread!", outname);
    }
	step_thread_test(&thread_data);
    
	expect_uint32(TEST_THREAD_STAGE_ACQUIRE_LOCK, thread_data.stage);
    result = rjd_lock_try_acquire(&thread_data.lock);
    expect_result_notok(result); // thread should have the lock
	step_thread_test(&thread_data);
    
    expect_uint32(TEST_THREAD_STAGE_ACQUIRE_READ_LOCK, thread_data.stage);
    result = rjd_lock_try_acquire(&thread_data.lock);
    expect_result_ok(result);
    result = rjd_lock_release(&thread_data.lock);
    expect_result_ok(result);
    result = rjd_rwlock_try_acquire_reader(&thread_data.rwlock);
    expect_result_ok(result);
    result = rjd_rwlock_release_reader(&thread_data.rwlock);
    expect_result_ok(result);
	step_thread_test(&thread_data);
    
    expect_uint32(TEST_THREAD_STAGE_ACQUIRE_WRITE_LOCK, thread_data.stage);
    result = rjd_rwlock_try_acquire_writer(&thread_data.rwlock);
    expect_result_notok(result);
	step_thread_test(&thread_data);
    
    expect_uint32(TEST_THREAD_STAGE_FINISHED, thread_data.stage);
    result = rjd_rwlock_try_acquire_writer(&thread_data.rwlock);
    expect_result_ok(result);
    result = rjd_rwlock_release_writer(&thread_data.rwlock);
    expect_result_ok(result);
    
	// cleanup
    result = rjd_thread_join(&thread);
    expect_result_ok(result);
    
    result = rjd_condvar_unlock(&thread_data.goto_next_main);
    expect_result_ok(result);

	result = rjd_condvar_destroy(&thread_data.goto_next_main);
	expect_result_ok(result);
	result = rjd_condvar_destroy(&thread_data.goto_next_thread);
	expect_result_ok(result);
	result = rjd_lock_destroy(&thread_data.lock);
	expect_result_ok(result);
	result = rjd_rwlock_destroy(&thread_data.rwlock);
	expect_result_ok(result);
	
	expect_no_leaks(&allocator);
}

enum test_atomic_type
{
	test_atomic_type_int64,
	test_atomic_type_int32,
	test_atomic_type_int16,
	test_atomic_type_int8,
	test_atomic_type_uint64,
	test_atomic_type_uint32,
	test_atomic_type_uint16,
	test_atomic_type_uint8,
};

struct test_atomic_thread_data
{
	enum test_atomic_type test_type;
	void* atomic;
};

void test_atomic_thread_entrypoint(void* userdata)
{
	struct test_atomic_thread_data* data = userdata;

	// total delta is 20 - 10 + 10 - 15 - 5 = 0
	#define ATOMIC_THREAD_TEST(type)						\
		for (int i = 0; i < 20; ++i)						\
			rjd_atomic_ ## type ## _inc(data->atomic);		\
		for (int i = 0; i < 10; ++i)						\
			rjd_atomic_ ## type ## _dec(data->atomic);		\
		for (int i = 0; i < 5; ++i)							\
			rjd_atomic_ ## type ## _add(data->atomic, 2);	\
		for (int i = 0; i < 5; ++i)							\
			rjd_atomic_ ## type ## _sub(data->atomic, 3);	\
		for (int i = 0; i < 5; ++i)							\
			rjd_atomic_ ## type ## _dec(data->atomic);

	switch (data->test_type)
	{
	case test_atomic_type_int64:
		ATOMIC_THREAD_TEST(int64);
		break;
	case test_atomic_type_int32:
		ATOMIC_THREAD_TEST(int32);
		break;
	case test_atomic_type_int16:
		ATOMIC_THREAD_TEST(int16);
		break;
	case test_atomic_type_int8:
		ATOMIC_THREAD_TEST(int8);
		break;
	case test_atomic_type_uint64:
		ATOMIC_THREAD_TEST(uint64);
		break;
	case test_atomic_type_uint32:
		ATOMIC_THREAD_TEST(uint32);
		break;
	case test_atomic_type_uint16:
		ATOMIC_THREAD_TEST(uint16);
		break;
	case test_atomic_type_uint8:
		ATOMIC_THREAD_TEST(uint8);
		break;
	}

	#undef ATOMIC_THREAD_TEST
}

void test_atomic()
{
	struct rjd_mem_allocator allocator = rjd_mem_allocator_init_default();

	#define ATOMIC_TEST(type) \
		{	\
			struct rjd_atomic_ ## type value = rjd_atomic_ ## type ## _init(0);	\
			expect_ ## type(10, rjd_atomic_ ## type ## _add(&value, 10));	\
			expect_ ## type(5, rjd_atomic_ ## type ## _sub(&value, 5));	\
			expect_ ## type(6, rjd_atomic_ ## type ## _inc(&value));	\
			expect_ ## type(5, rjd_atomic_ ## type ## _dec(&value));	\
			expect_ ## type(1, rjd_atomic_ ## type ## _set(&value, 1));	\
			expect_ ## type(1, rjd_atomic_ ## type ## _get(&value));	\
			const uint32_t max_iterations = 1000;	\
			uint32_t iterations = 0;	\
			type ## _t expected = 1;	\
			while (!rjd_atomic_ ## type ## _compare_exchange(&value, &expected, 0)) {	\
				rjd_atomic_ ## type ## _dec(&value);	\
				if (++iterations > max_iterations) {	\
					break;	\
				}	\
			}	\
			expect_true(iterations <= max_iterations);	\
			struct test_atomic_thread_data thread_data = {	\
				.test_type = test_atomic_type_ ## type,	\
				.atomic = &value,	\
			};	\
			struct rjd_thread_desc desc = {	\
				.entrypoint_func = test_atomic_thread_entrypoint,	\
				.allocator = &allocator,	\
				.userdata = &thread_data,	\
				.optional_name = "atomic_test"	\
			};	\
			struct rjd_thread thread[10] = { 0 };	\
			for (size_t i = 0; i < rjd_countof(thread); ++i) {	\
				struct rjd_result result = rjd_thread_create(thread + i, desc);	\
				expect_result_ok(result);	\
			}	\
			for (size_t i = 0; i < rjd_countof(thread); ++i) {	\
				struct rjd_result result = rjd_thread_join(thread + i);	\
				expect_result_ok(result);	\
			}	\
			expect_ ## type(0, rjd_atomic_ ## type ## _get(&value));	\
		}

	ATOMIC_TEST(int64)
	ATOMIC_TEST(int32)
	ATOMIC_TEST(int16)
	ATOMIC_TEST(int8)
	ATOMIC_TEST(uint64)
	ATOMIC_TEST(uint32)
	ATOMIC_TEST(uint16)
	ATOMIC_TEST(uint8)

	#undef ATOMIC_TEST

}

void test_strpool()
{
	struct rjd_mem_allocator allocator = rjd_mem_allocator_init_default();

	const char* test1 = "test1";
	const char* test2 = "a really really super long string that has an end lalalalalalalala";
	const char* test3 = " ";
	const char* test4 = "forma7 7e57";
	const char* test5 = "substring";

	struct rjd_strpool pool = rjd_strpool_init(&allocator, 2);

	struct rjd_strref* ref1 = rjd_strpool_add(&pool, test1);
	struct rjd_strref* ref2 = rjd_strpool_add(&pool, test2);
	struct rjd_strref* ref3 = rjd_strpool_add(&pool, test3);
	struct rjd_strref* ref4 = rjd_strpool_add(&pool, "forma%d %de%d%d",7,7,5,7);
	struct rjd_strref* ref5 = rjd_strpool_addl(&pool, "substring test", strlen(test5));

	expect_true(ref1 == rjd_strpool_add(&pool, test1));
	expect_true(ref2 == rjd_strpool_add(&pool, test2));
	expect_true(ref3 == rjd_strpool_add(&pool, test3));
	expect_true(ref4 == rjd_strpool_add(&pool, test4));
	expect_true(ref5 == rjd_strpool_add(&pool, test5));

	expect_str(rjd_strref_str(ref1), test1);
	expect_str(rjd_strref_str(ref2), test2);
	expect_str(rjd_strref_str(ref3), test3);
	expect_str(rjd_strref_str(ref4), test4);
	expect_str(rjd_strref_str(ref5), test5);

	expect_uint32((uint32_t)strlen(test1), rjd_strref_length(ref1));
	expect_uint32((uint32_t)strlen(test1), rjd_strref_length(ref1));
	expect_uint32((uint32_t)strlen(test1), rjd_strref_length(ref1));
	expect_uint32((uint32_t)strlen(test1), rjd_strref_length(ref1));
	expect_uint32((uint32_t)strlen(test1), rjd_strref_length(ref1));

	rjd_strref_release(ref1);
	rjd_strref_release(ref2);
	rjd_strref_release(ref3);
	rjd_strref_release(ref4);
	rjd_strref_release(ref5);

	expect_str(rjd_strref_str(ref1), test1);
	expect_str(rjd_strref_str(ref2), test2);
	expect_str(rjd_strref_str(ref3), test3);
	expect_str(rjd_strref_str(ref4), test4);
	expect_str(rjd_strref_str(ref5), test5);

	rjd_strpool_free(&pool);

	expect_no_leaks(&allocator);
}

void test_slotmap(void)
{
	struct rjd_mem_allocator allocator = rjd_mem_allocator_init_default();

	{
		uint32_t* map = rjd_slotmap_alloc(uint32_t, 8, &allocator);

        expect_uint32(0, rjd_slotmap_count(map));

        struct rjd_slot slots[25] = {0};
        for (uint32_t i = 0; i < rjd_countof(slots); ++i) {
            rjd_slotmap_insert(map, i + 1337, slots + i);
        }

        expect_uint32(25, rjd_slotmap_count(map));

        const uint32_t* const_map = map;

        for (uint32_t i = 0; i < rjd_countof(slots); ++i) {
            uint32_t value = *rjd_slotmap_get(const_map, slots[i]);
            expect_uint32(i + 1337, value);
        }

        uint32_t visited = 0;
        for (struct rjd_slot s = rjd_slotmap_next(map, NULL); rjd_slot_isvalid(s); s = rjd_slotmap_next(map, &s))
        {
            ++visited;
        }
        expect_uint32(25, visited);

        for (uint32_t i = 0; i < rjd_countof(slots); i += 5) {
            rjd_slotmap_erase(map, slots[i]);
        }

        for (uint32_t i = 0; i < rjd_countof(slots); ++i)
        {
            bool actual = rjd_slotmap_contains(map, slots[i]);
            bool expected = (i % 5) != 0;
            expect_uint32(expected, actual);
        }

        visited = 0;
        for (struct rjd_slot s = rjd_slotmap_next(map, NULL); rjd_slot_isvalid(s); s = rjd_slotmap_next(map, &s))
        {
            ++visited;
        }
        expect_uint32(20, visited);

        struct rjd_slot s = rjd_slotmap_next(map, NULL);
        expect_true(rjd_slot_isvalid(s));
        rjd_slot_invalidate(&s);
        expect_false(rjd_slot_isvalid(s));

		rjd_slotmap_free(map);
	}

	expect_no_leaks(&allocator);
}

void test_utf8(void)
{
	// rjd_utf8_bom_skip
	{
		const uint8_t buffer_with_bom[] = { 0xEF, 0xBB, 0xBF, 0 };
		const uint8_t buffer_no_bom[] = "hey this is a normal string";

		const uint8_t* expected_with_bom = buffer_with_bom + 3;
		const uint8_t* expected_no_bom = buffer_no_bom;

		expect_pointer(expected_with_bom, rjd_utf8_bom_skip((char*)buffer_with_bom));
		expect_pointer(expected_no_bom, rjd_utf8_bom_skip((char*)buffer_no_bom));
	}

	// rjd_utf8_bom_write
	{
		char zeroes[10] = {0};
		char buffer_with_bom[10] = {0};
		char buffer_no_bom[2] = {0};
		struct rjd_result result;

		result = rjd_utf8_bom_write(buffer_with_bom, sizeof(buffer_with_bom));
		expect_result_ok(result);
		expect_true(0 != memcmp(buffer_with_bom, zeroes, sizeof(buffer_with_bom)));

		result = rjd_utf8_bom_write(buffer_no_bom, sizeof(buffer_no_bom));
		expect_result_notok(result);
		expect_true(0 == memcmp(buffer_no_bom, zeroes, sizeof(buffer_no_bom)));
	}

	// rjd_utf8_next
	{
		const char ascii[] = "12345";
		const char utf8[] = "aæ࠳ab߉නc𐤈𐤀";
        //const char invalid_utf8[] = { 0b11111100, 0b11111110, 0b11111001, 0 };
        const uint8_t invalid_utf8[] = { 0xFB, 0xFE, 0xF9, 0 };

		const char* p = NULL;

		p = rjd_utf8_next(ascii);
		expect_pointer(ascii + 1, p);
		p = rjd_utf8_next(p);
		expect_pointer(ascii + 2, p);
		p = rjd_utf8_next(p);
		expect_pointer(ascii + 3, p);
		p = rjd_utf8_next(p);
		expect_pointer(ascii + 4, p);
		p = rjd_utf8_next(p);
		expect_pointer(ascii + 5, p);

		// mixed ascii and utf8 characters
		p = rjd_utf8_next(utf8);
		expect_pointer(utf8 + 1, p);
		p = rjd_utf8_next(p);
		expect_pointer(utf8 + 1+2, p);
		p = rjd_utf8_next(p);
		expect_pointer(utf8 + 1+2+3, p);
		p = rjd_utf8_next(p);
		expect_pointer(utf8 + 1+2+3+1, p);
		p = rjd_utf8_next(p);
		expect_pointer(utf8 + 1+2+3+1+1, p);
		p = rjd_utf8_next(p);
		expect_pointer(utf8 + 1+2+3+1+1+2, p);
		p = rjd_utf8_next(p);
		expect_pointer(utf8 + 1+2+3+1+1+2+3, p);
		p = rjd_utf8_next(p);
		expect_pointer(utf8 + 1+2+3+1+1+2+3+1, p);
		p = rjd_utf8_next(p);
		expect_pointer(utf8 + 1+2+3+1+1+2+3+1+4, p);
		p = rjd_utf8_next(p);
		expect_pointer(utf8 + 1+2+3+1+1+2+3+1+4+4, p);

		// middle of utf8 character
		p = rjd_utf8_next(utf8 + 2);
		expect_pointer(utf8 + 1+2, p);
		p = rjd_utf8_next(utf8 + 2+1+1);
		expect_pointer(utf8 + 1+2+3, p);

		p = rjd_utf8_next((char*)invalid_utf8 + 0);
		expect_pointer(NULL, p);
        p = rjd_utf8_next((char*)invalid_utf8 + 1);
        expect_pointer(NULL, p);
        p = rjd_utf8_next((char*)invalid_utf8 + 2);
        expect_pointer(NULL, p);
	}
}

void test_path(void)
{
	// path operations
    expect_path("", rjd_path_create());
    expect_path("/", rjd_path_create_with("/"));
    expect_path("/", rjd_path_create_with("///"));
    expect_path("a", rjd_path_create_with("a"));
    expect_path("a", rjd_path_create_with("a/"));
    expect_path("/a", rjd_path_create_with("/a"));
    expect_path("/a", rjd_path_create_with("///a"));
    expect_path("/a", rjd_path_create_with("///a///"));
    expect_path("/a/b/c", rjd_path_create_with("/a/b/c/"));
    expect_path("/a/b/c", rjd_path_create_with("/a/b/c"));
    expect_path("/a/b/c", rjd_path_create_with("/a//b//c"));
    expect_path("/a/b/c", rjd_path_create_with("///a//b//c//"));
    expect_path("/aaa/bb/c", rjd_path_create_with("///aaa//bb//c//"));
    expect_path("/a/bb/ccc", rjd_path_create_with("///a//bb//ccc//"));
    expect_path("/a/bbb/c", rjd_path_create_with("///a//bbb//c//"));
    expect_path("a/bbb/c/ddddd", rjd_path_create_with("a//bbb//c//ddddd"));
    expect_path("c:", rjd_path_create_with("c:///"));
    expect_path("c:/abc", rjd_path_create_with("c:///abc/"));

	{
		struct rjd_path path = rjd_path_create();
		rjd_path_append(&path, "/");
		expect_path("/", path);
		rjd_path_append(&path, "/");
        expect_path("/", path);
		rjd_path_append(&path, "///");
		expect_path("/", path);
		rjd_path_append(&path, "a");
		expect_path("/a", path);
		rjd_path_append(&path, "b");
		expect_path("/a/b", path);
		rjd_path_append(&path, "//c//");
		expect_path("/a/b/c", path);
        rjd_path_append(&path, "//d//");
        expect_path("/a/b/c/d", path);
		rjd_path_clear(&path);
		expect_path("", path);
	}

	{
		struct rjd_path path1 = rjd_path_create_with("a/b/c");
		struct rjd_path path2 = rjd_path_create_with("d/e");

		rjd_path_join(&path1, &path2);
		expect_path("a/b/c/d/e", path1);
	}

	// extension
	expect_str(NULL,   rjd_path_extension_str(NULL));
	expect_str(NULL,   rjd_path_extension_str(""));
	expect_str(NULL,   rjd_path_extension_str("no_extension"));
	expect_str(NULL,   rjd_path_extension_str("not_even_this_one."));
	expect_str(".txt", rjd_path_extension_str(".txt"));
	expect_str(".txt", rjd_path_extension_str("some_file_name.txt"));
	expect_str(".txt", rjd_path_extension_str("some/path/some_file.txt"));
	expect_str(".txt", rjd_path_extension_str("some\\path\\some_file.txt"));
	expect_str(".txt", rjd_path_extension_str("some\\path\\some.long.extension.txt"));
}

void test_stream()
{
	struct rjd_mem_allocator allocator = rjd_mem_allocator_init_default();

	// istream zeroes
	{
		char actual[27] = {0};
		char expected[27] = {0};

		struct rjd_istream stream = rjd_istream_from_zeroes();
		for (int i = 0; i < 20; ++i)
		{
			memset(actual, 0xFF, sizeof(actual));
			struct rjd_result result = rjd_istream_read(&stream, actual, sizeof(actual));
			expect_result_ok(result);
			int equal = memcmp(actual, expected, sizeof(actual));
			expect_int32(0, equal);
		}
		
		rjd_istream_close(&stream);
	}

	// istream memory
	{
		const char string[] = "how now brown cow. the quick\nbrown fox jumped\t\nover the lazy dog.";
		char buffer[512];

		struct rjd_istream stream = rjd_istream_from_memory(string, sizeof(string));
		struct rjd_result result = rjd_istream_read(&stream, buffer, 15);
		expect_result_ok(result);
		expect_int32(0, memcmp(string, buffer, 15));
		expect_true(0 != memcmp(string + 15, buffer + 15, sizeof(string) - 15));

		result = rjd_istream_read(&stream, buffer + 15, sizeof(string) - 15);
		expect_result_ok(result);
		expect_int32(0, memcmp(string, buffer, sizeof(string)));

		char zeroes[32] = {0};
		result = rjd_istream_read(&stream, buffer, sizeof(zeroes));
		expect_result_notok(result);
		expect_int32(0, memcmp(buffer, zeroes, sizeof(zeroes)));

		rjd_istream_close(&stream);
	}

    // istream file
    {
		char file_expected[500] = {0};
		for (size_t i = 0; i < sizeof(file_expected); ++i) {
			int range = 'z' - 'a';
			file_expected[i] = 'a' + (char)(i % range);
		}

		struct rjd_result result = rjd_fio_write("test.txt", file_expected, sizeof(file_expected), RJD_FIO_WRITEMODE_REPLACE);
		expect_result_ok(result);

        struct rjd_istream stream = rjd_istream_from_file("test.txt", &allocator, 32);
		char file_contents[500] = {0};
        result = rjd_istream_read(&stream, file_contents, sizeof(file_contents) / 2);
        expect_result_ok(result);
		result = rjd_istream_read(&stream, file_contents + sizeof(file_contents) / 2, sizeof(file_contents) / 2);
        expect_result_ok(result);
        {
            char expected[256] = {0};
            char tmp[256];
            memset(tmp, 1, sizeof(tmp));
            result = rjd_istream_read(&stream, tmp, sizeof(tmp));
            expect_result_notok(result); // end of file message
            expect_int32(0, memcmp(expected, tmp, sizeof(tmp)));
        }
		expect_int32(0, memcmp(file_expected, file_contents, sizeof(file_expected)));
        
        char after_feof[50];
        result = rjd_istream_read(&stream, after_feof, sizeof(after_feof));
        expect_result_notok(result);
        
		rjd_istream_close(&stream);

		result = rjd_fio_delete("test.txt");
        expect_result_ok(result);
    }
    
    // ostream memory
    {
        char output[500];
        const char expected[] = "balahaslksfdækljáááááaklsdfklasjflksajdfnjid"; // 50 chars
        const size_t expected_size = sizeof(expected) - 1;
        RJD_ASSERT(sizeof(output) % expected_size == 0);
        
        struct rjd_ostream stream = rjd_ostream_from_memory(output, sizeof(output));
        for (size_t i = 0; i < sizeof(output); i += expected_size) {
            struct rjd_result result = rjd_ostream_write(&stream, expected, expected_size);
            expect_result_ok(result);
            expect_int32(0, memcmp(expected, output + i, expected_size));
        }
        
        struct rjd_result result = rjd_ostream_write(&stream, expected, expected_size);
        expect_result_notok(result);
        
        rjd_ostream_close(&stream);
    }

    // ostream file
    {
        char file_expected[500] = {0};
        for (size_t i = 0; i < sizeof(file_expected); ++i) {
            int range = 'z' - 'a';
            file_expected[i] = 'a' + (char)(i % range);
        }
        
        const size_t chunksize = sizeof(file_expected) / 2;
        struct rjd_ostream stream = rjd_ostream_from_file("test.txt", RJD_OSTREAM_MODE_REPLACE);
        struct rjd_result result = rjd_ostream_write(&stream, file_expected, chunksize);
        expect_result_ok(result);
        rjd_ostream_close(&stream);
        
        stream = rjd_ostream_from_file("test.txt", RJD_OSTREAM_MODE_APPEND);
        result = rjd_ostream_write(&stream, file_expected + chunksize, chunksize);
        rjd_ostream_close(&stream);
        
        char file_actual[sizeof(file_expected)];
        FILE* file = fopen("test.txt", "rb");
        RJD_ASSERT(file);
        expect_uint64(sizeof(file_actual), fread(file_actual, 1, sizeof(file_actual), file));
        fclose(file);
        expect_int32(0, memcmp(file_expected, file_actual, sizeof(file_actual)));
        
        result = rjd_fio_delete("test.txt");
        expect_result_ok(result);
    }

	expect_no_leaks(&allocator);
}

struct test
{
	int64_t i64;
	uint64_t u64;
	uint32_t u32;
	int32_t i32;
	int16_t i16;
	uint16_t u16;
	int8_t i8;
	uint8_t u8;
	char pad[2]; // explicit to ensure the pad bytes get initialized as well
};

void test_struct_readwrite(struct rjd_binrw_state* state, struct test* data)
{
	struct rjd_result result;

	result = rjd_binrw_readwrite_int8(state, &data->i8);
	expect_result_ok(result);
	result = rjd_binrw_readwrite_int16(state, &data->i16);
	expect_result_ok(result);
	result = rjd_binrw_readwrite_int32(state, &data->i32);
	expect_result_ok(result);
	result = rjd_binrw_readwrite_int64(state, &data->i64);
	expect_result_ok(result);
	result = rjd_binrw_readwrite_uint8(state, &data->u8);
	expect_result_ok(result);
	result = rjd_binrw_readwrite_uint16(state, &data->u16);
	expect_result_ok(result);
	result = rjd_binrw_readwrite_uint32(state, &data->u32);
	expect_result_ok(result);
	result = rjd_binrw_readwrite_uint64(state, &data->u64);
	expect_result_ok(result);
}

void test_binrw()
{
	struct test expected_min = {
		.i8 = CHAR_MIN,
		.i16 = SHRT_MIN,
		.i32 = INT_MIN,
		.i64 = LLONG_MIN,
		.u8 = 0,
		.u16 = 0,
		.u32 = 0,
		.u64 = 0,
	};

	struct test expected_max = {
		.i8 = CHAR_MAX,
		.i16 = SHRT_MAX,
		.i32 = INT_MAX,
		.i64 = LLONG_MAX,
		.u8 = UCHAR_MAX,
		.u16 = USHRT_MAX,
		.u32 = UINT_MAX,
		.u64 = ULLONG_MAX,
	};

	char buffer[512];
	RJD_STATIC_ASSERT(sizeof(buffer) >= sizeof(struct test) * 2);

	struct rjd_ostream ostream = rjd_ostream_from_memory(buffer, sizeof(buffer));
	struct rjd_istream istream = rjd_istream_from_memory(buffer, sizeof(buffer));

	struct rjd_binrw_state writer = { .ostream = &ostream };
	struct rjd_binrw_state reader = { .istream = &istream };

	// The readwrite functions use the read_x() and write_x() functions, so we'll skip testing those
	test_struct_readwrite(&writer, &expected_min);
	test_struct_readwrite(&writer, &expected_max);

    struct test actual_min = {0};
    struct test actual_max = {0};

	test_struct_readwrite(&reader, &actual_min);
	test_struct_readwrite(&reader, &actual_max);

	expect_int32(0, memcmp(&expected_min, &actual_min, sizeof(actual_min)));
	expect_int32(0, memcmp(&expected_max, &actual_max, sizeof(actual_max)));
}

void test_strhash()
{
	struct rjd_mem_allocator allocator = rjd_mem_allocator_init_default();
    rjd_strhash_global_init(&allocator, 32);

    {
        const char* str1 = "test ok my friend";
        const char* str2 = "a different string! !@#$^&*()";
        
        struct rjd_strhash hash1 = rjd_strhash_init(NULL);
        struct rjd_strhash hash2 = rjd_strhash_init("");
        struct rjd_strhash hash3 = rjd_strhash_init(str1);
        struct rjd_strhash hash4 = rjd_strhash_init(str1);
        struct rjd_strhash hash5 = rjd_strhash_init(str2);
        
        expect_true(hash1.hash.value == hash2.hash.value);
        expect_false(hash1.hash.value == hash3.hash.value);
        expect_false(hash1.hash.value == hash4.hash.value);
        expect_false(hash1.hash.value == hash5.hash.value);
        expect_true(hash3.hash.value == hash4.hash.value);
        expect_false(hash4.hash.value == hash5.hash.value);
        
        expect_pointer(NULL, hash1.debug_string);
        expect_pointer(NULL, hash2.debug_string);
        expect_str(str1, rjd_strref_str(hash3.debug_string));
        expect_str(str1, rjd_strref_str(hash4.debug_string));
        expect_str(str2, rjd_strref_str(hash5.debug_string));
    }
    
    rjd_strhash_global_destroy();
    
    {
        struct rjd_strhash hash1 = rjd_strhash_init(NULL);
        struct rjd_strhash hash2 = rjd_strhash_init("");
        struct rjd_strhash hash3 = rjd_strhash_init("test ok my friend");
        
        expect_true(hash1.hash.value == hash2.hash.value);
        expect_false(hash1.hash.value == hash3.hash.value);
        
        expect_pointer(NULL, hash1.debug_string);
        expect_pointer(NULL, hash2.debug_string);
        expect_pointer(NULL, hash3.debug_string);
    }

	expect_no_leaks(&allocator);
}

////////////////////////////////////////////////////////////////////////////////
// resource test data and functions
struct test_material
{
	struct rjd_resource_handle shader;
	struct rjd_resource_handle texture;
};

struct test_shader
{
	char test_data[64];
};

struct test_texture
{
	uint32_t width;
	uint32_t height;
};

struct did_call_load_stages
{
	bool begin;
	bool end;
	bool unload;
};

void verify_shader_data(struct test_shader* shader)
{
	expect_str("shader_code_here", shader->test_data);
}

void verify_texture_data(struct test_texture* texture)
{
	expect_uint32(64, texture->width);
	expect_uint32(64, texture->height);
}

struct rjd_result test_material_load(struct rjd_resource_load_begin_params* params)
{
	struct did_call_load_stages* did_call_stages = (struct did_call_load_stages*)params->userdata;
	did_call_stages->begin = true;

	struct test_material* material = params->typed_resource_data;
	
	// all resources should be initialized to zeroes by the resource system
	const char zeroes[sizeof(struct test_material)] = {0};
	expect_int32(0, memcmp(zeroes, material, sizeof(zeroes)));

	char path1[256] = {0};
	char path2[256] = {0};

	int read = sscanf((char*)params->stream_data.start, "%s\n", path1);
    expect_uint32(1, read);
	read = sscanf((char*)params->stream_data.start + strlen(path1) + 1, "%s\n", path2);
    expect_uint32(1, read);

	struct rjd_resource_id res1 = rjd_resource_id_from_str(path1);
	struct rjd_resource_id res2 = rjd_resource_id_from_str(path2);

	material->shader = rjd_resource_handle_none();
	material->texture = rjd_resource_handle_none();

	struct rjd_result result1 = params->load_dependency_func(params->dependency_params, res1, &material->shader);
	struct rjd_result result2 = params->load_dependency_func(params->dependency_params, res2, &material->texture);

	expect_result_ok(result1);
	expect_result_ok(result2);

	return RJD_RESULT_OK();
}

struct rjd_result test_material_load_end(struct rjd_resource_load_end_params* params)
{
	struct did_call_load_stages* did_call_stages = (struct did_call_load_stages*)params->userdata;
	did_call_stages->end = true;

	struct test_material* material = (struct test_material*)params->typed_resource_data;
	struct rjd_resource_dependency shader = params->get_dependency_func(params->get_dependency_params, material->shader);
	struct rjd_resource_dependency texture = params->get_dependency_func(params->get_dependency_params, material->texture);
    
    expect_int32(RJD_RESOURCE_STATUS_READY, shader.status);
    expect_int32(RJD_RESOURCE_STATUS_READY, texture.status);

	verify_shader_data((struct test_shader*)shader.typed_resource_data);
	verify_texture_data((struct test_texture*)texture.typed_resource_data);

	return RJD_RESULT_OK();
}

void test_material_unload(struct rjd_resource_unload_params* params)
{
	struct test_material* material = (struct test_material*)params->typed_resource_data;

	// child resources should still be loaded
    struct rjd_resource_dependency shader = params->get_dependency_func(params->get_dependency_params, material->shader);
    struct rjd_resource_dependency texture = params->get_dependency_func(params->get_dependency_params, material->shader);
    
    expect_int32(RJD_RESOURCE_STATUS_READY, shader.status);
    expect_int32(RJD_RESOURCE_STATUS_READY, texture.status);
    
	expect_true(shader.typed_resource_data != NULL);
	expect_true(texture.typed_resource_data != NULL);

	struct did_call_load_stages* did_call_stages = (struct did_call_load_stages*)params->userdata;
	did_call_stages->unload = true;
}

struct rjd_result test_shader_load(struct rjd_resource_load_begin_params* params)
{
	struct did_call_load_stages* did_call_stages = (struct did_call_load_stages*)params->userdata;
	did_call_stages->begin = true;

	struct test_shader* shader = (struct test_shader*)params->typed_resource_data;

	const char zeroes[sizeof(struct test_shader)] = {0};
	expect_int32(0, memcmp(zeroes, shader, sizeof(zeroes)));

	int read = sscanf((char*)params->stream_data.start, "%s\n", shader->test_data);
	expect_true(read > 0);

	verify_shader_data(shader);

	return RJD_RESULT_OK();
}

struct rjd_result test_texture_load(struct rjd_resource_load_begin_params* params)
{
	struct did_call_load_stages* did_call_stages = (struct did_call_load_stages*)params->userdata;
	did_call_stages->begin = true;

	struct test_texture* texture = (struct test_texture*)params->typed_resource_data;

	const char zeroes[sizeof(struct test_texture)] = {0};
	expect_int32(0, memcmp(zeroes, texture, sizeof(zeroes)));

	int read = sscanf((char*)params->stream_data.start, "%u\n", &texture->width);
	expect_uint32(1, read);
	read = sscanf(strstr((char*)params->stream_data.start, "\n") + 1, "%u\n", &texture->height);
    expect_uint32(1, read);

	verify_texture_data(texture);

	return RJD_RESULT_OK();
}

void test_texture_unload(struct rjd_resource_unload_params* params)
{
	struct did_call_load_stages* did_call_stages = (struct did_call_load_stages*)params->userdata;
	did_call_stages->unload = true;
}

// 
////////////////////////////////////////////////////////////////////////////////

void test_resource()
{
	struct rjd_mem_allocator allocator = rjd_mem_allocator_init_default();
    rjd_strhash_global_init(&allocator, 32);

	// rjd_resource types
	{
		{
			struct rjd_resource_id id1 = rjd_resource_id_from_str(NULL);
			struct rjd_resource_id id2 = rjd_resource_id_from_str("");
			struct rjd_resource_id id3 = rjd_resource_id_from_str("some_asset.txt");

			expect_true(rjd_resource_id_equals(id1, id2));
			expect_false(rjd_resource_id_equals(id1, id3));
			expect_false(rjd_resource_id_equals(id2, id3));

			expect_false(rjd_resource_id_isvalid(id1));
			expect_false(rjd_resource_id_isvalid(id2));
			expect_true(rjd_resource_id_isvalid(id3));
		}

		{
			struct rjd_resource_type_id id1 = rjd_resource_type_id_from_str(NULL);
			struct rjd_resource_type_id id2 = rjd_resource_type_id_from_str("");
			struct rjd_resource_type_id id3 = rjd_resource_type_id_from_str("texture");

			expect_true(rjd_resource_type_id_equals(id1, id2));
			expect_false(rjd_resource_type_id_equals(id1, id3));
			expect_false(rjd_resource_type_id_equals(id2, id3));

			expect_false(rjd_resource_type_id_isvalid(id1));
			expect_false(rjd_resource_type_id_isvalid(id2));
			expect_true(rjd_resource_type_id_isvalid(id3));
		}

		{
			struct rjd_resource_handle handle1 = { .slot = { .salt = 0 } };
			struct rjd_resource_handle handle2 = { .slot = { .salt = 0 } };
			struct rjd_resource_handle handle3 = { .slot = { .salt = 1 } };
			struct rjd_resource_handle handle4 = { .slot = { .salt = 2 } };
			struct rjd_resource_handle handle5 = { .slot = { .salt = 2 } };

			expect_true(rjd_resource_handle_equals(handle1, handle2));
			expect_true(rjd_resource_handle_equals(handle4, handle5));
			expect_false(rjd_resource_handle_equals(handle2, handle3));
			expect_false(rjd_resource_handle_equals(handle3, handle4));

			expect_false(rjd_resource_handle_isvalid(handle1));
			expect_false(rjd_resource_handle_isvalid(handle2));
			expect_true(rjd_resource_handle_isvalid(handle3));
			expect_true(rjd_resource_handle_isvalid(handle4));
			expect_true(rjd_resource_handle_isvalid(handle5));
		}
	}

	// rjd_resource_loader
	{
        const struct rjd_resource_extension_to_type_id type_mappings[] = {
            { .type = rjd_strhash_init("config"), .extension = ".cfg" },
            { .type = rjd_strhash_init("level"), .extension = ".lvl" },
            { .type = rjd_strhash_init("shader"), .extension = ".shader" },
            { .type = rjd_strhash_init("texture"), .extension = ".bmp" },
        };

        const struct rjd_resource_loader_desc desc = {
            .type = RJD_RESOURCE_LOADER_TYPE_FILESYSTEM,
            .allocator = &allocator,
            .filesystem = {
                .root = "test_data/resource/loader_filesystem",
                .type_mappings = type_mappings,
                .type_mappings_count = rjd_countof(type_mappings),
            },
        };

        struct rjd_resource_loader loader;
        {
	        struct rjd_result result = rjd_resource_loader_create(&loader, desc);
	        expect_result_ok(result);
	    }

        struct expected_resource_data
        {
            struct rjd_resource_id id;
            int32_t mapping_index;
            const char* data;
        };

        const struct expected_resource_data expected_data[] = {
			{ rjd_resource_id_from_str("bootstrap.lvl"),			1, "bootstrap level\n" },
			{ rjd_resource_id_from_str("gfx/invalid.bmp"),			3, "placeholder bitmap\n" },
			{ rjd_resource_id_from_str("gfx/quad.shader"),			2, "shader code here\n" },
			{ rjd_resource_id_from_str("init.cfg"),					0, "some init data is in here\n" },
			{ rjd_resource_id_from_str("levels/mainmenu.lvl"),		1, "the main menu and ui\n" },
			{ rjd_resource_id_from_str("levels/dungeon.lvl"),		1, "first dungeon level\n" },
			{ rjd_resource_id_from_str("does_not_exist.bmp"),		-1, NULL },
			{ rjd_resource_id_from_str("unregistered_type.txt"),	-1, NULL },
        };

        {
            for (uint32_t i = 0; i < rjd_countof(expected_data); ++i) {
                struct rjd_resource_type_id out_type = {0};

                struct rjd_result result = rjd_resource_loader_get_type(&loader, expected_data[i].id, &out_type);
                int32_t mapping_index = expected_data[i].mapping_index;
                if (mapping_index == -1) {
                    expect_result_notok(result);
                } else {
                    expect_result_ok(result);
                    expect_true(rjd_resource_type_id_equals(type_mappings[mapping_index].type, out_type));
                }

                struct rjd_istream stream = {0};
                result = rjd_resource_loader_load(&loader, expected_data[i].id, &allocator, &stream);
                if (mapping_index == -1) {
                    expect_result_notok(result);
                } else {
                    const char* expected_str = expected_data[i].data;
                    expect_result_ok(result);

                    uint64_t length = stream.end - stream.start;

                    expect_int64(strlen(expected_str), length);
                    expect_int32(0, memcmp(expected_str, stream.start, (size_t)length));
                }
                rjd_istream_close(&stream);
            }
        }
        
        rjd_resource_loader_destroy(&loader);
	}

    // rjd_resource
    {
        const struct rjd_resource_extension_to_type_id type_material = { .type = rjd_strhash_init("test_material"), .extension = ".material" };
        const struct rjd_resource_extension_to_type_id type_shader = { .type = rjd_strhash_init("test_shader"), .extension = ".shader" };
        const struct rjd_resource_extension_to_type_id type_texture = { .type = rjd_strhash_init("test_texture"), .extension = ".bmp" };

        const struct rjd_resource_extension_to_type_id type_mappings[] = {
            type_material,
            type_shader,
            type_texture,
        };

        struct rjd_resource_loader loader;
        {
	        const struct rjd_resource_loader_desc desc = {
	            .type = RJD_RESOURCE_LOADER_TYPE_FILESYSTEM,
	            .allocator = &allocator,
	            .filesystem = {
	                .root = "test_data/resource/lib",
	                .type_mappings = type_mappings,
	                .type_mappings_count = rjd_countof(type_mappings),
	            },
	        };

            struct rjd_result result = rjd_resource_loader_create(&loader, desc);
            expect_result_ok(result);
        }

        struct rjd_resource_lib lib;
        {
            struct rjd_resource_lib_desc desc = {
                .allocator = &allocator,
                .loader = &loader,
            };

            rjd_resource_lib_create(&lib, desc);
        }

        struct did_call_load_stages did_call_stages_material = {0};
        struct did_call_load_stages did_call_stages_shader = {0};
        struct did_call_load_stages did_call_stages_texture = {0};

        struct rjd_resource_type resource_types[] =
        {
            {
                .id = type_material.type,
                .userdata = &did_call_stages_material,
                .load_begin_func = test_material_load,
                .optional_load_end_func = test_material_load_end,
                .optional_unload_func = test_material_unload,
                .in_memory_size = sizeof(struct test_material),
            },
            {
                .id = type_shader.type,
                .userdata = &did_call_stages_shader,
                .load_begin_func = test_shader_load,
                .optional_load_end_func = NULL,
                .optional_unload_func = NULL,
                .in_memory_size = sizeof(struct test_shader),
            },
            {
                .id = type_texture.type,
                .userdata = &did_call_stages_texture,
                .load_begin_func = test_texture_load,
                .optional_load_end_func = NULL,
                .optional_unload_func = test_texture_unload,
                .in_memory_size = sizeof(struct test_texture),
            },
        };

        for (uint32_t i = 0; i < rjd_countof(resource_types); ++i)
        {
            struct rjd_result result = rjd_resource_lib_register_type(&lib, resource_types[i]);
            expect_result_ok(result);
        }

        // these should have all been registered already
        for (uint32_t i = 0; i < rjd_countof(resource_types); ++i)
        {
            struct rjd_result result = rjd_resource_lib_register_type(&lib, resource_types[i]);
            expect_result_notok(result);
        }

        // does not exist test
        {
            const struct rjd_resource_id id_does_not_exist = rjd_resource_id_from_str("gfx/does_not_exist.shader");
            struct rjd_resource_handle handle_does_not_exist = rjd_resource_handle_none();

            struct rjd_result result = rjd_resource_load(&lib, id_does_not_exist, &handle_does_not_exist);
            expect_result_notok(result);

            enum rjd_resource_status status = rjd_resource_lib_status(&lib, handle_does_not_exist);
            expect_int32(RJD_RESOURCE_STATUS_INVALID, status);
        }

        const struct rjd_resource_id id_material = rjd_resource_id_from_str("gfx/test.material");
        const struct rjd_resource_id id_shader = rjd_resource_id_from_str("gfx/test.shader");
        const struct rjd_resource_id id_texture = rjd_resource_id_from_str("gfx/test.bmp");

        struct rjd_resource_handle handle_material = rjd_resource_handle_none();
        struct rjd_resource_handle handle_shader = rjd_resource_handle_none();
        struct rjd_resource_handle handle_texture = rjd_resource_handle_none();

        // load material
        {
            struct rjd_result result = rjd_resource_load(&lib, id_material, &handle_material);
            expect_result_ok(result);

            enum rjd_resource_status status = rjd_resource_lib_status(&lib, handle_material);
            expect_int32(RJD_RESOURCE_STATUS_LOAD_BEGIN, status);

            rjd_resource_lib_wait(&lib, &handle_material, 1);

            status = rjd_resource_lib_status(&lib, handle_material);
            expect_int32(RJD_RESOURCE_STATUS_READY, status);

            expect_true(did_call_stages_material.begin);
            expect_true(did_call_stages_material.end);
            expect_false(did_call_stages_material.unload);
        }

        // load shader
        {
            struct rjd_result result = rjd_resource_load(&lib, id_shader, &handle_shader);

            enum rjd_resource_status status = rjd_resource_lib_status(&lib, handle_shader);
            expect_int32(RJD_RESOURCE_STATUS_READY, status); // should have been loaded by the material

            expect_result_ok(result);
            expect_true(did_call_stages_shader.begin);
            expect_false(did_call_stages_shader.end); // doesn't have a load end function
            expect_false(did_call_stages_shader.unload);
            verify_shader_data((struct test_shader*)rjd_resource_get(&lib, handle_shader));
        }

        // load texture
        {
            struct rjd_result result = rjd_resource_load(&lib, id_texture, &handle_texture);

            enum rjd_resource_status status = rjd_resource_lib_status(&lib, handle_texture);
            expect_int32(RJD_RESOURCE_STATUS_READY, status); // should have been loaded by the material

            expect_result_ok(result);
            expect_true(did_call_stages_texture.begin);
            expect_false(did_call_stages_texture.end); // doesn't have a load end function
            expect_false(did_call_stages_texture.unload);
            verify_texture_data((struct test_texture*)rjd_resource_get(&lib, handle_texture));
        }

        // unload material
        {
            rjd_resource_unload(&lib, handle_material);

            enum rjd_resource_status status = rjd_resource_lib_status(&lib, handle_material);
            expect_uint32(RJD_RESOURCE_STATUS_READY, status);

            rjd_resource_lib_waitall(&lib);

            status = rjd_resource_lib_status(&lib, handle_material);
            expect_uint32(RJD_RESOURCE_STATUS_INVALID, status);
        }

        // unload shader (refcount should be 1 now)
        {
            enum rjd_resource_status status = rjd_resource_lib_status(&lib, handle_shader);
            expect_uint32(RJD_RESOURCE_STATUS_READY, status);

            rjd_resource_unload(&lib, handle_shader);
            rjd_resource_lib_waitall(&lib);

            status = rjd_resource_lib_status(&lib, handle_shader);
            expect_uint32(RJD_RESOURCE_STATUS_INVALID, status);
        }

        // unload texture (refcount should be 1 now)
        {
            enum rjd_resource_status status = rjd_resource_lib_status(&lib, handle_texture);
            expect_uint32(RJD_RESOURCE_STATUS_READY, status);

            rjd_resource_unload(&lib, handle_texture);
            rjd_resource_lib_waitall(&lib);

            status = rjd_resource_lib_status(&lib, handle_texture);
            expect_uint32(RJD_RESOURCE_STATUS_INVALID, status);
        }

        struct rjd_result result = rjd_resource_lib_destroy(&lib);
        expect_result_ok(result);
    }
    rjd_strhash_global_destroy();

	expect_no_leaks(&allocator);
}

int RJD_COMPILER_MSVC_ONLY(__cdecl) main(void) 
{
	test_logging();
	test_result();
	test_enum();
	test_hash();
	test_mem();
	test_rng();
	test_array();
	test_math();
	test_procgeo();
	test_easing();
	test_strbuf();
	test_profiler();
	test_cmd();
	test_dict();
	test_thread();
	test_atomic();
	test_fio();
	test_strpool();
	test_slotmap();
	test_utf8();
	test_path();
	test_stream();
	test_binrw();
	test_strhash();
	test_resource();

	return 0;
}
