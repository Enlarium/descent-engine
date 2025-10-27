#ifndef DESCENT_THREAD_RECURSIVE_MUTEX_H
#define DESCENT_THREAD_RECURSIVE_MUTEX_H

#include "../utilities/platform.h"
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <pthread.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <windows.h>
#endif



#ifdef __cplusplus
extern "C" {
#endif



/**
 * @struct RecursiveMutex
 * @brief A recursive mutex allowing the same thread to lock multiple times.
 *
 * Recursive mutexes allow a thread that already holds the lock to acquire it again
 * without deadlocking.
 * 
 * @note This mechanism is intra-process only. It cannot be shared between processes.
 */
typedef struct {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	pthread_mutex_t  _mutex;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	CRITICAL_SECTION _mutex;
#endif
} RecursiveMutex;



/**
 * @brief Initialize a recursive mutex.
 * @param m Pointer to the RecursiveMutex.
 * @return 0 on success, non-zero on failure.
 */
static inline int recursive_mutex_init(RecursiveMutex *m) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	pthread_mutexattr_t attr;
	if (!pthread_mutexattr_init(&attr)) {
		if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE)) return 1;
		
		int result = !!pthread_mutex_init(&m->_mutex, &attr);
		pthread_mutexattr_destroy(&attr);
		return result;
	}
	return 1;

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	InitializeCriticalSection(&m->_mutex);
	return 0;
#endif
}

/**
 * @brief Destroy a recursive mutex.
 * @param m Pointer to the RecursiveMutex.
 * @return 0 on success, non-zero on failure.
 */
static inline int recursive_mutex_destroy(RecursiveMutex *m) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_destroy(&m->_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	DeleteCriticalSection(&m->_mutex);
	return 0;
#endif
}

/**
 * @brief Lock a recursive mutex.
 * @param m Pointer to the RecursiveMutex.
 * @return 0 on success, non-zero on failure.
 *
 * Blocks until the mutex can be acquired. A thread can lock multiple times safely.
 */
static inline int recursive_mutex_lock(RecursiveMutex *m) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_lock(&m->_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	EnterCriticalSection(&m->_mutex);
	return 0;
#endif
}

/**
 * @brief Attempt to lock a recursive mutex without blocking.
 * @param m Pointer to the RecursiveMutex.
 * @return 0 if lock acquired, non-zero if already held by another thread.
 */
static inline int recursive_mutex_trylock(RecursiveMutex *m) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_trylock(&m->_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return !TryEnterCriticalSection(&m->_mutex);
#endif
}

/**
 * @brief Unlock a recursive mutex.
 * @param m Pointer to the RecursiveMutex.
 * @return 0 on success, non-zero on failure.
 *
 * Each call to unlock decrements the lock count; the mutex is fully released
 * only when the count reaches zero.
 */
static inline int recursive_mutex_unlock(RecursiveMutex *m) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_unlock(&m->_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	LeaveCriticalSection(&m->_mutex);
	return 0;
#endif
}



#ifdef __cplusplus
}
#endif



#endif