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

#include "../utilities/opaque.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct Condition
 * @brief A condition variable.
 *
 * Allows threads to wait for a condition to become true and supports signaling
 * one or all waiting threads.
 * 
 * @note This mechanism is intra-process only. It cannot be shared between processes.
 */
DESCENT_OPAQUE_DEFINE(Condition, DESCENT_OPAQUE_SIZE_CONDITION)

/**
 * @brief Initializes a condition variable.
 * @param c Pointer to the Condition.
 * @return 0 on success, non-zero on failure.
 */
int condition_init(Condition *c);

/**
 * @brief Destroys a condition variable.
 * @param c Pointer to the Condition.
 * @return 0 on success, non-zero on failure.
 */
int condition_destroy(Condition *c);

/**
 * @brief Signals one thread waiting on the condition.
 * @param c Pointer to the Condition.
 * @return 0 on success, non-zero on failure.
 */
int condition_signal(Condition *c);

/**
 * @brief Signals all threads waiting on the condition.
 * @param c Pointer to the Condition.
 * @return 0 on success, non-zero on failure.
 */
int condition_broadcast(Condition *c);

#ifdef __cplusplus
}
#endif

#endif
