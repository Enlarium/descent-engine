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

#ifndef DESCENT_INTERN_THREAD_CALL_ONCE_U_H
#define DESCENT_INTERN_THREAD_CALL_ONCE_U_H

#include <descent/rcode.h>
#include <descent/thread/atomic_types.h>

/**
 * @ingroup call-once
 * @note This symbol is internal, for engine developers only.
 * @brief Unchecked call-once initializer, equivalent to {0}.
 */
#define CALL_ONCE_U_INIT { ._state = ATOMIC_INIT(0) }

/**
 * @ingroup call-once
 * @note This symbol is internal, for engine developers only.
 * @struct CallOnce_u
 * @brief An unchecked one-time initialization primitive.
 *
 * Ensures that a specified function is executed exactly once, even in the
 * presence of multiple threads. Useful for initializing global or shared
 * resources safely.
 * 
 * Once a function has been executed, subsequent calls will return
 * immediately.
 * 
 * Must be zero-initialized ({0} or CALL_ONCE_FAST_INIT) for static instances,
 * or explicitly zeroed before use for dynamic instances.
 *
 * @warning This implementation does not validate the function pointer,
 * detect reentrant invocation, or report misuse.
 */
struct CallOnce_u {
	atomic_32  _state;
};

/**
 * @ingroup call-once
 * @note This symbol is internal, for engine developers only.
 * @brief Executes a function exactly once, using an "unsafe" call-once.
 *
 * This variant of call-once is intended for internal or performance-critical
 * code. It assumes that the call-once object and function pointer are valid
 * and does not perform safety checks present for the public @ref call_once().
 *
 * Behavior:
 * - Ensures that the provided function pointer @p f is called at most once
 *   across all threads using the same call-once object.
 * - Does **not** detect or prevent deadlocks if the calling thread is
 *   recursively invoking the same call-once.
 * - Does **not** check futex return values; loops until completion.
 * - Assumes memory backing the call-once object is valid for the duration.
 *
 * After @p f is called, subsequent calls with the same call-once will return
 * immediately, but unsafe checks are omitted.
 * 
 * @param c Pointer to the call-once. Must not be NULL and must be properly
 * initialized.
 * @param f Pointer to the function to execute once. Must not be NULL.
 * @warning Only use this function when you can guarantee correctness of
 * parameters and usage. Prefer @ref call_once() for general use.
 */
void call_once_u(struct CallOnce_u *c, void (*f)(void));

#endif
