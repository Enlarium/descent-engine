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
