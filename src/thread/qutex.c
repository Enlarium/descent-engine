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

#include <descent/thread/qutex.h>

#include <stdint.h>
#include <stddef.h>

#include <descent/rcode.h>
#include <descent/thread/atomic.h>
#include <descent/thread/futex.h>
#include <descent/thread/tls.h>

struct QutexNode {
	atomic_ptr _next;
	atomic_32 _state;
};

// If a thread is waiting on one qutex, it can't be waiting on another.
static TLS struct QutexNode node;

enum {
	QUTEX_UNUSED = 0,
	QUTEX_WAITING,
	QUTEX_READY,
};

rcode qutex_lock(struct Qutex *q) {
	if (!q) return DESCENT_ERROR_NULL;

	const uintptr_t SELF = (uintptr_t) &node;

	// Check to see if this thread is already waiting 
	uint32_t expected_state = QUTEX_UNUSED;
	if (!atomic_compare_exchange_32(&node._state, &expected_state, QUTEX_WAITING, ATOMIC_RELAXED, ATOMIC_RELAXED)) {
		return THREAD_ERROR_DEADLOCK;
	}

	// Initialize next node
	atomic_store_ptr(&node._next, (uintptr_t) NULL, ATOMIC_RELAXED);

	// Place self at tail
	uintptr_t previous = atomic_exchange_ptr(&q->_tail, SELF, ATOMIC_ACQ_REL);
	struct QutexNode *last_node = (struct QutexNode *)previous;

	// If queue was empty, we own the lock
	if (last_node == NULL) return 0;

	// Enter the queue at end
	atomic_store_ptr(&last_node->_next, SELF, ATOMIC_RELEASE);
	
	// Wait until predecessor releases
	while (atomic_load_32(&node._state, ATOMIC_ACQUIRE) == QUTEX_WAITING) {
		rcode result = futex_wait(&node._state, QUTEX_WAITING);
		if (result) return result;
	}

	return 0;
}

rcode qutex_trylock(struct Qutex *q) {
	if (!q) return DESCENT_ERROR_NULL;

	const uintptr_t SELF = (uintptr_t) &node;

	// Check to see if this thread is already waiting 
	uint32_t expected_state = QUTEX_UNUSED;
	if (!atomic_compare_exchange_32(&node._state, &expected_state, QUTEX_WAITING, ATOMIC_RELAXED, ATOMIC_RELAXED)) {
		return THREAD_ERROR_DEADLOCK;
	}

	// Initialize next node
	atomic_store_ptr(&node._next, (uintptr_t) NULL, ATOMIC_RELAXED);

	// Place self at tail
	uintptr_t expected_tail = 0;
	if (!atomic_compare_exchange_ptr(&q->_tail, &expected_tail, SELF, ATOMIC_ACQ_REL, ATOMIC_ACQUIRE)) {
		// Lock is busy
		atomic_store_32(&node._state, QUTEX_UNUSED, ATOMIC_RELEASE);
		return THREAD_INFO_BUSY;
	}

	return 0;
}

rcode qutex_unlock(struct Qutex *q) {
	if (!q) return DESCENT_ERROR_NULL;

	const uintptr_t SELF = (uintptr_t) &node;

	uintptr_t successor = atomic_load_ptr(&node._next, ATOMIC_ACQUIRE);

	if (!successor) {
		// Try to reset the queue
		uintptr_t expected = SELF;
		if (atomic_compare_exchange_ptr(&q->_tail, &expected, (uintptr_t) NULL, ATOMIC_ACQ_REL, ATOMIC_ACQUIRE)) {
			return 0;
		}

		// Wait for successor to link in
		do {
			successor = atomic_load_ptr(&node._next, ATOMIC_ACQUIRE);
		} while (!successor);
	}

	// Pass ownership
	struct QutexNode *next_node = (struct QutexNode *)successor;
	atomic_store_32(&next_node->_state, QUTEX_READY, ATOMIC_RELEASE);
	
	// Mark self as unused
	atomic_store_32(&node._state, QUTEX_UNUSED, ATOMIC_RELEASE);

	return futex_wake_next(&next_node->_state);
}
