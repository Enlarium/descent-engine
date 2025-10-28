#ifndef DESCENT_THREAD_BARRIER_H
#define DESCENT_THREAD_BARRIER_H

#include "../utilities/opaque.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct Barrier
 * @brief Thread barrier.
 *
 * Synchronizes a group of threads, making them wait until all threads have reached
 * the barrier point before any of them proceed.
 * 
 * @note This mechanism is intra-process only. It cannot be shared between processes.
 */
DESCENT_OPAQUE_DEFINE(Barrier, DESCENT_OPAQUE_SIZE_BARRIER)

/**
 * @brief Initialize a barrier.
 * 
 * @param b Pointer to the Barrier.
 * @param count Number of participating threads
 * @return 0 on success, non-zero on failure. 
 */
int barrier_init(Barrier *b, unsigned int count);

/**
 * @brief Destroy a barrier.
 * 
 * @param b Pointer to the Barrier.
 * @return 0 on success, non-zero on failure.
 */
int barrier_destroy(Barrier *b);

/**
 * @brief Wait at the barrier.
 * 
 * Block until 'count' threads have called wait.
 * 
 * @param b 
 * @return 0 on success, non-zero on failure. 
 */
int barrier_wait(Barrier *b);

#ifdef __cplusplus
}
#endif

#endif
