#include <math.h>
#define RJD_IMPL true
#define RJD_ENABLE_ASSERT true
#define RJD_ENABLE_LOGGING true
#define RJD_ENABLE_SHORTNAMES true
#define RJD_STRBUF_STATIC_SIZE 32
#include "rjd_all.h"

////////////////////////////////////////////////////////////////////////////////
// expect utils

#define expect_true(condition) if (!(condition)) { ASSERTFAIL("Expected \"%s\" to be true, but got false\n", #condition); }
#define expect_false(condition) if (condition) { ASSERTFAIL("Expected \"%s\" to be false, but got true\n", #condition); }

void expect_str(const char* expected, const char* actual)
{
	if (expected != actual && (expected == NULL || actual == NULL || strcmp(expected, actual))) {
		ASSERTFAIL("Expected:\n%s\nbut got:\n%s\n", expected ? expected : "", actual ? actual : "");
	}
}

void expect_float(double expected, double actual)
{
	if (fabs(expected - actual) > 0.00001) {
		ASSERTFAIL("Expected: %f, but got: %f", expected, actual);
	}
}

#define DEFINE_EXPECT_INTTYPE(type, format_specifier)															\
	void expect_##type(type##_t expected, type##_t actual) {													\
		if (expected != actual) {																				\
			ASSERTFAIL("Expected: " format_specifier ", but got: " format_specifier "\n", expected, actual);	\
		}																										\
	}

DEFINE_EXPECT_INTTYPE(int32,  "%d")
DEFINE_EXPECT_INTTYPE(int64,  "%lld")
DEFINE_EXPECT_INTTYPE(uint32, "%u")
DEFINE_EXPECT_INTTYPE(uint64, "%llu")

////////////////////////////////////////////////////////////////////////////////

RJD_STATIC_ASSERT(true);
RJD_STATIC_ASSERT(1 == 1);
RJD_STATIC_ASSERT(sizeof(uint32_t) == sizeof(char) * 4);

////////////////////////////////////////////////////////////////////////////////

const char* test = __FILE__;

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
		"tests.c(72): test\n"
		"tests.c(73): \n"
		"tests.c(74): forma11ed!\n"
		"tests.c(83): ok1\n"
		"tests.c(88): ok2\n"
		"tests.c(89): ok2\n"
		"tests.c(93): ok3\n"
		"tests.c(94): ok3\n"
		"tests.c(95): ok3\n"
		"tests.c(107): other channel\n";

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

	// TODO linear allocator
	{
		//char stackmem[1024 * 1024];
	}
}

void test_array()
{
	struct rjd_alloc_context context = alloc_initdefault();

	// countof
	{
		int a[3];
		expect_uint32(3, countof(a));

		unsigned long long b[64];
		expect_uint32(64, countof(b));

		struct { uint32_t a[4]; const char* b; unsigned c[20]; } c[20];
		expect_uint32(20, countof(c));
	}

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
			struct test v = { (int)i, 0, 0, 0 };
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

		int32_t two = 2;
		bool has2 = arr_contains(b, two);
		expect_true(has2);

		int32_t ten = 10;
		bool has10 = arr_contains(b, ten);
		expect_false(has10);

		arr_free(b);
	}

	// rng tests
	{
		struct rjd_rng rng = rng_init(0x1337C0DE);

		int32_t* a = arr_alloc(int32_t, 8, &context);
		for (int32_t i = 0; i < 8; ++i) {
			arr_push(a, i);
		}

		expect_int32(5, arr_sample(a, &rng));

		arr_shuffle(a, &rng);
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

void test_strbuf(void)
{	
	struct rjd_alloc_context context = alloc_initdefault();

	struct rjd_strbuf builder = rjd_strbuf_init(&context);

	rjd_strbuf_append(&builder, "test");
	expect_str("test", rjd_strbuf_str(&builder));

	rjd_strbuf_append(&builder, "123");
	rjd_strbuf_append(&builder, "123");
	expect_str("test123123", rjd_strbuf_str(&builder));

	rjd_strbuf_free(&builder);

	rjd_strbuf_append(&builder, "forma%d%ded", 1, 1);
	expect_str("forma11ed", rjd_strbuf_str(&builder));

	rjd_strbuf_append(&builder, "1234567890123456789012345678901234567890");

	expect_str("forma11ed1234567890123456789012345678901234567890", rjd_strbuf_str(&builder));

	rjd_strbuf_free(&builder);
	rjd_strbuf_append(&builder, "12345678901234567890123456789010");
	expect_str("12345678901234567890123456789010", rjd_strbuf_str(&builder));

	rjd_strbuf_free(&builder);
}

void test_profiler(void)
{
	PROFILE_SCOPE(Test1, {
		double d = 1;
		for (size_t i = 0; i < 1000; ++i) {
			d += d;
		}
		expect_true(d > 0);
	});
}

void test_cmd()
{
	struct rjd_alloc_context context = alloc_initdefault();
	
	const char* argv0[] = { "test.exe", NULL };
	struct rjd_cmd cmd = cmd_init(countof(argv0), argv0, &context);

	expect_true(cmd_ok(&cmd));

	cmd_add_req(&cmd, "PATTERN", "The regex pattern to search for.");
	cmd_add_req(&cmd, "FILE", "File in which to search for the pattern.");
	cmd_add_opt(&cmd, "-c", "--color", NULL, "Enable colorized output");
	cmd_add_opt(&cmd, "-w", "--wordsonly", NULL, "Force matching of whole words only.");
	cmd_add_opt(&cmd, "-z", "--zfile", "ZFILE", "Metafile.");

	expect_false(cmd_ok(&cmd));

	const char* argv1[] = { "a.exe", "mypattern", "file.txt", NULL };
	cmd.argc = countof(argv1);
	cmd.argv = argv1;
	expect_true(cmd_ok(&cmd));
	expect_str("mypattern", cmd_str(&cmd, "PATTERN"));
	expect_str("file.txt", cmd_str(&cmd, "FILE"));

	const char* argv2[] = { "a.exe", "-c", "-w", "mypattern", "file.txt", NULL };
	cmd.argc = countof(argv2);
	cmd.argv = argv2;
	expect_true(cmd_ok(&cmd));
	expect_true(cmd_bool(&cmd, "-c"));
	expect_true(cmd_bool(&cmd, "-w"));
	expect_false(cmd_bool(&cmd, "-z"));
	expect_str("mypattern", cmd_str(&cmd, "PATTERN"));
	expect_str("file.txt", cmd_str(&cmd, "FILE"));

	const char* argv3[] = { "a.exe", "-z", "meta.txt", "-w", "--color", "mypattern", "file.txt", NULL };
	cmd.argc = countof(argv3);
	cmd.argv = argv3;
	expect_true(cmd_ok(&cmd));
	expect_true(cmd_bool(&cmd, "-z"));
	expect_true(cmd_bool(&cmd, "-w"));
	expect_true(cmd_bool(&cmd, "-c"));
	expect_str("meta.txt", cmd_str(&cmd, "-z"));
	expect_str("mypattern", cmd_str(&cmd, "PATTERN"));
	expect_str("file.txt", cmd_str(&cmd, "FILE"));

	const char* argv4[] = { "a.exe", "-z", "1337", "mypattern", "file.txt", NULL };
	cmd.argc = countof(argv4);
	cmd.argv = argv4;
	expect_true(cmd_ok(&cmd));
	expect_int32(1337, cmd_int(&cmd, "-z", 0));
	expect_float(1337, cmd_int(&cmd, "-z", 0));
	expect_str("mypattern", cmd_str(&cmd, "PATTERN"));
	expect_str("file.txt", cmd_str(&cmd, "FILE"));

	cmd_usage(&cmd);
	cmd_help(&cmd);

	cmd_free(&cmd);
}

void test_rng()
{
	struct rjd_rng rng = rng_init(0xF7A290B49);
	expect_int64(11675382715271363104ull, rng_next(&rng));
	expect_int64(15127709716525989220ull, rng_next(&rng));
	expect_int64(6059634119111723720ull, rng_next(&rng));
	expect_int32(41, rng_range32(&rng, 0, 100));
	expect_int32(67, rng_range32(&rng, 0, 100));
	expect_int32(87, rng_range32(&rng, 0, 100));
	expect_int32(80, rng_range32(&rng, 0, 100));
	expect_float(0.619404, rng_float(&rng));
	expect_float(0.349893, rng_float(&rng));
	expect_float(0.877108, rng_float(&rng));
	expect_float(0.933046, rng_float(&rng));

	rng = rng_init(0xF7A290B49);
	expect_int64(11675382715271363104ull, rng_next(&rng));
	expect_int64(15127709716525989220ull, rng_next(&rng));
}

void test_dict()
{
	struct rjd_alloc_context context = alloc_initdefault();
	
	struct rjd_dict dict = dict_init(&context);
	expect_str(NULL, (char*)dict_get(&dict, "key"));
	expect_str(NULL, (char*)dict_erase(&dict, "key"));

	dict_insert(&dict, "key1", "ok1");
	dict_insert(&dict, "key2", "ok2");
	dict_insert(&dict, "key3", "ok3");
	dict_insert(&dict, "key4", "ok4");
	dict_insert(&dict, "key5", NULL);

	expect_str("ok1", (const char*)dict_get(&dict, "key1"));
	expect_str("ok2", (const char*)dict_get(&dict, "key2"));
	expect_str("ok3", (const char*)dict_get(&dict, "key3"));
	expect_str("ok4", (const char*)dict_get(&dict, "key4"));
	expect_str(NULL,  (const char*)dict_get(&dict, "key5"));

	expect_str("ok2", (const char*)dict_erase(&dict, "key2"));
	expect_str(NULL,  (const char*)dict_get(&dict, "key2"));

	int32_t data[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};

	for (size_t i = 0; i < countof(data); ++i) {
		char key[32] = {0};
		sprintf(key, "k%d", i);
		dict_insert(&dict, key, data + i);
	}

	for (size_t i = 0; i < countof(data); ++i) {
		char key[32] = {0};
		sprintf(key, "k%d", i);
		expect_int32(i, *(int32_t*)dict_get(&dict, key));
	}

	dict_free(&dict);
}

int main(void) 
{
	test_logging();
	test_alloc();
	test_array();
	test_strbuf();
	test_profiler();
	test_cmd();
	test_rng();
	test_dict();

	return 0;
}

