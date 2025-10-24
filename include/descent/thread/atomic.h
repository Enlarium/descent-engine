#ifndef DESCENT_ATOMIC_H
#define DESCENT_ATOMIC_H


#include <stdint.h>

#include "../utilities/platform.h"
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <stdatomic.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <windows.h>
#include <winnt.h>
#endif

#if defined(DESCENT_PLATFORM_WINDOWS_64) || (_WIN32_WINNT >= 0x0600)
#define WINDOWS_INTERLOCKED_64 1
#else
#define WINDOWS_INTERLOCKED_64 0
#endif


#include "mutex.h"

#if UINTPTR_MAX == 0xFFFFFFFFUL
#define UINTPTR_32
#elif UINTPTR_MAX != 0xFFFFFFFFFFFFFFFFULL
#error "Pointers must be either 32 or 64 bits wide"
#endif

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

typedef _Atomic uint32_t  atomic_32;
typedef _Atomic uint64_t  atomic_64;
typedef _Atomic uintptr_t atomic_ptr;

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

typedef __declspec(align(4)) volatile uint32_t  atomic_32;

#if WINDOWS_INTERLOCKED_64
typedef __declspec(align(8)) volatile uint64_t  atomic_64;
#else
typedef __declspec(align(8)) struct {
	mutex_t lock;
	volatile uint64_t value;
} atomic_64;
#endif

typedef __declspec(align(sizeof(uintptr_t))) volatile uintptr_t atomic_ptr;

#endif



static inline int atomic_compare_exchange_32(atomic_32 *object, uint32_t *expected, uint32_t desired) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_compare_exchange_strong(object, expected, desired);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	uint32_t original = (uint32_t) InterlockedCompareExchange((LONG*)object, (LONG)desired, (LONG)*expected);
	if (original == *expected) return 1;
	else {
		*expected = original;
		return 0;
	}
#endif
}

static inline int atomic_compare_exchange_64(atomic_64 *object, uint64_t *expected, uint64_t desired) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_compare_exchange_strong(object, expected, desired);
#elif WINDOWS_INTERLOCKED_64 == 1 || _WIN32_WINNT >= 0x0600
	uint64_t original = (uint64_t) InterlockedCompareExchange64((LONG64*)object, (LONG64)desired, (LONG64)*expected);
	if (original == *expected) return 1;
	else {
		*expected = original;
		return 0;
	}
#elif WINDOWS_INTERLOCKED_64 == 0
	mutex_lock(&object->lock);
	if (object->value == *expected) {
		object->value = desired;
		mutex_unlock(&object->lock);
		return 1;
	} else {
		*expected = object->value;
		mutex_unlock(&object->lock);
		return 0;
	}
#endif
}

static inline int atomic_compare_exchange_ptr(atomic_ptr *object, uintptr_t *expected, uintptr_t desired) {
#ifdef UINTPTR_32
	return atomic_compare_exchange_32((atomic_32 *)object, (uint32_t *)expected, (uint32_t) desired);
#else
	return atomic_compare_exchange_64((atomic_64 *)object, (uint64_t *)expected, (uint64_t) desired);
#endif
}

static inline uint32_t atomic_exchange_32(atomic_32 *object, uint32_t desired) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_exchange(object, desired);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return (uint32_t) InterlockedExchange((LONG*)object, (LONG)desired);
#endif
}

static inline uint64_t atomic_exchange_64(atomic_64 *object, uint64_t desired) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_exchange(object, desired);
#elif WINDOWS_INTERLOCKED_64 == 1
	return (uint64_t) InterlockedExchange64((LONG64*)object, (LONG64)desired);
#elif WINDOWS_INTERLOCKED_64 == 0
	mutex_lock(&object->lock);
	uint64_t old = object->value;
	object->value = desired;
	mutex_unlock(&object->lock);
	return old;
#endif
}

static inline uintptr_t atomic_exchange_ptr(atomic_ptr *object, uintptr_t desired) {
#ifdef UINTPTR_32
	return (uintptr_t) atomic_exchange_32((atomic_32 *)object, (uint32_t) desired);
#else
	return (uintptr_t) atomic_exchange_64((atomic_64 *)object, (uint64_t) desired);
#endif
}

static inline uint32_t atomic_fetch_add_32(atomic_32 *object, uint32_t operand) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_fetch_add(object, operand);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return (uint32_t) InterlockedExchangeAdd((LONG*)object, (LONG)operand);
#endif
}

static inline uint64_t atomic_fetch_add_64(atomic_64 *object, uint64_t operand) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_fetch_add(object, operand);
#elif WINDOWS_INTERLOCKED_64 == 1
	return (uint64_t) InterlockedExchangeAdd64((LONG64*)object, (LONG64)operand);
#elif WINDOWS_INTERLOCKED_64 == 0
	mutex_lock(&object->lock);
	uint64_t old = object->value;
	object->value += operand;
	mutex_unlock(&object->lock);
	return old;
#endif
}

static inline uintptr_t atomic_fetch_add_ptr(atomic_ptr *object, uintptr_t operand) {
#ifdef UINTPTR_32
	return (uintptr_t)  atomic_fetch_add_32((atomic_32 *)object, (uint32_t) operand);
#else
	return (uintptr_t)  atomic_fetch_add_64((atomic_64 *)object, (uint64_t) operand);
#endif
}

static inline uint32_t atomic_fetch_sub_32(atomic_32 *object, uint32_t operand) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_fetch_sub(object, operand);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return (uint32_t) InterlockedExchangeAdd((LONG*)object, -((LONG)operand));
#endif
}

static inline uint64_t atomic_fetch_sub_64(atomic_64 *object, uint64_t operand) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_fetch_sub(object, operand);
#elif WINDOWS_INTERLOCKED_64 == 1
	return (uint64_t) InterlockedExchangeAdd64((LONG64*)object, -((LONG64)operand));
#elif WINDOWS_INTERLOCKED_64 == 0
	mutex_lock(&object->lock);
	uint64_t old = object->value;
	object->value -= operand;
	mutex_unlock(&object->lock);
	return old;
#endif
}

static inline uintptr_t atomic_fetch_sub_ptr(atomic_ptr *object, uintptr_t operand) {
#ifdef UINTPTR_32
	return (uintptr_t)  atomic_fetch_sub_32((atomic_32 *)object, (uint32_t) operand);
#else
	return (uintptr_t)  atomic_fetch_sub_64((atomic_64 *)object, (uint64_t) operand);
#endif
}

static inline uint32_t atomic_fetch_and_32(atomic_32 *object, uint32_t operand) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_fetch_and(object, operand);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return (uint32_t) InterlockedAnd((LONG*)object, (LONG)operand);
#endif
}

static inline uint64_t atomic_fetch_and_64(atomic_64 *object, uint64_t operand) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_fetch_and(object, operand);
#elif WINDOWS_INTERLOCKED_64 == 1
	return (uint64_t) InterlockedAnd64((LONG64*)object, (LONG64)operand);
#elif WINDOWS_INTERLOCKED_64 == 0
	mutex_lock(&object->lock);
	uint64_t old = object->value;
	object->value &= operand;
	mutex_unlock(&object->lock);
	return old;
#endif
}

static inline uintptr_t atomic_fetch_and_ptr(atomic_ptr *object, uintptr_t operand) {
#ifdef UINTPTR_32
	return (uintptr_t)  atomic_fetch_and_32((atomic_32 *)object, (uint32_t) operand);
#else
	return (uintptr_t)  atomic_fetch_and_64((atomic_64 *)object, (uint64_t) operand);
#endif
}

static inline uint32_t atomic_fetch_or_32(atomic_32 *object, uint32_t operand) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_fetch_or(object, operand);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return (uint32_t) InterlockedOr((LONG*)object, (LONG)operand);
#endif
}

static inline uint64_t atomic_fetch_or_64(atomic_64 *object, uint64_t operand) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_fetch_or(object, operand);
#elif WINDOWS_INTERLOCKED_64 == 1
	return (uint64_t) InterlockedOr64((LONG64*)object, (LONG64)operand);
#elif WINDOWS_INTERLOCKED_64 == 0
	mutex_lock(&object->lock);
	uint64_t old = object->value;
	object->value |= operand;
	mutex_unlock(&object->lock);
	return old;
#endif
}

static inline uintptr_t atomic_fetch_or_ptr(atomic_ptr *object, uintptr_t operand) {
#ifdef UINTPTR_32
	return (uintptr_t)  atomic_fetch_or_32((atomic_32 *)object, (uint32_t) operand);
#else
	return (uintptr_t)  atomic_fetch_or_64((atomic_64 *)object, (uint64_t) operand);
#endif
}

static inline uint32_t atomic_fetch_xor_32(atomic_32 *object, uint32_t operand) {
	#if defined(DESCENT_PLATFORM_TYPE_POSIX)
		return atomic_fetch_xor(object, operand);
	#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
		return (uint32_t) InterlockedXor((LONG*)object, (LONG)operand);
	#endif
}

static inline uint64_t atomic_fetch_xor_64(atomic_64 *object, uint64_t operand) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_fetch_xor(object, operand);
#elif WINDOWS_INTERLOCKED_64 == 1
	return (uint64_t) InterlockedXor64((LONG64*)object, (LONG64)operand);
#elif WINDOWS_INTERLOCKED_64 == 0
	mutex_lock(&object->lock);
	uint64_t old = object->value;
	object->value ^= operand;
	mutex_unlock(&object->lock);
	return old;
#endif
}

static inline uintptr_t atomic_fetch_xor_ptr(atomic_ptr *object, uintptr_t operand) {
#ifdef UINTPTR_32
	return (uintptr_t)  atomic_fetch_xor_32((atomic_32 *)object, (uint32_t) operand);
#else
	return (uintptr_t)  atomic_fetch_xor_64((atomic_64 *)object, (uint64_t) operand);
#endif
}

static inline void atomic_init_32(atomic_32 *object, uint32_t desired) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	atomic_init(object, desired);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	*object = desired;
#endif
}

static inline void atomic_init_64(atomic_64 *object, uint64_t desired) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	atomic_init(object, desired);
#elif WINDOWS_INTERLOCKED_64 == 1
	*object = desired;
#elif WINDOWS_INTERLOCKED_64 == 0
	mutex_init(&object->lock);
	object->value = desired;
#endif
}

static inline void atomic_init_ptr(atomic_ptr *object, uintptr_t desired) {
#ifdef UINTPTR_32
	atomic_init_32((atomic_32 *)object, (uint32_t) desired);
#else
	atomic_init_64((atomic_64 *)object, (uint64_t) desired);
#endif
}

static inline uint32_t atomic_load_32(const atomic_32 *object) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_load(object);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return (uint32_t) InterlockedCompareExchange((LONG*)object, 0, 0);
#endif
}

static inline uint64_t atomic_load_64(const atomic_64 *object) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_load(object);
#elif WINDOWS_INTERLOCKED_64 == 1
	return (uint64_t) InterlockedCompareExchange64((LONG64*)object, 0, 0);
#elif WINDOWS_INTERLOCKED_64 == 0
	mutex_lock((mutex_t*)&object->lock);
	uint64_t val = object->value;
	mutex_unlock((mutex_t*)&object->lock);
	return val;
#endif
}

static inline uintptr_t atomic_load_ptr(atomic_ptr *object) {
#ifdef UINTPTR_32
	return atomic_load_32(object);
#else
	return atomic_load_64(object);
#endif
}

static inline void atomic_store_32(atomic_32 *object, uint32_t desired) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	atomic_store(object, desired);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	InterlockedExchange((LONG*)object, (LONG)desired);
#endif
}

static inline void atomic_store_64(atomic_64 *object, uint64_t desired) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	atomic_store(object, desired);
#elif WINDOWS_INTERLOCKED_64 == 1
	InterlockedExchange64((LONG64*)object, (LONG64)desired);
#elif WINDOWS_INTERLOCKED_64 == 0
	mutex_lock(&object->lock);
	object->value = desired;
	mutex_unlock(&object->lock);
#endif
}

static inline void atomic_store_ptr(atomic_ptr *object, uintptr_t desired) {
#ifdef UINTPTR_32
	atomic_store_32(object, desired);
#else
	atomic_store_64(object, desired);
#endif
}

#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#define atomic_s_fence() atomic_signal_fence(memory_order_seq_cst)
#define atomic_t_fence() atomic_thread_fence(memory_order_seq_cst)
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#define atomic_s_fence() _ReadWriteBarrier()
#define atomic_t_fence() MemoryBarrier()
#endif

#endif