#ifndef DESCENT_THREAD_CONDITION_H
#define DESCENT_THREAD_CONDITION_H

#include "../utilities/opaque.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct Condition
 * @brief Condition variable.
 *
 * Allows threads to wait for a condition to become true and supports signaling
 * one or all waiting threads.
 * 
 * @note This mechanism is intra-process only. It cannot be shared between processes.
 */
DESCENT_OPAQUE_DEFINE(Condition, DESCENT_OPAQUE_SIZE_CONDITION)

/**
 * @brief Initializes a condition variable.
 * @param c Pointer to the Condition.
 * @return 0 on success, non-zero on failure.
 */
int condition_init(Condition *c);

/**
 * @brief Destroys a condition variable.
 * @param c Pointer to the Condition.
 * @return 0 on success, non-zero on failure.
 */
int condition_destroy(Condition *c);

/**
 * @brief Signals one thread waiting on the condition.
 * @param c Pointer to the Condition.
 * @return 0 on success, non-zero on failure.
 */
int condition_signal(Condition *c);

/**
 * @brief Signals all threads waiting on the condition.
 * @param c Pointer to the Condition.
 * @return 0 on success, non-zero on failure.
 */
int condition_broadcast(Condition *c);

#ifdef __cplusplus
}
#endif

#endif
