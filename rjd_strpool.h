#pragma once

struct rjd_strpool
{
	struct rjd_dict storage;
};

struct rjd_strref;

struct rjd_strpool rjd_strpool_init(struct rjd_alloc_context* allocator, size_t initial_capacity);
void rjd_strpool_free(struct rjd_strpool* pool);
struct rjd_strref* rjd_strpool_add(struct rjd_strpool* pool, const char* str);
void rjd_strref_release(struct rjd_strref* ref);
const char* rjd_strref_str(const struct rjd_strref* ref);

#if RJD_ENABLE_SHORTNAMES
	#define strpool			rjd_strpool
	#define strref			rjd_strref

	#define strpool_init	rjd_strpool_init
	#define strpool_free	rjd_strpool_free
	#define strpool_add		rjd_strpool_add
	#define strref_release	rjd_strref_release
	#define	strref_str		rjd_strref_str
#endif

#if RJD_IMPL

struct rjd_strref
{
	const char* str;
	int32_t refcount;
	struct rjd_strpool* owner;
};

struct rjd_strpool rjd_strpool_init(struct rjd_alloc_context* allocator, size_t initial_capacity)
{
	RJD_ASSERT(allocator);

	struct rjd_strpool pool = { rjd_dict_init(allocator, initial_capacity * 2) };
	return pool;
}

void rjd_strpool_free(struct rjd_strpool* pool)
{
	RJD_ASSERT(pool);

	void** refs = pool->storage.values;
	for (uint32_t i = 0; i < rjd_array_count(refs); ++i) {
		if (refs[i]) {
			struct rjd_strref* ref = refs[i];
			rjd_free(ref->str, pool->storage.allocator);
			rjd_free(ref, pool->storage.allocator);
		}
	}
	rjd_dict_free(&pool->storage);
}

struct rjd_strref* rjd_strpool_add(struct rjd_strpool* pool, const char* str)
{
	RJD_ASSERT(pool);
	RJD_ASSERT(str);

	rjd_hash64 hash = rjd_hash64_data((const uint8_t*)str, -1);
	struct rjd_strref* ref = rjd_dict_get(&pool->storage, hash);
	if (!ref) {
		char* copied_str = rjd_malloc_array(char, strlen(str) + 1, pool->storage.allocator);
		strcpy(copied_str, str);

		ref = rjd_malloc(struct rjd_strref, pool->storage.allocator);
		ref->str = copied_str;
		ref->refcount = 0;
		ref->owner = pool;

		rjd_dict_insert(&pool->storage, hash, ref);
	}
	++ref->refcount;
	return ref;
}

void rjd_strref_release(struct rjd_strref* ref)
{
	RJD_ASSERT(ref);

	struct rjd_strpool* pool = ref->owner;

	rjd_hash64 hash = rjd_hash64_data((const uint8_t*)ref->str, -1);
	RJD_ASSERTMSG(rjd_dict_get(&pool->storage, hash) == ref, "ref was not contained in string pool");

	--ref->refcount;
	if (ref->refcount <= 0) {
		rjd_free(ref->str, pool->storage.allocator);
		rjd_free(ref, pool->storage.allocator);
		rjd_dict_erase(&pool->storage, hash);
	}
}

const char* rjd_strref_str(const struct rjd_strref* ref)
{
	RJD_ASSERT(ref);
	return ref->str;
}

#endif // RJD_IMPL

