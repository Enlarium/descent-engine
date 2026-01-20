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

// path_*: native type path functions. Not exposed.
// dpath_*: utf-8 descent virtual path functions.
// file_*: file manipulation functions.

// dpaths use virtual roots which are mapped to real paths for all calls:
// - "/" -> "<program-dir>/" (non-canonical, resolves to "//")
// - "//" -> "<program-dir>/"
// - "//CONFIG/" -> "<config-dir>/"
// - "//TEMP/" -> "<temp-dir>/"
// - "//CACHE/" -> "<cache-dir>/"

// - dpaths are UTF-8 encoded (translated to UTF-16 for windows when OS calls are made)
// - dpaths are case-insensitive (helps enforce that programs run the same way on Windows and non-Windows)
// - dpath segments (file and directory names) may not start with % (reserved for virtual roots)
// - the dpath separator is "/" (translated to native where necessary)

// Allow low-level file operations with this library - don't give users a reason to accidentally escape by using fopen.
// file_resolve - resolve paths to canonical within virtual space, forbid escape
// Even if temp contains cache and log, paths with cache or log roots can't escape to the temp space.

#define DPATH_SEPARATOR '/'
#define DPATH_TERMINATOR '\0'

#define DPATH_VIRTUAL_ROOT "/" // Treated as program root
#define DPATH_PROGRAM_ROOT "//"
#define DPATH_CONFIG_ROOT "//CONFIG/"
#define DPATH_CACHE_ROOT "//CACHE/"
#define DPATH_TEMP_ROOT "//TEMP/"

int dpath_canonical(size_t max_length, const char *destination, char *source);
int dpath_append(const char *dpath, char *out_dpath, size_t max_length);
int dpath_join(const char *dpath, char *out_dpath, size_t max_length);

// Cannot be used to escape virtual root
int dpath_dirname(char *path, unsigned int size) {
	if (!path) return DESCENT_ERROR_NULL;
	if (!*path) return DESCENT_ERROR_INVALID;
	if (size == 0) return DESCENT_ERROR_OVERFLOW;

	char *last_separator = strrchr(path, PATH_SEPARATOR);

	if (last_separator) {
		*last_separator = '\0';
		// Handle the case where the result might be empty
		if (*path == '\0') {
			if (size < 2) return DESCENT_ERROR_OVERFLOW;
			path[0] = '.';
			path[1] = '\0';
		}
	} else {
		// No separator found → return "."
		if (size < 2) return DESCENT_ERROR_OVERFLOW;
		path[0] = '.';
		path[1] = '\0';
	}

	return 0;
}

// dpath_canonical
// dpath_append // writes directly, no segment
// dpath_join
// dpath_parent
// dpath_filename
// dpath_stem
// dpath_extension

//// TO BE MOVED INTO UTILITIES
//static inline int string_copy(size_t size, char *destination, const char *source) {
//	if (!destination || !source) return DESCENT_ERROR_NULL;
//
//	size_t length = strlen(source) + 1;
//	if (length > size) return DESCENT_ERROR_OVERFLOW;
//
//	memcpy(destination, source, length);
//	return 0;
//}
