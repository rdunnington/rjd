#pragma once

struct rjd_atomic_int64  { char impl[8]; };
struct rjd_atomic_int32  { char impl[4]; };
struct rjd_atomic_int16  { char impl[2]; };
struct rjd_atomic_int8   { char impl[1]; };
struct rjd_atomic_uint64 { char impl[8]; };
struct rjd_atomic_uint32 { char impl[4]; };
struct rjd_atomic_uint16 { char impl[2]; };
struct rjd_atomic_uint8  { char impl[1]; };

struct rjd_atomic_int64 rjd_atomic_int64_init (int64_t  value);
int64_t rjd_atomic_int64_get(struct rjd_atomic_int64* atomic);
void rjd_atomic_int64_set(struct rjd_atomic_int64* atomic, int64_t value);
int64_t rjd_atomic_int64_add(struct rjd_atomic_int64* atomic, int64_t value);
int64_t rjd_atomic_int64_sub(struct rjd_atomic_int64* atomic, int64_t value);
int64_t rjd_atomic_int64_inc(struct rjd_atomic_int64* atomic);
int64_t rjd_atomic_int64_dec(struct rjd_atomic_int64* atomic);
bool rjd_atomic_int64_compare_exchange_weak(struct rjd_atomic_int64* atomic, int64_t* expected, int64_t desired);
bool rjd_atomic_int64_compare_exchange_strong(struct rjd_atomic_int64* atomic, int64_t* expected, int64_t desired);

struct rjd_atomic_int32 rjd_atomic_int32_init (int32_t  value);
int32_t rjd_atomic_int32_get(struct rjd_atomic_int32* atomic);
void rjd_atomic_int32_set(struct rjd_atomic_int32* atomic, int32_t value);
int32_t rjd_atomic_int32_add(struct rjd_atomic_int32* atomic, int32_t value);
int32_t rjd_atomic_int32_sub(struct rjd_atomic_int32* atomic, int32_t value);
int32_t rjd_atomic_int32_inc(struct rjd_atomic_int32* atomic);
int32_t rjd_atomic_int32_dec(struct rjd_atomic_int32* atomic);
bool rjd_atomic_int32_compare_exchange_weak(struct rjd_atomic_int32* atomic, int32_t* expected, int32_t desired);
bool rjd_atomic_int32_compare_exchange_strong(struct rjd_atomic_int32* atomic, int32_t* expected, int32_t desired);

struct rjd_atomic_int16 rjd_atomic_int16_init (int16_t  value);
int16_t rjd_atomic_int16_get(struct rjd_atomic_int16* atomic);
void rjd_atomic_int16_set(struct rjd_atomic_int16* atomic, int16_t value);
int16_t rjd_atomic_int16_add(struct rjd_atomic_int16* atomic, int16_t value);
int16_t rjd_atomic_int16_sub(struct rjd_atomic_int16* atomic, int16_t value);
int16_t rjd_atomic_int16_inc(struct rjd_atomic_int16* atomic);
int16_t rjd_atomic_int16_dec(struct rjd_atomic_int16* atomic);
bool rjd_atomic_int16_compare_exchange_weak(struct rjd_atomic_int16* atomic, int16_t* expected, int16_t desired);
bool rjd_atomic_int16_compare_exchange_strong(struct rjd_atomic_int16* atomic, int16_t* expected, int16_t desired);

struct rjd_atomic_int8 rjd_atomic_int8_init (int8_t  value);
int8_t rjd_atomic_int8_get(struct rjd_atomic_int8* atomic);
void rjd_atomic_int8_set(struct rjd_atomic_int8* atomic, int8_t value);
int8_t rjd_atomic_int8_add(struct rjd_atomic_int8* atomic, int8_t value);
int8_t rjd_atomic_int8_sub(struct rjd_atomic_int8* atomic, int8_t value);
int8_t rjd_atomic_int8_inc(struct rjd_atomic_int8* atomic);
int8_t rjd_atomic_int8_dec(struct rjd_atomic_int8* atomic);
bool rjd_atomic_int8_compare_exchange_weak(struct rjd_atomic_int8* atomic, int8_t* expected, int8_t desired);
bool rjd_atomic_int8_compare_exchange_strong(struct rjd_atomic_int8* atomic, int8_t* expected, int8_t desired);

struct rjd_atomic_uint64 rjd_atomic_uint64_init (uint64_t  value);
uint64_t rjd_atomic_uint64_get(struct rjd_atomic_uint64* atomic);
void rjd_atomic_uint64_set(struct rjd_atomic_uint64* atomic, uint64_t value);
uint64_t rjd_atomic_uint64_add(struct rjd_atomic_uint64* atomic, uint64_t value);
uint64_t rjd_atomic_uint64_sub(struct rjd_atomic_uint64* atomic, uint64_t value);
uint64_t rjd_atomic_uint64_inc(struct rjd_atomic_uint64* atomic);
uint64_t rjd_atomic_uint64_dec(struct rjd_atomic_uint64* atomic);
bool rjd_atomic_uint64_compare_exchange_weak(struct rjd_atomic_uint64* atomic, uint64_t* expected, uint64_t desired);
bool rjd_atomic_uint64_compare_exchange_strong(struct rjd_atomic_uint64* atomic, uint64_t* expected, uint64_t desired);

struct rjd_atomic_uint32 rjd_atomic_uint32_init (uint32_t  value);
uint32_t rjd_atomic_uint32_get(struct rjd_atomic_uint32* atomic);
void rjd_atomic_uint32_set(struct rjd_atomic_uint32* atomic, uint32_t value);
uint32_t rjd_atomic_uint32_add(struct rjd_atomic_uint32* atomic, uint32_t value);
uint32_t rjd_atomic_uint32_sub(struct rjd_atomic_uint32* atomic, uint32_t value);
uint32_t rjd_atomic_uint32_inc(struct rjd_atomic_uint32* atomic);
uint32_t rjd_atomic_uint32_dec(struct rjd_atomic_uint32* atomic);
bool rjd_atomic_uint32_compare_exchange_weak(struct rjd_atomic_uint32* atomic, uint32_t* expected, uint32_t desired);
bool rjd_atomic_uint32_compare_exchange_strong(struct rjd_atomic_uint32* atomic, uint32_t* expected, uint32_t desired);

struct rjd_atomic_uint16 rjd_atomic_uint16_init (uint16_t  value);
uint16_t rjd_atomic_uint16_get(struct rjd_atomic_uint16* atomic);
void rjd_atomic_uint16_set(struct rjd_atomic_uint16* atomic, uint16_t value);
uint16_t rjd_atomic_uint16_add(struct rjd_atomic_uint16* atomic, uint16_t value);
uint16_t rjd_atomic_uint16_sub(struct rjd_atomic_uint16* atomic, uint16_t value);
uint16_t rjd_atomic_uint16_inc(struct rjd_atomic_uint16* atomic);
uint16_t rjd_atomic_uint16_dec(struct rjd_atomic_uint16* atomic);
bool rjd_atomic_uint16_compare_exchange_weak(struct rjd_atomic_uint16* atomic, uint16_t* expected, uint16_t desired);
bool rjd_atomic_uint16_compare_exchange_strong(struct rjd_atomic_uint16* atomic, uint16_t* expected, uint16_t desired);

struct rjd_atomic_uint8 rjd_atomic_uint8_init (uint8_t  value);
uint8_t rjd_atomic_uint8_get(struct rjd_atomic_uint8* atomic);
void rjd_atomic_uint8_set(struct rjd_atomic_uint8* atomic, uint8_t value);
uint8_t rjd_atomic_uint8_add(struct rjd_atomic_uint8* atomic, uint8_t value);
uint8_t rjd_atomic_uint8_sub(struct rjd_atomic_uint8* atomic, uint8_t value);
uint8_t rjd_atomic_uint8_inc(struct rjd_atomic_uint8* atomic);
uint8_t rjd_atomic_uint8_dec(struct rjd_atomic_uint8* atomic);
bool rjd_atomic_uint8_compare_exchange_weak(struct rjd_atomic_uint8* atomic, uint8_t* expected, uint8_t desired);
bool rjd_atomic_uint8_compare_exchange_strong(struct rjd_atomic_uint8* atomic, uint8_t* expected, uint8_t desired);

#if RJD_IMPL

#if RJD_COMPILER_MSVC

// TODO

#elif RJD_COMPILER_GCC || RJD_COMPILER_CLANG
#include <stdatomic.h>

struct rjd_atomic_int64_osx
{
	_Atomic int64_t value;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_int64) <= sizeof(struct rjd_atomic_int64));

struct rjd_atomic_int32_osx
{
	_Atomic int32_t value;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_int32) <= sizeof(struct rjd_atomic_int32));

struct rjd_atomic_int16_osx
{
	_Atomic int16_t value;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_int16) <= sizeof(struct rjd_atomic_int16));

struct rjd_atomic_int8_osx
{
	_Atomic int8_t value;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_int8) <= sizeof(struct rjd_atomic_int8));

struct rjd_atomic_uint64_osx
{
	_Atomic uint64_t value;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_uint64) <= sizeof(struct rjd_atomic_uint64));

struct rjd_atomic_uint32_osx
{
	_Atomic uint32_t value;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_int32) <= sizeof(struct rjd_atomic_int32));

struct rjd_atomic_uint16_osx
{
	_Atomic uint16_t value;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_uint16) <= sizeof(struct rjd_atomic_uint16));

struct rjd_atomic_uint8_osx
{
	_Atomic uint8_t value;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_atomic_uint8) <= sizeof(struct rjd_atomic_uint8));

////////////////////////////////////////////////////////////////////////////////
// Implementation

struct rjd_atomic_int64 rjd_atomic_int64_init (int64_t value)
{
	struct rjd_atomic_int64 atomic;
	struct rjd_atomic_int64_osx* atomic_osx = (struct rjd_atomic_int64_osx*)&atomic;
	atomic_store(&atomic_osx->value, value);
	return atomic;
}

int64_t rjd_atomic_int64_get(struct rjd_atomic_int64* atomic)
{
	struct rjd_atomic_int64_osx* atomic_osx = (struct rjd_atomic_int64_osx*)atomic;
	return atomic_load(&atomic_osx->value);
}

void rjd_atomic_int64_set(struct rjd_atomic_int64* atomic, int64_t value)
{
	struct rjd_atomic_int64_osx* atomic_osx = (struct rjd_atomic_int64_osx*)atomic;
	atomic_store(&atomic_osx->value, value);
}

int64_t rjd_atomic_int64_add(struct rjd_atomic_int64* atomic, int64_t value)
{
	struct rjd_atomic_int64_osx* atomic_osx = (struct rjd_atomic_int64_osx*)atomic;
	return atomic_fetch_add(&atomic_osx->value, value); // returns original value
}

int64_t rjd_atomic_int64_sub(struct rjd_atomic_int64* atomic, int64_t value)
{
	struct rjd_atomic_int64_osx* atomic_osx = (struct rjd_atomic_int64_osx*)atomic;
	return atomic_fetch_sub(&atomic_osx->value, value); // returns original value
}

int64_t rjd_atomic_int64_inc(struct rjd_atomic_int64* atomic)
{
	return rjd_atomic_int64_add(atomic, 1);
}

int64_t rjd_atomic_int64_dec(struct rjd_atomic_int64* atomic)
{
	return rjd_atomic_int64_sub(atomic, 1);
}

bool rjd_atomic_int64_compare_exchange_weak(struct rjd_atomic_int64* atomic, int64_t* expected, int64_t desired)
{
	struct rjd_atomic_int64_osx* atomic_osx = (struct rjd_atomic_int64_osx*)atomic;
	return atomic_compare_exchange_weak(&atomic_osx->value, expected, desired);
}

bool rjd_atomic_int64_compare_exchange_strong(struct rjd_atomic_int64* atomic, int64_t* expected, int64_t desired)
{
	struct rjd_atomic_int64_osx* atomic_osx = (struct rjd_atomic_int64_osx*)atomic;
	return atomic_compare_exchange_strong(&atomic_osx->value, expected, desired);
}

////////////////////////////////////////////////////////////////////////////////

struct rjd_atomic_int32 rjd_atomic_int32_init (int32_t value)
{
	struct rjd_atomic_int32 atomic;
	struct rjd_atomic_int32_osx* atomic_osx = (struct rjd_atomic_int32_osx*)&atomic;
	atomic_store(&atomic_osx->value, value);
	return atomic;
}

int32_t rjd_atomic_int32_get(struct rjd_atomic_int32* atomic)
{
	struct rjd_atomic_int32_osx* atomic_osx = (struct rjd_atomic_int32_osx*)atomic;
	return atomic_load(&atomic_osx->value);
}

void rjd_atomic_int32_set(struct rjd_atomic_int32* atomic, int32_t value)
{
	struct rjd_atomic_int32_osx* atomic_osx = (struct rjd_atomic_int32_osx*)atomic;
	atomic_store(&atomic_osx->value, value);
}

int32_t rjd_atomic_int32_add(struct rjd_atomic_int32* atomic, int32_t value)
{
	struct rjd_atomic_int32_osx* atomic_osx = (struct rjd_atomic_int32_osx*)atomic;
	return atomic_fetch_add(&atomic_osx->value, value); // returns original value
}

int32_t rjd_atomic_int32_sub(struct rjd_atomic_int32* atomic, int32_t value)
{
	struct rjd_atomic_int32_osx* atomic_osx = (struct rjd_atomic_int32_osx*)atomic;
	return atomic_fetch_sub(&atomic_osx->value, value); // returns original value
}

int32_t rjd_atomic_int32_inc(struct rjd_atomic_int32* atomic)
{
	return rjd_atomic_int32_add(atomic, 1);
}

int32_t rjd_atomic_int32_dec(struct rjd_atomic_int32* atomic)
{
	return rjd_atomic_int32_sub(atomic, 1);
}

bool rjd_atomic_int32_compare_exchange_weak(struct rjd_atomic_int32* atomic, int32_t* expected, int32_t desired)
{
	struct rjd_atomic_int32_osx* atomic_osx = (struct rjd_atomic_int32_osx*)atomic;
	return atomic_compare_exchange_weak(&atomic_osx->value, expected, desired);
}

bool rjd_atomic_int32_compare_exchange_strong(struct rjd_atomic_int32* atomic, int32_t* expected, int32_t desired)
{
	struct rjd_atomic_int32_osx* atomic_osx = (struct rjd_atomic_int32_osx*)atomic;
	return atomic_compare_exchange_strong(&atomic_osx->value, expected, desired);
}

////////////////////////////////////////////////////////////////////////////////

struct rjd_atomic_int16 rjd_atomic_int16_init (int16_t value)
{
	struct rjd_atomic_int16 atomic;
	struct rjd_atomic_int16_osx* atomic_osx = (struct rjd_atomic_int16_osx*)&atomic;
	atomic_store(&atomic_osx->value, value);
	return atomic;
}

int16_t rjd_atomic_int16_get(struct rjd_atomic_int16* atomic)
{
	struct rjd_atomic_int16_osx* atomic_osx = (struct rjd_atomic_int16_osx*)atomic;
	return atomic_load(&atomic_osx->value);
}

void rjd_atomic_int16_set(struct rjd_atomic_int16* atomic, int16_t value)
{
	struct rjd_atomic_int16_osx* atomic_osx = (struct rjd_atomic_int16_osx*)atomic;
	atomic_store(&atomic_osx->value, value);
}

int16_t rjd_atomic_int16_add(struct rjd_atomic_int16* atomic, int16_t value)
{
	struct rjd_atomic_int16_osx* atomic_osx = (struct rjd_atomic_int16_osx*)atomic;
	return atomic_fetch_add(&atomic_osx->value, value); // returns original value
}

int16_t rjd_atomic_int16_sub(struct rjd_atomic_int16* atomic, int16_t value)
{
	struct rjd_atomic_int16_osx* atomic_osx = (struct rjd_atomic_int16_osx*)atomic;
	return atomic_fetch_sub(&atomic_osx->value, value); // returns original value
}

int16_t rjd_atomic_int16_inc(struct rjd_atomic_int16* atomic)
{
	return rjd_atomic_int16_add(atomic, 1);
}

int16_t rjd_atomic_int16_dec(struct rjd_atomic_int16* atomic)
{
	return rjd_atomic_int16_sub(atomic, 1);
}

bool rjd_atomic_int16_compare_exchange_weak(struct rjd_atomic_int16* atomic, int16_t* expected, int16_t desired)
{
	struct rjd_atomic_int16_osx* atomic_osx = (struct rjd_atomic_int16_osx*)atomic;
	return atomic_compare_exchange_weak(&atomic_osx->value, expected, desired);
}

bool rjd_atomic_int16_compare_exchange_strong(struct rjd_atomic_int16* atomic, int16_t* expected, int16_t desired)
{
	struct rjd_atomic_int16_osx* atomic_osx = (struct rjd_atomic_int16_osx*)atomic;
	return atomic_compare_exchange_strong(&atomic_osx->value, expected, desired);
}

////////////////////////////////////////////////////////////////////////////////

struct rjd_atomic_int8 rjd_atomic_int8_init (int8_t value)
{
	struct rjd_atomic_int8 atomic;
	struct rjd_atomic_int8_osx* atomic_osx = (struct rjd_atomic_int8_osx*)&atomic;
	atomic_store(&atomic_osx->value, value);
	return atomic;
}

int8_t rjd_atomic_int8_get(struct rjd_atomic_int8* atomic)
{
	struct rjd_atomic_int8_osx* atomic_osx = (struct rjd_atomic_int8_osx*)atomic;
	return atomic_load(&atomic_osx->value);
}

void rjd_atomic_int8_set(struct rjd_atomic_int8* atomic, int8_t value)
{
	struct rjd_atomic_int8_osx* atomic_osx = (struct rjd_atomic_int8_osx*)atomic;
	atomic_store(&atomic_osx->value, value);
}

int8_t rjd_atomic_int8_add(struct rjd_atomic_int8* atomic, int8_t value)
{
	struct rjd_atomic_int8_osx* atomic_osx = (struct rjd_atomic_int8_osx*)atomic;
	return atomic_fetch_add(&atomic_osx->value, value); // returns original value
}

int8_t rjd_atomic_int8_sub(struct rjd_atomic_int8* atomic, int8_t value)
{
	struct rjd_atomic_int8_osx* atomic_osx = (struct rjd_atomic_int8_osx*)atomic;
	return atomic_fetch_sub(&atomic_osx->value, value); // returns original value
}

int8_t rjd_atomic_int8_inc(struct rjd_atomic_int8* atomic)
{
	return rjd_atomic_int8_add(atomic, 1);
}

int8_t rjd_atomic_int8_dec(struct rjd_atomic_int8* atomic)
{
	return rjd_atomic_int8_sub(atomic, 1);
}

bool rjd_atomic_int8_compare_exchange_weak(struct rjd_atomic_int8* atomic, int8_t* expected, int8_t desired)
{
	struct rjd_atomic_int8_osx* atomic_osx = (struct rjd_atomic_int8_osx*)atomic;
	return atomic_compare_exchange_weak(&atomic_osx->value, expected, desired);
}

bool rjd_atomic_int8_compare_exchange_strong(struct rjd_atomic_int8* atomic, int8_t* expected, int8_t desired)
{
	struct rjd_atomic_int8_osx* atomic_osx = (struct rjd_atomic_int8_osx*)atomic;
	return atomic_compare_exchange_strong(&atomic_osx->value, expected, desired);
}

////////////////////////////////////////////////////////////////////////////////

struct rjd_atomic_uint64 rjd_atomic_uint64_init (uint64_t value)
{
	struct rjd_atomic_uint64 atomic;
	struct rjd_atomic_uint64_osx* atomic_osx = (struct rjd_atomic_uint64_osx*)&atomic;
	atomic_store(&atomic_osx->value, value);
	return atomic;
}

uint64_t rjd_atomic_uint64_get(struct rjd_atomic_uint64* atomic)
{
	struct rjd_atomic_uint64_osx* atomic_osx = (struct rjd_atomic_uint64_osx*)atomic;
	return atomic_load(&atomic_osx->value);
}

void rjd_atomic_uint64_set(struct rjd_atomic_uint64* atomic, uint64_t value)
{
	struct rjd_atomic_uint64_osx* atomic_osx = (struct rjd_atomic_uint64_osx*)atomic;
	atomic_store(&atomic_osx->value, value);
}

uint64_t rjd_atomic_uint64_add(struct rjd_atomic_uint64* atomic, uint64_t value)
{
	struct rjd_atomic_uint64_osx* atomic_osx = (struct rjd_atomic_uint64_osx*)atomic;
	return atomic_fetch_add(&atomic_osx->value, value); // returns original value
}

uint64_t rjd_atomic_uint64_sub(struct rjd_atomic_uint64* atomic, uint64_t value)
{
	struct rjd_atomic_uint64_osx* atomic_osx = (struct rjd_atomic_uint64_osx*)atomic;
	return atomic_fetch_sub(&atomic_osx->value, value); // returns original value
}

uint64_t rjd_atomic_uint64_inc(struct rjd_atomic_uint64* atomic)
{
	return rjd_atomic_uint64_add(atomic, 1);
}

uint64_t rjd_atomic_uint64_dec(struct rjd_atomic_uint64* atomic)
{
	return rjd_atomic_uint64_sub(atomic, 1);
}

bool rjd_atomic_uint64_compare_exchange_weak(struct rjd_atomic_uint64* atomic, uint64_t* expected, uint64_t desired)
{
	struct rjd_atomic_uint64_osx* atomic_osx = (struct rjd_atomic_uint64_osx*)atomic;
	return atomic_compare_exchange_weak(&atomic_osx->value, expected, desired);
}

bool rjd_atomic_uint64_compare_exchange_strong(struct rjd_atomic_uint64* atomic, uint64_t* expected, uint64_t desired)
{
	struct rjd_atomic_uint64_osx* atomic_osx = (struct rjd_atomic_uint64_osx*)atomic;
	return atomic_compare_exchange_strong(&atomic_osx->value, expected, desired);
}

////////////////////////////////////////////////////////////////////////////////

struct rjd_atomic_uint32 rjd_atomic_uint32_init (uint32_t value)
{
	struct rjd_atomic_uint32 atomic;
	struct rjd_atomic_uint32_osx* atomic_osx = (struct rjd_atomic_uint32_osx*)&atomic;
	atomic_store(&atomic_osx->value, value);
	return atomic;
}

uint32_t rjd_atomic_uint32_get(struct rjd_atomic_uint32* atomic)
{
	struct rjd_atomic_uint32_osx* atomic_osx = (struct rjd_atomic_uint32_osx*)atomic;
	return atomic_load(&atomic_osx->value);
}

void rjd_atomic_uint32_set(struct rjd_atomic_uint32* atomic, uint32_t value)
{
	struct rjd_atomic_uint32_osx* atomic_osx = (struct rjd_atomic_uint32_osx*)atomic;
	atomic_store(&atomic_osx->value, value);
}

uint32_t rjd_atomic_uint32_add(struct rjd_atomic_uint32* atomic, uint32_t value)
{
	struct rjd_atomic_uint32_osx* atomic_osx = (struct rjd_atomic_uint32_osx*)atomic;
	return atomic_fetch_add(&atomic_osx->value, value); // returns original value
}

uint32_t rjd_atomic_uint32_sub(struct rjd_atomic_uint32* atomic, uint32_t value)
{
	struct rjd_atomic_uint32_osx* atomic_osx = (struct rjd_atomic_uint32_osx*)atomic;
	return atomic_fetch_sub(&atomic_osx->value, value); // returns original value
}

uint32_t rjd_atomic_uint32_inc(struct rjd_atomic_uint32* atomic)
{
	return rjd_atomic_uint32_add(atomic, 1);
}

uint32_t rjd_atomic_uint32_dec(struct rjd_atomic_uint32* atomic)
{
	return rjd_atomic_uint32_sub(atomic, 1);
}

bool rjd_atomic_uint32_compare_exchange_weak(struct rjd_atomic_uint32* atomic, uint32_t* expected, uint32_t desired)
{
	struct rjd_atomic_uint32_osx* atomic_osx = (struct rjd_atomic_uint32_osx*)atomic;
	return atomic_compare_exchange_weak(&atomic_osx->value, expected, desired);
}

bool rjd_atomic_uint32_compare_exchange_strong(struct rjd_atomic_uint32* atomic, uint32_t* expected, uint32_t desired)
{
	struct rjd_atomic_uint32_osx* atomic_osx = (struct rjd_atomic_uint32_osx*)atomic;
	return atomic_compare_exchange_strong(&atomic_osx->value, expected, desired);
}

struct rjd_atomic_uint16 rjd_atomic_uint16_init (uint16_t value)
{
	struct rjd_atomic_uint16 atomic;
	struct rjd_atomic_uint16_osx* atomic_osx = (struct rjd_atomic_uint16_osx*)&atomic;
	atomic_store(&atomic_osx->value, value);
	return atomic;
}

////////////////////////////////////////////////////////////////////////////////

uint16_t rjd_atomic_uint16_get(struct rjd_atomic_uint16* atomic)
{
	struct rjd_atomic_uint16_osx* atomic_osx = (struct rjd_atomic_uint16_osx*)atomic;
	return atomic_load(&atomic_osx->value);
}

void rjd_atomic_uint16_set(struct rjd_atomic_uint16* atomic, uint16_t value)
{
	struct rjd_atomic_uint16_osx* atomic_osx = (struct rjd_atomic_uint16_osx*)atomic;
	atomic_store(&atomic_osx->value, value);
}

uint16_t rjd_atomic_uint16_add(struct rjd_atomic_uint16* atomic, uint16_t value)
{
	struct rjd_atomic_uint16_osx* atomic_osx = (struct rjd_atomic_uint16_osx*)atomic;
	return atomic_fetch_add(&atomic_osx->value, value); // returns original value
}

uint16_t rjd_atomic_uint16_sub(struct rjd_atomic_uint16* atomic, uint16_t value)
{
	struct rjd_atomic_uint16_osx* atomic_osx = (struct rjd_atomic_uint16_osx*)atomic;
	return atomic_fetch_sub(&atomic_osx->value, value); // returns original value
}

uint16_t rjd_atomic_uint16_inc(struct rjd_atomic_uint16* atomic)
{
	return rjd_atomic_uint16_add(atomic, 1);
}

uint16_t rjd_atomic_uint16_dec(struct rjd_atomic_uint16* atomic)
{
	return rjd_atomic_uint16_sub(atomic, 1);
}

bool rjd_atomic_uint16_compare_exchange_weak(struct rjd_atomic_uint16* atomic, uint16_t* expected, uint16_t desired)
{
	struct rjd_atomic_uint16_osx* atomic_osx = (struct rjd_atomic_uint16_osx*)atomic;
	return atomic_compare_exchange_weak(&atomic_osx->value, expected, desired);
}

bool rjd_atomic_uint16_compare_exchange_strong(struct rjd_atomic_uint16* atomic, uint16_t* expected, uint16_t desired)
{
	struct rjd_atomic_uint16_osx* atomic_osx = (struct rjd_atomic_uint16_osx*)atomic;
	return atomic_compare_exchange_strong(&atomic_osx->value, expected, desired);
}

////////////////////////////////////////////////////////////////////////////////

struct rjd_atomic_uint8 rjd_atomic_uint8_init (uint8_t value)
{
	struct rjd_atomic_uint8 atomic;
	struct rjd_atomic_uint8_osx* atomic_osx = (struct rjd_atomic_uint8_osx*)&atomic;
	atomic_store(&atomic_osx->value, value);
	return atomic;
}

uint8_t rjd_atomic_uint8_get(struct rjd_atomic_uint8* atomic)
{
	struct rjd_atomic_uint8_osx* atomic_osx = (struct rjd_atomic_uint8_osx*)atomic;
	return atomic_load(&atomic_osx->value);
}

void rjd_atomic_uint8_set(struct rjd_atomic_uint8* atomic, uint8_t value)
{
	struct rjd_atomic_uint8_osx* atomic_osx = (struct rjd_atomic_uint8_osx*)atomic;
	atomic_store(&atomic_osx->value, value);
}

uint8_t rjd_atomic_uint8_add(struct rjd_atomic_uint8* atomic, uint8_t value)
{
	struct rjd_atomic_uint8_osx* atomic_osx = (struct rjd_atomic_uint8_osx*)atomic;
	return atomic_fetch_add(&atomic_osx->value, value); // returns original value
}

uint8_t rjd_atomic_uint8_sub(struct rjd_atomic_uint8* atomic, uint8_t value)
{
	struct rjd_atomic_uint8_osx* atomic_osx = (struct rjd_atomic_uint8_osx*)atomic;
	return atomic_fetch_sub(&atomic_osx->value, value); // returns original value
}

uint8_t rjd_atomic_uint8_inc(struct rjd_atomic_uint8* atomic)
{
	return rjd_atomic_uint8_add(atomic, 1);
}

uint8_t rjd_atomic_uint8_dec(struct rjd_atomic_uint8* atomic)
{
	return rjd_atomic_uint8_sub(atomic, 1);
}

bool rjd_atomic_uint8_compare_exchange_weak(struct rjd_atomic_uint8* atomic, uint8_t* expected, uint8_t desired)
{
	struct rjd_atomic_uint8_osx* atomic_osx = (struct rjd_atomic_uint8_osx*)atomic;
	return atomic_compare_exchange_weak(&atomic_osx->value, expected, desired);
}

bool rjd_atomic_uint8_compare_exchange_strong(struct rjd_atomic_uint8* atomic, uint8_t* expected, uint8_t desired)
{
	struct rjd_atomic_uint8_osx* atomic_osx = (struct rjd_atomic_uint8_osx*)atomic;
	return atomic_compare_exchange_strong(&atomic_osx->value, expected, desired);
}

#endif // RJD_PLATFORM
#endif // RJD_IMPL

