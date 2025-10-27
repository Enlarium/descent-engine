#ifndef DESCENT_THREAD_MUTEX_H
#define DESCENT_THREAD_MUTEX_H

#include "../utilities/platform.h"
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <pthread.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <windows.h>
#endif

#include "condition.h"



#ifdef __cplusplus
extern "C" {
#endif



#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#define MUTEX_INITIALIZER { PTHREAD_MUTEX_INITIALIZER }
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#define MUTEX_INITIALIZER { SRWLOCK_INIT }
#endif



/**
 * @struct Mutex
 * @brief A non-recursive mutex.
 *
 * This mutex allows only one thread to hold the lock at a time. It is non-recursive,
 * meaning a thread attempting to lock it multiple times without unlocking will deadlock.
 * 
 * @note This mechanism is intra-process only. It cannot be shared between processes.
 */
typedef struct {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	pthread_mutex_t _mutex;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	SRWLOCK _mutex;
#endif
} Mutex;



/**
 * @brief Initialize a mutex.
 * @param m Pointer to the Mutex.
 * @return 0 on success, non-zero on failure.
 */
static inline int mutex_init(Mutex *m) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_init(&m->_mutex, NULL);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	InitializeSRWLock(&m->_mutex);
	return 0;
#endif
}

/**
 * @brief Destroy a mutex.
 * @param m Pointer to the Mutex.
 * @return 0 on success, non-zero on failure.
 */
static inline int mutex_destroy(Mutex *m) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_destroy(&m->_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	(void)m;
	return 0;
#endif
}

/**
 * @brief Lock a mutex.
 * @param m Pointer to the Mutex.
 * @return 0 on success, non-zero on failure.
 *
 * Blocks until the mutex is acquired.
 */
static inline int mutex_lock(Mutex *m) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_lock(&m->_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	AcquireSRWLockExclusive(&m->_mutex);
	return 0;
#endif
}

/**
 * @brief Attempt to lock a mutex without blocking.
 * @param m Pointer to the Mutex.
 * @return 0 if the lock was acquired, non-zero if the mutex is already held by another thread.
 */
static inline int mutex_trylock(Mutex *m) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_trylock(&m->_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return !TryAcquireSRWLockExclusive(&m->_mutex);
#endif
}

/**
 * @brief Unlock a mutex.
 * @param m Pointer to the Mutex.
 * @return 0 on success, non-zero on failure.
 */
static inline int mutex_unlock(Mutex *m) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_unlock(&m->_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	ReleaseSRWLockExclusive(&m->_mutex);
	return 0;
#endif
}

/**
 * @brief Wait for a condition to be signaled.
 *
 * Atomically releases the mutex and waits for the condition variable to be signaled.
 * When the condition is signaled or broadcast, the mutex is reacquired before returning.
 *
 * @param c Pointer to the Condition.
 * @param m Pointer to the Mutex.
 * @return 0 on success, non-zero on failure.
 */
static inline int condition_wait(Condition *c, Mutex *m) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_cond_wait(&c->_condition, &m->_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return !SleepConditionVariableSRW(&c->_condition, &m->_mutex, INFINITE, 0);
#endif
}



#ifdef __cplusplus
}
#endif



#endif