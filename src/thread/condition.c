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

#include <descent/thread/condition.h>

#include <descent/rcode.h>
#include <descent/thread/atomic.h>
#include <descent/thread/futex.h>
#include <descent/utilities/builtin.h>
#include <intern/thread/tid.h>

rcode condition_signal(struct Condition *c) {
	if (!c) return DESCENT_ERROR_NULL;

	// Check that the current thread has permission to call this function
	if (builtin_expect(tid_is_self(TID_NONE), false)) return DESCENT_ERROR_FORBIDDEN;
	
	atomic_add_fetch_32(&c->_generation, 1, ATOMIC_RELAXED);

	return futex_wake_next(&c->_generation);
}

rcode condition_broadcast(struct Condition *c) {
	if (!c) return DESCENT_ERROR_NULL;

	// Check that the current thread has permission to call this function
	if (builtin_expect(tid_is_self(TID_NONE), false)) return DESCENT_ERROR_FORBIDDEN;
	
	atomic_add_fetch_32(&c->_generation, 1, ATOMIC_RELAXED);

	return futex_wake_all(&c->_generation);
}
