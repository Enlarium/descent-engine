#ifndef DESCENT_THREAD_RWLOCK_H
#define DESCENT_THREAD_RWLOCK_H

#include "../utilities/platform.h"
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <pthread.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <windows.h>
#endif


#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#define RWLOCK_INITIALIZER { PTHREAD_RWLOCK_INITIALIZER }
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#define RWLOCK_INITIALIZER { SRWLOCK_INIT }
#endif


typedef struct {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	pthread_rwlock_t _rwlock;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	SRWLOCK _rwlock;
#endif
} RWLock;


static inline int rwlock_init(RWLock *l) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_rwlock_init(&l->_rwlock, NULL);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	InitializeSRWLock(&l->_rwlock);
	return 0;
#endif
}

static inline int rwlock_destroy(RWLock *l) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_rwlock_destroy(&l->_rwlock);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	(void)l;
	return 0;
#endif
}

static inline int rwlock_read_lock(RWLock *l) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_rwlock_rdlock(&l->_rwlock);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	AcquireSRWLockShared(&l->_rwlock);
	return 0;
#endif
}

static inline int rwlock_read_trylock(RWLock *l) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_rwlock_tryrdlock(&l->_rwlock);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return !TryAcquireSRWLockShared(&l->_rwlock);
#endif
}

static inline int rwlock_read_unlock(RWLock *l) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_rwlock_unlock(&l->_rwlock);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	ReleaseSRWLockShared(&l->_rwlock);
	return 0;
#endif
}

static inline int rwlock_write_lock(RWLock *l) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_rwlock_wrlock(&l->_rwlock);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	AcquireSRWLockExclusive(&l->_rwlock);
	return 0;
#endif
}

static inline int rwlock_write_trylock(RWLock *l) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_rwlock_trywrlock(&l->_rwlock);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return !TryAcquireSRWLockExclusive(&l->_rwlock);
#endif
}

static inline int rwlock_write_unlock(RWLock *l) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_rwlock_unlock(&l->_rwlock);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	ReleaseSRWLockExclusive(&l->_rwlock);
	return 0;
#endif
}

#endif