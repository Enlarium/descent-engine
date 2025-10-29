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

#ifndef DESCENT_SOURCE_THREAD_IMPLEMENTATION_H
#define DESCENT_SOURCE_THREAD_IMPLEMENTATION_H

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <pthread.h>
#include <semaphore.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <windows.h>
#endif

#include <descent/utilities/opaque.h>

typedef struct {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	pthread_barrier_t _barrier;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	SYNCHRONIZATION_BARRIER _barrier;
#endif
} BarrierImplementation;

typedef struct {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	pthread_once_t _once;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	INIT_ONCE _once;
#endif
} CallOnceImplementation;

typedef struct {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	pthread_cond_t _condition;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	CONDITION_VARIABLE _condition;
#endif
} ConditionImplementation;

typedef struct {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	pthread_mutex_t _mutex;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	SRWLOCK _mutex;
#endif
} MutexImplementation;

typedef struct {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	pthread_mutex_t  _mutex;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	CRITICAL_SECTION _mutex;
#endif
} RecursiveMutexImplementation;

typedef struct {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	pthread_rwlock_t _lock;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	SRWLOCK _lock;
#endif
} RWLockImplementation;

typedef struct {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	sem_t _semaphore;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	HANDLE _semaphore;
#endif
} SemaphoreImplementation;

typedef struct {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	pthread_t _thread;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	HANDLE _thread;
#endif
} ThreadImplementation;

#endif
