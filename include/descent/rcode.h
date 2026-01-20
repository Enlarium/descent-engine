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

#ifndef DESCENT_RCODE_H
#define DESCENT_RCODE_H

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

#include <descent/utilities/platform.h>
#include <descent/modules.h>

/*
Codes less than or equal to 0 have special meanings reserved by the library.
Positive values are user-defined and unregulated, but advised to follow this scheme.
If user-defined values follow this scheme, they can use functions defined here for their own
returns, otherwise they cannot.

Severity:
- 0: Operation fully succeeded; nothing special to report.
- INFO: Operation succeeded, but carries a state the caller may need to inspect.
	- *Only use for functions where the caller is expected to handle nuanced state.*
	- Do not use INFO for trivial informational messages.
- WARN: Operation partially succeeded, and the caller should check how it impacts them.
	- *Only use for functions where the function has different levels of success that the
		caller is expected to accept and handle*
	- Do not use INFO for irrelevant state warnings or states that cannot be reasonably
		continued from.
- ERROR: Operation failed, and the caller needs to check what went wrong.
- FATAL: Operation failed, and the caller needs to clean up and shut down.

INFO and WARN cannot be generic or unknown, they MUST contain discerning information about the state
resulting from the output.

Fatal means "somebody messed the system up badly enough we cannot continue"

Most codes are going to be errors
*/

/** @cond INTERNAL */
#define RCODE_WIDTH 32u

#define RC_USER    0u
#define RC_DESCENT 1u

#define RC_INFO  0u
#define RC_WARN  1u
#define RC_ERROR 2u
#define RC_FATAL 3u

#define RC_USER_MIN     0x00000001
#define RC_USER_MAX     0x7FFFFFFF
#define RC_DESCENT_MIN -0x80000000
#define RC_DESCENT_MAX -0x00000001

#define RCODE_ORIGIN_WIDTH   1u
#define RCODE_SEVERITY_WIDTH 2u
#define RCODE_MODULE_WIDTH   13u
#define RCODE_CODE_WIDTH     16u

#define RCODE_ORIGIN_MASK    ((1u << RCODE_ORIGIN_WIDTH)   - 1)
#define RCODE_SEVERITY_MASK  ((1u << RCODE_SEVERITY_WIDTH) - 1)
#define RCODE_MODULE_MASK    ((1u << RCODE_MODULE_WIDTH)   - 1)
#define RCODE_CODE_MASK      ((1u << RCODE_CODE_WIDTH)     - 1)

#define RCODE_ORIGIN_SHIFT   (RCODE_WIDTH          - RCODE_ORIGIN_WIDTH)
#define RCODE_SEVERITY_SHIFT (RCODE_ORIGIN_SHIFT   - RCODE_SEVERITY_WIDTH)
#define RCODE_MODULE_SHIFT   (RCODE_SEVERITY_SHIFT - RCODE_MODULE_WIDTH)
#define RCODE_CODE_SHIFT     (RCODE_MODULE_SHIFT   - RCODE_CODE_WIDTH)
/** @endcond */

#define RCODE_ORIGIN(rc)   ((((uint32_t)rc) >> RCODE_ORIGIN_SHIFT)   & RCODE_ORIGIN_MASK)
#define RCODE_SEVERITY(rc) ((((uint32_t)rc) >> RCODE_SEVERITY_SHIFT) & RCODE_SEVERITY_MASK)
#define RCODE_MODULE(rc)   ((((uint32_t)rc) >> RCODE_MODULE_SHIFT)   & RCODE_MODULE_MASK)
#define RCODE_CODE(rc)     ((((uint32_t)rc) >> RCODE_CODE_SHIFT)     & RCODE_CODE_MASK)

#define RCODE(origin, severity, module, code) ((int32_t)( \
	((origin   & RCODE_ORIGIN_MASK)   << RCODE_ORIGIN_SHIFT) | \
	((severity & RCODE_SEVERITY_MASK) << RCODE_SEVERITY_SHIFT) | \
	((module   & RCODE_MODULE_MASK)   << RCODE_MODULE_SHIFT) | \
	((code     & RCODE_CODE_MASK)     << RCODE_CODE_SHIFT) \
))

/** @cond INTERNAL */
#define RCODE_INFO(module, code)  RCODE(RC_DESCENT, RC_INFO,  module, code)
#define RCODE_WARN(module, code)  RCODE(RC_DESCENT, RC_WARN,  module, code)
#define RCODE_ERROR(module, code) RCODE(RC_DESCENT, RC_ERROR, module, code)
#define RCODE_FATAL(module, code) RCODE(RC_DESCENT, RC_FATAL, module, code)
/** @endcond */

#define RCODES \
	X(DESCENT_SUCCESS,               0, "Operation succeeded") \
	\
	X(DESCENT_INFO_BUSY,             RCODE_INFO(MODULE_CORE, 0x00u), "Device or resource busy") \
	X(DESCENT_INFO_TRUNCATION,       RCODE_INFO(MODULE_CORE, 0x01u), "Information was truncated") \
	\
	X(DESCENT_WARN_BUSY,             RCODE_WARN(MODULE_CORE, 0x00u), "Device or resource busy") \
	X(DESCENT_WARN_TRUNCATION,       RCODE_WARN(MODULE_CORE, 0x01u), "Information was truncated") \
	X(DESCENT_WARN_INCOMPLETE,       RCODE_WARN(MODULE_CORE, 0x02u), "Operation was incomplete") \
	\
	X(DESCENT_ERROR_GENERIC,         RCODE_ERROR(MODULE_CORE, 0x00u), "Generic error") \
	X(DESCENT_ERROR_UNKNOWN,         RCODE_ERROR(MODULE_CORE, 0x01u), "Unknown error") \
	X(DESCENT_ERROR_FORBIDDEN,       RCODE_ERROR(MODULE_CORE, 0x02u), "Operation is forbidden") \
	X(DESCENT_ERROR_UNSUPPORTED,     RCODE_ERROR(MODULE_CORE, 0x03u), "Operation is unsupported") \
	X(DESCENT_ERROR_UNIMPLEMENTED,   RCODE_ERROR(MODULE_CORE, 0x04u), "Operation is not implemented") \
	X(DESCENT_ERROR_NULL,            RCODE_ERROR(MODULE_CORE, 0x05u), "Operation received null pointer where a non-null pointer is required") \
	X(DESCENT_ERROR_INVALID,         RCODE_ERROR(MODULE_CORE, 0x06u), "Operation received invalid parameter(s)") \
	X(DESCENT_ERROR_INIT,            RCODE_ERROR(MODULE_CORE, 0x07u), "Operation is forbidden on initialized object") \
	X(DESCENT_ERROR_BUSY,            RCODE_ERROR(MODULE_CORE, 0x08u), "Operation is forbidden on object in use") \
	X(DESCENT_ERROR_STATE,           RCODE_ERROR(MODULE_CORE, 0x09u), "Operation is not valid in current state") \
	X(DESCENT_ERROR_MEMORY,          RCODE_ERROR(MODULE_CORE, 0x0Au), "Failed to allocate memory") \
	X(DESCENT_ERROR_OVERFLOW,        RCODE_ERROR(MODULE_CORE, 0x0Bu), "Numeric or buffer overflow occurred") \
	X(DESCENT_ERROR_MODULE,          RCODE_ERROR(MODULE_CORE, 0x0Cu), "Invalid engine module specified") \
	X(DESCENT_ERROR_OS,              RCODE_ERROR(MODULE_CORE, 0x0Du), "Uncontrollable failure at the OS level") \
	\
	X(DESCENT_FATAL_GENERIC,         RCODE_FATAL(MODULE_CORE, 0x00u), "Generic fatal event") \
	X(DESCENT_FATAL_UNKNOWN,         RCODE_FATAL(MODULE_CORE, 0x01u), "Unknown fatal event") \
	X(DESCENT_FATAL_TIMER,           RCODE_FATAL(MODULE_CORE, 0x02u), "No sufficient timer exists") \
	\
	\
	X(ALLOCATOR_ERROR_ALLOC,         RCODE_ERROR(MODULE_ALLOCATOR, 0x00u), "Tried to allocate memory with invalid parameters") \
	X(ALLOCATOR_ERROR_FREE,          RCODE_ERROR(MODULE_ALLOCATOR, 0x01u), "Tried to free an invalid allocation") \
	\
	\
	X(THREAD_INFO_BUSY,              RCODE_INFO(MODULE_THREADING, 0x00u), "Synchronization unavailable without contention") \
	X(THREAD_INFO_TIMEOUT,           RCODE_INFO(MODULE_THREADING, 0x01u), "Synchronization wait timed out") \
	\
	X(THREAD_ERROR_INVALID,          RCODE_ERROR(MODULE_THREADING, 0x00u), "Specified thread is invalid") \
	X(THREAD_ERROR_ACTIVE,           RCODE_ERROR(MODULE_THREADING, 0x01u), "Operation cannot be performed on an active thread") \
	X(THREAD_ERROR_INACTIVE,         RCODE_ERROR(MODULE_THREADING, 0x02u), "Operation cannot be performed on an inactive thread") \
	X(THREAD_ERROR_DEADLOCK,         RCODE_ERROR(MODULE_THREADING, 0x03u), "Operation results in a deadlock") \
	\
	\
	X(CLI_ERROR_NO_HANDLER,          RCODE_ERROR(MODULE_CLI, 0x00u), "No handler for CLI parameter") \
	X(CLI_ERROR_INVALID_PARAMETER,   RCODE_ERROR(MODULE_CLI, 0x01u), "Invalid CLI parameter") \
	X(CLI_ERROR_DUPLICATE_PARAMETER, RCODE_ERROR(MODULE_CLI, 0x02u), "Duplicate CLI parameter") \
	X(CLI_ERROR_ARGUMENT_COUNT,      RCODE_ERROR(MODULE_CLI, 0x03u), "Incorrect number of CLI arguments") \
	X(CLI_ERROR_INCORRECT_ARGUMENT,  RCODE_ERROR(MODULE_CLI, 0x04u), "Incorrect CLI argument") \
	\
	\
	X(FILE_ERROR_INVALID_PATH,       RCODE_ERROR(MODULE_FILESYSTEM, 0x00u), "Invalid file path") \
	X(FILE_ERROR_EXISTS,             RCODE_ERROR(MODULE_FILESYSTEM, 0x01u), "File already exists") \
	X(FILE_ERROR_NO_OBJECT,          RCODE_ERROR(MODULE_FILESYSTEM, 0x02u), "No such filesystem object") \
	X(FILE_ERROR_NO_PARENT,          RCODE_ERROR(MODULE_FILESYSTEM, 0x03u), "Parent directory missing") \
	X(FILE_ERROR_NO_SPACE,           RCODE_ERROR(MODULE_FILESYSTEM, 0x04u), "Not enough disk space") \
	X(FILE_ERROR_NOT_EMPTY,          RCODE_ERROR(MODULE_FILESYSTEM, 0x05u), "Directory not empty") \
	X(FILE_ERROR_NOT_FILE,           RCODE_ERROR(MODULE_FILESYSTEM, 0x06u), "Filesystem object is not a file") \
	X(FILE_ERROR_NOT_DIR,            RCODE_ERROR(MODULE_FILESYSTEM, 0x07u), "Filesystem object is not a directory") \
	X(FILE_ERROR_BAD_PATH,           RCODE_ERROR(MODULE_FILESYSTEM, 0x08u), "File path cannot be used for this operation") \
	X(FILE_ERROR_BUSY,               RCODE_ERROR(MODULE_FILESYSTEM, 0x09u), "Filesystem object is busy") \
	\
	\
	X(LOG_ERROR_FORMAT_MESSAGE,      RCODE_ERROR(MODULE_LOGGING, 0x00u), "Error formatting log message") \
	X(LOG_ERROR_INVALID_HANDLE,      RCODE_ERROR(MODULE_LOGGING, 0x01u), "Invalid log handle") \
	X(LOG_ERROR_INVALID_FORMAT,      RCODE_ERROR(MODULE_LOGGING, 0x02u), "Invalid log format") \
	X(LOG_ERROR_INVALID_LEVEL,       RCODE_ERROR(MODULE_LOGGING, 0x03u), "Invalid log level") \
	X(LOG_ERROR_INVALID_PRESENT,     RCODE_ERROR(MODULE_LOGGING, 0x04u), "Invalid logging presentation mode") \
	X(LOG_ERROR_INVALID_PATH,        RCODE_ERROR(MODULE_LOGGING, 0x05u), "Invalid log file path")

typedef int32_t rcode;

enum {
#define X(name, value, desc) name = value, /**< desc */
	RCODES
#undef X
};

_Static_assert(RCODE_WIDTH                      == 32u,         "rcode layout error: width must be 32");
_Static_assert(sizeof(int) * CHAR_BIT           >= RCODE_WIDTH, "rcode requires int to be at least 32 bits");
_Static_assert(RCODE_CODE_SHIFT                 == 0u,          "rcode layout error: code shift must be zero");
_Static_assert(RC_USER_MIN                      == 1u,          "rcode layout error: user min incorrect");
_Static_assert(RCODE_ORIGIN(RC_USER_MIN)        == RC_USER,     "rcode layout error: user min incorrect");
_Static_assert(RCODE_ORIGIN(RC_USER_MAX)        == RC_USER,     "rcode layout error: user max incorrect");
_Static_assert(RCODE_ORIGIN(RC_USER_MAX + 1)    == RC_DESCENT,  "rcode layout error: user max is not max");
_Static_assert(RCODE_ORIGIN(RC_DESCENT_MIN - 1) == RC_USER,     "rcode layout error: descent min is not min");
_Static_assert(RCODE_ORIGIN(RC_DESCENT_MIN)     == RC_DESCENT,  "rcode layout error: descent min incorrect");
_Static_assert(RCODE_ORIGIN(RC_DESCENT_MAX)     == RC_DESCENT,  "rcode layout error: descent max incorrect");
_Static_assert(RC_DESCENT_MAX                   == -0x00000001, "rcode layout error: descent max incorrect");
_Static_assert(DESCENT_SUCCESS                  == 0,           "DESCENT_SUCCESS must equal 0");

/**
 * @brief Checks if a code is reserved for user definition.
 * @param rc The code to check.
 * @return True if the code is user-side, false otherwise.
 */
static inline bool rcode_is_user(rcode rc) {
	return (rc != DESCENT_SUCCESS) && (RCODE_ORIGIN(rc) == RC_USER);
}

/**
 * @brief Checks if a code is reserved for library definition.
 * @param rc The code to check.
 * @return True if the code is library-side, false otherwise.
 */
static inline bool rcode_is_descent(rcode rc) {
	return rc == DESCENT_SUCCESS || RCODE_ORIGIN(rc) == RC_DESCENT;
}

/**
 * @brief Checks if a code is informational.
 * @param rc The code to check.
 * @return True if the code is informational, false otherwise.
 */
static inline bool rcode_is_info(rcode rc) {
	return rc != DESCENT_SUCCESS && RCODE_SEVERITY(rc) == RC_INFO;
}

/**
 * @brief Checks if a code is a warning.
 * @param rc The code to check.
 * @return True if the code is an warning, false otherwise.
 */
static inline bool rcode_is_warning(rcode rc) {
	return RCODE_SEVERITY(rc) == RC_WARN;
}

/**
 * @brief Checks if a code is an error.
 * @param rc The code to check.
 * @return True if the code is an error, false otherwise.
 */
static inline bool rcode_is_error(rcode rc) {
	return RCODE_SEVERITY(rc) == RC_ERROR;
}

/**
 * @brief Checks if a code is fatal.
 * @param rc The code to check.
 * @return True if the code is fatal, false otherwise.
 */
static inline bool rcode_is_fatal(rcode rc) {
	return RCODE_SEVERITY(rc) == RC_FATAL;
}

/**
 * @brief Checks if a code is successful (0 or info).
 * @param rc The code to check.
 * @return True if the code is successful, false otherwise.
 * @return The operation completed with expected state.
 */
static inline bool rcode_successful(rcode rc) {
	return RCODE_SEVERITY(rc) <= RC_INFO;
}

/**
 * @brief Checks if a code is acceptable (0, info, or warning).
 * @param rc The code to check.
 * @return True if the code is acceptable, false otherwise.
 * @note The operation completed with a state that can be accepted or rejected.
 */
static inline bool rcode_acceptable(rcode rc) {
	return RCODE_SEVERITY(rc) <= RC_WARN;
}

/**
 * @brief Checks if a code is recoverable (0, info, warning, or error).
 * @param rc The code to check.
 * @return True if the code is recoverable, false otherwise.
 * @note The operation failed with a state that can be recovered from.
 */
static inline bool rcode_recoverable(rcode rc) {
	return RCODE_SEVERITY(rc) <= RC_ERROR;
}

/**
 * @brief Converts a code to a human-readable string.
 * @param rc The code.
 * @return Pointer to a constant string naming the code.
 */
const char *rcode_string(rcode rc);

/**
 * @brief Provides a message for a code.
 * @param rc The code.
 * @return Pointer to a constant string describing the code.
 */
const char *rcode_message(rcode rc);

// Clean up internal definitions

#ifndef DESCENT_RCODE_IMPLEMENTATION
#undef RCODES
#endif

// Redefine RCODE_* helpers for users, to encourage them to use our system

#undef RCODE_INFO
#undef RCODE_WARN
#undef RCODE_ERROR
#undef RCODE_FATAL

#define RCODE_INFO(module, code)  RCODE(RC_USER, RC_INFO,  module, code)
#define RCODE_WARN(module, code)  RCODE(RC_USER, RC_WARN,  module, code)
#define RCODE_ERROR(module, code) RCODE(RC_USER, RC_ERROR, module, code)
#define RCODE_FATAL(module, code) RCODE(RC_USER, RC_FATAL, module, code)

#endif
