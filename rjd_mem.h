#pragma once

#define RJD_MEM 1

// TODO realloc
typedef void* (*rjd_mem_allocfunc)(size_t size);
typedef void* (*rjd_mem_allocfunc_scoped)(size_t size, void* allocator);

typedef void (*rjd_mem_freefunc)(void* memory);
typedef void (*rjd_mem_freefunc_scoped)(void* memory, void* heap);

struct rjd_mem_allocator
{
	rjd_mem_allocfunc alloc_global;
	rjd_mem_freefunc free_global;
	
	rjd_mem_allocfunc_scoped alloc_scoped;
	rjd_mem_freefunc_scoped free_scoped;
	void* scope;

	uint32_t debug_sentinel;
};

struct rjd_mem_allocator rjd_mem_allocator_initdefault(void);
struct rjd_mem_allocator rjd_mem_allocator_initglobal(rjd_mem_allocfunc a, rjd_mem_freefunc f);
struct rjd_mem_allocator rjd_mem_allocator_initscoped(rjd_mem_allocfunc_scoped a, rjd_mem_freefunc_scoped f, void* allocator);
struct rjd_mem_allocator rjd_mem_allocator_initlinear(void* mem, size_t heapsize);
void* rjd_mem_alloc_impl(size_t size, struct rjd_mem_allocator* allocator);
void rjd_mem_free(const void* mem);
void rjd_mem_swap(void* restrict mem1, void* restrict mem2, size_t size);

#define rjd_mem_alloc(type, allocator) ((type*)rjd_mem_alloc_impl(sizeof(type), allocator))
#define rjd_mem_alloc_array(type, count, allocator) ((type*)rjd_mem_alloc_impl(sizeof(type) * count, allocator))

#define RJD_MEM_ISALIGNED(p, align) (((uintptr_t)(p) & ((align)-1)) == 0)
#define RJD_MEM_ALIGNSIZE(size, align) ((size) + (RJD_MEM_ISALIGNED(size, align) ? 0 : ((align) - ((size) & ((align)-1)))))

#if RJD_ENABLE_SHORTNAMES
	#define mem_allocator_initdefault   	rjd_mem_allocator_initdefault
	#define mem_allocator_initglobal    	rjd_mem_allocator_initglobal
	#define mem_allocator_initscoped    	rjd_mem_allocator_initscoped
	#define mem_allocator_initlinear		rjd_mem_allocator_initlinear
	#define mem_alloc						rjd_mem_alloc
	#define mem_alloc_array					rjd_mem_alloc_array
	#define mem_free						rjd_mem_free
	#define mem_swap						rjd_mem_swap

	#define MEM_ISALIGNED 					RJD_MEM_ISALIGNED
	#define MEM_ALIGNSIZE					RJD_MEM_ALIGNSIZE
#endif

#ifdef RJD_IMPL

struct rjd_mem_heap_linear
{
	void* base;
	void* next;
	size_t size;
};

static struct rjd_mem_heap_linear rjd_mem_linear_init(void* mem, size_t size);
static void* rjd_mem_linear_alloc(size_t size, void* heap);
static void rjd_mem_free_scoped_noop(void* mem, void* heap);

#if RJD_COMPILER_MSVC
	static void* rjd_mem_alloc_global_wrapper(size_t size);
	static void rjd_mem_free_global_wrapper(void* mem);
#endif

#define RJD_MEM_DEBUG_SENTINEL32 (0xA7A7A7A7u)
#define RJD_MEM_DEBUG_SENTINEL64 (0xA7A7A7A7A7A7A7A7ull)

struct rjd_mem_allocator rjd_mem_allocator_initglobal(rjd_mem_allocfunc a, rjd_mem_freefunc f)
{
	struct rjd_mem_allocator allocator = { a, f, NULL, NULL, NULL, RJD_MEM_DEBUG_SENTINEL32 };
	return allocator;
}

struct rjd_mem_allocator rjd_mem_allocator_initscoped(rjd_mem_allocfunc_scoped a, rjd_mem_freefunc_scoped f, void* heap)
{
	struct rjd_mem_allocator allocator = { NULL, NULL, a, f, heap, RJD_MEM_DEBUG_SENTINEL32 };
	return allocator;
}

struct rjd_mem_allocator rjd_mem_allocator_initdefault()
{
	#if RJD_COMPILER_MSVC
		return rjd_mem_allocator_initglobal(rjd_mem_alloc_global_wrapper, rjd_mem_free_global_wrapper);
	#else
		return rjd_mem_allocator_initglobal(malloc, free);
	#endif
}

struct rjd_mem_allocator rjd_mem_allocator_initlinear(void* mem, size_t heapsize)
{
	char* bytes = (char*)mem;
	const size_t structsize = sizeof(struct rjd_mem_heap_linear);
	struct rjd_mem_heap_linear* heap = (struct rjd_mem_heap_linear*)bytes + heapsize - structsize;
	*heap = rjd_mem_linear_init(mem, heapsize - structsize);

	return rjd_mem_allocator_initscoped(rjd_mem_linear_alloc, rjd_mem_free_scoped_noop, heap);
}

void* rjd_mem_alloc_impl(size_t size, struct rjd_mem_allocator* allocator)
{
	const size_t aligned_size = RJD_MEM_ALIGNSIZE(sizeof(struct rjd_mem_allocator), 16);
	if (size == 0) {
		size = 4;
	}

	void* mem = NULL;
	if (allocator->alloc_global) {
		mem = allocator->alloc_global(size + aligned_size);
	} else {
		mem = allocator->alloc_scoped(size + aligned_size, allocator->scope);
	}

	struct rjd_mem_allocator** ptr = mem;
	*ptr = allocator;

	char* raw = mem;
	return (void*)(raw + aligned_size);
}

void rjd_mem_free(const void* mem)
{
	if (!mem) {
		return;
	}

	const size_t aligned_size = RJD_MEM_ALIGNSIZE(sizeof(struct rjd_mem_allocator), 16);

	char* raw = (void*)mem;
	char* begin = raw - aligned_size;
	struct rjd_mem_allocator** ptr = (struct rjd_mem_allocator**)begin;
	struct rjd_mem_allocator* allocator = *ptr;
	RJD_ASSERTMSG(allocator->debug_sentinel == RJD_MEM_DEBUG_SENTINEL32, "This memory was not allocated with rjd_mem_alloc.");

	if (allocator->free_global) {
		allocator->free_global(begin);
	} else {
		allocator->free_scoped(begin, allocator->scope);
	}
}

void rjd_mem_swap(void* restrict mem1, void* restrict mem2, size_t size)
{
	uint8_t tmp[1024];
	RJD_ASSERTMSG(size < sizeof(tmp), "Increase size of static tmp buffer to at least %z", size);

	memcpy(tmp, mem1, size);
	memcpy(mem1, mem2, size);
	memcpy(mem2, tmp, size);
}

static struct rjd_mem_heap_linear rjd_mem_linear_init(void* mem, size_t size)
{
	struct rjd_mem_heap_linear heap = { mem, mem, size };
	return heap;
}

static void* rjd_mem_linear_alloc(size_t size, void* userheap)
{
	size_t align_diff = size % 8;
	if (align_diff != 0) {
		size += align_diff;
	}

	struct rjd_mem_heap_linear* heap = (struct rjd_mem_heap_linear*)userheap;
	
	if ((char*)heap->next + size <= (char*)heap->base + heap->size) 
	{
		void* mem = (char*)heap->next;
		heap->next = (char*)heap->next + size;

		return mem;
	}

	return NULL;
}

static void rjd_mem_free_scoped_noop(void* mem, void* heap)
{
	RJD_UNUSED_PARAM(mem);
	RJD_UNUSED_PARAM(heap);
}

#if RJD_COMPILER_MSVC
	static void* rjd_mem_alloc_global_wrapper(size_t size)
	{
		return malloc(size);
	}
	
	static void rjd_mem_free_global_wrapper(void* mem)
	{
		free(mem);
	}
#endif

#endif // RJD_IMPL

