#pragma once

#define RJD_HASH

typedef struct {
	uint32_t value;
} rjd_hash32;

typedef struct {
	uint64_t value;
} rjd_hash64;

// You can pass -1 as the length to indicate a NULL-terminated buffer (e.g. c-style string)
rjd_hash32 rjd_hash32_data(const uint8_t* key, int length);
rjd_hash64 rjd_hash64_data(const uint8_t* key, int length);
bool rjd_hash32_valid(rjd_hash32 hash);
bool rjd_hash64_valid(rjd_hash64 hash);

#if RJD_ENABLE_SHORTNAMES
	#define hash32		rjd_hash32
	#define hash64		rjd_hash64
	#define hash32_data	rjd_hash32_data
	#define hash64_data	rjd_hash64_data
#endif

#if RJD_IMPL

// Code derived from:
// Copyright (c) 2011 Stephan Brumme. All rights reserved.
// see http://create.stephan-brumme.com/disclaimer.html
//
// originally developed by Fowler, Noll and Vo
// http://isthe.com/chongo/tech/comp/fnv/
//
// prime/seed from http://isthe.com/chongo/tech/comp/fnv/

rjd_hash32 rjd_hash32_data(const uint8_t* key, int length)
{
	RJD_ASSERT(length >= -1);

	if (key == NULL || length == 0 || (length == -1 && *key == '\0')) {
		rjd_hash32 hash = {0};
		return hash;
	}

	const uint32_t PRIME = 16777619;
	const uint32_t SEED  = 2166136261;

	rjd_hash32 hash = { SEED };
	if (length == -1) {
		while (*key) {
			hash.value = (*key++ ^ hash.value) * PRIME;
		}
	} else {
		while (length > 0)
		{
			--length;
			hash.value = (*key++ ^ hash.value) * PRIME;
		}
	}
	return hash;
}

rjd_hash64 rjd_hash64_data(const uint8_t* key, int length)
{
	RJD_ASSERT(length >= -1);

	if (key == NULL || length == 0 || (length == -1 && *key == '\0')) {
		rjd_hash64 hash = {0};
		return hash;
	}

	const uint64_t PRIME = 1099511628211ull;
	const uint64_t SEED  = 14695981039346656037ull;

	rjd_hash64 hash = { SEED };
	if (length == -1) {
		while (*key) {
			hash.value = (*key++ ^ hash.value) * PRIME;
		}
	} else {
		while (length > 0)
		{
			--length;
			hash.value = (*key++ ^ hash.value) * PRIME;
		}
	}
	return hash;
}

bool rjd_hash32_valid(rjd_hash32 hash)
{
	return hash.value != 0;
}

bool rjd_hash64_valid(rjd_hash64 hash)
{
	return hash.value != 0;
}

#endif // RJD_IMPL

