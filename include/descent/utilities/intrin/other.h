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

#ifndef DESCENT_UTILITIES_BUILTIN_OTHER_H
#define DESCENT_UTILITIES_BUILTIN_OTHER_H

#include <descent/utilities/builtin/api.h>

#ifndef unreachable
#if DESCENT_HAS_BUILTIN(__builtin_unreachable)
#define unreachable() __builtin_unreachable()
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC)
#define unreachable() __assume(0)
#else
#define unreachable() (void) 0
#endif
#endif

#endif
