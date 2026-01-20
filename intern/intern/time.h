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

#ifndef DESCENT_INTERN_TIME_H
#define DESCENT_INTERN_TIME_H

#include <stdint.h>

#include <descent/rcode.h>
#include <descent/thread/atomic.h>

#define NSEC_PER_SEC  1000000000ull
#define NSEC_PER_MSEC 1000000ull

#ifndef DESCENT_MAXIMUM_TIMEOUT
#define DESCENT_MAXIMUM_TIMEOUT (3600ull * NSEC_PER_SEC)
#endif

#if defined(DESCENT_PLATFORM_TYPE_POSIX)
typedef struct timespec timeout_native;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
typedef DWORD timeout_native;
#endif

/**
 * @brief Initializes the monotonic timer.
 * 
 * Must be called before any calls to @ref time_nanoseconds().
 * 
 * @return
 * - 0 on success.
 * - DESCENT_FATAL_TIMER if the timer could not be initialized.
 */
rcode time_init(void);

/**
 * @brief Converts a timeout in nanoseconds to a native timeout type.
 * 
 * Used for futexes.
 * 
 * Timeouts are capped at @ref DESCENT_MAXIMUM_TIMEOUT.
 * 
 * @param nanoseconds The number of nanoseconds to timeout.
 * @return The native equivalent of the timeout. 
 */
timeout_native time_to_timeout(uint64_t nanoseconds);

#endif