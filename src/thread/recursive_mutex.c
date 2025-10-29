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

#include <descent/thread/recursive_mutex.h>

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <pthread.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <windows.h>
#endif

#include "implementation.h"

_Static_assert(DESCENT_OPAQUE_SIZE_RECURSIVE_MUTEX >= sizeof(RecursiveMutexImplementation), "Recursive mutex opaque type is too small for its implementation!");
_Static_assert(_Alignof(RecursiveMutex) >= _Alignof(RecursiveMutexImplementation), "Recursive mutex opaque type is under-aligned for its implementation!");

int recursive_mutex_init(RecursiveMutex *om) {
	RecursiveMutexImplementation *m = (RecursiveMutexImplementation *)om;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	pthread_mutexattr_t attr;
	if (!pthread_mutexattr_init(&attr)) {
		if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE)) return 1;
		
		int result = !!pthread_mutex_init(&m->_mutex, &attr);
		pthread_mutexattr_destroy(&attr);
		return result;
	}
	return 1;

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	InitializeCriticalSection(&m->_mutex);
	return 0;
#endif
}

int recursive_mutex_destroy(RecursiveMutex *om) {
	RecursiveMutexImplementation *m = (RecursiveMutexImplementation *)om;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_destroy(&m->_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	DeleteCriticalSection(&m->_mutex);
	return 0;
#endif
}

int recursive_mutex_lock(RecursiveMutex *om) {
	RecursiveMutexImplementation *m = (RecursiveMutexImplementation *)om;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_lock(&m->_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	EnterCriticalSection(&m->_mutex);
	return 0;
#endif
}

int recursive_mutex_trylock(RecursiveMutex *om) {
	RecursiveMutexImplementation *m = (RecursiveMutexImplementation *)om;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_trylock(&m->_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return !TryEnterCriticalSection(&m->_mutex);
#endif
}

int recursive_mutex_unlock(RecursiveMutex *om) {
	RecursiveMutexImplementation *m = (RecursiveMutexImplementation *)om;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_unlock(&m->_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	LeaveCriticalSection(&m->_mutex);
	return 0;
#endif
}
