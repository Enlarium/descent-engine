#include <descent/thread/condition.h>

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <pthread.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <windows.h>
#endif

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
