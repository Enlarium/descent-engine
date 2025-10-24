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

#ifndef DESCENT_SOURCE_THREAD_THREAD_HANDLE_H
#define DESCENT_SOURCE_THREAD_THREAD_HANDLE_H

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <pthread.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <windows.h>
#endif

#include <descent/thread/atomic.h>

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

typedef pthread_t thread_handle;
typedef _Atomic thread_handle atomic_thread_handle;

static inline thread_handle atomic_load_thread_handle(atomic_thread_handle *object) {
	return atomic_load(object);
}

static inline void atomic_store_thread_handle(atomic_thread_handle *object, thread_handle desired) {
	atomic_store(object, desired);
}

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

typedef HANDLE thread_handle;
typedef atomic_ptr atomic_thread_handle;

static inline thread_handle atomic_load_thread_handle(atomic_thread_handle *object) {
	return (thread_handle) atomic_load_ptr(object);
}

static inline void atomic_store_thread_handle(atomic_thread_handle *object, thread_handle desired) {
	atomic_store_ptr(object, (thread_handle) desired);
}

#endif

#endif