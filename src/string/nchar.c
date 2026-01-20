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

#include <descent/string/nchar.h>

#include <stdio.h>
#include <stdarg.h>

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <string.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <wchar.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <descent/rcode.h>

size_t nchars_length(const nchar *ns) {
	if (!ns) return 0;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return strlen(ns);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return wcslen(ns);
#endif
}

nchar *nchars_first(nchar *ns, nchar c) {
	if (!ns) return NULL;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return strchr(ns, c);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return wcschr(ns, c);
#endif
}

const nchar *nchars_view_first(const nchar *ns, nchar c) {
	if (!ns) return NULL;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return (const nchar *)strchr(ns, c);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return (const nchar *)wcschr(ns, c);
#endif
}

nchar *nchars_last(nchar *ns, nchar c) {
	if (!ns) return NULL;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return strrchr(ns, c);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return wcsrchr(ns, c);
#endif
}

const nchar *nchars_view_last(const nchar *ns, nchar c) {
	if (!ns) return NULL;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return (const nchar *)strrchr(ns, c);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return (const nchar *)wcsrchr(ns, c);
#endif
}

rcode chars_to_nchars(size_t ns_size, nchar *ns, size_t *ns_length, const char *cs, size_t cs_length) {
	if (!ns || !cs) return DESCENT_ERROR_NULL;

	// Get input size
	size_t cs_size = cs_length + 1;
	if (!cs_length) cs_size = nchars_length(cs) + 1;

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

	if (cs_size > ns_size) return DESCENT_ERROR_OVERFLOW;
	memcpy(ns, cs, cs_size);
	size_t length = cs_size - 1;

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

	if ((cs_size - 1 > (size_t) INT_MAX) || (ns_size - 1 > (size_t) INT_MAX)) {
		return DESCENT_ERROR_OVERFLOW;
	}

	// Do not translate null terminator
	int ics_size = cs_size - 1;
	int ins_size = ns_size - 1;

	int result = MultiByteToWideChar(
		CP_UTF8,
		MB_ERR_INVALID_CHARS,
		cs,
		ics_size,
		ns,
		ins_size
	);

	if (!result) return DESCENT_ERROR_OS;
	if (result > ins_size) return DESCENT_ERROR_OVERFLOW;
	size_t length = result;

#endif

	ns[length] = NCHAR('\0');
	if (ns_length) *ns_length = length;

	return 0;
}

rcode nchars_to_chars(size_t cs_size, char *cs, size_t *cs_length, const nchar *ns, size_t ns_length) {
	if (!cs || !ns) return DESCENT_ERROR_NULL;

	// Get input size
	size_t ns_size = ns_length + 1;
	if (!ns_length) ns_size = nchars_length(ns) + 1;

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

	if (ns_size > cs_size) return DESCENT_ERROR_OVERFLOW;
	memcpy(cs, ns, ns_size);
	size_t length = ns_size - 1;

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

	if ((cs_size - 1 > (size_t) INT_MAX) || (ns_size - 1 > (size_t) INT_MAX)) {
		return DESCENT_ERROR_OVERFLOW;
	}

	// Do not translate null terminator
	int ics_size = cs_size - 1;
	int ins_size = ns_size - 1;

	int result = WideCharToMultiByte(
		CP_UTF8,
		WC_ERR_INVALID_CHARS,
		ns,
		ins_size,
		cs,
		ics_size,
		NULL,
		NULL
	);

	if (!result) return DESCENT_ERROR_OS;
	if (result > ics_size) return DESCENT_ERROR_OVERFLOW;
	size_t length = (size_t) result;

#endif

	cs[length] = '\0';
	if (cs_length) *cs_length = length;

	return 0;
}

rcode nchars_format(size_t ns_size, nchar *ns, nchar *format, ...) {
	if (!ns || !format) return DESCENT_ERROR_NULL;

	va_list args;
	va_start(args, format);

	int written;

#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	written = vsnprintf(ns, ns_size, format, args);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	written = _vsnwprintf(ns, ns_size, format, args);
#endif

	va_end(args);

	if (written < 0 || (size_t) written >= ns_size) {
		return DESCENT_WARN_TRUNCATION;
	}

	return 0;
}
