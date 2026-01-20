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

#ifndef DESCENT_THREAD_FUTEX_H
#define DESCENT_THREAD_FUTEX_H

#include <descent/thread/atomic.h>
#include <descent/rcode.h>

/**
 * @brief Waits on a futex until its value changes.
 *
 * This function atomically compares the current value of the futex with
 * @p expected. If the values are equal, the calling thread may be suspended
 * until another thread wakes the futex. If the value differs, the function
 * returns immediately without sleeping.
 *
 * The caller must always recheck the futex value after this function returns,
 * as spurious wakeups are possible.
 *
 * @param futex Pointer to a 32-bit atomic futex variable.
 * @param expected The value expected to be stored in the futex.
 *
 * @return
 * - 0: Success or spurious wakeup.
 * - @ref DESCENT_ERROR_NULL: @p futex is a null pointer.
 * - @ref DESCENT_ERROR_FORBIDDEN: The futex cannot be accessed.
 * - @ref DESCENT_ERROR_INVALID: Invalid futex address or parameters.
 * - @ref DESCENT_ERROR_OS: Other OS-level errors.
 */
rcode futex_wait(atomic_32 *futex, uint32_t expected);

/**
 * @brief Waits on a futex until its value changes or a timeout expires.
 *
 * This function atomically compares the current value of the futex with
 * @p expected. If the values are equal, the calling thread may be suspended
 * until another thread wakes the futex or the timeout expires. If the value
 * differs, the function returns immediately without sleeping.
 *
 * The caller must always recheck the futex value after this function returns,
 * as spurious wakeups are possible.
 * 
 * Timeouts are capped at the value returned by @ref time_max_timeout().
 *
 * @param futex Pointer to a 32-bit atomic futex variable.
 * @param expected The value expected to be stored in the futex.
 * @param nanoseconds The maximum number of nanoseconds to wait.
 *
 * @return
 * - 0: Success or spurious wakeup.
 * - @ref THREAD_INFO_TIMEOUT: The wait timed out.
 * - @ref DESCENT_ERROR_NULL: @p futex is a null pointer.
 * - @ref DESCENT_ERROR_FORBIDDEN: The futex cannot be accessed.
 * - @ref DESCENT_ERROR_INVALID: Invalid futex address or parameters.
 * - @ref DESCENT_ERROR_OS: Other OS-level errors.
 */
rcode futex_timedwait(atomic_32 *futex, uint32_t expected, uint64_t nanoseconds);

/**
 * @brief Wakes one or more threads waiting on a futex.
 *
 * Wakes up to @p count threads that are currently waiting on the given futex.
 * If fewer threads are waiting, all waiting threads may be woken.
 *
 * The futex value itself is not modified by this function.
 *
 * @param futex Pointer to a 32-bit atomic futex variable.
 * @param count Maximum number of waiting threads to wake.
 *
 * @return
 * - 0: Successfully woke zero or more threads.
 * - @ref DESCENT_ERROR_NULL: @p futex is a null pointer.
 * - @ref DESCENT_ERROR_FORBIDDEN: The futex cannot be accessed.
 * - @ref DESCENT_ERROR_INVALID: Invalid futex address or parameters.
 * - @ref DESCENT_ERROR_OS: Other OS-level errors.
 *
 * @note Waking threads does not guarantee immediate execution, and no ordering
 * or fairness guarantees are provided.
 * 
 * @note If compiled with DESCENT_WINDOWS_FUTEX_WAKE_COUNT_ALL on Windows, all
 * waiting threads will be woken if count is greater than 1.
 */
rcode futex_wake(atomic_32 *futex, uint32_t count);

/**
 * @brief Wakes a single thread waiting on a futex.
 *
 * If one or more threads are currently waiting on the futex, exactly one
 * waiting thread is woken. If no threads are waiting, the function has no
 * effect.
 *
 * The futex value itself is not modified by this function.
 *
 * @param futex Pointer to a 32-bit atomic futex variable.
 *
 * @return
 * - 0: Successfully woke the next waiting thread.
 * - @ref DESCENT_ERROR_NULL: @p futex is a null pointer.
 * - @ref DESCENT_ERROR_FORBIDDEN: The futex cannot be accessed.
 * - @ref DESCENT_ERROR_INVALID: Invalid futex address or parameters.
 * - @ref DESCENT_ERROR_OS: Other OS-level errors.
 *
 * @note This function is equivalent to calling @ref futex_wake with a count
 * of 1.
 */
rcode futex_wake_next(atomic_32 *futex);

/**
 * @brief Wakes all threads waiting on a futex.
 *
 * Wakes every thread that is currently waiting on the given futex.
 * If no threads are waiting, the function has no effect.
 *
 * The futex value itself is not modified by this function.
 *
 * @param futex Pointer to a 32-bit atomic futex variable.
 *
 * @return
 * - 0: Successfully woke all waiting threads.
 * - @ref DESCENT_ERROR_NULL: @p futex is a null pointer.
 * - @ref DESCENT_ERROR_FORBIDDEN: The futex cannot be accessed.
 * - @ref DESCENT_ERROR_INVALID: Invalid futex address or parameters.
 * - @ref DESCENT_ERROR_OS: Other OS-level errors.
 *
 * @note No guarantees are made regarding the order in which threads are woken.
 * 
 * @note This function is equivalent to calling @ref futex_wake with the number
 * of waiters, but may exhibit better performance on Windows.
 */
rcode futex_wake_all(atomic_32 *futex);

#endif
