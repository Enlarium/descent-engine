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

#ifndef DESCENT_THREAD_SPINLOCK_H
#define DESCENT_THREAD_SPINLOCK_H

#include "atomic.h"
#include "intrin.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SPINLOCK_INITIALIZER { 0 }

/**
 * @struct SpinLock
 * @brief A spinlock for synchronizing access to shared resources.
 * 
 * @note This mechanism is intra-process only. It cannot be shared between processes.
 */
typedef struct {
	atomic_32 _lock;
} SpinLock;

/**
 * @brief Initialize a spinlock.
 * @param m Pointer to the SpinLock.
 */
static inline void spinlock_init(SpinLock *l) {
	atomic_init_32(&l->_lock, 0);
}

/**
 * @brief Lock a spinlock.
 * @param m Pointer to the SpinLock.
 *
 * Busy-waits until the mutex is acquired.
 */
static inline void spinlock_lock(SpinLock *l) {
	uint32_t expected = 0;
	while (!atomic_compare_exchange_32(&l->_lock, &expected, 1)) {
		expected = 0;
		thread_pause();
  }
}

/**
 * @brief Attempt to lock a spinlock without blocking.
 * @param m Pointer to the SpinLock.
 * @return 0 if the lock was acquired, non-zero if the spinlock is already held by another thread.
 */
static inline int spinlock_trylock(SpinLock *l) {
	uint32_t expected = 0;
	return !atomic_compare_exchange_32(&l->_lock, &expected, 1);
}

/**
 * @brief Unlock a spinlock.
 * @param m Pointer to the SpinLock to unlock.
 * @return 0 on success, non-zero on failure.
 */
static inline void spinlock_unlock(SpinLock *l) {
	atomic_store_32(&l->_lock, 0);
}

#ifdef __cplusplus
}
#endif

#endif