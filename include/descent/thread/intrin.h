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

#ifndef DESCENT_THREAD_INTRIN_H
#define DESCENT_THREAD_INTRIN_H

/**
 * @brief Provide a CPU pause hint. Does not yield execution.
 */
static inline void thread_pause(void) {
#if defined(DESCENT_PLATFORM_COMPILER_TYPE_GCC) || defined(DESCENT_PLATFORM_COMPILER_TYPE_CLANG)
#if defined(DESCENT_PLATFORM_ARCHITECTURE_FAMILY_X86)
	__builtin_ia32_pause();
#elif defined(DESCENT_PLATFORM_ARCHITECTURE_FAMILY_ARM)
	__asm__ __volatile__("yield" ::: "memory");
#endif
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC)
#if defined(DESCENT_PLATFORM_ARCHITECTURE_FAMILY_X86)
	_mm_pause();
#elif defined(DESCENT_PLATFORM_ARCHITECTURE_FAMILY_ARM)
	__yield();
#endif
#else 
#error "what"
#endif
}

#endif
