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

#ifndef DESCENT_THREAD_H
#define DESCENT_THREAD_H

/**
 * @defgroup thread Thread
 *
 * @brief Threading and synchronization primitives.
 *
 * This module provides low-level threading and synchronization facilities,
 * including thread management, mutual exclusion, condition variables,
 * semaphores, and queue-based locks.
 *
 * All mechanisms in this module are intra-process only and are not safe for
 * use across process boundaries.
 */

#include <descent/thread/atomic.h>
#include <descent/thread/call_once.h>
// #include <descent/thread/barrier.h> // TODO: Unimplemented
#include <descent/thread/condition.h>
#include <descent/thread/futex.h>
#include <descent/thread/mutex.h>
#include <descent/thread/qutex.h>
// #include <descent/thread/rwlock.h> // TODO: Unimplemented
#include <descent/thread/semaphore.h>
#include <descent/thread/thread.h>
#include <descent/thread/tls.h>

#endif