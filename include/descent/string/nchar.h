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

#ifndef DESCENT_IM_STRING_NCHAR_H
#define DESCENT_IM_STRING_NCHAR_H

#include <stddef.h>

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <wchar.h>
#endif

#include <descent/rcode.h>

#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#define NCHAR(c) c
#define NCHARS(s) s
typedef char nchar;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#define NCHAR(c) L##c
#define NCHARS(s) L##s
typedef wchar_t nchar;
#endif

/**
 * @brief Calculate the length of an nchar string.
 * 
 * @param ns The nchar string to measure.
 * @return The length of the string.
 */
size_t nchars_length(const nchar *ns);

/**
 * @brief Find the first instance of an nchar in an nchar string.
 * 
 * @param ns The nchar string to search.
 * @param c The nchar to find.
 * @return A pointer to the first instance of the nchar, or NULL if not found.
 */
nchar *nchars_first(nchar *ns, nchar c);

/**
 * @brief Find the first instance of an nchar in a const nchar string.
 * 
 * @param ns The const nchar string to search.
 * @param c The nchar to find.
 * @return A const pointer to the first instance of the nchar, or NULL if not found.
 */
const nchar *nchars_view_first(const nchar *ns, nchar c);

/**
 * @brief Find the last instance of an nchar in an nchar string.
 * 
 * @param ns The nchar string to search.
 * @param c The nchar to find.
 * @return A pointer to the last instance of the nchar, or NULL if not found.
 */
nchar *nchars_last(nchar *ns, nchar c);

/**
 * @brief Find the last instance of an nchar in a const nchar string.
 * 
 * @param ns The const nchar string to search.
 * @param c The nchar to find.
 * @return A const pointer to the last instance of the nchar, or NULL if not found.
 */
const nchar *nchars_view_last(const nchar *ns, nchar c);

/**
 * @brief Converts a char string in cs to an nchar string in ns.
 * 
 * @param ns_size The size of the output buffer. Must include space for a null terminator.
 * @param ns The output buffer. Must not be NULL.
 * @param ns_length A pointer to receive the length of the string in the output buffer. May be NULL.
 * @param cs The input buffer. Must not be NULL.
 * @param cs_length The length of the input buffer, excluding the null terminator. If zero,
 * this function will find the length internally.
 * @return 0 on success, error code otherwise. 
 */
rcode chars_to_nchars(size_t ns_size, nchar *ns, size_t *ns_length, const char *cs, size_t cs_length);

/**
 * @brief Converts an nchar string in ns to a char string in cs.
 * 
 * @param cs_size The size of the output buffer. Must include space for a null terminator.
 * @param cs The output buffer. Must not be NULL.
 * @param cs_length A pointer to receive the length of the string in the output buffer. May be NULL.
 * @param ns The input buffer. Must not be NULL.
 * @param ns_length The length of the input buffer, excluding the null terminator. If zero,
 * this function will find the length internally.
 * @return 0 on success, error code otherwise.
 */
rcode nchars_to_chars(size_t cs_size, char *cs, size_t *cs_length, const nchar *ns, size_t ns_length);

/**
 * @brief Prints a formatted nchar string into a buffer. Extra characters are terminated
 * 
 * @param ns_size The size of the output buffer. Must include space for a null terminator.
 * @param ns The output buffer. Must not be NULL.
 * @param format The format string.
 * @param ... Variadic elements for the format string.
 * @return 0 on success, error code otherwise.
 */
rcode nchars_format(size_t ns_size, nchar *ns, nchar *format, ...);

#endif