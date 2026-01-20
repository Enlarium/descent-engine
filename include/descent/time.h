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

#ifndef DESCENT_TIME_H
#define DESCENT_TIME_H

#include <stdint.h>

/**
 * @brief Gets the current time in nanoseconds.
 * 
 * This timer measures from a reference time set approximately to the time
 * @ref descent_init() was called.
 * 
 * This timer is monotonic and not affected by system clock changes.
 * 
 * @return The current time in nanoseconds.
 */
uint64_t time_nanoseconds(void);

/**
 * @brief Returns the maximum allowed timeout for wait functions in
 * nanoseconds.
 * @return The maximum timeout.
 */
uint64_t time_max_timeout(void);

/**
 * @brief Returns the elapsed time between two timestamps in seconds as a
 * double.
 *
 * @param time_start Earlier time (from time_nanoseconds()).
 * @param time_end Later time (from time_nanoseconds()).
 * @return Elapsed time in seconds. May be negative if time_end < time_start.
 */
static inline double time_delta(uint64_t time_start, uint64_t time_end) {
	int64_t delta = (int64_t) (time_end - time_start);
	return (double) delta / 1e9;
}

#endif