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

#ifndef DESCENT_THREAD_QUTEX_H
#define DESCENT_THREAD_QUTEX_H

#include <stdint.h>

#include <descent/thread/atomic_types.h>
#include <descent/thread/condition.h>

/**
 * @brief Static qutex initializer, equivalent to {0}.
 */
#define QUTEX_INIT {._tail = ATOMIC_INIT(0)}

/**
 * @struct Qutex
 * @brief A FIFO, queue-based, non-recursive mutex.
 *
 * Only one thread can hold the lock at a time. Recursive locking is
 * not supported; attempting to re-lock from the same thread
 * returns a deadlock error.
 *
 * @note This mechanism is intra-process only; it cannot be shared across processes.
 */
struct Qutex {
	atomic_ptr _tail;
};
/**
 * @brief Locks the qutex, blocking the calling thread if necessary.
 *
 * If the qutex is already held by another thread, the calling thread will be
 * enqueued and blocked until it becomes available.
 *
 * @param q Pointer to the qutex to lock.
 * @return
 * - 0: Lock acquired successfully.
 * - @ref DESCENT_ERROR_NULL: @p q is NULL.
 * - @ref THREAD_ERROR_DEADLOCK: Calling thread is already waiting on this
 *   qutex.
 */
rcode qutex_lock(struct Qutex *m);

/**
 * @brief Attempts to lock the qutex without blocking.
 *
 * If the qutex is already held by another thread, this function returns
 * immediately. Recursive locking by the same thread is not allowed.
 *
 * @param q Pointer to the qutex to try locking.
 * @return
 * - 0: Lock acquired successfully.
 * - @ref THREAD_INFO_BUSY: Qutex is already locked by another thread.
 * - @ref DESCENT_ERROR_NULL: @p q is NULL.
 * - @ref THREAD_ERROR_DEADLOCK: Calling thread is already waiting on this
 *   qutex.
 */
rcode qutex_trylock(struct Qutex *m);

/**
 * @brief Unlocks a previously locked qutex.
 *
 * Only the thread that currently holds the qutex may unlock it. Ownership is
 * passed to the next enqueued thread, if any.
 *
 * @param q Pointer to the qutex to unlock.
 * @return
 * - 0: Qutex unlocked successfully.
 * - @ref DESCENT_ERROR_NULL: @p q is NULL.
 */
rcode qutex_unlock(struct Qutex *m);

#endif
