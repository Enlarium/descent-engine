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

#include <descent/thread/semaphore.h>

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <pthread.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <windows.h>
#endif

#include "implementation.h"

_Static_assert(DESCENT_OPAQUE_SIZE_SEMAPHORE >= sizeof(SemaphoreImplementation), "Semaphore opaque type is too small for its implementation!");
_Static_assert(_Alignof(Semaphore) >= _Alignof(SemaphoreImplementation), "Semaphore opaque type is under-aligned for its implementation!");

int semaphore_init(Semaphore *os, unsigned int initial_count) {
	SemaphoreImplementation *s = (SemaphoreImplementation *)os;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!sem_init(&s->_semaphore, 0, initial_count);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	s->_semaphore = CreateSemaphoreW(NULL, (LONG)initial_count, LONG_MAX, NULL);
	return !s->_semaphore;
#endif
}

int semaphore_destroy(Semaphore *os) {
	SemaphoreImplementation *s = (SemaphoreImplementation *)os;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!sem_destroy(&s->_semaphore);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return !CloseHandle(s->_semaphore);
#endif
}

int semaphore_wait(Semaphore *os) {
	SemaphoreImplementation *s = (SemaphoreImplementation *)os;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!sem_wait(&s->_semaphore);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return WaitForSingleObject(s->_semaphore, INFINITE) != WAIT_OBJECT_0;
#endif
}

int semaphore_trywait(Semaphore *os) {
	SemaphoreImplementation *s = (SemaphoreImplementation *)os;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!sem_trywait(&s->_semaphore);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return WaitForSingleObject(s->_semaphore, 0) != WAIT_OBJECT_0;
#endif
}

int semaphore_post(Semaphore *os) {
	SemaphoreImplementation *s = (SemaphoreImplementation *)os;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!sem_post(&s->_semaphore);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return !ReleaseSemaphore(s->_semaphore, 1, NULL);
#endif
}
