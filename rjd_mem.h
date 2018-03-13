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
};

struct rjd_mem_heap_linear
{
	void* base;
	void* next;
	size_t size;
};

struct rjd_mem_allocator rjd_mem_allocator_initdefault(void);
struct rjd_mem_allocator rjd_mem_allocator_initglobal(rjd_mem_allocfunc a, rjd_mem_freefunc f);
struct rjd_mem_allocator rjd_mem_allocator_initscoped(rjd_mem_allocfunc_scoped a, rjd_mem_freefunc_scoped f, void* allocator);
struct rjd_mem_allocator rjd_mem_allocator_initlinearheap(void* mem, size_t heapsize);
void* rjd_mem_alloc_impl(size_t size, struct rjd_mem_allocator* context);
void rjd_mem_free(const void* mem, struct rjd_mem_allocator* context);
void rjd_mem_swap(void* restrict mem1, void* restrict mem2, size_t size);

#define rjd_mem_alloc(type, context) ((type*)rjd_mem_alloc_impl(sizeof(type), context))
#define rjd_mem_alloc_array(type, count, context) ((type*)rjd_mem_alloc_impl(sizeof(type) * count, context))

#if RJD_ENABLE_SHORTNAMES
	#define mem_allocator_initdefault   	rjd_mem_allocator_initdefault
	#define mem_allocator_initglobal    	rjd_mem_allocator_initglobal
	#define mem_allocator_initscoped    	rjd_mem_allocator_initscoped
	#define mem_allocator_initlinearheap	rjd_mem_allocator_initlinearheap
	#define mem_alloc						rjd_mem_alloc
	#define mem_alloc_array					rjd_mem_alloc_array
	#define mem_free						rjd_mem_free
	#define mem_swap						rjd_mem_swap
#endif

#ifdef RJD_IMPL

// detail functions
static struct rjd_mem_heap_linear rjd_mem_linearheap_init(void* mem, size_t size);
static void* rjd_mem_linearheap_alloc(size_t size, void* heap);
static void rjd_mem_free_scoped_noop(void* mem, void* heap);

#if RJD_COMPILER_MSVC
	static void* rjd_mem_alloc_global_wrapper(size_t size);
	static void rjd_mem_free_global_wrapper(void* mem);
#endif

struct rjd_mem_allocator rjd_mem_allocator_initglobal(rjd_mem_allocfunc a, rjd_mem_freefunc f)
{
	struct rjd_mem_allocator context = { a, f, NULL, NULL, NULL };
	return context;
}

struct rjd_mem_allocator rjd_mem_allocator_initscoped(rjd_mem_allocfunc_scoped a, rjd_mem_freefunc_scoped f, void* heap)
{
	struct rjd_mem_allocator context = { NULL, NULL, a, f, heap };
	return context;
}

struct rjd_mem_allocator rjd_mem_allocator_initdefault()
{
	#if RJD_COMPILER_MSVC
		return rjd_mem_allocator_initglobal(rjd_mem_alloc_global_wrapper, rjd_mem_free_global_wrapper);
	#else
		return rjd_mem_allocator_initglobal(malloc, free);
	#endif
}

struct rjd_mem_allocator rjd_mem_allocator_initlinearheap(void* mem, size_t heapsize)
{
	char* bytes = (char*)mem;
	const size_t structsize = sizeof(struct rjd_mem_heap_linear);
	struct rjd_mem_heap_linear* heap = (struct rjd_mem_heap_linear*)bytes + heapsize - structsize;
	*heap = rjd_mem_linearheap_init(mem, heapsize - structsize);

	return rjd_mem_allocator_initscoped(rjd_mem_linearheap_alloc, rjd_mem_free_scoped_noop, heap);
}

void* rjd_mem_alloc_impl(size_t size, struct rjd_mem_allocator* context)
{
	if (context->alloc_global) {
		return context->alloc_global(size);
	}
	return context->alloc_scoped(size, context->scope);
}

void rjd_mem_free(const void* mem, struct rjd_mem_allocator* context)
{
	if (context->free_global) {
		context->free_global((void*)mem);
	} else {
		context->free_scoped((void*)mem, context->scope);
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

static struct rjd_mem_heap_linear rjd_mem_linearheap_init(void* mem, size_t size)
{
	struct rjd_mem_heap_linear heap = { mem, mem, size };
	return heap;
}

static void* rjd_mem_linearheap_alloc(size_t size, void* userheap)
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

