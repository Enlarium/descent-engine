// Copyright 2025 XavierHarkonnen9 and Enlarium
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef DESCENT_THREAD_BARRIER_H
#define DESCENT_THREAD_BARRIER_H

#include "../utilities/opaque.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct Barrier
 * @brief A thread barrier.
 *
 * Synchronizes a group of threads, making them wait until all threads have reached
 * the barrier point before any of them proceed.
 * 
 * @note This mechanism is intra-process only. It cannot be shared between processes.
 */
DESCENT_OPAQUE_DEFINE(Barrier, DESCENT_OPAQUE_SIZE_BARRIER)

/**
 * @brief Initialize a barrier.
 * 
 * @param b Pointer to the Barrier.
 * @param count Number of participating threads
 * @return 0 on success, non-zero on failure. 
 */
int barrier_init(Barrier *b, unsigned int count);

/**
 * @brief Destroy a barrier.
 * 
 * @param b Pointer to the Barrier.
 * @return 0 on success, non-zero on failure.
 */
int barrier_destroy(Barrier *b);

/**
 * @brief Wait at the barrier.
 * 
 * Block until 'count' threads have called wait.
 * 
 * @param b 
 * @return 0 on success, non-zero on failure. 
 */
int barrier_wait(Barrier *b);

#ifdef __cplusplus
}
#endif

#endif
