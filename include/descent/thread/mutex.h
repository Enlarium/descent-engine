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

#ifndef DESCENT_THREAD_MUTEX_H
#define DESCENT_THREAD_MUTEX_H

#include <stdint.h>

#include <descent/thread/atomic_types.h>
#include <descent/thread/condition.h>

/**
 * @brief Static mutex initializer, equivalent to {0}.
 */
#define MUTEX_INIT {._owner = ATOMIC_INIT(0), ._state = ATOMIC_INIT(0)}

/**
 * @struct Mutex
 * @brief A non-recursive mutex.
 *
 * Ensures that only one thread can hold the lock at a time. Recursive locking is
 * not allowed; attempts to re-lock from the owning thread return a deadlock error.
 * 
 * @warning Closing a thread while it owns a mutex leaves the mutex in an undefined state.
 * @note This mechanism is intra-process only and cannot be shared between processes.
 */
struct Mutex {
	atomic_64 _owner;
	atomic_32 _state;
};

/**
 * @brief Locks the mutex, blocking the calling thread if necessary.
 *
 * If the mutex is already locked by another thread, the calling thread
 * will wait until it becomes available. Recursive locking by the same
 * thread is not allowed.
 *
 * @param m Pointer to the mutex to lock.
 * @return
 * - 0: Lock acquired successfully.
 * - @ref DESCENT_ERROR_NULL: @p m is NULL.
 * - @ref DESCENT_ERROR_FORBIDDEN: Calling thread is invalid.
 * - @ref THREAD_ERROR_DEADLOCK: Calling thread already owns the mutex.
 */
rcode mutex_lock(struct Mutex *m);

/**
 * @brief Attempts to lock the mutex, blocking up to the specified timeout.
 *
 * If the mutex is already locked by another thread, the calling thread
 * will wait until it becomes available. Recursive locking by the same
 * thread is not allowed.
 *
 * Timeouts are capped at the value returned by @ref time_max_timeout().
 *
 * @param m Pointer to the mutex to lock.
 * @param nanoseconds The maximum number of nanoseconds to wait.
 * @return
 * - 0: Lock acquired successfully.
 * - @ref THREAD_INFO_TIMEOUT: Lock could not be acquired within the timeout.
 * - @ref DESCENT_ERROR_NULL: @p m is NULL.
 * - @ref DESCENT_ERROR_FORBIDDEN: Calling thread is invalid.
 * - @ref THREAD_ERROR_DEADLOCK: Calling thread already owns the mutex.
 */
rcode mutex_timedlock(struct Mutex *m, uint64_t nanoseconds);

/**
 * @brief Attempts to lock the mutex without blocking.
 *
 * If the mutex is already locked by another thread, this function returns
 * immediately. Recursive locking by the same thread is not allowed.
 * 
 * @param m Pointer to the mutex to try locking.
 * @return
 * - 0: Lock acquired successfully.
 * - @ref THREAD_INFO_BUSY: Mutex is already locked by another thread.
 * - @ref DESCENT_ERROR_NULL: @p m is NULL.
 * - @ref DESCENT_ERROR_FORBIDDEN: Calling thread is invalid.
 */
rcode mutex_trylock(struct Mutex *m);

/**
 * @brief Unlocks a previously locked mutex.
 *
 * Only the thread that owns the mutex may unlock it. Unlocking a mutex not
 * held by the calling thread will fail.
 *
 * @param m Pointer to the mutex to unlock.
 * @return
 * - 0: Mutex unlocked successfully.
 * - @ref DESCENT_ERROR_NULL: @p m is NULL.
 * - @ref DESCENT_ERROR_FORBIDDEN: Mutex is not owned by the calling thread.
 */
rcode mutex_unlock(struct Mutex *m);

/**
 * @brief Waits indefinitely on a condition variable while holding the mutex.
 *
 * The calling thread must hold the mutex before calling this function.
 * The mutex is released while waiting and re-acquired before returning.
 * 
 * @param m Pointer to the mutex currently held.
 * @param c Pointer to the condition variable to wait on.
 * @return
 * - 0: Successfully waited and re-acquired the mutex.
 * - @ref DESCENT_ERROR_NULL: Either @p m or @p c is NULL.
 * - @ref DESCENT_ERROR_FORBIDDEN: Calling thread is invalid.
 * - Other mutex errors: Any error returned by @ref mutex_unlock or
 *   @ref mutex_lock.
 * @note Spurious wakeups are possible; always check the associated predicate.
 */
rcode mutex_wait(struct Mutex *m, struct Condition *c);

#endif
