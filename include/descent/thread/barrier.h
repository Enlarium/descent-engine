#ifndef DESCENT_THREAD_BARRIER_H
#define DESCENT_THREAD_BARRIER_H

#include "../utilities/platform.h"
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <pthread.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <windows.h>
#endif



#ifdef __cplusplus
extern "C" {
#endif



typedef struct {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	pthread_barrier_t _barrier;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	SYNCHRONIZATION_BARRIER _barrier;
#endif
} Barrier;



/**
 * @brief Initialize a barrier
 * 
 * @param b Pointer to the Barrier.
 * @param count Number of participating threads
 * @return 0 on success, non-zero on failure. 
 */
static inline int barrier_init(Barrier *b, unsigned int count) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_barrier_init(&b->_barrier, NULL, count);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return !InitializeSynchronizationBarrier(&b->_barrier, (LONG)count, 0);
#endif
}

/**
 * @brief Destroy a barrier.
 * 
 * @param b Pointer to the Barrier.
 * @return 0 on success, non-zero on failure.
 */
static inline int barrier_destroy(Barrier *b) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_barrier_destroy(&b->_barrier);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	/* DeleteSynchronizationBarrier returns void. Call it unconditionally. */
	DeleteSynchronizationBarrier(&b->_barrier);
	return 0;
#endif
}

/**
 * @brief Wait at the barrier.
 * 
 * Block until 'count' threads have called wait.
 * 
 * @param b 
 * @return 0 on success, non-zero on failure. 
 */
static inline int barrier_wait(Barrier *b) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	int r = pthread_barrier_wait(&b->_barrier);
	if (r == 0 || r == PTHREAD_BARRIER_SERIAL_THREAD) return 0;
	return 1;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	(void) EnterSynchronizationBarrier(&b->_barrier, 0);
	return 0;
#endif
}



#ifdef __cplusplus
}
#endif



#endif
