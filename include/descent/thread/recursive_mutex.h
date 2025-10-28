#ifndef DESCENT_THREAD_RECURSIVE_MUTEX_H
#define DESCENT_THREAD_RECURSIVE_MUTEX_H

#include "../utilities/opaque.h"

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
DESCENT_OPAQUE_DEFINE(RecursiveMutex, DESCENT_OPAQUE_SIZE_RECURSIVE_MUTEX)

/**
 * @brief Initialize a recursive mutex.
 * @param m Pointer to the RecursiveMutex.
 * @return 0 on success, non-zero on failure.
 */
int recursive_mutex_init(RecursiveMutex *m);

/**
 * @brief Destroy a recursive mutex.
 * @param m Pointer to the RecursiveMutex.
 * @return 0 on success, non-zero on failure.
 */
int recursive_mutex_destroy(RecursiveMutex *m);

/**
 * @brief Lock a recursive mutex.
 * @param m Pointer to the RecursiveMutex.
 * @return 0 on success, non-zero on failure.
 *
 * Blocks until the mutex can be acquired. A thread can lock multiple times safely.
 */
int recursive_mutex_lock(RecursiveMutex *m);

/**
 * @brief Attempt to lock a recursive mutex without blocking.
 * @param m Pointer to the RecursiveMutex.
 * @return 0 if lock acquired, non-zero if already held by another thread.
 */
int recursive_mutex_trylock(RecursiveMutex *m);

/**
 * @brief Unlock a recursive mutex.
 * @param m Pointer to the RecursiveMutex.
 * @return 0 on success, non-zero on failure.
 *
 * Each call to unlock decrements the lock count; the mutex is fully released
 * only when the count reaches zero.
 */
int recursive_mutex_unlock(RecursiveMutex *m);

#ifdef __cplusplus
}
#endif

#endif