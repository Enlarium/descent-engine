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

#ifndef DESCENT_UTILITIES_DEBUG_H
#define DESCENT_UTILITIES_DEBUG_H

// DESCENT_DEBUG and DESCENT_NO_DEBUG_* are PUBLIC
// DESCENT_DEBUG_* are PRIVATE

#if defined(DESCENT_DEBUG)
#if (DESCENT_DEBUG != 0) && (DESCENT_DEBUG != 1)
#undef DESCENT_DEBUG
#define DESCENT_DEBUG 1
#endif
#ifdef NDEBUG
#undef NDEBUG
#endif
#elif defined(NDEBUG)
#define DESCENT_DEBUG 0
#else
#define DESCENT_DEBUG 1
#endif 

int debug_log_open(const char* path);

void debug_log_close(void);

void debug_log(const char* context, const char* fmt, ...);

#if defined(DESCENT_NO_DEBUG_THREAD) || (DESCENT_DEBUG == 0)
#define DESCENT_DEBUG_THREAD 0
#define debug_thread_log(context, ...) ((void)0)
#else
#define DESCENT_DEBUG_THREAD 1
#define debug_thread_log(context, ...) debug_log(context, __VA_ARGS__)
#endif

#endif