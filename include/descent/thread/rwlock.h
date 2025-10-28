#ifndef DESCENT_THREAD_RWLOCK_H
#define DESCENT_THREAD_RWLOCK_H

#include "../utilities/platform.h"
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <pthread.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <windows.h>
#endif



#ifdef __cplusplus
extern "C" {
#endif



#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#define RWLOCK_INITIALIZER { PTHREAD_RWLOCK_INITIALIZER }
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#define RWLOCK_INITIALIZER { SRWLOCK_INIT }
#endif

// static initialization functions, no init?



/**
 * @struct RWLock
 * @brief A read-write lock for synchronizing access to shared resources.
 *
 * Supports multiple readers or one writer at a time.
 * 
 * @note This mechanism is intra-process only. It cannot be shared between processes.
 */
typedef struct {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	pthread_rwlock_t _lock;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	SRWLOCK _lock;
#endif
} RWLock;



/**
 * @brief Initialize a read-write lock.
 * @param l Pointer to the RWLock.
 * @return 0 on success, non-zero on failure.
 */
static inline int rwlock_init(RWLock *l) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_rwlock_init(&l->_lock, NULL);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	InitializeSRWLock(&l->_lock);
	return 0;
#endif
}

/**
 * @brief Destroy a read-write lock.
 * @param l Pointer to the RWLock.
 * @return 0 on success, non-zero on failure.
 */
static inline int rwlock_destroy(RWLock *l) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_rwlock_destroy(&l->_lock);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	(void)l;
	return 0;
#endif
}

/**
 * @brief Acquire a read (shared) lock.
 * @param l Pointer to the RWLock.
 * @return 0 on success, non-zero on failure.
 *
 * Multiple threads can hold a read lock simultaneously.
 */
static inline int rwlock_read_lock(RWLock *l) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_rwlock_rdlock(&l->_lock);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	AcquireSRWLockShared(&l->_lock);
	return 0;
#endif
}

/**
 * @brief Attempt to acquire a read (shared) lock without blocking.
 * @param l Pointer to the RWLock.
 * @return 0 if lock acquired, non-zero if already held.
 */
static inline int rwlock_read_trylock(RWLock *l) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_rwlock_tryrdlock(&l->_lock);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return !TryAcquireSRWLockShared(&l->_lock);
#endif
}


/**
 * @brief Release a read (shared) lock.
 * @param l Pointer to the RWLock.
 * @return 0 on success, non-zero on failure.
 */
static inline int rwlock_read_unlock(RWLock *l) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_rwlock_unlock(&l->_lock);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	ReleaseSRWLockShared(&l->_lock);
	return 0;
#endif
}

/**
 * @brief Acquire a write (exclusive) lock.
 * @param l Pointer to the RWLock.
 * @return 0 on success, non-zero on failure.
 *
 * Only one thread can hold the write lock at a time. Blocks readers and other writers.
 */
static inline int rwlock_write_lock(RWLock *l) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_rwlock_wrlock(&l->_lock);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	AcquireSRWLockExclusive(&l->_lock);
	return 0;
#endif
}

/**
 * @brief Attempt to acquire a write (exclusive) lock without blocking.
 * @param l Pointer to the RWLock.
 * @return 0 if lock acquired, non-zero if already held.
 */
static inline int rwlock_write_trylock(RWLock *l) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_rwlock_trywrlock(&l->_lock);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return !TryAcquireSRWLockExclusive(&l->_lock);
#endif
}

/**
 * @brief Release a write (exclusive) lock.
 * @param l Pointer to the RWLock.
 * @return 0 on success, non-zero on failure.
 */
static inline int rwlock_write_unlock(RWLock *l) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_rwlock_unlock(&l->_lock);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	ReleaseSRWLockExclusive(&l->_lock);
	return 0;
#endif
}



#ifdef __cplusplus
}
#endif



#endif