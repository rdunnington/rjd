#pragma once

#define RJD_ARRAY 1

struct rjd_mem_allocator;

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
#define rjd_array_capacity(buf) 						((buf)?(const uint32_t)(*rjd_array_capacity_impl(buf)):0)
#define rjd_array_count(buf) 							((buf)?(const uint32_t)(*rjd_array_count_impl(buf)):0)
#define rjd_array_clear(buf)							(*rjd_array_count_impl(buf) = 0)
#define rjd_array_resize(buf, size) 					(buf = rjd_array_resize_impl((buf), size, sizeof(*(buf))))
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
#define rjd_array_map(in, out, pred)		rjd_array_resize((out), rjd_array_count(in)); for (size_t _i = 0; _i < rjd_array_count(in); ++_i) { out[_i] = pred(in[_i]); }
#define rjd_array_reduce(buf, acc, pred)	for(size_t _i = 0; _i < rjd_array_count(buf); ++_i) { (acc) = pred(acc, ((buf)[_i])); }
#define rjd_array_sum(buf, acc)				for(size_t _i = 0; _i < rjd_array_count(buf); ++_i) { (acc) = rjd_array_sum_pred((acc), ((buf)[_i])); }
#define rjd_array_reverse(buf)				rjd_array_reverse_impl(buf, sizeof(*buf))

#define rjd_array_sample(buf, rng)		((buf)[rjd_rng_range32(rng, 0, rjd_array_count(buf))])
#define rjd_array_shuffle(buf, rng)		rjd_array_shuffle_impl(buf, rng, sizeof(*buf))

#if RJD_ENABLE_SHORTNAMES
	#define countof					rjd_countof

	#define array_alloc    			rjd_array_alloc
	#define array_free    			rjd_array_free
	#define array_capacity 			rjd_array_capacity
	#define array_count    			rjd_array_count
	#define array_clear				rjd_array_clear
	#define array_resize   			rjd_array_resize
	#define array_erase    			rjd_array_erase
	#define array_erase_unordered	rjd_array_erase_unordered
	#define array_empty    			rjd_array_empty
	#define array_full     			rjd_array_full
	#define array_push     			rjd_array_push
	#define array_pop      			rjd_array_pop

	#define array_first				rjd_array_first
	#define array_last				rjd_array_last
	#define array_contains			rjd_array_contains
	#define array_filter			rjd_array_filter
	#define array_map				rjd_array_map
	#define	array_reduce			rjd_array_reduce
	#define array_sum				rjd_array_sum
	#define array_reverse			rjd_array_reverse

	#define array_sample			rjd_array_sample
	#define array_shuffle			rjd_array_shuffle
#endif

struct rjd_rng;

void* rjd_array_alloc_impl(uint32_t capacity, struct rjd_mem_allocator* allocator, size_t sizeof_type);
void rjd_array_free_impl(const void* array);
uint32_t* rjd_array_capacity_impl(const void* array);
uint32_t* rjd_array_count_impl(const void* array);
void* rjd_array_resize_impl(void* array, uint32_t newsize, size_t sizeof_type);
void rjd_array_erase_impl(void* array, uint32_t index, size_t sizeof_type);
void rjd_array_erase_unordered_impl(void* array, uint32_t index, size_t sizeof_type);
void* rjd_array_grow_impl(void* array, size_t sizeof_type);
bool rjd_array_contains_impl(void* array, void* value, size_t sizeof_type, size_t sizeof_value);
void rjd_array_shuffle_impl(void* array, struct rjd_rng* rng, size_t sizeof_type);
void rjd_array_reverse_impl(void* array, size_t sizeof_type);

#if RJD_IMPL

struct rjd_array_header
{
	struct rjd_mem_allocator* allocator;
	uint32_t capacity;
	uint32_t count;
	uint32_t debug_sentinel;
};

static struct rjd_array_header* rjd_array_getheader(void* array);
static struct rjd_mem_allocator* rjd_array_allocator(void* array);
static inline void rjd_array_validate(const void* array);

void* rjd_array_alloc_impl(uint32_t capacity, struct rjd_mem_allocator* allocator, size_t sizeof_type)
{
	RJD_ASSERT(capacity > 0);
	RJD_ASSERT(allocator);
	RJD_ASSERT(sizeof_type > 0);

	size_t rawsize = sizeof(struct rjd_array_header) + (sizeof_type * capacity);
	char* raw = rjd_mem_alloc_array(char, rawsize, allocator);

	struct rjd_array_header* header = (struct rjd_array_header*)raw;
	header->allocator = allocator;
	header->capacity = capacity;
	header->count = 0;
	header->debug_sentinel = RJD_MEM_DEBUG_SENTINEL32; 

	char* buf = raw + sizeof(struct rjd_array_header);
	memset(buf, 0, sizeof_type * capacity);

	return buf;
}

void rjd_array_free_impl(const void* array)
{
	if (!array) {
		return;
	}
	rjd_array_validate(array);

	char* raw = (char*)array;
	rjd_mem_free(raw - sizeof(struct rjd_array_header));
}

uint32_t* rjd_array_capacity_impl(const void* array)
{
	if (!array) {
		return 0;
	}
	rjd_array_validate(array);

	struct rjd_array_header* header = rjd_array_getheader((void*)array);
	return &header->capacity;
}

uint32_t* rjd_array_count_impl(const void* array)
{
	if (!array) {
		return 0;
	}
	rjd_array_validate(array);

	struct rjd_array_header* header = rjd_array_getheader((void*)array);
	return &header->count;
}

void* rjd_array_resize_impl(void* array, uint32_t newsize, size_t sizeof_type)
{
	RJD_ASSERT(array);
	RJD_ASSERT(sizeof_type > 0);

	rjd_array_validate(array);

	uint32_t newcapacity = newsize > 0 ? newsize : 1;

	uint32_t* count = rjd_array_count_impl(array);
	uint32_t* capacity = rjd_array_capacity_impl(array);

	if (*capacity < newcapacity) {
		struct rjd_mem_allocator* allocator = rjd_array_allocator(array);
		void* newbuf = rjd_array_alloc_impl(newcapacity, allocator, sizeof_type);

		uint32_t oldcount = *count;
		memcpy(newbuf, array, oldcount * sizeof_type);
		count = rjd_array_count_impl(newbuf);
		
		uint32_t diff = newcapacity - *capacity;
		memset(newbuf, 0, diff * sizeof_type);

		rjd_array_free(array);
		array = newbuf;
	}

	*count = newsize;
	return (void*)array;
}

void rjd_array_erase_impl(void* array, uint32_t index, size_t sizeof_type)
{
	RJD_ASSERT(array);
	RJD_ASSERT(index < rjd_array_count(array));
	RJD_ASSERT(sizeof_type > 0);

	rjd_array_validate(array);

	char* raw = array;
	size_t toshift = rjd_array_count(array) - index - 1;
	if (toshift > 0) {
		memmove(raw + index * sizeof_type, raw + (index + 1) * sizeof_type, toshift * sizeof_type);
	}
	--*rjd_array_count_impl(array);
}

void rjd_array_erase_unordered_impl(void* array, uint32_t index, size_t sizeof_type)
{
	RJD_ASSERT(array);
	RJD_ASSERT(index < rjd_array_count(array));
	RJD_ASSERT(sizeof_type > 0);

	rjd_array_validate(array);

	char* raw = array;

	uint32_t* count = rjd_array_count_impl(array);

	if (*count > 1) {
		char* erase = raw + index * sizeof_type;
		char* swap = raw + (*count - 1) * sizeof_type;
		memcpy(erase, swap, sizeof_type);
	}

	if (*count > 0) {
		--*rjd_array_count_impl(array);
	}
}

void* rjd_array_grow_impl(void* array, size_t sizeof_type) {
	RJD_ASSERT(array);
	RJD_ASSERT(sizeof_type > 0);

	rjd_array_validate(array);

	uint32_t capacity = rjd_array_capacity(array);
	if (capacity == rjd_array_count(array)) {
		array = rjd_array_resize_impl(array, capacity * 2, sizeof_type);
	}

	*rjd_array_count_impl(array) += 1;
	return array;
}

bool rjd_array_contains_impl(void* array, void* value, size_t sizeof_type, size_t sizeof_value)
{
	RJD_ASSERT(sizeof_type == sizeof_value);

	rjd_array_validate(array);

	if (!array) {
		return false;
	}

	char* raw = (char*)array;
	for (uint32_t i = 0; i < rjd_array_count(array); ++i) {
		if (!memcmp(raw + i * sizeof_type, value, sizeof_type)) {
			return true;
		}
	}
	return false;
}

void rjd_array_shuffle_impl(void* array, struct rjd_rng* rng, size_t sizeof_type)
{
	if (!array) {
		return;
	}

	rjd_array_validate(array);

	char tmp[512];
	RJD_ASSERTMSG(sizeof_type <= sizeof(tmp), 
		"tmp (%u bytes) must be greater than or equal to sizeof_type (%u bytes)", 
		(unsigned) sizeof(tmp), (unsigned) sizeof_type);

	char* raw = (char*)array;
	for (uint32_t i = 0; i < rjd_array_count(array); ++i) {
		uint32_t k = rjd_rng_range32(rng, 0, rjd_array_count(array));
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

void rjd_array_reverse_impl(void* array, size_t sizeof_type)
{
	if (!array) {
		return;
	}
	rjd_array_validate(array);

	uint8_t* raw = array;
	for (uint8_t* begin = raw, *end = raw + (int32_t)sizeof_type * ((int32_t)rjd_array_count(array) - 1); 
		begin < end; 
		begin += sizeof_type, end -= sizeof_type)
	{
		rjd_mem_swap(begin, end, sizeof_type);
	}
}

static struct rjd_array_header* rjd_array_getheader(void* array)
{
	if (!array) {
		return NULL;
	}
	rjd_array_validate(array);

	char* raw = array;
	char* raw_header = raw - sizeof(struct rjd_array_header);
	return (struct rjd_array_header*) raw_header;
}

static struct rjd_mem_allocator* rjd_array_allocator(void* array)
{
	RJD_ASSERT(array);
	return rjd_array_getheader(array)->allocator;
}

static inline void rjd_array_validate(const void* array)
{
	RJD_ASSERT(array);
	const char* raw = array;
	const struct rjd_array_header* header = (struct rjd_array_header*)(raw - sizeof(struct rjd_array_header));
	RJD_ASSERTMSG(header->debug_sentinel == RJD_MEM_DEBUG_SENTINEL32, 
		"Debug sentinel was either corrupted by an underrun or this is not an rjd_array.");
	RJD_UNUSED_PARAM(header);
}

#endif //RJD_IMPL

