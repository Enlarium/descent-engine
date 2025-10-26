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

#ifndef DESCENT_UTILITIES_PLATFORM_H
#define DESCENT_UTILITIES_PLATFORM_H

// Project Headers
#include "macros.h"
// Platform-Specific Headers
#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

// Platform

#if   defined(__linux__) && !defined(__ANDROID__)
#define DESCENT_PLATFORM_LINUX
#define DESCENT_PLATFORM_TYPE_POSIX
#elif defined(__FreeBSD__)
#define DESCENT_PLATFORM_FREEBSD
#define DESCENT_PLATFORM_TYPE_POSIX
#elif defined(__APPLE__) && TARGET_OS_MAC
#define DESCENT_PLATFORM_MACOS
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
#error "This platform is unsupported!"
#endif

// Compiler

#if   defined(__GNUC__) && !defined(__llvm__) && !defined(__INTEL_COMPILER)
#define DESCENT_PLATFORM_COMPILER_VERSION STRINGIFY(__GNUC__) "." STRINGIFY(__GNUC_MINOR__) "." STRINGIFY(__GNUC_PATCHLEVEL__)
#define DESCENT_PLATFORM_COMPILER_GCC
#define DESCENT_PLATFORM_COMPILER_TYPE_GCC
#elif defined(__clang__) && !defined(__apple_build_version__)
#define DESCENT_PLATFORM_COMPILER_VERSION STRINGIFY(__clang_major__) "." STRINGIFY(__clang_minor__) "." STRINGIFY(__clang_patchlevel__)
#define DESCENT_PLATFORM_COMPILER_CLANG
#define DESCENT_PLATFORM_COMPILER_TYPE_CLANG
#elif defined(__clang__) && defined(__apple_build_version__)
#define DESCENT_PLATFORM_COMPILER_VERSION STRINGIFY(__clang_major__) "." STRINGIFY(__clang_minor__) "." STRINGIFY(__clang_patchlevel__)
#define DESCENT_PLATFORM_COMPILER_APPLE_CLANG
#define DESCENT_PLATFORM_COMPILER_TYPE_CLANG
#elif defined(__INTEL_COMPILER)
#define DESCENT_PLATFORM_COMPILER_VERSION STRINGIFY(__INTEL_COMPILER / 100) "." STRINGIFY(__INTEL_COMPILER % 100)
#define DESCENT_PLATFORM_COMPILER_INTEL
#elif defined(__CYGWIN__)
#define DESCENT_PLATFORM_COMPILER_VERSION STRINGIFY(__GNUC__) "." STRINGIFY(__GNUC_MINOR__) "." STRINGIFY(__GNUC_PATCHLEVEL__)
#define DESCENT_PLATFORM_COMPILER_CYGWIN_GCC
#define DESCENT_PLATFORM_COMPILER_TYPE_GCC
#ifndef DESCENT_PLATFORM_TYPE_POSIX
#define DESCENT_PLATFORM_TYPE_POSIX
#endif
#elif defined(__MINGW32__) || defined(__MINGW64__)
#define DESCENT_PLATFORM_COMPILER_VERSION STRINGIFY(__GNUC__) "." STRINGIFY(__GNUC_MINOR__) "." STRINGIFY(__GNUC_PATCHLEVEL__)
#define DESCENT_PLATFORM_COMPILER_MINGW_GCC
#define DESCENT_PLATFORM_COMPILER_TYPE_GCC
#ifndef DESCENT_PLATFORM_TYPE_POSIX
#define DESCENT_PLATFORM_TYPE_POSIX
#endif
#elif defined(_MSC_VER)
#define DESCENT_PLATFORM_COMPILER_VERSION STRINGIFY(_MSC_FULL_VER / 1000000) "." STRINGIFY((_MSC_FULL_VER / 10000) % 100) "." STRINGIFY(_MSC_FULL_VER % 10000)
#define DESCENT_PLATFORM_COMPILER_MSVC
#define DESCENT_PLATFORM_COMPILER_TYPE_MSVC
#else
#warning "This compiler is unsupported!"
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
#elif defined(__riscv) && (__riscv_xlen == 32)
#define DESCENT_PLATFORM_ARCHITECTURE_SIZE 32
#define DESCENT_PLATFORM_ARCHITECTURE_RISC_V_32
#define DESCENT_PLATFORM_ARCHITECTURE_FAMILY_RISC_V
#elif defined(__riscv) && (__riscv_xlen == 64)
#define DESCENT_PLATFORM_ARCHITECTURE_SIZE 64
#define DESCENT_PLATFORM_ARCHITECTURE_RISC_V_64
#define DESCENT_PLATFORM_ARCHITECTURE_FAMILY_RISC_V
#else
#error "This architecture is unsupported!"
#endif

// Negative Number Representation

#if   (-1 & 3) == 1
#define DESCENT_PLATFORM_NEGATIVE_SIGN_MAGNITUDE
#elif (-1 & 3) == 2
#define DESCENT_PLATFORM_NEGATIVE_ONES_COMPLEMENT
#elif (-1 & 3) == 3
#define DESCENT_PLATFORM_NEGATIVE_TWOS_COMPLEMENT
#else
#error "This platform uses an unsupported representation for negative numbers!"
#endif

// Endianness

#if defined(__BYTE_ORDER__)
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define DESCENT_PLATFORM_ENDIAN_BIG
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define DESCENT_PLATFORM_ENDIAN_LITTLE
#else
#error "This platform does not define a byte ordering!"
#endif
#elif defined(_MSC_VER)
#define DESCENT_PLATFORM_ENDIAN_LITTLE
#else
#error "This platform does not define a byte ordering!"
#endif

#if (defined(DESCENT_PLATFORM_LINUX) || defined(DESCENT_PLATFORM_FREEBSD)) && (!defined(_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 200809L)
#error "Descent Engine requires that _POSIX_C_SOURCE be defined as at least 200809L on Linux and FreeBSD"
#elif defined(DESCENT_PLATFORM_MACOS) && !defined(_DARWIN_C_SOURCE)
#error "Descent Engine requires that _DARWIN_C_SOURCE be defined on macOS"
#endif

#endif
