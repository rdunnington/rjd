#include <math.h>
#define RJD_IMPL true
#define RJD_ENABLE_ASSERT true
#define RJD_ENABLE_LOGGING true
#define RJD_STRBUF_STATIC_SIZE 32
#include "rjd_all.h"

////////////////////////////////////////////////////////////////////////////////
// expect utils

#define expect_true(condition) if (!(condition)) { RJD_ASSERTFAIL("Expected \"%s\" to be true, but got false\n", #condition); }
#define expect_false(condition) if (condition) { RJD_ASSERTFAIL("Expected \"%s\" to be false, but got true\n", #condition); }

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

void expect_int32(int32_t expected, int32_t actual) {
	if (expected != actual) {
		RJD_ASSERTFAIL("Expected: %d, but got: %d\n", expected, actual);
	}
}

void expect_int64(int64_t expected, int64_t actual) {
	if (expected != actual) {
		RJD_ASSERTFAIL("Expected: %lld, but got: %lld\n", expected, actual);
	}
}

void expect_uint32(uint32_t expected, uint32_t actual) {
	if (expected != actual) {
		RJD_ASSERTFAIL("Expected: %u, but got: %u\n", expected, actual);
	}
}

void expect_uint64(uint64_t expected, uint64_t actual) {
	if (expected != actual) {
		RJD_ASSERTFAIL("Expected: %llu, but got: %llu\n", expected, actual);
	}
}

////////////////////////////////////////////////////////////////////////////////

static int compare_int32(const void* a, const void* b)
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

static int compare_int32_c(void* context, const void* a, const void* b)
{
	RJD_ASSERT(context);
	return compare_int32(a, b);
}

////////////////////////////////////////////////////////////////////////////////

RJD_STATIC_ASSERT(true);
RJD_STATIC_ASSERT(1 == 1);
RJD_STATIC_ASSERT(sizeof(uint32_t) == sizeof(char) * 4);

////////////////////////////////////////////////////////////////////////////////

char logbuffer[1024 * 128];
size_t logbuffer_pos = 0;
void test_log_hook(const char* msg, size_t length)
{
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

	expect_str(expected, logbuffer);
	
	// reset global logger for future tests
	rjd_log_resetglobal();
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
	const uint8_t* data1 = (const uint8_t*)"test1";
	const uint8_t* data2 = (const uint8_t*)"a longer string that has a bunch of characters in it!!$%^&*(";
	const uint8_t* data3 = NULL;

	expect_uint32(rjd_hash32_data(data1, (uint32_t)strlen((const char*)data1)).value, rjd_hash32_data(data1, -1).value);
	expect_uint32(rjd_hash32_data(data2, (uint32_t)strlen((const char*)data2)).value, rjd_hash32_data(data2, -1).value);
	expect_uint32(rjd_hash32_data(data3, 0).value, rjd_hash32_data(data3, -1).value);

	expect_uint64(rjd_hash64_data(data1, (uint32_t)strlen((const char*)data1)).value, rjd_hash64_data(data1, -1).value);
	expect_uint64(rjd_hash64_data(data2, (uint32_t)strlen((const char*)data2)).value, rjd_hash64_data(data2, -1).value);
	expect_uint64(rjd_hash64_data(data3, 0).value, rjd_hash64_data(data3, -1).value);
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
		struct rjd_mem_allocator a = rjd_mem_allocator_init_default();
		int32_t* p0 = rjd_mem_alloc(int32_t, &a);
		expect_true(p0 != NULL);
		*p0 = 1337;

		char* p1 = rjd_mem_alloc_array(char, 128, &a);
		expect_true(p1 != NULL);
		strncpy(p1, "thequickbrownfoxjumpedoverthesuperdeduperlazydog!", 128);
		p1[127] = 0;

		char* p2 = rjd_mem_alloc_array(char, 64, &a);
		expect_true(p2 != NULL);
		strncpy(p2, "this fox wasn't as quick as the last one", 64);
		p2[63] = 0;

		char* p3 = rjd_mem_alloc_aligned(char, &a, 64);
		expect_true(p3 != NULL);
		expect_uint64(RJD_MEM_ALIGN((uint64_t)p3, 64), (uint64_t)p3);

		struct aligned_struct {
			double a;
			double b;
			double c;
			double e;
		};
		char* p4 = rjd_mem_alloc_array_aligned(char, 8, &a, 32);
		expect_true(p4 != NULL);
		expect_uint64(RJD_MEM_ALIGN((uint64_t)p4, 32), (uint64_t)p4);

		rjd_mem_free(p0);
		rjd_mem_free(p1);
		rjd_mem_free(p2);
		rjd_mem_free(p3);
		rjd_mem_free(p4);

		expect_false(rjd_mem_allocator_reset(&a));
	}

	// linear allocator
	{
		char stackmem[1024];
		struct rjd_mem_allocator a = rjd_mem_allocator_init_linear(stackmem, sizeof(stackmem));

		expect_true(rjd_mem_allocator_type(&a) != NULL);

		expect_true(rjd_mem_alloc_array(char, 256, &a) != NULL);
		expect_true(rjd_mem_alloc_array(char, 256, &a) != NULL);
		expect_true(rjd_mem_alloc_array(char, 256, &a) != NULL);

		expect_true(a.stats.total_size <= sizeof(stackmem));
		{
			const uint32_t total = a.stats.total_size;
			expect_uint32(a.stats.current.used - a.stats.current.overhead, 256 * 3);
			expect_uint32(a.stats.current.peak, a.stats.current.used);
			expect_uint32(a.stats.current.unused, total - ((256 * 3) + a.stats.current.overhead));
			expect_uint32(a.stats.current.allocs, 3);
			expect_uint32(a.stats.current.frees, 0);

			expect_uint32(a.stats.lifetime.peak, a.stats.current.peak);
			expect_uint32(a.stats.lifetime.allocs, 3);
			expect_uint32(a.stats.lifetime.frees, 0);
			expect_uint32(a.stats.lifetime.resets, 0);
		}

        const uint32_t old_peak = a.stats.lifetime.peak;
		expect_true(rjd_mem_allocator_reset(&a));

		expect_true(rjd_mem_alloc_array(char, 512, &a) != NULL);
		{
			const uint32_t total = a.stats.total_size;
			expect_uint32(a.stats.current.used - a.stats.current.overhead, 512);
			expect_uint32(a.stats.current.peak, a.stats.current.used);
			expect_uint32(a.stats.current.unused, total - (512 + a.stats.current.overhead));
			expect_uint32(a.stats.current.allocs, 1);
			expect_uint32(a.stats.current.frees, 0);

			expect_uint32(a.stats.lifetime.peak, old_peak);
			expect_uint32(a.stats.lifetime.allocs, 4);
			expect_uint32(a.stats.lifetime.frees, 0);
			expect_uint32(a.stats.lifetime.resets, 1);
		}

		expect_true(rjd_mem_allocator_reset(&a));
		expect_uint32(a.stats.lifetime.resets, 2);
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
			expect_uint32(i, rjd_array_get(a, i).a);
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
	}

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
	}
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

void expect_float2(rjd_math_float2 expected, rjd_math_float2 actual)
{
	if (!rjd_math_isequalf(expected.x, actual.x) ||
		!rjd_math_isequalf(expected.y, actual.y))
	{
		RJD_ASSERTFAIL("Expected (%.2f, %.2f), but got: (%.2f, %.2f)", 
			expected.x, expected.y, actual.x, actual.y);
	}
}

void expect_float3(rjd_math_float3 expected, rjd_math_float3 actual)
{
	if (!rjd_math_isequalf(expected.x, actual.x) ||
		!rjd_math_isequalf(expected.y, actual.y) ||
		!rjd_math_isequalf(expected.z, actual.z))
	{
		RJD_ASSERTFAIL("Expected (%.2f, %.2f, %.2f), but got: (%.2f, %.2f, %.2f)", 
			expected.x, expected.y, expected.z, actual.x, actual.y, actual.z);
	}
}

void expect_float4(rjd_math_float4 expected, rjd_math_float4 actual)
{
	if (!rjd_math_isequalf(expected.x, actual.x) ||
		!rjd_math_isequalf(expected.y, actual.y) ||
		!rjd_math_isequalf(expected.z, actual.z) ||
		!rjd_math_isequalf(expected.w, actual.w))
	{
		RJD_ASSERTFAIL("Expected (%.2f, %.2f, %.2f, %.2f), but got: (%.2f, %.2f, %.2f, %.2f)", 
			expected.x, expected.y, expected.z, expected.w, actual.x, actual.y, actual.z, actual.w);
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

	// float <-> vec translations
	{
		rjd_math_float2 f2 = rjd_math_float2_xy(78, 99);
		rjd_math_float3 f3 = rjd_math_float3_xyz(12, 32, 11);
		rjd_math_float4 f4 = rjd_math_float4_xyzw(60, 56, 77, 28);

		expect_vec3(rjd_math_vec3_xyz(78, 99, 4), rjd_math_float2_to_vec3(f2, 4));
		expect_vec3(rjd_math_vec3_xyz(12, 32, 11), rjd_math_float3_to_vec3(f3));
		expect_vec3(rjd_math_vec3_xyz(60, 56, 77), rjd_math_float4_to_vec3(f4));
		
		expect_vec4(rjd_math_vec4_xyzw(78, 99, 55, 44), rjd_math_float2_to_vec4(f2, 55, 44));
		expect_vec4(rjd_math_vec4_xyzw(12, 32, 11, 55), rjd_math_float3_to_vec4(f3, 55));
		expect_vec4(rjd_math_vec4_xyzw(60, 56, 77, 28), rjd_math_float4_to_vec4(f4));
		
		expect_float2(f2, rjd_math_vec3_to_float2(rjd_math_vec3_xyz(78, 99, 23)));
		expect_float3(f3, rjd_math_vec3_to_float3(rjd_math_vec3_xyz(12, 32, 11)));
		expect_float4(f4, rjd_math_vec3_to_float4(rjd_math_vec3_xyz(60, 56, 77), 28));
		
		expect_float2(f2, rjd_math_vec4_to_float2(rjd_math_vec4_xyzw(78, 99, 23, 87)));
		expect_float3(f3, rjd_math_vec4_to_float3(rjd_math_vec4_xyzw(12, 32, 11, 87)));
		expect_float4(f4, rjd_math_vec4_to_float4(rjd_math_vec4_xyzw(60, 56, 77, 28)));
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
	expect_vec4(rjd_math_vec4_xyzw(26, 60, 44, 6), rjd_math_vec4_scale(rjd_math_vec4_xyzw(13, 30, 22, 3), 2));
	expect_vec4(rjd_math_vec4_xyzw(3,3,3,3), rjd_math_vec4_add(rjd_math_vec4_xyzw(1,1,1,1), rjd_math_vec4_xyzw(2,2,2,2)));
	expect_vec4(rjd_math_vec4_xyzw(-1,-1,-1,-1), rjd_math_vec4_sub(rjd_math_vec4_xyzw(1,1,1,1), rjd_math_vec4_xyzw(2,2,2,2)));
	expect_vec4(rjd_math_vec4_xyzw(2,2,2,2), rjd_math_vec4_mul(rjd_math_vec4_xyzw(1,1,1,1), rjd_math_vec4_xyzw(2,2,2,2)));
	expect_vec4(rjd_math_vec4_xyzw(.5,.5,.5,.5), rjd_math_vec4_div(rjd_math_vec4_xyzw(1,1,1,1), rjd_math_vec4_xyzw(2,2,2,2)));
	expect_vec4(rjd_math_vec4_xyzw(23,45,21,5), rjd_math_vec4_min(rjd_math_vec4_xyzw(23,45,72,5), rjd_math_vec4_xyzw(43,75,21,6)));
	expect_vec4(rjd_math_vec4_xyzw(43,75,72,6), rjd_math_vec4_max(rjd_math_vec4_xyzw(23,45,72,5), rjd_math_vec4_xyzw(43,75,21,6)));
	expect_vec4(rjd_math_vec4_xyzw(1,0,0,0), rjd_math_vec4_project(rjd_math_vec4_xyzw(1,1,1,1), rjd_math_vec4_xyzw(1,0,0,0)));
	expect_vec4(rjd_math_vec4_xyzw(1,2,4,8), rjd_math_vec4_lerp(rjd_math_vec4_zero(), rjd_math_vec4_xyzw(2, 4, 8, 16), .5));

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

	// point-rjd_geo_point_rect
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

	// rjd_geo_point_rect-rjd_geo_point_rect
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

	// rjd_geo_point_rect-rect
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
	struct rjd_mem_allocator context = rjd_mem_allocator_init_default();

	struct rjd_strbuf builder = rjd_strbuf_init(&context);

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
}

void test_profiler(void)
{
	RJD_PROFILE_SCOPE(Test1, {
		double d = 1;
		for (size_t i = 0; i < 1000; ++i) {
			d += d;
		}
		expect_true(d > 0);
	});
}

void test_cmd()
{
	struct rjd_mem_allocator context = rjd_mem_allocator_init_default();
	
	const char* argv0[] = { "test.exe", NULL };
	struct rjd_cmd cmd = rjd_cmd_init(rjd_countof(argv0), argv0, &context);

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
	rjd_cmd_help(&cmd);

	rjd_cmd_free(&cmd);
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
	struct rjd_mem_allocator context = rjd_mem_allocator_init_default();
	
	struct rjd_dict dict = rjd_dict_init(&context, 0);
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
	struct rjd_mem_allocator context = rjd_mem_allocator_init_default();

	const char expected_contents[] = "this is a test file that has a utf-8 character!";

	struct rjd_result result;

	result = rjd_fio_write("test.txt", expected_contents, sizeof(expected_contents), RJD_FIO_WRITEMODE_REPLACE);
	expect_fio_ok(true, result);

	char* buffer;
	result = rjd_fio_read("does_not_exist.txt", &buffer, &context);
	expect_fio_ok(false, result);
	result = rjd_fio_read("test.txt", &buffer, &context);
	expect_fio_ok(true, result);
	expect_uint32(sizeof(expected_contents), rjd_array_count(buffer));
	expect_true(!memcmp(buffer, expected_contents, sizeof(expected_contents)));

	result = rjd_fio_write("test2.txt", expected_contents, sizeof(expected_contents), RJD_FIO_WRITEMODE_REPLACE);
	expect_fio_ok(true, result);
	result = rjd_fio_write("test2.txt", expected_contents, sizeof(expected_contents), RJD_FIO_WRITEMODE_APPEND);
	char* buffer2;
	result = rjd_fio_read("test2.txt", &buffer2, &context);
	expect_fio_ok(true, result);
	expect_uint32(sizeof(expected_contents)*2, rjd_array_count(buffer2));

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

	expect_str(ref1->str, test1);
	expect_str(ref2->str, test2);
	expect_str(ref3->str, test3);
	expect_str(ref4->str, test4);
	expect_str(ref5->str, test5);

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

	expect_str(ref1->str, test1);
	expect_str(ref2->str, test2);
	expect_str(ref3->str, test3);
	expect_str(ref4->str, test4);
	expect_str(ref5->str, test5);

	rjd_strpool_free(&pool);
}

void test_slotmap(void)
{
	struct rjd_mem_allocator allocator = rjd_mem_allocator_init_default();

	{
		uint32_t* map = rjd_slotmap_alloc(uint32_t, 8, &allocator);

		expect_uint32(8, rjd_slotmap_count(map));

		struct rjd_slot slots[25] = {0};
		for (uint32_t i = 0; i < rjd_countof(slots); ++i) {
			rjd_slotmap_insert(map, i + 1337, slots + i);
		}

		expect_uint32(32, rjd_slotmap_count(map));

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

		visited = 0;
		for (struct rjd_slot s = rjd_slotmap_next(map, NULL); rjd_slot_isvalid(s); s = rjd_slotmap_next(map, &s))
		{
			++visited;
		}
		expect_uint32(20, visited);

		rjd_slotmap_free(map);
	}
}

void test_path(void)
{
	expect_str(NULL,   rjd_path_extension(NULL));
	expect_str(NULL,   rjd_path_extension(""));
	expect_str(NULL,   rjd_path_extension("no_extension"));
	expect_str(NULL,   rjd_path_extension("not_even_this_one."));
	expect_str(".txt", rjd_path_extension(".txt"));
	expect_str(".txt", rjd_path_extension("some_file_name.txt"));
	expect_str(".txt", rjd_path_extension("some/path/some_file.txt"));
	expect_str(".txt", rjd_path_extension("some\\path\\some_file.txt"));
	expect_str(".txt", rjd_path_extension("some\\path\\some.long.extension.txt"));
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
	test_easing();
	test_strbuf();
	test_profiler();
	test_cmd();
	test_dict();
	test_fio();
	test_strpool();
	test_slotmap();
	test_path();

	return 0;
}

