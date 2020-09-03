#pragma once

#define RJD_THREAD

#ifndef RJD_THREAD_STACKSIZE_DEFAULT
	#define RJD_THREAD_STACKSIZE_DEFAULT (4 * RJD_MB)
#endif

#define RJD_THREAD_NAME_MAX_LENGTH (16u)

typedef void rjd_thread_entrypoint_func(void* userdata);

struct rjd_thread_desc
{
	rjd_thread_entrypoint_func* entrypoint_func;
	struct rjd_mem_allocator* allocator;
	uint32_t stacksize;
	void* userdata;
	const char name[RJD_THREAD_NAME_MAX_LENGTH];
};

struct rjd_thread_id
{
	uint64_t id;
};

struct rjd_thread_platform
{
	char platform_impl[16];
};

struct rjd_thread
{
	struct rjd_thread_id id;
	struct rjd_thread_platform platform;
};

struct rjd_condvar
{
	char platform_impl[128];
};

struct rjd_lock
{
	char platform_impl[64];
};

struct rjd_rwlock
{
	char platform_impl[256];
};

struct rjd_thread_id rjd_thread_current(void);
struct rjd_result rjd_thread_create(struct rjd_thread* thread, struct rjd_thread_desc desc);
struct rjd_result rjd_thread_join(struct rjd_thread* thread);
struct rjd_result rjd_thread_getname(struct rjd_thread* thread, uint32_t destination_max_length, char* out);
void rjd_thread_sleep(uint32_t seconds);

struct rjd_result rjd_condvar_create(struct rjd_condvar* condvar);
struct rjd_result rjd_condvar_destroy(struct rjd_condvar* condvar);
struct rjd_result rjd_condvar_lock(struct rjd_condvar* condvar);
struct rjd_result rjd_condvar_unlock(struct rjd_condvar* condvar);
struct rjd_result rjd_condvar_signal_single(struct rjd_condvar* condvar);
struct rjd_result rjd_condvar_signal_all(struct rjd_condvar* condvar);
struct rjd_result rjd_condvar_wait(struct rjd_condvar* condvar);
struct rjd_result rjd_condvar_wait_timed(struct rjd_condvar* condvar, uint32_t seconds);

struct rjd_result rjd_lock_create(struct rjd_lock* lock);
struct rjd_result rjd_lock_destroy(struct rjd_lock* lock);
struct rjd_result rjd_lock_acquire(struct rjd_lock* lock);
struct rjd_result rjd_lock_try_acquire(struct rjd_lock* lock);
struct rjd_result rjd_lock_release(struct rjd_lock* lock);

struct rjd_result rjd_rwlock_create(struct rjd_rwlock* lock);
struct rjd_result rjd_rwlock_destroy(struct rjd_rwlock* lock);
struct rjd_result rjd_rwlock_acquire_reader(struct rjd_rwlock* lock);
struct rjd_result rjd_rwlock_acquire_writer(struct rjd_rwlock* lock);
struct rjd_result rjd_rwlock_try_acquire_reader(struct rjd_rwlock* lock);
struct rjd_result rjd_rwlock_try_acquire_writer(struct rjd_rwlock* lock);
struct rjd_result rjd_rwlock_release(struct rjd_rwlock* lock);

#if RJD_IMPL

#if RJD_PLATFORM_WINDOWS

#error "Unimplemented"

struct rjd_result rjd_lock_create(struct rjd_lock* lock)
{
	// posix mutex is non-rentrant but win32 critical section is. need to assert
	// that owning thread locking the CS isn't the current thread
}

#elif RJD_PLATFORM_OSX

#include <pthread.h>
#include <unistd.h> // sleep
#include <limits.h> // PTHREAD_STACK_MIN
#include <sys/errno.h>
#include <sys/time.h> // gettimeofday

struct rjd_thread_osx
{
	pthread_t handle;
	void* stack;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_thread_osx) <= sizeof(struct rjd_thread_platform));
RJD_STATIC_ASSERT(sizeof(pthread_t) <= sizeof(uint64_t));

struct rjd_thread_params
{
	rjd_thread_entrypoint_func* entrypoint_func;
	void* userdata;
	char name[RJD_THREAD_NAME_MAX_LENGTH];
};

struct rjd_condvar_osx
{
	pthread_cond_t condition_variable;
	struct rjd_lock lock;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_condvar_osx) <= sizeof(struct rjd_condvar));

struct rjd_lock_osx
{
	pthread_mutex_t lock;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_lock_osx) <= sizeof(struct rjd_lock));

struct rjd_rwlock_osx
{
	pthread_rwlock_t lock;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_rwlock_osx) <= sizeof(struct rjd_rwlock));

RJD_STATIC_ASSERT(RJD_THREAD_STACKSIZE_DEFAULT >= PTHREAD_STACK_MIN);

static inline struct rjd_thread_osx* rjd_thread_get_osx(struct rjd_thread* thread);
static inline struct rjd_condvar_osx* rjd_condvar_get_osx(struct rjd_condvar* condvar);
static inline struct rjd_lock_osx* rjd_lock_get_osx(struct rjd_lock* lock);
static inline struct rjd_rwlock_osx* rjd_rwlock_get_osx(struct rjd_rwlock* rwlock);

struct rjd_result rjd_error_to_result(int error);
static void* rjd_thread_entrypoint_osx(void* userdata);

////////////////////////////////////////////////////////////////////////////////
// interface implementation

struct rjd_result rjd_thread_create(struct rjd_thread* thread, struct rjd_thread_desc desc)
{
	struct rjd_thread_osx* thread_osx = rjd_thread_get_osx(thread);
	thread_osx->stack = NULL;

	if (desc.stacksize == 0) {
		desc.stacksize = RJD_THREAD_STACKSIZE_DEFAULT;
	}

	// pthreads require 4K alignment for stacks
	void* stack = rjd_mem_alloc_array_aligned_noclear(uint8_t, desc.stacksize, desc.allocator, 4 * RJD_KB);
	struct rjd_thread_params* params = rjd_mem_alloc(struct rjd_thread_params, desc.allocator);
	params->entrypoint_func = desc.entrypoint_func;
	params->userdata = desc.userdata;
	params->name[0] = 0;
	if (*desc.name) {
		strncpy(params->name, desc.name, RJD_THREAD_NAME_MAX_LENGTH);
        params->name[rjd_countof(params->name) - 1] = 0;
	}

    pthread_attr_t attributes = {0};
    int error = pthread_attr_init(&attributes);
	struct rjd_result result = rjd_error_to_result(error);

	if (rjd_result_isok(result)) {
		error = pthread_attr_setstack(&attributes, stack, desc.stacksize);
        if (error == EINVAL && desc.stacksize < PTHREAD_STACK_MIN) {
            result = RJD_RESULT("Failed to set the stack. Ensure it is at least as large as PTHREAD_STACK_MIN");
        } else {
            result = rjd_error_to_result(error);
        }
        
		if (rjd_result_isok(result)) {
			// NOTE params ownership is passed to thread
			error = pthread_create(&thread_osx->handle, &attributes, rjd_thread_entrypoint_osx, params);
			result = rjd_error_to_result(error);
			if (rjd_result_isok(result)) {
				thread->id.id = (uint64_t)thread_osx->handle;
				thread_osx->stack = stack;
			}
		}
	}

	if (!rjd_result_isok(result)) {
		rjd_mem_free(params);
		rjd_mem_free(stack);
	}

	return result;
}

struct rjd_result rjd_thread_join(struct rjd_thread* thread)
{
	struct rjd_thread_osx* thread_osx = rjd_thread_get_osx(thread);

	void* unused_return_value = NULL;
    int error = pthread_join(thread_osx->handle, &unused_return_value);

	rjd_mem_free(thread_osx->stack);

	return rjd_error_to_result(error);
}

struct rjd_result rjd_thread_getname(struct rjd_thread* thread, uint32_t destination_max_length, char* out)
{
	struct rjd_thread_osx* thread_osx = rjd_thread_get_osx(thread);

	struct rjd_result result = RJD_RESULT_OK();

	int error = pthread_getname_np(thread_osx->handle, out, destination_max_length);
	if (error == ERANGE) {
		result = RJD_RESULT("Destination buffer too short");
	}

	return result;
}

void rjd_thread_sleep(uint32_t seconds)
{
	sleep(seconds);
}

struct rjd_result rjd_condvar_create(struct rjd_condvar* condvar)
{
	struct rjd_condvar_osx* condvar_osx = rjd_condvar_get_osx(condvar);

	struct rjd_result result = rjd_lock_create(&condvar_osx->lock);
	if (rjd_result_isok(result)) {
		const int error = pthread_cond_init(&condvar_osx->condition_variable, NULL);
		result = rjd_error_to_result(error);
	}

	return result;
}

struct rjd_result rjd_condvar_destroy(struct rjd_condvar* condvar)
{
	struct rjd_condvar_osx* condvar_osx = rjd_condvar_get_osx(condvar);

    struct rjd_result result = rjd_lock_release(&condvar_osx->lock);
    if (rjd_result_isok(result)) {
        result = rjd_lock_destroy(&condvar_osx->lock);
        if (rjd_result_isok(result)) {
            const int error = pthread_cond_destroy(&condvar_osx->condition_variable);
            result = rjd_error_to_result(error);
        }
    }

	return result;
}

struct rjd_result rjd_condvar_lock(struct rjd_condvar* condvar)
{
	struct rjd_condvar_osx* condvar_osx = rjd_condvar_get_osx(condvar);
	return rjd_lock_acquire(&condvar_osx->lock);
}

struct rjd_result rjd_condvar_unlock(struct rjd_condvar* condvar)
{
	struct rjd_condvar_osx* condvar_osx = rjd_condvar_get_osx(condvar);
	return rjd_lock_release(&condvar_osx->lock);
}


struct rjd_result rjd_condvar_signal_single(struct rjd_condvar* condvar)
{
	struct rjd_condvar_osx* condvar_osx = rjd_condvar_get_osx(condvar);

	struct rjd_result result = rjd_lock_acquire(&condvar_osx->lock);
	if (rjd_result_isok(result)) {
		int error = pthread_cond_signal(&condvar_osx->condition_variable);
		result = rjd_error_to_result(error);
		rjd_lock_release(&condvar_osx->lock);
	}
	return result;
}

struct rjd_result rjd_condvar_signal_all(struct rjd_condvar* condvar)
{
	struct rjd_condvar_osx* condvar_osx = rjd_condvar_get_osx(condvar);
	
	struct rjd_result result = rjd_lock_acquire(&condvar_osx->lock);
	if (rjd_result_isok(result)) {
		int error = pthread_cond_broadcast(&condvar_osx->condition_variable);
		result = rjd_error_to_result(error);
		rjd_lock_release(&condvar_osx->lock);
	}
	return result;
}

struct rjd_result rjd_condvar_wait(struct rjd_condvar* condvar)
{
	struct rjd_condvar_osx* condvar_osx = rjd_condvar_get_osx(condvar);
	struct rjd_lock_osx* lock_osx = rjd_lock_get_osx(&condvar_osx->lock);

	int error = pthread_cond_wait(&condvar_osx->condition_variable, &lock_osx->lock);
	struct rjd_result result = rjd_error_to_result(error);
	return result;
}

struct rjd_result rjd_condvar_wait_timed(struct rjd_condvar* condvar, uint32_t seconds)
{
	struct rjd_condvar_osx* condvar_osx = rjd_condvar_get_osx(condvar);
	struct rjd_lock_osx* lock_osx = rjd_lock_get_osx(&condvar_osx->lock);

	struct timeval timeval = {0};
	gettimeofday(&timeval, NULL);

	struct timespec timespec = {
		.tv_sec = timeval.tv_sec + seconds,
        .tv_nsec = timeval.tv_usec * 1000,
	};
	int error = pthread_cond_timedwait(&condvar_osx->condition_variable, &lock_osx->lock, &timespec);
	struct rjd_result result = rjd_error_to_result(error);
   
	return result;
}

struct rjd_result rjd_lock_create(struct rjd_lock* lock)
{
	struct rjd_lock_osx* lock_osx = rjd_lock_get_osx(lock);

    int error = pthread_mutex_init(&lock_osx->lock, NULL);
	struct rjd_result result = rjd_error_to_result(error);
	return result;
}

struct rjd_result rjd_lock_destroy(struct rjd_lock* lock)
{
	struct rjd_lock_osx* lock_osx = rjd_lock_get_osx(lock);
	
	int error = pthread_mutex_destroy(&lock_osx->lock);
	struct rjd_result result = rjd_error_to_result(error);
	return result;
}

struct rjd_result rjd_lock_acquire(struct rjd_lock* lock)
{
	struct rjd_lock_osx* lock_osx = rjd_lock_get_osx(lock);
	
	int error = pthread_mutex_lock(&lock_osx->lock);
	struct rjd_result result = rjd_error_to_result(error);
	return result;
}

struct rjd_result rjd_lock_try_acquire(struct rjd_lock* lock)
{
	struct rjd_lock_osx* lock_osx = rjd_lock_get_osx(lock);
	
	int error = pthread_mutex_trylock(&lock_osx->lock);
	struct rjd_result result = rjd_error_to_result(error);
	return result;
}

struct rjd_result rjd_lock_release(struct rjd_lock* lock)
{
	struct rjd_lock_osx* lock_osx = rjd_lock_get_osx(lock);
	
	int error = pthread_mutex_unlock(&lock_osx->lock);
	struct rjd_result result = rjd_error_to_result(error);
	return result;
}

struct rjd_result rjd_rwlock_create(struct rjd_rwlock* lock)
{
	struct rjd_rwlock_osx* lock_osx = rjd_rwlock_get_osx(lock);
	
	int error = pthread_rwlock_init(&lock_osx->lock, NULL);
	struct rjd_result result = rjd_error_to_result(error);
	return result;
}

struct rjd_result rjd_rwlock_destroy(struct rjd_rwlock* lock)
{
	struct rjd_rwlock_osx* lock_osx = rjd_rwlock_get_osx(lock);
	
	int error = pthread_rwlock_destroy(&lock_osx->lock);
	struct rjd_result result = rjd_error_to_result(error);
	return result;
}

struct rjd_result rjd_rwlock_acquire_reader(struct rjd_rwlock* lock)
{
	struct rjd_rwlock_osx* lock_osx = rjd_rwlock_get_osx(lock);
	
	int error = pthread_rwlock_rdlock(&lock_osx->lock);
	struct rjd_result result = rjd_error_to_result(error);
	return result;
}

struct rjd_result rjd_rwlock_acquire_writer(struct rjd_rwlock* lock)
{
	struct rjd_rwlock_osx* lock_osx = rjd_rwlock_get_osx(lock);
	
	int error = pthread_rwlock_wrlock(&lock_osx->lock);
	struct rjd_result result = rjd_error_to_result(error);
	return result;
}

struct rjd_result rjd_rwlock_try_acquire_reader(struct rjd_rwlock* lock)
{
	struct rjd_rwlock_osx* lock_osx = rjd_rwlock_get_osx(lock);
	
	int error = pthread_rwlock_tryrdlock(&lock_osx->lock);
	struct rjd_result result = rjd_error_to_result(error);
	return result;
}

struct rjd_result rjd_rwlock_try_acquire_writer(struct rjd_rwlock* lock)
{
	struct rjd_rwlock_osx* lock_osx = rjd_rwlock_get_osx(lock);
	
	int error = pthread_rwlock_trywrlock(&lock_osx->lock);
	struct rjd_result result = rjd_error_to_result(error);
	return result;
}

struct rjd_result rjd_rwlock_release(struct rjd_rwlock* lock)
{
	struct rjd_rwlock_osx* lock_osx = rjd_rwlock_get_osx(lock);
	
	int error = pthread_rwlock_unlock(&lock_osx->lock);
	struct rjd_result result = rjd_error_to_result(error);
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// Local helper implementation

static inline struct rjd_thread_osx* rjd_thread_get_osx(struct rjd_thread* thread)
{
	RJD_ASSERT(thread);
	return (struct rjd_thread_osx*)&thread->platform;
}

static inline struct rjd_condvar_osx* rjd_condvar_get_osx(struct rjd_condvar* condvar)
{
	RJD_ASSERT(condvar);
	return (struct rjd_condvar_osx*)condvar;
}

static inline struct rjd_lock_osx* rjd_lock_get_osx(struct rjd_lock* lock)
{
	RJD_ASSERT(lock);
	return (struct rjd_lock_osx*)lock;
}

static inline struct rjd_rwlock_osx* rjd_rwlock_get_osx(struct rjd_rwlock* rwlock)
{
	RJD_ASSERT(rwlock);
	return (struct rjd_rwlock_osx*)rwlock;
}

struct rjd_result rjd_error_to_result(int error)
{
	switch (error)
	{
		case 0:			return RJD_RESULT_OK();
		case EAGAIN:	return RJD_RESULT("Not enough system resources to create this object or system capacity is already at maximum. Try again later.");
		case EBUSY:		return RJD_RESULT("Unable to perform operation without blocking. If the object is a lock, the thread may already be holding it.");
		case EDEADLK:	return RJD_RESULT("Deadlock detected.");
		case EINVAL:	return RJD_RESULT("Invalid object.");
		case ENOMEM:	return RJD_RESULT("Not enough system memory available.");
		case EPERM: 	return RJD_RESULT("This operation requires higher permissions.");
		case ESRCH:		return RJD_RESULT("Object not found.");
		case ETIMEDOUT:	return RJD_RESULT("Timed out");
	}
	
	return RJD_RESULT("Unknown error");
}

static void* rjd_thread_entrypoint_osx(void* params_untyped)
{
	struct rjd_thread_params params = *(struct rjd_thread_params*)params_untyped;
	rjd_mem_free(params_untyped);

	if (*params.name) {
		pthread_setname_np(params.name);
	}
	params.entrypoint_func(params.userdata);
	return NULL;
}

#endif

#endif
