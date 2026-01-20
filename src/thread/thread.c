/* Copyright 2025 XavierHarkonnen9 and Enlarium
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_LINUX)
// Needed for pthread_setname_np on Linux
#define _GNU_SOURCE
#endif

#include <descent/thread/thread.h>
#include <intern/thread/thread.h>

#include <stdint.h>

#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <pthread.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <descent/rcode.h>
#include <descent/string/nchar.h>
#include <descent/string/utf_8.h>
#include <descent/thread/atomic.h>
#include <descent/thread/tls.h>
#include <intern/thread/call_once_u.h>
#include <intern/thread/tid.h>

#if defined(DESCENT_PLATFORM_TYPE_POSIX)
typedef pthread_t thread_handle;
#define THREAD_WRAPPER_SIGNATURE void *
#define THREAD_WRAPPER_RETURN NULL
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
typedef HANDLE thread_handle;
#define THREAD_WRAPPER_SIGNATURE unsigned __stdcall
#define THREAD_WRAPPER_RETURN 0
#endif

struct Thread {
	thread_handle handle;
	nchar         name[DESCENT_THREAD_NAME_SIZE];
	int         (*function)(void *);
	void         *argument;
	thread_id     id;
	atomic_int    state;
	atomic_int    code;
};

static struct Thread unique[DESCENT_UNIQUE_THREAD_COUNT_MAX] = {0};
static struct Thread worker[DESCENT_WORKER_THREAD_COUNT_MAX] = {0};
static unsigned int worker_count = 0;

static inline rcode thread_set_name(const nchar *name) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	// Supported on Linux and FreeBSD, our only two POSIX targets
	int result = pthread_setname_np(pthread_self(), name);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	int result = FAILED(SetThreadDescription(GetCurrentThread(), name));
#endif
	return result ? DESCENT_ERROR_OS : 0;
}

static THREAD_WRAPPER_SIGNATURE thread_wrapper(void *p) {
	struct Thread *thread = (struct Thread *)p;

	// We don't particularly care if thread naming fails
	(void) thread_set_name(thread->name);

	// We do care if a TID assignment fails, since it's necessary for a lot of
	// library operations
	rcode tid_result = tid_assign(thread->id);
	if (tid_result) {
		atomic_store_int(&thread->code, tid_result, ATOMIC_RELEASE);
		atomic_store_int(&thread->state, THREAD_STATE_INCOMPLETE, ATOMIC_RELEASE);
		return THREAD_WRAPPER_RETURN;
	}

	// Mark thread as running
	atomic_store_int(&thread->state, THREAD_STATE_RUNNING, ATOMIC_RELEASE);

	// Run provided function
	int result = thread->function(thread->argument);

	// Mark thread as complete
	atomic_store_int(&thread->code, result, ATOMIC_RELEASE);
	atomic_store_int(&thread->state, THREAD_STATE_FINISHED, ATOMIC_RELEASE);
	
	return THREAD_WRAPPER_RETURN;
}

static inline rcode thread_spawn(
	struct Thread *thread,
	int (*function)(void *),
	void *argument,
	const char *name,
	thread_id id
) {
	if (!thread || !function) return DESCENT_ERROR_NULL;
	
	// Only the main thread has permission to call this function
	if (!tid_is_self(TID_MAIN)) return DESCENT_ERROR_FORBIDDEN;

	// Do not allow spawning in active slots.
	// Only unused and incomplete thread slots can be spawned.
	switch (atomic_load_int(&thread->state, ATOMIC_ACQUIRE)) {
		case THREAD_STATE_STARTING:
		case THREAD_STATE_RUNNING:
		case THREAD_STATE_FINISHED:
			return THREAD_ERROR_ACTIVE;
	}

	// Create a native thread name if possible
	char tname[DESCENT_THREAD_NAME_SIZE] = {0};

	if (utf8_copy_truncate(DESCENT_THREAD_NAME_SIZE, tname, name)) {
		chars_to_nchars(DESCENT_THREAD_NAME_SIZE, thread->name, NULL, "D-THREAD", 8);
	}
	else if (chars_to_nchars(DESCENT_THREAD_NAME_SIZE, thread->name, NULL, tname, 0)) {
		chars_to_nchars(DESCENT_THREAD_NAME_SIZE, thread->name, NULL, "D-THREAD", 8);
	}
	
	thread->function = function;
	thread->argument = argument;
	thread->id       = id;
	
	atomic_store_int(&thread->state, THREAD_STATE_STARTING, ATOMIC_RELEASE);
	atomic_store_int(&thread->code, 0, ATOMIC_RELEASE);

#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	int result = pthread_create(&thread->handle, NULL, thread_wrapper, thread);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	thread->handle = (thread_handle) _beginthreadex(NULL, 0, thread_wrapper, thread, 0, NULL);
	int result = !thread->handle;
#endif

	if (result) {
		atomic_store_int(&thread->state, THREAD_STATE_INCOMPLETE, ATOMIC_RELEASE);
		atomic_store_int(&thread->code, DESCENT_ERROR_OS, ATOMIC_RELEASE);
		return DESCENT_ERROR_OS;
	}

	return 0;
}

static inline int thread_collect(struct Thread *thread) {
	if (!thread) return DESCENT_ERROR_NULL;

	// Only the main thread has permission to call this function
	if (!tid_is_self(TID_MAIN)) return DESCENT_ERROR_FORBIDDEN;

	// Handle states that do not require joining
	switch (atomic_load_int(&thread->state, ATOMIC_ACQUIRE)) {
		case THREAD_STATE_UNUSED:
		case THREAD_STATE_INCOMPLETE:
			return THREAD_ERROR_INACTIVE;
	}

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

	int result = pthread_join(thread->handle, NULL);
	if (result) return DESCENT_ERROR_OS;

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

	DWORD ignore_code;
	if (
		(WaitForSingleObject(thread->handle, INFINITE) != WAIT_OBJECT_0) ||
		!GetExitCodeThread(thread->handle, &ignore_code)
	) return DESCENT_ERROR_OS;

	// Even if this fails, the thread has been joined
	// OS leaks are beyond our ability to address
	CloseHandle(thread->handle);

#endif

	atomic_store_int(&thread->state, THREAD_STATE_UNUSED, ATOMIC_RELEASE);
	atomic_store_int(&thread->code, 0, ATOMIC_RELEASE);

	return 0;
}

static inline int thread_state(struct Thread *thread) {
	if (!thread) return THREAD_STATE_INVALID;

	// Only the main thread has permission to call this function
	if (!tid_is_self(TID_MAIN)) return THREAD_STATE_INVALID;

	return (int) atomic_load_int(&thread->state, ATOMIC_ACQUIRE);
}

int thread_code(struct Thread *thread) {
	if (!thread) return 0;

	// Only the main thread has permission to call this function
	if (!tid_is_self(TID_MAIN)) return 0;

	return (int) atomic_load_int(&thread->code, ATOMIC_ACQUIRE);
}

unsigned int thread_unique_max(void) {
	return DESCENT_UNIQUE_THREAD_COUNT_MAX;
}

unsigned int thread_worker_max(void) {
	return DESCENT_WORKER_THREAD_COUNT_MAX;
}

rcode thread_spawn_unique(unsigned int id, int (*function)(void *), void *argument, const char *name) {
	if (id >= DESCENT_UNIQUE_THREAD_COUNT_MAX) return DESCENT_ERROR_FORBIDDEN;
	return thread_spawn(&unique[id], function, argument, name, tid_generate_unique(id));
}

rcode thread_collect_unique(unsigned int id) {
	if (id >= DESCENT_UNIQUE_THREAD_COUNT_MAX) return DESCENT_ERROR_FORBIDDEN;
	return thread_collect(&unique[id]);
}

int thread_state_unique(unsigned int id) {
	if (id >= DESCENT_UNIQUE_THREAD_COUNT_MAX) return THREAD_STATE_INVALID;
	return thread_state(&unique[id]);
}

int thread_code_unique(unsigned int id) {
	if (id >= DESCENT_UNIQUE_THREAD_COUNT_MAX) return 0;
	return thread_code(&unique[id]);
}

rcode thread_spawn_worker(unsigned int count, int (*function)(void *), void *argument) {
	// Only the main thread has permission to call this function
	if (!tid_is_self(TID_MAIN)) return DESCENT_ERROR_FORBIDDEN;

	if (count > DESCENT_WORKER_THREAD_COUNT_MAX) return THREAD_ERROR_INVALID;

	// Check if workers are already active
	if (worker_count) return THREAD_ERROR_ACTIVE;
	
	// Update worker count
	worker_count = count;

	int result = 0;
	nchar name[DESCENT_THREAD_NAME_SIZE];
	
	// Try to create all threads
	for (unsigned int i = 0; i < count; ++i) {
		nchars_format(DESCENT_THREAD_NAME_SIZE, name, "D-WORKER %u", i);
		
		int worker_result = thread_spawn(&worker[i], function, argument, name, tid_generate_worker(i));
		if (worker_result) result = DESCENT_WARN_INCOMPLETE;
	}

	return result;
}

rcode thread_collect_worker(void) {
	// Only the main thread has permission to call this function
	if (!tid_is_self(TID_MAIN)) return DESCENT_ERROR_FORBIDDEN;

	int result = 0;
	
	// Try to collect all threads
	for (unsigned int i = 0; i < worker_count; ++i) {
		int worker_result = thread_collect(&worker[i]);

		if (worker_result && worker_result != THREAD_ERROR_INACTIVE) {
			result = DESCENT_ERROR_STATE;
		}
	}

	// Update worker count
	if (!result) worker_count = 0;

	return result;
}

int thread_state_worker(unsigned int id) {
	if (id >= worker_count) return THREAD_STATE_INVALID;
	return thread_state(&worker[id]);
}

int thread_code_worker(unsigned int id) {
	if (id >= worker_count) return 0;
	return thread_code(&worker[id]);
}
