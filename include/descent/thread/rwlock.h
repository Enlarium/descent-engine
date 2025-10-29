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

#ifndef DESCENT_THREAD_RWLOCK_H
#define DESCENT_THREAD_RWLOCK_H

#include "../utilities/opaque.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct RWLock
 * @brief A read-write lock for synchronizing access to shared resources.
 *
 * Supports multiple readers or one writer at a time.
 * 
 * @note This mechanism is intra-process only. It cannot be shared between processes.
 */
DESCENT_OPAQUE_DEFINE(RWLock, DESCENT_OPAQUE_SIZE_RWLOCK)

/**
 * @brief Initialize a read-write lock.
 * @param l Pointer to the RWLock.
 * @return 0 on success, non-zero on failure.
 */
int rwlock_init(RWLock *l);

/**
 * @brief Destroy a read-write lock.
 * @param l Pointer to the RWLock.
 * @return 0 on success, non-zero on failure.
 */
int rwlock_destroy(RWLock *l);

/**
 * @brief Acquire a read (shared) lock.
 * @param l Pointer to the RWLock.
 * @return 0 on success, non-zero on failure.
 *
 * Multiple threads can hold a read lock simultaneously.
 */
int rwlock_read_lock(RWLock *l);

/**
 * @brief Attempt to acquire a read (shared) lock without blocking.
 * @param l Pointer to the RWLock.
 * @return 0 if lock acquired, non-zero if already held.
 */
int rwlock_read_trylock(RWLock *l);

/**
 * @brief Release a read (shared) lock.
 * @param l Pointer to the RWLock.
 * @return 0 on success, non-zero on failure.
 */
int rwlock_read_unlock(RWLock *l);

/**
 * @brief Acquire a write (exclusive) lock.
 * @param l Pointer to the RWLock.
 * @return 0 on success, non-zero on failure.
 *
 * Only one thread can hold the write lock at a time. Blocks readers and other writers.
 */
int rwlock_write_lock(RWLock *l);

/**
 * @brief Attempt to acquire a write (exclusive) lock without blocking.
 * @param l Pointer to the RWLock.
 * @return 0 if lock acquired, non-zero if already held.
 */
int rwlock_write_trylock(RWLock *l);

/**
 * @brief Release a write (exclusive) lock.
 * @param l Pointer to the RWLock.
 * @return 0 on success, non-zero on failure.
 */
int rwlock_write_unlock(RWLock *l);

#ifdef __cplusplus
}
#endif

#endif