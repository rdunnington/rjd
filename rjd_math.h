#pragma once

#define RJD_MATH 1

// TODO vec
// TODO mat3
// TODO mat4
#define RJD_MATH_EPSILON (0.000001)

uint32_t rjd_math_next_pow2(uint32_t v);
int32_t rjd_math_pow32(int32_t v, uint32_t power);
double rjd_math_remap(double v, double oldmin, double oldmax, double newmin, double newmax);

#define RJD_MATH_DECLARE_SIGN_FUNC(name, type) type name(type v);
#define RJD_MATH_DEFINE_SIGN_FUNC(name, type) type name(type v) { return (v < 0) ? -1 : 1; }
#define RJD_MATH_SIGN_FUNCS(xmacro)		\
	xmacro(rjd_math_sign32, int32_t)	\
	xmacro(rjd_math_sign, double)		\
	xmacro(rjd_math_signf, float)
RJD_MATH_SIGN_FUNCS(RJD_MATH_DECLARE_SIGN_FUNC)

#define RJD_MATH_DECLARE_ISEQUAL_FUNC(name, type) type name(type a, type b);
#define RJD_MATH_DEFINE_ISEQUAL_FUNC(name, type) type name(type a, type b) { return (type)fabs(a - b) < RJD_MATH_EPSILON; }
#define RJD_MATH_ISEQUAL_FUNCS(xmacro)	\
	xmacro(rjd_math_isequal, double)	\
	xmacro(rjd_math_isequalf, float)
RJD_MATH_ISEQUAL_FUNCS(RJD_MATH_DECLARE_ISEQUAL_FUNC)

#define RJD_MATH_DECLARE_MIN_FUNC(name, type) type name(type a, type b);
#define RJD_MATH_DEFINE_MIN_FUNC(name, type) type name(type a, type b) { return (a < b) ? a : b; }
#define RJD_MATH_MIN_FUNCS(xmacro)		\
	xmacro(rjd_math_min32, int32_t)		\
	xmacro(rjd_math_min64, int64_t)		\
	xmacro(rjd_math_minu32, uint32_t)	\
	xmacro(rjd_math_minu64, uint64_t)
RJD_MATH_MIN_FUNCS(RJD_MATH_DECLARE_MIN_FUNC)

#define RJD_MATH_DECLARE_MAX_FUNC(name, type) type name(type a, type b);
#define RJD_MATH_DEFINE_MAX_FUNC(name, type) type name(type a, type b) { return (a < b) ? b : a; }
#define RJD_MATH_MAX_FUNCS(xmacro)		\
	xmacro(rjd_math_max32, int32_t)		\
	xmacro(rjd_math_max64, uint64_t)	\
	xmacro(rjd_math_maxu64, int64_t)	\
	xmacro(rjd_math_maxu32, uint32_t)
RJD_MATH_MAX_FUNCS(RJD_MATH_DECLARE_MAX_FUNC)

#define RJD_MATH_DECLARE_CLAMP_FUNC(name, type) type name(type v, type minv, type maxv);
#define RJD_MATH_DEFINE_CLAMP_FUNC(name, type) type name(type v, type minv, type maxv) { return (v < minv) ? (minv) : (v > maxv ? maxv : v); }
#define RJD_MATH_CLAMP_FUNCS(xmacro)	\
	xmacro(rjd_math_clamp, double)		\
	xmacro(rjd_math_clampf, float)		\
	xmacro(rjd_math_clamp32, int32_t)	\
	xmacro(rjd_math_clamp64, int64_t)	\
	xmacro(rjd_math_clampu32, uint32_t)	\
	xmacro(rjd_math_clampu64, uint64_t)
RJD_MATH_CLAMP_FUNCS(RJD_MATH_DECLARE_CLAMP_FUNC)

#ifdef RJD_ENABLE_SHORTNAMES
	#define next_pow2	rjd_math_next_pow2
	#define pow32		rjd_math_pow32
	#define sign		rjd_math_sign
	#define signf		rjd_math_signf
	#define sign32		rjd_math_sign32
	#define isequal		rjd_math_isequal
	#define isequalf	rjd_math_isequalf
	#define remap		rjd_math_remap

	#define min32		rjd_math_min32
	#define min64		rjd_math_min64
	#define minu32		rjd_math_minu32
	#define minu64		rjd_math_minu64
	#define max32		rjd_math_max32
	#define max64		rjd_math_max64
	#define maxu32		rjd_math_maxu32
	#define maxu64		rjd_math_maxu64

	#define clamp		rjd_math_clamp
	#define clampf		rjd_math_clampf
	#define clamp32		rjd_math_clamp32
	#define clamp64		rjd_math_clamp64
	#define clampu32	rjd_math_clampu32
	#define clampu64	rjd_math_clampu64
#endif

#if RJD_IMPL

RJD_MATH_SIGN_FUNCS(RJD_MATH_DEFINE_SIGN_FUNC)
RJD_MATH_ISEQUAL_FUNCS(RJD_MATH_DEFINE_ISEQUAL_FUNC)
RJD_MATH_MIN_FUNCS(RJD_MATH_DEFINE_MIN_FUNC)
RJD_MATH_MAX_FUNCS(RJD_MATH_DEFINE_MAX_FUNC)
RJD_MATH_CLAMP_FUNCS(RJD_MATH_DEFINE_CLAMP_FUNC)

uint32_t rjd_math_next_pow2(uint32_t v) 
{
	--v;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;

	return v;
}

int32_t rjd_math_pow32(int32_t v, uint32_t power)
{
	int32_t r = 1;
	while (power) {
		r *= v;
		--power;
	}
	return r;
}

double rjd_math_remap(double v, double oldmin, double oldmax, double newmin, double newmax)
{
	float oldrange = oldmax - oldmin;
	float newrange = newmax - newmin;
	return ((v - oldmin) * newrange) / oldrange + newmin;
}

#endif

