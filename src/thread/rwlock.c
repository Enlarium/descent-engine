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
#include <descent/thread/tls.h>
#include <intern/thread/tid.h>

#define WRITE_STATE_BIT (1u << 31)
#define WRITE_PEND_BIT  (1u << 30)
#define MAXIMUM_READERS (THREAD_MAX > 0xFF ? 0xFF : THREAD_MAX)

_Static_assert(WRITE_STATE_BIT > MAXIMUM_READERS, "WRITE_STATE_BIT must be greater than MAXIMUM_READERS");
_Static_assert(WRITE_PEND_BIT > MAXIMUM_READERS, "WRITE_PEND_BIT must be greater than MAXIMUM_READERS");

enum {
	RWLOCK_UNUSED = 0,
	RWLOCK_READING,
	RWLOCK_WRITING,
};

struct RWLock {
	atomic_32 _state;   // Bits: [WRITE_STATE_BIT | WRITE_PEND_BIT | reader_count (30 bits)]
	atomic_64 _owner;   // Bitmask of owners (threads holding this lock)
};

// The highest bit of _state marks whether the lock is in read or write mode
// The second-highest bit of _state marks whether the lock is in read or write mode
// The lower 30 bits of _state count lock holders

// Considerations:
// There can be multiple locks
// A thread can hold one (or more) locks and try to obtain another
// Locks must be well-behaved:
// - Forbid waiting on a held lock
// - Forbid unlocking a lock not held

// Atomically switch lock from write to read
// Prefers writers initially, uses eventual fairness once the lock has been held by a
// single writer for more than 1 ms

static inline rcode _rwlock_read_lock_base(struct RWLock *r, bool try) {
	if (!r) return DESCENT_ERROR_NULL;

	for (;;) {
		uint32_t state = atomic_load_32(&r->_state, ATOMIC_ACQUIRE);

		// This triggers if:
		// - WRITE_STATE_BIT is set
		// - WRITE_PEND_BIT is set
		// - MAXIMUM_READERS is exceeded
		bool busy = state > MAXIMUM_READERS;

		if (busy) {
			if (try) {
				return THREAD_INFO_BUSY;
			} else {
				futex_wait(&r->_state, state);
			}
		}

		else if (atomic_compare_exchange_32(&r->_state, &state, state + 1, ATOMIC_ACQ_REL, ATOMIC_ACQUIRE)) {
			break;
		}
	}

	return 0;
}

static inline rcode _rwlock_write_lock_base(struct RWLock *r, bool try) {
	if (!r) return DESCENT_ERROR_NULL;
	if (thread_state != RWLOCK_UNUSED) return THREAD_ERROR_DEADLOCK;

	// Place a pend on the lock
	atomic_or_fetch_32(&r->_state, WRITE_PEND_BIT, ATOMIC_RELEASE);

	for (;;) {
		uint32_t state = WRITE_PEND_BIT;

		if (atomic_compare_exchange_32(&r->_state, &state, WRITE_STATE_BIT, ATOMIC_ACQ_REL, ATOMIC_ACQUIRE)) {
			break;
		}

		rcode result = futex_wait(&r->_state, state);
		if (result) return result;
	}

	thread_state = RWLOCK_WRITING;
	return 0;
}

rcode rwlock_read_lock(struct RWLock *r) {
	return _rwlock_read_lock_base(r, false);
}

rcode rwlock_read_trylock(struct RWLock *r) {
	return _rwlock_read_lock_base(r, true);
}

rcode rwlock_read_unlock(struct RWLock *r) {
	if (!r) return DESCENT_ERROR_NULL;

	// Writer bit is never set while any readers exist
	if (thread_state != RWLOCK_READING) return DESCENT_ERROR_FORBIDDEN;

	uint32_t state = atomic_fetch_sub_32(&r->_state, 1, ATOMIC_ACQ_REL);

	uint32_t readers = state & MAXIMUM_READERS;
	if (readers == 1u) {
		futex_wake_all(&r->_state);
	}

	thread_state = RWLOCK_UNUSED;
	return 0;
}

rcode rwlock_write_lock(struct RWLock *r) {
	return _rwlock_write_lock_base(r, false);
}

rcode rwlock_write_trylock(struct RWLock *r) {
	return _rwlock_write_lock_base(r, true);
}

rcode rwlock_downlock(struct RWLock *r) {
	if (!r) return DESCENT_ERROR_NULL;

	if (thread_state != RWLOCK_WRITING) return DESCENT_ERROR_FORBIDDEN;

	// TODO
	return DESCENT_ERROR_UNSUPPORTED;
}

rcode rwlock_write_unlock(struct RWLock *r) {
	if (!r) return DESCENT_ERROR_NULL;

	if (thread_state != RWLOCK_WRITING) return DESCENT_ERROR_FORBIDDEN;

	atomic_store_32(&r->_state, 0u, ATOMIC_RELEASE);
	futex_wake_all(&r->_state);

	thread_state = RWLOCK_UNUSED;
	return 0;
}

rcode mutex_lock(struct Mutex *m) {
	if (!m) return DESCENT_ERROR_NULL;

	uint32_t expected = MUTEX_UNLOCKED;
	if (atomic_compare_exchange_32(&m->_state, &expected, MUTEX_LOCKED, ATOMIC_ACQ_REL, ATOMIC_ACQUIRE)) {
		atomic_store_32(&m->_owner, ftid_self(), ATOMIC_RELEASE);
		return 0;
	}

	if (atomic_load_32(&m->_owner, ATOMIC_ACQUIRE) == ftid_self()) return THREAD_ERROR_DEADLOCK;

	do {
		expected = MUTEX_LOCKED;
		bool exchange = atomic_compare_exchange_32(&m->_state, &expected, MUTEX_CONTENDED, ATOMIC_ACQ_REL, ATOMIC_ACQUIRE);

		if (exchange || expected == MUTEX_CONTENDED) {
			futex_wait(&m->_state, MUTEX_CONTENDED);
		}

		expected = MUTEX_UNLOCKED;
	} while (atomic_compare_exchange_32(&m->_state, &expected, MUTEX_CONTENDED, ATOMIC_ACQ_REL, ATOMIC_ACQUIRE));

	atomic_store_32(&m->_owner, ftid_self(), ATOMIC_RELEASE);

	return 0;
}

rcode mutex_trylock(struct Mutex *m) {
	if (!m) return DESCENT_ERROR_NULL;

	uint32_t expected = MUTEX_UNLOCKED;
	if (atomic_compare_exchange_32(&m->_state, &expected, MUTEX_LOCKED, ATOMIC_ACQ_REL, ATOMIC_ACQUIRE)) {
		atomic_store_32(&m->_owner, ftid_self(), ATOMIC_RELEASE);
		return 0;
	}

	return DESCENT_WARN_BUSY;
}

rcode mutex_unlock(struct Mutex *m) {
	if (!m) return DESCENT_ERROR_NULL;

	// Only the owner can mutate the _owner field
	uint32_t self = ftid_self();
	if (!atomic_compare_exchange_32(&m->_owner, &self, FTID_NONE, ATOMIC_ACQ_REL, ATOMIC_ACQUIRE)) {
		return DESCENT_ERROR_FORBIDDEN;
	}

	if (atomic_fetch_sub_32(&m->_state, 1, ATOMIC_RELEASE)) {
		atomic_store_32(&m->_state, 0, ATOMIC_RELEASE);
		futex_wake_next(&m->_state);
	}

	return 0;
}

rcode mutex_wait(struct Mutex *m, struct Condition *c) {
	if (!m || !c) return DESCENT_ERROR_NULL;

	uint32_t expected = atomic_load_32(&c->_generation, ATOMIC_RELAXED);
	
	rcode result = mutex_unlock(m);
	if (result) return result;

	futex_wait(&c->_generation, expected);

	return mutex_lock(m);
}