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

#define TEST_ENUM1_LIST(macro)	\
	macro(e1_a)					\
	macro(e1_b)					\
	macro(e1_c)					\
	macro(e1_d)

#define TEST_ENUM2_LIST(macro)	\
	macro(e2_ok, "OK")			\
	macro(e2_notok, "NOTOK")

ENUM_DECLARE(e1, TEST_ENUM1_LIST)
ENUM_DEFINE(e1, TEST_ENUM1_LIST)

ENUM_DECLARE_WITH_STRINGS(e2, TEST_ENUM2_LIST)
ENUM_DEFINE_WITH_STRINGS(e2, TEST_ENUM2_LIST)

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
		struct test* a = array_alloc(struct test, 32, &context);
		expect_uint32(array_count(a), 0);
		expect_uint32(array_capacity(a), 32);
		expect_true(array_empty(a));
		expect_false(array_full(a));

		array_resize(a, 16);
		expect_uint32(16, array_count(a));
		expect_uint32(32, array_capacity(a));

		array_resize(a, 50);
		expect_uint32(50, array_count(a));
		expect_uint32(50, array_capacity(a));

		for (size_t i = 0; i < array_count(a); ++i) {
			struct test v = { (int)i, 0, 0, 0 };
			a[i] = v;
		}
		expect_false(array_empty(a));
		expect_true(array_full(a));

		array_erase(a, 0);
		expect_int32(1, a[0].a);
		array_erase(a, 1);
		expect_int32(3, a[1].a);

		expect_false(array_empty(a));
		expect_false(array_full(a));

		struct test end = array_pop(a);
		expect_int32(49, end.a);

		array_resize(a, 0);
		expect_true(array_empty(a));
		expect_uint32(0, array_count(a));
		expect_uint32(50, array_capacity(a));

		array_push(a, end);
		expect_int32(end.a, a[0].a);
		
		array_clear(a);
		expect_int32(0, array_count(a));
		expect_int32(50, array_capacity(a));

		array_free(a);
	}

	// first/last
	{
		int32_t* a = array_alloc(int32_t, 16, &context);
		for (int32_t i = 0; i < (int32_t)array_capacity(a); ++i) {
			array_push(a, 0xD00D + i);
		}

		int32_t* b = array_alloc(int32_t, 16, &context);

		int32_t first = array_first(b, 1337);
		expect_int32(1337, first);
		first = array_first(a, 0);
		expect_int32(0xD00D + 0, first);

		int32_t last = array_last(b, 1337);
		expect_int32(1337, last);
		last = array_last(a, 0);
		expect_int32(0xD00D + 15, last);

		array_free(a);
		array_free(b);
	}

	// functional-style tests
	{
		int32_t* b = array_alloc(int32_t, 16, &context);
		for (int32_t i = 0; i < 16; ++i) {
			array_push(b, i);
		}

		#define testfilter(element) (element < 8)
		array_filter(b, testfilter);
		expect_uint32(8, array_count(b));

		#define testsum(acc, element) (acc + element)
		int32_t sum1 = 0;
		array_reduce(b, sum1, testsum);
		expect_int32(1 + 2 + 3 + 4 + 5 + 6 + 7, sum1);

		int32_t sum2 = 0;
		array_sum(b, sum2);
		expect_int32(sum1, sum2);

		int32_t two = 2;
		bool has2 = array_contains(b, two);
		expect_true(has2);

		int32_t ten = 10;
		bool has10 = array_contains(b, ten);
		expect_false(has10);

		array_free(b);
	}

	// rng tests
	{
		struct rjd_rng rng = rng_init(0x1337C0DE);

		int32_t* a = array_alloc(int32_t, 8, &context);
		for (int32_t i = 0; i < 8; ++i) {
			array_push(a, i);
		}

		expect_int32(5, array_sample(a, &rng));

		array_shuffle(a, &rng);
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

void expect_vec4(vec4 expected, vec4 actual) 
{
	if (!vec4_eq(expected, actual)) {
		ASSERTFAIL("Expected (%.2f, %.2f, %.2f, %.2f), but got: (%.2f, %.2f, %.2f, %.2f)", 
			vec4_x(expected), vec4_y(expected), vec4_z(expected), vec4_w(expected),
			vec4_x(actual), vec4_y(actual), vec4_z(actual), vec4_w(actual));
	}
}


void expect_vec3(vec3 expected, vec3 actual) 
{
	if (!vec3_eq(expected, actual)) {
		ASSERTFAIL("Expected (%.2f, %.2f, %.2f), but got: (%.2f, %.2f, %.2f)", 
			vec3_x(expected), vec3_y(expected), vec3_z(expected), 
			vec3_x(actual), vec3_y(actual), vec3_z(actual));
	}
}

void test_math(void)
{
	// helper functions
	expect_uint32(0, next_pow2(0));
	expect_uint32(1, next_pow2(1));
	expect_uint32(2, next_pow2(2));
	expect_uint32(4, next_pow2(3));
	expect_uint32(4, next_pow2(4));
	expect_uint32(8, next_pow2(5));
	expect_uint32(8, next_pow2(6));
	expect_uint32(8, next_pow2(7));
	expect_uint32(8, next_pow2(8));

	expect_int32(1, pow32(2, 0));
	expect_int32(2, pow32(2, 1));
	expect_int32(64, pow32(2, 6));
	expect_int32(-32, pow32(-2, 5));
	expect_int32(1, pow32(23987, 0));
	expect_int32(23987, pow32(23987, 1));

	expect_int32(1, sign32(0));
	expect_int32(1, sign32(67));
	expect_int32(-1, sign32(-231));
	expect_float(1, sign(0.0));
	expect_float(1, sign(-0.0));
	expect_float(1, sign(234.0));
	expect_float(-1, sign(-234.0));
	expect_float(1, (double)signf(234.0f));
	expect_float(-1, (double)signf(-234.0f));

	expect_true(isequal(0, 0));
	expect_true(isequal(505, 505));
	expect_true(isequal(0.00000001, 0.00000002));
	expect_true(isequal(0.00000001f, 0.00000002f));
	expect_false(isequal(1, 2));
	expect_false(isequal(1/3.0, 1/3.1));

	expect_float(0, remap(0, 0, 1, 0, 2));
	expect_float(1, remap(.5, 0, 1, 0, 2));
	expect_float(2, remap(1, 0, 1, 0, 2));
	
	expect_int32(1, min32(2, 1));
	expect_int32(-1, min32(-1, 1));
	expect_int32(10, minu32(10, 1100));
	expect_int32(1, max32(-1, 1));
	expect_int32(2, max32(2, 1));
	expect_int32(1100, maxu32(10, 1100));

	// vec4
	expect_true(vec4_eq(vec4_zero(), vec4_zero()));
	expect_true(vec4_eq(vec4_xyzw(1,2,3,4), vec4_xyzw(1,2,3,4)));
	expect_vec4(vec4_zero(), vec4_xyzw(0,0,0,0));
	expect_vec4(vec4_xyzw(1,1,0,1), vec4_add(vec4_xyzw(1,0,0,0), vec4_xyzw(0,1,0,1)));
	expect_float(32, vec4_x(vec4_splat(32)));
	expect_float(128, vec4_y(vec4_splat(128)));
	expect_float(777, vec4_z(vec4_splat(777)));
	expect_float(987234, vec4_w(vec4_splat(987234)));
	expect_vec4(vec4_xyzw(7, 7, 9, 20), vec4_shuffle(vec4_xyzw(9,0,7,20), 2, 2, 0, 3));
	expect_float(8, vec4_x(vec4_setx(vec4_zero(), 8)));
	expect_float(8, vec4_y(vec4_sety(vec4_zero(), 8)));
	expect_float(8, vec4_z(vec4_setz(vec4_zero(), 8)));
	expect_float(8, vec4_w(vec4_setw(vec4_zero(), 8)));
	expect_float(8, vec4_sum(vec4_xyzw(2, 2, 2, 2)));
	expect_float(2, vec4_dot(vec4_xyzw(0,1,0,0), vec4_xyzw(0,2,0,0)));
	expect_float(0, vec4_dot(vec4_xyzw(0,1,0,0), vec4_xyzw(1,0,0,0)));
	expect_float(16, vec4_lengthsq(vec4_xyzw(4,0,0,0)));
	expect_float(4, vec4_length(vec4_xyzw(4,0,0,0)));
	expect_float(5, vec4_length(vec4_xyzw(3,0,0,4)));
	expect_vec4(vec4_xyzw(1,0,0,0), vec4_normalize(vec4_xyzw(7368,0,0,0)));
	expect_vec4(vec4_xyzw(26, 60, 44, 6), vec4_scale(vec4_xyzw(13, 30, 22, 3), 2));
	expect_vec4(vec4_xyzw(3,3,3,3), vec4_add(vec4_xyzw(1,1,1,1), vec4_xyzw(2,2,2,2)));
	expect_vec4(vec4_xyzw(-1,-1,-1,-1), vec4_sub(vec4_xyzw(1,1,1,1), vec4_xyzw(2,2,2,2)));
	expect_vec4(vec4_xyzw(2,2,2,2), vec4_mul(vec4_xyzw(1,1,1,1), vec4_xyzw(2,2,2,2)));
	expect_vec4(vec4_xyzw(.5,.5,.5,.5), vec4_div(vec4_xyzw(1,1,1,1), vec4_xyzw(2,2,2,2)));
	expect_vec4(vec4_xyzw(23,45,21,5), vec4_min(vec4_xyzw(23,45,72,5), vec4_xyzw(43,75,21,6)));
	expect_vec4(vec4_xyzw(43,75,72,6), vec4_max(vec4_xyzw(23,45,72,5), vec4_xyzw(43,75,21,6)));
	expect_vec4(vec4_xyzw(1,0,0,0), vec4_project(vec4_xyzw(1,1,1,1), vec4_xyzw(1,0,0,0)));
	expect_vec4(vec4_xyzw(1,2,4,8), vec4_lerp(vec4_zero(), vec4_xyzw(2, 4, 8, 16), .5));

	// vec3
	expect_true(vec3_eq(vec3_left(), vec3_left()));
	expect_vec3(vec3_zero(), vec3_xyz(0,0,0));
	expect_vec3(vec3_up(), vec3_xyz(0,1,0));
	expect_vec3(vec3_back(), vec3_xyz(0,0,-1));
	expect_vec3(vec3_xyz(1, 1, 0), vec3_add(vec3_up(), vec3_right()));
	expect_float(32, vec3_x(vec3_splat(32)));
	expect_float(128, vec3_y(vec3_splat(128)));
	expect_float(777, vec3_z(vec3_splat(777)));
	expect_vec3(vec3_xyz(7, 7, 9), vec3_shuffle(vec3_xyz(9,0,7), 2, 2, 0));
	expect_float(8, vec3_x(vec3_setx(vec3_zero(), 8)));
	expect_float(8, vec3_y(vec3_sety(vec3_zero(), 8)));
	expect_float(8, vec3_z(vec3_setz(vec3_zero(), 8)));
	expect_float(6, vec3_sum(vec3_xyz(2, 2, 2)));
	expect_float(2, vec3_dot(vec3_up(), vec3_xyz(0,2,0)));
	expect_float(0, vec3_dot(vec3_up(), vec3_xyz(1,0,0)));
	expect_float(16, vec3_lengthsq(vec3_xyz(4,0,0)));
	expect_float(4, vec3_length(vec3_xyz(4,0,0)));
	expect_float(5, vec3_length(vec3_xyz(3,4,0)));
	expect_vec3(vec3_xyz(1,0,0), vec3_normalize(vec3_xyz(7368, 0, 0)));
	expect_float(PI/4, vec3_angle(vec3_up(), vec3_xyz(1,1,0)));
	expect_vec3(vec3_xyz(26, 60, 44), vec3_scale(vec3_xyz(13, 30, 22), 2));
	expect_vec3(vec3_xyz(3,3,3), vec3_add(vec3_xyz(1,1,1), vec3_xyz(2,2,2)));
	expect_vec3(vec3_xyz(-1,-1,-1), vec3_sub(vec3_xyz(1,1,1), vec3_xyz(2,2,2)));
	expect_vec3(vec3_xyz(2,2,2), vec3_mul(vec3_xyz(1,1,1), vec3_xyz(2,2,2)));
	expect_vec3(vec3_xyz(.5,.5,.5), vec3_div(vec3_xyz(1,1,1), vec3_xyz(2,2,2)));
	expect_vec3(vec3_left(), vec3_cross(vec3_forward(), vec3_up()));
	expect_vec3(vec3_right(), vec3_cross(vec3_up(), vec3_forward()));
	expect_vec3(vec3_xyz(23, 45, 21), vec3_min(vec3_xyz(23,45,72), vec3_xyz(43, 75, 21)));
	expect_vec3(vec3_xyz(43, 75, 72), vec3_max(vec3_xyz(23,45,72), vec3_xyz(43, 75, 21)));
	expect_vec3(vec3_xyz(1,0,0), vec3_project(vec3_xyz(1,1,1), vec3_xyz(1,0,0)));
	expect_vec3(vec3_xyz(1,1,0), vec3_reflect(vec3_xyz(1,-1,0), vec3_up()));
	expect_vec3(vec3_xyz(1,2,4), vec3_lerp(vec3_zero(), vec3_xyz(2, 4, 8), .5));

	// matrix
}

void test_geo()
{
	// point-rect
	{
		vec3 p1 = vec3_xyz(0,0,0);
		vec3 p2 = vec3_xyz(10,1,0);
		vec3 p3 = vec3_xyz(2,2,10);

		rect r1 = rect_minmax(-1,-1,4,4);
		rect r2 = rect_minmax(0,0,20,1);
		rect r3 = rect_minmax(1,1,2,2);

		expect_true(point_rect(p1, r1));
		expect_true(point_rect(p1, r2));
		expect_false(point_rect(p1, r3));

		expect_false(point_rect(p2, r1));
		expect_true(point_rect(p2, r2));
		expect_false(point_rect(p2, r3));

		expect_true(point_rect(p3, r1));
		expect_false(point_rect(p3, r2));
		expect_true(point_rect(p3, r3));
	}

	// point-box
	{
		vec3 p1 = vec3_xyz(0,0,0);
		vec3 p2 = vec3_xyz(-1,1,0);
		vec3 p3 = vec3_xyz(0,1,-1);
		vec3 p4 = vec3_xyz(1,0,1);

		box b1 = box_minmax(vec3_xyz(-1,-1,-1), vec3_xyz(1,1,1));
		box b2 = box_minmax(vec3_xyz(0,0,0), vec3_xyz(1,1,1));
		box b3 = box_minmax(vec3_xyz(-1,-1,-1), vec3_xyz(0,0,0));
		box b4 = box_minmax(vec3_xyz(-1,0,-1), vec3_xyz(1,1,0));

		expect_true(point_box(p1, b1));
		expect_true(point_box(p1, b2));
		expect_true(point_box(p1, b3));
		expect_true(point_box(p1, b4));

		expect_true(point_box(p2, b1));
		expect_false(point_box(p2, b2));
		expect_false(point_box(p2, b3));
		expect_true(point_box(p2, b4));

		expect_true(point_box(p3, b1));
		expect_false(point_box(p3, b2));
		expect_false(point_box(p3, b3));
		expect_true(point_box(p3, b4));

		expect_true(point_box(p4, b1));
		expect_true(point_box(p4, b2));
		expect_false(point_box(p4, b3));
		expect_false(point_box(p4, b4));
	}

	// point-circle
	{
		vec3 p1 = vec3_xyz(0,0,0);
		vec3 p2 = vec3_xyz(5,1,0);
		vec3 p3 = vec3_xyz(-3,2,0);
		vec3 p4 = vec3_xyz(-2,-2,0);

		circle c1 = circle_xyr(0,0,1);
		circle c2 = circle_xyr(4,0,4);
		circle c3 = circle_xyr(-1,-1,2);

		expect_true(point_circle(p1, c1));
		expect_true(point_circle(p1, c2));
		expect_true(point_circle(p1, c3));

		expect_false(point_circle(p2, c1));
		expect_true(point_circle(p2, c2));
		expect_false(point_circle(p2, c3));

		expect_false(point_circle(p3, c1));
		expect_false(point_circle(p3, c2));
		expect_false(point_circle(p3, c3));

		expect_false(point_circle(p4, c1));
		expect_false(point_circle(p4, c2));
		expect_true(point_circle(p4, c3));
	}

	// point-sphere
	{
		vec3 p1 = vec3_xyz(0,0,0);
		vec3 p2 = vec3_xyz(1,2,3);
		vec3 p3 = vec3_xyz(-2,2,7);
		vec3 p4 = vec3_xyz(1,-2,-3);

		sphere s1 = sphere_xyzr(0,0,0,4);
		sphere s2 = sphere_xyzr(2,2,4,5);
		sphere s3 = sphere_xyzr(-1,-4,0,3);

		expect_true(point_sphere(p1, s1));
		expect_true(point_sphere(p1, s2));
		expect_false(point_sphere(p1, s3));

		expect_true(point_sphere(p2, s1));
		expect_true(point_sphere(p2, s2));
		expect_false(point_sphere(p2, s3));

		expect_false(point_sphere(p3, s1));
		expect_true(point_sphere(p3, s2));
		expect_false(point_sphere(p3, s3));

		expect_true(point_sphere(p4, s1));
		expect_false(point_sphere(p4, s2));
		expect_false(point_sphere(p4, s3));
	}

	// circle-circle
	{
		circle c1 = circle_xyr(0,0,1);
		circle c2 = circle_xyr(-2,-1,2);
		circle c3 = circle_xyr(3,0,5);
		circle c4 = circle_xyr(-4,-1,1);

		expect_true(circle_circle(c1, c1));
		expect_true(circle_circle(c1, c2));
		expect_true(circle_circle(c1, c3));
		expect_false(circle_circle(c1, c4));

		expect_true(circle_circle(c2, c3));
		expect_true(circle_circle(c2, c4));

		expect_false(circle_circle(c3, c4));
	}

	// circle-rect
	{
		circle c1 = circle_xyr(0,0,1);

		rect r1 = rect_minmax(-1,-1,1,1);
		//rect r2 = rect_minmax(.5,0,5,1);

		expect_true(circle_rect(c1, r1));
		//expect_true(circle_rect(c1, r2));
	}

}

void expect_ease(ease_func f, float f1, float f2, float f3)
{
	expect_float(f1, f(0.25f));
	expect_float(f2, f(0.5f));
	expect_float(f3, f(0.75f));
}

void test_easing()
{
	expect_ease(ease_line,        0.250000f,  0.500000f,  0.750000f);
	expect_ease(ease_in_sine,     0.076120f,  0.292893f,  0.617317f);
	expect_ease(ease_in_quad,     0.062500f,  0.250000f,  0.562500f);
	expect_ease(ease_in_cube,     0.015625f,  0.125000f,  0.421875f);
	expect_ease(ease_in_quar,     0.003906f,  0.062500f,  0.316406f);
	expect_ease(ease_in_quin,     0.000977f,  0.031250f,  0.237305f);
	expect_ease(ease_in_expo,     0.005524f,  0.031250f,  0.176777f);
	expect_ease(ease_in_circ,     0.031754f,  0.133975f,  0.338562f);
	expect_ease(ease_in_back,    -0.161152f, -0.375000f, -0.108455f);
	expect_ease(ease_in_elas,    -0.005104f, -0.022097f,  0.067650f);
	expect_ease(ease_in_boun,     0.041136f,  0.281250f,  0.527344f);
	expect_ease(ease_out_sine,    0.382683f,  0.707107f,  0.923880f);
	expect_ease(ease_out_quad,    0.437500f,  0.750000f,  0.937500f);
	expect_ease(ease_out_cube,    0.578125f,  0.875000f,  0.984375f);
	expect_ease(ease_out_quar,    0.683594f,  0.937500f,  0.996094f);
	expect_ease(ease_out_quin,    0.762695f,  0.968750f,  0.999023f);
	expect_ease(ease_out_expo,    0.823223f,  0.968750f,  0.994476f);
	expect_ease(ease_out_circ,    0.661438f,  0.866025f,  0.968246f);
	expect_ease(ease_out_back,    1.108455f,  1.375000f,  1.161152f);
	expect_ease(ease_out_elas,    0.932351f,  1.022097f,  1.005104f);
	expect_ease(ease_out_boun,    0.472656f,  0.718750f,  0.958864f);
	expect_ease(ease_inout_sine,  0.146447f,  0.500000f,  0.853553f);
	expect_ease(ease_inout_quad,  0.125000f,  0.500000f,  0.875000f);
	expect_ease(ease_inout_cube,  0.062500f,  0.500000f,  0.937500f);
	expect_ease(ease_inout_quar,  0.031250f,  0.500000f,  0.968750f);
	expect_ease(ease_inout_quin,  0.015625f,  0.500000f,  0.984375f);
	expect_ease(ease_inout_expo,  0.015625f,  0.500000f,  0.984375f);
	expect_ease(ease_inout_circ,  0.066987f,  0.500000f,  0.933013f);
	expect_ease(ease_inout_back, -0.187500f,  0.500000f,  1.187500f);
	expect_ease(ease_inout_elas, -0.011049f,  0.500000f,  1.011049f);
	expect_ease(ease_inout_boun,  0.140625f,  0.500000f,  0.859375f);
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
		sprintf(key, "k%zu", i);
		dict_insert(&dict, key, data + i);
	}

	for (size_t i = 0; i < countof(data); ++i) {
		char key[32] = {0};
		sprintf(key, "k%zu", i);
		expect_int32(i, *(int32_t*)dict_get(&dict, key));
	}

	dict_free(&dict);
}

void expect_fio(enum fio_err expected, enum fio_err actual)
{
	if (expected != actual) {
		ASSERTFAIL("fio: expected %d, but got %d\n", expected, actual);
	}
}

void test_fio()
{
	struct rjd_alloc_context context = alloc_initdefault();

	const char expected_contents[] = "this is a test file that has a utf-8 character!";

	enum fio_err err;
	err = fio_write("test.txt", expected_contents, sizeof(expected_contents), FIO_WRITEMODE_REPLACE);
	expect_fio(FIO_ERR_OK, err);

	char* buffer;
	err = fio_read("does_not_exist.txt", &buffer, &context);
	expect_fio(FIO_ERR_IO, err);
	err = fio_read("test.txt", &buffer, &context);
	expect_fio(FIO_ERR_OK, err);
	expect_uint32(sizeof(expected_contents), array_count(buffer));
	expect_true(!memcmp(buffer, expected_contents, sizeof(expected_contents)));

	err = fio_write("test2.txt", expected_contents, sizeof(expected_contents), FIO_WRITEMODE_REPLACE);
	expect_fio(FIO_ERR_OK, err);
	err = fio_write("test2.txt", expected_contents, sizeof(expected_contents), FIO_WRITEMODE_APPEND);
	char* buffer2;
	err = fio_read("test2.txt", &buffer2, &context);
	expect_fio(FIO_ERR_OK, err);
	expect_uint32(sizeof(expected_contents)*2, array_count(buffer2));

	size_t filesize;
	err = fio_size("does_not_exist.txt", &filesize);
	expect_fio(FIO_ERR_IO, err);
	err = fio_size("test.txt", &filesize);
	expect_fio(FIO_ERR_OK, err);
	expect_uint32(sizeof(expected_contents), (uint32_t)filesize);

	err = fio_delete("does_not_exist.txt");
	expect_fio(FIO_ERR_IO, err);
	err = fio_delete("test.txt");
	expect_fio(FIO_ERR_OK, err);
	err = fio_delete("test2.txt");
	expect_fio(FIO_ERR_OK, err);
}

int main(void) 
{
	test_logging();
	test_enum();
	test_alloc();
	test_rng();
	test_array();
	test_math();
	test_easing();
	test_strbuf();
	test_profiler();
	test_cmd();
	test_dict();
	test_fio();

	return 0;
}

