#pragma once

// SSE3 math library with some functions for normal floats

#define RJD_MATH_H 1

#define RJD_MATH_PI (3.141592653589793238462643f)
#define RJD_MATH_EPSILON (0.000001)

////////////////////////////////////////////////////////////////////////////////
// utils

static inline uint32_t rjd_math_next_pow2_u32(uint32_t v);
static inline int32_t rjd_math_pow_u32(int32_t v, uint32_t power);

#define RJD_MATH_DECLARE_SIGN_FUNC(name, type) static inline type name(type v);
#define RJD_MATH_DEFINE_SIGN_FUNC(name, type) static inline type name(type v) { return (v < (type)0) ? (type)-1 : (type)1; }
#define RJD_MATH_SIGN_FUNCS(xmacro)			\
	xmacro(rjd_math_sign_i64,	int64_t)	\
	xmacro(rjd_math_sign_i32,	int32_t)	\
	xmacro(rjd_math_sign_i16,	int16_t)	\
	xmacro(rjd_math_sign_i8,	int8_t)		\
	xmacro(rjd_math_sign_f64,	double)		\
	xmacro(rjd_math_sign_f32,	float)
RJD_MATH_SIGN_FUNCS(RJD_MATH_DECLARE_SIGN_FUNC)

#define rjd_math_sign(v) _Generic((v),	\
	int64_t:	rjd_math_sign_i64, 		\
	int32_t:	rjd_math_sign_i32,		\
	int16_t:	rjd_math_sign_i16, 		\
	int8_t:		rjd_math_sign_i8,		\
	double:		rjd_math_sign_f64, 		\
	float:		rjd_math_sign_f32)(v)

#define RJD_MATH_DECLARE_ISEQUAL_FUNC(name, type) static inline bool name(type a, type b);
#define RJD_MATH_DEFINE_ISEQUAL_FUNC(name, type) static inline bool name(type a, type b) { return (type)fabs(a - b) < RJD_MATH_EPSILON; }
#define RJD_MATH_ISEQUAL_FUNCS(xmacro)		\
	xmacro(rjd_math_isequal_f64, double)	\
	xmacro(rjd_math_isequal_f32, float)
RJD_MATH_ISEQUAL_FUNCS(RJD_MATH_DECLARE_ISEQUAL_FUNC)

#define rjd_math_isequal(a, b) _Generic((a),	\
	double:	rjd_math_isequal_f64,				\
	float:	rjd_math_isequal_f32)((a), (b))

// size_t versions are needed on OSX due to them being defined as unsigned long, which is different than 
#define RJD_MATH_DECLARE_MIN_FUNC(name, type) static inline type name(type a, type b);
#define RJD_MATH_DEFINE_MIN_FUNC(name, type) static inline type name(type a, type b) { return (a < b) ? a : b; }
#if RJD_PLATFORM_OSX
	#define RJD_MATH_MIN_FUNCS(xmacro)			\
		xmacro(rjd_math_min_i64,	int64_t)	\
		xmacro(rjd_math_min_i32,	int32_t)	\
		xmacro(rjd_math_min_i16,	int16_t)	\
		xmacro(rjd_math_min_i8,		int8_t)		\
		xmacro(rjd_math_min_sizet,	size_t)		\
		xmacro(rjd_math_min_u64,	uint64_t)	\
		xmacro(rjd_math_min_u32,	uint32_t)	\
		xmacro(rjd_math_min_u16,	uint16_t)	\
		xmacro(rjd_math_min_u8,		uint8_t)	\
		xmacro(rjd_math_min_f32,	float)		\
		xmacro(rjd_math_min_f64,	double)

	#define rjd_math_min(a, b) _Generic((a),	\
		int64_t:	rjd_math_min_i64,			\
		int32_t:	rjd_math_min_i32,			\
		int16_t:	rjd_math_min_i16,			\
		int8_t:		rjd_math_min_i8,			\
		size_t:		rjd_math_min_sizet,			\
		uint64_t:	rjd_math_min_u64,			\
		uint32_t:	rjd_math_min_u32,			\
		uint16_t:	rjd_math_min_u16,			\
		uint8_t:	rjd_math_min_u8,			\
		double:		rjd_math_min_f64,			\
		float:		rjd_math_min_f32)((a), (b))
#else
	#define RJD_MATH_MIN_FUNCS(xmacro)			\
		xmacro(rjd_math_min_i64,	int64_t)	\
		xmacro(rjd_math_min_i32,	int32_t)	\
		xmacro(rjd_math_min_i16,	int16_t)	\
		xmacro(rjd_math_min_i8,		int8_t)		\
		xmacro(rjd_math_min_u64,	uint64_t)	\
		xmacro(rjd_math_min_u32,	uint32_t)	\
		xmacro(rjd_math_min_u16,	uint16_t)	\
		xmacro(rjd_math_min_u8,		uint8_t)	\
		xmacro(rjd_math_min_f32,	float)		\
		xmacro(rjd_math_min_f64,	double)

	#define rjd_math_min(a, b) _Generic((a),	\
		int64_t:	rjd_math_min_i64,			\
		int32_t:	rjd_math_min_i32,			\
		int16_t:	rjd_math_min_i16,			\
		int8_t:		rjd_math_min_i8,			\
		uint64_t:	rjd_math_min_u64,			\
		uint32_t:	rjd_math_min_u32,			\
		uint16_t:	rjd_math_min_u16,			\
		uint8_t:	rjd_math_min_u8,			\
		double:		rjd_math_min_f64,			\
		float:		rjd_math_min_f32)((a), (b))
#endif
RJD_MATH_MIN_FUNCS(RJD_MATH_DECLARE_MIN_FUNC)

#define RJD_MATH_DECLARE_MAX_FUNC(name, type) static inline type name(type a, type b);
#define RJD_MATH_DEFINE_MAX_FUNC(name, type) static inline type name(type a, type b) { return (a < b) ? b : a; }
#if RJD_PLATFORM_OSX
	#define RJD_MATH_MAX_FUNCS(xmacro)		\
		xmacro(rjd_math_max_i64, int64_t)	\
		xmacro(rjd_math_max_i32, int32_t)	\
		xmacro(rjd_math_max_i16, int16_t)	\
		xmacro(rjd_math_max_i8,  int8_t)	\
		xmacro(rjd_math_max_sizet, size_t)	\
		xmacro(rjd_math_max_u64, uint64_t)	\
		xmacro(rjd_math_max_u32, uint32_t)	\
		xmacro(rjd_math_max_u16, uint16_t)	\
		xmacro(rjd_math_max_u8,  uint8_t)	\
		xmacro(rjd_math_max_f64, double)	\
		xmacro(rjd_math_max_f32, float)

	#define rjd_math_max(a, b) _Generic((a),	\
		int64_t:	rjd_math_max_i64,			\
		int32_t:	rjd_math_max_i32,			\
		int16_t:	rjd_math_max_i16,			\
		int8_t:		rjd_math_max_i8,			\
		size_t:		rjd_math_max_sizet,			\
		uint64_t:	rjd_math_max_u64,			\
		uint32_t:	rjd_math_max_u32,			\
		uint16_t:	rjd_math_max_u16,			\
		uint8_t:	rjd_math_max_u8,			\
		double:		rjd_math_max_f64,			\
		float:		rjd_math_max_f32)((a), (b))
#else
	#define RJD_MATH_MAX_FUNCS(xmacro)		\
		xmacro(rjd_math_max_i64, int64_t)	\
		xmacro(rjd_math_max_i32, int32_t)	\
		xmacro(rjd_math_max_i16, int16_t)	\
		xmacro(rjd_math_max_i8,  int8_t)	\
		xmacro(rjd_math_max_u64, uint64_t)	\
		xmacro(rjd_math_max_u32, uint32_t)	\
		xmacro(rjd_math_max_u16, uint16_t)	\
		xmacro(rjd_math_max_u8,  uint8_t)	\
		xmacro(rjd_math_max_f64, double)	\
		xmacro(rjd_math_max_f32, float)

	#define rjd_math_max(a, b) _Generic((a),	\
		int64_t:	rjd_math_max_i64,			\
		int32_t:	rjd_math_max_i32,			\
		int16_t:	rjd_math_max_i16,			\
		int8_t:		rjd_math_max_i8,			\
		uint64_t:	rjd_math_max_u64,			\
		uint32_t:	rjd_math_max_u32,			\
		uint16_t:	rjd_math_max_u16,			\
		uint8_t:	rjd_math_max_u8,			\
		double:		rjd_math_max_f64,			\
		float:		rjd_math_max_f32)((a), (b))
#endif
RJD_MATH_MAX_FUNCS(RJD_MATH_DECLARE_MAX_FUNC)

#define RJD_MATH_DECLARE_CLAMP_FUNC(name, type) static inline type name(type v, type minv, type maxv);
#define RJD_MATH_DEFINE_CLAMP_FUNC(name, type) static inline type name(type v, type minv, type maxv) { return (v < minv) ? (minv) : (v > maxv ? maxv : v); }
#if RJD_PLATFORM_OSX
	#define RJD_MATH_CLAMP_FUNCS(xmacro)		\
		xmacro(rjd_math_clamp_i64,	int64_t)	\
		xmacro(rjd_math_clamp_i32,	int32_t)	\
		xmacro(rjd_math_clamp_i16,	int16_t)	\
		xmacro(rjd_math_clamp_i8,	int8_t)		\
		xmacro(rjd_math_clamp_sizet,size_t)		\
		xmacro(rjd_math_clamp_u64,	uint64_t)	\
		xmacro(rjd_math_clamp_u32,	uint32_t)	\
		xmacro(rjd_math_clamp_u16,	uint16_t)	\
		xmacro(rjd_math_clamp_u8,	uint8_t)	\
		xmacro(rjd_math_clamp_f64,	double)		\
		xmacro(rjd_math_clamp_f32,	float)

	#define rjd_math_clamp(v, min, max) _Generic((v),	\
		int64_t:	rjd_math_clamp_i64,					\
		int32_t:	rjd_math_clamp_i32,					\
		int16_t:	rjd_math_clamp_i16,					\
		int8_t:		rjd_math_clamp_i8,					\
		size_t:		rjd_math_clamp_sizet,				\
		uint64_t:	rjd_math_clamp_u64,					\
		uint32_t:	rjd_math_clamp_u32,					\
		uint16_t:	rjd_math_clamp_u16,					\
		uint8_t:	rjd_math_clamp_u8,					\
		double:		rjd_math_clamp_f64,					\
		float:		rjd_math_clamp_f32)((v), (min), (max))
#else
	#define RJD_MATH_CLAMP_FUNCS(xmacro)		\
		xmacro(rjd_math_clamp_i64,	int64_t)	\
		xmacro(rjd_math_clamp_i32,	int32_t)	\
		xmacro(rjd_math_clamp_i16,	int16_t)	\
		xmacro(rjd_math_clamp_i8,	int8_t)		\
		xmacro(rjd_math_clamp_u64,	uint64_t)	\
		xmacro(rjd_math_clamp_u32,	uint32_t)	\
		xmacro(rjd_math_clamp_u16,	uint16_t)	\
		xmacro(rjd_math_clamp_u8,	uint8_t)	\
		xmacro(rjd_math_clamp_f64,	double)		\
		xmacro(rjd_math_clamp_f32,	float)

	#define rjd_math_clamp(v, min, max) _Generic((v),	\
		int64_t:	rjd_math_clamp_i64,					\
		int32_t:	rjd_math_clamp_i32,					\
		int16_t:	rjd_math_clamp_i16,					\
		int8_t:		rjd_math_clamp_i8,					\
		uint64_t:	rjd_math_clamp_u64,					\
		uint32_t:	rjd_math_clamp_u32,					\
		uint16_t:	rjd_math_clamp_u16,					\
		uint8_t:	rjd_math_clamp_u8,					\
		double:		rjd_math_clamp_f64,					\
		float:		rjd_math_clamp_f32)((v), (min), (max))
#endif
RJD_MATH_CLAMP_FUNCS(RJD_MATH_DECLARE_CLAMP_FUNC)

#define RJD_MATH_DECLARE_TRUNCATE_FUNC(name, bigtype, smalltype) static inline smalltype name(bigtype v);
#define RJD_MATH_DEFINE_TRUNCATE_FUNC(name, bigtype, smalltype) static inline smalltype name(bigtype v) { RJD_ASSERT(v <= (smalltype)-1); return (smalltype)v; }
#define RJD_MATH_TRUNCATE_FUNCS(xmacro) 						\
	xmacro(rjd_math_truncate_u64_to_u32, 	uint64_t, uint32_t)	\
	xmacro(rjd_math_truncate_u64_to_u16, 	uint64_t, uint16_t)	\
	xmacro(rjd_math_truncate_u64_to_u8,  	uint64_t, uint8_t)	\
	xmacro(rjd_math_truncate_u64_to_sizet,	uint64_t, size_t)	\
	xmacro(rjd_math_truncate_u32_to_u16, 	uint32_t, uint16_t)	\
	xmacro(rjd_math_truncate_u32_to_u8,  	uint32_t, uint8_t)	\
	xmacro(rjd_math_truncate_u16_to_u8,  	uint16_t, uint8_t)
RJD_MATH_TRUNCATE_FUNCS(RJD_MATH_DECLARE_TRUNCATE_FUNC)

#define RJD_MATH_DECLARE_REMAP_FUNC(name, type) static inline type name(type v, type oldmin, type oldmax, type newmin, type newmax);
#define RJD_MATH_DEFINE_REMAP_FUNC(name, type) static inline type name(type v, type oldmin, type oldmax, type newmin, type newmax) { type oldrange = oldmax - oldmin; type newrange = newmax - newmin; return ((v - oldmin) * newrange) / oldrange + newmin; }
#define RJD_MATH_REMAP_FUNCS(xmacro)		\
	xmacro(rjd_math_remap_f64, double)		\
	xmacro(rjd_math_remap_f32, float)
RJD_MATH_REMAP_FUNCS(RJD_MATH_DECLARE_REMAP_FUNC)

#define rjd_math_remap(v, oldmin, oldmax, newmin, newmax)	_Generic((v),	\
	double:	rjd_math_remap_f64,												\
	float:	rjd_math_remap_f32)(v, oldmin, oldmax, newmin, newmax)

// vector structs

// NOTE: this is the one place we break the "no typedef" rule for convenience
RJD_FORCE_ALIGN(16, typedef struct rjd_math_vec3 {
	__m128 v;
} rjd_math_vec3);

RJD_FORCE_ALIGN(16, typedef struct rjd_math_vec4 {
	__m128 v;
} rjd_math_vec4);

// column-major 4x4 matrix
RJD_FORCE_ALIGN(16, typedef struct rjd_math_mat4 {
	rjd_math_vec4 m[4];
} rjd_math_mat4);

// vec4

#define rjd_math_vec4_shuffle(v4, x, y, z, w) ((rjd_math_vec4){_mm_shuffle_ps((v4).v, (v4).v, _MM_SHUFFLE(w, z, y, x))})

static inline rjd_math_vec4 rjd_math_vec4_zero(void);
static inline rjd_math_vec4 rjd_math_vec4_xyzw(float x, float y, float z, float w);
static inline rjd_math_vec4 rjd_math_vec4_splat(float v);
static inline rjd_math_vec4 rjd_math_vec4_one(void);
static inline rjd_math_vec4 rjd_math_vec4_setx(rjd_math_vec4 v, float x);
static inline rjd_math_vec4 rjd_math_vec4_sety(rjd_math_vec4 v, float y);
static inline rjd_math_vec4 rjd_math_vec4_setz(rjd_math_vec4 v, float z);
static inline rjd_math_vec4 rjd_math_vec4_setw(rjd_math_vec4 v, float w);
static inline float 		rjd_math_vec4_x(rjd_math_vec4 v);
static inline float 		rjd_math_vec4_y(rjd_math_vec4 v);
static inline float 		rjd_math_vec4_z(rjd_math_vec4 v);
static inline float 		rjd_math_vec4_w(rjd_math_vec4 v);
static inline float 		rjd_math_vec4_sum(rjd_math_vec4 v);
static inline float 		rjd_math_vec4_dot(rjd_math_vec4 a, rjd_math_vec4 b);
static inline float 		rjd_math_vec4_lengthsq(rjd_math_vec4 v);
static inline float 		rjd_math_vec4_length(rjd_math_vec4 v);
static inline float 		rjd_math_vec4_i(rjd_math_vec4 v, size_t index);
static inline float			rjd_math_vec4_hmin(rjd_math_vec4 v);
static inline float			rjd_math_vec4_hmax(rjd_math_vec4 v);
static inline rjd_math_vec4 rjd_math_vec4_normalize(rjd_math_vec4 v);
static inline rjd_math_vec4 rjd_math_vec4_abs(rjd_math_vec4 v);
static inline rjd_math_vec4 rjd_math_vec4_neg(rjd_math_vec4 v);
static inline rjd_math_vec4 rjd_math_vec4_floor(rjd_math_vec4 v);
static inline rjd_math_vec4 rjd_math_vec4_ceil(rjd_math_vec4 v);
static inline rjd_math_vec4 rjd_math_vec4_round(rjd_math_vec4 v);
static inline rjd_math_vec4 rjd_math_vec4_scale(rjd_math_vec4 v, float s);
static inline rjd_math_vec4 rjd_math_vec4_add(rjd_math_vec4 a, rjd_math_vec4 b);
static inline rjd_math_vec4 rjd_math_vec4_sub(rjd_math_vec4 a, rjd_math_vec4 b);
static inline rjd_math_vec4 rjd_math_vec4_mul(rjd_math_vec4 a, rjd_math_vec4 b);
static inline rjd_math_vec4 rjd_math_vec4_div(rjd_math_vec4 a, rjd_math_vec4 b);
static inline rjd_math_vec4 rjd_math_vec4_min(rjd_math_vec4 a, rjd_math_vec4 b);
static inline rjd_math_vec4 rjd_math_vec4_max(rjd_math_vec4 a, rjd_math_vec4 b);
static inline rjd_math_vec4 rjd_math_vec4_project(rjd_math_vec4 a, rjd_math_vec4 b);
static inline rjd_math_vec4 rjd_math_vec4_lerp(rjd_math_vec4 a, rjd_math_vec4 b, float t);
static inline bool			rjd_math_vec4_eq(rjd_math_vec4 a, rjd_math_vec4 b);
static inline bool			rjd_math_vec4_ge(rjd_math_vec4 a, rjd_math_vec4 b);
static inline float*		rjd_math_vec4_write(rjd_math_vec4 v, float* out);

// vec3

#define rjd_math_vec3_shuffle(v3, x, y, z) ((rjd_math_vec3){_mm_shuffle_ps((v3).v, (v3).v, _MM_SHUFFLE(3, z, y, x))})

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
static inline float			rjd_math_vec3_hmin(rjd_math_vec3 v);
static inline float			rjd_math_vec3_hmax(rjd_math_vec3 v);
static inline rjd_math_vec3 rjd_math_vec3_normalize(rjd_math_vec3 v);
static inline rjd_math_vec3 rjd_math_vec3_abs(rjd_math_vec3 v);
static inline rjd_math_vec3 rjd_math_vec3_neg(rjd_math_vec3 v);
static inline rjd_math_vec4 rjd_math_vec4_floor(rjd_math_vec4 v);
static inline rjd_math_vec4 rjd_math_vec4_ceil(rjd_math_vec4 v);
static inline rjd_math_vec4 rjd_math_vec4_round(rjd_math_vec4 v);
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
static inline bool			rjd_math_vec3_ge(rjd_math_vec3 a, rjd_math_vec3 b);
static inline float*		rjd_math_vec3_write(rjd_math_vec3 v, float* out);
static inline float*		rjd_math_vec3_writefast(rjd_math_vec3 v, float* out); // writes 4 floats to out

// mat4

static inline rjd_math_mat4 rjd_math_mat4_identity(void);
static inline rjd_math_mat4 rjd_math_mat4_translation(rjd_math_vec3 trans);
//static inline rjd_math_mat4 rjd_math_mat4_rotation(rjd_math_quat rot); // TODO quaternions
static inline rjd_math_mat4 rjd_math_mat4_angleaxis(float angle, rjd_math_vec3 axis);
static inline rjd_math_mat4 rjd_math_mat4_rotationx(float angle);
static inline rjd_math_mat4 rjd_math_mat4_rotationy(float angle);
static inline rjd_math_mat4 rjd_math_mat4_rotationz(float angle);
static inline rjd_math_mat4 rjd_math_mat4_rotationbasis(rjd_math_vec3 x, rjd_math_vec3 y, rjd_math_vec3 z);
static inline rjd_math_mat4 rjd_math_mat4_scaling(float scale);
static inline rjd_math_mat4 rjd_math_mat4_scaling_nonuniform(rjd_math_vec3 scale);
static inline rjd_math_mat4 rjd_math_mat4_add(rjd_math_mat4 a, rjd_math_mat4 b);
static inline rjd_math_mat4 rjd_math_mat4_mul(rjd_math_mat4 a, rjd_math_mat4 b);
static inline rjd_math_vec3 rjd_math_mat4_mulv3(rjd_math_mat4 m, rjd_math_vec3 v);
static inline rjd_math_vec4 rjd_math_mat4_mulv4(rjd_math_mat4 m, rjd_math_vec4 v);
static inline rjd_math_mat4 rjd_math_mat4_inv(rjd_math_mat4 m);
static inline rjd_math_mat4 rjd_math_mat4_transpose(rjd_math_mat4 m);
static inline rjd_math_mat4 rjd_math_mat4_frustum_righthanded(float left, float right, float top, float bot, float near_plane, float far_plane);
static inline rjd_math_mat4 rjd_math_mat4_ortho_righthanded(float left, float right, float top, float bot, float near_plane, float far_plane);
static inline rjd_math_mat4 rjd_math_mat4_ortho_lefthanded(float left, float right, float top, float bot, float near_plane, float far_plane);
static inline rjd_math_mat4 rjd_math_mat4_perspective_righthanded(float y_fov, float aspect, float near_plane, float far_plane);
static inline rjd_math_mat4 rjd_math_mat4_perspective_lefthanded(float y_fov, float aspect, float near_plane, float far_plane);
static inline rjd_math_mat4 rjd_math_mat4_lookat_righthanded(rjd_math_vec3 eye, rjd_math_vec3 target, rjd_math_vec3 up);
static inline rjd_math_mat4 rjd_math_mat4_lookat_lefthanded(rjd_math_vec3 eye, rjd_math_vec3 target, rjd_math_vec3 up);
static inline float*		rjd_math_mat4_write_colmajor(rjd_math_mat4 m, float* out);
static inline float*		rjd_math_mat4_write_rowmajor(rjd_math_mat4 m, float* out);

// implementation

RJD_MATH_SIGN_FUNCS(RJD_MATH_DEFINE_SIGN_FUNC)
RJD_MATH_ISEQUAL_FUNCS(RJD_MATH_DEFINE_ISEQUAL_FUNC)
RJD_MATH_MIN_FUNCS(RJD_MATH_DEFINE_MIN_FUNC)
RJD_MATH_MAX_FUNCS(RJD_MATH_DEFINE_MAX_FUNC)
RJD_MATH_CLAMP_FUNCS(RJD_MATH_DEFINE_CLAMP_FUNC)
RJD_MATH_TRUNCATE_FUNCS(RJD_MATH_DEFINE_TRUNCATE_FUNC)
RJD_MATH_REMAP_FUNCS(RJD_MATH_DEFINE_REMAP_FUNC)

static inline uint32_t rjd_math_next_pow2_u32(uint32_t v) 
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

static inline int32_t rjd_math_pow_u32(int32_t v, uint32_t power)
{
	int32_t r = 1;
	while (power) {
		r *= v;
		--power;
	}
	return r;
}

// vec3 <-> vec4 conversion helpers

static inline rjd_math_vec4 rjd_math_vec3to4(rjd_math_vec3 v3) {
	rjd_math_vec4 v4 = { v3.v };
	return v4;
}

static inline rjd_math_vec3 rjd_math_vec4to3(rjd_math_vec4 v4) {
	rjd_math_vec3 v3 = { v4.v };
	return v3;
}

static inline rjd_math_vec4 rjd_math_vec3to4w(rjd_math_vec3 v3, float w) {
	rjd_math_vec4 v4 = rjd_math_vec3to4(v3);
	return rjd_math_vec4_setw(v4, w);
}

static inline rjd_math_vec3 rjd_math_vec4to3w(rjd_math_vec4 v4) {
	v4 = rjd_math_vec4_setw(v4, 0);
	return rjd_math_vec4to3(v4);
}

// vec4

static inline rjd_math_vec4 rjd_math_vec4_zero(void) {
	rjd_math_vec4 v = { _mm_setzero_ps() };
	return v;
}
static inline rjd_math_vec4 rjd_math_vec4_xyzw(float x, float y, float z, float w) {
	rjd_math_vec4 v = { _mm_set_ps(w, z, y, x) }; 
	return v;
}
static inline rjd_math_vec4 rjd_math_vec4_splat(float f) {
	rjd_math_vec4 v = { _mm_set1_ps(f) };
	return v;
}
static inline rjd_math_vec4 rjd_math_vec4_one(void) {
	return rjd_math_vec4_splat(1);
}
static inline rjd_math_vec4 rjd_math_vec4_setx(rjd_math_vec4 v, float x) {
	v.v = _mm_move_ss(v.v, rjd_math_vec4_splat(x).v);
	return v;
}
static inline rjd_math_vec4 rjd_math_vec4_sety(rjd_math_vec4 v, float y) {
	v = rjd_math_vec4_shuffle(v, 1, 0, 2, 3);
	v = rjd_math_vec4_setx(v, y);
	v = rjd_math_vec4_shuffle(v, 1, 0, 2, 3);
	return v;
}
static inline rjd_math_vec4 rjd_math_vec4_setz(rjd_math_vec4 v, float z) {
	v = rjd_math_vec4_shuffle(v, 2, 1, 0, 3);
	v = rjd_math_vec4_setx(v, z);
	v = rjd_math_vec4_shuffle(v, 2, 1, 0, 3);
	return v;
}
static inline rjd_math_vec4 rjd_math_vec4_setw(rjd_math_vec4 v, float w) {
	v = rjd_math_vec4_shuffle(v, 3, 1, 2, 0);
	v = rjd_math_vec4_setx(v, w);
	v = rjd_math_vec4_shuffle(v, 3, 1, 2, 0);
	return v;
}
static inline float rjd_math_vec4_x(rjd_math_vec4 v) {
	return _mm_cvtss_f32(v.v);
}
static inline float rjd_math_vec4_y(rjd_math_vec4 v) {
	return rjd_math_vec4_x(rjd_math_vec4_shuffle(v, 1, 0, 2, 3));
}
static inline float rjd_math_vec4_z(rjd_math_vec4 v) {
	return rjd_math_vec4_x(rjd_math_vec4_shuffle(v, 2, 1, 0, 3));
}
static inline float rjd_math_vec4_w(rjd_math_vec4 v) {
	return rjd_math_vec4_x(rjd_math_vec4_shuffle(v, 3, 1, 2, 0));
}
static inline float rjd_math_vec4_sum(rjd_math_vec4 v) {
	v.v = _mm_hadd_ps(v.v, v.v);
	v.v = _mm_hadd_ps(v.v, v.v);
	return rjd_math_vec4_x(v);
}
static inline float rjd_math_vec4_dot(rjd_math_vec4 a, rjd_math_vec4 b) {
	rjd_math_vec4 product = rjd_math_vec4_mul(a,b);
	return rjd_math_vec4_sum(product);
}
static inline float rjd_math_vec4_lengthsq(rjd_math_vec4 v) {
	return rjd_math_vec4_dot(v, v);
}
static inline float rjd_math_vec4_length(rjd_math_vec4 v) {
	return sqrtf(rjd_math_vec4_lengthsq(v));
}
static inline float rjd_math_vec4_hmin(rjd_math_vec4 v) {
	v = rjd_math_vec4_min(v, rjd_math_vec4_shuffle(v,1,1,2,3));
	v = rjd_math_vec4_min(v, rjd_math_vec4_shuffle(v,2,1,2,3));
	v = rjd_math_vec4_min(v, rjd_math_vec4_shuffle(v,3,1,2,3));
	return rjd_math_vec4_x(v);
}
static inline float rjd_math_vec4_hmax(rjd_math_vec4 v) {
	v = rjd_math_vec4_max(v, rjd_math_vec4_shuffle(v,1,1,2,3));
	v = rjd_math_vec4_max(v, rjd_math_vec4_shuffle(v,2,1,2,3));
	v = rjd_math_vec4_max(v, rjd_math_vec4_shuffle(v,3,1,2,3));
	return rjd_math_vec4_x(v);
}
static inline float rjd_math_vec4_i(rjd_math_vec4 v, size_t index) {
	switch(index) {
		case 0:	v = rjd_math_vec4_shuffle(v,0,0,0,0); break;
		case 1:	v = rjd_math_vec4_shuffle(v,1,1,1,1); break;
		case 2:	v = rjd_math_vec4_shuffle(v,2,2,2,2); break;
		case 3:	v = rjd_math_vec4_shuffle(v,3,3,3,3); break;
		default:
			RJD_ASSERTFAIL("index must be between 0 and 3");
			break;
	}
	return rjd_math_vec4_x(v);
}
static inline rjd_math_vec4 rjd_math_vec4_normalize(rjd_math_vec4 v) {
	float length = rjd_math_vec4_length(v);
	RJD_ASSERT(length != 0);
	rjd_math_vec4 l = rjd_math_vec4_splat(length);
	return rjd_math_vec4_div(v, l);
}
static inline rjd_math_vec4 rjd_math_vec4_abs(rjd_math_vec4 v) {
	__m128 signbits = _mm_set1_ps(-0.0f);
	v.v = _mm_andnot_ps(signbits, v.v); // Remove the sign bit
	return v;
}
static inline rjd_math_vec4 rjd_math_vec4_neg(rjd_math_vec4 v) {
	return rjd_math_vec4_scale(v, -1);
}
static inline rjd_math_vec4 rjd_math_vec4_floor(rjd_math_vec4 v) {
	v.v = _mm_floor_ps(v.v);
	return v;
}
static inline rjd_math_vec4 rjd_math_vec4_ceil(rjd_math_vec4 v) {
	v.v = _mm_ceil_ps(v.v);
	return v;
}
static inline rjd_math_vec4 rjd_math_vec4_round(rjd_math_vec4 v) {
	v.v = _mm_round_ps(v.v, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
	return v;
}
static inline rjd_math_vec4 rjd_math_vec4_scale(rjd_math_vec4 v, float s) {
	rjd_math_vec4 scales = rjd_math_vec4_splat(s);
	return rjd_math_vec4_mul(v, scales);
}
static inline rjd_math_vec4 rjd_math_vec4_add(rjd_math_vec4 a, rjd_math_vec4 b) {
	a.v = _mm_add_ps(a.v, b.v);
	return a;
}
static inline rjd_math_vec4 rjd_math_vec4_sub(rjd_math_vec4 a, rjd_math_vec4 b) {
	a.v = _mm_sub_ps(a.v, b.v);
	return a;
}
static inline rjd_math_vec4 rjd_math_vec4_mul(rjd_math_vec4 a, rjd_math_vec4 b) {
	a.v = _mm_mul_ps(a.v, b.v);
	return a;
}
static inline rjd_math_vec4 rjd_math_vec4_div(rjd_math_vec4 a, rjd_math_vec4 b) {
	a.v = _mm_div_ps(a.v, b.v);
	return a;
}
static inline rjd_math_vec4 rjd_math_vec4_min(rjd_math_vec4 a, rjd_math_vec4 b) {
	a.v = _mm_min_ps(a.v, b.v);
	return a;
}
static inline rjd_math_vec4 rjd_math_vec4_max(rjd_math_vec4 a, rjd_math_vec4 b) {
	a.v = _mm_max_ps(a.v, b.v);
	return a;
}
static inline rjd_math_vec4 rjd_math_vec4_project(rjd_math_vec4 a, rjd_math_vec4 b) {
	float dot = rjd_math_vec4_dot(a, b);
	float lb = rjd_math_vec4_length(b);
	return rjd_math_vec4_scale(b, dot / lb);
}
static inline rjd_math_vec4 rjd_math_vec4_lerp(rjd_math_vec4 a, rjd_math_vec4 b, float t) {
	rjd_math_vec4 v = rjd_math_vec4_sub(b, a);
	v = rjd_math_vec4_scale(v, t);
	v = rjd_math_vec4_add(v, a);
	return v;
}
static inline bool rjd_math_vec4_eq(rjd_math_vec4 a, rjd_math_vec4 b) {
	return (_mm_movemask_ps(_mm_cmpeq_ps(a.v, b.v)) & 0xF) == 0xF;
}
static inline bool rjd_math_vec4_ge(rjd_math_vec4 a, rjd_math_vec4 b) {
	return (_mm_movemask_ps(_mm_cmpge_ps(a.v, b.v)) & 0xF) == 0xF;
}
static inline float* rjd_math_vec4_write(rjd_math_vec4 v, float* out) {
	RJD_ASSERT(RJD_MEM_ISALIGNED(out, 16));
	_mm_stream_ps(out,  v.v);
	return out + 4;
}

// vec3

static inline rjd_math_vec3 rjd_math_vec3_zero(void) { 
	return rjd_math_vec4to3(rjd_math_vec4_zero());
}
static inline rjd_math_vec3 rjd_math_vec3_xyz(float x, float y, float z) {
	return rjd_math_vec4to3(rjd_math_vec4_xyzw(x,y,z,0));
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
	return rjd_math_vec4to3(rjd_math_vec4_sety(rjd_math_vec3to4(v), y));
}
static inline rjd_math_vec3 rjd_math_vec3_setz(rjd_math_vec3 v, float z) {
	return rjd_math_vec4to3(rjd_math_vec4_setz(rjd_math_vec3to4(v), z));
}
static inline float rjd_math_vec3_x(rjd_math_vec3 v) {
	return rjd_math_vec4_x(rjd_math_vec3to4(v));
}
static inline float rjd_math_vec3_y(rjd_math_vec3 v) {
	return rjd_math_vec4_y(rjd_math_vec3to4(v));
}
static inline float rjd_math_vec3_z(rjd_math_vec3 v) {
	return rjd_math_vec4_z(rjd_math_vec3to4(v));
}
static inline float rjd_math_vec3_sum(rjd_math_vec3 v) {
	return rjd_math_vec4_sum(rjd_math_vec3to4(v));
}
static inline float rjd_math_vec3_dot(rjd_math_vec3 a, rjd_math_vec3 b) {
	return rjd_math_vec4_dot(rjd_math_vec3to4(a), rjd_math_vec3to4(b));
}
static inline float rjd_math_vec3_angle(rjd_math_vec3 a, rjd_math_vec3 b) {
	float dot = rjd_math_vec3_dot(a, b);
	float la = rjd_math_vec3_length(a);
	float lb = rjd_math_vec3_length(b);
	return acosf(dot / (la * lb));
}
static inline float rjd_math_vec3_lengthsq(rjd_math_vec3 v) {
	return rjd_math_vec4_lengthsq(rjd_math_vec3to4(v));
}
static inline float rjd_math_vec3_length(rjd_math_vec3 v) {
	return rjd_math_vec4_length(rjd_math_vec3to4(v));
}
static inline float rjd_math_vec3_hmin(rjd_math_vec3 v) {
	v = rjd_math_vec3_min(v, rjd_math_vec3_shuffle(v,1,1,2));
	v = rjd_math_vec3_min(v, rjd_math_vec3_shuffle(v,2,1,2));
	return rjd_math_vec3_x(v);
}
static inline float rjd_math_vec3_hmax(rjd_math_vec3 v) {
	v = rjd_math_vec3_max(v, rjd_math_vec3_shuffle(v,1,1,2));
	v = rjd_math_vec3_max(v, rjd_math_vec3_shuffle(v,2,1,2));
	return rjd_math_vec3_x(v);
}
static inline rjd_math_vec3 rjd_math_vec3_normalize(rjd_math_vec3 v) {
	return rjd_math_vec4to3(rjd_math_vec4_normalize(rjd_math_vec3to4(v)));
}
static inline rjd_math_vec3 rjd_math_vec3_abs(rjd_math_vec3 v) {
	return rjd_math_vec4to3(rjd_math_vec4_abs(rjd_math_vec3to4(v)));
}
static inline rjd_math_vec3 rjd_math_vec3_neg(rjd_math_vec3 v) {
	return rjd_math_vec4to3(rjd_math_vec4_neg(rjd_math_vec3to4(v)));
}
static inline rjd_math_vec3 rjd_math_vec3_floor(rjd_math_vec3 v) {
	return rjd_math_vec4to3(rjd_math_vec4_floor(rjd_math_vec3to4(v)));
}
static inline rjd_math_vec3 rjd_math_vec3_ceil(rjd_math_vec3 v) {
	return rjd_math_vec4to3(rjd_math_vec4_ceil(rjd_math_vec3to4(v)));
}
static inline rjd_math_vec3 rjd_math_vec3_round(rjd_math_vec3 v) {
	return rjd_math_vec4to3(rjd_math_vec4_abs(rjd_math_vec3to4(v)));
}
static inline rjd_math_vec3 rjd_math_vec3_scale(rjd_math_vec3 v, float s) {
	return rjd_math_vec4to3(rjd_math_vec4_scale(rjd_math_vec3to4(v), s));
}
static inline rjd_math_vec3 rjd_math_vec3_add(rjd_math_vec3 a, rjd_math_vec3 b) {
	return rjd_math_vec4to3(rjd_math_vec4_add(rjd_math_vec3to4(a), rjd_math_vec3to4(b)));
}
static inline rjd_math_vec3 rjd_math_vec3_sub(rjd_math_vec3 a, rjd_math_vec3 b) {
	return rjd_math_vec4to3(rjd_math_vec4_sub(rjd_math_vec3to4(a), rjd_math_vec3to4(b)));
}
static inline rjd_math_vec3 rjd_math_vec3_mul(rjd_math_vec3 a, rjd_math_vec3 b) {
	return rjd_math_vec4to3(rjd_math_vec4_mul(rjd_math_vec3to4(a), rjd_math_vec3to4(b)));
}
static inline rjd_math_vec3 rjd_math_vec3_div(rjd_math_vec3 a, rjd_math_vec3 b) {
	return rjd_math_vec4to3(rjd_math_vec4_div(rjd_math_vec3to4(a), rjd_math_vec3to4(b)));
}
static inline rjd_math_vec3 rjd_math_vec3_cross(rjd_math_vec3 a, rjd_math_vec3 b) {
	rjd_math_vec3 ap = rjd_math_vec3_mul(rjd_math_vec3_yzx(a), rjd_math_vec3_zxy(b));
	rjd_math_vec3 bp = rjd_math_vec3_mul(rjd_math_vec3_zxy(a), rjd_math_vec3_yzx(b));
	return rjd_math_vec3_sub(ap, bp);
}
static inline rjd_math_vec3 rjd_math_vec3_min(rjd_math_vec3 a, rjd_math_vec3 b) {
	return rjd_math_vec4to3(rjd_math_vec4_min(rjd_math_vec3to4(a), rjd_math_vec3to4(b)));
}
static inline rjd_math_vec3 rjd_math_vec3_max(rjd_math_vec3 a, rjd_math_vec3 b) {
	return rjd_math_vec4to3(rjd_math_vec4_max(rjd_math_vec3to4(a), rjd_math_vec3to4(b)));
}
static inline rjd_math_vec3 rjd_math_vec3_project(rjd_math_vec3 a, rjd_math_vec3 b) {
	return rjd_math_vec4to3(rjd_math_vec4_project(rjd_math_vec3to4(a), rjd_math_vec3to4(b)));
}
static inline rjd_math_vec3 rjd_math_vec3_reflect(rjd_math_vec3 v, rjd_math_vec3 n) {
	RJD_ASSERT(rjd_math_vec3_eq(n, rjd_math_vec3_normalize(n)));
	rjd_math_vec3 projected = rjd_math_vec3_scale(n, 2 * rjd_math_vec3_dot(v, n));
	return rjd_math_vec3_sub(v, projected);
}
static inline rjd_math_vec3 rjd_math_vec3_lerp(rjd_math_vec3 a, rjd_math_vec3 b, float t) {
	return rjd_math_vec4to3(rjd_math_vec4_lerp(rjd_math_vec3to4(a), rjd_math_vec3to4(b), t));
}
static inline bool rjd_math_vec3_eq(rjd_math_vec3 a, rjd_math_vec3 b) {
	return (_mm_movemask_ps(_mm_cmpeq_ps(a.v, b.v)) & 7) == 7; // 7 is the platform-independent version of 0b111
}
static inline bool rjd_math_vec3_ge(rjd_math_vec3 a, rjd_math_vec3 b) {
	return (_mm_movemask_ps(_mm_cmpge_ps(a.v, b.v)) & 7) == 7;
}
static inline float* rjd_math_vec3_write(rjd_math_vec3 v, float* out) {
	RJD_FORCE_ALIGN(16, float) tmp[4];
	_mm_stream_ps(tmp, v.v);
	memcpy(out, tmp, sizeof(float) * 3);
	return out + 3;
}
static inline float* rjd_math_vec3_writefast(rjd_math_vec3 v, float* out) {
	RJD_ASSERT(RJD_MEM_ISALIGNED(out, 16));
	_mm_stream_ps(out, v.v);
	return out + 3;
}

// mat4

static inline rjd_math_mat4 rjd_math_mat4_identity(void) {
	rjd_math_mat4 m;
	m.m[0] = rjd_math_vec4_xyzw(1,0,0,0);
	m.m[1] = rjd_math_vec4_xyzw(0,1,0,0);
	m.m[2] = rjd_math_vec4_xyzw(0,0,1,0);
	m.m[3] = rjd_math_vec4_xyzw(0,0,0,1);
	return m;
}
static inline rjd_math_mat4 rjd_math_mat4_translation(rjd_math_vec3 trans) {
	rjd_math_mat4 m;
	m.m[0] = rjd_math_vec4_xyzw(1,0,0,0);
	m.m[1] = rjd_math_vec4_xyzw(0,1,0,0);
	m.m[2] = rjd_math_vec4_xyzw(0,0,1,0);
	m.m[3] = rjd_math_vec3to4w(trans, 1);
	return m;
}
static inline rjd_math_mat4 rjd_math_mat4_angleaxis(float angle, rjd_math_vec3 axis) {
	rjd_math_mat4 m;

	// TODO optimize
	float c = cosf(angle);
	float k = 1 - c;
	float s = sinf(angle);

	float x = rjd_math_vec3_x(axis);
	float y = rjd_math_vec3_y(axis);
	float z = rjd_math_vec3_z(axis);

	//rjd_math_vec4 axis4 = rjd_math_vec3to4w(axis,1); // x,y,z,1
	//rjd_math_vec4 tmp1 = rjd_math_vec4_mul(axis4, rjd_math_vec4_xyzw(s,s,s,c)); // x*s, y*s, z*s, c
	//rjd_math_vec4 vk = rjd_math_vec4_splat(k);

	//rjd_math_vec4 v0 = rjd_math_vec4_mul(axis4, rjd_math_vec4_shuffle(axis4,0,0,0,3));
	//v0 = rjd_math_vec4_mul(v0, vk);

	//v0 = rjd_math_vec4_add(v0, rjd_math_vec4_shuffle(tmp1,3,2,1));

	//m.m[0] = 


	//vec3 diagonal = rjd_math_vec3_mul(axis, axis);
	//diagonal = rjd_math_vec3_mul(diagonal, veck);
	//diagonal = rjd_math_vec3_add(diagonal, rjd_math_vec3_splat(c));
	
	float m00 = k*x*x + c;
	float m10 = k*x*y + z*s;
	float m20 = k*x*z - y*s;

	float m01 = k*x*y - z*s;
	float m11 = k*y*y + c;
	float m21 = k*y*z + x*s;

	float m02 = k*x*z + y*s;
	float m12 = k*y*z - x*s;
	float m22 = k*z*z + c;

	m.m[0] = rjd_math_vec4_xyzw(m00, m10, m20, 0);
	m.m[1] = rjd_math_vec4_xyzw(m01, m11, m21, 0);
	m.m[2] = rjd_math_vec4_xyzw(m02, m12, m22, 0);
	m.m[3] = rjd_math_vec4_xyzw(  0,   0,   0, 1);
	return m;
}
static inline rjd_math_mat4 rjd_math_mat4_rotationx(float angle) {
	return rjd_math_mat4_angleaxis(angle, rjd_math_vec3_right());
}
static inline rjd_math_mat4 rjd_math_mat4_rotationy(float angle) {
	return rjd_math_mat4_angleaxis(angle, rjd_math_vec3_up());
}
static inline rjd_math_mat4 rjd_math_mat4_rotationz(float angle) {
	return rjd_math_mat4_angleaxis(angle, rjd_math_vec3_forward());
}
static inline rjd_math_mat4 rjd_math_mat4_rotationbasis(rjd_math_vec3 x, rjd_math_vec3 y, rjd_math_vec3 z) {
	rjd_math_vec4 xx = rjd_math_vec3to4(x);
	rjd_math_vec4 yy = rjd_math_vec3to4(y);
	rjd_math_vec4 zz = rjd_math_vec3to4(z);
	rjd_math_mat4 m = { { xx, yy, zz, rjd_math_vec4_xyzw(0,0,0,1) } };
	return m;
}
static inline rjd_math_mat4 rjd_math_mat4_scaling(float s) {
	rjd_math_mat4 m;
	m.m[0] = rjd_math_vec4_xyzw(s,0,0,0);
	m.m[1] = rjd_math_vec4_xyzw(0,s,0,0);
	m.m[2] = rjd_math_vec4_xyzw(0,0,s,0);
	m.m[3] = rjd_math_vec4_xyzw(0,0,0,1);
	return m;
}
static inline rjd_math_mat4 rjd_math_mat4_scaling_nonuniform(rjd_math_vec3 scale) {
	float x = rjd_math_vec3_x(scale);
	float y = rjd_math_vec3_y(scale);
	float z = rjd_math_vec3_z(scale);

	rjd_math_mat4 m;
	m.m[0] = rjd_math_vec4_xyzw(x,0,0,0);
	m.m[1] = rjd_math_vec4_xyzw(0,y,0,0);
	m.m[2] = rjd_math_vec4_xyzw(0,0,z,0);
	m.m[3] = rjd_math_vec4_xyzw(0,0,0,1);
	return m;
}
static inline rjd_math_mat4 rjd_math_mat4_add(rjd_math_mat4 a, rjd_math_mat4 b) {
    rjd_math_mat4 m = {0};
	for (size_t i = 0; i < rjd_countof(m.m); ++i) {
		m.m[i] = rjd_math_vec4_add(a.m[i], b.m[i]);
	}
	return m;
}
static inline rjd_math_mat4 rjd_math_mat4_mul(rjd_math_mat4 a, rjd_math_mat4 b) {
    rjd_math_mat4 m = {0};
	for (size_t i = 0; i < rjd_countof(m.m); ++i) {
		m.m[i] = rjd_math_mat4_mulv4(a, b.m[i]);
	}
	return m;
}
static inline rjd_math_vec3 rjd_math_mat4_mulv3(rjd_math_mat4 m, rjd_math_vec3 v) {
	return rjd_math_vec4to3(rjd_math_mat4_mulv4(m, rjd_math_vec3to4w(v, 1)));
}
static inline rjd_math_vec4 rjd_math_mat4_mulv4(rjd_math_mat4 m, rjd_math_vec4 v) {
	// TODO optimize
	rjd_math_mat4 trans = rjd_math_mat4_transpose(m);
	float x = rjd_math_vec4_dot(trans.m[0], v);
	float y = rjd_math_vec4_dot(trans.m[1], v);
	float z = rjd_math_vec4_dot(trans.m[2], v);
	float w = rjd_math_vec4_dot(trans.m[3], v);
	return rjd_math_vec4_xyzw(x, y, z, w);
}
static inline rjd_math_mat4 rjd_math_mat4_inv(rjd_math_mat4 m) {
	rjd_math_mat4 t = rjd_math_mat4_transpose(m);
	rjd_math_vec4 t0 = t.m[0];
	rjd_math_vec4 t1 = t.m[1];
	rjd_math_vec4 t2 = t.m[2];
	rjd_math_vec4 t3 = t.m[3];
	
	rjd_math_mat4 inv;
	rjd_math_vec4 term;

	// first column
	//inv0.x = m11m22m33 + m12m23m31 + m13m21m32 - m11m23m32 - m12m21m33 - m13m22m31;
	//inv0.y = m10m23m32 + m12m20m33 + m13m22m30 - m10m22m33 - m12m23m30 - m13m20m32;
	//inv0.z = m10m21m31 + m11m23m30 + m13m20m31 - m10m23m30 - m11m20m33 - m13m21m30;
	//inv0.w = m10m22m31 + m11m20m32 + m12m21m30 - m10m21m32 - m11m22m30 - m12m20m31;
	//																		x		  y			z		  w
	term = rjd_math_vec4_shuffle(t1,1,0,0,0);							// m11,		 m10,      m10,      m10
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t2,2,3,1,2)); 	// m11m22,   m10m23,   m10m21,   m10m22
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t3,3,2,1,1)); 	// m11m22m33,m10m23m32,m10m21m31,m10m22m31
	inv.m[0] = term;

	term = rjd_math_vec4_shuffle(t1,2,2,1,1);							// m12,      m12,      m11,      m11
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t2,3,0,3,0)); 	// m12m23,   m12m20,   m11m23,   m11m20
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t3,1,3,0,2)); 	// m12m23m31,m12m20m33,m11m23m30,m11m20m32
	inv.m[0] = rjd_math_vec4_add(inv.m[0], term);

	term = rjd_math_vec4_shuffle(t1,3,3,3,2);							// m13,      m13,      m13,      m12
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t2,2,3,1,2)); 	// m13m21,   m13m22,   m13m20,   m12m21
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t3,3,2,1,1)); 	// m13m21m32,m13m22m30,m13m20m31,m12m21m30
	inv.m[0] = rjd_math_vec4_add(inv.m[0], term);

	term = rjd_math_vec4_shuffle(t1,1,0,0,0);							// m11,      m10,      m10,      m10
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t2,3,2,3,1)); 	// m11m23,   m10m22,   m10m23,   m10m21
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t3,2,3,0,2)); 	// m11m23m32,m10m22m33,m10m23m30,m10m21m32
	inv.m[0] = rjd_math_vec4_sub(inv.m[0], term);

	term = rjd_math_vec4_shuffle(t1,2,2,1,1);							// m12,      m12,      m11,      m11
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t2,1,3,0,2)); 	// m12m21,   m12m23,   m11m20,   m11m22
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t3,3,0,3,0)); 	// m12m21m33,m12m23m30,m11m20m33,m11m22m30
	inv.m[0] = rjd_math_vec4_sub(inv.m[0], term);

	term = rjd_math_vec4_shuffle(t1,3,3,3,2);							// m13,      m13,      m13,      m12
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t2,2,0,1,0)); 	// m13m22,   m13m20,   m13m21,   m12m20
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t3,1,2,0,1)); 	// m13m22m31,m13m20m32,m13m21m30,m12m20m31
	inv.m[0] = rjd_math_vec4_sub(inv.m[0], term);

	// second column
	// inv.m[1].x = m01m23m32 + m02m21m33 + m03m22m31 - m01m22m33 - m02m23m31 - m03m21m32
	// inv.m[1].y = m00m22m33 + m02m23m30 + m03m20m30 - m00m23m32 - m02m20m33 - m03m22m30
	// inv.m[1].z = m00m23m31 + m01m20m33 + m03m21m30 - m00m21m33 - m01m23m30 - m03m20m31
	// inv.m[1].w = m00m21m32 + m01m22m30 + m02m20m31 - m00m22m31 - m01m20m32 - m02m21m30
	term = rjd_math_vec4_shuffle(t0,1,0,0,0);                         // m01,      m00,      m00,      m00
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t2,3,2,3,1)); // m01m23,   m00m22,   m00m23,   m00m21
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t3,2,3,1,2)); // m01m23m32,m00m22m33,m00m23m31,m00m21m32
	inv.m[1] = term;

	term = rjd_math_vec4_shuffle(t0,2,2,1,1);                         // m02,      m02,      m01,      m01
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t2,1,3,0,2)); // m02m21,   m02m23,   m01m20,   m01m22
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t3,3,0,3,0)); // m02m21m33,m02m23m30,m01m20m33,m01m22m30
	inv.m[1] = rjd_math_vec4_add(inv.m[1], term);

	term = rjd_math_vec4_shuffle(t0,3,3,3,2);                         // m03,      m03,      m03,      m02
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t2,2,0,1,0)); // m03m22,   m03m20,   m03m21,   m02m20
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t3,1,0,0,1)); // m03m22m31,m03m20m30,m03m21m30,m02m20m31
	inv.m[1] = rjd_math_vec4_add(inv.m[1], term);

	term = rjd_math_vec4_shuffle(t0,1,0,0,0);                         // m01,      m00,      m00,      m00
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t2,2,3,1,2)); // m01m22,   m00m23,   m00m21,   m00m22
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t3,3,2,3,1)); // m01m22m33,m00m23m32,m00m21m33,m00m22m31
	inv.m[1] = rjd_math_vec4_sub(inv.m[1], term);

	term = rjd_math_vec4_shuffle(t0,2,2,1,1);                         // m02,      m02,      m01,      m01
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t2,3,0,3,0)); // m02m23,   m02m20,   m01m23,   m01m20
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t3,1,3,0,2)); // m02m23m31,m02m20m33,m01m23m30,m01m20m32
	inv.m[1] = rjd_math_vec4_sub(inv.m[1], term);

	term = rjd_math_vec4_shuffle(t0,3,3,3,2);                         // m03,      m03,      m03,      m02
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t2,1,2,0,1)); // m03m21,   m03m22,   m03m20,   m02m21
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t3,2,0,1,0)); // m03m21m32,m03m22m30,m03m20m31,m02m21m30
	inv.m[1] = rjd_math_vec4_sub(inv.m[1], term);

	// third column
	// inv.m[2].x = m01m12m33 + m02m13m31 + m03m11m32 - m01m13m32 - m02m11m33 - m03m12m31
	// inv.m[2].y = m00m13m32 + m02m10m33 + m03m12m30 - m00m12m33 - m02m13m30 - m03m10m32
	// inv.m[2].z = m00m11m33 + m01m13m30 + m03m10m31 - m00m13m31 - m01m10m33 - m01m11m30
	// inv.m[2].w = m00m12m31 + m01m10m32 + m02m11m30 - m00m11m32 - m01m12m30 - m02m10m31
	term = rjd_math_vec4_shuffle(t0,1,0,0,0);                          // m01       m00       m00       m00
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t1,2,3,1,2)); // m01m12    m00m13    m00m11    m00m12
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t3,3,2,3,1)); // m01m12m33 m00m13m32 m00m11m33 m00m12m31
	inv.m[2] = term;

	term = rjd_math_vec4_shuffle(t0,2,2,1,1);                          // m02       m02       m01       m01
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t1,3,0,3,0)); // m02m13    m02m10    m01m13    m01m10
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t3,1,3,0,2)); // m02m13m31 m02m10m33 m01m13m30 m01m10m32
	inv.m[2] = rjd_math_vec4_add(inv.m[2], term);

	term = rjd_math_vec4_shuffle(t0,3,3,3,2);                          // m03       m03       m03       m02
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t1,1,2,0,1)); // m03m11    m03m12    m03m10    m02m11
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t3,2,0,1,0)); // m03m11m32 m03m12m30 m03m10m31 m02m11m30
	inv.m[2] = rjd_math_vec4_add(inv.m[2], term);

	term = rjd_math_vec4_shuffle(t0,1,0,0,0);                          // m01       m00       m00       m00
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t1,3,2,3,1)); // m01m13    m00m12    m00m13    m00m11
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t3,2,3,1,2)); // m01m13m32 m00m12m33 m00m13m31 m00m11m32
	inv.m[2] = rjd_math_vec4_sub(inv.m[2], term);

	term = rjd_math_vec4_shuffle(t0,2,2,1,1);                          // m02       m02       m01       m01
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t1,1,3,0,2)); // m02m11    m02m13    m01m10    m01m12
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t3,3,0,3,0)); // m02m11m33 m02m13m30 m01m10m33 m01m12m30
	inv.m[2] = rjd_math_vec4_sub(inv.m[2], term);

	term = rjd_math_vec4_shuffle(t0,3,3,1,2);                          // m03       m03       m01       m02
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t1,2,0,1,0)); // m03m12    m03m10    m01m11    m02m10
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t3,1,2,0,1)); // m03m12m31 m03m10m32 m01m11m30 m02m10m31
	inv.m[2] = rjd_math_vec4_sub(inv.m[2], term);	

	// fourth column
	// inv.m[3].x = m01m13m22 + m02m11m23 + m03m12m21 - m01m12m23 - m02m13m21 - m03m11m22
	// inv.m[3].y = m00m12m23 + m02m13m20 + m03m10m22 - m00m13m22 - m02m10m23 - m03m12m20
	// inv.m[3].z = m00m13m21 + m01m13m23 + m03m11m20 - m00m11m23 - m01m13m20 - m03m10m21
	// inv.m[3].w = m00m11m22 + m01m12m20 + m02m10m21 - m00m12m21 - m01m10m22 - m02m11m20
	term = rjd_math_vec4_shuffle(t0,1,0,0,0);                          // m01 m00 m00 m00
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t1,3,2,3,1)); // m01m13 m00m12 m00m13 m00m11
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t2,2,3,1,2)); // m01m13m22 m00m12m23 m00m13m21 m00m11m22
	inv.m[3] = term;

	term = rjd_math_vec4_shuffle(t0,2,2,1,1);                          // m02 m02 m01 m01
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t1,1,3,3,2)); // m02m11 m02m13 m01m13 m01m12
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t2,3,0,3,0)); // m02m11m23 m02m13m20 m01m13m23 m01m12m20
	inv.m[3] = rjd_math_vec4_add(inv.m[3], term);

	term = rjd_math_vec4_shuffle(t0,3,3,3,2);                          // m03 m03 m03 m02
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t1,2,0,1,0)); // m03m12 m03m10 m03m11 m02m10
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t2,1,2,0,1)); // m03m12m21 m03m10m22 m03m11m20 m02m10m21
	inv.m[3] = rjd_math_vec4_add(inv.m[3], term);

	term = rjd_math_vec4_shuffle(t0,1,0,0,0);                          // m01 m00 m00 m00
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t1,2,3,1,2)); // m01m12 m00m13 m00m11 m00m12
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t2,3,2,3,1)); // m01m12m23 m00m13m22 m00m11m23 m00m12m21
	inv.m[3] = rjd_math_vec4_sub(inv.m[3], term);

	term = rjd_math_vec4_shuffle(t0,2,2,1,1);                          // m02 m02 m01 m01
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t1,3,0,3,0)); // m02m13 m02m10 m01m13 m01m10
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t2,1,3,0,2)); // m02m13m21 m02m10m23 m01m13m20 m01m10m22
	inv.m[3] = rjd_math_vec4_sub(inv.m[3], term);

	term = rjd_math_vec4_shuffle(t0,3,3,3,2);                          // m03 m03 m03 m02
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t1,1,2,0,1)); // m03m11 m03m12 m03m10 m02m11
	term = rjd_math_vec4_mul(term, rjd_math_vec4_shuffle(t2,2,0,1,0)); // m03m11m22 m03m12m20 m03m10m21 m02m11m20
	inv.m[3] = rjd_math_vec4_sub(inv.m[3], term);

	rjd_math_mat4 trans = rjd_math_mat4_transpose(inv);
	rjd_math_vec4 det = rjd_math_vec4_mul(trans.m[0], m.m[0]);

	det.v = _mm_hadd_ps(det.v, det.v);
	det.v = _mm_hadd_ps(det.v, det.v);
	det = rjd_math_vec4_shuffle(det,0,0,0,0);

	RJD_ASSERTMSG(!rjd_math_isequal(rjd_math_vec4_x(det), 0), "Matrix is not invertible - if you're not sure, check rjd_math_mat4_det() == 0 beforehand");

	rjd_math_vec4 det_reciprocal = {_mm_rcp_ps(det.v)};

    rjd_math_mat4 out = {0};
	for (size_t i = 0; i < rjd_countof(out.m); ++i) {
		out.m[i] = rjd_math_vec4_mul(det_reciprocal, inv.m[i]);
	}

	// NOTE that intel's implementation is 82 intrinsics while this is at least 140(cofactor) + 9(determinant) + 
	// 		12(transpose) + 12(transpose), but this implementation is more straightforward

	return out;
}
static inline rjd_math_mat4 rjd_math_mat4_transpose(rjd_math_mat4 m) {
	_MM_TRANSPOSE4_PS(m.m[0].v, m.m[1].v, m.m[2].v, m.m[3].v);
	return m;
}
static inline rjd_math_mat4 rjd_math_mat4_frustum_righthanded(float left, float right, float top, float bot, float near_plane, float far_plane) {
	float np = near_plane;
	float fp = far_plane;

	rjd_math_mat4 m = {0};
	m.m[0] = rjd_math_vec4_xyzw(2*np/(right-left),			0,						0,					0);
	m.m[1] = rjd_math_vec4_xyzw(0,							2*np/(top-bot),			0,					0);
	m.m[2] = rjd_math_vec4_xyzw((right+left)/(right-left),	(top+bot)/(top-bot),	-(fp+np)/(fp-np), 	-1);
	m.m[3] = rjd_math_vec4_xyzw(0,							0,						-2*fp*np/(fp-np),	0);
	return m;;
}
static inline rjd_math_mat4 rjd_math_mat4_ortho_righthanded(float left, float right, float top, float bot, float near_plane, float far_plane) {
	float np = near_plane;
	float fp = far_plane;

	rjd_math_mat4 m;
	m.m[0] = rjd_math_vec4_xyzw(           2/(right-left),                   0,                0, 0);
	m.m[1] = rjd_math_vec4_xyzw(                        0,         2/(top-bot),                0, 0);
	m.m[2] = rjd_math_vec4_xyzw(                        0,                   0,    1/(np-fp),  0);
	m.m[3] = rjd_math_vec4_xyzw((left+right)/(left-right), (top+bot)/(bot-top),	   np/(np-fp), 1);
	return m;
}
static inline rjd_math_mat4 rjd_math_mat4_ortho_lefthanded(float left, float right, float top, float bot, float near_plane, float far_plane) {
	RJD_ASSERTFAIL("not implemented");
	RJD_UNUSED_PARAM(left);
	RJD_UNUSED_PARAM(right);
	RJD_UNUSED_PARAM(top);
	RJD_UNUSED_PARAM(bot);
	RJD_UNUSED_PARAM(near_plane);
	RJD_UNUSED_PARAM(far_plane);
	return rjd_math_mat4_identity();
}
static inline rjd_math_mat4 rjd_math_mat4_perspective_righthanded(float y_fov, float aspect, float near_plane, float far_plane) {
    float scale = tanf(y_fov * 0.5f * RJD_MATH_PI / 180.0f) * near_plane; 
    float right = aspect * scale;
	float left = -right; 
	float top = scale;
	float bot = -top;
	return rjd_math_mat4_frustum_righthanded(left, right, top, bot, near_plane, far_plane);
}
static inline rjd_math_mat4 rjd_math_mat4_perspective_lefthanded(float y_fov, float aspect, float near_plane, float far_plane) {
	RJD_ASSERTFAIL("not implemented");
	RJD_UNUSED_PARAM(y_fov);
	RJD_UNUSED_PARAM(aspect);
	RJD_UNUSED_PARAM(near_plane);
	RJD_UNUSED_PARAM(far_plane);
	return rjd_math_mat4_identity();
}
static inline rjd_math_mat4 rjd_math_mat4_lookat_righthanded(rjd_math_vec3 eye, rjd_math_vec3 target, rjd_math_vec3 up) {
	rjd_math_vec3 forward = rjd_math_vec3_normalize(rjd_math_vec3_sub(target, eye));
	rjd_math_vec3 left = rjd_math_vec3_normalize(rjd_math_vec3_cross(forward, up));
	up = rjd_math_vec3_normalize(rjd_math_vec3_cross(left, forward));

	rjd_math_mat4 rot = rjd_math_mat4_rotationbasis(left, up, rjd_math_vec3_neg(forward));
	rjd_math_mat4 trans = rjd_math_mat4_translation(rjd_math_vec3_neg(eye));
	return rjd_math_mat4_mul(trans, rjd_math_mat4_transpose(rot));
}
static inline rjd_math_mat4 rjd_math_mat4_lookat_lefthanded(rjd_math_vec3 eye, rjd_math_vec3 target, rjd_math_vec3 up) {
	RJD_ASSERTFAIL("not implemented");
	RJD_UNUSED_PARAM(eye);
	RJD_UNUSED_PARAM(target);
	RJD_UNUSED_PARAM(up);
	return rjd_math_mat4_identity();
}
static inline float* rjd_math_mat4_write_colmajor(rjd_math_mat4 m, float* out) {
	RJD_ASSERT(RJD_MEM_ISALIGNED(out, 16));
	_mm_stream_ps(out + 0,  m.m[0].v);
	_mm_stream_ps(out + 4,  m.m[1].v);
	_mm_stream_ps(out + 8,  m.m[2].v);
	_mm_stream_ps(out + 12, m.m[3].v);
	return out + 16;
}
static inline float* rjd_math_mat4_write_rowmajor(rjd_math_mat4 m, float* out) {
	rjd_math_mat4 transpose = rjd_math_mat4_transpose(m);
	return rjd_math_mat4_write_colmajor(transpose, out);
}

