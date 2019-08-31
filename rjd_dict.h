#pragma once

#define RJD_DICT_H 1

struct rjd_mem_allocator;

struct rjd_dict
{
	uint32_t count;
	struct rjd_hash64* hashes;
	void** values;
	struct rjd_mem_allocator* allocator;
};

struct rjd_dict rjd_dict_init(struct rjd_mem_allocator* allocator, size_t initial_capacity);
void rjd_dict_insert(struct rjd_dict* dict, struct rjd_hash64 hash, void* item);
void* rjd_dict_erase(struct rjd_dict* dict, struct rjd_hash64 hash);
void* rjd_dict_get(const struct rjd_dict* dict, struct rjd_hash64 hash);
bool rjd_dict_has(const struct rjd_dict* dict, struct rjd_hash64 hash);
void rjd_dict_free(struct rjd_dict* dict);

static inline void rjd_dict_insert_hashstr(struct rjd_dict* dict, const char* key, void* item);
static inline void* rjd_dict_erase_hashstr(struct rjd_dict* dict, const char* key);
static inline void* rjd_dict_get_hashstr(const struct rjd_dict* dict, const char* key);
static inline bool rjd_dict_has_hashstr(const struct rjd_dict* dict, const char* key);

static inline void rjd_dict_insert_hashstr(struct rjd_dict* dict, const char* key, void* item)
{
	rjd_dict_insert(dict, rjd_hash64_data((uint8_t*)key, -1), item);
}

static inline void* rjd_dict_erase_hashstr(struct rjd_dict* dict, const char* key)
{
	return rjd_dict_erase(dict, rjd_hash64_data((uint8_t*)key, -1));
}

static inline void* rjd_dict_get_hashstr(const struct rjd_dict* dict, const char* key)
{
	return rjd_dict_get(dict, rjd_hash64_data((uint8_t*)key, -1));
}

static inline bool rjd_dict_has_hashstr(const struct rjd_dict* dict, const char* key)
{
	return rjd_dict_has(dict, rjd_hash64_data((uint8_t*)key, -1));
}

#if RJD_IMPL

enum rjd_dict_findmode
{
	RJD_DICT_FINDMODE_INSERTION,
	RJD_DICT_FINDMODE_EXISTING,
};

static void rjd_dict_grow(struct rjd_dict* dict, size_t capacity);
static int32_t rjd_dict_findindex(const struct rjd_hash64* hashes, struct rjd_hash64 hash, enum rjd_dict_findmode mode);
 
struct rjd_dict rjd_dict_init(struct rjd_mem_allocator* allocator, size_t initial_capacity)
{
	RJD_ASSERT(allocator);

	struct rjd_dict dict = { 0, NULL, NULL, allocator };

	if (initial_capacity > 0) {
		rjd_dict_grow(&dict, initial_capacity);
	}

	return dict;
}

void rjd_dict_insert(struct rjd_dict* dict, struct rjd_hash64 hash, void* value)
{
	RJD_ASSERT(dict);
	RJD_ASSERT(rjd_hash64_valid(hash));

	const float load = dict->hashes ? (dict->count + 1) / (float)rjd_array_capacity(dict->hashes) : 1;
	if (load > 0.6) {
		uint32_t capacity = dict->hashes ? rjd_array_capacity(dict->hashes) * 2 : 32;
		rjd_dict_grow(dict, capacity);
	}

	int32_t i = rjd_dict_findindex(dict->hashes, hash, RJD_DICT_FINDMODE_INSERTION);
	RJD_ASSERT(i >= 0);
	RJD_ASSERT(!rjd_hash64_valid(dict->hashes[i]));

	dict->hashes[i] = hash;
	dict->values[i] = value;
	++dict->count;
}

void* rjd_dict_erase(struct rjd_dict* dict, struct rjd_hash64 hash)
{
	RJD_ASSERT(dict);
	
	int32_t index = rjd_dict_findindex(dict->hashes, hash, RJD_DICT_FINDMODE_EXISTING);
	if (index < 0) {
		return NULL;
	}

	void* v = dict->values[index];

	dict->hashes[index] = rjd_hash64_data(NULL, 0);
	dict->values[index] = NULL;
	--dict->count;

	return v;
}

void* rjd_dict_get(const struct rjd_dict* dict, struct rjd_hash64 hash)
{
	RJD_ASSERT(dict);

	int32_t index = rjd_dict_findindex(dict->hashes, hash, RJD_DICT_FINDMODE_EXISTING);
	if (index < 0) {
		return NULL;
	}

	return dict->values[index];
}

bool rjd_dict_has(const struct rjd_dict* dict, struct rjd_hash64 hash)
{
	RJD_ASSERT(dict);

	if (!rjd_hash64_valid(hash)) {
		return false;
	}

	int32_t index = rjd_dict_findindex(dict->hashes, hash, RJD_DICT_FINDMODE_EXISTING);
	if (index >= 0) {
		return rjd_hash64_valid(dict->hashes[index]);
	}

	return false;
}

void rjd_dict_free(struct rjd_dict* dict)
{
	RJD_ASSERT(dict);

	rjd_array_free(dict->hashes);
	rjd_array_free(dict->values);
	dict->hashes = NULL;
	dict->values = NULL;
	dict->count = 0;
}

static void rjd_dict_grow(struct rjd_dict* dict, size_t capacity)
{
	RJD_ASSERT(capacity > 0);
	RJD_ASSERT(dict);

	struct rjd_hash64* hashes = rjd_array_alloc(struct rjd_hash64, (uint32_t)capacity, dict->allocator);
	void** values = rjd_array_alloc(void*, (uint32_t)capacity, dict->allocator);

	rjd_array_resize(hashes, (uint32_t)capacity);
	rjd_array_resize(values, (uint32_t)capacity);

	for (uint32_t i = 0; i < rjd_array_count(dict->hashes); ++i) {
		if (rjd_hash64_valid(dict->hashes[i])) {
			int32_t k = rjd_dict_findindex(hashes, dict->hashes[i], RJD_DICT_FINDMODE_INSERTION);
			RJD_ASSERT(k >= 0);
			hashes[k] = dict->hashes[i];
			values[k] = dict->values[i];
		}
	}

	rjd_array_free(dict->hashes);
	rjd_array_free(dict->values);

	dict->hashes = hashes;
	dict->values = values;
}

static int32_t rjd_dict_findindex(const struct rjd_hash64* hashes, struct rjd_hash64 hash, enum rjd_dict_findmode mode)
{
	if (!hashes) {
		return -1;
	}

	const uint32_t capacity = rjd_array_capacity(hashes);
	const uint32_t start = hash.value % capacity;
	uint32_t i = start;
	do {
		if (mode == RJD_DICT_FINDMODE_INSERTION && !rjd_hash64_valid(hashes[i])) {
			return (int32_t)i;
		} else if (mode == RJD_DICT_FINDMODE_EXISTING && hashes[i].value == hash.value) {
			return (int32_t)i;
		}

		i = (i + 1) % capacity;
	} while (i != start);

	return -1;
}

#endif

