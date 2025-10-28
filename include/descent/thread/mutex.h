#ifndef DESCENT_THREAD_MUTEX_H
#define DESCENT_THREAD_MUTEX_H

#include "../utilities/platform.h"
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <pthread.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <windows.h>
#endif

#include "condition.h"

#define MUTEX_INITIALIZER {0} // TODO: needs checks

#ifdef __cplusplus
extern "C" {
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
DESCENT_OPAQUE_DEFINE(Mutex, DESCENT_OPAQUE_SIZE_MUTEX)

/**
 * @brief Initialize a mutex.
 * @param m Pointer to the Mutex.
 * @return 0 on success, non-zero on failure.
 */
int mutex_init(Mutex *m);

/**
 * @brief Destroy a mutex.
 * @param m Pointer to the Mutex.
 * @return 0 on success, non-zero on failure.
 */
int mutex_destroy(Mutex *m);

/**
 * @brief Lock a mutex.
 * @param m Pointer to the Mutex.
 * @return 0 on success, non-zero on failure.
 *
 * Blocks until the mutex is acquired.
 */
int mutex_lock(Mutex *m);

/**
 * @brief Attempt to lock a mutex without blocking.
 * @param m Pointer to the Mutex.
 * @return 0 if the lock was acquired, non-zero if the mutex is already held by another thread.
 */
int mutex_trylock(Mutex *m);

/**
 * @brief Unlock a mutex.
 * @param m Pointer to the Mutex.
 * @return 0 on success, non-zero on failure.
 */
int mutex_unlock(Mutex *m);

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
int condition_wait(Condition *c, Mutex *m);

#ifdef __cplusplus
}
#endif

#endif