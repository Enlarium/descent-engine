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

#ifndef DESCENT_INTERN_THREAD_TID_H
#define DESCENT_INTERN_THREAD_TID_H

#include <stdbool.h>
#include <stdint.h>

#include <descent/rcode.h>
#include <intern/thread/thread.h>

#define TID_NONE 0ull /**< No thread assigned */
#define TID_MAIN 1ull /**< Main thread ID */

#define BIT_RANGE_MASK(min, max) (((1ull << ((max) - (min) + 1)) - 1ull) << (min))

#define TID_UNIQUE_INDEX_MIN 1ull
#define TID_UNIQUE_INDEX_MAX DESCENT_UNIQUE_THREAD_COUNT_MAX
#define TID_WORKER_INDEX_MIN (DESCENT_UNIQUE_THREAD_COUNT_MAX + 1ull)
#define TID_WORKER_INDEX_MAX (DESCENT_UNIQUE_THREAD_COUNT_MAX + DESCENT_WORKER_THREAD_COUNT_MAX)
#define TID_UNIQUE_MASK      BIT_RANGE_MASK(TID_UNIQUE_INDEX_MIN, TID_UNIQUE_INDEX_MAX)
#define TID_WORKER_MASK      BIT_RANGE_MASK(TID_WORKER_INDEX_MIN, TID_WORKER_INDEX_MAX)

typedef uint64_t thread_id;     /**< Single-thread identifier (one bit set) */
typedef uint64_t thread_id_set; /**< Set of thread IDs (bitmask) */

/**
 * @brief Returns the calling thread's ID.
 * @return The thread ID, or TID_NONE if unassigned.
 */
thread_id tid_self(void);

/**
 * @brief Generate a unique thread ID from a zero-based index.
 * @param index Zero-based unique thread index.
 * @return Corresponding thread_id, or TID_NONE if out of range.
 */
static inline thread_id tid_generate_unique(unsigned int index) {
	// index is zero-based, maps to bit [TID_UNIQUE_INDEX_MIN + index]
	if (index >= DESCENT_UNIQUE_THREAD_COUNT_MAX) return TID_NONE;
	return 1ull << (TID_UNIQUE_INDEX_MIN + index);
}

/**
 * @brief Generate a worker thread ID from a zero-based index.
 * @param index Zero-based worker thread index.
 * @return Corresponding thread_id, or TID_NONE if out of range.
 */
static inline thread_id tid_generate_worker(unsigned int index) {
	// index is zero-based, maps to bit [TID_WORKER_INDEX_MIN + index]
	if (index >= DESCENT_WORKER_THREAD_COUNT_MAX) return TID_NONE;
	return 1ull << (TID_WORKER_INDEX_MIN + index);
}

/**
 * @brief Returns true if the thread ID represents the main thread.
 * @param t The thread ID to check.
 * @return True if the thread ID represents the main thread, false otherwise.
 */
static inline bool tid_is_main(thread_id t) {
	return t == TID_MAIN;
}

/**
 * @brief Returns true if the thread ID represents a unique thread.
 * @param t The thread ID to check.
 * @return True if the thread ID represents a unique thread, false otherwise.
 */
static inline bool tid_is_unique(thread_id t) {
	// This checks that only one bit is set
	// - "(t & TID_UNIQUE_MASK)" is true if "t" is nonzero and within the mask
	// - "t & (t - 1)" is equal to "t" with the least significant set bit cleared
	// - "!(t & (t - 1))" is therefore true if one or zero bits were set
	return (t & TID_UNIQUE_MASK) && !(t & (t - 1));
}

/**
 * @brief Returns true if the thread ID represents a worker thread.
 * @param t The thread ID to check.
 * @return True if the thread ID represents a worker thread, false otherwise.
 */
static inline bool tid_is_worker(thread_id t) {
	return (t & TID_WORKER_MASK) && !(t & (t - 1));
}

/**
 * @brief Returns true if the thread ID is a managed thread (main, unique, or
 * worker).
 * @param t The thread ID to check.
 * @return True if the thread ID represents a managed thread, false otherwise.
 */
static inline bool tid_is_managed(thread_id t) {
	return (t & (TID_MAIN | TID_UNIQUE_MASK | TID_WORKER_MASK)) && !(t & (t - 1));
}

/**
 * @brief Returns true if the given thread ID matches the calling thread's ID.
 * @param t The thread ID to check.
 * @return True if the thread ID matches the calling thread's ID, false
 * otherwise.
 */
bool tid_is_self(thread_id t);

/**
 * @brief Returns true if the thread ID is assigned to a thread.
 * @param t The thread ID to check.
 * @return True if the thread ID is assigned to a thread, false otherwise.
 */
bool tid_is_assigned(thread_id t);

/**
 * @brief Assigns a thread ID to the calling thread.
 * @param t The thread id to assign.
 * @return
 * - 0 on success.
 * - @ref DESCENT_ERROR_INIT if the calling thread already has a thread ID
 *   assigned.
 * - @ref DESCENT_ERROR_STATE if the requested ID is already assigned to
 *   another thread.
 * - @ref DESCENT_ERROR_INVALID if the requested id is TID_NONE.
 */
rcode tid_assign(thread_id t);

/**
 * @brief Assigns the main thread ID to the calling thread.
 * @return
 * - 0 on success.
 * - @ref DESCENT_ERROR_INIT if the calling thread already has a thread ID
 *   assigned.
 * - @ref DESCENT_ERROR_STATE if the requested ID is already assigned to
 *   another thread.
 */
rcode tid_assign_main(void);

/**
 * @brief Assigns a unique thread ID to the calling thread.
 * @param index Zero-based unique thread index.
 * @return
 * - 0 on success.
 * - @ref DESCENT_ERROR_INIT if the calling thread already has a thread ID
 *   assigned.
 * - @ref DESCENT_ERROR_STATE if the requested ID is already assigned to
 *   another thread.
 * - @ref DESCENT_ERROR_INVALID if the index is out of range. (>=
 *   DESCENT_UNIQUE_THREAD_COUNT_MAX).
 */
rcode tid_assign_unique(unsigned int index);

/**
 * @brief Assigns a worker thread ID to the calling thread.
 * @param index Zero-based worker thread index.
 * @return
 * - 0 on success.
 * - @ref DESCENT_ERROR_INIT if the calling thread already has a thread ID
 *   assigned.
 * - @ref DESCENT_ERROR_STATE if the requested ID is already assigned to
 *   another thread.
 * - @ref DESCENT_ERROR_INVALID if the index is out of range (>=
 *   DESCENT_WORKER_THREAD_COUNT_MAX).
 */
rcode tid_assign_worker(unsigned int index);

/**
 * @brief Clears the calling thread's ID.
 */
void tid_assign_clear(void);

/**
 * @brief Adds a managed thread ID to a thread ID set.
 * @param s The thread ID set to modify.
 * @param t The thread ID to add.
 * @return The updated thread ID set.
 */
static inline thread_id_set tid_set_add(thread_id_set s, thread_id t) {
	return tid_is_managed(t) ? (s | t) : s;
}

/**
 * @brief Removes a managed thread ID from a thread ID set.
 * @param s The thread ID set to modify.
 * @param t The thread ID to remove.
 * @return The updated thread ID set.
 */
static inline thread_id_set tid_set_remove(thread_id_set s, thread_id t) {
	return tid_is_managed(t) ? (s & ~t) : s;
}

/**
 * @brief Creates a union of two thread ID sets.
 * @param s1 The first thread ID set to join.
 * @param s2 The second thread ID set to join.
 * @return The joined thread ID set.
 */
static inline thread_id_set tid_set_union(thread_id_set s1, thread_id_set s2) {
	return s1 | s2;
}

/**
 * @brief Checks if a thread ID set contains the specified thread ID.
 * @param s The thread ID set to check.
 * @param t The thread ID to check.
 * @return True if the thread ID set contains the thread ID, false otherwise.
 */
static inline bool tid_set_contains(thread_id_set s, thread_id t) {
	return tid_is_managed(t) && (s & t);
}

/**
 * @brief Checks if two thread ID sets share any thread IDs.
 * @param s1 The first thread ID set to check.
 * @param s2 The second thread ID set to check.
 * @return True if the two thread ID sets share any thread IDs, false
 * otherwise.
 */
static inline bool tid_set_intersects(thread_id_set s1, thread_id_set s2) {
	return s1 & s2;
}

/**
 * @brief Returns true if the thread ID set contains only managed thread IDs
 * (main, unique, or worker).
 * @param s The thread ID set to check.
 * @return True if the thread ID set contains only managed thread IDs, false
 * otherwise.
 */
static inline bool tid_set_is_managed(thread_id_set s) {
	return !(s & ~(TID_MAIN | TID_UNIQUE_MASK | TID_WORKER_MASK));
}

/**
 * @brief Checks if the thread ID set is empty.
 * @param s The thread ID set to check.
 * @return True if the thread ID set is empty, false otherwise.
 */
static inline bool tid_set_is_empty(thread_id_set s) {
	return !(s & (TID_MAIN | TID_UNIQUE_MASK | TID_WORKER_MASK));
}

/**
 * @brief Checks if the calling thread is included in a thread ID set.
 * @param s The thread ID set to check.
 * @return True if the thread ID set contains the calling thread's ID, false
 * otherwise.
 */
bool tid_set_contains_self(thread_id_set s);

#undef TID_WORKER_MASK
#undef TID_UNIQUE_MASK
#undef TID_WORKER_INDEX_MAX
#undef TID_WORKER_INDEX_MIN
#undef TID_UNIQUE_INDEX_MAX
#undef TID_UNIQUE_INDEX_MIN
#undef BIT_RANGE_MASK

#endif
