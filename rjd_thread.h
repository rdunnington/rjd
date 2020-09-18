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
	const char optional_name[RJD_THREAD_NAME_MAX_LENGTH];
};

struct rjd_thread_id
{
	char platform_impl[8];
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
	char platform_impl[128 + 256];
};

struct rjd_lock
{
	char platform_impl[64];
};

struct rjd_rwlock
{
	char platform_impl[256];
};

struct rjd_thread_id rjd_thread_id_current(void);
bool rjd_thread_id_equals(const struct rjd_thread_id a, const struct rjd_thread_id b);

struct rjd_result rjd_thread_create(struct rjd_thread* thread, struct rjd_thread_desc desc);
struct rjd_result rjd_thread_join(struct rjd_thread* thread);
struct rjd_result rjd_thread_getname(struct rjd_thread* thread, uint32_t destination_max_length, char* out);
void rjd_thread_sleep(uint32_t seconds);

struct rjd_result rjd_condvar_create(struct rjd_condvar* condvar);
struct rjd_result rjd_condvar_destroy(struct rjd_condvar* condvar);
struct rjd_result rjd_condvar_lock(struct rjd_condvar* condvar);
struct rjd_result rjd_condvar_unlock(struct rjd_condvar* condvar);
struct rjd_result rjd_condvar_signal_single(struct rjd_condvar* condvar); // automatically unlocks
struct rjd_result rjd_condvar_signal_all(struct rjd_condvar* condvar); // automatically unlocks
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
struct rjd_result rjd_rwlock_release_reader(struct rjd_rwlock* lock);
struct rjd_result rjd_rwlock_release_writer(struct rjd_rwlock* lock);

#if RJD_IMPL

#if RJD_PLATFORM_WINDOWS

#include <processthreadsapi.h>
#include <synchapi.h>

const struct rjd_thread_id RJD_THREAD_ID_INVALID = { 0 };

struct rjd_thread_id_win32
{
	uint64_t id;
};

struct rjd_thread_win32
{
	HANDLE handle;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_thread_win32) <= sizeof(struct rjd_thread_platform));
RJD_STATIC_ASSERT(sizeof(HANDLE) <= sizeof(uint64_t));

struct rjd_thread_params
{
	rjd_thread_entrypoint_func* entrypoint_func;
	void* userdata;
};

struct rjd_condvar_win32
{
	CONDITION_VARIABLE condition_variable;
	struct rjd_rwlock lock;
	bool is_locked;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_condvar_win32) <= sizeof(struct rjd_condvar));

struct rjd_lock_win32
{
	struct rjd_thread_id owning_thread;
	CRITICAL_SECTION cs;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_lock_win32) <= sizeof(struct rjd_lock));

struct rjd_rwlock_win32
{
	SRWLOCK lock;
	struct rjd_thread_id exclusive_owning_thread;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_rwlock_win32) <= sizeof(struct rjd_rwlock));

DWORD WINAPI rjd_thread_entrypoint_win32(LPVOID lpParameter);

#if defined(_WIN32_WINNT) &&(_WIN32_WINNT >= _WIN32_WINNT_WIN10)	\
	&& defined(WINVER) && (WINVER >= _WIN32_WINNT_WIN10)			\
	&& defined(WDK_NTDDI_VERSION) && (WDK_NTDDI_VERSION >= NTDDI_WIN10_RS1)
	#define RJD_THREAD_NAME_AVAILABLE 1
#else 
	#define RJD_THREAD_NAME_AVAILABLE 0

	const struct rjd_result RJD_RESULT_THREADNAME_UNAVAILABLE = {
		"Thread naming uses features that require WINVER and _WIN32_WINNT to be set to at least "
		"_WIN32_WINNT_WIN10 (0x0A00), and WDK_NTDDI_VERSION set to NTDDI_WIN10_RS1 (0x0A000002)" };
#endif

#if RJD_THREAD_NAME_AVAILABLE && RJD_COMPILER_GCC
	// The headers don't have the declarations for these functions but the imports exist in the libs,
	// so this is a workaround until that bug gets fixed.
	WINBASEAPI HRESULT WINAPI SetThreadDescription(
	    _In_ HANDLE hThread,
	    _In_ PCWSTR lpThreadDescription
	    );

	 
	WINBASEAPI HRESULT WINAPI GetThreadDescription(
	    _In_ HANDLE hThread,
	    _Outptr_result_z_ PWSTR* ppszThreadDescription
	    );
#endif

////////////////////////////////////////////////////////////////////////////////
// interface implementation

struct rjd_thread_id rjd_thread_id_current(void)
{
	struct rjd_thread_id id = {0};
	struct rjd_thread_id_win32* id_win32 = (struct rjd_thread_id_win32*)&id;
	id_win32->id GetCurrentThreadId();
	return id;
}

bool rjd_thread_id_equals(const struct rjd_thread_id a, const struct rjd_thread_id b)
{
	struct rjd_thread_id_win32* a_win32 = (struct rjd_thread_id_win32*)&a;
	struct rjd_thread_id_win32* b_win32 = (struct rjd_thread_id_win32*)&b;
	return a_win32->id == b_win32->id;
}

struct rjd_result rjd_thread_create(struct rjd_thread* thread, struct rjd_thread_desc desc)
{
	struct rjd_thread_win32* thread_win32 = (struct rjd_thread_win32*)thread;

	SECURITY_ATTRIBUTES security = { 
		.nLength = sizeof(SECURITY_ATTRIBUTES),
		.bInheritHandle = false,
	};

	struct rjd_thread_params* params = rjd_mem_alloc(struct rjd_thread_params, desc.allocator);
	params->entrypoint_func = desc.entrypoint_func;
	params->userdata = desc.userdata;

	// Note that there's no way to provide stack memory to CreateThread() on windows, so we just ignore
	// the desc.allocator param on this platform.
	DWORD flags = 0;
	DWORD threadId = 0;
	HANDLE handle = CreateThread(&security, desc.stacksize, rjd_thread_entrypoint_win32, params, flags, &threadId);
	if (handle == NULL) {
		return RJD_RESULT("CreateThread() failed. Call GetLastError() for more info");
	}

	thread_win32->handle = handle;

	if (desc.optional_name) {
		#if RJD_THREAD_NAME_AVAILABLE
			wchar_t widename[32];
			size_t length = mbstowcs(widename, desc.optional_name, rjd_countof(widename) - 1);
			widename[length] = '\0';

			if (FAILED(SetThreadDescription(handle, widename))) {
				return RJD_RESULT("Thread was created, but name was unable to be set. Call GetLastError for more info");
			}
		#else
			return RJD_RESULT_THREADNAME_UNAVAILABLE;
		#endif
	}

	return RJD_RESULT_OK();
}

struct rjd_result rjd_thread_join(struct rjd_thread* thread)
{
	struct rjd_thread_win32* thread_win32 = (struct rjd_thread_win32*)thread;
	DWORD result = WaitForSingleObject(thread_win32->handle, INFINITE);
	if (result == WAIT_FAILED) {
		return RJD_RESULT("Wait failed. Call GetLastError for more info");
	}

	return RJD_RESULT_OK();
}

struct rjd_result rjd_thread_getname(struct rjd_thread* thread, uint32_t destination_max_length, char* out)
{
	#if RJD_THREAD_NAME_AVAILABLE
		struct rjd_thread_win32* thread_win32 = (struct rjd_thread_win32*)thread;

		wchar_t* thread_name = NULL;
		if (FAILED(GetThreadDescription(thread_win32->handle, &thread_name)) || !thread_name) {
			return RJD_RESULT("Unable to get thread name. Call GetLastError for more info");
		}

		size_t length = wcstombs(out, thread_name, destination_max_length - 1);
		out[length] = '\0';

		LocalFree(thread_name);
		
		return RJD_RESULT_OK();	
	#else
		RJD_UNUSED_PARAM(thread);
		if (destination_max_length > 0) {
			out[0] = '\0';
		}
		return RJD_RESULT_THREADNAME_UNAVAILABLE;
	#endif
}

void rjd_thread_sleep(uint32_t seconds)
{
	Sleep(seconds * 1000);
}

struct rjd_result rjd_condvar_create(struct rjd_condvar* condvar)
{
	struct rjd_condvar_win32* condvar_win32 = (struct rjd_condvar_win32*)condvar;
	
	InitializeConditionVariable(&condvar_win32->condition_variable);
	return rjd_rwlock_create(&condvar_win32->lock);
}

struct rjd_result rjd_condvar_destroy(struct rjd_condvar* condvar)
{
	// nothing to do for CONDITION_VARIABLE
	struct rjd_condvar_win32* condvar_win32 = (struct rjd_condvar_win32*)condvar;
	return rjd_rwlock_destroy(&condvar_win32->lock);
}

struct rjd_result rjd_condvar_lock(struct rjd_condvar* condvar)
{
	struct rjd_condvar_win32* condvar_win32 = (struct rjd_condvar_win32*)condvar;
	return rjd_rwlock_acquire_writer(&condvar_win32->lock);
}

struct rjd_result rjd_condvar_unlock(struct rjd_condvar* condvar)
{
	struct rjd_condvar_win32* condvar_win32 = (struct rjd_condvar_win32*)condvar;
	return rjd_rwlock_release_writer(&condvar_win32->lock);
}

struct rjd_result rjd_condvar_signal_single(struct rjd_condvar* condvar)
{
	struct rjd_condvar_win32* condvar_win32 = (struct rjd_condvar_win32*)condvar;
	WakeConditionVariable(&condvar_win32->condition_variable);
	return RJD_RESULT_OK();
}

struct rjd_result rjd_condvar_signal_all(struct rjd_condvar* condvar)
{
	struct rjd_condvar_win32* condvar_win32 = (struct rjd_condvar_win32*)condvar;
	WakeAllConditionVariable(&condvar_win32->condition_variable);
	return RJD_RESULT_OK();
}

struct rjd_result rjd_condvar_wait(struct rjd_condvar* condvar)
{
	return rjd_condvar_wait_timed(condvar, INFINITE);
}

struct rjd_result rjd_condvar_wait_timed(struct rjd_condvar* condvar, uint32_t seconds)
{
	struct rjd_condvar_win32* condvar_win32 = (struct rjd_condvar_win32*)condvar;
	struct rjd_rwlock_win32* lock_win32 = (struct rjd_rwlock_win32*)&condvar_win32->lock;
	RJD_ASSERTMSG(!rjd_thread_id_equals(lock_win32->exclusive_owning_thread, RJD_THREAD_ID_INVALID.id),
		"You must lock the condvar before waiting on it.");

	uint32_t ms = (seconds == INFINITE) ? INFINITE : seconds * 1000;

	lock_win32->exclusive_owning_thread = RJD_THREAD_ID_INVALID;

	if (!SleepConditionVariableSRW(&condvar_win32->condition_variable, &lock_win32->lock, ms, 0)) {
		if (GetLastError() == ERROR_TIMEOUT) {
			return RJD_RESULT("Timed out");
		} else {
			return RJD_RESULT("Unknown failure. Call GetLastError() to know more");
		}
	}

	const struct rjd_thread_id current_thread = rjd_thread_id_current();
	lock_win32->exclusive_owning_thread = current_thread;

	return RJD_RESULT_OK(); 
}

struct rjd_result rjd_lock_create(struct rjd_lock* lock)
{
	struct rjd_lock_win32* lock_win32 = (struct rjd_lock_win32*)lock;

	DWORD SPIN_COUNT = 0;
	DWORD FLAGS = 0; // TODO maybe use CRITICAL_SECTION_NO_DEBUG_INFO in release?

	lock_win32->owning_thread = RJD_THREAD_ID_INVALID;
	if (InitializeCriticalSectionEx(&lock_win32->cs, SPIN_COUNT, FLAGS)) {
		return RJD_RESULT_OK();
	}

	return RJD_RESULT("Win32 API call failed. Call GetLastError() for more info.");
}

struct rjd_result rjd_lock_destroy(struct rjd_lock* lock)
{
	struct rjd_lock_win32* lock_win32 = (struct rjd_lock_win32*)lock;

	if (!rjd_thread_id_equals(RJD_THREAD_ID_INVALID, lock_win32->owning_thread)) {
		return RJD_RESULT("The lock is still in use by a thread");
	}

	DeleteCriticalSection(&lock_win32->cs);
	return RJD_RESULT_OK();
}

struct rjd_result rjd_lock_acquire(struct rjd_lock* lock)
{
	struct rjd_lock_win32* lock_win32 = (struct rjd_lock_win32*)lock;
	const struct rjd_thread_id current_thread = rjd_thread_id_current();

	// posix mutex is non-reentrant but win32 critical section is. To keep functionality the same,
	// we ensure this CS is non-recursive as well.
	if (rjd_thread_id_equals(lock_win32->owning_thread, current_thread)) {
		return RJD_RESULT("Lock recursion detected.");
	}

	EnterCriticalSection(&lock_win32->cs);
	lock_win32->owning_thread = current_thread;
	return RJD_RESULT_OK();
}

struct rjd_result rjd_lock_try_acquire(struct rjd_lock* lock)
{
	struct rjd_lock_win32* lock_win32 = (struct rjd_lock_win32*)lock;
	const struct rjd_thread_id current_thread = rjd_thread_id_current();

	// posix mutex is non-reentrant but win32 critical section is. To keep functionality the same,
	// we ensure this CS is non-recursive as well.
	if (rjd_thread_id_equals(lock_win32->owning_thread, current_thread)) {
		return RJD_RESULT("Lock recursion detected.");
	}

	if (!TryEnterCriticalSection(&lock_win32->cs)) {
		return RJD_RESULT("Lock in use");
	}
	lock_win32->owning_thread = current_thread;
	return RJD_RESULT_OK();
}

struct rjd_result rjd_lock_release(struct rjd_lock* lock)
{
	struct rjd_lock_win32* lock_win32 = (struct rjd_lock_win32*)lock;
	const struct rjd_thread_id current_thread = rjd_thread_id_current();

	if (!rjd_thread_id_equals(lock_win32->owning_thread, current_thread)) {
		return RJD_RESULT("This thread does not own this lock.");
	}

	lock_win32->owning_thread = RJD_THREAD_ID_INVALID;
	LeaveCriticalSection(&lock_win32->cs);
	return RJD_RESULT_OK();
}

struct rjd_result rjd_rwlock_create(struct rjd_rwlock* lock)
{
	struct rjd_rwlock_win32* lock_win32 = (struct rjd_rwlock_win32*)lock;
	InitializeSRWLock(&lock_win32->lock);
	return RJD_RESULT_OK();
}

struct rjd_result rjd_rwlock_destroy(struct rjd_rwlock* lock)
{
	RJD_UNUSED_PARAM(lock);
	return RJD_RESULT_OK();
}

struct rjd_result rjd_rwlock_acquire_reader(struct rjd_rwlock* lock)
{
	struct rjd_rwlock_win32* lock_win32 = (struct rjd_rwlock_win32*)lock;
	AcquireSRWLockShared(&lock_win32->lock);
	return RJD_RESULT_OK();
}

struct rjd_result rjd_rwlock_acquire_writer(struct rjd_rwlock* lock)
{
	struct rjd_rwlock_win32* lock_win32 = (struct rjd_rwlock_win32*)lock;
	const struct rjd_thread_id current_thread = rjd_thread_id_current();
	AcquireSRWLockExclusive(&lock_win32->lock);
	lock_win32->exclusive_owning_thread = current_thread;
	return RJD_RESULT_OK();
}

struct rjd_result rjd_rwlock_try_acquire_reader(struct rjd_rwlock* lock)
{
	struct rjd_rwlock_win32* lock_win32 = (struct rjd_rwlock_win32*)lock;
	if (!TryAcquireSRWLockShared(&lock_win32->lock)) {
		return RJD_RESULT("Lock in use");
	}
	return RJD_RESULT_OK();
}

struct rjd_result rjd_rwlock_try_acquire_writer(struct rjd_rwlock* lock)
{
	struct rjd_rwlock_win32* lock_win32 = (struct rjd_rwlock_win32*)lock;
	const struct rjd_thread_id current_thread = rjd_thread_id_current();
	if (!TryAcquireSRWLockExclusive(&lock_win32->lock)) {
		return RJD_RESULT("Lock in use");
	}
	lock_win32->exclusive_owning_thread = current_thread;
	return RJD_RESULT_OK();
}

struct rjd_result rjd_rwlock_release_reader(struct rjd_rwlock* lock)
{
	struct rjd_rwlock_win32* lock_win32 = (struct rjd_rwlock_win32*)lock;
	ReleaseSRWLockShared(&lock_win32->lock);
	return RJD_RESULT_OK();
}

struct rjd_result rjd_rwlock_release_writer(struct rjd_rwlock* lock)
{
	struct rjd_rwlock_win32* lock_win32 = (struct rjd_rwlock_win32*)lock;
	const struct rjd_thread_id current_thread = rjd_thread_id_current();

	if (!rjd_thread_id_equals(lock_win32->exclusive_owning_thread, current_thread)) {
		return RJD_RESULT("This thread does not own this lock.");
	}
	lock_win32->exclusive_owning_thread = RJD_THREAD_ID_INVALID;
	ReleaseSRWLockExclusive(&lock_win32->lock);
	return RJD_RESULT_OK();
}

////////////////////////////////////////////////////////////////////////////////
// local helpers

DWORD WINAPI rjd_thread_entrypoint_win32(LPVOID lpParameter)
{
	struct rjd_thread_params params = *(struct rjd_thread_params*)lpParameter;
	rjd_mem_free(lpParameter);

	params.entrypoint_func(params.userdata);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#elif RJD_PLATFORM_OSX

#include <pthread.h>
#include <unistd.h> // sleep
#include <limits.h> // PTHREAD_STACK_MIN
#include <sys/errno.h>
#include <sys/time.h> // gettimeofday

struct rjd_thread_id_osx
{
	pthread_t handle;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_thread_id_osx) <= sizeof(struct rjd_thread_id));

struct rjd_thread_osx
{
	pthread_t handle;
	void* stack;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_thread_osx) <= sizeof(struct rjd_thread_platform));

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
	struct rjd_thread_id exclusive_owning_thread;
	bool is_exclusively_locked;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_rwlock_osx) <= sizeof(struct rjd_rwlock));

RJD_STATIC_ASSERT(RJD_THREAD_STACKSIZE_DEFAULT >= PTHREAD_STACK_MIN);

static inline struct rjd_thread_id rjd_thread_id_from_pthread(pthread_t handle);
static inline struct rjd_thread_osx* rjd_thread_get_osx(struct rjd_thread* thread);
static inline struct rjd_condvar_osx* rjd_condvar_get_osx(struct rjd_condvar* condvar);
static inline struct rjd_lock_osx* rjd_lock_get_osx(struct rjd_lock* lock);
static inline struct rjd_rwlock_osx* rjd_rwlock_get_osx(struct rjd_rwlock* rwlock);

struct rjd_result rjd_error_to_result(int error);
static void* rjd_thread_entrypoint_osx(void* userdata);

////////////////////////////////////////////////////////////////////////////////
// interface implementation

struct rjd_thread_id rjd_thread_id_current(void)
{
	struct rjd_thread_id id = {0};
	struct rjd_thread_id_osx* id_osx = (struct rjd_thread_id_osx*)&id;
	id_osx->handle = pthread_self();
	return id;
}

bool rjd_thread_id_equals(const struct rjd_thread_id a, const struct rjd_thread_id b)
{
	struct rjd_thread_id_osx* a_osx = (struct rjd_thread_id_osx*)&a;
	struct rjd_thread_id_osx* b_osx = (struct rjd_thread_id_osx*)&b;
	return pthread_equal(a_osx->handle, b_osx->handle);
}

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
	if (*desc.optional_name) {
		strncpy(params->name, desc.optional_name, RJD_THREAD_NAME_MAX_LENGTH);
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
                thread->id = rjd_thread_id_from_pthread(thread_osx->handle);
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

    int error = pthread_cond_signal(&condvar_osx->condition_variable);
    struct rjd_result result = rjd_error_to_result(error);
	return result;
}

struct rjd_result rjd_condvar_signal_all(struct rjd_condvar* condvar)
{
	struct rjd_condvar_osx* condvar_osx = rjd_condvar_get_osx(condvar);

    int error = pthread_cond_broadcast(&condvar_osx->condition_variable);
    struct rjd_result result = rjd_error_to_result(error);
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
	if (rjd_result_isok(result)) {
		lock_osx->is_exclusively_locked = true;
		lock_osx->exclusive_owning_thread = rjd_thread_id_current();
	}
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
	if (rjd_result_isok(result)) {
		lock_osx->is_exclusively_locked = true;
		lock_osx->exclusive_owning_thread = rjd_thread_id_current();
	}
	return result;
}

struct rjd_result rjd_rwlock_release_reader(struct rjd_rwlock* lock)
{
	struct rjd_rwlock_osx* lock_osx = rjd_rwlock_get_osx(lock);

    if (lock_osx->is_exclusively_locked) {
        const struct rjd_thread_id current_thread = rjd_thread_id_current();
        if (rjd_thread_id_equals(current_thread, lock_osx->exclusive_owning_thread)) {
			return RJD_RESULT("This is under a writer lock. Release via writer instead.");
		} else {
            return RJD_RESULT("This thread does not own this lock");
        }
    }
    
	int error = pthread_rwlock_unlock(&lock_osx->lock);
	struct rjd_result result = rjd_error_to_result(error);
	return result;
}

struct rjd_result rjd_rwlock_release_writer(struct rjd_rwlock* lock)
{
	struct rjd_rwlock_osx* lock_osx = rjd_rwlock_get_osx(lock);
	if (lock_osx->is_exclusively_locked) {
		const struct rjd_thread_id current_thread = rjd_thread_id_current();
		if (!rjd_thread_id_equals(current_thread, lock_osx->exclusive_owning_thread)) {
			return RJD_RESULT("This thread does not own this lock");
		}
	}
    
    lock_osx->is_exclusively_locked = false;
	
	int error = pthread_rwlock_unlock(&lock_osx->lock);
	struct rjd_result result = rjd_error_to_result(error);
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// Local helper implementation

static inline struct rjd_thread_id rjd_thread_id_from_pthread(pthread_t handle)
{
    struct rjd_thread_id thread_id = {0};
    struct rjd_thread_id_osx* id_osx = (struct rjd_thread_id_osx*)&thread_id;
    id_osx->handle = handle;
    return thread_id;
}
                  
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
