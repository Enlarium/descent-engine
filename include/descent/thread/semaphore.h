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

#include "../utilities/opaque.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct Semaphore
 * @brief A counting semaphore.
 *
 * Represents a counting semaphore that can be used to control access to
 * a finite number of shared resources.
 * 
 * @note This mechanism is intra-process only. It cannot be shared between processes.
 */
DESCENT_OPAQUE_DEFINE(Semaphore, DESCENT_OPAQUE_SIZE_SEMAPHORE)

/**
 * @brief Initialize a semaphore.
 * 
 * @param s Pointer to the Semaphore.
 * @param initial_count Initial value of the semaphore counter.
 * @return 0 on success, non-zero on failure.
 */
int semaphore_init(Semaphore *s, unsigned int initial_count);

/**
 * @brief Destroy a semaphore.
 * 
 * @param s Pointer to the Semaphore.
 * @return 0 on success, non-zero on failure. 
 */
int semaphore_destroy(Semaphore *s);

/**
 * @brief Lock a semaphore.
 * 
 * Decrements the semaphore counter, blocking if the counter is zero until another
 * thread posts to it.
 * 
 * @param s Pointer to the Semaphore.
 * @return 0 on success, non-zero on failure. 
 */
int semaphore_wait(Semaphore *s);

/**
 * @brief Try to wait (non-blocking) on a semaphore.
 *
 * Attempts to decrement the semaphore counter without blocking.
 *
 * @param s Pointer to the Semaphore structure.
 * @return 0 if the semaphore was successfully decremented, non-zero if the semaphore i already at zero.
 */
int semaphore_trywait(Semaphore *s);

/**
 * @brief Post (increment) a semaphore.
 *
 * Increments the semaphore counter, potentially unblocking a thread waiting on it.
 *
 * @param s Pointer to the Semaphore structure.
 * @return 0 on success, non-zero on failure.
 */
int semaphore_post(Semaphore *s);

#ifdef __cplusplus
}
#endif

#endif