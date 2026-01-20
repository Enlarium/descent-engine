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

#ifndef DESCENT_THREAD_ATOMIC_TYPES_H
#define DESCENT_THREAD_ATOMIC_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @defgroup atomic Atomic
 * @ingroup thread
 *
 * @brief Low-level atomic types and operations.
 *
 * This header provides lightweight atomic wrapper types.
 *
 * These atomics provide a lightweight interface and do not implement the full
 * C11 `<stdatomic.h>` API.
 *
 * @note All atomic objects must be naturally aligned.
 * @note Atomic operations may or may not be lock-free depending on the
 * platform and type. Use @ref atomic_is_lock_free() to determine if a type is
 * lock free.
 */

/**
 * @def ATOMIC_INIT
 * @ingroup atomic
 * @brief Static initializer for atomic objects.
 * @param value Initial value of the atomic object.
 */
#define ATOMIC_INIT(value) { ._atomic = value }

/**
 * @struct atomic_int
 * @ingroup atomic
 * @brief Atomic integer type.
 */
typedef struct {
	int _atomic;
} atomic_int;

/**
 * @struct atomic_32
 * @ingroup atomic
 * @brief Atomic 32-bit unsigned integer type.
 */
typedef struct {
	uint32_t _atomic;
} atomic_32;

/**
 * @struct atomic_64
 * @ingroup atomic
 * @brief Atomic 64-bit unsigned integer type.
 */
typedef struct {
	uint64_t _atomic;
} atomic_64;

/**
 * @struct atomic_ptr
 * @ingroup atomic
 * @brief Atomic pointer type.
 */
typedef struct {
	uintptr_t _atomic;
} atomic_ptr;

/**
 * @struct atomic_bool
 * @ingroup atomic
 * @brief Atomic boolean type.
 */
typedef struct {
	bool _atomic;
} atomic_bool;

#endif
