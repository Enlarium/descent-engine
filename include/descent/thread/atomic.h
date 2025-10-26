#ifndef DESCENT_ATOMIC_H
#define DESCENT_ATOMIC_H

#include <stdint.h>

#include "../utilities/platform.h"
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <stdatomic.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <windows.h>
#endif

#include "mutex.h"



// Check whether true 64-bit atomic operations are available on Windows
#if defined(DESCENT_PLATFORM_WINDOWS_64) || (_WIN32_WINNT >= 0x0600)
#define DESCENT_WINDOWS_INTERLOCKED_64 1
#else
#define DESCENT_WINDOWS_INTERLOCKED_64 0
#endif

// Check the pointer type size
#if UINTPTR_MAX == 0xFFFFFFFFUL
#define DESCENT_POINTER_32
#elif UINTPTR_MAX != 0xFFFFFFFFFFFFFFFFULL
#error "Pointers must be either 32 or 64 bits wide"
#endif



// All memory ordering is sequentially consistent to ensure cross-platform behavior is consistent



#ifdef __cplusplus
extern "C" {
#endif



#if defined(DESCENT_PLATFORM_TYPE_POSIX)

/**
 * @typedef atomic_32
 * @brief 32-bit atomic type.
 * 
 * @note All defined operations on this type are sequentially consistent.
 */
typedef _Atomic uint32_t  atomic_32;

/**
 * @typedef atomic_64
 * @brief 64-bit atomic type.
 * 
 * @note All defined operations on this type are sequentially consistent.
 */
typedef _Atomic uint64_t  atomic_64;

/**
 * @typedef atomic_ptr
 * @brief Atomic pointer container.
 * 
 * @note All defined operations on this type are sequentially consistent.
 */
typedef _Atomic uintptr_t atomic_ptr;

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

/**
 * @typedef atomic_32
 * @brief 32-bit atomic type.
 * 
 * @note All defined operations on this type are sequentially consistent.
 */
typedef __declspec(align(4)) volatile uint32_t  atomic_32;

#if DESCENT_WINDOWS_INTERLOCKED_64
/**
 * @typedef atomic_64
 * @brief 64-bit atomic type.
 * 
 * @note All defined operations on this type are sequentially consistent.
 */
typedef __declspec(align(8)) volatile uint64_t  atomic_64;
#else
/**
 * @typedef atomic_64
 * @brief 64-bit atomic type.
 * 
 * @note All defined operations on this type are sequentially consistent.
 */
typedef __declspec(align(8)) struct {
	mutex_t lock;
	volatile uint64_t value;
} atomic_64;
#endif

/**
 * @typedef atomic_ptr
 * @brief Atomic pointer container.
 * 
 * @note All defined operations on this type are sequentially consistent.
 */
typedef __declspec(align(sizeof(uintptr_t))) volatile uintptr_t atomic_ptr;

#endif



/**
 * @brief Atomically compare and exchange a 32-bit value.
 * @param object Pointer to the atomic_32 variable.
 * @param expected Pointer to the expected value. Updated to `object` if exchange fails.
 * @param desired Value to store if `object` equals `expected`.
 * @return 1 if successful, 0 otherwise.
 */
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

/**
 * @brief Atomically compare and exchange a 64-bit value.
 * @param object Pointer to the atomic_64 variable.
 * @param expected Pointer to the expected value. Updated to `object` if exchange fails.
 * @param desired Value to store if `object` equals `expected`.
 * @return 1 if successful, 0 otherwise.
 */
static inline int atomic_compare_exchange_64(atomic_64 *object, uint64_t *expected, uint64_t desired) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_compare_exchange_strong(object, expected, desired);
#elif DESCENT_WINDOWS_INTERLOCKED_64 == 1
	uint64_t original = (uint64_t) InterlockedCompareExchange64((LONG64*)object, (LONG64)desired, (LONG64)*expected);
	if (original == *expected) return 1;
	else {
		*expected = original;
		return 0;
	}
#elif DESCENT_WINDOWS_INTERLOCKED_64 == 0
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

/**
 * @brief Atomically compare and exchange a pointer value.
 * @param object Pointer to the atomic_ptr variable.
 * @param expected Pointer to the expected value. Updated to `object` if exchange fails.
 * @param desired Value to store if `object` equals `expected`.
 * @return 1 if successful, 0 otherwise.
 */
static inline int atomic_compare_exchange_ptr(atomic_ptr *object, uintptr_t *expected, uintptr_t desired) {
#ifdef DESCENT_POINTER_32
	return atomic_compare_exchange_32((atomic_32 *)object, (uint32_t *)expected, (uint32_t) desired);
#else
	return atomic_compare_exchange_64((atomic_64 *)object, (uint64_t *)expected, (uint64_t) desired);
#endif
}

/**
 * @brief Atomically exchanges a 32-bit value.
 * @param object Pointer to the atomic_32 variable.
 * @param desired New value to set.
 * @return The previous value of the atomic variable.
 */
static inline uint32_t atomic_exchange_32(atomic_32 *object, uint32_t desired) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_exchange(object, desired);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return (uint32_t) InterlockedExchange((LONG*)object, (LONG)desired);
#endif
}

/**
 * @brief Atomically exchanges a 64-bit value.
 * @param object Pointer to the atomic_64 variable.
 * @param desired New value to set.
 * @return The previous value of the atomic variable.
 */
static inline uint64_t atomic_exchange_64(atomic_64 *object, uint64_t desired) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_exchange(object, desired);
#elif DESCENT_WINDOWS_INTERLOCKED_64 == 1
	return (uint64_t) InterlockedExchange64((LONG64*)object, (LONG64)desired);
#elif DESCENT_WINDOWS_INTERLOCKED_64 == 0
	mutex_lock(&object->lock);
	uint64_t old = object->value;
	object->value = desired;
	mutex_unlock(&object->lock);
	return old;
#endif
}


/**
 * @brief Atomically exchanges a pointer value.
 * @param object Pointer to the atomic_ptr variable.
 * @param desired New pointer value to set.
 * @return The previous pointer value.
 */
static inline uintptr_t atomic_exchange_ptr(atomic_ptr *object, uintptr_t desired) {
#ifdef DESCENT_POINTER_32
	return (uintptr_t) atomic_exchange_32((atomic_32 *)object, (uint32_t) desired);
#else
	return (uintptr_t) atomic_exchange_64((atomic_64 *)object, (uint64_t) desired);
#endif
}


/**
 * @brief Atomically adds a value to a 32-bit atomic variable.
 * @param object Pointer to the atomic_32 variable.
 * @param operand Value to add.
 * @return The value before the addition.
 */
static inline uint32_t atomic_fetch_add_32(atomic_32 *object, uint32_t operand) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_fetch_add(object, operand);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return (uint32_t) InterlockedExchangeAdd((LONG*)object, (LONG)operand);
#endif
}

/**
 * @brief Atomically adds a value to a 64-bit atomic variable.
 * @param object Pointer to the atomic_64 variable.
 * @param operand Value to add.
 * @return The value before the addition.
 */
static inline uint64_t atomic_fetch_add_64(atomic_64 *object, uint64_t operand) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_fetch_add(object, operand);
#elif DESCENT_WINDOWS_INTERLOCKED_64 == 1
	return (uint64_t) InterlockedExchangeAdd64((LONG64*)object, (LONG64)operand);
#elif DESCENT_WINDOWS_INTERLOCKED_64 == 0
	mutex_lock(&object->lock);
	uint64_t old = object->value;
	object->value += operand;
	mutex_unlock(&object->lock);
	return old;
#endif
}

/**
 * @brief Atomically adds a value to a pointer-sized atomic variable.
 * @param object Pointer to the atomic_ptr variable.
 * @param operand Value to add.
 * @return The value before the addition.
 */
static inline uintptr_t atomic_fetch_add_ptr(atomic_ptr *object, uintptr_t operand) {
#ifdef DESCENT_POINTER_32
	return (uintptr_t)  atomic_fetch_add_32((atomic_32 *)object, (uint32_t) operand);
#else
	return (uintptr_t)  atomic_fetch_add_64((atomic_64 *)object, (uint64_t) operand);
#endif
}

/**
 * @brief Atomically subtracts a value from a 32-bit atomic variable.
 * @param object Pointer to the atomic_32 variable.
 * @param operand Value to subtract.
 * @return The value before the subtraction.
 */
static inline uint32_t atomic_fetch_sub_32(atomic_32 *object, uint32_t operand) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_fetch_sub(object, operand);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return (uint32_t) InterlockedExchangeAdd((LONG*)object, -((LONG)operand));
#endif
}

/**
 * @brief Atomically subtracts a value from a 64-bit atomic variable.
 * @param object Pointer to the atomic_64 variable.
 * @param operand Value to subtract.
 * @return The value before the subtraction.
 */
static inline uint64_t atomic_fetch_sub_64(atomic_64 *object, uint64_t operand) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_fetch_sub(object, operand);
#elif DESCENT_WINDOWS_INTERLOCKED_64 == 1
	return (uint64_t) InterlockedExchangeAdd64((LONG64*)object, -((LONG64)operand));
#elif DESCENT_WINDOWS_INTERLOCKED_64 == 0
	mutex_lock(&object->lock);
	uint64_t old = object->value;
	object->value -= operand;
	mutex_unlock(&object->lock);
	return old;
#endif
}

/**
 * @brief Atomically subtracts a value from a pointer-sized atomic variable.
 * @param object Pointer to the atomic_ptr variable.
 * @param operand Value to subtract.
 * @return The value before the subtraction.
 */
static inline uintptr_t atomic_fetch_sub_ptr(atomic_ptr *object, uintptr_t operand) {
#ifdef DESCENT_POINTER_32
	return (uintptr_t)  atomic_fetch_sub_32((atomic_32 *)object, (uint32_t) operand);
#else
	return (uintptr_t)  atomic_fetch_sub_64((atomic_64 *)object, (uint64_t) operand);
#endif
}

/**
 * @brief Atomically performs bitwise AND on a 32-bit atomic variable.
 * @param object Pointer to the atomic_32 variable.
 * @param operand Operand for AND operation.
 * @return The value before the operation.
 */
static inline uint32_t atomic_fetch_and_32(atomic_32 *object, uint32_t operand) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_fetch_and(object, operand);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return (uint32_t) InterlockedAnd((LONG*)object, (LONG)operand);
#endif
}


/**
 * @brief Atomically performs bitwise AND on a 64-bit atomic variable.
 * @param object Pointer to the atomic_64 variable.
 * @param operand Operand for AND operation.
 * @return The value before the operation.
 */
static inline uint64_t atomic_fetch_and_64(atomic_64 *object, uint64_t operand) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_fetch_and(object, operand);
#elif DESCENT_WINDOWS_INTERLOCKED_64 == 1
	return (uint64_t) InterlockedAnd64((LONG64*)object, (LONG64)operand);
#elif DESCENT_WINDOWS_INTERLOCKED_64 == 0
	mutex_lock(&object->lock);
	uint64_t old = object->value;
	object->value &= operand;
	mutex_unlock(&object->lock);
	return old;
#endif
}

/**
 * @brief Atomically performs bitwise AND on a pointer-sized atomic variable.
 * @param object Pointer to the atomic_ptr variable.
 * @param operand Operand for AND operation.
 * @return The value before the operation.
 */
static inline uintptr_t atomic_fetch_and_ptr(atomic_ptr *object, uintptr_t operand) {
#ifdef DESCENT_POINTER_32
	return (uintptr_t)  atomic_fetch_and_32((atomic_32 *)object, (uint32_t) operand);
#else
	return (uintptr_t)  atomic_fetch_and_64((atomic_64 *)object, (uint64_t) operand);
#endif
}

/**
 * @brief Atomically performs bitwise OR on a 32-bit atomic variable.
 * @param object Pointer to the atomic_32 variable.
 * @param operand Operand for OR operation.
 * @return The value before the operation.
 */
static inline uint32_t atomic_fetch_or_32(atomic_32 *object, uint32_t operand) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_fetch_or(object, operand);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return (uint32_t) InterlockedOr((LONG*)object, (LONG)operand);
#endif
}


/**
 * @brief Atomically performs bitwise OR on a 64-bit atomic variable.
 * @param object Pointer to the atomic_64 variable.
 * @param operand Operand for OR operation.
 * @return The value before the operation.
 */
static inline uint64_t atomic_fetch_or_64(atomic_64 *object, uint64_t operand) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_fetch_or(object, operand);
#elif DESCENT_WINDOWS_INTERLOCKED_64 == 1
	return (uint64_t) InterlockedOr64((LONG64*)object, (LONG64)operand);
#elif DESCENT_WINDOWS_INTERLOCKED_64 == 0
	mutex_lock(&object->lock);
	uint64_t old = object->value;
	object->value |= operand;
	mutex_unlock(&object->lock);
	return old;
#endif
}

/**
 * @brief Atomically performs bitwise OR on a pointer-sized atomic variable.
 * @param object Pointer to the atomic_ptr variable.
 * @param operand Operand for OR operation.
 * @return The value before the operation.
 */
static inline uintptr_t atomic_fetch_or_ptr(atomic_ptr *object, uintptr_t operand) {
#ifdef DESCENT_POINTER_32
	return (uintptr_t)  atomic_fetch_or_32((atomic_32 *)object, (uint32_t) operand);
#else
	return (uintptr_t)  atomic_fetch_or_64((atomic_64 *)object, (uint64_t) operand);
#endif
}

/**
 * @brief Atomically performs bitwise XOR on a 32-bit atomic variable.
 * @param object Pointer to the atomic_32 variable.
 * @param operand Operand for XOR operation.
 * @return The value before the operation.
 */
static inline uint32_t atomic_fetch_xor_32(atomic_32 *object, uint32_t operand) {
	#if defined(DESCENT_PLATFORM_TYPE_POSIX)
		return atomic_fetch_xor(object, operand);
	#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
		return (uint32_t) InterlockedXor((LONG*)object, (LONG)operand);
	#endif
}

/**
 * @brief Atomically performs bitwise XOR on a 64-bit atomic variable.
 * @param object Pointer to the atomic_64 variable.
 * @param operand Operand for XOR operation.
 * @return The value before the operation.
 */
static inline uint64_t atomic_fetch_xor_64(atomic_64 *object, uint64_t operand) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_fetch_xor(object, operand);
#elif DESCENT_WINDOWS_INTERLOCKED_64 == 1
	return (uint64_t) InterlockedXor64((LONG64*)object, (LONG64)operand);
#elif DESCENT_WINDOWS_INTERLOCKED_64 == 0
	mutex_lock(&object->lock);
	uint64_t old = object->value;
	object->value ^= operand;
	mutex_unlock(&object->lock);
	return old;
#endif
}

/**
 * @brief Atomically performs bitwise XOR on a pointer-sized atomic variable.
 * @param object Pointer to the atomic_ptr variable.
 * @param operand Operand for XOR operation.
 * @return The value before the operation.
 */
static inline uintptr_t atomic_fetch_xor_ptr(atomic_ptr *object, uintptr_t operand) {
#ifdef DESCENT_POINTER_32
	return (uintptr_t)  atomic_fetch_xor_32((atomic_32 *)object, (uint32_t) operand);
#else
	return (uintptr_t)  atomic_fetch_xor_64((atomic_64 *)object, (uint64_t) operand);
#endif
}


/**
 * @brief Initializes a 32-bit atomic variable.
 * @param object Pointer to the atomic_32 variable.
 * @param desired Initial value.
 */
static inline void atomic_init_32(atomic_32 *object, uint32_t desired) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	atomic_init(object, desired);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	*object = desired;
#endif
}

/**
 * @brief Initializes a 64-bit atomic variable.
 * @param object Pointer to the atomic_64 variable.
 * @param desired Initial value.
 */
static inline void atomic_init_64(atomic_64 *object, uint64_t desired) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	atomic_init(object, desired);
#elif DESCENT_WINDOWS_INTERLOCKED_64 == 1
	*object = desired;
#elif DESCENT_WINDOWS_INTERLOCKED_64 == 0
	mutex_init(&object->lock);
	object->value = desired;
#endif
}

/**
 * @brief Initializes a pointer-sized atomic variable.
 * @param object Pointer to the atomic_ptr variable.
 * @param desired Initial pointer value.
 */
static inline void atomic_init_ptr(atomic_ptr *object, uintptr_t desired) {
#ifdef DESCENT_POINTER_32
	atomic_init_32((atomic_32 *)object, (uint32_t) desired);
#else
	atomic_init_64((atomic_64 *)object, (uint64_t) desired);
#endif
}

/**
 * @brief Loads a 32-bit atomic variable.
 * @param object Pointer to the atomic_32 variable.
 * @return The current value.
 */
static inline uint32_t atomic_load_32(const atomic_32 *object) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_load(object);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	return (uint32_t) InterlockedCompareExchange((LONG*)object, 0, 0);
#endif
}

/**
 * @brief Loads a 64-bit atomic variable.
 * @param object Pointer to the atomic_64 variable.
 * @return The current value.
 */
static inline uint64_t atomic_load_64(const atomic_64 *object) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	return atomic_load(object);
#elif DESCENT_WINDOWS_INTERLOCKED_64 == 1
	return (uint64_t) InterlockedCompareExchange64((LONG64*)object, 0, 0);
#elif DESCENT_WINDOWS_INTERLOCKED_64 == 0
	mutex_lock((mutex_t*)&object->lock);
	uint64_t val = object->value;
	mutex_unlock((mutex_t*)&object->lock);
	return val;
#endif
}


/**
 * @brief Loads a pointer-sized atomic variable.
 * @param object Pointer to the atomic_ptr variable.
 * @return The current pointer value.
 */
static inline uintptr_t atomic_load_ptr(atomic_ptr *object) {
#ifdef DESCENT_POINTER_32
	return atomic_load_32((atomic_32 *)object);
#else
	return atomic_load_64((atomic_64 *)object);
#endif
}

/**
 * @brief Stores a value to a 32-bit atomic variable.
 * @param object Pointer to the atomic_32 variable.
 * @param desired Value to store.
 */
static inline void atomic_store_32(atomic_32 *object, uint32_t desired) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	atomic_store(object, desired);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	InterlockedExchange((LONG*)object, (LONG)desired);
#endif
}

/**
 * @brief Stores a value to a 64-bit atomic variable.
 * @param object Pointer to the atomic_64 variable.
 * @param desired Value to store.
 */
static inline void atomic_store_64(atomic_64 *object, uint64_t desired) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	atomic_store(object, desired);
#elif DESCENT_WINDOWS_INTERLOCKED_64 == 1
	InterlockedExchange64((LONG64*)object, (LONG64)desired);
#elif DESCENT_WINDOWS_INTERLOCKED_64 == 0
	mutex_lock(&object->lock);
	object->value = desired;
	mutex_unlock(&object->lock);
#endif
}

/**
 * @brief Stores a value to a pointer-sized atomic variable.
 * @param object Pointer to the atomic_ptr variable.
 * @param desired Pointer value to store.
 */
static inline void atomic_store_ptr(atomic_ptr *object, uintptr_t desired) {
#ifdef DESCENT_POINTER_32
	atomic_store_32((atomic_32 *)object, (uint32_t) desired);
#else
	atomic_store_64((atomic_64 *)object, (uint64_t) desired);
#endif
}

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

/**
 * @brief Compiler fence to prevent reordering of memory operations around this point.
 */
#define atomic_s_fence() atomic_signal_fence(memory_order_seq_cst)

/**
 * @brief Thread fence to enforce ordering of memory operations across threads.
 */
#define atomic_t_fence() atomic_thread_fence(memory_order_seq_cst)

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

/**
 * @brief Compiler fence to prevent reordering of memory operations around this point.
 */
#define atomic_s_fence() _ReadWriteBarrier()

/**
 * @brief Thread fence to enforce ordering of memory operations across threads.
 */
#define atomic_t_fence() MemoryBarrier()

#endif



#ifdef __cplusplus
}
#endif



#endif