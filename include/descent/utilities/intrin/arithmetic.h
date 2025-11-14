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

#ifndef DESCENT_UTILITIES_BUILTIN_ARITHMETIC_H
#define DESCENT_UTILITIES_BUILTIN_ARITHMETIC_H

#include <stdbool.h>
#include <stdint.h>

#include <descent/utilities/builtin/api.h>

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_COMPILER_MSVC)
#include <intrin.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Sign

/**
 * @brief Gets the sign of a signed 64-bit integer.
 * @param v The operand.
 * @return 0 if the operand is 0, 1 if the operand is positive, -1 if the operand is negative.
 */
DESCENT_BUILTIN_API int
sign_64(int64_t v) { return (v > 0) - (v < 0); }

/**
 * @brief Gets the sign of a signed 32-bit integer.
 * @param v The operand.
 * @return 0 if the operand is 0, 1 if the operand is positive, -1 if the operand is negative.
 */
DESCENT_BUILTIN_API int
sign_32(int32_t v) { return (v > 0) - (v < 0); }

/**
 * @brief Gets the sign of a signed 16-bit integer.
 * @param v The operand.
 * @return 0 if the operand is 0, 1 if the operand is positive, -1 if the operand is negative.
 */
DESCENT_BUILTIN_API int
sign_16(int16_t v) { return (v > 0) - (v < 0); }

/**
 * @brief Gets the sign of a signed 8-bit integer.
 * @param v The operand.
 * @return 0 if the operand is 0, 1 if the operand is positive, -1 if the operand is negative.
 */
DESCENT_BUILTIN_API int
sign_8(int8_t v) { return (v > 0) - (v < 0); }

// Signed Addition with Exceeded Flow

/**
 * @brief Adds two signed 64-bit integers with overflow and underflow detection.
 * @param a The first operand.
 * @param b The second operand.
 * @param result Pointer to a signed 64-bit integer where the sum will be stored.
 * @return 0 if no overflow occurs, 1 if overflow occurs, -1 if underflow occurs.
 */
DESCENT_BUILTIN_API int
add_exflow_64(int64_t a, int64_t b, int64_t *result) {
#if DESCENT_HAS_BUILTIN(__builtin_saddll_overflow)
	return __builtin_saddll_overflow(a, b, (long long *)result) ? sign_64(b) : 0;
#else
	*result = (int64_t) ((uint64_t) a + (uint64_t) b);
	return (b > 0 && a > INT64_MAX - b) - (b < 0 && a < INT64_MIN - b);
#endif
}

/**
 * @brief Adds two signed 32-bit integers with overflow and underflow detection.
 * @param a The first operand.
 * @param b The second operand.
 * @param result Pointer to a signed 32-bit integer where the sum will be stored.
 * @return 0 if no overflow occurs, 1 if overflow occurs, -1 if underflow occurs.
 */
DESCENT_BUILTIN_API int
add_exflow_32(int32_t a, int32_t b, int32_t* result) {
#if DESCENT_HAS_BUILTIN(__builtin_sadd_overflow)
	return __builtin_sadd_overflow(a, b, (int *)result) ? sign_32(b) : 0;
#else
	int64_t wide_result = (int64_t) a + (int64_t) b;
	*result = (int32_t) wide_result;
	return (wide_result > INT32_MAX) - (wide_result < INT32_MIN);
#endif
}

/**
 * @brief Adds two signed 16-bit integers with overflow and underflow detection.
 * @param a The first operand.
 * @param b The second operand.
 * @param result Pointer to a signed 16-bit integer where the sum will be stored.
 * @return 0 if no overflow occurs, 1 if overflow occurs, -1 if underflow occurs.
 */
DESCENT_BUILTIN_API int
add_exflow_16(int16_t a, int16_t b, int16_t* result) {
	int32_t wide_result = (int32_t) a + (int32_t) b;
	*result = (int16_t) wide_result;
	return (wide_result > INT16_MAX) - (wide_result < INT16_MIN);
}

/**
 * @brief Adds two signed 8-bit integers with overflow and underflow detection.
 * @param a The first operand.
 * @param b The second operand.
 * @param result Pointer to a signed 8-bit integer where the sum will be stored.
 * @return 0 if no overflow occurs, 1 if overflow occurs, -1 if underflow occurs.
 */
DESCENT_BUILTIN_API int
add_exflow_8(int8_t a, int8_t b, int8_t* result) {
	int16_t wide_result = (int16_t) a + (int16_t) b;
	*result = (int8_t) wide_result;
	return (wide_result > INT8_MAX) - (wide_result < INT8_MIN);
}

// Addition with Overflow

/**
 * @brief Adds two unsigned 64-bit integers with overflow and underflow detection.
 * @param a The first operand.
 * @param b The second operand.
 * @param result Pointer to an unsigned 64-bit integer where the sum will be stored.
 * @return 0 if no overflow occurs, 1 if overflow occurs.
 */
DESCENT_BUILTIN_API int
uadd_overflow_64(uint64_t a, uint64_t b, uint64_t *result) {
#if DESCENT_HAS_BUILTIN(__builtin_uaddll_overflow)
return __builtin_uaddll_overflow(a, b, (unsigned long long *)result);
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC) && (DESCENT_PLATFORM_ARCHITECTURE_SIZE == 64) && (_MSC_VER >= 1800)
	return _addcarry_u64(0, a, b, result);
#else
	*result = a + b;
	return *result < a;
#endif
}

/**
 * @brief Adds two unsigned 32-bit integers with overflow and underflow detection.
 * @param a The first operand.
 * @param b The second operand.
 * @param result Pointer to an unsigned 32-bit integer where the sum will be stored.
 * @return 0 if no overflow occurs, 1 if overflow occurs.
 */
DESCENT_BUILTIN_API int
uadd_overflow_32(uint32_t a, uint32_t b, uint32_t *result) {
#if DESCENT_HAS_BUILTIN(__builtin_uadd_overflow)
	return __builtin_uadd_overflow(a, b, result);
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC) && (_MSC_VER >= 1800)
	return _addcarry_u32(0, a, b, result);
#else
	*result = a + b;
	return *result < a;
#endif
}

/**
 * @brief Adds two unsigned 16-bit integers with overflow and underflow detection.
 * @param a The first operand.
 * @param b The second operand.
 * @param result Pointer to an unsigned 16-bit integer where the sum will be stored.
 * @return 0 if no overflow occurs, 1 if overflow occurs.
 */
DESCENT_BUILTIN_API int
uadd_overflow_16(uint16_t a, uint16_t b, uint16_t *result) {
#if DESCENT_HAS_BUILTIN(__builtin_uadd_overflow)
	unsigned int wide_result = 0;
	int overflow = __builtin_uadd_overflow(a, b, &wide_result);
	*result = (uint16_t) wide_result;
	return overflow;
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC) && (_MSC_VER >= 1800)
	return _addcarry_u16(0, a, b, result);
#else
	*result = a + b;
	return *result < a;
#endif
}

/**
 * @brief Adds two unsigned 8-bit integers with overflow and underflow detection.
 * @param a The first operand.
 * @param b The second operand.
 * @param result Pointer to an unsigned 8-bit integer where the sum will be stored.
 * @return 0 if no overflow occurs, 1 if overflow occurs.
 */
DESCENT_BUILTIN_API int
uadd_overflow_8(uint8_t a, uint8_t b, uint8_t *result) {
#if DESCENT_HAS_BUILTIN(__builtin_uadd_overflow)
	unsigned int wide_result = 0;
	int overflow = __builtin_uadd_overflow(a, b, &wide_result);
	*result = (uint8_t) wide_result;
	return overflow;
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC) && (_MSC_VER >= 1800)
	return _addcarry_u8(0, a, b, result);
#else
	*result = a + b;
	return *result < a;
#endif
}

/**
 * @brief Adds two signed 64-bit integers with overflow and underflow detection.
 * @param a The first operand.
 * @param b The second operand.
 * @param result Pointer to a signed 64-bit integer where the sum will be stored.
 * @return 0 if no overflow occurs, 1 if overflow occurs.
 */
DESCENT_BUILTIN_API int
add_overflow_64(int64_t a, int64_t b, int64_t *result) {
#if DESCENT_HAS_BUILTIN(__builtin_saddll_overflow)
return __builtin_saddll_overflow(a, b, (long long *)result);
#else
	return !!add_exflow_64(a, b, result);
#endif
}

/**
 * @brief Adds two signed 32-bit integers with overflow and underflow detection.
 * @param a The first operand.
 * @param b The second operand.
 * @param result Pointer to a signed 32-bit integer where the sum will be stored.
 * @return 0 if no overflow occurs, 1 if overflow occurs.
 */
DESCENT_BUILTIN_API int
add_overflow_32(int32_t a, int32_t b, int32_t *result) {
#if DESCENT_HAS_BUILTIN(__builtin_sadd_overflow)
	return __builtin_sadd_overflow(a, b, result);
#else
	return !!add_exflow_32(a, b, result);
#endif
}

/**
 * @brief Adds two signed 16-bit integers with overflow and underflow detection.
 * @param a The first operand.
 * @param b The second operand.
 * @param result Pointer to a signed 16-bit integer where the sum will be stored.
 * @return 0 if no overflow occurs, 1 if overflow occurs.
 */
DESCENT_BUILTIN_API int
add_overflow_16(int16_t a, int16_t b, int16_t *result) {
#if DESCENT_HAS_BUILTIN(__builtin_sadd_overflow)
	int wide_result = 0;
	int overflow = __builtin_sadd_overflow(a, b, &wide_result);
	*result = (int16_t) wide_result;
	return overflow;
#else
	return !!add_exflow_16(a, b, result);
#endif
}

/**
 * @brief Adds two signed 8-bit integers with overflow and underflow detection.
 * @param a The first operand.
 * @param b The second operand.
 * @param result Pointer to a signed 8-bit integer where the sum will be stored.
 * @return 0 if no overflow occurs, 1 if overflow occurs.
 */
DESCENT_BUILTIN_API int
add_overflow_8(int8_t a, int8_t b, int8_t *result) {
#if DESCENT_HAS_BUILTIN(__builtin_sadd_overflow)
	int wide_result = 0;
	int overflow = __builtin_sadd_overflow(a, b, &wide_result);
	*result = (int8_t) wide_result;
	return overflow;
#else
	return !!add_exflow_8(a, b, result);
#endif
}

// Addition with Carry

/**
 * @brief Adds two unsigned 64-bit integers with an incoming carry.
 * @param a The first operand.
 * @param b The second operand.
 * @param carry_in The incoming carry (0 or 1).
 * @param result Pointer to an unsigned 64-bit integer where the sum will be stored.
 * @return The carry-out (0 or 1).
 */
DESCENT_BUILTIN_API int
uadd_carry_64(uint64_t a, uint64_t b, int carry_in, uint64_t *result) {
#if DESCENT_HAS_BUILTIN(__builtin_addcll)
	unsigned long long carry_out = 0;
	*result = __builtin_addcll((unsigned long long) a, (unsigned long long) b, !!(unsigned long long) carry_in, &carry_out);
	return (uint64_t) carry_out;
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC) && (_MSC_VER >= 1800)
	return _addcarry_u64((unsigned char) !!carry_in, a, b, result);
#else
	uint64_t temp = a + b + !!carry_in;
	*result = temp;
	return (temp < a) || (carry_in && temp == a + b);
#endif
}

/**
 * @brief Adds two unsigned 32-bit integers with an incoming carry.
 * @param a The first operand.
 * @param b The second operand.
 * @param carry_in The incoming carry (0 or 1).
 * @param result Pointer to an unsigned 32-bit integer where the sum will be stored.
 * @return The carry-out (0 or 1).
 */
DESCENT_BUILTIN_API int
uadd_carry_32(uint32_t a, uint32_t b, int carry_in, uint32_t *result) {
#if DESCENT_HAS_BUILTIN(__builtin_addc)
	unsigned int carry_out = 0;
	*result = __builtin_addc((unsigned int) a, (unsigned int) b, !!(unsigned int) carry_in, &carry_out);
	return (uint32_t) carry_out;
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC) && (_MSC_VER >= 1800)
	return _addcarry_u32((unsigned char) !!carry_in, a, b, result);
#else
	uint64_t temp = (uint64_t) a + (uint64_t) b + !!(uint64_t) carry_in;
	*result = (uint32_t) temp;
	return !!(int) (temp >> 32);;
#endif
}

/**
 * @brief Adds two unsigned 16-bit integers with an incoming carry.
 * @param a The first operand.
 * @param b The second operand.
 * @param carry_in The incoming carry (0 or 1).
 * @param result Pointer to an unsigned 16-bit integer where the sum will be stored.
 * @return The carry-out (0 or 1).
 */
DESCENT_BUILTIN_API int
uadd_carry_16(uint16_t a, uint16_t b, int carry_in, uint16_t *result) {
#if DESCENT_HAS_BUILTIN(__builtin_addcs)
	unsigned short carry_out = 0;
	*result = __builtin_addcs((unsigned short) a, (unsigned short) b, !!(unsigned short) carry_in, &carry_out);
	return (uint16_t) carry_out;
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC) && (_MSC_VER >= 1800)
	return _addcarry_u16((unsigned char) !!carry_in, a, b, result);
#else
	uint32_t temp = (uint32_t) a + (uint32_t) b + !!(uint32_t) carry_in;
	*result = (uint16_t) temp;
	return !!(int) (temp >> 16);;
#endif
}

/**
 * @brief Adds two unsigned 8-bit integers with an incoming carry.
 * @param a The first operand.
 * @param b The second operand.
 * @param carry_in The incoming carry (0 or 1).
 * @param result Pointer to an unsigned 8-bit integer where the sum will be stored.
 * @return The carry-out (0 or 1).
 */
DESCENT_BUILTIN_API int
uadd_carry_8(uint8_t a, uint8_t b, int carry_in, uint8_t *result) {
#if DESCENT_HAS_BUILTIN(__builtin_addcb)
	unsigned char carry_out = 0;
	*result = __builtin_addcb((unsigned char) a, (unsigned char) b, !!(unsigned char) carry_in, &carry_out);
	return (uint8_t) carry_out;
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC) && (_MSC_VER >= 1800)
	return _addcarry_u8((unsigned char) !!carry_in, a, b, result);
#else
	uint16_t temp = (uint16_t) a + (uint16_t) b + !!(uint16_t) carry_in;
	*result = (uint8_t) temp;
	return !!(int) (temp >> 8);;
#endif
}

/**
 * @brief Adds two signed 64-bit integers with an incoming carry or borrow.
 * @param a The first operand.
 * @param b The second operand.
 * @param carry_in The incoming carry or borrow (-1, 0, or 1).
 * @param result Pointer to a signed 64-bit integer where the sum will be stored.
 * @return The carry-out (-1, 0, or 1).
 */
DESCENT_BUILTIN_API int
add_carry_64(int64_t a, int64_t b, int carry_in, int64_t *result) {
	carry_in = sign_64(carry_in);
	// Overflow and underflow can only occur once if the third operand is {-1,0,1}
	return add_exflow_64(a, b, result) + add_exflow_64(*result, carry_in, result);
}

/**
 * @brief Adds two signed 32-bit integers with an incoming carry or borrow.
 * @param a The first operand.
 * @param b The second operand.
 * @param carry_in The incoming carry or borrow (-1, 0, or 1).
 * @param result Pointer to a signed 32-bit integer where the sum will be stored.
 * @return The carry-out (-1, 0, or 1).
 */
DESCENT_BUILTIN_API int
add_carry_32(int32_t a, int32_t b, int carry_in, int32_t *result) {
	carry_in = sign_32(carry_in);
	// Overflow and underflow can only occur once if the third operand is {-1,0,1}
	return add_exflow_32(a, b, result) + add_exflow_32(*result, carry_in, result);
}

/**
 * @brief Adds two signed 16-bit integers with an incoming carry or borrow.
 * @param a The first operand.
 * @param b The second operand.
 * @param carry_in The incoming carry or borrow (-1, 0, or 1).
 * @param result Pointer to a signed 16-bit integer where the sum will be stored.
 * @return The carry-out (-1, 0, or 1).
 */
DESCENT_BUILTIN_API int
add_carry_16(int16_t a, int16_t b, int carry_in, int16_t *result) {
	carry_in = sign_16(carry_in);
	// Overflow and underflow can only occur once if the third operand is {-1,0,1}
	return add_exflow_16(a, b, result) + add_exflow_16(*result, carry_in, result);
}

/**
 * @brief Adds two signed 8-bit integers with an incoming carry or borrow.
 * @param a The first operand.
 * @param b The second operand.
 * @param carry_in The incoming carry or borrow (-1, 0, or 1).
 * @param result Pointer to a signed 8-bit integer where the sum will be stored.
 * @return The carry-out (-1, 0, or 1).
 */
DESCENT_BUILTIN_API int
add_carry_8(int8_t a, int8_t b, int carry_in, int8_t *result) {
	carry_in = sign_8(carry_in);
	// Overflow and underflow can only occur once if the third operand is {-1,0,1}
	return add_exflow_8(a, b, result) + add_exflow_8(*result, carry_in, result);
}

// Saturating Addition

/**
 * @brief Adds two unsigned 64-bit integers with saturation, clamping between 0 and UINT64_MAX.
 * @param a The first operand.
 * @param b The second operand.
 * @return The saturated sum of a and b.
 */
DESCENT_BUILTIN_API uint64_t
uadd_saturating_64(uint64_t a, uint64_t b) {
	uint64_t result = 0;
	int overflow = uadd_overflow_64(a, b, &result);
	return overflow ? UINT64_MAX : result;
}

/**
 * @brief Adds two unsigned 32-bit integers with saturation, clamping between 0 and UINT32_MAX.
 * @param a The first operand.
 * @param b The second operand.
 * @return The saturated sum of a and b.
 */
DESCENT_BUILTIN_API uint32_t
uadd_saturating_32(uint32_t a, uint32_t b) {
	uint32_t result = 0;
	int overflow = uadd_overflow_32(a, b, &result);
	return overflow ? UINT32_MAX : result;
}

/**
 * @brief Adds two unsigned 16-bit integers with saturation, clamping between 0 and UINT16_MAX.
 * @param a The first operand.
 * @param b The second operand.
 * @return The saturated sum of a and b.
 */
DESCENT_BUILTIN_API uint16_t
uadd_saturating_16(uint16_t a, uint16_t b) {
	uint16_t result = 0;
	int overflow = uadd_overflow_16(a, b, &result);
	return overflow ? UINT16_MAX : result;
}

/**
 * @brief Adds two unsigned 8-bit integers with saturation, clamping between 0 and UINT8_MAX.
 * @param a The first operand.
 * @param b The second operand.
 * @return The saturated sum of a and b.
 */
DESCENT_BUILTIN_API uint8_t
uadd_saturating_8(uint8_t a, uint8_t b) {
	uint8_t result = 0;
	int overflow = uadd_overflow_8(a, b, &result);
	return overflow ? UINT8_MAX : result;
}

/**
 * @brief Adds two signed 64-bit integers with saturation, clamping between INT64_MIN and INT64_MAX.
 * @param a The first operand.
 * @param b The second operand.
 * @return The saturated sum of a and b.
 */
DESCENT_BUILTIN_API int64_t
add_saturating_64(int64_t a, int64_t b) {
	int64_t result = 0;
	if (add_overflow_64(a, b, &result)) {
		return (a > 0) ? INT64_MAX : INT64_MIN;
	}
	return result;
}

/**
 * @brief Adds two signed 32-bit integers with saturation, clamping between INT32_MIN and INT32_MAX.
 * @param a The first operand.
 * @param b The second operand.
 * @return The saturated sum of a and b.
 */
DESCENT_BUILTIN_API int32_t
add_saturating_32(int32_t a, int32_t b) {
	int32_t result = 0;
	if (add_overflow_32(a, b, &result)) {
		return (a > 0) ? INT32_MAX : INT32_MIN;
	}
	return result;
}

/**
 * @brief Adds two signed 16-bit integers with saturation, clamping between INT16_MIN and INT16_MAX.
 * @param a The first operand.
 * @param b The second operand.
 * @return The saturated sum of a and b.
 */
DESCENT_BUILTIN_API int16_t
add_saturating_16(int16_t a, int16_t b) {
	int16_t result = 0;
	if (add_overflow_16(a, b, &result)) {
		return (a > 0) ? INT16_MAX : INT16_MIN;
	}
	return result;
}

/**
 * @brief Adds two signed 8-bit integers with saturation, clamping between INT8_MIN and INT8_MAX.
 * @param a The first operand.
 * @param b The second operand.
 * @return The saturated sum of a and b.
 */
DESCENT_BUILTIN_API int8_t
add_saturating_8(int8_t a, int8_t b) {
	int8_t result = 0;
	if (add_overflow_8(a, b, &result)) {
		return (a > 0) ? INT8_MAX : INT8_MIN;
	}
	return result;
}

// Signed Subtraction with Exceeded Flow

/**
 * @brief Subtracts two signed 64-bit integers with overflow and underflow detection.
 * @param a The first operand.
 * @param b The second operand.
 * @param result Pointer to a signed 64-bit integer where the sum will be stored.
 * @return 0 if no overflow occurs, 1 if overflow occurs, -1 if underflow occurs.
 */

DESCENT_BUILTIN_API int
sub_exflow_64(int64_t a, int64_t b, int64_t *result) {
#if DESCENT_HAS_BUILTIN(__builtin_ssubll_overflow)
	return __builtin_ssubll_overflow(a, b, (long long *)result) ? -sign_64(b) : 0;
#else
	*result = (int64_t) ((uint64_t) a - (uint64_t) b);
	return (b < 0 && a > INT64_MAX + b) - (b > 0 && a < INT64_MIN + b);
#endif
}

DESCENT_BUILTIN_API int
sub_exflow_32(int32_t a, int32_t b, int32_t *result) {
	*result = (int32_t) ((uint32_t) a - (uint32_t) b);
	return (b < 0 && a > INT32_MAX + b) - (b > 0 && a < INT32_MIN + b);
}

DESCENT_BUILTIN_API int
sub_exflow_16(int16_t a, int16_t b, int16_t *result) {
	*result = (int16_t) ((uint16_t) a - (uint16_t) b);
	return (b < 0 && a > INT16_MAX + b) - (b > 0 && a < INT16_MIN + b);
}

DESCENT_BUILTIN_API int
sub_exflow_8(int8_t a, int8_t b, int8_t *result) {
	*result = (int8_t) ((uint8_t) a - (uint8_t) b);
	return (b < 0 && a > INT8_MAX + b) - (b > 0 && a < INT8_MIN + b);
}

// Subtraction with Overflow

DESCENT_BUILTIN_API int
usub_overflow_64(uint64_t a, uint64_t b, uint64_t *result) {
	*result = a - b;
	return a < b;
}

DESCENT_BUILTIN_API int
usub_overflow_32(uint32_t a, uint32_t b, uint32_t *result) {
	*result = a - b;
	return a < b;
}

DESCENT_BUILTIN_API int
usub_overflow_16(uint16_t a, uint16_t b, uint16_t *result) {
	*result = a - b;
	return a < b;
}

DESCENT_BUILTIN_API int
usub_overflow_8(uint8_t a, uint8_t b, uint8_t *result) {
	*result = a - b;
	return a < b;
}

// Saturating Subtraction

DESCENT_BUILTIN_API int
usub_saturating_64(uint64_t a, uint64_t b, uint64_t *result) {
	*result = a - b;
	return a < b;
}

// TODO

// Partial 128-Bit Multiplication

/**
 * @brief Multiplies two unsigned 64-bit integers into the upper and lower halves of a 128 bit unsigned integer.
 * @param a The first multiplicand.
 * @param b The second multiplicand.
 * @param high Pointer to an unsigned 64-bit integer where the high half of the result will be stored.
 * @param low Pointer to an unsigned 64-bit integer where the low half of the result will be stored.
 */
DESCENT_BUILTIN_API void
umul_partial_128(const uint64_t a, const uint64_t b, uint64_t *high, uint64_t *low) {
#if defined(DESCENT_PLATFORM_COMPILER_MSVC) && (DESCENT_PLATFORM_ARCHITECTURE_SIZE == 64) && (_MSC_VER >= 1400)
	*low = _umul128(a, b, high);
#elif __SIZEOF_INT128__
	__uint128_t result = (__uint128_t) a * (__uint128_t) b;
	*high = (uint64_t) (result >> 64);
	*low = (uint64_t) result;
#else
	DESCENT_BUILTIN_CONST uint64_t MASK = 0xFFFFFFFFull;

	uint64_t a_low = a & MASK;
	uint64_t b_low = b & MASK;
	uint64_t a_high = (a >> 32) & MASK;
	uint64_t b_high = (b >> 32) & MASK;

	uint64_t product_0 = a_low * b_low;
	uint64_t product_1 = a_low * b_high;
	uint64_t product_2 = a_high * b_low;
	uint64_t product_3 = a_high * b_high;

	uint64_t temp = (product_0 >> 32) + (product_1 & MASK) + (product_2 & MASK);
	uint64_t carry = temp >> 32;

	*low = (product_0 & MASK) | (temp << 32);
	*high = product_3 + (product_2 >> 32) + (product_1 >> 32) + carry;
#endif
}

/**
 * @brief Multiplies two signed 64-bit integers into the upper and lower halves of a 128 bit signed integer.
 * @param a The first multiplicand.
 * @param b The second multiplicand.
 * @param high Pointer to a signed 64-bit integer where the high half of the result will be stored.
 * @param low Pointer to a signed 64-bit integer where the low half of the result will be stored.
 */
DESCENT_BUILTIN_API void
mul_partial_128(const int64_t a, const int64_t b, int64_t *high, int64_t *low) {
#if defined(DESCENT_PLATFORM_COMPILER_MSVC) && (DESCENT_PLATFORM_ARCHITECTURE_SIZE == 64) && (_MSC_VER >= 1400)
	*low = _mul128(a, b, high);
#elif __SIZEOF_INT128__
	__int128_t result = (__int128_t) a * (__int128_t) b;
	*high = (int64_t) (result >> 64);
	*low = (int64_t) result;
#else
	int negative = (a < 0) ^ (b < 0);

	uint64_t abs_a = a < 0 ? -(uint64_t)a : (uint64_t)a;
	uint64_t abs_b = b < 0 ? -(uint64_t)b : (uint64_t)b;

	uint64_t abs_high = 0;
	uint64_t abs_low = 0;

	umul_partial_128(abs_a, abs_b, &abs_high, &abs_low);

	if (negative) {
		abs_low = ~abs_low + 1;
		abs_high = ~abs_high + (abs_low == 0 ? 1 : 0);
	}

	*low = (int64_t) abs_low;
	*high = (int64_t) abs_high;
#endif
}

// Multiplication with Overflow

DESCENT_BUILTIN_API int
umul_overflow_64(uint64_t a, uint64_t b, uint64_t *result) {
	uint64_t high = 0;
	umul_partial_128(a, b, &high, result);
	return !!high;
}

DESCENT_BUILTIN_API int
umul_overflow_32(uint32_t a, uint32_t b, uint32_t *result) {
	uint64_t product = (uint64_t) a * (uint64_t) b;
	*result = (uint32_t) product;
	return product > UINT32_MAX;
}

DESCENT_BUILTIN_API int
umul_overflow_16(uint16_t a, uint16_t b, uint16_t *result) {
	uint32_t product = (uint32_t) a * (uint32_t) b;
	*result = (uint16_t) product;
	return product > UINT16_MAX;
}

DESCENT_BUILTIN_API int
umul_overflow_8(uint8_t a, uint8_t b, uint8_t *result) {
	uint16_t product = (uint16_t) a * (uint16_t) b;
	*result = (uint8_t) product;
	return product > UINT8_MAX;
}

// TODO: Signed Multiplication with Overflow

// Saturating Multiplication

// TODO

// Next Largest Power of 2

// Round to the next highest power of two
DESCENT_BUILTIN_API uint64_t
pow2ceil_64(uint64_t v) {
	// Increment twice if v == 0 so that the next power is 1, not 2. Overflow will return 0.
	uint64_t increment = (v == 0) + 1;
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v |= v >> 32;
	return v + increment;
}

// Round to the next highest power of two
DESCENT_BUILTIN_API uint32_t
pow2ceil_32(uint32_t v) {
	// Increment twice if v == 0 so that the next power is 1, not 2. Overflow will return 0.
  uint32_t increment = (v == 0) + 1;
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	return v + increment;
}

// Round to the next highest power of two
DESCENT_BUILTIN_API uint16_t
pow2ceil_16(uint16_t v) {
	// Increment twice if v == 0 so that the next power is 1, not 2. Overflow will return 0.
  uint16_t increment = (v == 0) + 1;
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	return v + increment;
}

// Round to the next highest power of two
DESCENT_BUILTIN_API uint8_t
pow2ceil_8(uint8_t v) {
	// Increment twice if v == 0 so that the next power is 1, not 2. Overflow will return 0.
  uint8_t increment = (v == 0) + 1; 
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	return v + increment;
}

#ifdef __cplusplus
}
#endif

#endif
