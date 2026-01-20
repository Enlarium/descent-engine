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

#ifndef DESCENT_UTILITIES_PLATFORM_H
#define DESCENT_UTILITIES_PLATFORM_H

// Platform

#if   defined(__linux__) && !defined(__ANDROID__)
#define DESCENT_PLATFORM_LINUX
#define DESCENT_PLATFORM_TYPE_POSIX
#elif defined(__FreeBSD__)
#define DESCENT_PLATFORM_FREEBSD
#define DESCENT_PLATFORM_TYPE_POSIX
#elif defined(_WIN64)
#if   defined(_M_ARM64)
#define DESCENT_PLATFORM_WINDOWS_ARM_64
#else
#define DESCENT_PLATFORM_WINDOWS_64
#endif
#define DESCENT_PLATFORM_TYPE_WINDOWS
#elif defined(_WIN32)
#if defined(_M_ARM)
#define DESCENT_PLATFORM_WINDOWS_ARM_32
#else
#define DESCENT_PLATFORM_WINDOWS_32
#endif
#define DESCENT_PLATFORM_TYPE_WINDOWS
#else
#error "Descent Engine does not support this platform!"
#endif

// Compiler

#if   defined(__CYGWIN__)
#define DESCENT_PLATFORM_COMPILER_CYGWIN_GCC
#define DESCENT_PLATFORM_COMPILER_TYPE_GCC
#define DESCENT_PLATFORM_TYPE_POSIX_PARTIAL
#elif defined(__MINGW32__) || defined(__MINGW64__)
#define DESCENT_PLATFORM_COMPILER_MINGW_GCC
#define DESCENT_PLATFORM_COMPILER_TYPE_GCC
#define DESCENT_PLATFORM_TYPE_POSIX_PARTIAL
#elif defined(__INTEL_COMPILER) && defined(__clang__)
#define DESCENT_PLATFORM_COMPILER_ICX
#define DESCENT_PLATFORM_COMPILER_TYPE_CLANG
#elif defined(__clang__)
#define DESCENT_PLATFORM_COMPILER_CLANG
#define DESCENT_PLATFORM_COMPILER_TYPE_CLANG
#elif defined(__GNUC__)
#define DESCENT_PLATFORM_COMPILER_GCC
#define DESCENT_PLATFORM_COMPILER_TYPE_GCC
#else
#warning "Descent Engine does not support this compiler!"
#endif

// Architecture

#if   defined(_M_IX86) || defined(__i386__)
#define DESCENT_PLATFORM_ARCHITECTURE_SIZE 32
#define DESCENT_PLATFORM_ARCHITECTURE_X86
#define DESCENT_PLATFORM_ARCHITECTURE_FAMILY_X86
#elif defined(_M_X64) || defined(__x86_64__)
#define DESCENT_PLATFORM_ARCHITECTURE_SIZE 64
#define DESCENT_PLATFORM_ARCHITECTURE_X86_64
#define DESCENT_PLATFORM_ARCHITECTURE_FAMILY_X86
#elif defined(__arm__) || defined(_M_ARM)
#define DESCENT_PLATFORM_ARCHITECTURE_SIZE 32
#define DESCENT_PLATFORM_ARCHITECTURE_ARM_32
#define DESCENT_PLATFORM_ARCHITECTURE_FAMILY_ARM
#elif defined(__aarch64__) || defined(_M_ARM64)
#define DESCENT_PLATFORM_ARCHITECTURE_SIZE 64
#define DESCENT_PLATFORM_ARCHITECTURE_ARM_64
#define DESCENT_PLATFORM_ARCHITECTURE_FAMILY_ARM
#else
#error "Descent Engine does not support this architecture!"
#endif

// Language

#if defined(__STDC_VERSION__) && __STDC_VERSION__ < 201112L
#error "Descent Engine requires C11 or newer!"
#endif

#if defined(__cplusplus) && __cplusplus < 201103L
#error "Descent Engine requires C++11 or newer!"
#endif

// Negative Number Representation

#if (-1 & 3) != 3
#error "Descent Engine requires two's complement for negative numbers!"
#endif

// Endianness

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define DESCENT_PLATFORM_ENDIAN_BIG
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define DESCENT_PLATFORM_ENDIAN_LITTLE
#elif defined(_WIN32)
#define DESCENT_PLATFORM_ENDIAN_LITTLE
#else
#error "This platform does not define a byte ordering!"
#endif

#endif
