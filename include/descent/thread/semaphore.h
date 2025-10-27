#ifndef DESCENT_THREAD_SEMAPHORE_H
#define DESCENT_THREAD_SEMAPHORE_H

#include "../utilities/platform.h"
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <semaphore.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <windows.h>
#endif



#ifdef __cplusplus
extern "C" {
#endif



/**
 * @struct Semaphore
 * @brief A counting semaphore.
 *
 * Represents a counting semaphore that can be used to control access to
 * a finite number of shared resources.
 * 
 * @note This mechanism is intra-process only. It cannot be shared between processes.
 */
typedef struct {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	sem_t _semaphore;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	HANDLE _semaphore;
#endif
} Semaphore;



/**
 * @brief Initialize a semaphore.
 * 
 * @param s Pointer to the Semaphore.
 * @param initial_count Initial value of the semaphore counter.
 * @return 0 on success, non-zero on failure.
 */
static inline int semaphore_init(Semaphore *s, unsigned int initial_count) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!sem_init(&s->_semaphore, 0, initial_count);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	s->_semaphore = CreateSemaphoreW(NULL, (LONG)initial_count, LONG_MAX, NULL);
	return !s->_semaphore;
#endif
}

/**
 * @brief Destroy a semaphore.
 * 
 * @param s Pointer to the Semaphore.
 * @return 0 on success, non-zero on failure. 
 */
static inline int semaphore_destroy(Semaphore *s) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!sem_destroy(&s->_semaphore);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return !CloseHandle(s->_semaphore);
#endif
}

/**
 * @brief Lock a semaphore.
 * 
 * Decrements the semaphore counter, blocking if the counter is zero until another
 * thread posts to it.
 * 
 * @param s Pointer to the Semaphore.
 * @return 0 on success, non-zero on failure. 
 */
static inline int semaphore_wait(Semaphore *s) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!sem_wait(&s->_semaphore);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return WaitForSingleObject(s->_semaphore, INFINITE) != WAIT_OBJECT_0;
#endif
}

/**
 * @brief Try to wait (non-blocking) on a semaphore.
 *
 * Attempts to decrement the semaphore counter without blocking.
 *
 * @param s Pointer to the Semaphore structure.
 * @return 0 if the semaphore was successfully decremented, non-zero if the semaphore i already at zero.
 */
static inline int semaphore_trywait(Semaphore *s) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!sem_trywait(&s->_semaphore);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return WaitForSingleObject(s->_semaphore, 0) != WAIT_OBJECT_0;
#endif
}

/**
 * @brief Post (increment) a semaphore.
 *
 * Increments the semaphore counter, potentially unblocking a thread waiting on it.
 *
 * @param s Pointer to the Semaphore structure.
 * @return 0 on success, non-zero on failure.
 */
static inline int semaphore_post(Semaphore *s) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!sem_post(&s->_semaphore);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return !ReleaseSemaphore(s->_semaphore, 1, NULL);
#endif
}



#ifdef __cplusplus
}
#endif



#endif