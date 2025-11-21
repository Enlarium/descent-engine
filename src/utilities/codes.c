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

#include <descent/utilities/codes.h>

const char *code_string(int code) {
	switch (code) {
		case DESCENT_WARNING_TRUNCATION:      return "DESCENT_WARNING_TRUNCATION";
		case DESCENT_WARNING_GENERIC:         return "DESCENT_WARNING_GENERIC";
		case DESCENT_SUCCESS:                 return "DESCENT_SUCCESS";
		case DESCENT_ERROR_GENERIC:           return "DESCENT_ERROR_GENERIC";
		case DESCENT_ERROR_UNKNOWN:           return "DESCENT_ERROR_UNKNOWN";
		case DESCENT_ERROR_FORBIDDEN:         return "DESCENT_ERROR_FORBIDDEN";
		case DESCENT_ERROR_UNSUPPORTED:       return "DESCENT_ERROR_UNSUPPORTED";
		case DESCENT_ERROR_NOT_IMPLEMENTED:   return "DESCENT_ERROR_NOT_IMPLEMENTED";
		case DESCENT_ERROR_INVALID_PARAMETER: return "DESCENT_ERROR_INVALID_PARAMETER";
		case DESCENT_ERROR_NULL_POINTER:      return "DESCENT_ERROR_NULL_POINTER";
		case DESCENT_ERROR_OUT_OF_MEMORY:     return "DESCENT_ERROR_OUT_OF_MEMORY";
		case DESCENT_ERROR_INVALID_STATE:     return "DESCENT_ERROR_INVALID_STATE";
		case DESCENT_ERROR_OVERFLOW:          return "DESCENT_ERROR_OVERFLOW";
		case DESCENT_ERROR_LONG_STRING:       return "DESCENT_ERROR_LONG_STRING";
		case DESCENT_ERROR_INVALID_MODULE:    return "DESCENT_ERROR_INVALID_MODULE";
		case CLI_ERROR_NO_HANDLER:            return "CLI_ERROR_NO_HANDLER";
		case CLI_ERROR_INVALID_PARAMETER:     return "CLI_ERROR_INVALID_PARAMETER";
		case CLI_ERROR_DUPLICATE_PARAMETER:   return "CLI_ERROR_DUPLICATE_PARAMETER";
		case CLI_ERROR_ARGUMENT_COUNT:        return "CLI_ERROR_ARGUMENT_COUNT";
		case CLI_ERROR_INCORRECT_ARGUMENT:    return "CLI_ERROR_INCORRECT_ARGUMENT";
		case LOG_ERROR_FORMAT_MESSAGE:        return "LOG_ERROR_FORMAT_MESSAGE";
		case LOG_ERROR_INVALID_HANDLE:        return "LOG_ERROR_INVALID_HANDLE";
		case LOG_ERROR_INVALID_FORMAT:        return "LOG_ERROR_INVALID_FORMAT";
		case LOG_ERROR_INVALID_LEVEL:         return "LOG_ERROR_INVALID_LEVEL";
		case LOG_ERROR_INVALID_PRESENT:       return "LOG_ERROR_INVALID_PRESENT";
		case LOG_ERROR_INVALID_PATH:          return "LOG_ERROR_INVALID_PATH";
		case THREAD_ERROR_NO_SLOTS:           return "THREAD_ERROR_NO_SLOTS";
		case THREAD_ERROR_HANDLE_INVALID:     return "THREAD_ERROR_HANDLE_INVALID";
		case THREAD_ERROR_HANDLE_UNMANAGED:   return "THREAD_ERROR_HANDLE_UNMANAGED";
		case THREAD_ERROR_HANDLE_DETACHED:    return "THREAD_ERROR_HANDLE_DETACHED";
		case THREAD_ERROR_HANDLE_CLOSED:      return "THREAD_ERROR_HANDLE_CLOSED";
		case THREAD_ERROR_FUNCTION_INVALID:   return "THREAD_ERROR_FUNCTION_INVALID";
		case THREAD_ERROR_OS_CREATE_FAILED:   return "THREAD_ERROR_OS_CREATE_FAILED";
		case THREAD_ERROR_OS_JOIN_FAILED:     return "THREAD_ERROR_OS_JOIN_FAILED";
		case THREAD_ERROR_OS_DETACH_FAILED:   return "THREAD_ERROR_OS_DETACH_FAILED";
		case THREAD_ERROR_OS_NAME_FAILED:     return "THREAD_ERROR_OS_NAME_FAILED";
		case THREAD_ERROR_OS_AFFINITY_FAILED: return "THREAD_ERROR_OS_AFFINITY_FAILED";
		case THREAD_ERROR_OS_PRIORITY_FAILED: return "THREAD_ERROR_OS_PRIORITY_FAILED";
		default:                              return "UNKNOWN_SIGNAL";
	}
}
