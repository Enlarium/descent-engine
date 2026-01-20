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

#ifndef DESCENT_THREAD_THREAD_H
#define DESCENT_THREAD_THREAD_H

#include <stdint.h>
#include <stdbool.h>

#include <descent/rcode.h>

enum {
	THREAD_STATE_INVALID = -1,
	THREAD_STATE_UNUSED  = 0,
	THREAD_STATE_STARTING,
	THREAD_STATE_RUNNING,
	THREAD_STATE_FINISHED,
	THREAD_STATE_INCOMPLETE
};

#include <descent/thread/tls.h>

// Unmanaged TIDS cannot use this library's threading functions

// Persistent subsystems like rendering, audio, and networking can be implemented through
// unique threads. Easily-parallelized tasks should be implemented through a job system
// on worker threads.

/**
 * @brief Gets the maximum number of unique threads that can be created.
 * @return The maximum number of unique threads.
 */
unsigned int thread_unique_max(void);

/**
 * @brief Gets the maximum number of worker threads that can be created.
 * @return The maximum number of worker threads.
 */
unsigned int thread_worker_max(void);

/**
 * @brief Spawns a unique thread on the given thread ID.
 * 
 * Unique threads implement a single function on a single thread.
 * 
 * @param id The ID of the unique thread. Must be less than thread_unique_max().
 * @param function The function invoked by the spawned thread. Must not be NULL
 * @param argument The argument passed to the thread function. Can be NULL if the
 * function supports it.
 * @param name A name for the thread, used for debugging purposes. Should be
 * UTF-8 encoded. Can be NULL. Thread names may be truncated on some platforms.
 * @return 0 on success, non-zero error code on failure.
 * @note This function should only be called from the main thread. Calling it from
 * any other thread will return DESCENT_ERROR_FORBIDDEN.
 */
rcode thread_spawn_unique(unsigned int id, int (*function)(void *), void *argument, const char *name);

/**
 * @brief Collects the unique thread on the given thread ID.
 * 
 * Joins the specified unique thread. After collection, the codes and state will be
 * reset and the thread ID can be used to spawn a new thread.
 * 
 * @param id The ID of the unique thread.
 * @return 0 on success, non-zero error code on failure.
 * @note This function should only be called from the main thread. Calling it from
 * any other thread will return DESCENT_ERROR_FORBIDDEN.
 */
rcode thread_collect_unique(unsigned int id);

/**
 * @brief Gets the state of the unique thread on the given thread ID.
 * @param id The ID of the unique thread.
 * @return The state of the unique thread.
 * @note This function should only be called from the main thread. Calling it from
 * any other thread will return THREAD_STATE_INVALID.
 */
int thread_state_unique(unsigned int id);

/**
 * @brief Gets the return code of the unique thread on the given thread ID.
 * @param id The ID of the unique thread.
 * @return The return code of the unique thread. If the unique thread is invalid or
 * has not finished, this will return 0.
 * @note This function should only be called from the main thread. Calling it from
 * any other thread will return 0.
 */
int thread_code_unique(unsigned int id);

/**
 * @brief Spawns a batch of worker threads.
 * 
 * Worker threads implement a single function concurrently across multiple
 * threads. All worker threads invoke the same function and receive the same
 * argument.
 * 
 * Only one batch can be spawned and active at a time.
 * 
 * @param count The number of worker threads to spawn. Must not be greater than
 * thread_worker_max().
 * @param function The function invoked by the spawned threads. Must not be NULL.
 * @param argument The argument passed to each thread function. Can be NULL if the
 * function supports it.
 * @return 0 on success, non-zero error code on failure.
 * @note This function should only be called from the main thread. Calling it from
 * any other thread will return DESCENT_ERROR_FORBIDDEN.
 * @note If any worker thread fails to spawn, this function will return
 * DESCENT_WARN_INCOMPLETE. Any threads that failed to spawn will have state
 * THREAD_STATE_INCOMPLETE, and their code will be set to the relevant failure
 * code.
 */
rcode thread_spawn_worker(unsigned int count, int (*function)(void *), void *argument);

/**
 * @brief Collects all worker threads.
 * 
 * Joins the worker threads. After collection, the codes and state will be reset
 * and new worker threads can be spawned.
 * 
 * @return 0 on success, non-zero error code on failure.
 * @note This function should only be called from the main thread. Calling it from
 * any other thread will return DESCENT_ERROR_FORBIDDEN.
 */
rcode thread_collect_worker(void);

/**
 * @brief Gets the state of the worker thread on the given thread ID.
 * @param id The ID of the worker thread.
 * @return The state of the worker thread.
 * @note This function should only be called from the main thread. Calling it from
 * any other thread will return THREAD_STATE_INVALID.
 */
int thread_state_worker(unsigned int id);

/**
 * @brief Gets the return code of the worker thread on the given thread ID.
 * @param id The ID of the worker thread.
 * @return The return code of the worker thread. If the worker thread is invalid or
 * has not finished, this will return 0.
 * @note This function should only be called from the main thread. Calling it from
 * any other thread will return 0.
 */
int thread_code_worker(unsigned int id);

#endif
