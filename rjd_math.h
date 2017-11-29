#pragma once

#define RJD_MATH 1

#define RJD_MATH_PI (3.141592653589793238462643f)
#define RJD_MATH_EPSILON (0.000001)

static inline uint32_t rjd_math_next_pow2(uint32_t v);
static inline int32_t rjd_math_pow32(int32_t v, uint32_t power);
static inline double rjd_math_remap(double v, double oldmin, double oldmax, double newmin, double newmax);

#define RJD_MATH_DECLARE_SIGN_FUNC(name, type) static inline type name(type v);
#define RJD_MATH_DEFINE_SIGN_FUNC(name, type) static inline type name(type v) { return (v < 0) ? -1 : 1; }
#define RJD_MATH_SIGN_FUNCS(xmacro)		\
	xmacro(rjd_math_sign32, int32_t)	\
	xmacro(rjd_math_sign, double)		\
	xmacro(rjd_math_signf, float)
RJD_MATH_SIGN_FUNCS(RJD_MATH_DECLARE_SIGN_FUNC)

#define RJD_MATH_DECLARE_ISEQUAL_FUNC(name, type) static inline type name(type a, type b);
#define RJD_MATH_DEFINE_ISEQUAL_FUNC(name, type) static inline type name(type a, type b) { return (type)fabs(a - b) < RJD_MATH_EPSILON; }
#define RJD_MATH_ISEQUAL_FUNCS(xmacro)	\
	xmacro(rjd_math_isequal, double)	\
	xmacro(rjd_math_isequalf, float)
RJD_MATH_ISEQUAL_FUNCS(RJD_MATH_DECLARE_ISEQUAL_FUNC)

#define RJD_MATH_DECLARE_MIN_FUNC(name, type) static inline type name(type a, type b);
#define RJD_MATH_DEFINE_MIN_FUNC(name, type) static inline type name(type a, type b) { return (a < b) ? a : b; }
#define RJD_MATH_MIN_FUNCS(xmacro)		\
	xmacro(rjd_math_min32, int32_t)		\
	xmacro(rjd_math_min64, int64_t)		\
	xmacro(rjd_math_minu32, uint32_t)	\
	xmacro(rjd_math_minu64, uint64_t)
RJD_MATH_MIN_FUNCS(RJD_MATH_DECLARE_MIN_FUNC)

#define RJD_MATH_DECLARE_MAX_FUNC(name, type) static inline type name(type a, type b);
#define RJD_MATH_DEFINE_MAX_FUNC(name, type) static inline type name(type a, type b) { return (a < b) ? b : a; }
#define RJD_MATH_MAX_FUNCS(xmacro)		\
	xmacro(rjd_math_max32, int32_t)		\
	xmacro(rjd_math_max64, uint64_t)	\
	xmacro(rjd_math_maxu64, int64_t)	\
	xmacro(rjd_math_maxu32, uint32_t)
RJD_MATH_MAX_FUNCS(RJD_MATH_DECLARE_MAX_FUNC)

#define RJD_MATH_DECLARE_CLAMP_FUNC(name, type) static inline type name(type v, type minv, type maxv);
#define RJD_MATH_DEFINE_CLAMP_FUNC(name, type) static inline type name(type v, type minv, type maxv) { return (v < minv) ? (minv) : (v > maxv ? maxv : v); }
#define RJD_MATH_CLAMP_FUNCS(xmacro)	\
	xmacro(rjd_math_clamp, double)		\
	xmacro(rjd_math_clampf, float)		\
	xmacro(rjd_math_clamp32, int32_t)	\
	xmacro(rjd_math_clamp64, int64_t)	\
	xmacro(rjd_math_clampu32, uint32_t)	\
	xmacro(rjd_math_clampu64, uint64_t)
RJD_MATH_CLAMP_FUNCS(RJD_MATH_DECLARE_CLAMP_FUNC)

// vector

typedef struct
{
	__m128 v;
} rjd_math_vec3;

#define rjd_math_vec3_shuffle(v3, x, y, z) ((rjd_math_vec3){_mm_shuffle_ps((v3).v, (v3).v, _MM_SHUFFLE(z, z, y, x))})

static inline rjd_math_vec3 rjd_math_vec3_zero(void);
static inline rjd_math_vec3 rjd_math_vec3_xyz(float x, float y, float z);
static inline rjd_math_vec3 rjd_math_vec3_splat(float v);
static inline rjd_math_vec3 rjd_math_vec3_one(void);
static inline rjd_math_vec3 rjd_math_vec3_up(void);
static inline rjd_math_vec3 rjd_math_vec3_down(void);
static inline rjd_math_vec3 rjd_math_vec3_left(void);
static inline rjd_math_vec3 rjd_math_vec3_right(void);
static inline rjd_math_vec3 rjd_math_vec3_forward(void);
static inline rjd_math_vec3 rjd_math_vec3_back(void);
static inline rjd_math_vec3 rjd_math_vec3_yzx(rjd_math_vec3 v);
static inline rjd_math_vec3 rjd_math_vec3_zxy(rjd_math_vec3 v);
static inline rjd_math_vec3 rjd_math_vec3_setx(rjd_math_vec3 v, float x);
static inline rjd_math_vec3 rjd_math_vec3_sety(rjd_math_vec3 v, float y);
static inline rjd_math_vec3 rjd_math_vec3_setz(rjd_math_vec3 v, float z);
static inline float 		rjd_math_vec3_x(rjd_math_vec3 v);
static inline float 		rjd_math_vec3_y(rjd_math_vec3 v);
static inline float 		rjd_math_vec3_z(rjd_math_vec3 v);
static inline float 		rjd_math_vec3_sum(rjd_math_vec3 v);
static inline float 		rjd_math_vec3_dot(rjd_math_vec3 a, rjd_math_vec3 b);
static inline float 		rjd_math_vec3_angle(rjd_math_vec3 a, rjd_math_vec3 b);
static inline float 		rjd_math_vec3_lengthsq(rjd_math_vec3 v);
static inline float 		rjd_math_vec3_length(rjd_math_vec3 v);
static inline rjd_math_vec3 rjd_math_vec3_normalize(rjd_math_vec3 v);
static inline rjd_math_vec3 rjd_math_vec3_scale(rjd_math_vec3 v, float s);
static inline rjd_math_vec3 rjd_math_vec3_add(rjd_math_vec3 a, rjd_math_vec3 b);
static inline rjd_math_vec3 rjd_math_vec3_sub(rjd_math_vec3 a, rjd_math_vec3 b);
static inline rjd_math_vec3 rjd_math_vec3_mul(rjd_math_vec3 a, rjd_math_vec3 b);
static inline rjd_math_vec3 rjd_math_vec3_div(rjd_math_vec3 a, rjd_math_vec3 b);
static inline rjd_math_vec3 rjd_math_vec3_cross(rjd_math_vec3 a, rjd_math_vec3 b);
static inline rjd_math_vec3 rjd_math_vec3_min(rjd_math_vec3 a, rjd_math_vec3 b);
static inline rjd_math_vec3 rjd_math_vec3_max(rjd_math_vec3 a, rjd_math_vec3 b);
static inline rjd_math_vec3 rjd_math_vec3_project(rjd_math_vec3 a, rjd_math_vec3 b);
static inline rjd_math_vec3 rjd_math_vec3_reflect(rjd_math_vec3 a, rjd_math_vec3 b);
static inline rjd_math_vec3 rjd_math_vec3_lerp(rjd_math_vec3 a, rjd_math_vec3 b, float t);
static inline bool			rjd_math_vec3_eq(rjd_math_vec3 a, rjd_math_vec3 b);

typedef struct 
{
	rjd_math_vec3 v[3];
} mat3;

#ifdef RJD_ENABLE_SHORTNAMES
	#define PI		RJD_MATH_PI
	#define EPSILON	RJD_MATH_EPSILON

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

	#define vec3			rjd_math_vec3
	#define vec3_zero     	rjd_math_vec3_zero
	#define vec3_xyz      	rjd_math_vec3_xyz
	#define vec3_splat    	rjd_math_vec3_splat
	#define vec3_one      	rjd_math_vec3_one
	#define vec3_up       	rjd_math_vec3_up
	#define vec3_down     	rjd_math_vec3_down
	#define vec3_left     	rjd_math_vec3_left
	#define vec3_right    	rjd_math_vec3_right
	#define vec3_forward  	rjd_math_vec3_forward
	#define vec3_back     	rjd_math_vec3_back
	#define vec3_shuffle  	rjd_math_vec3_shuffle
	#define vec3_yzx      	rjd_math_vec3_yzx
	#define vec3_zxy      	rjd_math_vec3_zxy
	#define vec3_setx     	rjd_math_vec3_setx
	#define vec3_sety     	rjd_math_vec3_sety
	#define vec3_setz     	rjd_math_vec3_setz
	#define vec3_x        	rjd_math_vec3_x
	#define vec3_y        	rjd_math_vec3_y
	#define vec3_z        	rjd_math_vec3_z
	#define vec3_sum      	rjd_math_vec3_sum
	#define vec3_dot      	rjd_math_vec3_dot
	#define vec3_angle    	rjd_math_vec3_angle
	#define vec3_lengthsq 	rjd_math_vec3_lengthsq
	#define vec3_length   	rjd_math_vec3_length
	#define vec3_normalize	rjd_math_vec3_normalize
	#define vec3_scale    	rjd_math_vec3_scale
	#define vec3_add      	rjd_math_vec3_add
	#define vec3_sub      	rjd_math_vec3_sub
	#define vec3_mul      	rjd_math_vec3_mul
	#define vec3_div      	rjd_math_vec3_div
	#define vec3_cross    	rjd_math_vec3_cross
	#define vec3_min      	rjd_math_vec3_min
	#define vec3_max      	rjd_math_vec3_max
	#define vec3_project  	rjd_math_vec3_project
	#define vec3_reflect  	rjd_math_vec3_reflect
	#define vec3_lerp     	rjd_math_vec3_lerp
	#define vec3_eq       	rjd_math_vec3_eq
#endif

RJD_MATH_SIGN_FUNCS(RJD_MATH_DEFINE_SIGN_FUNC)
RJD_MATH_ISEQUAL_FUNCS(RJD_MATH_DEFINE_ISEQUAL_FUNC)
RJD_MATH_MIN_FUNCS(RJD_MATH_DEFINE_MIN_FUNC)
RJD_MATH_MAX_FUNCS(RJD_MATH_DEFINE_MAX_FUNC)
RJD_MATH_CLAMP_FUNCS(RJD_MATH_DEFINE_CLAMP_FUNC)

static inline uint32_t rjd_math_next_pow2(uint32_t v) 
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

static inline int32_t rjd_math_pow32(int32_t v, uint32_t power)
{
	int32_t r = 1;
	while (power) {
		r *= v;
		--power;
	}
	return r;
}

static inline double rjd_math_remap(double v, double oldmin, double oldmax, double newmin, double newmax)
{
	float oldrange = oldmax - oldmin;
	float newrange = newmax - newmin;
	return ((v - oldmin) * newrange) / oldrange + newmin;
}

static inline rjd_math_vec3 rjd_math_vec3_zero(void) { 
	rjd_math_vec3 v = { _mm_setzero_ps() };
	return v;
}
static inline rjd_math_vec3 rjd_math_vec3_xyz(float x, float y, float z) {
	rjd_math_vec3 v = { _mm_set_ps(0, z, y, x) }; 
	return v;
}
static inline rjd_math_vec3 rjd_math_vec3_splat(float v) { return rjd_math_vec3_xyz(v,v,v); }
static inline rjd_math_vec3 rjd_math_vec3_one(void) { return rjd_math_vec3_xyz(1,1,1); }
static inline rjd_math_vec3 rjd_math_vec3_up(void) { return rjd_math_vec3_xyz(0,1,0); }
static inline rjd_math_vec3 rjd_math_vec3_down(void) { return rjd_math_vec3_xyz(0,-1,0); }
static inline rjd_math_vec3 rjd_math_vec3_left(void) { return rjd_math_vec3_xyz(-1,0,0); }
static inline rjd_math_vec3 rjd_math_vec3_right(void) { return rjd_math_vec3_xyz(1,0,0); }
static inline rjd_math_vec3 rjd_math_vec3_forward(void) { return rjd_math_vec3_xyz(0,0,1); }
static inline rjd_math_vec3 rjd_math_vec3_back(void) { return rjd_math_vec3_xyz(0,0,-1); }
static inline rjd_math_vec3 rjd_math_vec3_yzx(rjd_math_vec3 v) {
	return rjd_math_vec3_shuffle(v, 1, 2, 0);
}
static inline rjd_math_vec3 rjd_math_vec3_zxy(rjd_math_vec3 v) {
	return rjd_math_vec3_shuffle(v, 2, 0, 1);
}
static inline rjd_math_vec3 rjd_math_vec3_setx(rjd_math_vec3 v, float x) {
	v.v = _mm_move_ss(v.v, rjd_math_vec3_splat(x).v);
	return v;
}
static inline rjd_math_vec3 rjd_math_vec3_sety(rjd_math_vec3 v, float y) {
	v = rjd_math_vec3_shuffle(v, 1, 0, 2);
	v = rjd_math_vec3_setx(v, y);
	v = rjd_math_vec3_shuffle(v, 1, 0, 2);
	return v;
}
static inline rjd_math_vec3 rjd_math_vec3_setz(rjd_math_vec3 v, float z) {
	v = rjd_math_vec3_shuffle(v, 2, 1, 0);
	v = rjd_math_vec3_setx(v, z);
	v = rjd_math_vec3_shuffle(v, 2, 1, 0);
	return v;
}
static inline float rjd_math_vec3_x(rjd_math_vec3 v) {
	return _mm_cvtss_f32(v.v);
}
static inline float rjd_math_vec3_y(rjd_math_vec3 v) {
	return rjd_math_vec3_x(rjd_math_vec3_shuffle(v, 1, 0, 2));
}
static inline float rjd_math_vec3_z(rjd_math_vec3 v) {
	return rjd_math_vec3_x(rjd_math_vec3_shuffle(v, 2, 1, 0));
}
static inline float rjd_math_vec3_sum(rjd_math_vec3 v) {
	v.v = _mm_hadd_ps(v.v, v.v);
	v.v = _mm_hadd_ps(v.v, v.v);
	return rjd_math_vec3_x(v);
}
static inline float rjd_math_vec3_dot(rjd_math_vec3 a, rjd_math_vec3 b) {
	rjd_math_vec3 product = rjd_math_vec3_mul(a,b);
	return rjd_math_vec3_sum(product);
}
static inline float rjd_math_vec3_angle(rjd_math_vec3 a, rjd_math_vec3 b) {
	float dot = rjd_math_vec3_dot(a, b);
	float la = rjd_math_vec3_length(a);
	float lb = rjd_math_vec3_length(b);
	return acos(dot / (la * lb));
}
static inline float rjd_math_vec3_lengthsq(rjd_math_vec3 v) {
	return rjd_math_vec3_dot(v, v);
}
static inline float rjd_math_vec3_length(rjd_math_vec3 v) {
	return sqrt(rjd_math_vec3_lengthsq(v));
}
static inline rjd_math_vec3 rjd_math_vec3_normalize(rjd_math_vec3 v) {
	float length = rjd_math_vec3_length(v);
	RJD_ASSERT(length != 0);
	rjd_math_vec3 l = rjd_math_vec3_splat(length);
	return rjd_math_vec3_div(v, l);
}
static inline rjd_math_vec3 rjd_math_vec3_scale(rjd_math_vec3 v, float s) {
	rjd_math_vec3 scales = rjd_math_vec3_splat(s);
	return rjd_math_vec3_mul(v, scales);
}
static inline rjd_math_vec3 rjd_math_vec3_add(rjd_math_vec3 a, rjd_math_vec3 b) {
	a.v = _mm_add_ps(a.v, b.v);
	return a;
}
static inline rjd_math_vec3 rjd_math_vec3_sub(rjd_math_vec3 a, rjd_math_vec3 b) {
	a.v = _mm_sub_ps(a.v, b.v);
	return a;
}
static inline rjd_math_vec3 rjd_math_vec3_mul(rjd_math_vec3 a, rjd_math_vec3 b) {
	a.v = _mm_mul_ps(a.v, b.v);
	return a;
}
static inline rjd_math_vec3 rjd_math_vec3_div(rjd_math_vec3 a, rjd_math_vec3 b) {
	a.v = _mm_div_ps(a.v, b.v);
	return a;
}
static inline rjd_math_vec3 rjd_math_vec3_cross(rjd_math_vec3 a, rjd_math_vec3 b) {
	rjd_math_vec3 ap = rjd_math_vec3_mul(rjd_math_vec3_yzx(a), rjd_math_vec3_zxy(b));
	rjd_math_vec3 bp = rjd_math_vec3_mul(rjd_math_vec3_zxy(a), rjd_math_vec3_yzx(b));
	return rjd_math_vec3_sub(ap, bp);
}
static inline rjd_math_vec3 rjd_math_vec3_min(rjd_math_vec3 a, rjd_math_vec3 b) {
	a.v = _mm_min_ps(a.v, b.v);
	return a;
}
static inline rjd_math_vec3 rjd_math_vec3_max(rjd_math_vec3 a, rjd_math_vec3 b) {
	a.v = _mm_max_ps(a.v, b.v);
	return a;
}
static inline rjd_math_vec3 rjd_math_vec3_project(rjd_math_vec3 a, rjd_math_vec3 b) {
	float dot = rjd_math_vec3_dot(a, b);
	float lb = rjd_math_vec3_length(b);
	return rjd_math_vec3_scale(b, dot / lb);
}
static inline rjd_math_vec3 rjd_math_vec3_reflect(rjd_math_vec3 v, rjd_math_vec3 n) {
	RJD_ASSERT(rjd_math_vec3_eq(n, rjd_math_vec3_normalize(n)));
	rjd_math_vec3 projected = rjd_math_vec3_scale(n, 2 * rjd_math_vec3_dot(v, n));
	return rjd_math_vec3_sub(v, projected);
}
static inline rjd_math_vec3 rjd_math_vec3_lerp(rjd_math_vec3 a, rjd_math_vec3 b, float t) {
	rjd_math_vec3 v = rjd_math_vec3_sub(b, a);
	v = rjd_math_vec3_scale(v, t);
	v = rjd_math_vec3_add(v, a);
	return v;
}
static inline bool rjd_math_vec3_eq(rjd_math_vec3 a, rjd_math_vec3 b) {
	return (_mm_movemask_ps(_mm_cmpeq_ps(a.v, b.v)) & 7) == 7;
}

