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

#include <descent/thread/call_once.h>
#include <intern/thread/call_once_u.h>

#include <stdbool.h>
#include <stdint.h>

#include <descent/rcode.h>
#include <descent/thread/atomic.h>
#include <descent/thread/futex.h>
#include <descent/utilities/builtin.h>
#include <intern/thread/hints.h>
#include <intern/thread/tid.h>

enum {
	CALL_ONCE_UNCALLED = 0,
	CALL_ONCE_CALLED,
	CALL_ONCE_COMPLETE,
};

rcode call_once(struct CallOnce *c, void (*f)(void)) {
	if (!c || !f) return DESCENT_ERROR_NULL;

	// Optimize for fast path where function is already called
	bool fast_path = (atomic_load_32(&c->_state, ATOMIC_ACQUIRE) == CALL_ONCE_COMPLETE);
	if (builtin_expect(fast_path, true)) {
		// Internal function record never changes after completion
		// Check that the provided function matches the called function
		int valid_function = atomic_load_ptr(&c->_function, ATOMIC_ACQUIRE) == (uintptr_t) f;
		return valid_function ? 0 : DESCENT_ERROR_INVALID;
	}

	// Check that the current thread has permission to call this function
	if (builtin_expect(tid_is_self(TID_NONE), false)) return DESCENT_ERROR_FORBIDDEN;
	
	uint32_t expected = CALL_ONCE_UNCALLED;
	if (atomic_compare_exchange_32(&c->_state, &expected, CALL_ONCE_CALLED, ATOMIC_RELEASE, ATOMIC_RELAXED)) {
		atomic_store_ptr (&c->_function, (uintptr_t) f, ATOMIC_RELEASE);
		atomic_store_64(&c->_owner, tid_self(), ATOMIC_RELEASE);

		f();

		atomic_store_64(&c->_owner, TID_NONE, ATOMIC_RELEASE);
		atomic_store_32(&c->_state, CALL_ONCE_COMPLETE, ATOMIC_RELEASE);

		rcode result = futex_wake_all(&c->_state);
		if (result) return result;
	}

	else if (atomic_load_64(&c->_owner, ATOMIC_ACQUIRE) == tid_self()) {
		return THREAD_ERROR_DEADLOCK;
	}

	else while (atomic_load_32(&c->_state, ATOMIC_ACQUIRE) != CALL_ONCE_COMPLETE) {
		// Since reversion from CALL_ONCE_CALLED to CALL_ONCE_UNCALLED is impossible,
		// the state is either CALL_ONCE_CALLED or CALL_ONCE_COMPLETE
		rcode result = futex_wait(&c->_state, CALL_ONCE_CALLED);
		if (result) return result;
	}

	int valid_function = atomic_load_ptr(&c->_function, ATOMIC_ACQUIRE) == (uintptr_t) f;
	return valid_function ? 0 : DESCENT_ERROR_INVALID;
}

void call_once_u(struct CallOnce_u *c, void (*f)(void)) {
	// Optimize for fast path
	bool fast_path = (atomic_load_32(&c->_state, ATOMIC_ACQUIRE) == CALL_ONCE_COMPLETE);
	if (builtin_expect(fast_path, true)) return;
	
	uint32_t expected = CALL_ONCE_UNCALLED;
	if (atomic_compare_exchange_32(&c->_state, &expected, CALL_ONCE_CALLED, ATOMIC_RELEASE, ATOMIC_RELAXED)) {
		f();
		atomic_store_32(&c->_state, CALL_ONCE_COMPLETE, ATOMIC_RELEASE);
		futex_wake_all(&c->_state);
	}

	else while (atomic_load_32(&c->_state, ATOMIC_ACQUIRE) != CALL_ONCE_COMPLETE) {
		// Since reversion from CALL_ONCE_CALLED to CALL_ONCE_UNCALLED is impossible,
		// the state is either CALL_ONCE_CALLED or CALL_ONCE_COMPLETE
		futex_wait(&c->_state, CALL_ONCE_CALLED);
	}
}
