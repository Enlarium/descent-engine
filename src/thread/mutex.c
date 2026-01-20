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

#include <descent/thread/mutex.h>

#include <stdint.h>
#include <stdbool.h>

#include <descent/rcode.h>
#include <descent/thread/atomic.h>
#include <descent/thread/condition.h>
#include <descent/thread/futex.h>
#include <descent/time.h>
#include <descent/utilities/builtin.h>
#include <intern/thread/tid.h>

// TODO: Implement eventual fairness
// If a thread has consecutively held a lock for more than 1 ms, the next transition
// must be fair

enum {
	MUTEX_UNLOCKED = 0,
	MUTEX_LOCKED,
	MUTEX_CONTENDED,
};

rcode mutex_lock(struct Mutex *m) {
	if (!m) return DESCENT_ERROR_NULL;

	// Check that the current thread has permission to call this function
	if (builtin_expect(tid_is_self(TID_NONE), false)) return DESCENT_ERROR_FORBIDDEN;

	// Fast path where lock isn't held
	uint32_t expected = MUTEX_UNLOCKED;
	if (atomic_compare_exchange_32(&m->_state, &expected, MUTEX_LOCKED, ATOMIC_ACQ_REL, ATOMIC_ACQUIRE)) {
		atomic_store_64(&m->_owner, tid_self(), ATOMIC_RELEASE);
		return 0;
	}

	// Detect re-entrant deadlocks
	if (tid_is_self(atomic_load_64(&m->_owner, ATOMIC_ACQUIRE))) return THREAD_ERROR_DEADLOCK;

	do {
		expected = MUTEX_LOCKED;
		bool exchange = atomic_compare_exchange_32(&m->_state, &expected, MUTEX_CONTENDED, ATOMIC_ACQ_REL, ATOMIC_ACQUIRE);

		if (exchange || expected == MUTEX_CONTENDED) {
			rcode result = futex_wait(&m->_state, MUTEX_CONTENDED);
			if (result) return result;
		}

		expected = MUTEX_UNLOCKED;
	} while (atomic_compare_exchange_32(&m->_state, &expected, MUTEX_CONTENDED, ATOMIC_ACQ_REL, ATOMIC_ACQUIRE));

	atomic_store_64(&m->_owner, tid_self(), ATOMIC_RELEASE);

	return 0;
}

rcode mutex_timedlock(struct Mutex *m, uint64_t nanoseconds) {
	if (!m) return DESCENT_ERROR_NULL;

	// Check that the current thread has permission to call this function
	if (builtin_expect(tid_is_self(TID_NONE), false)) return DESCENT_ERROR_FORBIDDEN;

	// Fast path where lock isn't held
	uint32_t expected = MUTEX_UNLOCKED;
	if (atomic_compare_exchange_32(&m->_state, &expected, MUTEX_LOCKED, ATOMIC_ACQ_REL, ATOMIC_ACQUIRE)) {
		atomic_store_64(&m->_owner, tid_self(), ATOMIC_RELEASE);
		return 0;
	}

	// Detect re-entrant deadlocks
	if (tid_is_self(atomic_load_64(&m->_owner, ATOMIC_ACQUIRE))) return THREAD_ERROR_DEADLOCK;

	uint64_t start = time_nanoseconds();
	uint64_t remaining = nanoseconds;

	do {
		expected = MUTEX_LOCKED;
		bool exchange = atomic_compare_exchange_32(&m->_state, &expected, MUTEX_CONTENDED, ATOMIC_ACQ_REL, ATOMIC_ACQUIRE);

		if (exchange || expected == MUTEX_CONTENDED) {
			// Wait with remaining timeout
			rcode result = futex_timedwait(&m->_state, MUTEX_CONTENDED, remaining);
			if (result) return result;

			// Adjust remaining timeout
			uint64_t now = time_nanoseconds();
			if (now - start >= remaining) return THREAD_INFO_TIMEOUT;
			remaining = nanoseconds - (now - start);
		}

		expected = MUTEX_UNLOCKED;
	} while (!atomic_compare_exchange_32(&m->_state, &expected, MUTEX_CONTENDED, ATOMIC_ACQ_REL, ATOMIC_ACQUIRE));

	atomic_store_64(&m->_owner, tid_self(), ATOMIC_RELEASE);

	return 0;
}

rcode mutex_trylock(struct Mutex *m) {
	if (!m) return DESCENT_ERROR_NULL;

	// Check that the current thread has permission to call this function
	if (builtin_expect(tid_is_self(TID_NONE), false)) return DESCENT_ERROR_FORBIDDEN;
	
	uint32_t expected = MUTEX_UNLOCKED;
	if (atomic_compare_exchange_32(&m->_state, &expected, MUTEX_LOCKED, ATOMIC_ACQ_REL, ATOMIC_ACQUIRE)) {
		atomic_store_64(&m->_owner, tid_self(), ATOMIC_RELEASE);
		return 0;
	}

	return THREAD_INFO_BUSY;
}

rcode mutex_unlock(struct Mutex *m) {
	if (!m) return DESCENT_ERROR_NULL;

	rcode result = 0;

	// Check that the current thread has permission to call this function
	// Only the owner is ever allowed to mutate the _owner field
	thread_id self = tid_self();
	if (!atomic_compare_exchange_64(&m->_owner, &self, TID_NONE, ATOMIC_ACQ_REL, ATOMIC_ACQUIRE)) {
		return DESCENT_ERROR_FORBIDDEN;
	}

	uint32_t old_state = atomic_exchange_32(&m->_state, MUTEX_UNLOCKED, ATOMIC_RELEASE);
	if (old_state == MUTEX_CONTENDED) result = futex_wake_next(&m->_state);

	return result;
}

rcode mutex_wait(struct Mutex *m, struct Condition *c) {
	if (!m || !c) return DESCENT_ERROR_NULL;

	rcode result = 0;

	// Check that the current thread has permission to call this function
	if (builtin_expect(tid_is_self(TID_NONE), false)) return DESCENT_ERROR_FORBIDDEN;
	
	uint32_t expected = atomic_load_32(&c->_generation, ATOMIC_RELAXED);
	
	result = mutex_unlock(m);
	if (result) return result;

	rcode futex_result = futex_wait(&c->_generation, expected);

	result = mutex_lock(m);
	if (result) return result;

	return futex_result;
}
