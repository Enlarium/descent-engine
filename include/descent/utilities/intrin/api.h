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

#ifndef DESCENT_UTILITIES_BUILTIN_API
#define DESCENT_UTILITIES_BUILTIN_API

#include <descent/utilities/platform.h>

#ifdef __cplusplus
#define DESCENT_BUILTIN_API static constexpr inline
#define DESCENT_BUILTIN_CONST constexpr
#define DESCENT_BUILTIN_FALLBACK descent::builtin::fallback::
#else
#define DESCENT_BUILTIN_API static inline
#define DESCENT_BUILTIN_CONST static const
#define DESCENT_BUILTIN_FALLBACK
#endif

#ifdef __has_builtin
#define DESCENT_HAS_BUILTIN(x) __has_builtin(x)
#else
#define DESCENT_HAS_BUILTIN(x) 0
#endif

#ifdef __has_attribute
#define DESCENT_HAS_ATTRIBUTE(x) __has_attribute(x)
#else
#define DESCENT_HAS_ATTRIBUTE(x) 0
#endif

#ifdef __has_feature
#define DESCENT_HAS_FEATURE(x) __has_feature(x)
#else
#define DESCENT_HAS_FEATURE(x) 0
#endif

#endif
