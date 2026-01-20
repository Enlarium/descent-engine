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

#ifndef DESCENT_SOURCE_FILE_DIR_H
#define DESCENT_SOURCE_FILE_DIR_H

#include <stddef.h>

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <errno.h>
#include <ftw.h>
#include <sys/stat.h>
#include <unistd.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <direct.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <descent/file.h>
#include <descent/rcode.h>

#include "fobj.h"
#include "path.h"

// TO GO ELSEWHERE
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

#if defined(DESCENT_PLATFORM_TYPE_POSIX)
static int dir_delete_posix(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
static int dir_remove_recursive_posix(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
typedef int (*DirWalkCallback)(Path *p, const WIN32_FIND_DATAW *data);
static int dir_walk_windows(Path *p, DirWalkCallback callback);
static int dir_delete_windows(Path *p, const WIN32_FIND_DATAW *data);
static int dir_remove_recursive_windows(Path *p, const WIN32_FIND_DATAW *data);
#endif

static inline int dir_create_pchars(const pchar *ps) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)

	if (!mkdir(ps, S_IRWXU)) return 0;

	switch (errno) {
		case EEXIST:
			return FILE_ERROR_EXISTS;
		case ENOENT:
			return FILE_ERROR_NO_PARENT;
		case EDQUOT:
		case ENOSPC:
		case EMLINK:
			return FILE_ERROR_NO_SPACE;
		case EINVAL:
		case ENAMETOOLONG:
			return FILE_ERROR_INVALID_PATH;
		case ENOTDIR:
		case ELOOP:
			return FILE_ERROR_BAD_PATH;
		case EROFS:
		case EACCES:
			return DESCENT_ERROR_FORBIDDEN;
		default:
			return DESCENT_ERROR_OS;
	}

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

	if (!_wmkdir(ps)) return 0;

	switch (GetLastError()) {
		case ERROR_ALREADY_EXISTS:
			return FILE_ERROR_EXISTS;
		case ERROR_PATH_NOT_FOUND:
			return FILE_ERROR_NO_PARENT;
		case ERROR_DISK_FULL:
		case ERROR_DISK_TOO_FRAGMENTED:
		case ERROR_HANDLE_DISK_FULL:
			return FILE_ERROR_NO_SPACE;
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

static inline int dir_remove_pchars(const pchar *ps) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)

	if (!rmdir(ps)) return 0;
	switch (errno) {
		case EEXIST: // Under POSIX.1, this is allowed instead of ENOTEMPTY
		case ENOTEMPTY:
			return FILE_ERROR_NOT_EMPTY;
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

	if (RemoveDirectoryW(ps)) return 0;

	switch (GetLastError()) {
		case ERROR_DIR_NOT_EMPTY:
			return FILE_ERROR_NOT_EMPTY;
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

static inline int dir_create_path(const Path *path) {
	if (!path) return DESCENT_ERROR_NULL;
	if (path->length == 0 || !path->string[0]) return FILE_ERROR_INVALID_PATH;

	return dir_create_pchars(path->string);
}

static inline int dir_ensure_path(const Path *path) {
	if (!path) return DESCENT_ERROR_NULL;
	if (path->length == 0 || !path->string[0]) return FILE_ERROR_INVALID_PATH;

	int result = dir_create_path(path);
	if (result) {
		if (result != FILE_ERROR_EXISTS) return result;

		FobjInfo info = {0};

		int stat_result = fobj_stat_path(&info, path);
		if (stat_result) return stat_result;

		if (info.type != FOBJ_DIR) return result;
	}

	return 0;
}

static inline int dir_create_recursive_path(Path *path) {
	if (!path) return DESCENT_ERROR_NULL;
	if (path->length == 0 || !path->string[0]) return FILE_ERROR_INVALID_PATH;

	int result = 0;

	// Temporarily remove trailing separator
	int trailing_separator = path->string[path->length - 1] == PATH_SEPARATOR;
	if (trailing_separator) path->string[--path->length] = PATH_TERMINATOR;

	// Don't need to create a directory for root
	if (path->length == 0) {
		if (trailing_separator) path->string[++path->length] = PATH_SEPARATOR;
		return 0;
	}

	// Check for drive roots on Windows
#if defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	else if (path->length == 2 && path->string[1] == ':') {
		if (trailing_separator) path->string[++path->length] = PATH_SEPARATOR;
		return 0;
	}
#endif

	// Start with terminator at end of root, if root exists
	size_t start = !path->subpath ? 0 : path->subpath - 1;

	// Create each parent segment after the root
	for (pchar *p = path->string + start; *p; p++) {
		if (*p == PATH_SEPARATOR) {
			// Skip successive separators
			// Reading forward is safe (null terminator), backwards is not
			if (*(p+1) == PATH_SEPARATOR) continue;

			*p = PATH_TERMINATOR;
			if ((result = dir_ensure_path(path))) {
				if (trailing_separator) path->string[++path->length] = PATH_SEPARATOR;
				return result;
			}
			*p = PATH_SEPARATOR;
		}
	}

	// Create final directory
	if (trailing_separator) path->string[++path->length] = PATH_SEPARATOR;
	return dir_ensure_path(path);
}

static inline int dir_remove_path(const Path *path) {
	if (!path) return DESCENT_ERROR_NULL;
	if (path->length == 0 || !path->string[0]) return FILE_ERROR_INVALID_PATH;

	return dir_remove_pchars(path->string);
}

// Delete everything
static inline int dir_delete_path(const Path *path) {
	if (!path) return DESCENT_ERROR_NULL;
	if (path->length == 0 || !path->string[0]) return FILE_ERROR_INVALID_PATH;

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

	int result = nftw(path->string, dir_delete_posix, 64, FTW_DEPTH | FTW_PHYS);
	// dir_remove_recursive_posix doesn't set warnings (negative codes)
	// Therefore negative returns are from nftw itself
	if (result < 0) return DESCENT_ERROR_OS;
	return result;

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

#endif

}

// Do not delete files or folders containing files
static inline int dir_remove_recursive_path(const Path *path) {
	if (!path) return DESCENT_ERROR_NULL;
	if (path->length == 0 || !path->string[0]) return FILE_ERROR_INVALID_PATH;

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

	int result = nftw(path->string, dir_remove_recursive_posix, 64, FTW_DEPTH | FTW_PHYS);
	// dir_remove_recursive_posix doesn't set warnings (negative codes)
	// Therefore negative returns are from nftw itself
	if (result < 0) return DESCENT_ERROR_OS;
	return result;

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

#endif
}

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

static int dir_delete_posix(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
	(void) ftwbuf;
	(void) sb;

	switch(typeflag) {
		case FTW_F:
		case FTW_SL:
		case FTW_SLN:
			return file_delete_pchars(path);
		case FTW_D:
			return dir_remove_pchars(path);
		default:
			return DESCENT_ERROR_OS;
	}
}

static int dir_remove_recursive_posix(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
	(void) ftwbuf;
	(void) sb;

	switch(typeflag) {
		case FTW_F:
		case FTW_SL:
		case FTW_SLN:
			return 0;
		case FTW_D:
		case FTW_DP: {
			int result = dir_remove_pchars(path);
			if (result == FILE_ERROR_NOT_EMPTY) return 0;
			return result;
		}
		default:
			return DESCENT_ERROR_OS;
	}
	return 0;
}

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

static int dir_walk_windows(Path *p, DirWalkCallback callback) {
	WIN32_FIND_DATAW data;

	// Path objects cost 4 KB each on Windows, so reuse is mandatory to avoid blowing out the stack

	// Ensure that the input path ends with a separator
	int result = path_ensure_separator(p);
	if (result) return result;

	// Store length for validation
	size_t length = p->length;

	// Reuse p for search
	int result = path_join_pchars(p, PCHARS("*"));
	if (result) return result;

	HANDLE hFind = FindFirstFileW(p, &data);
	if (hFind == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND) return FILE_ERROR_NO_OBJECT;
		return DESCENT_ERROR_OS;
	}

	do {
		const wchar_t *name = data.cFileName;

		// Skip "." and ".."
		if (!wcscmp(name, PCHARS(".")) || !wcscmp(name, PCHARS(".."))) continue;

		// Reset p for traversal
		// Undoes "\*" (search) or "\name" (last iteration)
		result = path_dirname(p, p);
		if (result) {
			FindClose(hFind);
			return result;
		}

		// Build full path
		result = path_join_pchars(p, name, 0);
		if (result) {
			FindClose(hFind);
			return result;
		}

		// Do not follow reparse points
		if (
			(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
			!(data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
		) {
			result = dir_walk_windows(p, callback);
			if (result) {
				FindClose(hFind);
				return result;
			}
		}

		// Call callback for this entry
		result = callback(p, &data);
		if (result) {
			FindClose(hFind);
			return result;
		}

	} while (FindNextFileW(hFind, &data));

	FindClose(hFind);

	// Reset p to original state
	// Undoes "\name" (last iteration) or "\*" (search, no iterations)
	result = path_dirname(p, p);
	if (result) return result;

	if (length != p->length) return DESCENT_ERROR_STATE; // Should never happen

	return 0;
}

static int dir_delete_windows(Path *p, const WIN32_FIND_DATAW *data) {
	if (
		(data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
		!(data->dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
	) return dir_remove_path(p);
	else return file_delete(p);
}

static int dir_remove_recursive_windows(Path *p, const WIN32_FIND_DATAW *data) {
	if (
		(data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
		!(data->dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
	) {
			int result = dir_remove_path(p);
			if (result == FILE_ERROR_NOT_EMPTY) return 0;
			return result;
	}
	return 0;
}

#endif

#endif