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

#ifndef DESCENT_UTILITIES_VERSION_H
#define DESCENT_UTILITIES_VERSION_H

#include <stdint.h>

typedef struct {
	uint16_t major;
	uint16_t minor;
	uint16_t patch;
	uint16_t variant;
} Version;

// Checks whether a is newer (positive), the same (zero) or older (negative)than b
static inline int version_compare(Version a, Version b) {
    if (a.major != b.major) return a.major - b.major;
    if (a.minor != b.minor) return a.minor - b.minor;
    if (a.patch != b.patch) return a.patch - b.patch;
    return a.variant - b.variant;
}

static inline uint32_t version_to_vk_api(Version v) {
	if (v.variant >= 8 || v.major >= 128 || v.minor >= 1024 || v.major >= 4096) return 0;
	return ((((uint32_t)(v.variant)) << 29U) | (((uint32_t)(v.major)) << 22U) | (((uint32_t)(v.minor)) << 12U) | ((uint32_t)(v.patch)));
}

static inline Version version_from_vk_api(uint32_t v) {
	Version result;
	result.variant = (uint16_t)((v >> 29U) & 0x7U);
	result.major   = (uint16_t)((v >> 22U) & 0x7FU);
	result.minor   = (uint16_t)((v >> 12U) & 0x3FFU);
	result.patch   = (uint16_t)(v & 0xFFFU);
	return result;
}

#endif