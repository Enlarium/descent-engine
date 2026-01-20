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

#ifndef DESCENT_UTILITIES_BUILTIN_H
#define DESCENT_UTILITIES_BUILTIN_H

#define builtin_expect             __builtin_expect
#define builtin_expect_probability __builtin_expect_with_probability
#define builtin_trap               __builtin_trap
#define builtin_unreachable        __builtin_unreachable
#define builtin_clear_cache        __builtin___clear_cache
#define builtin_prefetch           __builtin_prefetch

#endif