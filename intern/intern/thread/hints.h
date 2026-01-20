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

#ifndef DESCENT_THREAD_HINTS_H
#define DESCENT_THREAD_HINTS_H

static inline void thread_spin_hint(void) {
#if defined(DESCENT_PLATFORM_ARCHITECTURE_FAMILY_X86)
	__asm__ volatile("pause" ::: "memory");
#elif defined(DESCENT_PLATFORM_ARCHITECTURE_FAMILY_ARM)
	__asm__ volatile("yield" ::: "memory");
#endif
}

#endif