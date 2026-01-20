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

#ifndef DESCENT_THREAD_ATOMIC_H
#define DESCENT_THREAD_ATOMIC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <descent/thread/atomic_types.h>

/**
 * @def atomic_always_lock_free
 * @ingroup atomic
 * @brief Indicates whether atomic operations on the given type are always
 * lock-free.
 * 
 * This macro evaluates to a compile-time constant expression.
 * 
 * @param ptr Pointer to an instance of the atomic data type.
 * @return True if operations are always lock-free, false otherwise.
 */
#define atomic_always_lock_free(ptr) __atomic_always_lock_free(sizeof(*(ptr)), ptr)

/**
 * @def atomic_is_lock_free
 * @ingroup atomic
 * @brief Indicates whether atomic operations on the given object are
 * lock-free.
 * 
 * The result may depend on runtime properties such as object alignment or
 * address.
 * 
 * @param ptr Pointer to an instance of the atomic data type.
 * @return True if operations are lock-free, false otherwise.
 */
#define atomic_is_lock_free(ptr) __atomic_is_lock_free(sizeof(*(ptr)), ptr)

/**
 * @ingroup atomic
 * @brief Memory ordering constraints for atomic operations.
 */
enum {
	ATOMIC_RELAXED = __ATOMIC_RELAXED, /**< Provides atomicity with no ordering or synchronization guarantees. */
	ATOMIC_ACQUIRE = __ATOMIC_ACQUIRE, /**< Prevents later memory operations from moving before the atomic operation. */
	ATOMIC_RELEASE = __ATOMIC_RELEASE, /**< Prevents earlier memory operations from moving after the atomic operation. */
	ATOMIC_ACQ_REL = __ATOMIC_ACQ_REL, /**< Combines @ref ATOMIC_ACQUIRE and @ref ATOMIC_RELEASE semantics. */
	ATOMIC_SEQ_CST = __ATOMIC_SEQ_CST  /**< All threads appear to execute atomics in the same sequence. */
};

/**
 * @ingroup atomic
 * @brief Atomically loads the value of an atomic integer and returns it.
 * 
 * Valid memory orderings are:
 * - @ref ATOMIC_RELAXED
 * - @ref ATOMIC_ACQUIRE
 * - @ref ATOMIC_SEQ_CST
 * 
 * @param ptr Pointer to the atomic integer.
 * @param order Memory ordering constraint.
 * @return The loaded value.
 */
static inline int atomic_load_int(atomic_int *ptr, int order) {
	return (int) __atomic_load_n(&ptr->_atomic, order);
}

/**
 * @ingroup atomic
 * @brief Atomically stores a value into an atomic integer.
 * 
 * Valid memory orderings are:
 * - @ref ATOMIC_RELAXED
 * - @ref ATOMIC_RELEASE
 * - @ref ATOMIC_SEQ_CST
 * 
 * @param ptr Pointer to the atomic integer.
 * @param val The value to store.
 * @param order Memory ordering constraint.
 */
static inline void atomic_store_int(atomic_int *ptr, int val, int order) {
	__atomic_store_n(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically stores a value into an atomic integer and returns the old
 * value.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic integer.
 * @param val The value to store.
 * @param order Memory ordering constraint.
 * @return The old value of the atomic integer.
 */
static inline int atomic_exchange_int(atomic_int *ptr, int val, int order) {
	return (int) __atomic_exchange_n(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically compares the value of an atomic integer with an expected
 * value and, if they are equal, replaces it with a desired value.
 * 
 * All memory orderings are valid for @p success_order.
 * 
 * Valid memory orderings for @p failure_order are:
 * - @ref ATOMIC_RELAXED
 * - @ref ATOMIC_ACQUIRE
 * - @ref ATOMIC_SEQ_CST
 * 
 * @param ptr Pointer to the atomic integer.
 * @param expected Pointer to the expected value. If the comparison fails, this
 * value is updated with the current value of the atomic integer.
 * @param desired The value to store if the comparison succeeds.
 * @param success_order Memory ordering constraint if the exchange is successful.
 * @param failure_order Memory ordering constraint if the exchange fails.
 * @return True if the exchange was performed, false otherwise.
 */
static inline bool atomic_compare_exchange_int(atomic_int *ptr, int *expected, int desired, int success_order, int failure_order) {
	return __atomic_compare_exchange_n(&ptr->_atomic, expected, desired, 0, success_order, failure_order);
}

/**
 * @ingroup atomic
 * @brief Atomically adds a value to an atomic integer and returns the new
 * value.
 *
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic integer.
 * @param val The value to add.
 * @param order Memory ordering constraint.
 * @return The new value after addition.
 */
static inline int atomic_add_fetch_int(atomic_int *ptr, int val, int order) {
	return (int) __atomic_add_fetch(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically subtracts a value from an atomic integer and returns the
 * new value.
 *
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic integer.
 * @param val The value to subtract.
 * @param order Memory ordering constraint.
 * @return The new value after subtraction.
 */
static inline int atomic_sub_fetch_int(atomic_int *ptr, int val, int order) {
	return (int) __atomic_sub_fetch(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically performs a bitwise AND between an atomic integer and a
 * value, storing the result into the atomic integer and returning it.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic integer.
 * @param val The value to AND with.
 * @param order Memory ordering constraint.
 * @return The new value after the AND operation.
 */
static inline int atomic_and_fetch_int(atomic_int *ptr, int val, int order) {
	return (int) __atomic_and_fetch(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically performs a bitwise XOR between an atomic integer and a
 * value, storing the result into the atomic integer and returning it.
 *
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic integer.
 * @param val The value to XOR with.
 * @param order Memory ordering constraint.
 * @return The new value after the XOR operation.
 */
static inline int atomic_xor_fetch_int(atomic_int *ptr, int val, int order) {
	return (int) __atomic_xor_fetch(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically performs a bitwise OR between an atomic integer and a
 * value, storing the result into the atomic integer and returning it.
 *
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic integer.
 * @param val The value to OR with.
 * @param order Memory ordering constraint.
 * @return The new value after the OR operation.
 */
static inline int atomic_or_fetch_int(atomic_int *ptr, int val, int order) {
	return (int) __atomic_or_fetch(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically performs a bitwise NAND between an atomic integer and a
 * value, storing the result into the atomic integer and returning it.
 *
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic integer.
 * @param val The value to NAND with.
 * @param order Memory ordering constraint.
 * @return The new value after the NAND operation.
 */
static inline int atomic_nand_fetch_int(atomic_int *ptr, int val, int order) {
	return (int) __atomic_nand_fetch(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically adds a value to an atomic integer and returns the old
 * value.
 *
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic integer.
 * @param val The value to add.
 * @param order Memory ordering constraint.
 * @return The old value of the atomic integer.
 */
static inline int atomic_fetch_add_int(atomic_int *ptr, int val, int order) {
	return (int) __atomic_fetch_add(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically subtracts a value from an atomic integer and returns the
 * old value.
 *
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic integer.
 * @param val The value to subtract.
 * @param order Memory ordering constraint.
 * @return The old value of the atomic integer.
 */
static inline int atomic_fetch_sub_int(atomic_int *ptr, int val, int order) {
	return (int) __atomic_fetch_sub(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically performs a bitwise AND between an atomic integer and a
 * value, storing the result into the atomic integer and returning the old
 * value.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic integer.
 * @param val The value to AND with.
 * @param order Memory ordering constraint.
 * @return The old value of the atomic integer.
 */
static inline int atomic_fetch_and_int(atomic_int *ptr, int val, int order) {
	return (int) __atomic_fetch_and(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically performs a bitwise XOR between an atomic integer and a
 * value, storing the result into the atomic integer and returning the old
 * value.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic integer.
 * @param val The value to XOR with.
 * @param order Memory ordering constraint.
 * @return The old value of the atomic integer.
 */
static inline int atomic_fetch_xor_int(atomic_int *ptr, int val, int order) {
	return (int) __atomic_fetch_xor(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically performs a bitwise OR between an atomic integer and a
 * value, storing the result into the atomic integer and returning the old
 * value.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic integer.
 * @param val The value to OR with.
 * @param order Memory ordering constraint.
 * @return The old value of the atomic integer.
 */
static inline int atomic_fetch_or_int(atomic_int *ptr, int val, int order) {
	return (int) __atomic_fetch_or(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically performs a bitwise NAND between an atomic integer and a
 * value, storing the result into the atomic integer and returning the old
 * value.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic integer.
 * @param val The value to NAND with.
 * @param order Memory ordering constraint.
 * @return The old value of the atomic integer.
 */
static inline int atomic_fetch_nand_int(atomic_int *ptr, int val, int order) {
	return (int) __atomic_fetch_nand(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically loads the value of an atomic uint32_t and returns it.
 * 
 * Valid memory orderings are:
 * - @ref ATOMIC_RELAXED
 * - @ref ATOMIC_ACQUIRE
 * - @ref ATOMIC_SEQ_CST
 * 
 * @param ptr Pointer to the atomic uint32_t.
 * @param order Memory ordering constraint.
 * @return The loaded value.
 */
static inline uint32_t atomic_load_32(atomic_32 *ptr, int order) {
	return (uint32_t) __atomic_load_n(&ptr->_atomic, order);
}

/**
 * @ingroup atomic
 * @brief Atomically stores a value into an atomic uint32_t.
 * 
 * Valid memory orderings are:
 * - @ref ATOMIC_RELAXED
 * - @ref ATOMIC_RELEASE
 * - @ref ATOMIC_SEQ_CST
 * 
 * @param ptr Pointer to the atomic uint32_t.
 * @param val The value to store.
 * @param order Memory ordering constraint.
 */
static inline void atomic_store_32(atomic_32 *ptr, uint32_t val, int order) {
	__atomic_store_n(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically stores a value into an atomic uint32_t and returns the old
 * value.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint32_t.
 * @param val The value to store.
 * @param order Memory ordering constraint.
 * @return The old value of the atomic uint32_t.
 */
static inline uint32_t atomic_exchange_32(atomic_32 *ptr, uint32_t val, int order) {
	return (uint32_t) __atomic_exchange_n(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically compares the value of an atomic uint32_t with an expected
 * value and, if they are equal, replaces it with a desired value.
 * 
 * All memory orderings are valid for @p success_order.
 * 
 * Valid memory orderings for @p failure_order are:
 * - @ref ATOMIC_RELAXED
 * - @ref ATOMIC_ACQUIRE
 * - @ref ATOMIC_SEQ_CST
 * 
 * @param ptr Pointer to the atomic uint32_t.
 * @param expected Pointer to the expected value. If the comparison fails, this
 * value is updated with the current value of the atomic uint32_t.
 * @param desired The value to store if the comparison succeeds.
 * @param success_order Memory ordering constraint if the exchange is successful.
 * @param failure_order Memory ordering constraint if the exchange fails.
 * @return True if the exchange was performed, false otherwise.
 */
static inline bool atomic_compare_exchange_32(atomic_32 *ptr, uint32_t *expected, uint32_t desired, int success_order, int failure_order) {
	return __atomic_compare_exchange_n(&ptr->_atomic, expected, desired, 0, success_order, failure_order);
}

/**
 * @ingroup atomic
 * @brief Atomically adds a value to an atomic uint32_t and returns the new
 * value.
 *
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint32_t.
 * @param val The value to add.
 * @param order Memory ordering constraint.
 * @return The new value after addition.
 */
static inline uint32_t atomic_add_fetch_32(atomic_32 *ptr, uint32_t val, int order) {
	return (uint32_t) __atomic_add_fetch(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically subtracts a value from an atomic uint32_t and returns the
 * new value.
 *
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint32_t.
 * @param val The value to subtract.
 * @param order Memory ordering constraint.
 * @return The new value after subtraction.
 */
static inline uint32_t atomic_sub_fetch_32(atomic_32 *ptr, uint32_t val, int order) {
	return (uint32_t) __atomic_sub_fetch(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically performs a bitwise AND between an atomic uint32_t and a
 * value, storing the result into the atomic uint32_t and returning it.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint32_t.
 * @param val The value to AND with.
 * @param order Memory ordering constraint.
 * @return The new value after the AND operation.
 */
static inline uint32_t atomic_and_fetch_32(atomic_32 *ptr, uint32_t val, int order) {
	return (uint32_t) __atomic_and_fetch(&ptr->_atomic, val, order);
}


/**
 * @ingroup atomic
 * @brief Atomically performs a bitwise XOR between an atomic uint32_t and a
 * value, storing the result into the atomic uint32_t and returning it.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint32_t.
 * @param val The value to XOR with.
 * @param order Memory ordering constraint.
 * @return The new value after the XOR operation.
 */
static inline uint32_t atomic_xor_fetch_32(atomic_32 *ptr, uint32_t val, int order) {
	return (uint32_t) __atomic_xor_fetch(&ptr->_atomic, val, order);
}


/**
 * @ingroup atomic
 * @brief Atomically performs a bitwise OR between an atomic uint32_t and a
 * value, storing the result into the atomic uint32_t and returning it.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint32_t.
 * @param val The value to OR with.
 * @param order Memory ordering constraint.
 * @return The new value after the OR operation.
 */
static inline uint32_t atomic_or_fetch_32(atomic_32 *ptr, uint32_t val, int order) {
	return (uint32_t) __atomic_or_fetch(&ptr->_atomic, val, order);
}


/**
 * @ingroup atomic
 * @brief Atomically performs a bitwise NAND between an atomic uint32_t and a
 * value, storing the result into the atomic uint32_t and returning it.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint32_t.
 * @param val The value to NAND with.
 * @param order Memory ordering constraint.
 * @return The new value after the NAND operation.
 */
static inline uint32_t atomic_nand_fetch_32(atomic_32 *ptr, uint32_t val, int order) {
	return (uint32_t) __atomic_nand_fetch(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically adds a value to an atomic uint32_t and returns the old
 * value.
 *
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint32_t.
 * @param val The value to add.
 * @param order Memory ordering constraint.
 * @return The old value of the atomic uint32_t.
 */
static inline uint32_t atomic_fetch_add_32(atomic_32 *ptr, uint32_t val, int order) {
	return (uint32_t) __atomic_fetch_add(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically subtracts a value from an atomic uint32_t and returns the
 * old value.
 *
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint32_t.
 * @param val The value to subtract.
 * @param order Memory ordering constraint.
 * @return The old value of the atomic uint32_t.
 */
static inline uint32_t atomic_fetch_sub_32(atomic_32 *ptr, uint32_t val, int order) {
	return (uint32_t) __atomic_fetch_sub(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically performs a bitwise AND between an atomic uint32_t and a
 * value, storing the result into the atomic uint32_t and returning the old
 * value.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint32_t.
 * @param val The value to AND with.
 * @param order Memory ordering constraint.
 * @return The old value of the atomic uint32_t.
 */
static inline uint32_t atomic_fetch_and_32(atomic_32 *ptr, uint32_t val, int order) {
	return (uint32_t) __atomic_fetch_and(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically performs a bitwise XOR between an atomic uint32_t and a
 * value, storing the result into the atomic uint32_t and returning the old
 * value.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint32_t.
 * @param val The value to XOR with.
 * @param order Memory ordering constraint.
 * @return The old value of the atomic uint32_t.
 */
static inline uint32_t atomic_fetch_xor_32(atomic_32 *ptr, uint32_t val, int order) {
	return (uint32_t) __atomic_fetch_xor(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically performs a bitwise OR between an atomic uint32_t and a
 * value, storing the result into the atomic uint32_t and returning the old
 * value.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint32_t.
 * @param val The value to OR with.
 * @param order Memory ordering constraint.
 * @return The old value of the atomic uint32_t.
 */
static inline uint32_t atomic_fetch_or_32(atomic_32 *ptr, uint32_t val, int order) {
	return (uint32_t) __atomic_fetch_or(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically performs a bitwise NAND between an atomic uint32_t and a
 * value, storing the result into the atomic uint32_t and returning the old
 * value.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint32_t.
 * @param val The value to NAND with.
 * @param order Memory ordering constraint.
 * @return The old value of the atomic uint32_t.
 */
static inline uint32_t atomic_fetch_nand_32(atomic_32 *ptr, uint32_t val, int order) {
	return (uint32_t) __atomic_fetch_nand(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically loads the value of an atomic uint64_t and returns it.
 * 
 * Valid memory orderings are:
 * - @ref ATOMIC_RELAXED
 * - @ref ATOMIC_ACQUIRE
 * - @ref ATOMIC_SEQ_CST
 * 
 * @param ptr Pointer to the atomic uint64_t.
 * @param order Memory ordering constraint.
 * @return The loaded value.
 */
static inline uint64_t atomic_load_64(atomic_64 *ptr, int order) {
	return (uint64_t) __atomic_load_n(&ptr->_atomic, order);
}

/**
 * @ingroup atomic
 * @brief Atomically stores a value into an atomic uint64_t.
 * 
 * Valid memory orderings are:
 * - @ref ATOMIC_RELAXED
 * - @ref ATOMIC_RELEASE
 * - @ref ATOMIC_SEQ_CST
 * 
 * @param ptr Pointer to the atomic uint64_t.
 * @param val The value to store.
 * @param order Memory ordering constraint.
 */
static inline void atomic_store_64(atomic_64 *ptr, uint64_t val, int order) {
	__atomic_store_n(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically stores a value into an atomic uint64_t and returns the old
 * value.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint64_t.
 * @param val The value to store.
 * @param order Memory ordering constraint.
 * @return The old value of the atomic uint64_t.
 */
static inline uint64_t atomic_exchange_64(atomic_64 *ptr, uint64_t val, int order) {
	return (uint64_t) __atomic_exchange_n(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically compares the value of an atomic uint64_t with an expected
 * value and, if they are equal, replaces it with a desired value.
 * 
 * All memory orderings are valid for @p success_order.
 * 
 * Valid memory orderings for @p failure_order are:
 * - @ref ATOMIC_RELAXED
 * - @ref ATOMIC_ACQUIRE
 * - @ref ATOMIC_SEQ_CST
 * 
 * @param ptr Pointer to the atomic uint64_t.
 * @param expected Pointer to the expected value. If the comparison fails, this
 * value is updated with the current value of the atomic uint64_t.
 * @param desired The value to store if the comparison succeeds.
 * @param success_order Memory ordering constraint if the exchange is successful.
 * @param failure_order Memory ordering constraint if the exchange fails.
 * @return True if the exchange was performed, false otherwise.
 */
static inline bool atomic_compare_exchange_64(atomic_64 *ptr, uint64_t *expected, uint64_t desired, int success_order, int failure_order) {
	return __atomic_compare_exchange_n(&ptr->_atomic, expected, desired, 0, success_order, failure_order);
}

/**
 * @ingroup atomic
 * @brief Atomically adds a value to an atomic uint64_t and returns the new
 * value.
 *
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint64_t.
 * @param val The value to add.
 * @param order Memory ordering constraint.
 * @return The new value after addition.
 */
static inline uint64_t atomic_add_fetch_64(atomic_64 *ptr, uint64_t val, int order) {
	return (uint64_t) __atomic_add_fetch(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically subtracts a value from an atomic uint64_t and returns the
 * new value.
 *
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint64_t.
 * @param val The value to subtract.
 * @param order Memory ordering constraint.
 * @return The new value after subtraction.
 */
static inline uint64_t atomic_sub_fetch_64(atomic_64 *ptr, uint64_t val, int order) {
	return (uint64_t) __atomic_sub_fetch(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically performs a bitwise AND between an atomic uint64_t and a
 * value, storing the result into the atomic uint64_t and returning it.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint64_t.
 * @param val The value to AND with.
 * @param order Memory ordering constraint.
 * @return The new value after the AND operation.
 */
static inline uint64_t atomic_and_fetch_64(atomic_64 *ptr, uint64_t val, int order) {
	return (uint64_t) __atomic_and_fetch(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically performs a bitwise XOR between an atomic uint64_t and a
 * value, storing the result into the atomic uint64_t and returning it.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint64_t.
 * @param val The value to XOR with.
 * @param order Memory ordering constraint.
 * @return The new value after the XOR operation.
 */
static inline uint64_t atomic_xor_fetch_64(atomic_64 *ptr, uint64_t val, int order) {
	return (uint64_t) __atomic_xor_fetch(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically performs a bitwise OR between an atomic uint64_t and a
 * value, storing the result into the atomic uint64_t and returning it.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint64_t.
 * @param val The value to OR with.
 * @param order Memory ordering constraint.
 * @return The new value after the OR operation.
 */
static inline uint64_t atomic_or_fetch_64(atomic_64 *ptr, uint64_t val, int order) {
	return (uint64_t) __atomic_or_fetch(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically performs a bitwise NAND between an atomic uint64_t and a
 * value, storing the result into the atomic uint64_t and returning it.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint64_t.
 * @param val The value to NAND with.
 * @param order Memory ordering constraint.
 * @return The new value after the NAND operation.
 */
static inline uint64_t atomic_nand_fetch_64(atomic_64 *ptr, uint64_t val, int order) {
	return (uint64_t) __atomic_nand_fetch(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically adds a value to an atomic uint64_t and returns the old
 * value.
 *
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint64_t.
 * @param val The value to add.
 * @param order Memory ordering constraint.
 * @return The old value of the atomic uint64_t.
 */
static inline uint64_t atomic_fetch_add_64(atomic_64 *ptr, uint64_t val, int order) {
	return (uint64_t) __atomic_fetch_add(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically subtracts a value from an atomic uint64_t and returns the
 * old value.
 *
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint64_t.
 * @param val The value to subtract.
 * @param order Memory ordering constraint.
 * @return The old value of the atomic uint64_t.
 */
static inline uint64_t atomic_fetch_sub_64(atomic_64 *ptr, uint64_t val, int order) {
	return (uint64_t) __atomic_fetch_sub(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically performs a bitwise AND between an atomic uint64_t and a
 * value, storing the result into the atomic uint64_t and returning the old
 * value.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint64_t.
 * @param val The value to AND with.
 * @param order Memory ordering constraint.
 * @return The old value of the atomic uint64_t.
 */
static inline uint64_t atomic_fetch_and_64(atomic_64 *ptr, uint64_t val, int order) {
	return (uint64_t) __atomic_fetch_and(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically performs a bitwise XOR between an atomic uint64_t and a
 * value, storing the result into the atomic uint64_t and returning the old
 * value.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint64_t.
 * @param val The value to XOR with.
 * @param order Memory ordering constraint.
 * @return The old value of the atomic uint64_t.
 */
static inline uint64_t atomic_fetch_xor_64(atomic_64 *ptr, uint64_t val, int order) {
	return (uint64_t) __atomic_fetch_xor(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically performs a bitwise OR between an atomic uint64_t and a
 * value, storing the result into the atomic uint64_t and returning the old
 * value.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint64_t.
 * @param val The value to OR with.
 * @param order Memory ordering constraint.
 * @return The old value of the atomic uint64_t.
 */
static inline uint64_t atomic_fetch_or_64(atomic_64 *ptr, uint64_t val, int order) {
	return (uint64_t) __atomic_fetch_or(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically performs a bitwise NAND between an atomic uint64_t and a
 * value, storing the result into the atomic uint64_t and returning the old
 * value.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic uint64_t.
 * @param val The value to NAND with.
 * @param order Memory ordering constraint.
 * @return The old value of the atomic uint64_t.
 */
static inline uint64_t atomic_fetch_nand_64(atomic_64 *ptr, uint64_t val, int order) {
	return (uint64_t) __atomic_fetch_nand(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically loads the value of an atomic pointer and returns it.
 * 
 * Valid memory orderings are:
 * - @ref ATOMIC_RELAXED
 * - @ref ATOMIC_ACQUIRE
 * - @ref ATOMIC_SEQ_CST
 * 
 * @param ptr Pointer to the atomic pointer.
 * @param order Memory ordering constraint.
 * @return The loaded value.
 */
static inline uintptr_t atomic_load_ptr(atomic_ptr *ptr, int order) {
	return (uintptr_t) __atomic_load_n(&ptr->_atomic, order);
}

/**
 * @ingroup atomic
 * @brief Atomically stores a value into an atomic pointer.
 * 
 * Valid memory orderings are:
 * - @ref ATOMIC_RELAXED
 * - @ref ATOMIC_RELEASE
 * - @ref ATOMIC_SEQ_CST
 * 
 * @param ptr Pointer to the atomic pointer.
 * @param val The value to store.
 * @param order Memory ordering constraint.
 */
static inline void atomic_store_ptr(atomic_ptr *ptr, uintptr_t val, int order) {
	__atomic_store_n(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically stores a value into an atomic pointer and returns the old
 * value.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic pointer.
 * @param val The value to store.
 * @param order Memory ordering constraint.
 * @return The old value of the atomic pointer.
 */
static inline uintptr_t atomic_exchange_ptr(atomic_ptr *ptr, uintptr_t val, int order) {
	return (uintptr_t) __atomic_exchange_n(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically compares the value of an atomic pointer with an expected
 * value and, if they are equal, replaces it with a desired value.
 * 
 * All memory orderings are valid for @p success_order.
 * 
 * Valid memory orderings for @p failure_order are:
 * - @ref ATOMIC_RELAXED
 * - @ref ATOMIC_ACQUIRE
 * - @ref ATOMIC_SEQ_CST
 * 
 * @param ptr Pointer to the atomic pointer.
 * @param expected Pointer to the expected value. If the comparison fails, this
 * value is updated with the current value of the atomic pointer.
 * @param desired The value to store if the comparison succeeds.
 * @param success_order Memory ordering constraint if the exchange is successful.
 * @param failure_order Memory ordering constraint if the exchange fails.
 * @return True if the exchange was performed, false otherwise.
 */
static inline bool atomic_compare_exchange_ptr(atomic_ptr *ptr, uintptr_t *expected, uintptr_t desired, int success_order, int failure_order) {
	return __atomic_compare_exchange_n(&ptr->_atomic, expected, desired, 0, success_order, failure_order);
}

/**
 * @ingroup atomic
 * @brief Atomically loads the value of an atomic boolean and returns it.
 * 
 * Valid memory orderings are:
 * - @ref ATOMIC_RELAXED
 * - @ref ATOMIC_ACQUIRE
 * - @ref ATOMIC_SEQ_CST
 * 
 * @param ptr Pointer to the atomic boolean.
 * @param order Memory ordering constraint.
 * @return The loaded value.
 */
static inline bool atomic_load_bool(atomic_bool *ptr, int order) {
	return (bool) __atomic_load_n(&ptr->_atomic, order);
}

/**
 * @ingroup atomic
 * @brief Atomically stores a value into an atomic boolean.
 * 
 * Valid memory orderings are:
 * - @ref ATOMIC_RELAXED
 * - @ref ATOMIC_RELEASE
 * - @ref ATOMIC_SEQ_CST
 * 
 * @param ptr Pointer to the atomic boolean.
 * @param val The value to store.
 * @param order Memory ordering constraint.
 */
static inline void atomic_store_bool(atomic_bool *ptr, bool val, int order) {
	__atomic_store_n(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically stores a value into an atomic boolean and returns the old
 * value.
 * 
 * All memory orderings are valid.
 *
 * @param ptr Pointer to the atomic boolean.
 * @param val The value to store.
 * @param order Memory ordering constraint.
 * @return The old value of the atomic boolean.
 */
static inline bool atomic_exchange_bool(atomic_bool *ptr, bool val, int order) {
	return (bool) __atomic_exchange_n(&ptr->_atomic, val, order);
}

/**
 * @ingroup atomic
 * @brief Atomically compares the value of an atomic bool with an expected
 * value and, if they are equal, replaces it with a desired value.
 * 
 * All memory orderings are valid for @p success_order.
 * 
 * Valid memory orderings for @p failure_order are:
 * - @ref ATOMIC_RELAXED
 * - @ref ATOMIC_ACQUIRE
 * - @ref ATOMIC_SEQ_CST
 * 
 * @param ptr Pointer to the atomic bool.
 * @param expected Pointer to the expected value. If the comparison fails, this
 * value is updated with the current value of the atomic bool.
 * @param desired The value to store if the comparison succeeds.
 * @param success_order Memory ordering constraint if the exchange is successful.
 * @param failure_order Memory ordering constraint if the exchange fails.
 * @return True if the exchange was performed, false otherwise.
 */
static inline bool atomic_compare_exchange_bool(atomic_bool *ptr, bool *expected, bool desired, int success_order, int failure_order) {
	return __atomic_compare_exchange_n(&ptr->_atomic, expected, desired, 0, success_order, failure_order);
}

/**
 * @ingroup atomic
 * @brief Atomically sets an atomic boolean and returns the old value.
 * 
 * All memory orderings are valid.
 * 
 * @param ptr Pointer to the atomic boolean.
 * @param order Memory ordering constraint.
 */
static inline bool atomic_test_and_set(atomic_bool *ptr, int order) {
	return __atomic_test_and_set(&ptr->_atomic, order);
}

/**
 * @ingroup atomic
 * @brief Atomically clears an atomic boolean.
 * 
 * Valid memory orderings are:
 * - @ref ATOMIC_RELAXED
 * - @ref ATOMIC_RELEASE
 * - @ref ATOMIC_SEQ_CST
 * 
 * @param ptr Pointer to the atomic boolean.
 * @param order Memory ordering constraint.
 */
static inline void atomic_clear(atomic_bool *ptr, int order) {
	__atomic_clear(&ptr->_atomic, order);
}

/**
 * @ingroup atomic
 * @brief Establishes a memory fence between threads.
 *
 * Prevents certain memory operations issued before the fence from being
 * reordered with memory operations issued after the fence, according to the
 * specified memory ordering constraint.
 *
 * This function does not access or modify any atomic object; it only enforces
 * ordering constraints on memory operations as observed by other threads.
 *
 * All memory orderings are valid.
 *
 * @param order Memory ordering constraint.
 */
static inline void atomic_thread_fence(int order) {
	__atomic_thread_fence(order);
}

/**
 * @ingroup atomic
 * @brief Establishes a memory fence with respect to signal handlers.
 *
 * Prevents certain memory operations issued before the fence from being
 * reordered with memory operations issued after the fence, according to the
 * specified memory ordering constraint.
 *
 * This function only affects compiler reordering with respect to signal
 * handlers and does not emit any hardware memory barrier instructions.
 *
 * All memory orderings are valid.
 *
 * @param order Memory ordering constraint.
 */
static inline void atomic_signal_fence(int order) {
	__atomic_signal_fence(order);
}

#endif