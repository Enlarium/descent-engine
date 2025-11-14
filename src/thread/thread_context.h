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

#ifndef DESCENT_SOURCE_THREAD_THREAD_CONTEXT_H
#define DESCENT_SOURCE_THREAD_THREAD_CONTEXT_H

#include <descent/thread/atomic.h>
#include <descent/thread/thread.h>

#include "thread_handle.h"

#ifndef DESCENT_MAX_THREADS
#define DESCENT_MAX_THREADS 32
#endif

typedef enum {
	THREAD_STATE_INVALID = -1,
	THREAD_STATE_UNUSED = 0,
	THREAD_STATE_RESERVED,
	THREAD_STATE_RUNNING,
	THREAD_STATE_FINISHED,
	THREAD_STATE_DETACHED,
	THREAD_STATE_JOINING
} ThreadState;

typedef struct {
	atomic_thread_handle  handle;
	ThreadFunction        function;
	void                 *argument;
	atomic_64             meta;
} ThreadContext;

static inline const char *thread_state(int s) {
	switch (s) {
		case THREAD_STATE_UNUSED:   return "THREAD_STATE_UNUSED";
		case THREAD_STATE_RESERVED: return "THREAD_STATE_RESERVED";
		case THREAD_STATE_RUNNING:  return "THREAD_STATE_RUNNING";
		case THREAD_STATE_FINISHED: return "THREAD_STATE_FINISHED";
		case THREAD_STATE_DETACHED: return "THREAD_STATE_DETACHED";
		case THREAD_STATE_JOINING:  return "THREAD_STATE_JOINING";
		default:                    return "THREAD_STATE_INVALID";
	}
}

// Strips "THREAD_STATE_" prefix from thread_state function
static inline const char *thread_state_short(int s) {
	// The null terminator "stands in" for the last underscore
	return thread_state(s) + sizeof("THREAD_STATE");
}

static inline uint32_t thread_index(Thread t) {
	return (uint32_t) t;
}

static inline uint32_t thread_generation(Thread t) {
	return (uint32_t) (t >> 32);
}

static inline Thread thread_construct(uint32_t index, uint32_t generation) {
	return ((Thread) generation << 32) | (Thread) index;
}

static inline int thread_is_managed(Thread t) {
	uint32_t index = thread_index(t);
	return (index < DESCENT_MAX_THREADS);
}

static inline uint32_t thread_context_generation(uint64_t meta) {
	return (uint32_t) meta;
}

static inline uint32_t thread_context_state(uint64_t meta) {
	return (uint32_t) (meta >> 32);
}

static inline uint64_t thread_context_construct(uint32_t state, uint32_t generation) {
	return (((uint64_t) state) << 32 | generation);
}

#endif