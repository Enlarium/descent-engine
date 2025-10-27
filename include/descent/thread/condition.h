#ifndef DESCENT_THREAD_CONDITION_H
#define DESCENT_THREAD_CONDITION_H

#include "../utilities/platform.h"
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <pthread.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <windows.h>
#endif



#ifdef __cplusplus
extern "C" {
#endif



#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#define CONDITION_INITIALIZER { PTHREAD_COND_INITIALIZER }
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#define CONDITION_INITIALIZER { CONDITION_VARIABLE_INIT }
#endif



typedef struct {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	pthread_cond_t _condition;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	CONDITION_VARIABLE _condition;
#endif
} Condition;



static inline int condition_init(Condition *c) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_cond_init(&c->_condition, NULL);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	InitializeConditionVariable(&c->_condition);
	return 0;
#endif
}

static inline int condition_destroy(Condition *c) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_cond_destroy(&c->_condition);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	(void)c;
	return 0;
#endif
}

static inline int condition_signal(Condition *c) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_cond_signal(&c->_condition);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	WakeConditionVariable(&c->_condition);
	return 0;
#endif
}

static inline int condition_broadcast(Condition *c) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return !!pthread_cond_broadcast(&c->_condition);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	WakeAllConditionVariable(&c->_condition);
	return 0;
#endif
}



#ifdef __cplusplus
}
#endif



#endif
