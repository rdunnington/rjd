#pragma once

#define RJD_STRHASH_H 1

struct rjd_strhash
{
	struct rjd_hash64 hash;
	struct rjd_strref* debug_string;
};

void rjd_strhash_global_init(struct rjd_mem_allocator* debug_allocator, uint32_t initial_capacity);
void rjd_strhash_global_destroy();

struct rjd_strhash rjd_strhash_init(const char* str);
bool rjd_strhash_isequal(struct rjd_strhash a, struct rjd_strhash b);
int rjd_strhash_compare(const struct rjd_strhash* a, const struct rjd_strhash* b);

#define RJD_STRHASH(string) (rjd_strhash_init(string))

#if RJD_IMPL

struct rjd_strpool* g_strhash_strpool;

void rjd_strhash_global_init(struct rjd_mem_allocator* debug_allocator, uint32_t initial_capacity)
{
	if (initial_capacity == 0) {
		initial_capacity = 128;
	}

	g_strhash_strpool = rjd_mem_alloc(struct rjd_strpool, debug_allocator);
	*g_strhash_strpool = rjd_strpool_init(debug_allocator, initial_capacity);
}

void rjd_strhash_global_destroy()
{
	rjd_strpool_free(g_strhash_strpool);
	rjd_mem_free(g_strhash_strpool);
	g_strhash_strpool = NULL;
}

struct rjd_strhash rjd_strhash_init(const char* str)
{
	struct rjd_hash64 hash = {0};
	struct rjd_strref* debug_string = NULL;

	if (str != NULL) {
		hash = rjd_hash64_str(str);
		if (g_strhash_strpool)
		{
			// TODO make threadsafe
			debug_string = rjd_strpool_add(g_strhash_strpool, str);
		}
	}

	struct rjd_strhash strhash = {
		.debug_string = debug_string,
		.hash = hash,
	};
	return strhash;
}

bool rjd_strhash_isequal(struct rjd_strhash a, struct rjd_strhash b)
{
	return a.hash.value == b.hash.value;
}

int rjd_strhash_compare(const struct rjd_strhash* a, const struct rjd_strhash* b)
{
	return a->hash.value < b->hash.value;
}

#endif // RJD_IMPL

