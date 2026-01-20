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

#ifndef DESCENT_IM_STRING_UTF_8_H
#define DESCENT_IM_STRING_UTF_8_H

#include <stddef.h>
#include <string.h>

#include <descent/rcode.h>

static inline size_t utf8_char_length(unsigned char c) {
	if ((c & 0x80) == 0)    return 1; // ASCII
	if ((c & 0xE0) == 0xC0) return 2; // 110xxxxx
	if ((c & 0xF0) == 0xE0) return 3; // 1110xxxx
	if ((c & 0xF8) == 0xF0) return 4; // 11110xxx
	return 0; // invalid byte
}

static inline int utf8_copy_truncate(size_t size, char *destination, const char *source) {
	if (!destination || !source) return DESCENT_ERROR_NULL;
	if (!size) return DESCENT_ERROR_OVERFLOW;

	int result = 0;

	size_t length;
	size_t c_length;
	for (length = 0; source[length]; length += c_length) {
		c_length = utf8_char_length((unsigned char) source[length]);

		if (!c_length) {
			result = DESCENT_ERROR_INVALID;
			break; // invalid UTF-8 byte
		}
		if (length + c_length >= size) {
			result = DESCENT_WARN_TRUNCATION;
			break; // prevent buffer overflow
		}
	}

	memcpy(destination, source, length);
	destination[length] = '\0';

	return result;
}

#endif