// Copyright 2025 XavierHarkonnen9 and Enlarium
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef DESCENT_UTILITIES_BUILTIN_FALLBACK_ARITHMETIC_H
#define DESCENT_UTILITIES_BUILTIN_FALLBACK_ARITHMETIC_H

#include <stdint.h>

#include <descent/utilities/builtin/api.h>

#ifdef __cplusplus
namespace descent::builtin {
#endif

// All fallbacks must be cross platform, and thus implemented according to the base C standard.
// Exceptions include:
// - The existence of int*_t/uint*_t types for 8, 16, 32, and 64
// - Signed shifts are arithmetic
// - Negative numbers use two's complement

// Subtraction with Overflow

DESCENT_BUILTIN_API int
fb_usub_overflow_64(uint64_t a, uint64_t b, uint64_t *result) {
	*result = a - b;
	return a < b;
}

DESCENT_BUILTIN_API int
fb_usub_overflow_32(uint32_t a, uint32_t b, uint32_t *result) {
	*result = a - b;
	return a < b;
}

DESCENT_BUILTIN_API int
fb_usub_overflow_16(uint16_t a, uint16_t b, uint16_t *result) {
	*result = a - b;
	return a < b;
}

DESCENT_BUILTIN_API int
fb_usub_overflow_8(uint8_t a, uint8_t b, uint8_t *result) {
	*result = a - b;
	return a < b;
}

DESCENT_BUILTIN_API int
fb_sub_overflow_64(int64_t a, int64_t b, int64_t *result) {
	*result = (int64_t) ((uint64_t) a - (uint64_t) b);
	return (b > 0 && a < INT64_MIN + b) || (b < 0 && a > INT64_MAX + b);
}

DESCENT_BUILTIN_API int
fb_sub_overflow_32(int32_t a, int32_t b, int32_t *result) {
	*result = (int32_t) ((uint32_t) a - (uint32_t) b);
	return (b > 0 && a < INT32_MIN + b) || (b < 0 && a > INT32_MAX + b);
}

DESCENT_BUILTIN_API int
fb_sub_overflow_16(int16_t a, int16_t b, int16_t *result) {
	*result = (int16_t) ((uint16_t) a - (uint16_t) b);
	return (b > 0 && a < INT16_MIN + b) || (b < 0 && a > INT16_MAX + b);
}

DESCENT_BUILTIN_API int
fb_sub_overflow_8(int8_t a, int8_t b, int8_t *result) {
	*result = (int8_t) ((uint8_t) a - (uint8_t) b);
	return (b > 0 && a < INT8_MIN + b) || (b < 0 && a > INT8_MAX + b);
}

// Multiplication with Overflow

DESCENT_BUILTIN_API int
fb_umul_overflow_64(uint64_t a, uint64_t b, uint64_t *result) {
	uint64_t high = 0;
	fb_umul_partial_128(a, b, &high, result);
	return !!high;
}

DESCENT_BUILTIN_API int
fb_umul_overflow_32(uint32_t a, uint32_t b, uint32_t *result) {
	uint64_t product = (uint64_t) a * (uint64_t) b;
	*result = (uint32_t) product;
	return product > UINT32_MAX;
}

DESCENT_BUILTIN_API int
fb_umul_overflow_16(uint16_t a, uint16_t b, uint16_t *result) {
	uint32_t product = (uint32_t) a * (uint32_t) b;
	*result = (uint16_t) product;
	return product > UINT16_MAX;
}

DESCENT_BUILTIN_API int
fb_umul_overflow_8(uint8_t a, uint8_t b, uint8_t *result) {
	uint16_t product = (uint16_t) a * (uint16_t) b;
	*result = (uint8_t) product;
	return product > UINT8_MAX;
}

// TODO: Signed Multiplication with Overflow

#ifdef __cplusplus
}
#endif

#endif
