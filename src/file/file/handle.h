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

#ifndef DESCENT_SOURCE_FILE_FILE_HANDLE_H
#define DESCENT_SOURCE_FILE_FILE_HANDLE_H

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <descent/file.h>
#include <descent/rcode.h>

#include "../path.h"

// TODO: These should be utils
static inline void *void_pointer_add(void *p, size_t offset) {
	return (void *)((char *)p + offset);
}
static inline const void *void_pointer_add_const(const void *p, size_t offset) {
	return (const void *)((const char *)p + offset);
}

// TODO: For all functions, use errno/GetLastError to return translated error codes

// These functions are internal, and expect valid inputs
// File access controls must be implemented externally

static inline int file_mode_canonical(int mode) {
	if (mode & FILE_MODE_APPEND) {
		// Append implies (and overrides) write
		mode |= FILE_MODE_WRITE;
		// Append is mutually exclusive with (and overrides) read and truncate
		mode &= ~(FILE_MODE_TRUNCATE | FILE_MODE_READ);
	}
	else if ((mode & FILE_MODE_TRUNCATE) && !(mode & FILE_MODE_WRITE)) {
		// Truncate requires write
		mode &= ~FILE_MODE_TRUNCATE;
	}

	// Exclusive requires create
	if ((mode & FILE_MODE_EXCLUSIVE) && !(mode & FILE_MODE_CREATE)) {
		mode &= ~FILE_MODE_EXCLUSIVE;
	}

	return mode;
}

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

static inline int file_flags_from_mode_posix(int mode) {
	int flags = 0;

	// Default to read-only
	if ((mode & FILE_MODE_READ) && (mode & FILE_MODE_WRITE))
		flags |= O_RDWR;
	else if (mode & FILE_MODE_WRITE)
		flags |= O_WRONLY;
	else
		flags |= O_RDONLY;

	if (mode & FILE_MODE_APPEND)    flags |= O_APPEND;
	if (mode & FILE_MODE_TRUNCATE)  flags |= O_TRUNC;
	if (mode & FILE_MODE_CREATE)    flags |= O_CREAT;
	if (mode & FILE_MODE_EXCLUSIVE) flags |= O_EXCL;

	return flags;
}

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

static inline DWORD file_access_from_mode_windows(int mode) {
	DWORD access = 0;

	if (mode & FILE_MODE_READ)
		access |= GENERIC_READ;

	// Append and write are mutually exclusive
	if (mode & FILE_MODE_APPEND)
		access |= FILE_APPEND_DATA;
	else if (mode & FILE_MODE_WRITE)
		access |= GENERIC_WRITE;

	// Default to read-only
	if (!access) access = GENERIC_READ;

	return access;
}

static inline DWORD file_creation_from_mode_windows(int mode) {
	if (mode & FILE_MODE_CREATE) {
		if (mode & FILE_MODE_EXCLUSIVE)
			return CREATE_NEW;
		if (mode & FILE_MODE_TRUNCATE)
			return CREATE_ALWAYS;
		else
			return OPEN_ALWAYS;
	}

	if (mode & FILE_MODE_TRUNCATE) return TRUNCATE_EXISTING;
	
	return OPEN_EXISTING;
}

#endif

static inline int file_open_handle(uintptr_t *handle, Path *p, int mode) {
	mode = file_mode_canonical(mode);

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

	int flags = file_flags_from_mode_posix(mode);
	int h;

	if (mode & FILE_MODE_CREATE) h = open(p->string, flags, 0644);
	else h = open(p->string, flags);

	if (h < 0) {
		switch (errno) {
			case EEXIST:
				return FILE_ERROR_EXISTS;
			case ENOENT:
				if (mode & FILE_MODE_CREATE)
					return FILE_ERROR_NO_PARENT;
				return FILE_ERROR_NO_OBJECT;
			case EISDIR:
				return FILE_ERROR_NOT_FILE;
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
	}

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

	DWORD access = file_access_from_mode_windows(mode);
	DWORD share = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
	DWORD creation = file_creation_from_mode_windows(mode);

	HANDLE h = CreateFileW(p->string, access, share, NULL, creation, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h == INVALID_HANDLE_VALUE) {
				switch (GetLastError()) {
			case ERROR_FILE_EXISTS:
			case ERROR_ALREADY_EXISTS:
				return FILE_ERROR_EXISTS;
			case ERROR_FILE_NOT_FOUND:
				return FILE_ERROR_NO_OBJECT;
			case ERROR_PATH_NOT_FOUND:
				return FILE_ERROR_NO_PARENT;
			case /* EISDIR */:
				return FILE_ERROR_NOT_FILE;
			case ERROR_INVALID_NAME:
			case ERROR_INVALID_DRIVE:
			case ERROR_FILENAME_EXCED_RANGE:
				return FILE_ERROR_INVALID_PATH;
			case ERROR_DIRECTORY:
			case ERROR_CANT_ACCESS_FILE:
			case ERROR_INVALID_REPARSE_DATA:
			case ERROR_BAD_NET_NAME:
				return FILE_ERROR_BAD_PATH;
			case ERROR_SHARING_VIOLATION:
			case ERROR_LOCK_VIOLATION:
				return FILE_ERROR_BUSY;
			case ERROR_ACCESS_DENIED:
			case ERROR_WRITE_PROTECT:
				return DESCENT_ERROR_FORBIDDEN;
			default:
				return DESCENT_ERROR_OS;
		}
	}

#endif

	*handle = (uintptr_t) h;
	
	return 0;
}

static inline int file_close_handle(uintptr_t handle) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	if (close((int) handle)) return DESCENT_ERROR_OS;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	if (!CloseHandle((HANDLE) handle)) return DESCENT_ERROR_OS;
#endif
	return 0;
}

static inline int file_write_handle(uintptr_t handle, size_t size, const void *source, size_t *n_written) {
	*n_written = 0;

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

	ssize_t written = write((int) handle, source, size);
	if (written < 0) return DESCENT_ERROR_OS;

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

	DWORD written = 0;
	BOOL result = WriteFile((HANDLE) handle, source, (DWORD) size, &written, NULL);
	if (!result) return DESCENT_ERROR_OS;

#endif

	*n_written = (size_t) written;

	return 0;
}

static inline int file_write_all_handle(uintptr_t handle, size_t size, const void *source, size_t *n_written) {
	*n_written = 0;

	while (size > 0) {
		size_t written = 0;
		int result = file_write_handle(handle, size, source, &written);
		if (result < 0) return result;

		source = void_pointer_add_const(source, written);
		size -= written;
		*n_written += written;
	}

	return 0;
}

static inline int file_read_handle(uintptr_t handle, size_t size, void *destination, size_t *n_read) {
	*n_read = 0;

#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	ssize_t nread = read((int) handle, destination, size);
	if (nread < 0) return DESCENT_ERROR_OS;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	DWORD nread = 0;
	if (!ReadFile((HANDLE) handle, destination, (DWORD) size, &nread, NULL)) {
		return DESCENT_ERROR_OS;
	}
#endif

	*n_read = (size_t) nread;

	return 0;
}

static inline int file_seek_handle(uintptr_t handle, size_t position) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	off_t offset = lseek((int) handle, (off_t) position, SEEK_SET);
	if (offset < 0) return DESCENT_ERROR_OS;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	LARGE_INTEGER li;
	li.QuadPart = (LONGLONG) position;
	if (!SetFilePointerEx((HANDLE) handle, li, NULL, FILE_BEGIN)) return DESCENT_ERROR_OS;
#endif
	return 0;
}

#endif