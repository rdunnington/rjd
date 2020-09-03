#pragma once

#define RJD_HASH_H 1

struct rjd_hash32
{
	uint32_t value;
};

struct rjd_hash64
{
	uint64_t value;
};

enum
{
	RJD_HASH_NULLTERMINATED_BUFFER = -1,
};

const extern struct rjd_hash32 RJD_HASH32_INVALID;
const extern struct rjd_hash64 RJD_HASH64_INVALID;

// You can pass -1 as the length to indicate a NULL-terminated buffer (e.g. c-style string)
struct rjd_hash32 rjd_hash32_data(const uint8_t* key, int length);
struct rjd_hash64 rjd_hash64_data(const uint8_t* key, int length);
static inline struct rjd_hash32 rjd_hash32_str(const char* key);
static inline struct rjd_hash64 rjd_hash64_str(const char* key);
bool rjd_hash32_valid(struct rjd_hash32 hash);
bool rjd_hash64_valid(struct rjd_hash64 hash);

////////////////////////////////////////////////////////////////////////////////
// Inline implementation

static inline struct rjd_hash32 rjd_hash32_str(const char* key)
{
	const void* data = key;
	return rjd_hash32_data(data, -1);
}

static inline struct rjd_hash64 rjd_hash64_str(const char* key)
{
	const void* data = key;
	return rjd_hash64_data(data, -1);
}

#if RJD_IMPL

const struct rjd_hash32 RJD_HASH32_INVALID = {0};
const struct rjd_hash64 RJD_HASH64_INVALID = {0};

// Code derived from:
// Copyright (c) 2011 Stephan Brumme. All rights reserved.
// see http://create.stephan-brumme.com/disclaimer.html
//
// originally developed by Fowler, Noll and Vo
// http://isthe.com/chongo/tech/comp/fnv/
//
// prime/seed from http://isthe.com/chongo/tech/comp/fnv/

struct rjd_hash32 rjd_hash32_data(const uint8_t* key, int length)
{
	RJD_ASSERT(length >= -1);

	if (key == NULL || length == 0 || (length == -1 && *key == '\0')) {
		struct rjd_hash32 hash = {0};
		return hash;
	}

	const uint64_t PRIME = 16777619;
	const uint64_t SEED  = 2166136261;

	struct rjd_hash32 hash = { SEED };
	if (length == -1) {
		while (*key) {
			hash.value = (*key++ ^ hash.value) * PRIME;
		}
	} else {
		while (length > 0)
		{
			--length;
			hash.value = (uint32_t)((*key++ ^ hash.value) * PRIME);
		}
	}
	return hash;
}

struct rjd_hash64 rjd_hash64_data(const uint8_t* key, int length)
{
	RJD_ASSERT(length >= -1);

	if (key == NULL || length == 0 || (length == -1 && *key == '\0')) {
		struct rjd_hash64 hash = {0};
		return hash;
	}

	const uint64_t PRIME = 1099511628211ull;
	const uint64_t SEED  = 14695981039346656037ull;

	struct rjd_hash64 hash = { SEED };
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

bool rjd_hash32_valid(struct rjd_hash32 hash)
{
	return hash.value != 0;
}

bool rjd_hash64_valid(struct rjd_hash64 hash)
{
	return hash.value != 0;
}

#endif // RJD_IMPL

