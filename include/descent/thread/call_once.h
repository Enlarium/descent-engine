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

#ifndef DESCENT_THREAD_CALL_ONCE_H
#define DESCENT_THREAD_CALL_ONCE_H

#include "../utilities/opaque.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct CallOnce
 * @brief Ensures a function is executed only once across all threads.
 *
 * Useful for initializing global or shared resources in a thread-safe manner.
 * 
 * @note This mechanism is intra-process only. It cannot be shared between processes.
 */
DESCENT_OPAQUE_DEFINE(CallOnce, DESCENT_OPAQUE_SIZE_CALL_ONCE)

/**
 * @typedef CallOnceFunction
 * @brief Function type to call-once.
 */
typedef void (*CallOnceFunction)(void);

/**
 * @brief Initialize a call-once.
 * @param c Pointer to the CallOnce.
 * 
 * @note After call_once executes, re-initializing the same CallOnce object is undefined behavior.
 */
void call_once_init(CallOnce *c);

/**
 * @brief Executes a function exactly once across all threads.
 * @param c Pointer to the CallOnce.
 * @param f Function to execute.
 *
 * Subsequent calls after the first successful execution are no-ops.
 */
void call_once(CallOnce *c, CallOnceFunction f);

#ifdef __cplusplus
}
#endif

#endif