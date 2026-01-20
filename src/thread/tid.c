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

#include <intern/thread/tid.h>

#include <descent/rcode.h>
#include <descent/thread/atomic.h>
#include <descent/thread/tls.h>

static TLS thread_id self = TID_NONE;

static atomic_64 assigned_tid_set = {0};

static inline rcode tid_assign_checked(thread_id t) {
	if (self != TID_NONE) return DESCENT_ERROR_INIT;
	thread_id_set previous = atomic_fetch_or_64(&assigned_tid_set, t, ATOMIC_RELEASE);
	// If a collision is detected, the bit was already set for another thread.
	// Clearing it would artificially indicate that it is not held by that other
	// thread, so we do not revert.
	if (t & previous) return DESCENT_ERROR_STATE;
	self = t;
	return 0;
}

thread_id tid_self(void) {
	return self;
}

bool tid_is_self(thread_id t) {
	return self == t;
}

bool tid_is_assigned(thread_id t) {
	thread_id_set assigned = atomic_load_64(&assigned_tid_set, ATOMIC_ACQUIRE);
	return (t & assigned) && !(t & (t - 1));
}

rcode tid_assign(thread_id t) {
	if (t == TID_NONE) return DESCENT_ERROR_INVALID;
	return tid_assign_checked(t);
}

rcode tid_assign_main(void) {
	return tid_assign_checked(TID_MAIN);
}

rcode tid_assign_unique(unsigned int index) {
	thread_id t = tid_generate_unique(index);
	return tid_assign(t);
}

rcode tid_assign_worker(unsigned int index) {
	thread_id t = tid_generate_worker(index);
	return tid_assign(t);
}

void tid_assign_clear(void) {
	atomic_fetch_and_64(&assigned_tid_set, ~self, ATOMIC_RELEASE);
	self = TID_NONE;
}

bool tid_set_contains_self(thread_id_set s) {
	return tid_set_contains(s, self);
}
