#pragma once

#define RJD_MATH 1

// TODO mat3
// TODO mat4
#define RJD_MATH_EPSILON (0.000001)

static inline uint32_t rjd_math_next_pow2(uint32_t v);
static inline int32_t rjd_math_pow32(int32_t v, uint32_t power);
static inline double rjd_math_remap(double v, double oldmin, double oldmax, double newmin, double newmax);

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

// vector

struct rjd_math_float3
{
	__m128 v;
};

static inline struct rjd_math_float3 rjd_math_float3_zero(void);
static inline struct rjd_math_float3 rjd_math_float3_xyz(float x, float y, float z);
static inline struct rjd_math_float3 rjd_math_float3_splat(float v);
static inline struct rjd_math_float3 rjd_math_float3_one(void);
static inline struct rjd_math_float3 rjd_math_float3_up(void);
static inline struct rjd_math_float3 rjd_math_float3_down(void);
static inline struct rjd_math_float3 rjd_math_float3_left(void);
static inline struct rjd_math_float3 rjd_math_float3_right(void);
static inline struct rjd_math_float3 rjd_math_float3_forward(void);
static inline struct rjd_math_float3 rjd_math_float3_back(void);
static inline struct rjd_math_float3 rjd_math_float3_shuffle(struct rjd_math_float3 v, int x, int y, int z);
static inline struct rjd_math_float3 rjd_math_float3_yzx(struct rjd_math_float3 v);
static inline struct rjd_math_float3 rjd_math_float3_zxy(struct rjd_math_float3 v);
static inline struct rjd_math_float3 rjd_math_float3_setx(struct rjd_math_float3 v, float x);
static inline struct rjd_math_float3 rjd_math_float3_sety(struct rjd_math_float3 v, float y);
static inline struct rjd_math_float3 rjd_math_float3_setz(struct rjd_math_float3 v, float z);
static inline float 				 rjd_math_float3_x(struct rjd_math_float3 v);
static inline float 				 rjd_math_float3_y(struct rjd_math_float3 v);
static inline float 				 rjd_math_float3_z(struct rjd_math_float3 v);
static inline float 				 rjd_math_float3_sum(struct rjd_math_float3 v);
static inline float 				 rjd_math_float3_dot(struct rjd_math_float3 a, struct rjd_math_float3 b);
static inline float 				 rjd_math_float3_angle(struct rjd_math_float3 a, struct rjd_math_float3 b);
static inline float 				 rjd_math_float3_lengthsq(struct rjd_math_float3 v);
static inline float 				 rjd_math_float3_length(struct rjd_math_float3 v);
static inline struct rjd_math_float3 rjd_math_float3_normalize(struct rjd_math_float3 v);
static inline struct rjd_math_float3 rjd_math_float3_scale(struct rjd_math_float3 v, float s);
static inline struct rjd_math_float3 rjd_math_float3_add(struct rjd_math_float3 a, struct rjd_math_float3 b);
static inline struct rjd_math_float3 rjd_math_float3_sub(struct rjd_math_float3 a, struct rjd_math_float3 b);
static inline struct rjd_math_float3 rjd_math_float3_mul(struct rjd_math_float3 a, struct rjd_math_float3 b);
static inline struct rjd_math_float3 rjd_math_float3_div(struct rjd_math_float3 a, struct rjd_math_float3 b);
static inline struct rjd_math_float3 rjd_math_float3_cross(struct rjd_math_float3 a, struct rjd_math_float3 b);
static inline struct rjd_math_float3 rjd_math_float3_min(struct rjd_math_float3 a, struct rjd_math_float3 b);
static inline struct rjd_math_float3 rjd_math_float3_max(struct rjd_math_float3 a, struct rjd_math_float3 b);
static inline struct rjd_math_float3 rjd_math_float3_project(struct rjd_math_float3 a, struct rjd_math_float3 b);
static inline struct rjd_math_float3 rjd_math_float3_reflect(struct rjd_math_float3 a, struct rjd_math_float3 b);
static inline struct rjd_math_float3 rjd_math_float3_lerp(struct rjd_math_float3 a, struct rjd_math_float3 b, float t);
static inline bool					 rjd_math_float3_eq(struct rjd_math_float3 a, struct rjd_math_float3 b);

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

	#define float3				rjd_math_float3
	#define float3_xyz      	rjd_math_float3_xyz
	#define float3_zero     	rjd_math_float3_zero
	#define float3_one      	rjd_math_float3_one
	#define float3_up       	rjd_math_float3_up
	#define float3_down     	rjd_math_float3_down
	#define float3_left     	rjd_math_float3_left
	#define float3_right    	rjd_math_float3_right
	#define float3_forward  	rjd_math_float3_forward
	#define float3_back     	rjd_math_float3_back
	#define float3_setx     	rjd_math_float3_setx
	#define float3_sety     	rjd_math_float3_sety
	#define float3_setz     	rjd_math_float3_setz
	#define float3_normalize	rjd_math_float3_normalize
	#define float3_scale    	rjd_math_float3_scale
	#define float3_add      	rjd_math_float3_add
	#define float3_sub      	rjd_math_float3_sub
	#define float3_dot      	rjd_math_float3_dot
	#define float3_cross    	rjd_math_float3_cross
	#define float3_min      	rjd_math_float3_min
	#define float3_max      	rjd_math_float3_max
	#define float3_project  	rjd_math_float3_project
	#define float3_reflect  	rjd_math_float3_reflect
	#define float3_lerp     	rjd_math_float3_lerp
	#define float3_slerp    	rjd_math_float3_slerp
	#define float3_x        	rjd_math_float3_x
	#define float3_y        	rjd_math_float3_y
	#define float3_z        	rjd_math_float3_z
	#define float3_length   	rjd_math_float3_length
	#define float3_lengthsq 	rjd_math_float3_lengthsq
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

static inline struct rjd_math_float3 rjd_math_float3_zero(void) { 
	struct rjd_math_float3 v = { _mm_setzero_ps() };
	return v;
}
static inline struct rjd_math_float3 rjd_math_float3_xyz(float x, float y, float z) {
	struct rjd_math_float3 v = { _mm_set_ps(x, y, z, 0) }; 
	return v;
}
static inline struct rjd_math_float3 rjd_math_float3_splat(float v) { return rjd_math_float3_xyz(v,v,v); }
static inline struct rjd_math_float3 rjd_math_float3_one(void) { return rjd_math_float3_xyz(1,1,1); }
static inline struct rjd_math_float3 rjd_math_float3_up(void) { return rjd_math_float3_xyz(0,1,0); }
static inline struct rjd_math_float3 rjd_math_float3_down(void) { return rjd_math_float3_xyz(0,-1,0); }
static inline struct rjd_math_float3 rjd_math_float3_left(void) { return rjd_math_float3_xyz(-1,0,0); }
static inline struct rjd_math_float3 rjd_math_float3_right(void) { return rjd_math_float3_xyz(1,0,0); }
static inline struct rjd_math_float3 rjd_math_float3_forward(void) { return rjd_math_float3_xyz(0,0,1); }
static inline struct rjd_math_float3 rjd_math_float3_back(void) { return rjd_math_float3_xyz(0,0,-1); }
static inline struct rjd_math_float3 rjd_math_float3_shuffle(struct rjd_math_float3 v, int x, int y, int z) {
	v.v = _mm_shuffle_ps(v.v, v.v, _MM_SHUFFLE(z, z, y, x));
	return v;
}
static inline struct rjd_math_float3 rjd_math_float3_yzx(struct rjd_math_float3 v) {
	return rjd_math_float3_shuffle(v, 1, 2, 0);
}
static inline struct rjd_math_float3 rjd_math_float3_zxy(struct rjd_math_float3 v) {
	return rjd_math_float3_shuffle(v, 2, 0, 1);
}
static inline struct rjd_math_float3 rjd_math_float3_setx(struct rjd_math_float3 v, float x) {
	v.v = _mm_move_ss(v.v, rjd_math_float3_splat(x).v);
	return v;
}
static inline struct rjd_math_float3 rjd_math_float3_sety(struct rjd_math_float3 v, float y) {
	v = rjd_math_float3_shuffle(v, 1, 0, 2);
	v = rjd_math_float3_setx(v, y);
	v = rjd_math_float3_shuffle(v, 1, 0, 2);
	return v;
}
static inline struct rjd_math_float3 rjd_math_float3_setz(struct rjd_math_float3 v, float z) {
	v = rjd_math_float3_shuffle(v, 2, 1, 0);
	v = rjd_math_float3_setx(v, z);
	v = rjd_math_float3_shuffle(v, 2, 1, 0);
	return v;
}
static inline float rjd_math_float3_x(struct rjd_math_float3 v) {
	return _mm_cvtss_f32(v.v);
}
static inline float rjd_math_float3_y(struct rjd_math_float3 v) {
	return rjd_math_float3_x(rjd_math_float3_shuffle(v, 1, 0, 2));
}
static inline float rjd_math_float3_z(struct rjd_math_float3 v) {
	return rjd_math_float3_x(rjd_math_float3_shuffle(v, 2, 1, 0));
}
static inline float rjd_math_float3_sum(struct rjd_math_float3 v) {
	v.v = _mm_hadd_ps(v.v, v.v);
	v.v = _mm_hadd_ps(v.v, v.v);
	return rjd_math_float3_x(v);
}
static inline float rjd_math_float3_dot(struct rjd_math_float3 a, struct rjd_math_float3 b) {
	struct rjd_math_float3 product = rjd_math_float3_mul(a,b);
	return rjd_math_float3_sum(product);
}
static inline float rjd_math_float3_angle(struct rjd_math_float3 a, struct rjd_math_float3 b) {
	float dot = rjd_math_float3_dot(a, b);
	float la = rjd_math_float3_length(a);
	float lb = rjd_math_float3_length(b);
	return acos(dot / (la * lb));
}
static inline float rjd_math_float3_lengthsq(struct rjd_math_float3 v) {
	return rjd_math_float3_dot(v, v);
}
static inline float rjd_math_float3_length(struct rjd_math_float3 v) {
	return sqrt(rjd_math_float3_lengthsq(v));
}
static inline struct rjd_math_float3 rjd_math_float3_normalize(struct rjd_math_float3 v) {
	float length = rjd_math_float3_length(v);
	RJD_ASSERT(length != 0);
	struct rjd_math_float3 l = rjd_math_float3_splat(length);
	return rjd_math_float3_div(v, l);
}
static inline struct rjd_math_float3 rjd_math_float3_scale(struct rjd_math_float3 v, float s) {
	struct rjd_math_float3 scales = rjd_math_float3_splat(s);
	return rjd_math_float3_mul(v, scales);
}
static inline struct rjd_math_float3 rjd_math_float3_add(struct rjd_math_float3 a, struct rjd_math_float3 b) {
	a.v = _mm_add_ps(a.v, b.v);
	return a;
}
static inline struct rjd_math_float3 rjd_math_float3_sub(struct rjd_math_float3 a, struct rjd_math_float3 b) {
	a.v = _mm_sub_ps(a.v, b.v);
	return a;
}
static inline struct rjd_math_float3 rjd_math_float3_mul(struct rjd_math_float3 a, struct rjd_math_float3 b) {
	a.v = _mm_mul_ps(a.v, b.v);
	return a;
}
static inline struct rjd_math_float3 rjd_math_float3_div(struct rjd_math_float3 a, struct rjd_math_float3 b) {
	a.v = _mm_div_ps(a.v, b.v);
	return a;
}
static inline struct rjd_math_float3 rjd_math_float3_cross(struct rjd_math_float3 a, struct rjd_math_float3 b) {
	struct rjd_math_float3 ap = rjd_math_float3_mul(rjd_math_float3_yzx(a), rjd_math_float3_zxy(b));
	struct rjd_math_float3 bp = rjd_math_float3_mul(rjd_math_float3_zxy(a), rjd_math_float3_yzx(b));
	return rjd_math_float3_sub(ap, bp);
}
static inline struct rjd_math_float3 rjd_math_float3_min(struct rjd_math_float3 a, struct rjd_math_float3 b) {
	a.v = _mm_min_ps(a.v, b.v);
	return a;
}
static inline struct rjd_math_float3 rjd_math_float3_max(struct rjd_math_float3 a, struct rjd_math_float3 b) {
	a.v = _mm_max_ps(a.v, b.v);
	return a;
}
static inline struct rjd_math_float3 rjd_math_float3_project(struct rjd_math_float3 a, struct rjd_math_float3 b) {
	float dot = rjd_math_float3_dot(a, b);
	float lb = rjd_math_float3_length(b);
	return rjd_math_float3_scale(b, dot / lb);
}
static inline struct rjd_math_float3 rjd_math_float3_reflect(struct rjd_math_float3 v, struct rjd_math_float3 n) {
	RJD_ASSERT(rjd_math_float3_eq(n, rjd_math_float3_normalize(n)));
	struct rjd_math_float3 projected = rjd_math_float3_scale(n, 2 * rjd_math_float3_dot(v, n));
	return rjd_math_float3_sub(v, projected);
}
static inline struct rjd_math_float3 rjd_math_float3_lerp(struct rjd_math_float3 a, struct rjd_math_float3 b, float t) {
	struct rjd_math_float3 v = rjd_math_float3_sub(b, a);
	v = rjd_math_float3_scale(v, t);
	v = rjd_math_float3_add(v, a);
	return v;
}
static inline bool rjd_math_float3_eq(struct rjd_math_float3 a, struct rjd_math_float3 b) {
	return (_mm_movemask_ps(_mm_cmpeq_ps(a.v, b.v)) & 7) == 7;
}

