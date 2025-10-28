// Copyright 2025 XavierHarkonnen9 and Enlarium
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Don't want to include platform.h yet because it will stop us from using _GNU_SOURCE
#if defined(__linux__) && !defined(__ANDROID__)
#define _GNU_SOURCE
#endif

#include <descent/thread/thread.h>

#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <unistd.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <processthreadsapi.h>
#include <windows.h>
#endif

#include <descent/thread/atomic.h>
#include <descent/utilities/debug.h>

#include "thread_handle.h"



// Thread State Progression:
// UNUSED --choose-> RESERVED --wrapper(init)-> RUNNING --wrapper(exit)-> FINISHED --join/detach-> UNUSED
// RUNNING --detach-> DETACHED --wrapper(exit)-> UNUSED



#ifndef DESCENT_MAX_THREADS
#define DESCENT_MAX_THREADS 32
#endif

// Any unmanaged threads will be parsed as invalid, so they can't be joined or detached
#define THREAD_SELF_UNMANAGED    ((Thread) 0xFFFFFFFFFFFFFFFF)
#define THREAD_NAME_LENGTH 16
#define THREAD_META_INVALID (((uint64_t) THREAD_STATE_INVALID) << 32 | UINT32_MAX)

#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#define THREAD_WRAPPER_SIGNATURE void *
#define THREAD_WRAPPER_RETURN (void*)(intptr_t)
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#define THREAD_WRAPPER_SIGNATURE unsigned __stdcall
#define THREAD_WRAPPER_RETURN (unsigned)
#endif



typedef enum {
	THREAD_STATE_INVALID = -1,
	THREAD_STATE_UNUSED = 0,
	THREAD_STATE_RESERVED,
	THREAD_STATE_RUNNING,
	THREAD_STATE_FINISHED,
	THREAD_STATE_DETACHED,
	THREAD_STATE_JOINING
} ThreadState;

typedef struct {
	char                  name[THREAD_NAME_LENGTH];
	atomic_thread_handle  handle;
	ThreadFunction        function;
	void                 *argument;
	atomic_64             meta;
	uint64_t              affinity;
	int                   priority;
} ThreadContext;



static ThreadContext thread_pool[DESCENT_MAX_THREADS] = {0};

// All managed threads start as unmanaged, but update themselves when they initialize
static TLS Thread self = THREAD_SELF_UNMANAGED;



static inline uint32_t thread_index(Thread t) {
	return (uint32_t) t;
}

static inline uint32_t thread_generation(Thread t) {
	return (uint32_t) (t >> 32);
}

static inline Thread thread_construct(uint32_t index, uint32_t generation) {
	return ((Thread) generation << 32) | (Thread) index;
}

static inline int thread_valid(Thread t) {
	uint32_t index = thread_index(t);
	return (index < DESCENT_MAX_THREADS);
}

static inline uint32_t thread_context_generation(uint64_t meta) {
	return (uint32_t) meta;
}

static inline uint32_t thread_context_state(uint64_t meta) {
	return (uint32_t) (meta >> 32);
}

static inline uint64_t thread_context_construct(uint32_t state, uint32_t generation) {
	return (((uint64_t) state) << 32 | generation);
}

// Strips "THREAD_STATE_" prefix from thread_state function
static inline const char *thread_state_short(int s) {
	// The null terminator "stands in" for the last underscore
	return thread_state(s) + sizeof("THREAD_STATE");
}

static inline ThreadContext *choose_thread_context(void) {
	for (int i = 0; i < DESCENT_MAX_THREADS; ++i) {
		ThreadContext *context = &thread_pool[i];

		for (;;) {
			uint64_t meta       = atomic_load_64(&context->meta);
			uint32_t state      = thread_context_state(meta);
			uint32_t generation = thread_context_generation(meta);

			if (state != THREAD_STATE_UNUSED) break;

			uint64_t new_meta = thread_context_construct(THREAD_STATE_RESERVED, generation);

			if (atomic_compare_exchange_64(&context->meta, &meta, new_meta)) return context;

			thread_pause();
		}
	}

	return NULL;
}

static inline void thread_set_name(const char *name) {
	if (name) {
		debug_thread_log(__func__, "[%016" PRIX64 "] setting name to %s", thread_self(), name);

#if defined(DESCENT_PLATFORM_LINUX)

		pthread_setname_np(pthread_self(), name);

#elif defined(DESCENT_PLATFORM_FREEBSD)

		pthread_set_name_np(pthread_self(), name);

#elif defined(DESCENT_PLATFORM_MACOS)
		pthread_setname_np(name);

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

		wchar_t wide_name[THREAD_NAME_LENGTH] = {};
		if(mbstowcs_s(NULL, wide_name, THREAD_NAME_LENGTH, name, THREAD_NAME_LENGTH - 1)) wide_name[0] = '\0';
		wide_name[THREAD_NAME_LENGTH - 1] = '\0';

		typedef HRESULT (WINAPI *SetThreadDescriptionFunc)(HANDLE, PCWSTR);
		SetThreadDescriptionFunc pSetThreadDescription = NULL;
		if (!pSetThreadDescription) pSetThreadDescription = (SetThreadDescriptionFunc)GetProcAddress(GetModuleHandle(L"Kernel32.dll"), "SetThreadDescription");
		if (pSetThreadDescription) pSetThreadDescription(GetCurrentThread(), wide_name);

#endif

	}
}

static inline void thread_set_affinity(uint64_t affinity) {
	if (affinity) {
		debug_thread_log(__func__, "[%016" PRIX64 "] setting affinity to %zu", thread_self(), affinity);

#if defined(DESCENT_PLATFORM_LINUX) || defined(DESCENT_PLATFORM_FREEBSD)

		cpu_set_t cpuset;
		CPU_ZERO(&cpuset);

		for (unsigned i = 0; i < sizeof(affinity) * 8; ++i) if (affinity & (1ULL << i)) CPU_SET(i, &cpuset);

		pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

#elif defined(DESCENT_PLATFORM_MACOS)

	// No thread affinity analogue

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

		SetThreadAffinityMask(GetCurrentThread(), (DWORD_PTR)affinity);
#endif

	}
}

static inline void thread_set_priority(int priority) {
	if (priority) {
		debug_thread_log(__func__, "[%016" PRIX64 "] setting priority to %d", thread_self(), priority);
		
		int result = 0;

#if defined(DESCENT_PLATFORM_LINUX)

		struct sched_param param = {0};
		switch (priority) {
			case THREAD_PRIORITY_LOW:
				result = pthread_setschedparam(pthread_self(), SCHED_BATCH, &param);
				break;
			case THREAD_PRIORITY_DEFAULT:
				result = pthread_setschedparam(pthread_self(), SCHED_OTHER, &param);
				break;
			case THREAD_PRIORITY_HIGH:
				result = pthread_setschedparam(pthread_self(), SCHED_OTHER, &param);
				break;
		}

#elif defined(DESCENT_PLATFORM_FREEBSD)

	// No thread priority analogue

#elif defined(DESCENT_PLATFORM_MACOS)

	switch (priority) {
		case THREAD_PRIORITY_LOW:
			result = pthread_set_qos_class_self_np(QOS_CLASS_UTILITY, 0);
			break;
		case THREAD_PRIORITY_DEFAULT:
			result = pthread_set_qos_class_self_np(QOS_CLASS_DEFAULT, 0);
			break;
		case THREAD_PRIORITY_HIGH:
			result = pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0);
			break;
	}

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

		switch (priority) {
			case THREAD_PRIORITY_LOW:
				result = !SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
				break;
			case THREAD_PRIORITY_DEFAULT:
				result = !SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
				break;
			case THREAD_PRIORITY_HIGH:
				result = !SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
				break;
		}
#endif

		if (result) debug_thread_log(__func__, "[%016" PRIX64 "] could not set priority to %d", thread_self(), priority);

	}
}



static THREAD_WRAPPER_SIGNATURE thread_function_wrapper(void *p) {
	// ThreadContext's static fields are not to be accessed by any function other than this
	// or thread_create, and thread_create cannot modify a context while it is running.

	ThreadContext *context = (ThreadContext*)p;

	uint64_t meta = atomic_load_64(&context->meta);
	uint32_t generation = thread_context_generation(meta);

	self = thread_construct((uint32_t) (context - thread_pool), generation);

	debug_thread_log(__func__, "[%016" PRIX64 "] called", thread_self());

	thread_set_name(context->name);
	thread_set_affinity(context->affinity);
	thread_set_priority(context->priority);

	atomic_store_64(&context->meta, thread_context_construct(THREAD_STATE_RUNNING, generation));

	int result = context->function(context->argument);

	// Clear fields before closing
	context->name[0] = '\0';
	context->function = NULL;
	context->argument = NULL;
	context->affinity = 0;
	context->priority = 0;

	// Check if the thread is still in the default running state first
	// If THREAD_STATE_RUNNING, go to THREAD_STATE_FINISHED
	// Expect THREAD_STATE_RUNNING (testing) on the current generation (certain)
	meta = thread_context_construct(THREAD_STATE_RUNNING, generation);
	if (atomic_compare_exchange_64(&context->meta, &meta, thread_context_construct(THREAD_STATE_FINISHED, generation))) {
		debug_thread_log(__func__, "[%016" PRIX64 "] set state from RUNNING to FINISHED", thread_self());
	} else {
		debug_thread_log(__func__, "[%016" PRIX64 "] state is %s, skipping transition to FINISHED", thread_self(), thread_state_short(thread_context_state(meta)));
	}
	
	// The thread can still be detached while finished, check this second
	// If THREAD_STATE_DETACHED, go straight to THREAD_STATE_UNUSED and increment generation
	// Expect THREAD_STATE_DETACHED (testing) on the current generation (certain)
	meta = thread_context_construct(THREAD_STATE_DETACHED, generation);
	if (atomic_compare_exchange_64(&context->meta, &meta, thread_context_construct(THREAD_STATE_UNUSED, generation + 1))) {
		debug_thread_log(__func__, "[%016" PRIX64 "] set state from DETACHED to UNUSED, incremented generation", thread_self());
	} else {
		debug_thread_log(__func__, "[%016" PRIX64 "] state is %s, cleanup deferred", thread_self(), thread_state_short(thread_context_state(meta)));
	}
	
	return THREAD_WRAPPER_RETURN result;
}



int thread_max_concurrent(void) {
	return DESCENT_MAX_THREADS;
}

Thread thread_self(void) {
	return self;
}

const char* thread_name(void) {
	if (self == THREAD_SELF_UNMANAGED) return "Unmanaged Thread";
	return thread_pool[thread_index(self)].name;
}

const char *thread_state(int s) {
	switch (s) {
		case THREAD_STATE_UNUSED:   return "THREAD_STATE_UNUSED";
		case THREAD_STATE_RESERVED: return "THREAD_STATE_RESERVED";
		case THREAD_STATE_RUNNING:  return "THREAD_STATE_RUNNING";
		case THREAD_STATE_FINISHED: return "THREAD_STATE_FINISHED";
		case THREAD_STATE_DETACHED: return "THREAD_STATE_DETACHED";
		case THREAD_STATE_JOINING:  return "THREAD_STATE_JOINING";
		default:                    return "THREAD_STATE_INVALID";
	}
}

const char *thread_error(int e) {
	switch (e) {
		case THREAD_SUCCESS:                return "THREAD_SUCCESS";
		case THREAD_ERROR_NO_SLOTS:         return "THREAD_ERROR_NO_SLOTS";
		case THREAD_ERROR_HANDLE_INVALID:   return "THREAD_ERROR_HANDLE_INVALID";
		case THREAD_ERROR_HANDLE_UNMANAGED: return "THREAD_ERROR_HANDLE_UNMANAGED";
		case THREAD_ERROR_HANDLE_DETACHED:  return "THREAD_ERROR_HANDLE_DETACHED";
		case THREAD_ERROR_HANDLE_CLOSED:    return "THREAD_ERROR_HANDLE_CLOSED";
		case THREAD_ERROR_FUNCTION_INVALID: return "THREAD_ERROR_FUNCTION_INVALID";
		case THREAD_ERROR_OS_CREATE_FAILED: return "THREAD_ERROR_OS_CREATE_FAILED";
		case THREAD_ERROR_OS_JOIN_FAILED:   return "THREAD_ERROR_OS_JOIN_FAILED";
		case THREAD_ERROR_OS_DETACH_FAILED: return "THREAD_ERROR_OS_DETACH_FAILED";
		default:                            return "THREAD_ERROR_UNKNOWN";
	}
}

int thread_create(Thread *t, ThreadFunction f, void *arg) {
	return thread_create_attr(t, f, arg, NULL);
}

int thread_create_attr(Thread *t, ThreadFunction f, void *arg, const ThreadAttributes *a) {
	debug_thread_log(__func__, "[%016" PRIX64 "] called", thread_self());

	if (!t) {
		debug_thread_log(__func__, "[%016" PRIX64 "] caller provided null thread ", thread_self());
		return THREAD_ERROR_HANDLE_INVALID;
	}

	if (!f) {
		debug_thread_log(__func__, "[%016" PRIX64 "] caller provided null function", thread_self());
		return THREAD_ERROR_FUNCTION_INVALID;
	}

	ThreadContext *context = choose_thread_context();
	if (!context) {
		debug_thread_log(__func__, "[%016" PRIX64 "] out of thread slots", thread_self());
		return THREAD_ERROR_NO_SLOTS;
	}
	debug_thread_log(__func__, "[%016" PRIX64 "] reserved context at index %u, set state to RESERVED", thread_self(), (uint32_t) (context - thread_pool));

	ThreadAttributes attributes = {0};

	if (a) attributes = *a;

	if (attributes.name) {
		// Truncate names to THREAD_NAME_LENGTH - 1 characters
		strncpy(context->name, attributes.name, THREAD_NAME_LENGTH - 1);
		context->name[THREAD_NAME_LENGTH - 1] = '\0';
	} else {
		context->name[0] = '\0';
	}

	context->function = f;
	context->argument = arg;
	context->affinity = attributes.affinity;
	context->priority = attributes.priority;

	thread_handle handle;

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

	pthread_attr_t pthread_attributes;
	pthread_attr_init(&pthread_attributes);

	if (attributes.stack_size) {
		// Round up to minimum size
		size_t stack_size = (size_t) ((long) attributes.stack_size > PTHREAD_STACK_MIN ? attributes.stack_size : PTHREAD_STACK_MIN);

		// Round up to page size
		long page_size = sysconf(_SC_PAGESIZE);
		stack_size = (stack_size + page_size - 1) & ~(page_size - 1);

		pthread_attr_setstacksize(&pthread_attributes, stack_size);

		debug_thread_log(__func__, "[%016" PRIX64 "] set stack size to %zu", thread_self(), stack_size);
	}

	int result = pthread_create(&handle, &pthread_attributes, thread_function_wrapper, context);

	pthread_attr_destroy(&pthread_attributes);

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

	if (attributes.stack_size) debug_thread_log(__func__, "[%016" PRIX64 "] set stack size to %u", thread_self(), stack_size);

	handle = (thread_handle) _beginthreadex(NULL, attributes.stack_size, thread_function_wrapper, context, 0, NULL);
	int result = !handle;
	
#endif

	uint64_t meta = atomic_load_64(&context->meta);
	uint32_t generation = thread_context_generation(meta);

	if (result) {
		// Reset the slot: state = UNUSED, generation unchanged
		atomic_store_64(&context->meta, thread_context_construct(THREAD_STATE_UNUSED, generation));
		debug_thread_log(__func__, "[%016" PRIX64 "] OS could not create thread", thread_self());
		return THREAD_ERROR_OS_CREATE_FAILED;
	}

	atomic_store_thread_handle(&context->handle, handle);

	*t = thread_construct((uint32_t) (context - thread_pool), generation);

	debug_thread_log(__func__, "[%016" PRIX64 "] created thread %016" PRIX64 "", thread_self(), *t);
	return 0;
}

void thread_exit(int code) {
	debug_thread_log(__func__, "[%016" PRIX64 "] called with code %d", thread_self(), code);

#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	pthread_exit((void *)(intptr_t)code);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	_endthreadex((unsigned)code);
#endif
}

int thread_join(Thread t, int *code) {
	debug_thread_log(__func__, "[%016" PRIX64 "] called on thread %016" PRIX64 "", thread_self(), t);

	if (t == THREAD_SELF_UNMANAGED) {
		debug_thread_log(__func__, "[%016" PRIX64 "] cannot join unmanaged thread", thread_self());
		return THREAD_ERROR_HANDLE_UNMANAGED;
	}

	if (!thread_valid(t)) {
		debug_thread_log(__func__, "[%016" PRIX64 "] provided invalid thread", thread_self());
		return THREAD_ERROR_HANDLE_INVALID;
	}

	uint32_t index = thread_index(t);
	uint32_t expected_generation = thread_generation(t);
	ThreadContext *context = &thread_pool[index];

	thread_handle handle = atomic_load_thread_handle(&context->handle);

	// Load meta AFTER handle - meta will always be at least as new as the handle.
	for (;;) {
		uint64_t meta       = atomic_load_64(&context->meta);
		uint32_t state      = thread_context_state(meta);
		uint32_t generation = thread_context_generation(meta);

		// If the generation of the index is greater than the generation of the handle,
		// the handle refers to an thread that has been closed.
		if (generation > expected_generation) {
			debug_thread_log(__func__, "[%016" PRIX64 "] thread %016" PRIX64 " is already closed", thread_self(), t);
			return THREAD_ERROR_HANDLE_CLOSED;
		}

		// If the generation of the index is less than the generation of the handle,
		// the handle refers to a "future" thread. This indicates that the handle
		// has been tampered with.
		if (generation < expected_generation) {
			debug_thread_log(__func__, "[%016" PRIX64 "] thread %016" PRIX64 " refers to future thread", thread_self(), t);
			return THREAD_ERROR_HANDLE_INVALID;
		}

		switch (state) {
			case THREAD_STATE_RESERVED:
				// Wait until the thread is initialized to join it
				continue;
			case THREAD_STATE_RUNNING: // Fall through
			case THREAD_STATE_FINISHED: {
				uint64_t desired = thread_context_construct(THREAD_STATE_JOINING, generation);
				if (atomic_compare_exchange_64(&context->meta, &meta, desired)) {
					debug_thread_log(__func__, "[%016" PRIX64 "] set state of %016" PRIX64 " to JOINING", thread_self(), t);
					goto thread_join_state_complete;
				} else {
					debug_thread_log(__func__, "[%016" PRIX64 "] could not set state of %016" PRIX64 " to JOINING, retrying", thread_self(), t);
					continue;
				}
			}
			case THREAD_STATE_DETACHED:
					debug_thread_log(__func__, "[%016" PRIX64 "] thread %016" PRIX64 " is already detached", thread_self(), t);
				return THREAD_ERROR_HANDLE_DETACHED;
			default: // INVALID (impossible), JOINING, UNUSED
					debug_thread_log(__func__, "[%016" PRIX64 "] thread %016" PRIX64 " is already closed", thread_self(), t);
				return THREAD_ERROR_HANDLE_CLOSED;
		}

		thread_pause();
	}

	thread_join_state_complete:;

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

	void *exit_code;
	int error = pthread_join(handle, &exit_code);
	if (error) {
		debug_thread_log(__func__, "[%016" PRIX64 "] OS could not join thread %016" PRIX64 "", thread_self(), t);
		return THREAD_ERROR_OS_JOIN_FAILED;
	}

	if (code) *code = (int)(intptr_t)exit_code;

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

	DWORD exit_code;
	if (
		(WaitForSingleObject(handle, INFINITE) != WAIT_OBJECT_0) ||
		!GetExitCodeThread(handle, &exit_code)
	) {
		debug_thread_log(__func__, "[%016" PRIX64 "] OS could not join thread %016" PRIX64 "", thread_self(), t);
		return THREAD_ERROR_OS_JOIN_FAILED;
	}

	// Even if this fails, the thread has been joined
	CloseHandle(handle);

	if (code) *code = (int)exit_code;

#endif

	uint64_t new_meta = thread_context_construct(THREAD_STATE_UNUSED, expected_generation + 1);
	atomic_store_64(&context->meta, new_meta);
	debug_thread_log(__func__, "[%016" PRIX64 "] closed thread %016" PRIX64 "", thread_self(), t);

	return 0;
}

int thread_detach(Thread t) {
	debug_thread_log(__func__, "[%016" PRIX64 "] called on thread %016" PRIX64 "", thread_self(), t);

	if (t == THREAD_SELF_UNMANAGED) {
		debug_thread_log(__func__, "[%016" PRIX64 "] cannot detach unmanaged thread", thread_self());
		return THREAD_ERROR_HANDLE_UNMANAGED;
	}

	if (!thread_valid(t)) {
		debug_thread_log(__func__, "[%016" PRIX64 "] provided invalid thread", thread_self());
		return THREAD_ERROR_HANDLE_INVALID;
	}

	uint32_t index = thread_index(t);
	uint32_t expected_generation = thread_generation(t);
	ThreadContext *context = &thread_pool[index];

	thread_handle handle = atomic_load_thread_handle(&context->handle);

	// Load meta AFTER handle - meta will always be at least as new as the handle.
	int finished = 0;
	for (;;) {
		uint64_t meta       = atomic_load_64(&context->meta);
		uint32_t state      = thread_context_state(meta);
		uint32_t generation = thread_context_generation(meta);

		// If the generation of the index is greater than the generation of the handle,
		// the handle refers to an thread that has been closed.
		if (generation > expected_generation) {
			debug_thread_log(__func__, "[%016" PRIX64 "] thread %016" PRIX64 " is already closed", thread_self(), t);
			return THREAD_ERROR_HANDLE_CLOSED;
		}

		// If the generation of the index is less than the generation of the handle,
		// the handle refers to a "future" thread. This indicates that the handle
		// has been tampered with.
		if (generation < expected_generation) {
			debug_thread_log(__func__, "[%016" PRIX64 "] thread %016" PRIX64 " refers to future thread", thread_self(), t);
			return THREAD_ERROR_HANDLE_INVALID;
		}

		switch (state) {
			case THREAD_STATE_RESERVED:
				// Wait until the thread is initialized to detach it
				continue;
			case THREAD_STATE_FINISHED: finished = 1; // Fall through
			case THREAD_STATE_RUNNING: {
				uint64_t desired = thread_context_construct(THREAD_STATE_DETACHED, generation);
				if (atomic_compare_exchange_64(&context->meta, &meta, desired)) {
					debug_thread_log(__func__, "[%016" PRIX64 "] set state of %016" PRIX64 " to DETACHED", thread_self(), t);
					goto thread_detach_state_complete;
				} else {
					debug_thread_log(__func__, "[%016" PRIX64 "] could not set state of %016" PRIX64 " to DETACHED, retrying", thread_self(), t);
					continue;
				}
			}
			case THREAD_STATE_DETACHED:
					debug_thread_log(__func__, "[%016" PRIX64 "] thread %016" PRIX64 " is already detached", thread_self(), t);
				return THREAD_ERROR_HANDLE_DETACHED;
			default: // INVALID (impossible), JOINING, UNUSED
					debug_thread_log(__func__, "[%016" PRIX64 "] thread %016" PRIX64 " is already closed", thread_self(), t);
				return THREAD_ERROR_HANDLE_CLOSED;
		}

		thread_pause();
	}

	thread_detach_state_complete:;

#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	if (pthread_detach(handle)) {
		debug_thread_log(__func__, "[%016" PRIX64 "] OS could not detach thread %016" PRIX64 "", thread_self(), t);
		return THREAD_ERROR_OS_DETACH_FAILED;
	}
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	if (!CloseHandle(handle)) {
		debug_thread_log(__func__, "[%016" PRIX64 "] OS could not detach thread %016" PRIX64 "", thread_self(), t);
		return THREAD_ERROR_OS_DETACH_FAILED;
	}
#endif

	if (finished) {
		uint64_t new_meta = thread_context_construct(THREAD_STATE_UNUSED, expected_generation + 1);
		atomic_store_64(&context->meta, new_meta);
		debug_thread_log(__func__, "[%016" PRIX64 "] closed thread %016" PRIX64 ", set state to UNUSED", thread_self(), t);
	}

	return 0;
}

int thread_equal(Thread t1, Thread t2) {
	return (t1 == t2) && (thread_valid(t1) || (t1 == THREAD_SELF_UNMANAGED));
}

// Functions below are unorganized or unfinished

// Replace this function with one that can busywait for smaller adjustments.
int thread_sleep(unsigned long long milliseconds) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	struct timespec duration;
	duration.tv_sec  = (time_t)(milliseconds / 1000);
	duration.tv_nsec = (long)((milliseconds % 1000) * 1000000ULL);
	return !!nanosleep(&duration, NULL);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	Sleep((DWORD)milliseconds);
	return 0;
#endif
}

/* Ways to Wait
< 1 µs:
- NOP chains

1 µs – 10 µs
- _mm_pause: x86 SSE2
- wfe, yield: ARM
- pause, yield: RISC-V


µs–10ms:
- clock_nanosleep(CLOCK_MONOTONIC, ...)
- SetWaitableTimer

>10ms:
- clock_nanosleep(CLOCK_MONOTONIC, ...)
- WaitForSingleObject()

Linux:
prctl(PR_SET_TIMERSLACK, 1, 0, 0, 0) to increase timer granularity?
*/