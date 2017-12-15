#pragma once

#define RJD_ALLOC 1

// DEPENDENCIES: 
// stdlib.h (malloc/free)
// rjd_debug.h

// TODO realloc
typedef void* (*rjd_func_alloc)(size_t size);
typedef void* (*rjd_func_alloc_scoped)(size_t size, void* allocator);

typedef void (*rjd_func_free)(void* memory);
typedef void (*rjd_func_free_scoped)(void* memory, void* heap);

struct rjd_alloc_context
{
	rjd_func_alloc alloc_global;
	rjd_func_free free_global;
	
	rjd_func_alloc_scoped alloc_scoped;
	rjd_func_free_scoped free_scoped;
	void* scope;
};

struct rjd_linearheap
{
	void* base;
	void* next;
	size_t size;
};

struct rjd_alloc_context rjd_alloc_initdefault(void);
struct rjd_alloc_context rjd_alloc_initglobal(rjd_func_alloc a, rjd_func_free f);
struct rjd_alloc_context rjd_alloc_initscoped(rjd_func_alloc_scoped a, rjd_func_free_scoped f, void* allocator);
struct rjd_alloc_context rjd_alloc_initlinearheap(void* mem, size_t heapsize);
void* rjd_malloc_impl(size_t size, struct rjd_alloc_context* context);
void rjd_free(const void* mem, struct rjd_alloc_context* context);

#define rjd_malloc(type, context) ((type*)rjd_malloc_impl(sizeof(type), context))
#define rjd_malloc_array(type, count, context) ((type*)rjd_malloc_impl(sizeof(type) * count, context))

#define RJD_ISALIGNED(p, align) ((p) & ((align)-1) == 0)
#if RJD_COMPILER_MSVC
	#define RJD_FORCEALIGN(alignment) (__declspec(alignment))
#elif RJD_COMPILER_GCC || RJD_COMPILER_CLANG
	#define RJD_FORCEALIGN(alignment) (__attribute__((aligned(alignment))))
#else
	#error Unhandled compiler
#endif

#if RJD_ENABLE_SHORTNAMES
	#define ISALIGNED RJD_ISALIGNED
	#define FORCEALIGN RJD_FORCEALIGN
	
	#define alloc_initdefault    rjd_alloc_initdefault
	#define alloc_initglobal     rjd_alloc_initglobal
	#define alloc_initscoped     rjd_alloc_initscoped
	#define alloc_initlinearheap rjd_alloc_initlinearheap
	#define rmalloc rjd_malloc
	#define rmalloc_array rjd_malloc_array
	#define rfree rjd_free
#endif

#ifdef RJD_IMPL

// detail functions
static struct rjd_linearheap rjd_linearheap_init(void* mem, size_t size);
static void* rjd_linearheap_malloc(size_t size, void* heap);
static void rjd_free_scoped_noop(void* mem, void* heap);

struct rjd_alloc_context rjd_alloc_initglobal(rjd_func_alloc a, rjd_func_free f)
{
	struct rjd_alloc_context context = { a, f, NULL, NULL, NULL };
	return context;
}

struct rjd_alloc_context rjd_alloc_initscoped(rjd_func_alloc_scoped a, rjd_func_free_scoped f, void* heap)
{
	struct rjd_alloc_context context = { NULL, NULL, a, f, heap };
	return context;
}

struct rjd_alloc_context rjd_alloc_initdefault()
{
	return rjd_alloc_initglobal(malloc, free);
}

struct rjd_alloc_context rjd_alloc_initlinearheap(void* mem, size_t heapsize)
{
	char* bytes = (char*)mem;
	const size_t structsize = sizeof(struct rjd_linearheap);
	struct rjd_linearheap* heap = (struct rjd_linearheap*)bytes + heapsize - structsize;
	*heap = rjd_linearheap_init(mem, heapsize - structsize);

	return rjd_alloc_initscoped(rjd_linearheap_malloc, rjd_free_scoped_noop, heap);
}

void* rjd_malloc_impl(size_t size, struct rjd_alloc_context* context)
{
	if (context->alloc_global) {
		return context->alloc_global(size);
	}
	return context->alloc_scoped(size, context->scope);
}

void rjd_free(const void* mem, struct rjd_alloc_context* context)
{
	if (context->free_global) {
		context->free_global((void*)mem);
	} else {
		context->free_scoped((void*)mem, context->scope);
	}
}

static struct rjd_linearheap rjd_linearheap_init(void* mem, size_t size)
{
	struct rjd_linearheap heap = { mem, mem, size };
	return heap;
}

static void* rjd_linearheap_malloc(size_t size, void* userheap)
{
	size_t align_diff = size % 8;
	if (align_diff != 0) {
		size += align_diff;
	}

	struct rjd_linearheap* heap = (struct rjd_linearheap*)userheap;
	
	if ((char*)heap->next + size <= (char*)heap->base + heap->size) 
	{
		void* mem = (char*)heap->next;
		heap->next = (char*)heap->next + size;

		return mem;
	}

	return NULL;
}

static void rjd_free_scoped_noop(void* mem, void* heap)
{
	RJD_UNUSED_PARAM(mem);
	RJD_UNUSED_PARAM(heap);
}

#endif // RJD_IMPL

