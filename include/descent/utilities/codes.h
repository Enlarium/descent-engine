/* Copyright 2025 XavierHarkonnen9 and Enlarium
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DESCENT_UTILITIES_CODES_H
#define DESCENT_UTILITIES_CODES_H

/**
 * @enum DescentCode
 * @brief Defines codes returned by Descent Engine functions.
 */
typedef enum {
	// General codes
	DESCENT_WARNING_TRUNCATION = -2, /**< Information was truncated */
	DESCENT_WARNING_GENERIC    = -1, /**< Generic qualified success */
	DESCENT_SUCCESS            = 0,  /**< Operation succeeded */
	DESCENT_ERROR_GENERIC      = 1,  /**< Generic error occurred */
	DESCENT_ERROR_UNKNOWN,           /**< Unknown error occurred */
	DESCENT_ERROR_FORBIDDEN,         /**< Operation forbidden */
	DESCENT_ERROR_UNSUPPORTED,       /**< Operation is unsupported */
	DESCENT_ERROR_NOT_IMPLEMENTED,   /**< Functionality not implemented */
	DESCENT_ERROR_INVALID_PARAMETER, /**< Function received invalid parameter(s) */
	DESCENT_ERROR_NULL_POINTER,      /**< Null pointer passed where non-null required */
	DESCENT_ERROR_OUT_OF_MEMORY,     /**< Failed to allocate memory */
	DESCENT_ERROR_INVALID_STATE,     /**< Operation not allowed in current state */
	DESCENT_ERROR_OVERFLOW,          /**< Numeric or buffer overflow occurred */
	DESCENT_ERROR_LONG_STRING,       /**< Dynamic object length is too large */
	DESCENT_ERROR_INVALID_MODULE,    /**< Invalid engine module specified */

	// Command Line Interface
	CLI_ERROR_NO_HANDLER,
	CLI_ERROR_INVALID_PARAMETER,
	CLI_ERROR_DUPLICATE_PARAMETER,
	CLI_ERROR_ARGUMENT_COUNT,
	CLI_ERROR_INCORRECT_ARGUMENT,

	// Logging codes
	LOG_ERROR_FORMAT_MESSAGE,  /**< Error formatting log message */
	LOG_ERROR_INVALID_HANDLE,  /**< Invalid log handle */
	LOG_ERROR_INVALID_FORMAT,  /**< Invalid log format */
	LOG_ERROR_INVALID_LEVEL,   /**< Invalid log level */
	LOG_ERROR_INVALID_PRESENT, /**< Invalid logging presentation mode */
	LOG_ERROR_INVALID_PATH,    /**< File path is invalid */ // Should go in filesystem?

	// Threading codes
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
} DescentCode;

/**
 * @brief Checks if a code is a warning.
 * @param code The code to check.
 * @return Non-zero if code is a warning, zero otherwise.
 */
static inline int is_warning(int code) {
	return code < DESCENT_SUCCESS;
}

/**
 * @brief Checks if a code is an error.
 * @param code The code to check.
 * @return Non-zero if code is an error, zero otherwise.
 */
static inline int is_error(int code) {
	return code > DESCENT_SUCCESS;
}

/**
 * @brief Converts a code to a human-readable string.
 * @param code The code code.
 * @return Pointer to a constant string describing the code.
 */
const char *code_string(int code);

#endif