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

#ifndef DESCENT_SOURCE_FILE_FILE_H
#define DESCENT_SOURCE_FILE_FILE_H

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <errno.h>
#include <unistd.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <descent/rcode.h>

#include "path.h"

static inline int file_delete_pchars(const pchar *ps) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)

	if (!unlink(ps)) return 0;

	switch (errno) {
		case ENOENT:
			return FILE_ERROR_NO_OBJECT;
		case EINVAL:
		case ENAMETOOLONG:
			return FILE_ERROR_INVALID_PATH;
		case ENOTDIR:
		case ELOOP:
			return FILE_ERROR_BAD_PATH;
		case EBUSY:
			return FILE_ERROR_BUSY;
		case EROFS:
		case EPERM:
		case EACCES:
			return DESCENT_ERROR_FORBIDDEN;
		default:
			return DESCENT_ERROR_OS;
	}

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

	if (DeleteFileW(ps)) return 0;

	switch (GetLastError()) {
		case ERROR_FILE_NOT_FOUND:
			return FILE_ERROR_NO_OBJECT;
		case ERROR_SHARING_VIOLATION:
		case ERROR_LOCK_VIOLATION:
			return FILE_ERROR_BUSY;
		case ERROR_INVALID_NAME:
		case ERROR_BAD_PATHNAME:
			return FILE_ERROR_INVALID_PATH;
		case ERROR_DIRECTORY:
			return FILE_ERROR_BAD_PATH;
		case ERROR_ACCESS_DENIED:
		case ERROR_WRITE_PROTECT:
			return DESCENT_ERROR_FORBIDDEN;
		default:
			return DESCENT_ERROR_OS;
	}

#endif
}

#endif