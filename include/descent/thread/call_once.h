#ifndef DESCENT_THREAD_CALL_ONCE_H
#define DESCENT_THREAD_CALL_ONCE_H

#include "../utilities/opaque.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct CallOnce
 * @brief Ensures a function is executed only once across all threads.
 *
 * Useful for initializing global or shared resources in a thread-safe manner.
 * 
 * @note This mechanism is intra-process only. It cannot be shared between processes.
 */
DESCENT_OPAQUE_DEFINE(CallOnce, DESCENT_OPAQUE_SIZE_CALL_ONCE)

/**
 * @typedef CallOnceFunction
 * @brief Function type to call-once.
 */
typedef void (*CallOnceFunction)(void);

/**
 * @brief Initialize a call-once.
 * @param c Pointer to the CallOnce.
 * 
 * @note After call_once executes, re-initializing the same CallOnce object is undefined behavior.
 */
void call_once_init(CallOnce *c);

/**
 * @brief Executes a function exactly once across all threads.
 * @param c Pointer to the CallOnce.
 * @param f Function to execute.
 *
 * Subsequent calls after the first successful execution are no-ops.
 */
void call_once(CallOnce *c, CallOnceFunction f);

#ifdef __cplusplus
}
#endif

#endif