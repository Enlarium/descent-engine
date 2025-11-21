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

#include <descent/thread/condition.h>

#include <stddef.h>

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <pthread.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <windows.h>
#endif

#include <descent/utilities/opaque.h>

#include "implementation.h"

_Static_assert(DESCENT_OPAQUE_SIZE_CONDITION >= sizeof(ConditionImplementation), "Condition opaque type is too small for its implementation!");
_Static_assert(_Alignof(Condition) >= _Alignof(ConditionImplementation), "Condition opaque type is under-aligned for its implementation!");

int condition_init(Condition *oc) {
	ConditionImplementation *c = (ConditionImplementation *)oc;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_cond_init(&c->_condition, NULL);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	InitializeConditionVariable(&c->_condition);
	return 0;
#endif
}

int condition_destroy(Condition *oc) {
	ConditionImplementation *c = (ConditionImplementation *)oc;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_cond_destroy(&c->_condition);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	(void)c;
	return 0;
#endif
}

int condition_signal(Condition *oc) {
	ConditionImplementation *c = (ConditionImplementation *)oc;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_cond_signal(&c->_condition);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	WakeConditionVariable(&c->_condition);
	return 0;
#endif
}

int condition_broadcast(Condition *oc) {
	ConditionImplementation *c = (ConditionImplementation *)oc;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_cond_broadcast(&c->_condition);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	WakeAllConditionVariable(&c->_condition);
	return 0;
#endif
}
