#pragma once

// Uniform interface for low-level atomic operations. Ideally you could just use stdatomic but MSVC doesn't support it.
// Note that these functions all return the _new_ value stored in the atomic.

struct rjd_atomic_int64  { char impl[8]; };
struct rjd_atomic_int32  { char impl[4]; };
struct rjd_atomic_int16  { char impl[2]; };
struct rjd_atomic_int8   { char impl[1]; };
struct rjd_atomic_uint64 { char impl[8]; };
struct rjd_atomic_uint32 { char impl[4]; };
struct rjd_atomic_uint16 { char impl[2]; };
struct rjd_atomic_uint8  { char impl[1]; };

RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_int64) == sizeof(int64_t));
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_int32) == sizeof(int32_t));
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_int16) == sizeof(int16_t));
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_int8) == sizeof(int8_t));
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_uint64) == sizeof(uint64_t));
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_uint32) == sizeof(uint32_t));
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_uint16) == sizeof(uint16_t));
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_uint8) == sizeof(uint8_t));

struct rjd_atomic_int64 rjd_atomic_int64_init (int64_t  value);
int64_t rjd_atomic_int64_get(struct rjd_atomic_int64* atomic);
int64_t rjd_atomic_int64_set(struct rjd_atomic_int64* atomic, int64_t value);
int64_t rjd_atomic_int64_add(struct rjd_atomic_int64* atomic, int64_t value);
int64_t rjd_atomic_int64_sub(struct rjd_atomic_int64* atomic, int64_t value);
int64_t rjd_atomic_int64_inc(struct rjd_atomic_int64* atomic);
int64_t rjd_atomic_int64_dec(struct rjd_atomic_int64* atomic);
bool rjd_atomic_int64_compare_exchange(struct rjd_atomic_int64* atomic, int64_t* expected, int64_t desired);

struct rjd_atomic_int32 rjd_atomic_int32_init (int32_t  value);
int32_t rjd_atomic_int32_get(struct rjd_atomic_int32* atomic);
int32_t rjd_atomic_int32_set(struct rjd_atomic_int32* atomic, int32_t value);
int32_t rjd_atomic_int32_add(struct rjd_atomic_int32* atomic, int32_t value);
int32_t rjd_atomic_int32_sub(struct rjd_atomic_int32* atomic, int32_t value);
int32_t rjd_atomic_int32_inc(struct rjd_atomic_int32* atomic);
int32_t rjd_atomic_int32_dec(struct rjd_atomic_int32* atomic);
bool rjd_atomic_int_compare_exchange(struct rjd_atomic_int32* atomic, int32_t* expected, int32_t desired);

struct rjd_atomic_int16 rjd_atomic_int16_init (int16_t  value);
int16_t rjd_atomic_int16_get(struct rjd_atomic_int16* atomic);
int16_t rjd_atomic_int16_set(struct rjd_atomic_int16* atomic, int16_t value);
int16_t rjd_atomic_int16_add(struct rjd_atomic_int16* atomic, int16_t value);
int16_t rjd_atomic_int16_sub(struct rjd_atomic_int16* atomic, int16_t value);
int16_t rjd_atomic_int16_inc(struct rjd_atomic_int16* atomic);
int16_t rjd_atomic_int16_dec(struct rjd_atomic_int16* atomic);
bool rjd_atomic_int16_compare_exchange(struct rjd_atomic_int16* atomic, int16_t* expected, int16_t desired);

struct rjd_atomic_int8 rjd_atomic_int8_init (int8_t  value);
int8_t rjd_atomic_int8_get(struct rjd_atomic_int8* atomic);
int8_t rjd_atomic_int8_set(struct rjd_atomic_int8* atomic, int8_t value);
int8_t rjd_atomic_int8_add(struct rjd_atomic_int8* atomic, int8_t value);
int8_t rjd_atomic_int8_sub(struct rjd_atomic_int8* atomic, int8_t value);
int8_t rjd_atomic_int8_inc(struct rjd_atomic_int8* atomic);
int8_t rjd_atomic_int8_dec(struct rjd_atomic_int8* atomic);
bool rjd_atomic_int8_compare_exchange(struct rjd_atomic_int8* atomic, int8_t* expected, int8_t desired);

struct rjd_atomic_uint64 rjd_atomic_uint64_init (uint64_t  value);
uint64_t rjd_atomic_uint64_get(struct rjd_atomic_uint64* atomic);
uint64_t rjd_atomic_uint64_set(struct rjd_atomic_uint64* atomic, uint64_t value);
uint64_t rjd_atomic_uint64_add(struct rjd_atomic_uint64* atomic, uint64_t value);
uint64_t rjd_atomic_uint64_sub(struct rjd_atomic_uint64* atomic, uint64_t value);
uint64_t rjd_atomic_uint64_inc(struct rjd_atomic_uint64* atomic);
uint64_t rjd_atomic_uint64_dec(struct rjd_atomic_uint64* atomic);
bool rjd_atomic_uint64_compare_exchange(struct rjd_atomic_uint64* atomic, uint64_t* expected, uint64_t desired);

struct rjd_atomic_uint32 rjd_atomic_uint32_init (uint32_t  value);
uint32_t rjd_atomic_uint32_get(struct rjd_atomic_uint32* atomic);
uint32_t rjd_atomic_uint32_set(struct rjd_atomic_uint32* atomic, uint32_t value);
uint32_t rjd_atomic_uint32_add(struct rjd_atomic_uint32* atomic, uint32_t value);
uint32_t rjd_atomic_uint32_sub(struct rjd_atomic_uint32* atomic, uint32_t value);
uint32_t rjd_atomic_uint32_inc(struct rjd_atomic_uint32* atomic);
uint32_t rjd_atomic_uint32_dec(struct rjd_atomic_uint32* atomic);
bool rjd_atomic_uint32_compare_exchange(struct rjd_atomic_uint32* atomic, uint32_t* expected, uint32_t desired);

struct rjd_atomic_uint16 rjd_atomic_uint16_init (uint16_t  value);
uint16_t rjd_atomic_uint16_get(struct rjd_atomic_uint16* atomic);
uint16_t rjd_atomic_uint16_set(struct rjd_atomic_uint16* atomic, uint16_t value);
uint16_t rjd_atomic_uint16_add(struct rjd_atomic_uint16* atomic, uint16_t value);
uint16_t rjd_atomic_uint16_sub(struct rjd_atomic_uint16* atomic, uint16_t value);
uint16_t rjd_atomic_uint16_inc(struct rjd_atomic_uint16* atomic);
uint16_t rjd_atomic_uint16_dec(struct rjd_atomic_uint16* atomic);
bool rjd_atomic_uint16_compare_exchange(struct rjd_atomic_uint16* atomic, uint16_t* expected, uint16_t desired);

struct rjd_atomic_uint8 rjd_atomic_uint8_init (uint8_t  value);
uint8_t rjd_atomic_uint8_get(struct rjd_atomic_uint8* atomic);
uint8_t rjd_atomic_uint8_set(struct rjd_atomic_uint8* atomic, uint8_t value);
uint8_t rjd_atomic_uint8_add(struct rjd_atomic_uint8* atomic, uint8_t value);
uint8_t rjd_atomic_uint8_sub(struct rjd_atomic_uint8* atomic, uint8_t value);
uint8_t rjd_atomic_uint8_inc(struct rjd_atomic_uint8* atomic);
uint8_t rjd_atomic_uint8_dec(struct rjd_atomic_uint8* atomic);
bool rjd_atomic_uint8_compare_exchange(struct rjd_atomic_uint8* atomic, uint8_t* expected, uint8_t desired);

#if RJD_IMPL

// MSVC doesn't support stdatomic.h :(
#if RJD_COMPILER_MSVC

inline uint64_t rjd_reinterpret_i64_as_u64(int64_t value)
{
	union { int64_t s; uint64_t u; } caster;
	caster.s = value;
	return caster.u;
}

inline int64_t rjd_reinterpret_u64_as_i64(uint64_t value)
{
	union { int64_t s; uint64_t u; } caster;
	caster.u = value;
	return caster.s;
}

inline uint32_t rjd_reinterpret_i32_as_u32(int32_t value)
{
	union { int32_t s; int32_t u; } caster;
	caster.s = value;
	return caster.u;
}

inline int32_t rjd_reinterpret_u32_as_i32(uint32_t value)
{
	union { int32_t s; int32_t u; } caster;
	caster.u = value;
	return caster.s;
}

inline uint16_t rjd_reinterpret_i16_as_u16(int16_t value)
{
	union { int16_t s; int16_t u; } caster;
	caster.s = value;
	return caster.u;
}

inline int16_t rjd_reinterpret_u16_as_i16(uint16_t value)
{
	union { int16_t s; int16_t u; } caster;
	caster.u = value;
	return caster.s;
}

inline uint8_t rjd_reinterpret_i8_as_u8(int8_t value)
{
	union { int8_t s; int8_t u; } caster;
	caster.s = value;
	return caster.u;
}

inline int8_t rjd_reinterpret_u8_as_i8(uint8_t value)
{
	union { int8_t s; int8_t u; } caster;
	caster.u = value;
	return caster.s;
}

struct rjd_atomic_int64_msvc  { union { volatile __int64 value_signed; volatile uint64_t value_unsigned; }; };
struct rjd_atomic_int32_msvc  { union { volatile long value_signed; volatile long value_unsigned; }; };
struct rjd_atomic_int16_msvc  { union { volatile short value_signed; volatile short value_unsigned; }; };
struct rjd_atomic_int8_msvc   { union { volatile char value_signed; volatile char value_unsigned; }; };
struct rjd_atomic_uint64_msvc { struct rjd_atomic_int64 atomic; };
struct rjd_atomic_uint32_msvc { struct rjd_atomic_int32 atomic; };
struct rjd_atomic_uint16_msvc { struct rjd_atomic_int16 atomic; };
struct rjd_atomic_uint8_msvc  { struct rjd_atomic_int8 atomic; };

RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_int64_msvc) <= sizeof(struct rjd_atomic_int64));
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_int32_msvc) <= sizeof(struct rjd_atomic_int32));
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_int16_msvc) <= sizeof(struct rjd_atomic_int16));
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_int8_msvc) <= sizeof(struct rjd_atomic_int8));
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_uint64_msvc) <= sizeof(struct rjd_atomic_uint64));
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_uint32_msvc) <= sizeof(struct rjd_atomic_uint32));
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_uint16_msvc) <= sizeof(struct rjd_atomic_uint16));
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_uint8_msvc) <= sizeof(struct rjd_atomic_uint8));

////////////////////////////////////////////////////////////////////////////////

struct rjd_atomic_int64 rjd_atomic_int64_init(int64_t value)
{
	struct rjd_atomic_int64 out = {0};
	struct rjd_atomic_int64_msvc* atomic_msvc = (struct rjd_atomic_int64_msvc*)&out;
	atomic_msvc->value_signed = value;
	return out;
}

int64_t rjd_atomic_int64_get(struct rjd_atomic_int64* atomic)
{
	struct rjd_atomic_int64_msvc* atomic_msvc = (struct rjd_atomic_int64_msvc*)atomic;
	return atomic_msvc->value_signed;
}

int64_t rjd_atomic_int64_set(struct rjd_atomic_int64* atomic, int64_t value)
{
	struct rjd_atomic_int64_msvc* atomic_msvc = (struct rjd_atomic_int64_msvc*)atomic;
	atomic_msvc->value_signed = value;
	return value;
}

int64_t rjd_atomic_int64_add(struct rjd_atomic_int64* atomic, int64_t value)
{
	struct rjd_atomic_int64_msvc* atomic_msvc = (struct rjd_atomic_int64_msvc*)atomic;
	return InterlockedExchangeAdd64(&atomic_msvc->value_signed, value);
}

int64_t rjd_atomic_int64_sub(struct rjd_atomic_int64* atomic, int64_t value)
{
	return rjd_atomic_int64_add(atomic, -value);
}

int64_t rjd_atomic_int64_inc(struct rjd_atomic_int64* atomic)
{
	struct rjd_atomic_int64_msvc* atomic_msvc = (struct rjd_atomic_int64_msvc*)atomic;
	return InterlockedIncrement64(&atomic_msvc->value_signed);
}

int64_t rjd_atomic_int64_dec(struct rjd_atomic_int64* atomic)
{
	struct rjd_atomic_int64_msvc* atomic_msvc = (struct rjd_atomic_int64_msvc*)atomic;
	return InterlockedDecrement64(&atomic_msvc->value_signed);
}

bool rjd_atomic_int64_compare_exchange(struct rjd_atomic_int64* atomic, int64_t* expected, int64_t desired)
{
	struct rjd_atomic_int64_msvc* atomic_msvc = (struct rjd_atomic_int64_msvc*)atomic;
	return InterlockedCompareExchange64(&atomic_msvc->value_signed, desired, *expected);
}

////////////////////////////////////////////////////////////////////////////////

struct rjd_atomic_int32 rjd_atomic_int32_init(int32_t value)
{
	struct rjd_atomic_int32 out = {0};
	struct rjd_atomic_int32_msvc* atomic_msvc = (struct rjd_atomic_int32_msvc*)&out;
	atomic_msvc->value_signed = value;
	return out;
}

int32_t rjd_atomic_int32_get(struct rjd_atomic_int32* atomic)
{
	struct rjd_atomic_int32_msvc* atomic_msvc = (struct rjd_atomic_int32_msvc*)atomic;
	return atomic_msvc->value_signed;
}

int32_t rjd_atomic_int32_set(struct rjd_atomic_int32* atomic, int32_t value)
{
	struct rjd_atomic_int32_msvc* atomic_msvc = (struct rjd_atomic_int32_msvc*)atomic;
	atomic_msvc->value_signed = value;
	return value;
}

int32_t rjd_atomic_int32_add(struct rjd_atomic_int32* atomic, int32_t value)
{
	struct rjd_atomic_int32_msvc* atomic_msvc = (struct rjd_atomic_int32_msvc*)atomic;
	return InterlockedExchangeAdd(&atomic_msvc->value_signed, value);
}

int32_t rjd_atomic_int32_sub(struct rjd_atomic_int32* atomic, int32_t value)
{
	return rjd_atomic_int32_add(atomic, -value);
}

int32_t rjd_atomic_int32_inc(struct rjd_atomic_int32* atomic)
{
	struct rjd_atomic_int32_msvc* atomic_msvc = (struct rjd_atomic_int32_msvc*)atomic;
	return InterlockedIncrement(&atomic_msvc->value_signed);
}

int32_t rjd_atomic_int32_dec(struct rjd_atomic_int32* atomic)
{
	struct rjd_atomic_int32_msvc* atomic_msvc = (struct rjd_atomic_int32_msvc*)atomic;
	return InterlockedDecrement(&atomic_msvc->value_signed);
}

bool rjd_atomic_int32_compare_exchange(struct rjd_atomic_int32* atomic, int32_t* expected, int32_t desired)
{
	struct rjd_atomic_int32_msvc* atomic_msvc = (struct rjd_atomic_int32_msvc*)atomic;
	return InterlockedCompareExchange(&atomic_msvc->value_signed, desired, *expected);
}

////////////////////////////////////////////////////////////////////////////////

struct rjd_atomic_int16 rjd_atomic_int16_init(int16_t value)
{
	struct rjd_atomic_int16 out = {0};
	struct rjd_atomic_int16_msvc* atomic_msvc =  (struct rjd_atomic_int16_msvc*)&out;
	atomic_msvc->value_signed = value;
	return out;
}

int16_t rjd_atomic_int16_get(struct rjd_atomic_int16* atomic)
{
	struct rjd_atomic_int16_msvc* atomic_msvc = (struct rjd_atomic_int16_msvc*)atomic;
	return atomic_msvc->value_signed;
}

int16_t rjd_atomic_int16_set(struct rjd_atomic_int16* atomic, int16_t value)
{
	struct rjd_atomic_int16_msvc* atomic_msvc = (struct rjd_atomic_int16_msvc*)atomic;
	atomic_msvc->value_signed = value;
	return value;
}

int16_t rjd_atomic_int16_add(struct rjd_atomic_int16* atomic, int16_t value)
{
	struct rjd_atomic_int16_msvc* atomic_msvc = (struct rjd_atomic_int16_msvc*)atomic;

	int16_t expected;
	int16_t desired;
	do
	{
		expected = atomic_msvc->value_signed;
		desired = atomic_msvc->value_signed + value;
	} while (InterlockedCompareExchange16(&atomic_msvc->value_signed, desired, expected) != expected);
	return desired;
}

int16_t rjd_atomic_int16_sub(struct rjd_atomic_int16* atomic, int16_t value)
{
	return rjd_atomic_int16_add(atomic, -value);
}

int16_t rjd_atomic_int16_inc(struct rjd_atomic_int16* atomic)
{
	struct rjd_atomic_int16_msvc* atomic_msvc = (struct rjd_atomic_int16_msvc*)atomic;
	return InterlockedIncrement16(&atomic_msvc->value_signed);
}

int16_t rjd_atomic_int16_dec(struct rjd_atomic_int16* atomic)
{
	struct rjd_atomic_int16_msvc* atomic_msvc = (struct rjd_atomic_int16_msvc*)atomic;
	return InterlockedDecrement16(&atomic_msvc->value_signed);
}

bool rjd_atomic_int16_compare_exchange(struct rjd_atomic_int16* atomic, int16_t* expected, int16_t desired)
{
	struct rjd_atomic_int16_msvc* atomic_msvc = (struct rjd_atomic_int16_msvc*)atomic;
	return InterlockedCompareExchange16(&atomic_msvc->value_signed, desired, *expected);
}

////////////////////////////////////////////////////////////////////////////////

struct rjd_atomic_int8 rjd_atomic_int8_init (int8_t value)
{
	struct rjd_atomic_int8 out = {0};
	struct rjd_atomic_int8_msvc* atomic_msvc =  (struct rjd_atomic_int8_msvc*)&out;
	atomic_msvc->value_signed = value;
	return out;
}

int8_t rjd_atomic_int8_get(struct rjd_atomic_int8* atomic)
{
	struct rjd_atomic_int8_msvc* atomic_msvc = (struct rjd_atomic_int8_msvc*)atomic;
	return atomic_msvc->value_signed;
}

int8_t rjd_atomic_int8_set(struct rjd_atomic_int8* atomic, int8_t value)
{
	struct rjd_atomic_int8_msvc* atomic_msvc = (struct rjd_atomic_int8_msvc*)atomic;
	atomic_msvc->value_signed = value;
	return value;
}

int8_t rjd_atomic_int8_add(struct rjd_atomic_int8* atomic, int8_t value)
{
	struct rjd_atomic_int8_msvc* atomic_msvc = (struct rjd_atomic_int8_msvc*)atomic;

	int8_t expected;
	int8_t desired;
	do
	{
		expected = atomic_msvc->value_signed;
		desired = atomic_msvc->value_signed + value;
	} while (_InterlockedCompareExchange8(&atomic_msvc->value_signed, desired, expected) != expected);
	return desired;
}

int8_t rjd_atomic_int8_sub(struct rjd_atomic_int8* atomic, int8_t value)
{
	return rjd_atomic_int8_add(atomic, -value);
}

int8_t rjd_atomic_int8_inc(struct rjd_atomic_int8* atomic)
{
	return rjd_atomic_int8_add(atomic, 1);
}

int8_t rjd_atomic_int8_dec(struct rjd_atomic_int8* atomic)
{
	return rjd_atomic_int8_add(atomic, -1);
}

bool rjd_atomic_int8_compare_exchange(struct rjd_atomic_int8* atomic, int8_t* expected, int8_t desired)
{
	struct rjd_atomic_int8_msvc* atomic_msvc = (struct rjd_atomic_int8_msvc*)atomic;
	return _InterlockedCompareExchange8(&atomic_msvc->value_signed, desired, *expected);
}

////////////////////////////////////////////////////////////////////////////////

struct rjd_atomic_uint64 rjd_atomic_uint64_init(uint64_t value)
{
	struct rjd_atomic_uint64 out = {0};
	struct rjd_atomic_uint64_msvc* atomic_msvc = (struct rjd_atomic_uint64_msvc*)&out;
	struct rjd_atomic_int64_msvc* atomic_signed_msvc = (struct rjd_atomic_int64_msvc*)&atomic_msvc->atomic;
	atomic_signed_msvc->value_unsigned = value;
	return out;
}

uint64_t rjd_atomic_uint64_get(struct rjd_atomic_uint64* atomic)
{
	struct rjd_atomic_uint64_msvc* atomic_msvc = (struct rjd_atomic_uint64_msvc*)atomic;
	struct rjd_atomic_int64_msvc* atomic_signed_msvc = (struct rjd_atomic_int64_msvc*)&atomic_msvc->atomic;
	return atomic_signed_msvc->value_unsigned;
}

uint64_t rjd_atomic_uint64_set(struct rjd_atomic_uint64* atomic, uint64_t value)
{
	struct rjd_atomic_uint64_msvc* atomic_msvc = (struct rjd_atomic_uint64_msvc*)atomic;
	struct rjd_atomic_int64_msvc* atomic_signed_msvc = (struct rjd_atomic_int64_msvc*)&atomic_msvc->atomic;
	atomic_signed_msvc->value_unsigned = value;
	return value;
}

uint64_t rjd_atomic_uint64_add(struct rjd_atomic_uint64* atomic, uint64_t value)
{
	struct rjd_atomic_uint64_msvc* atomic_msvc = (struct rjd_atomic_uint64_msvc*)atomic;
	int64_t result = rjd_atomic_int64_add(&atomic_msvc->atomic, rjd_reinterpret_u64_as_i64(value));
	return rjd_reinterpret_i64_as_u64(result);
}

uint64_t rjd_atomic_uint64_sub(struct rjd_atomic_uint64* atomic, uint64_t value)
{
	struct rjd_atomic_uint64_msvc* atomic_msvc = (struct rjd_atomic_uint64_msvc*)atomic;
	int64_t result = rjd_atomic_int64_add(&atomic_msvc->atomic, -rjd_reinterpret_u64_as_i64(value));
	return rjd_reinterpret_i64_as_u64(result);
}

uint64_t rjd_atomic_uint64_inc(struct rjd_atomic_uint64* atomic)
{
	struct rjd_atomic_uint64_msvc* atomic_msvc = (struct rjd_atomic_uint64_msvc*)atomic;
	int64_t result = rjd_atomic_int64_inc(&atomic_msvc->atomic);
	return rjd_reinterpret_i64_as_u64(result);
}

uint64_t rjd_atomic_uint64_dec(struct rjd_atomic_uint64* atomic)
{
	struct rjd_atomic_uint64_msvc* atomic_msvc = (struct rjd_atomic_uint64_msvc*)atomic;
	int64_t result = rjd_atomic_int64_dec(&atomic_msvc->atomic);
	return rjd_reinterpret_i64_as_u64(result);
}

bool rjd_atomic_uint64_compare_exchange(struct rjd_atomic_uint64* atomic, uint64_t* expected, uint64_t desired)
{
	struct rjd_atomic_uint64_msvc* atomic_msvc = (struct rjd_atomic_uint64_msvc*)atomic;
	return rjd_atomic_int64_compare_exchange(&atomic_msvc->atomic, (int64_t*)expected, rjd_reinterpret_u64_as_i64(desired));
}

////////////////////////////////////////////////////////////////////////////////

struct rjd_atomic_uint32 rjd_atomic_uint32_init(uint32_t value)
{
	struct rjd_atomic_uint32 out = {0};
	struct rjd_atomic_uint32_msvc* atomic_msvc = (struct rjd_atomic_uint32_msvc*)&out;
	struct rjd_atomic_int32_msvc* atomic_signed_msvc = (struct rjd_atomic_int32_msvc*)&atomic_msvc->atomic;
	atomic_signed_msvc->value_unsigned = value;
	return out;
}

uint32_t rjd_atomic_uint32_get(struct rjd_atomic_uint32* atomic)
{
	struct rjd_atomic_uint32_msvc* atomic_msvc = (struct rjd_atomic_uint32_msvc*)atomic;
	struct rjd_atomic_int32_msvc* atomic_signed_msvc = (struct rjd_atomic_int32_msvc*)&atomic_msvc->atomic;
	return atomic_signed_msvc->value_unsigned;
}

uint32_t rjd_atomic_uint32_set(struct rjd_atomic_uint32* atomic, uint32_t value)
{
	struct rjd_atomic_uint32_msvc* atomic_msvc = (struct rjd_atomic_uint32_msvc*)atomic;
	struct rjd_atomic_int32_msvc* atomic_signed_msvc = (struct rjd_atomic_int32_msvc*)&atomic_msvc->atomic;
	atomic_signed_msvc->value_unsigned = value;
	return value;
}

uint32_t rjd_atomic_uint32_add(struct rjd_atomic_uint32* atomic, uint32_t value)
{
	struct rjd_atomic_uint32_msvc* atomic_msvc = (struct rjd_atomic_uint32_msvc*)atomic;
	int32_t result = rjd_atomic_int32_add(&atomic_msvc->atomic, rjd_reinterpret_u32_as_i32(value));
	return rjd_reinterpret_i32_as_u32(result);
}

uint32_t rjd_atomic_uint32_sub(struct rjd_atomic_uint32* atomic, uint32_t value)
{
	struct rjd_atomic_uint32_msvc* atomic_msvc = (struct rjd_atomic_uint32_msvc*)atomic;
	int32_t result = rjd_atomic_int32_add(&atomic_msvc->atomic, -rjd_reinterpret_u32_as_i32(value));
	return rjd_reinterpret_i32_as_u32(result);
}

uint32_t rjd_atomic_uint32_inc(struct rjd_atomic_uint32* atomic)
{
	struct rjd_atomic_uint32_msvc* atomic_msvc = (struct rjd_atomic_uint32_msvc*)atomic;
	int32_t result = rjd_atomic_int32_inc(&atomic_msvc->atomic);
	return rjd_reinterpret_i32_as_u32(result);
}

uint32_t rjd_atomic_uint32_dec(struct rjd_atomic_uint32* atomic)
{
	struct rjd_atomic_uint32_msvc* atomic_msvc = (struct rjd_atomic_uint32_msvc*)atomic;
	int32_t result = rjd_atomic_int32_dec(&atomic_msvc->atomic);
	return rjd_reinterpret_i32_as_u32(result);
}

bool rjd_atomic_uint32_compare_exchange(struct rjd_atomic_uint32* atomic, uint32_t* expected, uint32_t desired)
{
	struct rjd_atomic_uint32_msvc* atomic_msvc = (struct rjd_atomic_uint32_msvc*)atomic;
	return rjd_atomic_int32_compare_exchange(&atomic_msvc->atomic, (int32_t*)expected, rjd_reinterpret_u32_as_i32(desired));
}

////////////////////////////////////////////////////////////////////////////////

struct rjd_atomic_uint16 rjd_atomic_uint16_init(uint16_t value)
{
	struct rjd_atomic_uint16 out = {0};
	struct rjd_atomic_uint16_msvc* atomic_msvc = (struct rjd_atomic_uint16_msvc*)&out;
	struct rjd_atomic_int16_msvc* atomic_signed_msvc = (struct rjd_atomic_int16_msvc*)&atomic_msvc->atomic;
	atomic_signed_msvc->value_unsigned = value;
	return out;
}

uint16_t rjd_atomic_uint16_get(struct rjd_atomic_uint16* atomic)
{
	struct rjd_atomic_uint16_msvc* atomic_msvc = (struct rjd_atomic_uint16_msvc*)atomic;
	struct rjd_atomic_int16_msvc* atomic_signed_msvc = (struct rjd_atomic_int16_msvc*)&atomic_msvc->atomic;
	return atomic_signed_msvc->value_unsigned;
}

uint16_t rjd_atomic_uint16_set(struct rjd_atomic_uint16* atomic, uint16_t value)
{
	struct rjd_atomic_uint16_msvc* atomic_msvc = (struct rjd_atomic_uint16_msvc*)atomic;
	struct rjd_atomic_int16_msvc* atomic_signed_msvc = (struct rjd_atomic_int16_msvc*)&atomic_msvc->atomic;
	atomic_signed_msvc->value_unsigned = value;
	return value;
}

uint16_t rjd_atomic_uint16_add(struct rjd_atomic_uint16* atomic, uint16_t value)
{
	struct rjd_atomic_uint16_msvc* atomic_msvc = (struct rjd_atomic_uint16_msvc*)atomic;
	int16_t result = rjd_atomic_int16_add(&atomic_msvc->atomic, rjd_reinterpret_u16_as_i16(value));
	return rjd_reinterpret_i16_as_u16(result);
}

uint16_t rjd_atomic_uint16_sub(struct rjd_atomic_uint16* atomic, uint16_t value)
{
	struct rjd_atomic_uint16_msvc* atomic_msvc = (struct rjd_atomic_uint16_msvc*)atomic;
	int16_t result = rjd_atomic_int16_add(&atomic_msvc->atomic, -rjd_reinterpret_u16_as_i16(value));
	return rjd_reinterpret_i16_as_u16(result);
}

uint16_t rjd_atomic_uint16_inc(struct rjd_atomic_uint16* atomic)
{
	struct rjd_atomic_uint16_msvc* atomic_msvc = (struct rjd_atomic_uint16_msvc*)atomic;
	int16_t result = rjd_atomic_int16_inc(&atomic_msvc->atomic);
	return rjd_reinterpret_i16_as_u16(result);
}

uint16_t rjd_atomic_uint16_dec(struct rjd_atomic_uint16* atomic)
{
	struct rjd_atomic_uint16_msvc* atomic_msvc = (struct rjd_atomic_uint16_msvc*)atomic;
	int16_t result = rjd_atomic_int16_dec(&atomic_msvc->atomic);
	return rjd_reinterpret_i16_as_u16(result);
}

bool rjd_atomic_uint16_compare_exchange(struct rjd_atomic_uint16* atomic, uint16_t* expected, uint16_t desired)
{
	struct rjd_atomic_uint16_msvc* atomic_msvc = (struct rjd_atomic_uint16_msvc*)atomic;
	return rjd_atomic_int16_compare_exchange(&atomic_msvc->atomic, (int16_t*)expected, rjd_reinterpret_u16_as_i16(desired));
}

////////////////////////////////////////////////////////////////////////////////

struct rjd_atomic_uint8 rjd_atomic_uint8_init(uint8_t value)
{
	struct rjd_atomic_uint8 out = {0};
	struct rjd_atomic_uint8_msvc* atomic_msvc = (struct rjd_atomic_uint8_msvc*)&out;
	struct rjd_atomic_int8_msvc* atomic_signed_msvc = (struct rjd_atomic_int8_msvc*)&atomic_msvc->atomic;
	atomic_signed_msvc->value_unsigned = value;
	return out;
}

uint8_t rjd_atomic_uint8_get(struct rjd_atomic_uint8* atomic)
{
	struct rjd_atomic_uint8_msvc* atomic_msvc = (struct rjd_atomic_uint8_msvc*)atomic;
	struct rjd_atomic_int8_msvc* atomic_signed_msvc = (struct rjd_atomic_int8_msvc*)&atomic_msvc->atomic;
	return atomic_signed_msvc->value_unsigned;
}

uint8_t rjd_atomic_uint8_set(struct rjd_atomic_uint8* atomic, uint8_t value)
{
	struct rjd_atomic_uint8_msvc* atomic_msvc = (struct rjd_atomic_uint8_msvc*)atomic;
	struct rjd_atomic_int8_msvc* atomic_signed_msvc = (struct rjd_atomic_int8_msvc*)&atomic_msvc->atomic;
	atomic_signed_msvc->value_unsigned = value;
	return value;
}

uint8_t rjd_atomic_uint8_add(struct rjd_atomic_uint8* atomic, uint8_t value)
{
	struct rjd_atomic_uint8_msvc* atomic_msvc = (struct rjd_atomic_uint8_msvc*)atomic;
	int8_t result = rjd_atomic_int8_add(&atomic_msvc->atomic, rjd_reinterpret_u8_as_i8(value));
	return rjd_reinterpret_i8_as_u8(result);
}

uint8_t rjd_atomic_uint8_sub(struct rjd_atomic_uint8* atomic, uint8_t value)
{
	struct rjd_atomic_uint8_msvc* atomic_msvc = (struct rjd_atomic_uint8_msvc*)atomic;
	int8_t result = rjd_atomic_int8_add(&atomic_msvc->atomic, -rjd_reinterpret_u8_as_i8(value));
	return rjd_reinterpret_i8_as_u8(result);
}

uint8_t rjd_atomic_uint8_inc(struct rjd_atomic_uint8* atomic)
{
	struct rjd_atomic_uint8_msvc* atomic_msvc = (struct rjd_atomic_uint8_msvc*)atomic;
	int8_t result = rjd_atomic_int8_inc(&atomic_msvc->atomic);
	return rjd_reinterpret_i8_as_u8(result);
}

uint8_t rjd_atomic_uint8_dec(struct rjd_atomic_uint8* atomic)
{
	struct rjd_atomic_uint8_msvc* atomic_msvc = (struct rjd_atomic_uint8_msvc*)atomic;
	int8_t result = rjd_atomic_int8_dec(&atomic_msvc->atomic);
	return rjd_reinterpret_i8_as_u8(result);
}

bool rjd_atomic_uint8_compare_exchange(struct rjd_atomic_uint8* atomic, uint8_t* expected, uint8_t desired)
{
	struct rjd_atomic_uint8_msvc* atomic_msvc = (struct rjd_atomic_uint8_msvc*)atomic;
	return rjd_atomic_int8_compare_exchange(&atomic_msvc->atomic, (int8_t*)expected, rjd_reinterpret_u8_as_i8(desired));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#elif RJD_COMPILER_GCC || RJD_COMPILER_CLANG
#include <stdatomic.h>

struct rjd_atomic_int64_c11  { _Atomic int64_t value; };
struct rjd_atomic_int32_c11  { _Atomic int32_t value; };
struct rjd_atomic_int16_c11  { _Atomic int16_t value; };
struct rjd_atomic_int8_c11   { _Atomic int8_t value; };
struct rjd_atomic_uint64_c11 { _Atomic uint64_t value; };
struct rjd_atomic_uint32_c11 { _Atomic uint32_t value; };
struct rjd_atomic_uint16_c11 { _Atomic uint16_t value; };
struct rjd_atomic_uint8_c11  { _Atomic uint8_t value; };

RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_int64_c11) <= sizeof(struct rjd_atomic_int64));
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_int32_c11) <= sizeof(struct rjd_atomic_int32));
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_int16_c11) <= sizeof(struct rjd_atomic_int16));
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_int8_c11) <= sizeof(struct rjd_atomic_int8));
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_uint64_c11) <= sizeof(struct rjd_atomic_uint64));
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_uint32_c11) <= sizeof(struct rjd_atomic_uint32));
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_uint16_c11) <= sizeof(struct rjd_atomic_uint16));
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_uint8_c11) <= sizeof(struct rjd_atomic_uint8));

////////////////////////////////////////////////////////////////////////////////
// Implementation

struct rjd_atomic_int64 rjd_atomic_int64_init(int64_t value)
{
	struct rjd_atomic_int64 atomic;
	struct rjd_atomic_int64_c11* atomic_c11 = (struct rjd_atomic_int64_c11*)&atomic;
	atomic_store(&atomic_c11->value, value);
	return atomic;
}

int64_t rjd_atomic_int64_get(struct rjd_atomic_int64* atomic)
{
	struct rjd_atomic_int64_c11* atomic_c11 = (struct rjd_atomic_int64_c11*)atomic;
	return atomic_load(&atomic_c11->value);
}

int64_t rjd_atomic_int64_set(struct rjd_atomic_int64* atomic, int64_t value)
{
	struct rjd_atomic_int64_c11* atomic_c11 = (struct rjd_atomic_int64_c11*)atomic;
	atomic_store(&atomic_c11->value, value);
	return value;
}

int64_t rjd_atomic_int64_add(struct rjd_atomic_int64* atomic, int64_t value)
{
	struct rjd_atomic_int64_c11* atomic_c11 = (struct rjd_atomic_int64_c11*)atomic;
	return atomic_fetch_add(&atomic_c11->value, value) + value;
}

int64_t rjd_atomic_int64_sub(struct rjd_atomic_int64* atomic, int64_t value)
{
	struct rjd_atomic_int64_c11* atomic_c11 = (struct rjd_atomic_int64_c11*)atomic;
	return atomic_fetch_sub(&atomic_c11->value, value) + value;
}

int64_t rjd_atomic_int64_inc(struct rjd_atomic_int64* atomic)
{
	return rjd_atomic_int64_add(atomic, 1);
}

int64_t rjd_atomic_int64_dec(struct rjd_atomic_int64* atomic)
{
	return rjd_atomic_int64_sub(atomic, 1);
}

bool rjd_atomic_int64_compare_exchange(struct rjd_atomic_int64* atomic, int64_t* expected, int64_t desired)
{
	struct rjd_atomic_int64_c11* atomic_c11 = (struct rjd_atomic_int64_c11*)atomic;
	return atomic_compare_exchange_strong(&atomic_c11->value, expected, desired);
}

////////////////////////////////////////////////////////////////////////////////

struct rjd_atomic_int32 rjd_atomic_int32_init(int32_t value)
{
	struct rjd_atomic_int32 atomic;
	struct rjd_atomic_int32_c11* atomic_c11 = (struct rjd_atomic_int32_c11*)&atomic;
	atomic_store(&atomic_c11->value, value);
	return atomic;
}

int32_t rjd_atomic_int32_get(struct rjd_atomic_int32* atomic)
{
	struct rjd_atomic_int32_c11* atomic_c11 = (struct rjd_atomic_int32_c11*)atomic;
	return atomic_load(&atomic_c11->value);
}

int32_t rjd_atomic_int32_set(struct rjd_atomic_int32* atomic, int32_t value)
{
	struct rjd_atomic_int32_c11* atomic_c11 = (struct rjd_atomic_int32_c11*)atomic;
	atomic_store(&atomic_c11->value, value);
	return value;
}

int32_t rjd_atomic_int32_add(struct rjd_atomic_int32* atomic, int32_t value)
{
	struct rjd_atomic_int32_c11* atomic_c11 = (struct rjd_atomic_int32_c11*)atomic;
	return atomic_fetch_add(&atomic_c11->value, value) + value;
}

int32_t rjd_atomic_int32_sub(struct rjd_atomic_int32* atomic, int32_t value)
{
	struct rjd_atomic_int32_c11* atomic_c11 = (struct rjd_atomic_int32_c11*)atomic;
	return atomic_fetch_sub(&atomic_c11->value, value) + value;
}

int32_t rjd_atomic_int32_inc(struct rjd_atomic_int32* atomic)
{
	return rjd_atomic_int32_add(atomic, 1);
}

int32_t rjd_atomic_int32_dec(struct rjd_atomic_int32* atomic)
{
	return rjd_atomic_int32_sub(atomic, 1);
}

bool rjd_atomic_int32_compare_exchange(struct rjd_atomic_int32* atomic, int32_t* expected, int32_t desired)
{
	struct rjd_atomic_int32_c11* atomic_c11 = (struct rjd_atomic_int32_c11*)atomic;
	return atomic_compare_exchange_strong(&atomic_c11->value, expected, desired);
}

////////////////////////////////////////////////////////////////////////////////

struct rjd_atomic_int16 rjd_atomic_int16_init(int16_t value)
{
	struct rjd_atomic_int16 atomic;
	struct rjd_atomic_int16_c11* atomic_c11 = (struct rjd_atomic_int16_c11*)&atomic;
	atomic_store(&atomic_c11->value, value);
	return atomic;
}

int16_t rjd_atomic_int16_get(struct rjd_atomic_int16* atomic)
{
	struct rjd_atomic_int16_c11* atomic_c11 = (struct rjd_atomic_int16_c11*)atomic;
	return atomic_load(&atomic_c11->value);
}

int16_t rjd_atomic_int16_set(struct rjd_atomic_int16* atomic, int16_t value)
{
	struct rjd_atomic_int16_c11* atomic_c11 = (struct rjd_atomic_int16_c11*)atomic;
	atomic_store(&atomic_c11->value, value);
	return value;
}

int16_t rjd_atomic_int16_add(struct rjd_atomic_int16* atomic, int16_t value)
{
	struct rjd_atomic_int16_c11* atomic_c11 = (struct rjd_atomic_int16_c11*)atomic;
	return atomic_fetch_add(&atomic_c11->value, value) + value;
}

int16_t rjd_atomic_int16_sub(struct rjd_atomic_int16* atomic, int16_t value)
{
	struct rjd_atomic_int16_c11* atomic_c11 = (struct rjd_atomic_int16_c11*)atomic;
	return atomic_fetch_sub(&atomic_c11->value, value) + value;
}

int16_t rjd_atomic_int16_inc(struct rjd_atomic_int16* atomic)
{
	return rjd_atomic_int16_add(atomic, 1);
}

int16_t rjd_atomic_int16_dec(struct rjd_atomic_int16* atomic)
{
	return rjd_atomic_int16_sub(atomic, 1);
}

bool rjd_atomic_int16_compare_exchange(struct rjd_atomic_int16* atomic, int16_t* expected, int16_t desired)
{
	struct rjd_atomic_int16_c11* atomic_c11 = (struct rjd_atomic_int16_c11*)atomic;
	return atomic_compare_exchange_strong(&atomic_c11->value, expected, desired);
}

////////////////////////////////////////////////////////////////////////////////

struct rjd_atomic_int8 rjd_atomic_int8_init(int8_t value)
{
	struct rjd_atomic_int8 atomic;
	struct rjd_atomic_int8_c11* atomic_c11 = (struct rjd_atomic_int8_c11*)&atomic;
	atomic_store(&atomic_c11->value, value);
	return atomic;
}

int8_t rjd_atomic_int8_get(struct rjd_atomic_int8* atomic)
{
	struct rjd_atomic_int8_c11* atomic_c11 = (struct rjd_atomic_int8_c11*)atomic;
	return atomic_load(&atomic_c11->value);
}

int8_t rjd_atomic_int8_set(struct rjd_atomic_int8* atomic, int8_t value)
{
	struct rjd_atomic_int8_c11* atomic_c11 = (struct rjd_atomic_int8_c11*)atomic;
	atomic_store(&atomic_c11->value, value);
	return value;
}

int8_t rjd_atomic_int8_add(struct rjd_atomic_int8* atomic, int8_t value)
{
	struct rjd_atomic_int8_c11* atomic_c11 = (struct rjd_atomic_int8_c11*)atomic;
	return atomic_fetch_add(&atomic_c11->value, value) + value;
}

int8_t rjd_atomic_int8_sub(struct rjd_atomic_int8* atomic, int8_t value)
{
	struct rjd_atomic_int8_c11* atomic_c11 = (struct rjd_atomic_int8_c11*)atomic;
	return atomic_fetch_sub(&atomic_c11->value, value) + value;
}

int8_t rjd_atomic_int8_inc(struct rjd_atomic_int8* atomic)
{
	return rjd_atomic_int8_add(atomic, 1);
}

int8_t rjd_atomic_int8_dec(struct rjd_atomic_int8* atomic)
{
	return rjd_atomic_int8_sub(atomic, 1);
}

bool rjd_atomic_int8_compare_exchange_strong(struct rjd_atomic_int8* atomic, int8_t* expected, int8_t desired)
{
	struct rjd_atomic_int8_c11* atomic_c11 = (struct rjd_atomic_int8_c11*)atomic;
	return atomic_compare_exchange_strong(&atomic_c11->value, expected, desired);
}

////////////////////////////////////////////////////////////////////////////////

struct rjd_atomic_uint64 rjd_atomic_uint64_init(uint64_t value)
{
	struct rjd_atomic_uint64 atomic;
	struct rjd_atomic_uint64_c11* atomic_c11 = (struct rjd_atomic_uint64_c11*)&atomic;
	atomic_store(&atomic_c11->value, value);
	return atomic;
}

uint64_t rjd_atomic_uint64_get(struct rjd_atomic_uint64* atomic)
{
	struct rjd_atomic_uint64_c11* atomic_c11 = (struct rjd_atomic_uint64_c11*)atomic;
	return atomic_load(&atomic_c11->value);
}

uint64_t rjd_atomic_uint64_set(struct rjd_atomic_uint64* atomic, uint64_t value)
{
	struct rjd_atomic_uint64_c11* atomic_c11 = (struct rjd_atomic_uint64_c11*)atomic;
	atomic_store(&atomic_c11->value, value);
	return value;
}

uint64_t rjd_atomic_uint64_add(struct rjd_atomic_uint64* atomic, uint64_t value)
{
	struct rjd_atomic_uint64_c11* atomic_c11 = (struct rjd_atomic_uint64_c11*)atomic;
	return atomic_fetch_add(&atomic_c11->value, value) + value;
}

uint64_t rjd_atomic_uint64_sub(struct rjd_atomic_uint64* atomic, uint64_t value)
{
	struct rjd_atomic_uint64_c11* atomic_c11 = (struct rjd_atomic_uint64_c11*)atomic;
	return atomic_fetch_sub(&atomic_c11->value, value) + value;
}

uint64_t rjd_atomic_uint64_inc(struct rjd_atomic_uint64* atomic)
{
	return rjd_atomic_uint64_add(atomic, 1);
}

uint64_t rjd_atomic_uint64_dec(struct rjd_atomic_uint64* atomic)
{
	return rjd_atomic_uint64_sub(atomic, 1);
}

bool rjd_atomic_uint64_compare_exchange(struct rjd_atomic_uint64* atomic, uint64_t* expected, uint64_t desired)
{
	struct rjd_atomic_uint64_c11* atomic_c11 = (struct rjd_atomic_uint64_c11*)atomic;
	return atomic_compare_exchange_strong(&atomic_c11->value, expected, desired);
}

////////////////////////////////////////////////////////////////////////////////

struct rjd_atomic_uint32 rjd_atomic_uint32_init(uint32_t value)
{
	struct rjd_atomic_uint32 atomic;
	struct rjd_atomic_uint32_c11* atomic_c11 = (struct rjd_atomic_uint32_c11*)&atomic;
	atomic_store(&atomic_c11->value, value);
	return atomic;
}

uint32_t rjd_atomic_uint32_get(struct rjd_atomic_uint32* atomic)
{
	struct rjd_atomic_uint32_c11* atomic_c11 = (struct rjd_atomic_uint32_c11*)atomic;
	return atomic_load(&atomic_c11->value);
}

uint32_t rjd_atomic_uint32_set(struct rjd_atomic_uint32* atomic, uint32_t value)
{
	struct rjd_atomic_uint32_c11* atomic_c11 = (struct rjd_atomic_uint32_c11*)atomic;
	atomic_store(&atomic_c11->value, value);
	return value;
}

uint32_t rjd_atomic_uint32_add(struct rjd_atomic_uint32* atomic, uint32_t value)
{
	struct rjd_atomic_uint32_c11* atomic_c11 = (struct rjd_atomic_uint32_c11*)atomic;
	return atomic_fetch_add(&atomic_c11->value, value) + value;
}

uint32_t rjd_atomic_uint32_sub(struct rjd_atomic_uint32* atomic, uint32_t value)
{
	struct rjd_atomic_uint32_c11* atomic_c11 = (struct rjd_atomic_uint32_c11*)atomic;
	return atomic_fetch_sub(&atomic_c11->value, value) + value;
}

uint32_t rjd_atomic_uint32_inc(struct rjd_atomic_uint32* atomic)
{
	return rjd_atomic_uint32_add(atomic, 1);
}

uint32_t rjd_atomic_uint32_dec(struct rjd_atomic_uint32* atomic)
{
	return rjd_atomic_uint32_sub(atomic, 1);
}

bool rjd_atomic_uint32_compare_exchange(struct rjd_atomic_uint32* atomic, uint32_t* expected, uint32_t desired)
{
	struct rjd_atomic_uint32_c11* atomic_c11 = (struct rjd_atomic_uint32_c11*)atomic;
	return atomic_compare_exchange_strong(&atomic_c11->value, expected, desired);
}

////////////////////////////////////////////////////////////////////////////////

struct rjd_atomic_uint16 rjd_atomic_uint16_init(uint16_t value)
{
	struct rjd_atomic_uint16 atomic;
	struct rjd_atomic_uint16_c11* atomic_c11 = (struct rjd_atomic_uint16_c11*)&atomic;
	atomic_store(&atomic_c11->value, value);
	return atomic;
}

uint16_t rjd_atomic_uint16_get(struct rjd_atomic_uint16* atomic)
{
	struct rjd_atomic_uint16_c11* atomic_c11 = (struct rjd_atomic_uint16_c11*)atomic;
	return atomic_load(&atomic_c11->value);
}

uint16_t rjd_atomic_uint16_set(struct rjd_atomic_uint16* atomic, uint16_t value)
{
	struct rjd_atomic_uint16_c11* atomic_c11 = (struct rjd_atomic_uint16_c11*)atomic;
	atomic_store(&atomic_c11->value, value);
	return value;
}

uint16_t rjd_atomic_uint16_add(struct rjd_atomic_uint16* atomic, uint16_t value)
{
	struct rjd_atomic_uint16_c11* atomic_c11 = (struct rjd_atomic_uint16_c11*)atomic;
	return atomic_fetch_add(&atomic_c11->value, value) + value;
}

uint16_t rjd_atomic_uint16_sub(struct rjd_atomic_uint16* atomic, uint16_t value)
{
	struct rjd_atomic_uint16_c11* atomic_c11 = (struct rjd_atomic_uint16_c11*)atomic;
	return atomic_fetch_sub(&atomic_c11->value, value) + value;
}

uint16_t rjd_atomic_uint16_inc(struct rjd_atomic_uint16* atomic)
{
	return rjd_atomic_uint16_add(atomic, 1);
}

uint16_t rjd_atomic_uint16_dec(struct rjd_atomic_uint16* atomic)
{
	return rjd_atomic_uint16_sub(atomic, 1);
}

bool rjd_atomic_uint16_compare_exchange(struct rjd_atomic_uint16* atomic, uint16_t* expected, uint16_t desired)
{
	struct rjd_atomic_uint16_c11* atomic_c11 = (struct rjd_atomic_uint16_c11*)atomic;
	return atomic_compare_exchange_strong(&atomic_c11->value, expected, desired);
}

////////////////////////////////////////////////////////////////////////////////

struct rjd_atomic_uint8 rjd_atomic_uint8_init(uint8_t value)
{
	struct rjd_atomic_uint8 atomic;
	struct rjd_atomic_uint8_c11* atomic_c11 = (struct rjd_atomic_uint8_c11*)&atomic;
	atomic_store(&atomic_c11->value, value);
	return atomic;
}

uint8_t rjd_atomic_uint8_get(struct rjd_atomic_uint8* atomic)
{
	struct rjd_atomic_uint8_c11* atomic_c11 = (struct rjd_atomic_uint8_c11*)atomic;
	return atomic_load(&atomic_c11->value);
}

uint8_t rjd_atomic_uint8_set(struct rjd_atomic_uint8* atomic, uint8_t value)
{
	struct rjd_atomic_uint8_c11* atomic_c11 = (struct rjd_atomic_uint8_c11*)atomic;
	atomic_store(&atomic_c11->value, value);
	return value;
}

uint8_t rjd_atomic_uint8_add(struct rjd_atomic_uint8* atomic, uint8_t value)
{
	struct rjd_atomic_uint8_c11* atomic_c11 = (struct rjd_atomic_uint8_c11*)atomic;
	return atomic_fetch_add(&atomic_c11->value, value) + value;
}

uint8_t rjd_atomic_uint8_sub(struct rjd_atomic_uint8* atomic, uint8_t value)
{
	struct rjd_atomic_uint8_c11* atomic_c11 = (struct rjd_atomic_uint8_c11*)atomic;
	return atomic_fetch_sub(&atomic_c11->value, value) + value;
}

uint8_t rjd_atomic_uint8_inc(struct rjd_atomic_uint8* atomic)
{
	return rjd_atomic_uint8_add(atomic, 1);
}

uint8_t rjd_atomic_uint8_dec(struct rjd_atomic_uint8* atomic)
{
	return rjd_atomic_uint8_sub(atomic, 1);
}

bool rjd_atomic_uint8_compare_exchange(struct rjd_atomic_uint8* atomic, uint8_t* expected, uint8_t desired)
{
	struct rjd_atomic_uint8_c11* atomic_c11 = (struct rjd_atomic_uint8_c11*)atomic;
	return atomic_compare_exchange_strong(&atomic_c11->value, expected, desired);
}

#endif // RJD_PLATFORM
#endif // RJD_IMPL
