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

#include <descent/thread/rwlock.h>

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <pthread.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <windows.h>
#endif

#include "implementation.h"

_Static_assert(DESCENT_OPAQUE_SIZE_RWLOCK >= sizeof(RWLockImplementation), "Read-write lock opaque type is too small for its implementation!");
_Static_assert(_Alignof(RWLock) >= _Alignof(RWLockImplementation), "Read-write lock opaque type is under-aligned for its implementation!");

int rwlock_init(RWLock *ol) {
	RWLockImplementation *l = (RWLockImplementation *)ol;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_rwlock_init(&l->_lock, NULL);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	InitializeSRWLock(&l->_lock);
	return 0;
#endif
}

int rwlock_destroy(RWLock *ol) {
	RWLockImplementation *l = (RWLockImplementation *)ol;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_rwlock_destroy(&l->_lock);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	(void)l;
	return 0;
#endif
}

int rwlock_read_lock(RWLock *ol) {
	RWLockImplementation *l = (RWLockImplementation *)ol;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_rwlock_rdlock(&l->_lock);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	AcquireSRWLockShared(&l->_lock);
	return 0;
#endif
}

int rwlock_read_trylock(RWLock *ol) {
	RWLockImplementation *l = (RWLockImplementation *)ol;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_rwlock_tryrdlock(&l->_lock);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return !TryAcquireSRWLockShared(&l->_lock);
#endif
}

int rwlock_read_unlock(RWLock *ol) {
	RWLockImplementation *l = (RWLockImplementation *)ol;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_rwlock_unlock(&l->_lock);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	ReleaseSRWLockShared(&l->_lock);
	return 0;
#endif
}

int rwlock_write_lock(RWLock *ol) {
	RWLockImplementation *l = (RWLockImplementation *)ol;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_rwlock_wrlock(&l->_lock);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	AcquireSRWLockExclusive(&l->_lock);
	return 0;
#endif
}

int rwlock_write_trylock(RWLock *ol) {
	RWLockImplementation *l = (RWLockImplementation *)ol;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_rwlock_trywrlock(&l->_lock);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return !TryAcquireSRWLockExclusive(&l->_lock);
#endif
}

int rwlock_write_unlock(RWLock *ol) {
	RWLockImplementation *l = (RWLockImplementation *)ol;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_rwlock_unlock(&l->_lock);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	ReleaseSRWLockExclusive(&l->_lock);
	return 0;
#endif
}
