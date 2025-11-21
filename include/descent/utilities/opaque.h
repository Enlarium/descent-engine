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

#ifndef DESCENT_UTILITIES_OPAQUE_H
#define DESCENT_UTILITIES_OPAQUE_H

#include <stdint.h>

#include "platform.h"

#if DESCENT_PLATFORM_ARCHITECTURE_SIZE == 32
#define DESCENT_OPAQUE_TYPE uint32_t
#elif DESCENT_PLATFORM_ARCHITECTURE_SIZE == 64
#define DESCENT_OPAQUE_TYPE uint64_t
#endif

/**
 * @brief Define an opaque type.
 * 
 * Type will be aligned at least to the platform's native word size.
 * 
 * @param name Name of the opaque type.
 * @param size Size of the opaque type in bytes.
 */
#define DESCENT_OPAQUE_DEFINE(name, size) typedef struct { DESCENT_OPAQUE_TYPE _data[(size + sizeof(DESCENT_OPAQUE_TYPE) - 1) / sizeof(DESCENT_OPAQUE_TYPE)]; } name;

// List of opaque types and sizes on different platforms.
// Sizes must be checked where implemented.

#if defined(DESCENT_PLATFORM_ABI_GLIBC_32)
#define DESCENT_OPAQUE_SIZE_BARRIER         20
#define DESCENT_OPAQUE_SIZE_CALL_ONCE       4
#define DESCENT_OPAQUE_SIZE_CONDITION       12
#define DESCENT_OPAQUE_SIZE_MUTEX           24
#define DESCENT_OPAQUE_SIZE_RECURSIVE_MUTEX 24
#define DESCENT_OPAQUE_SIZE_RWLOCK          48
#define DESCENT_OPAQUE_SIZE_SEMAPHORE       32
#define DESCENT_OPAQUE_SIZE_THREAD          4
#elif defined(DESCENT_PLATFORM_ABI_GLIBC_64)
#define DESCENT_OPAQUE_SIZE_BARRIER         32
#define DESCENT_OPAQUE_SIZE_CALL_ONCE       4
#define DESCENT_OPAQUE_SIZE_CONDITION       48
#define DESCENT_OPAQUE_SIZE_MUTEX           40
#define DESCENT_OPAQUE_SIZE_RECURSIVE_MUTEX 40
#define DESCENT_OPAQUE_SIZE_RWLOCK          56
#define DESCENT_OPAQUE_SIZE_SEMAPHORE       32
#define DESCENT_OPAQUE_SIZE_THREAD          8
#elif defined(DESCENT_PLATFORM_ABI_MUSL_32)
#define DESCENT_OPAQUE_SIZE_BARRIER         20
#define DESCENT_OPAQUE_SIZE_CALL_ONCE       4
#define DESCENT_OPAQUE_SIZE_CONDITION       8
#define DESCENT_OPAQUE_SIZE_MUTEX           12
#define DESCENT_OPAQUE_SIZE_RECURSIVE_MUTEX 12
#define DESCENT_OPAQUE_SIZE_RWLOCK          16
#define DESCENT_OPAQUE_SIZE_SEMAPHORE       4
#define DESCENT_OPAQUE_SIZE_THREAD          4
#elif defined(DESCENT_PLATFORM_ABI_MUSL_64)
#define DESCENT_OPAQUE_SIZE_BARRIER         20
#define DESCENT_OPAQUE_SIZE_CALL_ONCE       8
#define DESCENT_OPAQUE_SIZE_CONDITION       32
#define DESCENT_OPAQUE_SIZE_MUTEX           24
#define DESCENT_OPAQUE_SIZE_RECURSIVE_MUTEX 24
#define DESCENT_OPAQUE_SIZE_RWLOCK          32
#define DESCENT_OPAQUE_SIZE_SEMAPHORE       8
#define DESCENT_OPAQUE_SIZE_THREAD          8
#elif defined(DESCENT_PLATFORM_ABI_FREEBSD_32)
#define DESCENT_OPAQUE_SIZE_BARRIER         20
#define DESCENT_OPAQUE_SIZE_CALL_ONCE       4
#define DESCENT_OPAQUE_SIZE_CONDITION       8
#define DESCENT_OPAQUE_SIZE_MUTEX           8
#define DESCENT_OPAQUE_SIZE_RECURSIVE_MUTEX 8
#define DESCENT_OPAQUE_SIZE_RWLOCK          16
#define DESCENT_OPAQUE_SIZE_SEMAPHORE       4
#define DESCENT_OPAQUE_SIZE_THREAD          4
#elif defined(DESCENT_PLATFORM_ABI_FREEBSD_64)
#define DESCENT_OPAQUE_SIZE_BARRIER         20
#define DESCENT_OPAQUE_SIZE_CALL_ONCE       8
#define DESCENT_OPAQUE_SIZE_CONDITION       8
#define DESCENT_OPAQUE_SIZE_MUTEX           8
#define DESCENT_OPAQUE_SIZE_RECURSIVE_MUTEX 8
#define DESCENT_OPAQUE_SIZE_RWLOCK          16
#define DESCENT_OPAQUE_SIZE_SEMAPHORE       8
#define DESCENT_OPAQUE_SIZE_THREAD          8
#elif defined(DESCENT_PLATFORM_ABI_MACOS_32)
#define DESCENT_OPAQUE_SIZE_BARRIER         24
#define DESCENT_OPAQUE_SIZE_CALL_ONCE       4
#define DESCENT_OPAQUE_SIZE_CONDITION       40
#define DESCENT_OPAQUE_SIZE_MUTEX           56
#define DESCENT_OPAQUE_SIZE_RECURSIVE_MUTEX 56
#define DESCENT_OPAQUE_SIZE_RWLOCK          184
#define DESCENT_OPAQUE_SIZE_SEMAPHORE       4
#define DESCENT_OPAQUE_SIZE_THREAD          4
#elif defined(DESCENT_PLATFORM_ABI_MACOS_64)
#define DESCENT_OPAQUE_SIZE_BARRIER         24
#define DESCENT_OPAQUE_SIZE_CALL_ONCE       8
#define DESCENT_OPAQUE_SIZE_CONDITION       40
#define DESCENT_OPAQUE_SIZE_MUTEX           64
#define DESCENT_OPAQUE_SIZE_RECURSIVE_MUTEX 64
#define DESCENT_OPAQUE_SIZE_RWLOCK          200
#define DESCENT_OPAQUE_SIZE_SEMAPHORE       8
#define DESCENT_OPAQUE_SIZE_THREAD          8
#elif defined(DESCENT_PLATFORM_ABI_CYGWIN_32)
#define DESCENT_OPAQUE_SIZE_BARRIER         20
#define DESCENT_OPAQUE_SIZE_CALL_ONCE       4
#define DESCENT_OPAQUE_SIZE_CONDITION       12
#define DESCENT_OPAQUE_SIZE_MUTEX           24
#define DESCENT_OPAQUE_SIZE_RECURSIVE_MUTEX 24
#define DESCENT_OPAQUE_SIZE_RWLOCK          48
#define DESCENT_OPAQUE_SIZE_SEMAPHORE       32
#define DESCENT_OPAQUE_SIZE_THREAD          4
#elif defined(DESCENT_PLATFORM_ABI_CYGWIN_64)
#define DESCENT_OPAQUE_SIZE_BARRIER         20
#define DESCENT_OPAQUE_SIZE_CALL_ONCE       8
#define DESCENT_OPAQUE_SIZE_CONDITION       48
#define DESCENT_OPAQUE_SIZE_MUTEX           40
#define DESCENT_OPAQUE_SIZE_RECURSIVE_MUTEX 40
#define DESCENT_OPAQUE_SIZE_RWLOCK          56
#define DESCENT_OPAQUE_SIZE_SEMAPHORE       32
#define DESCENT_OPAQUE_SIZE_THREAD          8
#elif defined(DESCENT_PLATFORM_ABI_WINDOWS_32)
#define DESCENT_OPAQUE_SIZE_BARRIER         20
#define DESCENT_OPAQUE_SIZE_CALL_ONCE       4
#define DESCENT_OPAQUE_SIZE_CONDITION       4
#define DESCENT_OPAQUE_SIZE_MUTEX           24
#define DESCENT_OPAQUE_SIZE_RECURSIVE_MUTEX 24
#define DESCENT_OPAQUE_SIZE_RWLOCK          4
#define DESCENT_OPAQUE_SIZE_SEMAPHORE       4
#define DESCENT_OPAQUE_SIZE_THREAD          4
#elif defined(DESCENT_PLATFORM_ABI_WINDOWS_64)
#define DESCENT_OPAQUE_SIZE_BARRIER         24
#define DESCENT_OPAQUE_SIZE_CALL_ONCE       8
#define DESCENT_OPAQUE_SIZE_CONDITION       8
#define DESCENT_OPAQUE_SIZE_MUTEX           40
#define DESCENT_OPAQUE_SIZE_RECURSIVE_MUTEX 40
#define DESCENT_OPAQUE_SIZE_RWLOCK          8
#define DESCENT_OPAQUE_SIZE_SEMAPHORE       8
#define DESCENT_OPAQUE_SIZE_THREAD          8
#endif

#endif