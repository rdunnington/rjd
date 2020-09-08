#pragma once

#define RJD_ARRAY_H 1

// static array count
// Note that GCC is awesome and has a warning if buf is a pointer. See -Wsizeof-pointer-div
#define rjd_countof(buf) (sizeof(buf) / sizeof(*(buf)))

// dyanmic array
#define rjd_array_alloc(type, capacity, allocator)	((type*)(rjd_array_alloc_impl((capacity), (allocator), sizeof(type))))
#define rjd_array_clone(buf, allocator)				rjd_array_clone_impl((buf), allocator, sizeof(*(buf)))
#define rjd_array_free(buf)							rjd_array_free_impl(buf)
#define rjd_array_capacity(buf) 					((buf)?(const uint32_t)(*rjd_array_capacity_impl(buf)):0)
#define rjd_array_count(buf) 						((buf)?(const uint32_t)(*rjd_array_count_impl(buf)):0)
#define rjd_array_clear(buf)						(*rjd_array_count_impl(buf) = 0)
#define rjd_array_reserve(buf, capacity)			(buf = rjd_array_reserve_impl((buf), capacity, sizeof(*(buf))))
#define rjd_array_resize(buf, size) 				(buf = rjd_array_resize_impl((buf), size, sizeof(*(buf))))
#define rjd_array_trim(buf)							(buf = rjd_array_trim_impl((buf), sizeof(*(buf))))
#define rjd_array_erase(buf, index) 				rjd_array_erase_impl((buf), index, sizeof(*(buf)))
#define rjd_array_erase_unordered(buf, index) 		rjd_array_erase_unordered_impl((buf), index, sizeof(*(buf)))
#define rjd_array_empty(buf) 						(rjd_array_count(buf) == 0)
#define rjd_array_full(buf) 						(rjd_array_count(buf) == rjd_array_capacity(buf))
#define rjd_array_push(buf, value) 					(buf = rjd_array_push_impl((buf), \
													sizeof(*(buf))), (buf)[rjd_array_count(buf) - 1] = value)
#define rjd_array_pop(buf)		 					(rjd_array_pop_impl(buf), 		\
													--*rjd_array_count_impl(buf), 	\
													*(buf + rjd_array_count(buf)))
#define rjd_array_insert(buf, ptr, index)			(buf = rjd_array_insert_impl((buf), (ptr), sizeof(*(buf)), index))
#define rjd_array_get(buf, index)					(rjd_array_get_validate((buf), (index)), (buf + index))
#define rjd_array_first(buf)						(rjd_array_get_validate((buf), 0), (buf)[0])
#define rjd_array_last(buf)							(rjd_array_get_validate((buf), 0), (buf)[rjd_array_count(buf) - 1])

// searching/sorting
typedef int32_t RJD_COMPILER_MSVC_ONLY(__cdecl) rjd_array_compare_func(const void* left, const void* right);
typedef int32_t RJD_COMPILER_MSVC_ONLY(__cdecl) rjd_array_compare_c_func(void* context, const void* left, const void* right);

enum { RJD_ARRAY_NOT_FOUND = -1 };

#define rjd_array_find(buf, ptr)						rjd_array_find_impl((buf), (ptr), sizeof(*(buf)), RJD_MUST_BE_SAME_TYPE_TEST((buf), (ptr)))
#define rjd_array_contains(buf, ptr)					rjd_array_contains_impl((buf), (ptr), sizeof(*(buf)), RJD_MUST_BE_SAME_TYPE_TEST((buf), (ptr)))

// These functions sort or deal with sorted data
#define rjd_array_sort(buf, comparer) \
		rjd_array_sort_impl((buf), sizeof(*(buf)), comparer)
#define rjd_array_lowerbound(buf, ptr, comparer) \
		rjd_array_lowerbound_impl((buf), ptr, sizeof(*(buf)), comparer, RJD_MUST_BE_SAME_TYPE_TEST((buf), (ptr)))
#define rjd_array_find_sorted(buf, ptr, comparer) \
		rjd_array_find_sorted_impl((buf), (ptr), sizeof(*(buf)), comparer, RJD_MUST_BE_SAME_TYPE_TEST((buf), (ptr)))
#define rjd_array_contains_sorted(buf, ptr, comparer) \
		rjd_array_contains_sorted_impl((buf), (ptr), sizeof(*(buf)), comparer, RJD_MUST_BE_SAME_TYPE_TEST((buf), (ptr)))

// These versions of the sort/find sorted functions allow you to provide a content parameter that 
// is passed into the compare func.
#define rjd_array_sort_c(buf, comparer, context) \
		rjd_array_sort_c_impl((buf), sizeof(*(buf)), comparer, context)
#define rjd_array_lowerbound_c(buf, ptr, comparer, context) \
		rjd_array_lowerbound_c_impl((buf), (ptr), sizeof(*(buf)), comparer, context, RJD_MUST_BE_SAME_TYPE_TEST((buf), (ptr)))
#define rjd_array_find_sorted_c(buf, ptr, comparer, context) \
		rjd_array_find_sorted_c_impl((buf), (ptr), sizeof(*(buf)), comparer, context, RJD_MUST_BE_SAME_TYPE_TEST((buf), (ptr)))
#define rjd_array_contains_sorted_c(buf, ptr, comparer, context) \
		rjd_array_contains_sorted_c_impl((buf), (ptr), sizeof(*(buf)), comparer, context, RJD_MUST_BE_SAME_TYPE_TEST((buf), (ptr)))

// predicate helpers for the functional-style interface
#define rjd_array_sum_predicate(acc, element) 		(acc + element)

// functional-style helpers
#define rjd_array_filter(buf, predicate, context)	for(int _i = (int)rjd_array_count(buf) - 1; _i >= 0; --_i) { 	 	\
															if (!(predicate((buf)[_i]))) { rjd_array_erase((buf), _i); } \
													}
#define rjd_array_map(in, out, predicate)			rjd_array_resize((out), rjd_array_count(in)); 					\
													for (size_t _i = 0; _i < rjd_array_count(in); ++_i) {			\
														out[_i] = predicate(in[_i]); 								\
													}
#define rjd_array_reduce(buf, acc, predicate)		for(size_t _i = 0; _i < rjd_array_count(buf); ++_i) {			\
														(acc) = predicate(acc, ((buf)[_i]));						\
													}
#define rjd_array_sum(buf, acc)						for(size_t _i = 0; _i < rjd_array_count(buf); ++_i) {			\
														(acc) = rjd_array_sum_predicate((acc), ((buf)[_i]));		\
													}
#define rjd_array_reverse(buf)						rjd_array_reverse_impl(buf, sizeof(*buf))

// randomness helpers
#define rjd_array_sample(buf, rng)					((buf)[rjd_rng_range32(rng, 0, rjd_array_count(buf))])
#define rjd_array_shuffle(buf, rng)					rjd_array_shuffle_impl(buf, rng, sizeof(*buf))

struct rjd_mem_allocator;
struct rjd_rng;

void* rjd_array_alloc_impl(uint32_t capacity, struct rjd_mem_allocator* allocator, size_t sizeof_type);
void* rjd_array_clone_impl(const void* array, struct rjd_mem_allocator* allocator, size_t sizeof_type);
void rjd_array_free_impl(const void* array);
uint32_t* rjd_array_capacity_impl(const void* array);
uint32_t* rjd_array_count_impl(const void* array);
void* rjd_array_reserve_impl(void* array, uint32_t newcapacity, size_t sizeof_type);
void* rjd_array_resize_impl(void* array, uint32_t newcount, size_t sizeof_type);
void* rjd_array_trim_impl(void* array, size_t sizeof_type);
void rjd_array_erase_impl(void* array, uint32_t index, size_t sizeof_type);
void rjd_array_erase_unordered_impl(void* array, uint32_t index, size_t sizeof_type);
void* rjd_array_push_impl(void* array, size_t sizeof_type);
void rjd_array_pop_impl(void* array);
void* rjd_array_insert_impl(void* array, const void* insert, size_t sizeof_type, uint32_t index);
void rjd_array_get_validate(void* array, uint32_t index);
int32_t rjd_array_find_impl(const void* array, const void* search, size_t sizeof_type, int unused);
bool rjd_array_contains_impl(const void* array, const void* search, size_t sizeof_type, int unused);
void rjd_array_sort_impl(void* array, size_t sizeof_type, rjd_array_compare_func* comparer);
int32_t rjd_array_lowerbound_impl(const void* array, const void* needle, size_t sizeof_type, rjd_array_compare_func* comparer, int unused);
int32_t rjd_array_find_sorted_impl(const void* array, const void* needle, size_t sizeof_type, rjd_array_compare_func* comparer, int unused);
bool rjd_array_contains_sorted_impl(const void* array, const void* needle, size_t sizeof_type, rjd_array_compare_func* comparer, int unused);
void rjd_array_sort_c_impl(void* array, size_t sizeof_type, rjd_array_compare_c_func* comparer, void* context);
int32_t rjd_array_lowerbound_c_impl(const void* array, const void* needle, size_t sizeof_type, rjd_array_compare_c_func* comparer, void* context, int unused);
int32_t rjd_array_find_sorted_c_impl(const void* array, const void* needle, size_t sizeof_type, rjd_array_compare_c_func* comparer, void* context, int unused);
bool rjd_array_contains_sorted_c_impl(const void* array, const void* needle, size_t sizeof_type, rjd_array_compare_c_func* comparer, void* context, int unused);
void rjd_array_shuffle_impl(void* array, struct rjd_rng* rng, size_t sizeof_type);
void rjd_array_reverse_impl(void* array, size_t sizeof_type);

#if RJD_IMPL

// Copied from stdlib.h. We want this extra platform functionality but don't want to turn on all the defines
// to get us there.
#if RJD_COMPILER_GCC
void __bsd_qsort_r (void *__base, size_t __nmemb, size_t __size, void *__thunk, int (*_compar)(void *, const void *, const void *));
#  define qsort_r __bsd_qsort_r
#endif

struct rjd_array_header
{
	struct rjd_mem_allocator* allocator;
	uint32_t capacity;
	uint32_t count;
	uint32_t debug_sentinel;
};

const uint32_t RJD_ARRAY_DEBUG_SENTINEL32 = 0xA7A7A7A7;

static struct rjd_array_header* rjd_array_getheader(void* array);
static struct rjd_mem_allocator* rjd_array_allocator(void* array);
static inline void rjd_array_validate(const void* array);
static void* rjd_array_grow(void* array, size_t sizeof_type);

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
	header->debug_sentinel = RJD_ARRAY_DEBUG_SENTINEL32; 

	char* buf = raw + sizeof(struct rjd_array_header);
	return buf;
}

void* rjd_array_clone_impl(const void* array, struct rjd_mem_allocator* allocator, size_t sizeof_type)
{
	uint32_t count = rjd_array_count(array);
	void* clone = rjd_array_alloc_impl(count, allocator, sizeof_type);
	clone = rjd_array_resize_impl(clone, count, sizeof_type);
	memcpy(clone, array, count * sizeof_type);
	return clone;
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
	RJD_ASSERT(array);

	rjd_array_validate(array);

	struct rjd_array_header* header = rjd_array_getheader((void*)array);
	return &header->capacity;
}

uint32_t* rjd_array_count_impl(const void* array)
{
	RJD_ASSERT(array);

	rjd_array_validate(array);

	struct rjd_array_header* header = rjd_array_getheader((void*)array);
	return &header->count;
}

void* rjd_array_reserve_impl(void* array, uint32_t newcapacity, size_t sizeof_type)
{
	RJD_ASSERT(array);
	RJD_ASSERT(sizeof_type > 0);

	rjd_array_validate(array);

	uint32_t oldcapacity = rjd_array_capacity(array);

	if (oldcapacity < newcapacity) {
		struct rjd_mem_allocator* allocator = rjd_array_allocator(array);
		char* newarray = rjd_array_alloc_impl(newcapacity, allocator, sizeof_type);

		uint32_t* count = rjd_array_count_impl(newarray);
		*count = rjd_array_count(array);

		memcpy(newarray, array, (*count * sizeof_type));

		rjd_array_free(array);
		return newarray;
	}

	return array;
}

void* rjd_array_resize_impl(void* array, uint32_t newcount, size_t sizeof_type)
{
	RJD_ASSERT(array);
	RJD_ASSERT(sizeof_type > 0);

	rjd_array_validate(array);

	array = rjd_array_reserve_impl(array, newcount, sizeof_type);
	uint32_t* count = rjd_array_count_impl(array);

	// zero new members
	if (newcount > *count) {
		memset((char*)array + (*count * sizeof_type), 0, (newcount - *count) * sizeof_type);
	}

	*count = newcount;
	return array;
}

void* rjd_array_trim_impl(void* array, size_t sizeof_type)
{
	if (rjd_array_count(array) == rjd_array_capacity(array)) {
		return array;
	}

	void* newarray = rjd_array_alloc_impl(rjd_array_count(array), rjd_array_allocator(array), sizeof_type);
	memcpy(newarray, array, rjd_array_count(array) * sizeof_type);
	*rjd_array_count_impl(newarray) = rjd_array_count(array);

	rjd_array_free(array);

	return newarray;
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

void* rjd_array_push_impl(void* array, size_t sizeof_type) 
{
	array = rjd_array_grow(array, sizeof_type);
	++*rjd_array_count_impl(array);

	// skip init new element to 0 since it will be set in the push macro

	return array;
}

void rjd_array_pop_impl(void* array)
{
	RJD_ASSERT(rjd_array_count(array) > 0);
}

void* rjd_array_insert_impl(void* array, const void* insert, size_t sizeof_type, uint32_t index)
{
	array = rjd_array_grow(array, sizeof_type);
	uint32_t* count = rjd_array_count_impl(array);
	++*count;

	RJD_ASSERT(*count > index);

	void* where_to_insert = (char*)array + sizeof_type * index;
	void* element_after_insert = (char*)where_to_insert + sizeof_type;
	memmove(element_after_insert, where_to_insert, sizeof_type * (*count - index));
	memcpy(where_to_insert, insert, sizeof_type);

	return array;
}

void rjd_array_get_validate(void* array, uint32_t index)
{
	rjd_array_validate(array);
	RJD_ASSERT(index < rjd_array_count(array));
}

int32_t rjd_array_find_impl(const void* array, const void* search, size_t sizeof_type, int unused)
{
	RJD_UNUSED_PARAM(unused);

	rjd_array_validate(array);

	if (!array) {
		return false;
	}

	char* raw = (char*)array;
	for (int32_t i = 0; i < (int32_t)rjd_array_count(array); ++i) {
		if (!memcmp(raw + i * sizeof_type, search, sizeof_type)) {
			return i;
		}
	}
	return RJD_ARRAY_NOT_FOUND;
}

bool rjd_array_contains_impl(const void* array, const void* search, size_t sizeof_type, int unused)
{
	RJD_UNUSED_PARAM(unused);

	const int32_t index = rjd_array_find_impl(array, search, sizeof_type, 0);
	return index != RJD_ARRAY_NOT_FOUND;
}

void rjd_array_sort_impl(void* array, size_t sizeof_type, rjd_array_compare_func* comparer)
{
	rjd_array_validate(array);
	size_t length = rjd_array_count(array);
	qsort(array, length, sizeof_type, comparer);
}

int32_t rjd_array_lowerbound_impl(const void* array, const void* needle, size_t sizeof_type, rjd_array_compare_func* comparer, int unused)
{
	RJD_UNUSED_PARAM(unused);

	rjd_array_validate(array);

	uint32_t length = rjd_array_count(array);

	if (length == 0) {
		return 0;
	}

	int32_t first = 0;
	int32_t index = 0;
	
	while (length > 0)
	{
		int32_t step = length / 2;
		index = first + step;

		const void* value = (const char*)array + index * sizeof_type;
		const int32_t compared_value = comparer(value, needle);

		if (compared_value < 0) {
			++index;
			first = index;
			length -= step + 1;
		} else if (compared_value > 0) {
			length = step;
		} else {
			break;
		}
	}
	return index;
}

int32_t rjd_array_find_sorted_impl(const void* array, const void* needle, size_t sizeof_type, rjd_array_compare_func* comparer, int unused)
{
	RJD_UNUSED_PARAM(unused);

	const int32_t index = rjd_array_lowerbound_impl(array, needle, sizeof_type, comparer, 0);
	const void* value = (const char*)array + (index * sizeof_type);
	if (comparer(array, value) == 0) {
		return index;
	}
	return RJD_ARRAY_NOT_FOUND;
}

bool rjd_array_contains_sorted_impl(const void* array, const void* needle, size_t sizeof_type, rjd_array_compare_func* comparer, int unused)
{
	RJD_UNUSED_PARAM(unused);

	const int32_t index = rjd_array_find_sorted_impl(array, needle, sizeof_type, comparer, 0);
	return index != RJD_ARRAY_NOT_FOUND;
}

void rjd_array_sort_c_impl(void* array, size_t sizeof_type, rjd_array_compare_c_func* comparer, void* context)
{
	rjd_array_validate(array);
	const size_t length = rjd_array_count(array);
#if RJD_COMPILER_MSVC
	qsort_s(array, length, sizeof_type, comparer, context);
#else
	qsort_r(array, length, sizeof_type, context, comparer);
#endif
}

int32_t rjd_array_lowerbound_c_impl(const void* array, const void* needle, size_t sizeof_type, rjd_array_compare_c_func* comparer, void* context, int unused)
{
	RJD_UNUSED_PARAM(unused);

	rjd_array_validate(array);

	uint32_t length = rjd_array_count(array);

	if (length == 0) {
		return 0;
	}

	int32_t first = 0;
	int32_t index = 0;
	
	while (length > 0)
	{
		int32_t step = length / 2;
		index = first + step;

		const void* value = (const char*)array + index * sizeof_type;
		const int32_t compared_value = comparer(context, value, needle);

		if (compared_value < 0) {
			++index;
			first = index;
			length -= step + 1;
		} else if (compared_value > 0) {
			length = step;
		} else {
			break;
		}
	}
	return index;
}

int32_t rjd_array_find_sorted_c_impl(const void* array, const void* needle, size_t sizeof_type, rjd_array_compare_c_func* comparer, void* context, int unused)
{
	RJD_UNUSED_PARAM(unused);

	const int32_t index = rjd_array_lowerbound_c_impl(array, needle, sizeof_type, comparer, context, 0);
	const void* value = (const char*)array + (index * sizeof_type);
	if (comparer(context, array, value) == 0) {
		return index;
	}
	return RJD_ARRAY_NOT_FOUND;
}

bool rjd_array_contains_sorted_c_impl(const void* array, const void* needle, size_t sizeof_type, rjd_array_compare_c_func* comparer, void* context, int unused)
{
	RJD_UNUSED_PARAM(unused);

	const int32_t index = rjd_array_find_sorted_c_impl(array, needle, sizeof_type, comparer, context, 0);
	return index != RJD_ARRAY_NOT_FOUND;
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

////////////////////////////////////////////////////////////////////////////////
// local helpers

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
	RJD_ASSERTMSG(header->debug_sentinel == RJD_ARRAY_DEBUG_SENTINEL32, 
		"Debug sentinel was either corrupted by an underrun or this is not an rjd_array.");
	RJD_UNUSED_PARAM(header);
}

static void* rjd_array_grow(void* array, size_t sizeof_type)
{
	RJD_ASSERT(array);
	RJD_ASSERT(sizeof_type > 0);

	rjd_array_validate(array);

	uint32_t count = rjd_array_count(array);
	uint32_t capacity = rjd_array_capacity(array);
	if (count == capacity) {
		array = rjd_array_reserve_impl(array, capacity * 2, sizeof_type);
	}
	return array;
}

#endif //RJD_IMPL

