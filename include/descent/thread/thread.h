#ifndef DESCENT_THREAD_THREAD_H
#define DESCENT_THREAD_THREAD_H

#include <stddef.h>
#include <stdint.h>

#define THREAD_NAME_LENGTH 16

#define THREAD_AFFINITY_ALL UINT64_MAX

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
	THREAD_ERROR_UNKNOWN     = -1,   /**< Unknown error occurred */
	THREAD_SUCCESS           = 0,    /**< Operation succeeded */
	THREAD_ERROR_NO_SLOTS,           /**< No thread slots available */
	THREAD_ERROR_HANDLE_INVALID,     /**< Thread handle is invalid */
	THREAD_ERROR_HANDLE_UNMANAGED,   /**< Operation not allowed on unmanaged thread */
	THREAD_ERROR_HANDLE_DETACHED,    /**< Operation not allowed on detached thread */
	THREAD_ERROR_HANDLE_CLOSED,      /**< Operation on a closed thread */
	THREAD_ERROR_FUNCTION_INVALID,   /**< Thread function pointer is invalid */
	THREAD_ERROR_OS_CREATE_FAILED,   /**< Creating the thread failed at the OS level */
	THREAD_ERROR_OS_JOIN_FAILED,     /**< Joining the thread failed at the OS level */
	THREAD_ERROR_OS_DETACH_FAILED,   /**< Detaching the thread failed at the OS level */
	THREAD_ERROR_OS_NAME_FAILED,     /**< Setting the thread's name failed at the OS level */
	THREAD_ERROR_OS_AFFINITY_FAILED, /**< Setting the thread's affinity failed at the OS level */
	THREAD_ERROR_OS_PRIORITY_FAILED, /**< Setting the thread's priority failed at the OS level */
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
 * @struct ThreadName
 * @brief Container for thread name. Limited to 15 characters followed by a null terminator.
 */
typedef struct {
	char name[THREAD_NAME_LENGTH];
} ThreadName;

/**
 * @struct ThreadAttributes
 * @brief Attributes for creating a thread.
 */
typedef struct {
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
 * 
 * @note This handle is not the same as the platform's native thread handle.
 * 
 * @note Threads created through the Descent thread system must not be manipulated
 * using the underlying thread API or other thread libraries. Doing so results
 * in undefined behavior.
 */
typedef uint64_t Thread;

/**
 * @brief Get the maximum number of concurrent threads supported.
 * @return Maximum concurrent threads.
 */
int thread_max_concurrent(void);

/**
 * @brief Return a human-readable string for a thread error enum.
 * @param e Thread error.
 * @return Pointer to string describing the error.
 */
const char *thread_error_message(int e);

/**
 * @brief Create a new thread.
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
int thread_create(Thread *t, ThreadFunction f, void *arg, const ThreadAttributes *a);

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
 * @brief Exit the current thread.
 * @param code Exit code of the thread.
 */
void thread_exit(int code);

/**
 * @brief Get the handle of the current thread.
 * @return Current thread handle.
 * @note This is not the same as the platform's native thread handle.
 */
Thread thread_self(void);

/**
 * @brief Compare two thread handles for equality.
 * 
 * @param t1 First thread handle.
 * @param t2 Second thread handle.
 * @return Non-zero if threads are equal, 0 otherwise.
 */
int thread_equal(Thread t1, Thread t2);

/**
 * @brief Get the name of the current thread.
 * @return Name of the current thread.
 * 
 * @note If this function is called on an unmanaged thread, the returned name will be a zero-length string.
 */
ThreadName thread_get_name(void);

/**
 * @brief Set the name of the current thread.
 * @param n The name to set the thread to.
 * @return 0 on success, or ThreadError on failure.
 * 
 * @note This function will only succeed on managed threads.
 */
int thread_set_name(ThreadName n);

/**
 * @brief Get the affinity of the current thread.
 * @return Affinity of the current thread as a bitmask.
 * 
 * @note If this function is called on an unmanaged thread, the returned affinity will be zero.
 * @note Not all platforms fully support thread affinity.
 */
uint64_t thread_get_affinity(void);

/**
 * @brief Set the affinity of the current thread.
 * @param affinity The affinity to set the thread to as a bitmask. If this parameter is zero, the thread affinity will be unchanged
 * @return 0 on success, or ThreadError on failure.
 * 
 * @note This function will only succeed on managed threads.
 * @note Not all platforms fully support thread affinity.
 */
int thread_set_affinity(uint64_t affinity);

/**
 * @brief Get the priority of the current thread.
 * @return The priority of the current thread 
 * 
 * @note If this function is called on an unmanaged thread, the returned priority will be DEFAULT.
 * @note Not all platforms fully support thread priority.
 */
ThreadPriority thread_get_priority(void);

/**
 * @brief Set the priority of the current thread.
 * 
 * @param p The priority to set the thread to.
 * @return 0 on success, or ThreadError on failure. 
 * 
 * @note This function will only succeed on managed threads.
 * @note Not all platforms fully support thread priority.
 */
int thread_set_priority(ThreadPriority p);

/**
 * @brief Yields to other threads ready to run on the current processor
 * @return 0 on success, nonzero on failure.
 * 
 * @note Provides no behavior guarantees on non-realtime threads.
 */
int thread_yield(void);

// Functions below are unorganized or unfinished

long thread_sleep_granularity(void);

//(multiple sleep modes?)
int thread_sleep(uint64_t nanoseconds);

// doesn't usually work / provide guarantees for non-realtime threads

/*
dynamic TLS
Per-thread CPU usage sampling

try join? timed join?

Other files:
timed waits for locks
change spinlock implementation to native
*/

#ifdef __cplusplus
}
#endif

#endif