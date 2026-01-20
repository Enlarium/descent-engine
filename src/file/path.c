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

#include <descent/file.h>
#include "path.h"

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wchar.h>
#elif defined(DESCENT_PLATFORM_TYPE_POSIX)
#if defined(DESCENT_PLATFORM_FREEBSD)
#include <sys/sysctl.h>
#endif
#include <string.h>
#include <stdlib.h>
#endif

#include <descent/rcode.h>
#include <descent/thread/rwlock.h>

static Path program_root = {0};
static Path config_root = {0};
static Path cache_root = {0};
static Path temp_root = {0};

RWLock roots_lock = RWLOCK_INIT;

static inline int path_find_program_root(void) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)

	const char *raw = NULL;

#if defined(DESCENT_PLATFORM_LINUX)

	raw = "/proc/self/exe";

#elif defined(DESCENT_PLATFORM_FREEBSD)

	char raw_path[PATH_SIZE] = {0};
	size_t raw_length = sizeof(raw_path);

	int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
	if (sysctl(mib, 4, raw_path, &raw_length, NULL, 0)) {
		return path_invalidate(&program_root, DESCENT_ERROR_OS);
	}

	// Exclude extraneous null terminators
	while (raw_path[raw_length - 1] == PATH_TERMINATOR) --raw_length;

	if (raw_length >= PATH_SIZE) {
		return path_invalidate(&program_root, DESCENT_ERROR_OVERFLOW);
	}

	raw = raw_path;

#endif

	// Get real path
	if (!realpath(raw, program_root.string)) {
		return path_invalidate(&program_root, DESCENT_ERROR_OS);
	}

	program_root.length = pchars_length(program_root.string);

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

	Path raw_path = {0};

	raw_path.length = (size_t) GetModuleFileNameW(NULL, raw_path.string, PATH_SIZE);
	if (!raw_path.length) {
		return path_invalidate(&program_root, DESCENT_ERROR_OS);
	}

	if (raw_path.length >= PATH_SIZE) {
		return path_invalidate(&program_root, DESCENT_ERROR_OVERFLOW);
	}

	// Get real path
	program_root.length = (size_t) GetFullPathNameW(raw_path.string, PATH_SIZE, program_root.string, NULL);
	if (!program_root.length) {
		return path_invalidate(&program_root, DESCENT_ERROR_OS);
	}

	if (program_root.length >= PATH_SIZE) {
		return path_invalidate(&program_root, DESCENT_ERROR_OVERFLOW);
	}

	program_root.string[program_root.length] = PATH_TERMINATOR;
	
#endif

	// Get dirname in-place
	int result = path_dirname(&program_root, &program_root);
	if (result) return path_invalidate(&program_root, result);

	// Set start of subpath
	program_root.subpath = program_root.length;

	return 0;
}

static inline int path_find_root(
	Path *root,
	const char *env,
	const pchar *seg,
	const char *program_name
) {
	if (!program_name || !program_name[0]) return DESCENT_ERROR_INVALID;

	int result = 0;
	int env_bool = env && env[0];
	int seg_bool = seg && seg[0];

	// Require that either env or segment is set
	if (!(env_bool || seg_bool)) return DESCENT_ERROR_INVALID;

	// Try the environment
	if (env_bool) {
		const char* directory = getenv(env);
		if (!directory || !directory[0]) return path_invalidate(root, DESCENT_ERROR_OS);

		result = path_copy_chars(root, directory, 0);
		if (result) return path_invalidate(root, result);

	}

	// Try the segment
	if (seg_bool) {
		result = path_join_pchars(root, seg, 0);
		if (result) return path_invalidate(root, result);
	}

	// Append the program name	
	result = path_join_chars(root, program_name, 0);
	if (result) return path_invalidate(root, result);

	// Ensure the root ends with a separator
	result = path_ensure_separator(root);
	if (result) return path_invalidate(root, result);
	
	// Set start of subpath 
	root->subpath = root->length;

	return 0;
}

static inline int path_find_config_root(const char *program_name) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	if (path_find_root(&config_root, "XDG_CONFIG_HOME", NULL, program_name)) {
		return path_find_root(&config_root, "HOME", PCHARS(".config"), program_name);
	}
	return 0;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return path_find_root(&config_root, "APPDATA", NULL, program_name);
#endif
}

static inline int path_find_cache_root(const char *program_name) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	if (path_find_root(&cache_root, "XDG_CACHE_HOME", NULL, program_name)) {
		return path_find_root(&cache_root, "HOME", PCHARS(".cache"), program_name);
	}
	return 0;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return path_find_root(&cache_root, "LOCALAPPDATA", NULL, program_name);
#endif
}

static inline int path_find_temp_root(const char *program_name) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	if (path_find_root(&temp_root, "TMPDIR", NULL, program_name)) {
		return path_find_root(&temp_root, NULL, PCHARS("/tmp/"), program_name);
	}
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	if (path_find_root(&temp_root, "TEMP", NULL, program_name)) {
		return path_find_root(&temp_root, "TMP", NULL, program_name);
	}
#endif
	return 0;
}

Path* path_resolve_root(const char **rest) {
	if ((*rest)[0] == '/' && (*rest)[1] == '/') {
		*rest += 2;
		if (!strncmp(*rest, "CONFIG/", 7)) {
			*rest += 7;
			return &config_root;
		}
		else if (!strncmp(*rest, "CACHE/", 6)) {
			*rest += 6;
			return &cache_root;
		}
		else if (!strncmp(*rest, "TEMP/", 5)) {
			*rest += 5;
			return &temp_root;
		} 
	}
	
	return &program_root;
}

static inline int path_normalize_subpath(char *normalized, size_t *normalized_length, const char *subpath) {
	size_t start = 0;
	size_t length = 0;

	for (size_t i = 0; ; ++i) {
		char c = subpath[i];

		// Handle the end of a segment
		if (c == '/' || c == '\0') {
			size_t segment_length = i - start;

			if (!segment_length) {} // Skip empty segments
			else if (segment_length == 1 && subpath[start] == '.') {} // Skip segments consisting of just "/./"
			else if (segment_length == 2 && subpath[start] == '.' && subpath[start + 1] == '.') {
				// Return an error if the subpath escapes the virtual root
				if (length == 0) return FILE_ERROR_INVALID_PATH;

				// Find previous slash
				size_t new_length = length;
				while (new_length > 0 && normalized[new_length - 1] != '/') --new_length;

				// Remove prior segment
				length = new_length;
			}
			else {
				if (length && normalized[length - 1] != '/') normalized[length++] = '/';

				if (length + segment_length >= PATH_SIZE) return DESCENT_ERROR_OVERFLOW;

				memcpy(normalized + length, subpath + start, segment_length);
				length += segment_length;
			}

			start = i + 1;
		}

		if (c == '\0') break;
	}

	normalized[length] = PATH_TERMINATOR;
	*normalized_length = length;
	return 0;
}

int path_resolve_dpath(Path *p, const char *dpath) {
	if (!p || !dpath) return DESCENT_ERROR_NULL;

	// Clear the output path
	path_invalidate(p, 0);

	// Resolve the dpath
	const char *rest = dpath;
	Path *root = path_resolve_root(&rest);

	// Validate virtual root
	if (root->subpath == 0) return DESCENT_ERROR_STATE;
	if (root->string[root->subpath - 1] != PATH_SEPARATOR) return DESCENT_ERROR_STATE;

	// Remove leading separators
	while (rest[0] == '/') ++rest;

	char normalized[PATH_SIZE];
	size_t length = 0;

	int result = path_normalize_subpath(normalized, &length, rest);
	if (result) return path_invalidate(p, result);
	
	result = path_copy(p, root);
	if (result) return path_invalidate(p, result);

	result = path_join_chars(p, normalized, length);
	if (result) return path_invalidate(p, result);

	return 0;
}

const Path *path_get_program_root(void) {
	return &program_root;
}

const Path *path_get_config_root(void) {
	return &config_root;
}

const Path *path_get_cache_root(void) {
	return &cache_root;
}

const Path *path_get_temp_root(void) {
	return &temp_root;
}

int file_init_virtual_roots(const char *program_name) {
	if (!program_name) return DESCENT_ERROR_NULL;
	if (!program_name[0]) return DESCENT_ERROR_INVALID;
	
	static int set_roots = 0;

	int result = 0;

	rwlock_write_lock(&roots_lock);

	if (set_roots) {
		rwlock_write_unlock(&roots_lock);
		return DESCENT_ERROR_FORBIDDEN;
	}

	set_roots = 1;

	if ((result = path_find_program_root())) {
		rwlock_write_unlock(&roots_lock);
		return result;
	}

	if ((result = path_find_config_root(program_name))) {
		rwlock_write_unlock(&roots_lock);
		return result;
	}

	if ((result = path_find_cache_root(program_name))) {
		rwlock_write_unlock(&roots_lock);
		return result;
	}

	if ((result = path_find_temp_root(program_name))) {
		rwlock_write_unlock(&roots_lock);
		return result;
	}

	rwlock_write_unlock(&roots_lock);

	return 0;
}
