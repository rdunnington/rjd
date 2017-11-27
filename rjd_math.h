#pragma once

#define RJD_MATH 1

// TODO vec
// TODO mat3
// TODO mat4

uint32_t next_pow2(uint32_t v);

#define DEFINE_MIN_FUNC(name, type) inline type name(type a, type b) { return (a < b) ? a : b; }
	DEFINE_MIN_FUNC(min32, int32_t)
	DEFINE_MIN_FUNC(minu32, uint32_t)
	DEFINE_MIN_FUNC(min64, int64_t)
	DEFINE_MIN_FUNC(minu64, uint64_t)
#undef DEFINE_MIN_FUNC

#define DEFINE_MAX_FUNC(name, type) inline type name(type a, type b) { return (a < b) ? b : a; }
	DEFINE_MAX_FUNC(max32, int32_t)
	DEFINE_MAX_FUNC(maxu32, uint32_t)
	DEFINE_MAX_FUNC(maxu64, int64_t)
	DEFINE_MAX_FUNC(max64, uint64_t)
#undef DEFINE_MAX_FUNC

#if RJD_IMPL

uint32_t next_pow2(uint32_t v) 
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

#endif

