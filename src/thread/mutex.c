#include <descent/thread/mutex.h>

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <pthread.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <windows.h>
#endif

#include <descent/thread/condition.h>

#include "implementation.h"

_Static_assert(DESCENT_OPAQUE_SIZE_MUTEX >= sizeof(MutexImplementation), "Mutex opaque type is too small for its implementation!");
_Static_assert(_Alignof(Mutex) >= _Alignof(MutexImplementation), "Mutex opaque type is under-aligned for its implementation!");

int mutex_init(Mutex *om) {
	MutexImplementation *m = (MutexImplementation *)om;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_init(&m->_mutex, NULL);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	InitializeSRWLock(&m->_mutex);
	return 0;
#endif
}

int mutex_destroy(Mutex *om) {
	MutexImplementation *m = (MutexImplementation *)om;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_destroy(&m->_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	(void)m;
	return 0;
#endif
}

int mutex_lock(Mutex *om) {
	MutexImplementation *m = (MutexImplementation *)om;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_lock(&m->_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	AcquireSRWLockExclusive(&m->_mutex);
	return 0;
#endif
}

int mutex_trylock(Mutex *om) {
	MutexImplementation *m = (MutexImplementation *)om;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_trylock(&m->_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return !TryAcquireSRWLockExclusive(&m->_mutex);
#endif
}

int mutex_unlock(Mutex *om) {
	MutexImplementation *m = (MutexImplementation *)om;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_mutex_unlock(&m->_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	ReleaseSRWLockExclusive(&m->_mutex);
	return 0;
#endif
}

int condition_wait(Condition *oc, Mutex *om) {
	MutexImplementation *m = (MutexImplementation *)om;
	ConditionImplementation *c = (ConditionImplementation *)oc;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_cond_wait(&c->_condition, &m->_mutex);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return !SleepConditionVariableSRW(&c->_condition, &m->_mutex, INFINITE, 0);
#endif
}
