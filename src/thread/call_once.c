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

#include <descent/thread/call_once.h>

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <pthread.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <windows.h>
#endif

#include "implementation.h"

_Static_assert(DESCENT_OPAQUE_SIZE_CALL_ONCE >= sizeof(CallOnceImplementation), "Call-once opaque type is too small for its implementation!");
_Static_assert(_Alignof(CallOnce) >= _Alignof(CallOnceImplementation), "Call-once opaque type is under-aligned for its implementation!");

#if defined(DESCENT_PLATFORM_TYPE_WINDOWS)
BOOL CALLBACK call_once_wrapper(PINIT_ONCE InitOnce, PVOID Parameter,PVOID *Context) {
	CallOnceFunction function = (CallOnceFunction) Parameter;
  function();
  return TRUE;
}
#endif

void call_once_init(CallOnce *oc) {
	CallOnceImplementation *c = (CallOnceImplementation *)oc;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	c->_once = PTHREAD_ONCE_INIT;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	c->_once = INIT_ONCE_STATIC_INIT;
#endif
}

void call_once(CallOnce *oc, CallOnceFunction f) {
	CallOnceImplementation *c = (CallOnceImplementation *)oc;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	pthread_once(&c->_once, f);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	InitOnceExecuteOnce(&c->_once, call_once_wrapper, (PVOID)f, NULL);
#endif
}
