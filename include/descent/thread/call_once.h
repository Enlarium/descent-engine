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

#ifndef DESCENT_THREAD_CALL_ONCE_H
#define DESCENT_THREAD_CALL_ONCE_H

#include <descent/rcode.h>
#include <descent/thread/atomic_types.h>

/**
 * @defgroup call-once Call-Once
 * @ingroup thread
 *
 * @brief One-time function calls.
 *
 * Provides a call-once primitive for ensuring that a function is executed
 * exactly once across all threads using the same call-once object.
 *
 * If multiple threads attempt to execute the function concurrently, only one
 * will execute it; the others will block until initialization completes.
 * 
 * Once initialization completes successfully, all memory effects of the
 * initialization function are visible to all threads returning from the
 * call-once operation.
 * 
 * This mechanism is intra-process only and cannot be shared between processes.
 */

/**
 * @ingroup call-once
 * @brief Call-once initializer, equivalent to {0}.
 */
#define CALL_ONCE_INIT { ._function = ATOMIC_INIT(0), ._owner = ATOMIC_INIT(0), ._state = ATOMIC_INIT(0) }

/**
 * @ingroup call-once
 * @struct CallOnce
 * @brief A one-time initialization primitive.
 *
 * Ensures that a specified function is executed exactly once, even in the
 * presence of multiple threads. Useful for initializing global or shared
 * resources safely.
 * 
 * Once a function has been executed, subsequent calls with the same
 * call-once will return immediately.
 * 
 * Must be zero-initialized ({0} or CALL_ONCE_INIT) for static instances, or
 * explicitly zeroed before use for dynamic instances.
 */
struct CallOnce {
	atomic_ptr _function;
	atomic_64  _owner;
	atomic_32  _state;
};

/**
 * @ingroup call-once
 * @brief Executes a function exactly once, using the specified call-once.
 *
 * This function ensures that the provided function pointer @p f is called at
 * most once across all threads that use the same call-once object. If multiple
 * threads attempt to call @p f simultaneously, only one will execute it, and
 * the others will block until execution is complete.
 * 
 * After @p f is called, calling this function with the same call-once and
 * function will immediately return 0. If a different function is provided,
 * this function will return @ref DESCENT_ERROR_INVALID.
 * 
 * If @p f calls @ref call_once on the same call-once used to invoke it, this
 * function will return @ref THREAD_ERROR_DEADLOCK.
 * 
 * @param c Pointer to the call-once. Must not be NULL.
 * @param f Pointer to the function to execute once. Must not be NULL.
 * @return
 * - 0 on success.
 * - @ref DESCENT_ERROR_NULL if either parameter is null.
 * - @ref THREAD_ERROR_DEADLOCK on re-entrant invocation.
 * - @ref DESCENT_ERROR_INVALID if a different function is provided on re-call.
 */
rcode call_once(struct CallOnce *c, void (*f)(void));

#endif
