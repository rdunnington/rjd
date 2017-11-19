#pragma once

#define RJD_ARRAY 1

// DEPENDENCIES
// rjd_alloc.h
// rjd_debug.h
// string.h

struct rjd_alloc_context;

#if RJD_COMPILER_MSVC
	#define rjd_countof(buf) _countof(buf)
#elif RJD_COMPILER_CLANG || RJD_COMPILER_GCC
	#define RJD_SAME_TYPE(a, b) (__builtin_types_compatible_p(__typeof__(a), __typeof__(b)))
	#define RJD_STATIC_ZERO(a) (sizeof(int[(a)?1:-1]) * 0)
	#define RJD_MUST_BE_ARRAY(a) (RJD_STATIC_ZERO(!RJD_SAME_TYPE((a), &(*a))))

	#define rjd_countof(buf) (sizeof(buf) / sizeof(*(buf)) + RJD_MUST_BE_ARRAY(buf))
#else
	#define rjd_countof(buf) ( (sizeof(buf) / sizeof(*(buf))) / ((size_t)(!sizeof(buf) % sizeof(0[buf]))) )
#endif

#define rjd_array_alloc(type, capacity, alloc_context)	((type*)(rjd_array_alloc_impl((capacity), (alloc_context), sizeof(type))))
#define rjd_array_free(buf)								rjd_array_free_impl(buf)
#define rjd_array_capacity(buf) 						((const uint32_t)(*rjd_array_capacity_impl(buf)))
#define rjd_array_count(buf) 							((const uint32_t)(*rjd_array_count_impl(buf)))
#define rjd_array_clear(buf)							(*rjd_array_count_impl(buf) = 0)
#define rjd_array_resize(buf, size) 					buf = rjd_array_resize_impl((buf), size, sizeof(*(buf)))
#define rjd_array_erase(buf, index) 					rjd_array_erase_impl((buf), index, sizeof(*(buf)))
#define rjd_array_erase_unordered(buf, index) 			rjd_array_erase_unordered_impl((buf), index, sizeof(*(buf)))
#define rjd_array_empty(buf) 							(rjd_array_count(buf) == 0)
#define rjd_array_full(buf) 							(rjd_array_count(buf) == rjd_array_capacity(buf))
#define rjd_array_push(buf, value) 						((buf) = rjd_array_grow_impl((buf), sizeof(*buf)), (buf)[rjd_array_count(buf) - 1] = value)
#define rjd_array_pop(buf)		 						(--*rjd_array_count_impl(buf), *(buf + rjd_array_count(buf)))

#define rjd_array_sum_pred(acc, element) (acc + element)

#define rjd_array_first(buf, _default)		(((buf) && rjd_array_count(buf) > 0) ? ((buf)[0]) : (_default))
#define rjd_array_last(buf, _default)		(((buf) && rjd_array_count(buf) > 0) ? ((buf)[rjd_array_count(buf) - 1]) : (_default))
#define rjd_array_contains(buf, value)		rjd_array_contains_impl((buf), &(value), sizeof(*buf), sizeof(value))
#define rjd_array_filter(buf, pred)			for(int _i = (int)rjd_array_count(buf) - 1; _i >= 0; --_i) { if (!(pred((buf)[_i]))) { rjd_array_erase((buf), _i); } }
#define rjd_array_map(in, out, pred)		rjd_array_resize((out), rjd_array_count(in)), for (size_t _i = 0; _i < rjd_array_count(in); ++_i) { out[_i] = pred(in[_i]); }
#define rjd_array_reduce(buf, acc, pred)	for(size_t _i = 0; _i < rjd_array_count(buf); ++_i) { (acc) = pred(acc, ((buf)[_i])); }
#define rjd_array_sum(buf, acc)				for(size_t _i = 0; _i < rjd_array_count(buf); ++_i) { (acc) = rjd_array_sum_pred((acc), ((buf)[_i])); }

#if RJD_RNG
	#define rjd_array_sample(buf, rng)		((buf)[rjd_rng_range32(rng, 0, rjd_array_count(buf))])
	#define rjd_array_shuffle(buf, rng)		rjd_array_shuffle_impl(buf, rng, sizeof(*buf))
#endif

#if RJD_ENABLE_SHORTNAMES
	#define countof					rjd_countof

	#define arr_alloc    			rjd_array_alloc
	#define arr_free    			rjd_array_free
	#define arr_capacity 			rjd_array_capacity
	#define arr_count    			rjd_array_count
	#define arr_clear				rjd_array_clear
	#define arr_resize   			rjd_array_resize
	#define arr_erase    			rjd_array_erase
	#define arr_erase_unordered		rjd_array_erase_unordered
	#define arr_empty    			rjd_array_empty
	#define arr_full     			rjd_array_full
	#define arr_push     			rjd_array_push
	#define arr_pop      			rjd_array_pop

	#define arr_first				rjd_array_first
	#define arr_last				rjd_array_last
	#define arr_contains			rjd_array_contains
	#define arr_filter				rjd_array_filter
	#define arr_map					rjd_array_map
	#define	arr_reduce				rjd_array_reduce
	#define arr_sum					rjd_array_sum

	#define arr_sample				rjd_array_sample
	#define arr_shuffle				rjd_array_shuffle
#endif

struct rjd_rng;

void* rjd_array_alloc_impl(uint32_t capacity, struct rjd_alloc_context* context, size_t sizeof_type);
void rjd_array_free_impl(void* buffer);
uint32_t* rjd_array_capacity_impl(void* buffer);
uint32_t* rjd_array_count_impl(void* buffer);
void* rjd_array_resize_impl(void* buffer, uint32_t newsize, size_t sizeof_type);
void rjd_array_erase_impl(void* buffer, uint32_t index, size_t sizeof_type);
void rjd_array_erase_unordered_impl(void* buffer, uint32_t index, size_t sizeof_type);
void* rjd_array_grow_impl(void* buffer, size_t sizeof_type);
bool rjd_array_contains_impl(void* buffer, void* value, size_t sizeof_type, size_t sizeof_value);
void rjd_array_shuffle_impl(void* buffer, struct rjd_rng* rng, size_t sizeof_type);

#if RJD_IMPL

void* rjd_array_alloc_impl(uint32_t capacity, struct rjd_alloc_context* context, size_t sizeof_type)
{
	RJD_ASSERT(capacity > 0);
	RJD_ASSERT(context);
	RJD_ASSERT(sizeof_type > 0);

	size_t rawsize = sizeof(struct rjd_alloc_context*) + (sizeof(uint32_t) * 2) + (sizeof_type * capacity);
	char* raw = (char*)rjd_malloc_impl(rawsize, context);

	struct rjd_alloc_context** contextCache = (struct rjd_alloc_context**)raw;
	*contextCache = context;
	uint32_t* pcapacity = (uint32_t*)(raw + sizeof(struct rjd_alloc_context*));
	uint32_t* pcount = (uint32_t*)(raw + sizeof(struct rjd_alloc_context*) + sizeof(uint32_t));
	char* buf = (char*)(raw + sizeof(struct rjd_alloc_context*) + sizeof(uint32_t) + sizeof(uint32_t));

	memset(buf, 0, sizeof_type * capacity);

	*pcapacity = capacity;
	*pcount = 0;

	return buf;
}

static struct rjd_alloc_context* rjd_array_getcontext_impl(void* buffer)
{
	RJD_ASSERT(buffer);

	char* raw = buffer;
	size_t headersize = sizeof(struct rjd_alloc_context*) + sizeof(uint32_t) + sizeof(uint32_t);
	struct rjd_alloc_context** context = (struct rjd_alloc_context**)(raw - headersize);
	return *context;
}

void rjd_array_free_impl(void* buffer)
{
	if (!buffer) {
		return;
	}

	struct rjd_alloc_context* context = rjd_array_getcontext_impl(buffer);

	char* raw = buffer;
	size_t headersize = sizeof(struct rjd_alloc_context*) + sizeof(uint32_t) + sizeof(uint32_t);

	rjd_free(raw - headersize, context);
}

uint32_t* rjd_array_capacity_impl(void* buffer)
{
	RJD_ASSERT(buffer);

	char* raw = buffer;
	return (uint32_t*)(raw - sizeof(uint32_t) - sizeof(uint32_t));
}

uint32_t* rjd_array_count_impl(void* buffer)
{
	RJD_ASSERT(buffer);

	char* raw = buffer;
	return (uint32_t*)(raw - sizeof(uint32_t));
}

void* rjd_array_resize_impl(void* buffer, uint32_t newsize, size_t sizeof_type)
{
	RJD_ASSERT(buffer);
	RJD_ASSERT(sizeof_type > 0);

	uint32_t newcapacity = newsize > 0 ? newsize : 1;

	uint32_t* count = rjd_array_count_impl(buffer);
	uint32_t* capacity = rjd_array_capacity_impl(buffer);

	if (*capacity < newcapacity) {
		struct rjd_alloc_context* context = rjd_array_getcontext_impl(buffer);
		void* newbuf = rjd_array_alloc_impl(newcapacity, context, sizeof_type);

		uint32_t oldcount = *count;
		memcpy(newbuf, buffer, oldcount * sizeof_type);
		count = rjd_array_count_impl(newbuf);

		rjd_array_free(buffer);
		buffer = newbuf;
	}

	*count = newsize;
	return buffer;
}

void rjd_array_erase_impl(void* buffer, uint32_t index, size_t sizeof_type)
{
	RJD_ASSERT(buffer);
	RJD_ASSERT(index < rjd_array_count(buffer));
	RJD_ASSERT(sizeof_type > 0);

	char* raw = buffer;
	size_t toshift = rjd_array_count(buffer) - index - 1;
	if (toshift > 0) {
		memmove(raw + index * sizeof_type, raw + (index + 1) * sizeof_type, toshift * sizeof_type);
	}
	--*rjd_array_count_impl(buffer);
}

void rjd_array_erase_unordered_impl(void* buffer, uint32_t index, size_t sizeof_type)
{
	RJD_ASSERT(buffer);
	RJD_ASSERT(index < rjd_array_count(buffer));
	RJD_ASSERT(sizeof_type > 0);

	char* raw = buffer;

	uint32_t* count = rjd_array_count_impl(buffer);

	if (*count > 1) {
		char* erase = raw + index * sizeof_type;
		char* swap = raw + (*count - 1) * sizeof_type;
		memcpy(erase, swap, sizeof_type);
	}

	if (*count > 0) {
		--*rjd_array_count_impl(buffer);
	}
}

void* rjd_array_grow_impl(void* buffer, size_t sizeof_type) {
	RJD_ASSERT(buffer);
	RJD_ASSERT(sizeof_type > 0);

	uint32_t capacity = rjd_array_capacity(buffer);
	if (capacity == rjd_array_count(buffer)) {
		buffer = rjd_array_resize_impl(buffer, capacity * 2, sizeof_type);
	}

	*rjd_array_count_impl(buffer) += 1;
	return buffer;
}

bool rjd_array_contains_impl(void* buffer, void* value, size_t sizeof_type, size_t sizeof_value)
{
	RJD_ASSERT(sizeof_type == sizeof_value);

	char* raw = (char*)buffer;
	for (uint32_t i = 0; i < rjd_array_count(buffer); ++i) {
		if (!memcmp(raw + i * sizeof_type, value, sizeof_type)) {
			return true;
		}
	}
	return false;
}

#if RJD_RNG
	void rjd_array_shuffle_impl(void* buffer, struct rjd_rng* rng, size_t sizeof_type)
	{
		char tmp[512];
		RJD_ASSERTMSG(sizeof_type <= sizeof(tmp), 
			"tmp (%u bytes) must be greater than or equal to sizeof_type (%u bytes)", 
			(unsigned) sizeof(tmp), (unsigned) sizeof_type);

		char* raw = (char*)buffer;
		for (uint32_t i = 0; i < rjd_array_count(buffer); ++i) {
			uint32_t k = rjd_rng_range32(rng, 0, rjd_array_count(buffer));
			if (i == k) {
				continue;
			}

			char* a = raw + (i * sizeof_type);
			char* b = raw + (k * sizeof_type);

			memcpy(tmp, a, sizeof_type);
			memcpy(a, b, sizeof_type);
			memcpy(b, tmp, sizeof_type);
		}
	}
#endif // RJD_RNG

#endif //RJD_IMPL

