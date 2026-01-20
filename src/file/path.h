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

#ifndef DESCENT_SOURCE_FILE_PATH_H
#define DESCENT_SOURCE_FILE_PATH_H

#include <limits.h>

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <wchar.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <string.h>
#endif

#include <descent/rcode.h>

#ifndef DESCENT_PATH_SIZE_WINDOWS
#define DESCENT_PATH_SIZE_WINDOWS 4096
#endif

// Paths longer than PATH_SIZE are unsupported.
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#define PATH_SIZE PATH_MAX
#define PCHAR(c) c
#define PCHARS(s) s
#define PATH_SEPARATOR PCHAR('/')
typedef char pchar;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#define PATH_SIZE DESCENT_PATH_SIZE_WINDOWS
#define PCHAR(c) L##c
#define PCHARS(s) L##s
#define PATH_SEPARATOR PCHAR('\\')
typedef wchar_t pchar;
#endif

#define PATH_MAX_LENGTH (PATH_SIZE - 1)
#define PATH_TERMINATOR PCHAR('\0')

typedef struct {
	pchar string[PATH_SIZE];
	size_t length;
	size_t subpath;
} Path;

static inline void path_normalize_separators(Path *p) {
#if defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	if (p) for (size_t i = 0; p->string[i]; ++i) if (p->string[i] == PCHAR('/')) p->string[i] = PATH_SEPARATOR;
#else
	(void) p;
#endif
}

static inline int path_invalidate(Path *p, int result) {
	if (!p) return DESCENT_ERROR_NULL;

	p->length = 0;
	p->subpath = 0;
	p->string[0] = PATH_TERMINATOR;
	return result;
}

static inline int path_ensure_separator(Path *p) {
	if (!p) return DESCENT_ERROR_NULL;
	
	if (p->length && p->string[p->length - 1] != PATH_SEPARATOR) {
		if (p->length >= PATH_MAX_LENGTH) {
			return path_invalidate(p, DESCENT_ERROR_OVERFLOW);
		}

		p->string[p->length++] = PATH_SEPARATOR;
	}

	return 0;
}

static inline size_t pchars_length(const pchar *ps) {
	if (!ps) return 0;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return strlen(ps);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return wcslen(ps);
#endif
}

static inline pchar *path_first_pchar(pchar *ps, pchar c) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return strchr(ps, c);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return wcschr(ps, c);
#endif
}

static inline const pchar *path_first_pchar_view(const pchar *ps, pchar c) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return (const pchar *)strchr(ps, c);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return (const pchar *)wcschr(ps, c);
#endif
}

static inline pchar *path_last_pchar(pchar *ps, pchar c) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return strrchr(ps, c);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return wcsrchr(ps, c);
#endif
}

static inline const pchar *path_last_pchar_view(const pchar *ps, pchar c) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return (const pchar *)strrchr(ps, c);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return (const pchar *)wcsrchr(ps, c);
#endif
}

/**
 * @brief Converts a char string in cs to a pchar string in ps.
 * 
 * @param ps The output buffer. Must not be NULL.
 * @param ps_size The size of the output buffer. Must include space for a null terminator.
 * @param ps_length A pointer to receive the length of the string in the output buffer. May be NULL.
 * @param cs The input buffer. Must not be NULL.
 * @param cs_length The length of the input buffer, excluding the null terminator. If zero,
 * this function will find the length internally.
 * @return 0 on success, error code otherwise. 
 */
static inline int chars_to_pchars(
	pchar *ps,
	size_t ps_size,
	size_t *ps_length,
	const char *cs,
	size_t cs_length
) {
	if (!ps || !cs) return DESCENT_ERROR_NULL;

	size_t cs_size = cs_length + 1;
	if (!cs_length) cs_size = strlen(cs) + 1;

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

	if (cs_size > ps_size) return DESCENT_ERROR_OVERFLOW;
	memcpy(ps, cs, cs_size);
	size_t length = cs_size - 1;

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

	if ((cs_size - 1 > (size_t) INT_MAX) || (ps_size - 1 > (size_t) INT_MAX)) {
		return DESCENT_ERROR_OVERFLOW;
	}

	// Do not translate null terminator
	int ics_size = cs_size - 1;
	int ips_size = ps_size - 1;

	int result = MultiByteToWideChar(
		CP_UTF8,
		MB_ERR_INVALID_CHARS,
		cs,
		ics_size,
		ps,
		ips_size
	);

	if (!result) return DESCENT_ERROR_OS;
	if (result > ips_size) return DESCENT_ERROR_OVERFLOW;
	size_t length = result;

#endif

	ps[length] = PATH_TERMINATOR;
	if (ps_length) *ps_length = length;

	return 0;
}

/**
 * @brief Converts a pchar string in ps to a char string in cs.
 * 
 * @param cs The output buffer. Must not be NULL.
 * @param cs_size The size of the output buffer. Must include space for a null terminator.
 * @param cs_length A pointer to receive the length of the string in the output buffer. May be NULL.
 * @param ps The input buffer. Must not be NULL.
 * @param ps_length The length of the input buffer, excluding the null terminator. If zero,
 * this function will find the length internally.
 * @return 0 on success, error code otherwise. 
 */
static inline int pchars_to_chars(
	char *cs,
	size_t cs_size,
	size_t *cs_length,
	const pchar *ps,
	size_t ps_length
) {
	if (!cs || !ps) return DESCENT_ERROR_NULL;

	size_t ps_size = ps_length + 1;
	if (!ps_length) ps_size = pchars_length(ps) + 1;

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

	if (ps_size > cs_size) return DESCENT_ERROR_OVERFLOW;
	memcpy(cs, ps, ps_size);
	size_t length = ps_size - 1;

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

	if ((cs_size - 1 > (size_t) INT_MAX) || (ps_size - 1 > (size_t) INT_MAX)) {
		return DESCENT_ERROR_OVERFLOW;
	}

	// Do not translate null terminator
	int ics_size = cs_size - 1;
	int ips_size = ps_size - 1;

	int result = WideCharToMultiByte(
		CP_UTF8,
		WC_ERR_INVALID_CHARS,
		ps,
		ips_size,
		cs,
		ics_size,
		NULL,
		NULL
	);

	if (!result) return DESCENT_ERROR_OS;
	if (result > ics_size) return DESCENT_ERROR_OVERFLOW;
	size_t length = (size_t) result;

#endif

	cs[length] = PATH_TERMINATOR;
	if (cs_length) *cs_length = length;

	return 0;
}

static inline int chars_to_path(Path *p, const char *cs, size_t cs_length) {
	if (!p || !cs) return DESCENT_ERROR_NULL;

	int result = chars_to_pchars(p->string, PATH_SIZE, &p->length, cs, cs_length);
	if (result) return path_invalidate(p, result);
	
	p->subpath = 0;
	path_normalize_separators(p);

	return 0;
}

static inline int path_to_chars(char *cs, size_t cs_size, size_t *cs_length, const Path *p) {
	if (!cs || !p) return DESCENT_ERROR_NULL;
	
	return pchars_to_chars(cs, cs_size, cs_length, p->string, p->length);
}

static inline int path_copy_pchars(Path *p, const pchar *ps, size_t ps_length) {
	if (!p || !ps) return DESCENT_ERROR_NULL;

	if (!ps_length) ps_length = pchars_length(ps);
	if (ps_length > PATH_MAX_LENGTH) return path_invalidate(p, DESCENT_ERROR_OVERFLOW);

	memcpy(p->string, ps, ps_length * sizeof(pchar));
	path_normalize_separators(p);

	p->string[ps_length] = PATH_TERMINATOR;
	p->length = ps_length;
	p->subpath = 0;

	return 0;
}

static inline int path_copy_chars(Path *p, const char *cs, size_t cs_length) {
	if (!p || !cs) return DESCENT_ERROR_NULL;

#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return path_copy_pchars(p, cs, cs_length);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return chars_to_path(p, cs, cs_length);
#endif
}

static inline int path_copy(Path *destination, const Path *source) {
	if (!destination || !source) return DESCENT_ERROR_NULL;

	if (destination == source) return 0;

	size_t length = source->length;
	length = length > PATH_MAX_LENGTH ? PATH_MAX_LENGTH : length;

	memcpy(destination->string, source->string, length * sizeof(pchar));
	destination->length = length;
	destination->subpath = source->subpath;
	destination->string[length] = PATH_TERMINATOR;

	return 0;
}

static inline int path_join_pchars(Path *p, const pchar *ps, size_t ps_length) {
	if (!p || !ps) return DESCENT_ERROR_NULL;
	
	if (!ps_length) ps_length = pchars_length(ps);
	if (ps_length == 0) return 0;

	int result = path_ensure_separator(p);
	if (result) return path_invalidate(p, result);

	if (p->length + ps_length > PATH_MAX_LENGTH) {
		return path_invalidate(p, DESCENT_ERROR_OVERFLOW);
	}

	memcpy(p->string + p->length, ps, ps_length * sizeof(pchar));
	path_normalize_separators(p);

	p->length += ps_length;
	p->string[p->length] = PATH_TERMINATOR;

	return 0;
}

static inline int path_join_chars(Path *p, const char *cs, size_t cs_length) {
	if (!p || !cs) return DESCENT_ERROR_NULL;

#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return path_join_pchars(p, cs, cs_length);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	Path wide = {0};

	int result = chars_to_path(&wide, cs, cs_length);
	if (result) return result;

	return path_join_pchars(p, wide.string, wide.length);
#endif
}

// Leaves trailing separator
static inline int path_dirname(Path *destination, const Path *source) {
	if (!destination || !source) return DESCENT_ERROR_NULL;

	int result = path_copy(destination, source);
	if (result) return result;

	pchar *subpath = destination->string + destination->subpath;
	pchar *last_separator = path_last_pchar(subpath, PATH_SEPARATOR);

	for (;;) {
		if (!last_separator) {
			// No root
			if (destination->subpath == 0) {
				destination->string[0] = PCHAR('.');
				destination->string[1] = PATH_TERMINATOR;
				destination->length = 1;
			}

			// Truncate to root
			else {
				*subpath = PATH_TERMINATOR;
				destination->length = destination->subpath;
			}

			return 0;
		}

		// If the last separator has a character after it, truncate there
		if (last_separator[1]) {
			last_separator[1] = PATH_TERMINATOR;
			destination->length = (last_separator - destination->string) + 1;
			return 0;
		}

		// Last separator is trailing — remove and continue
		*last_separator = PATH_TERMINATOR;
		destination->length = (size_t)(last_separator - destination->string);
	}
}

int path_resolve_dpath(Path *p, const char *dpath);

const Path *path_get_program_root(void);

const Path *path_get_config_root(void);

const Path *path_get_cache_root(void);

const Path *path_get_temp_root(void);

#endif