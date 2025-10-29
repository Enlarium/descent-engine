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

#include <descent/thread/barrier.h>

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <pthread.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <windows.h>
#endif

#include "implementation.h"

_Static_assert(DESCENT_OPAQUE_SIZE_BARRIER >= sizeof(BarrierImplementation), "Barrier opaque type is too small for its implementation!");
_Static_assert(_Alignof(Barrier) >= _Alignof(BarrierImplementation), "Barrier opaque type is under-aligned for its implementation!");

int barrier_init(Barrier *ob, unsigned int count) {
	BarrierImplementation *b = (BarrierImplementation *)ob;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_barrier_init(&b->_barrier, NULL, count);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return !InitializeSynchronizationBarrier(&b->_barrier, (LONG)count, 0);
#endif
}

int barrier_destroy(Barrier *ob) {
	BarrierImplementation *b = (BarrierImplementation *)ob;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_barrier_destroy(&b->_barrier);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	DeleteSynchronizationBarrier(&b->_barrier);
	return 0;
#endif
}

int barrier_wait(Barrier *ob) {
	BarrierImplementation *b = (BarrierImplementation *)ob;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	int r = pthread_barrier_wait(&b->_barrier);
	if (r == 0 || r == PTHREAD_BARRIER_SERIAL_THREAD) return 0;
	return 1;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	(void) EnterSynchronizationBarrier(&b->_barrier, 0);
	return 0;
#endif
}
