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

#include <descent/thread/semaphore.h>

#include <stdint.h>
#include <stdbool.h>

#include <descent/rcode.h>
#include <descent/thread/atomic.h>
#include <descent/thread/condition.h>
#include <descent/thread/futex.h>

int semaphore_wait(struct Semaphore *s) {
	if (!s) return DESCENT_ERROR_NULL;
	
	for (;;)  {
		uint32_t count = atomic_load_32(&s->_count, ATOMIC_RELAXED);

		if (count > 0) {
			if (atomic_compare_exchange_32(&s->_count, &count, count - 1, ATOMIC_ACQUIRE, ATOMIC_RELAXED)) {
				return 0;
			}
		}

		else {
			rcode result = futex_wait(&s->_count, 0);
			if (result) return result;
		}
	}
}

int semaphore_trywait(struct Semaphore *s) {
	if (!s) return DESCENT_ERROR_NULL;

	uint32_t count = atomic_load_32(&s->_count, ATOMIC_RELAXED);

	if (count > 0) {
		if (atomic_compare_exchange_32(&s->_count, &count, count - 1, ATOMIC_ACQUIRE, ATOMIC_RELAXED)) {
			return 0;
		}
	}

	return THREAD_INFO_BUSY;
}

int semaphore_signal(struct Semaphore *s) {
	if (!s) return DESCENT_ERROR_NULL;
	
	for (;;) {
		uint32_t count = atomic_load_32(&s->_count, ATOMIC_RELAXED);

		if (count >= s->_maximum) return DESCENT_ERROR_OVERFLOW;

		if (atomic_compare_exchange_32(&s->_count, &count, count + 1, ATOMIC_RELEASE, ATOMIC_RELAXED)) {
			return futex_wake_next(&s->_count);
		}
	}
}
