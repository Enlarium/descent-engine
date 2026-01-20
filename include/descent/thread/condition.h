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

#ifndef DESCENT_THREAD_CONDITION_H
#define DESCENT_THREAD_CONDITION_H

#include <descent/rcode.h>
#include <descent/thread/atomic_types.h>

/**
 * @defgroup condition Condition Variables
 * @ingroup thread
 *
 * @brief Thread synchronization via condition variables.
 *
 * Condition variables allow threads to wait for arbitrary conditions to
 * become true while releasing an associated lock. A waiting thread will
 * atomically release the lock and suspend execution until it is signaled,
 * then re-acquire the lock before returning.
 *
 * Condition variables do not carry state. A signal or broadcast has no
 * effect if no threads are waiting at the time it is issued.
 *
 * All waits may experience spurious wakeups. Callers must always re-check
 * the associated condition predicate after waking.
 *
 * Signaling a condition variable establishes a happens-before relationship
 * with threads that successfully return from a corresponding wait.
 *
 * This mechanism is intra-process only and cannot be shared between processes.
 */

/**
 * @ingroup condition Condition
 * @brief Condition variable initializer, equivalent to {0}.
 */
#define CONDITION_INIT { ._generation = ATOMIC_INIT(0) }

/**
 * @ingroup condition Condition
 * @struct Condition
 * @brief A cross-platform condition variable.
 *
 * Used in combination with a lock to allow threads to wait for specific
 * conditions to be signaled. Condition variables allow a thread to sleep
 * while temporarily releasing a lock, then reacquire it upon waking.
 *
 * Must be zero-initialized ({0} or CONDITION_INIT) for static instances, or
 * explicitly zeroed before use for dynamic instances.
 */
struct Condition {
	atomic_32 _generation;
};

/**
 * @ingroup condition Condition
 * @brief Wakes one thread waiting on the condition variable.
 *
 * If multiple threads are waiting, only one is guaranteed to be woken.
 *
 * @param c Pointer to the condition variable.
 * @return
 * - 0 on success.
 * - @ref DESCENT_ERROR_NULL if @p c is null.
 * - @ref DESCENT_ERROR_FORBIDDEN if the calling thread is unmanaged.
 */
rcode condition_signal(struct Condition *c);

/**
 * @ingroup condition Condition
 * @brief Wakes all threads waiting on the condition variable.
 *
 * All threads currently waiting on the condition variable will be woken.
 *
 * @param c Pointer to the condition variable.
 * @return
 * - 0 on success.
 * - @ref DESCENT_ERROR_NULL if @p c is null.
 * - @ref DESCENT_ERROR_FORBIDDEN if the calling thread is unmanaged.
 */
rcode condition_broadcast(struct Condition *c);

#endif
