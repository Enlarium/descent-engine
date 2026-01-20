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

#ifndef DESCENT_INTERN_THREAD_THREAD_H
#define DESCENT_INTERN_THREAD_THREAD_H

#ifndef DESCENT_UNIQUE_THREAD_COUNT_MAX
#define DESCENT_UNIQUE_THREAD_COUNT_MAX 15u
#endif

#ifndef DESCENT_WORKER_THREAD_COUNT_MAX
#define DESCENT_WORKER_THREAD_COUNT_MAX 48u
#endif

#ifndef DESCENT_THREAD_NAME_SIZE
#define DESCENT_THREAD_NAME_SIZE 16u
#endif

// Main thread + unique threads + worker threads
#define THREAD_MAX (1 + DESCENT_UNIQUE_THREAD_COUNT_MAX + DESCENT_WORKER_THREAD_COUNT_MAX)
_Static_assert(THREAD_MAX <= 64, "Maximum managed thread count must not exceed 64");

#endif
