#pragma once

#define RJD_STRPOOL_H 1

struct rjd_strpool
{
	struct rjd_dict storage;
};

struct rjd_strref;

struct rjd_strpool rjd_strpool_init(struct rjd_mem_allocator* allocator, size_t initial_capacity);
void rjd_strpool_free(struct rjd_strpool* pool);
struct rjd_strref* rjd_strpool_add(struct rjd_strpool* pool, const char* fmt, ...);
struct rjd_strref* rjd_strpool_addv(struct rjd_strpool* pool, const char* fmt, va_list args); 
struct rjd_strref* rjd_strpool_addl(struct rjd_strpool* pool, const char* str, size_t length);
void rjd_strref_release(struct rjd_strref* ref);
const char* rjd_strref_str(const struct rjd_strref* ref);
uint32_t rjd_strref_length(const struct rjd_strref* ref);

#if RJD_IMPL

struct rjd_strref
{
	const char* str;
	struct rjd_strpool* owner;
	int32_t refcount;
	uint32_t length;
};

static struct rjd_strref* rjd_strpool_addimpl(struct rjd_strpool* pool, const char* str);

struct rjd_strpool rjd_strpool_init(struct rjd_mem_allocator* allocator, size_t initial_capacity)
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
			rjd_mem_free(ref); // struct and string are part of the same allocation block
		}
	}
	rjd_dict_free(&pool->storage);
}

struct rjd_strref* rjd_strpool_add(struct rjd_strpool* pool, const char* format, ...)
{
	RJD_ASSERT(pool);
	RJD_ASSERT(format);

	struct rjd_strref* ref = NULL;

	va_list args;
	va_start(args, format);
		ref = rjd_strpool_addv(pool, format, args);
	va_end(args);

	return ref;
}

struct rjd_strref* rjd_strpool_addv(struct rjd_strpool* pool, const char* format, va_list args)
{
	RJD_ASSERT(pool);
	RJD_ASSERT(format);

	struct rjd_strref* ref = NULL;

	RJD_STRBUF_SCOPED(buffer, pool->storage.allocator, {
		rjd_strbuf_appendv(&buffer, format, args);
		ref = rjd_strpool_addimpl(pool, rjd_strbuf_str(&buffer));
	});

	return ref;
}

struct rjd_strref* rjd_strpool_addl(struct rjd_strpool* pool, const char* format, size_t length)
{
	RJD_ASSERT(pool);
	RJD_ASSERT(format);

	struct rjd_strref* ref = NULL;

	RJD_STRBUF_SCOPED(buffer, pool->storage.allocator, {
		rjd_strbuf_appendl(&buffer, format, (uint32_t)length);
		ref = rjd_strpool_addimpl(pool, rjd_strbuf_str(&buffer));
	});

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
		rjd_mem_free(ref); // struct and string are part of the same allocation block
		rjd_dict_erase(&pool->storage, hash);
	}
}

const char* rjd_strref_str(const struct rjd_strref* ref)
{
	RJD_ASSERT(ref);
	return ref->str;
}

uint32_t rjd_strref_length(const struct rjd_strref* ref)
{
	RJD_ASSERT(ref);
	return ref->length;
}

static struct rjd_strref* rjd_strpool_addimpl(struct rjd_strpool* pool, const char* str) 
{
	RJD_ASSERT(pool);
	RJD_ASSERT(str);

	rjd_hash64 hash = rjd_hash64_data((const uint8_t*)str, -1);
	struct rjd_strref* ref = rjd_dict_get(&pool->storage, hash);
	if (!ref) {
		uint8_t* mem = rjd_mem_alloc_array(uint8_t, sizeof(struct rjd_strref) + strlen(str) + 1, pool->storage.allocator);
		ref = (struct rjd_strref*)mem;

		char* copied_str = (char*)(mem + sizeof(struct rjd_strref));
		strcpy(copied_str, str);

		ref->str = copied_str;
		ref->owner = pool;
		ref->refcount = 0;
		ref->length = (uint32_t)strlen(ref->str);

		rjd_dict_insert(&pool->storage, hash, ref);
	}
	++ref->refcount;

	return ref;
}

#endif // RJD_IMPL

