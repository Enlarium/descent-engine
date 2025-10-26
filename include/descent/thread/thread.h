#ifndef DESCENT_THREAD_THREAD_H
#define DESCENT_THREAD_THREAD_H

#include <stdint.h>



// Threads created through the Descent thread system must not be manipulated
// the underlying thread API or other thread libraries. Doing so results in
// undefined behavior.



#ifdef __cplusplus
extern "C" {
#endif



#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#define TLS _Thread_local
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#define TLS __declspec(thread)
#endif

/**
 * @enum ThreadError
 * @brief Defines error codes returned by thread functions.
 */
typedef enum {
	THREAD_ERROR_UNKNOWN     = -1, /**< Unknown error occurred */
	THREAD_SUCCESS           = 0,  /**< Operation succeeded */
	THREAD_ERROR_NO_SLOTS,         /**< No thread slots available */
	THREAD_ERROR_HANDLE_INVALID,   /**< Thread handle is invalid */
	THREAD_ERROR_HANDLE_UNMANAGED, /**< Operation not allowed on unmanaged thread */
	THREAD_ERROR_HANDLE_DETACHED,  /**< Operation not allowed on detached thread */
	THREAD_ERROR_HANDLE_CLOSED,    /**< Operation on a closed thread */
	THREAD_ERROR_FUNCTION_INVALID, /**< Thread function pointer is invalid */
	THREAD_ERROR_OS_CREATE_FAILED, /**< Underlying OS thread creation failed */
	THREAD_ERROR_OS_YIELD_FAILED,  /**< Yielding the thread failed */
	THREAD_ERROR_OS_JOIN_FAILED,   /**< Joining the thread failed */
	THREAD_ERROR_OS_DETACH_FAILED, /**< Detaching the thread failed */
} ThreadError;

/**
 * @enum ThreadPriority
 * @brief Thread priority levels.
 */
typedef enum {
	THREAD_PRIORITY_LOW     = -1, /**< Low priority thread */
	THREAD_PRIORITY_DEFAULT = 0,  /**< Default priority thread */
	THREAD_PRIORITY_HIGH    = 1   /**< High priority thread */
} ThreadPriority;


/**
 * @struct ThreadAttributes
 * @brief Attributes for creating a thread.
 */
typedef struct {
	const char  *name;       /**< Optional thread name (max 15 chars) */
	uint64_t     affinity;   /**< CPU affinity mask (0 for any CPU) */
	int          priority;   /**< Thread priority (ThreadPriority) */
	unsigned int stack_size; /**< Thread stack size in bytes (0 for default) */
} ThreadAttributes;

/**
 * @typedef ThreadFunction
 * @brief Function type executed by a thread.
 * @param arg Pointer to arguments for the thread function (can be NULL).
 * @return Integer exit code for the thread.
 */
typedef int (*ThreadFunction)(void *arg);

/**
 * @typedef Thread
 * @brief Opaque handle representing a thread.
 * @note This is not the same as the platform's native thread handle.
 */
typedef uint64_t Thread;

/**
 * @brief Get the maximum number of concurrent threads supported.
 * @return Maximum concurrent threads.
 */
int thread_max_concurrent(void);

/**
 * @brief Return a human-readable string for a thread state enum.
 * @param s Thread state.
 * @return Pointer to string describing the state.
 */
const char *thread_state(int s);

/**
 * @brief Return a human-readable string for a thread error enum.
 * @param e Thread error.
 * @return Pointer to string describing the error.
 */
const char *thread_error(int e);

/**
 * @brief Get the handle of the current thread.
 * @return Current thread handle.
 * @note This is not the same as the platform's native thread handle.
 */
Thread thread_self(void);

/**
 * @brief Get the name of the current thread.
 * @return Name of the current thread.
 */
const char *thread_name(void);

/**
 * @brief Create a new thread.
 * 
 * The thread executes the function `f` with argument `arg`.
 * The thread must be joined or detached to free resources.
 * 
 * @param t Output pointer to receive the thread handle. Should not be NULL.
 * @param f Thread function to execute. Should not be NULL.
 * @param arg Argument passed to the thread function. Can be NULL.
 * @return 0 on success, ThreadError on failure.
 */
int thread_create(Thread *t, ThreadFunction f, void *arg);

/**
 * @brief Create a new thread with custom attributes.
 * 
 * The thread executes the function `f` with argument `arg`.
 * The thread must be joined or detached to free resources.
 * 
 * @param t Output pointer to receive the thread handle. Should not be NULL.
 * @param f Thread function to execute. Should not be NULL.
 * @param arg Argument passed to the thread function. Can be NULL.
 * @param a Thread attributes. Can be NULL for defaults.
 * @return 0 on success, ThreadError on failure.
 */
int thread_create_attr(Thread *t, ThreadFunction f, void *arg, const ThreadAttributes *a);

/**
 * @brief Exit the current thread.
 * @param code Exit code of the thread.
 */
void thread_exit(int code);

/**
 * @brief Wait for a thread to finish and retrieve its exit code.
 * @param t Thread handle to join.
 * @param code Output pointer to store the exit code. Can be NULL.
 * @return 0 on success, or ThreadError on failure.
 */
int thread_join(Thread t, int *code);

/**
 * @brief Detach a thread.
 * 
 * Detached threads free their resources automatically when finished.
 * 
 * @param t Thread handle to detach.
 * @return 0 on success, or ThreadError on failure.
 */
int thread_detach(Thread t);

/**
 * @brief Compare two thread handles for equality.
 * 
 * @param t1 First thread handle.
 * @param t2 Second thread handle.
 * @return Non-zero if threads are equal, 0 otherwise.
 */
int thread_equal(Thread t1, Thread t2);

/**
 * @brief Yield the processor to allow other threads to run.
 * @return 0 on success, non-zero on failure.
 */
int thread_yield(void);



#ifdef __cplusplus
}
#endif



#endif