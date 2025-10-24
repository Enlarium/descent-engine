#ifndef DESCENT_THREAD_RECURSIVE_MUTEX_H
#define DESCENT_THREAD_RECURSIVE_MUTEX_H

#include "../utilities/platform.h"
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <pthread.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <windows.h>
#endif

typedef struct {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	pthread_mutex_t  _recursive_mutex;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	CRITICAL_SECTION _recursive_mutex;
#endif
} RecursiveMutex;

static inline int recursive_mutex_init(RecursiveMutex *m) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	int result = !!pthread_mutex_init(&m->_recursive_mutex, &attr);
	pthread_mutexattr_destroy(&attr);
	return result;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	InitializeCriticalSection(&m->_recursive_mutex);
	return 0;
#endif
}

static inline int recursive_mutex_destroy(RecursiveMutex *m) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_destroy(&m->_recursive_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	DeleteCriticalSection(&m->_recursive_mutex);
	return 0;
#endif
}

static inline int recursive_mutex_lock(RecursiveMutex *m) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_lock(&m->_recursive_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	EnterCriticalSection(&m->_recursive_mutex);
	return 0;
#endif
}

static inline int recursive_mutex_trylock(RecursiveMutex *m) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_trylock(&m->_recursive_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return !TryEnterCriticalSection(&m->_recursive_mutex);
#endif
}

static inline int recursive_mutex_unlock(RecursiveMutex *m) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_unlock(&m->_recursive_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	LeaveCriticalSection(&m->_recursive_mutex);
	return 0;
#endif
}

#endif