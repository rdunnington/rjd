#pragma once

#define RJD_DICT 1

struct rjd_alloc_context;

struct rjd_dict
{
	uint32_t count;
	const char** keys;
	void** values;
	struct rjd_alloc_context* allocator;
};

struct rjd_dict rjd_dict_init(struct rjd_alloc_context* allocator);
void rjd_dict_insert(struct rjd_dict* dict, const char* key, void* item);
void* rjd_dict_erase(struct rjd_dict* dict, const char* key);
void* rjd_dict_get(const struct rjd_dict* dict, const char* key);
void rjd_dict_free(struct rjd_dict* dict);

#if RJD_ENABLE_SHORTNAMES
	#define dict_init	rjd_dict_init
	#define dict_insert	rjd_dict_insert
	#define dict_erase	rjd_dict_erase
	#define dict_get	rjd_dict_get
	#define dict_free	rjd_dict_free
#endif

#if RJD_IMPL

enum rjd_dict_findmode
{
	RJD_DICT_FINDMODE_INSERTION,
	RJD_DICT_FINDMODE_EXISTING,
};

static const char* rjd_dict_copystr(const char* s, struct rjd_alloc_context* allocator);
static uint32_t rjd_dict_hash_string(const char* key);
static int32_t rjd_dict_findindex(const char** keys, const char* key, enum rjd_dict_findmode mode);
 
struct rjd_dict rjd_dict_init(struct rjd_alloc_context* allocator)
{
	RJD_ASSERT(allocator);

	struct rjd_dict dict = { 0, NULL, NULL, allocator };
	return dict;
}

void rjd_dict_insert(struct rjd_dict* dict, const char* key, void* value)
{
	RJD_ASSERT(dict);
	RJD_ASSERT(key);

	const float load = dict->keys ? (dict->count + 1) / (const float)rjd_array_capacity(dict->keys) : 1;
	if (load > 0.6) {
		uint32_t capacity = dict->keys ? rjd_array_capacity(dict->keys) * 2 : 32;

		const char** keys = rjd_array_alloc(const char*, capacity, dict->allocator);
		void** values = rjd_array_alloc(void*, capacity, dict->allocator);

		rjd_array_resize(keys, capacity);
		rjd_array_resize(values, capacity);

		for (uint32_t i = 0; i < rjd_array_count(dict->keys); ++i) {
			if (dict->keys[i]) {
				int32_t k = rjd_dict_findindex(keys, dict->keys[i], RJD_DICT_FINDMODE_INSERTION);
				RJD_ASSERT(k >= 0);
				keys[k] = dict->keys[i];
				values[k] = dict->values[i];
			}
		}

		rjd_array_free(dict->keys);
		rjd_array_free(dict->values);

		dict->keys = keys;
		dict->values = values;
	}

	int32_t i = rjd_dict_findindex(dict->keys, key, RJD_DICT_FINDMODE_INSERTION);
	RJD_ASSERT(i >= 0);
	RJD_ASSERT(!dict->keys[i]);

	dict->keys[i] = rjd_dict_copystr(key, dict->allocator);
	dict->values[i] = value;
	++dict->count;
}

void* rjd_dict_erase(struct rjd_dict* dict, const char* key)
{
	RJD_ASSERT(dict);
	RJD_ASSERT(key);
	
	int32_t index = rjd_dict_findindex(dict->keys, key, RJD_DICT_FINDMODE_EXISTING);
	if (index < 0) {
		return NULL;
	}

	const char* k = dict->keys[index];
	void* v = dict->values[index];

	dict->keys[index] = NULL;
	dict->values[index] = NULL;
	--dict->count;

	rjd_free(k, dict->allocator);
	return v;
}

void* rjd_dict_get(const struct rjd_dict* dict, const char* key)
{
	RJD_ASSERT(dict);
	RJD_ASSERT(key);

	int32_t index = rjd_dict_findindex(dict->keys, key, RJD_DICT_FINDMODE_EXISTING);
	if (index < 0) {
		return NULL;
	}

	return dict->values[index];
}

void rjd_dict_free(struct rjd_dict* dict)
{
	rjd_array_free(dict->keys);
	rjd_array_free(dict->values);
	dict->keys = NULL;
	dict->values = NULL;
	dict->count = 0;
}

static const char* rjd_dict_copystr(const char* s, struct rjd_alloc_context* allocator)
{
	uint32_t length = strlen(s) + 1;
	char* copy = rmalloc_array(char, length, allocator);
	strncpy(copy, s, length);
	return copy;
}

static uint32_t rjd_dict_hash_string(const char* key)
{
	RJD_ASSERT(key);

	if (*key == '\0') {
		return 0;
	}

	const uint32_t PRIME = 0x01000193; //   16777619
	const uint32_t SEED  = 0x811C9DC5; // 2166136261

	uint32_t hash = SEED;
	const unsigned char* ptr = (const unsigned char*)key;
	while (*ptr)
	{
		hash = (*ptr++ ^ hash) * PRIME;
	}
	return hash;
}

static int32_t rjd_dict_findindex(const char** keys, const char* key, enum rjd_dict_findmode mode)
{
	if (!keys || !key) {
		return -1;
	}

	const uint32_t hash = rjd_dict_hash_string(key);
	const uint32_t capacity = rjd_array_capacity(keys);
	const uint32_t start = hash % capacity;
	uint32_t i = start;
	do {
		if (mode == RJD_DICT_FINDMODE_INSERTION && !keys[i]) {
			return (int32_t)i;
		} else if (mode == RJD_DICT_FINDMODE_EXISTING && keys[i] && !strcmp(keys[i], key)) {
			return (int32_t)i;
		}

		i = (i + 1) % capacity;
	} while (i != start);

	return -1;
}

#endif

