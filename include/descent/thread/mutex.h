#ifndef DESCENT_THREAD_MUTEX_H
#define DESCENT_THREAD_MUTEX_H

#include "../utilities/platform.h"
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <pthread.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <windows.h>
#endif


#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#define MUTEX_INITIALIZER { PTHREAD_MUTEX_INITIALIZER }
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#define MUTEX_INITIALIZER { SRWLOCK_INIT }
#endif


typedef struct {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	pthread_mutex_t _mutex;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	SRWLOCK _mutex;
#endif
} Mutex;

static inline int mutex_init(Mutex *m) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_init(&m->_mutex, NULL);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	InitializeSRWLock(&m->_mutex);
	return 0;
#endif
}

static inline int mutex_destroy(Mutex *m) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_destroy(&m->_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	(void)m;
	return 0;
#endif
}

static inline int mutex_lock(Mutex *m) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_lock(&m->_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	AcquireSRWLockExclusive(&m->_mutex);
	return 0;
#endif
}

static inline int mutex_trylock(Mutex *m) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_trylock(&m->_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return !TryAcquireSRWLockExclusive(&m->_mutex);
#endif
}

static inline int mutex_unlock(Mutex *m) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_unlock(&m->_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	ReleaseSRWLockExclusive(&m->_mutex);
	return 0;
#endif
}

#endif