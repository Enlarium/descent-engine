#ifndef DESCENT_SOURCE_THREAD_THREAD_HANDLE_H
#define DESCENT_SOURCE_THREAD_THREAD_HANDLE_H

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <pthread.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <windows.h>
#endif

#include <descent/thread/atomic.h>

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

typedef pthread_t thread_handle;
typedef _Atomic thread_handle atomic_thread_handle;

static inline thread_handle atomic_load_thread_handle(atomic_thread_handle *object) {
	return atomic_load(object);
}

static inline void atomic_store_thread_handle(atomic_thread_handle *object, thread_handle desired) {
	atomic_store(object, desired);
}

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

typedef HANDLE thread_handle;
typedef atomic_ptr atomic_thread_handle;

static inline thread_handle atomic_load_thread_handle(atomic_thread_handle *object) {
	return (thread_handle) atomic_load_ptr(object);
}

static inline void atomic_store_thread_handle(atomic_thread_handle *object, thread_handle desired) {
	atomic_store_ptr(object, (thread_handle) desired);
}

#endif

#endif