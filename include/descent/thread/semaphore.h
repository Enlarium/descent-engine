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

#ifndef DESCENT_THREAD_SEMAPHORE_H
#define DESCENT_THREAD_SEMAPHORE_H

#include <stdint.h>

#include <descent/thread/atomic_types.h>

/**
 * @brief Static semaphore initializer.
 * @param maximum The maximum count of the semaphore
 * @param initial The initial count of the semaphore
 */
#define SEMAPHORE_INIT(maximum, initial) {._maximum = (maximum), ._count = ATOMIC_INIT(initial)}

/**
 * @struct Semaphore
 * @brief A counting semaphore.
 *
 * A semaphore maintains a non-negative count representing the number of
 * available resources. Threads may decrement the count by waiting on the
 * semaphore, or increment the count by signaling it.
 *
 * Semaphores do not have ownership semantics:
 * - Any thread may signal a semaphore.
 * - The thread that signals need not be the one that previously waited.
 *
 * @note This semaphore is intra-process only and cannot be shared between
 * processes.
 * @warning Semaphores do not provide mutual exclusion guarantees. Using a
 * semaphore as a mutex is discouraged; use a lock like @ref Mutex, @ref Qutex,
 * or @ref RWLock instead.
 */
struct Semaphore {
	uint32_t _maximum;
	atomic_32 _count;
};

/**
 * @brief Decrements the semaphore count, blocking if necessary.
 *
 * If the semaphore count is greater than zero, it is decremented and the
 * function returns immediately. If the count is zero, the calling thread
 * blocks until another thread signals the semaphore.
 *
 * This function may block indefinitely.
 *
 * @param s Pointer to the semaphore.
 * @return
 * - 0: Semaphore successfully acquired.
 * - @ref DESCENT_ERROR_NULL: @p s is NULL.
 */
int semaphore_wait(struct Semaphore *s);

/**
 * @brief Attempts to decrement the semaphore count without blocking.
 *
 * If the semaphore count is greater than zero, it is decremented and the
 * function returns immediately. If the count is zero, the function returns
 * without blocking.
 *
 * @param s Pointer to the semaphore.
 * @return
 * - 0: Semaphore successfully acquired.
 * - @ref THREAD_INFO_BUSY: Semaphore count is zero.
 * - @ref DESCENT_ERROR_NULL: @p s is NULL.
 */
int semaphore_trywait(struct Semaphore *s);

/**
 * @brief Increments the semaphore count and wakes a waiting thread, if any.
 *
 * Increments the semaphore count by one, making an additional unit of the
 * resource available. If one or more threads are blocked waiting on the
 * semaphore, one waiting thread is woken.
 *
 * The calling thread does not need to have previously waited on the semaphore.
 *
 * @param s Pointer to the semaphore.
 * @return
 * - 0: Semaphore successfully signaled.
 * - @ref DESCENT_ERROR_NULL: @p s is NULL.
 * - @ref DESCENT_ERROR_OVERFLOW: Semaphore count would exceed its maximum value.
 */
int semaphore_signal(struct Semaphore *s);

#endif
