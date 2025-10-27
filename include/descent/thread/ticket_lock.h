#ifndef DESCENT_THREAD_TICKET_LOCK_H
#define DESCENT_THREAD_TICKET_LOCK_H



#include "atomic.h"
#include "thread.h"



#ifdef __cplusplus
extern "C" {
#endif



#define TICKET_LOCK_INITIALIZER { 0, 0 }



/**
 * @struct TicketLock
 * @brief A ticket lock for synchronizing access to shared resources in a fair, FIFO manner.
 * 
 * @note This mechanism is intra-process only. It cannot be shared between processes.
 */
typedef struct {
	atomic_32 _next;
	atomic_32 _current;
} TicketLock;



/**
 * @brief Initialize a ticket lock.
 * @param m Pointer to the TicketLock.
 */
static inline void ticketlock_init(TicketLock *l) {
	atomic_init_32(&l->_next, 0);
	atomic_init_32(&l->_current, 0);
}

/**
 * @brief Lock a ticket lock.
 * @param m Pointer to the TicketLock.
 *
 * Busy-waits until the mutex is acquired.
 */
static inline void ticketlock_lock(TicketLock *l) {
	uint32_t my_ticket = atomic_fetch_add_32(&l->_next, 1);
	while (atomic_load_32(&l->_current) != my_ticket) {
		thread_pause();
	}
}

/**
 * @brief Attempt to lock a ticket lock without blocking.
 * @param m Pointer to the TicketLock.
 * @return 0 if the lock was acquired, non-zero if the ticket lock is already held by another thread.
 */
static inline int ticketlock_trylock(TicketLock *l) {
	uint32_t current_ticket = atomic_load_32(&l->_next);
	uint32_t serving = atomic_load_32(&l->_current);

	if (current_ticket != serving) return 1;

	uint32_t expected = current_ticket;
	return !atomic_compare_exchange_32(&l->_next, &expected, current_ticket + 1);
}

/**
 * @brief Unlock a ticket lock.
 * @param m Pointer to the TicketLock to unlock.
 * @return 0 on success, non-zero on failure.
 */
static inline void ticketlock_unlock(TicketLock *l) {
	atomic_fetch_add_32(&l->_current, 1);
}



#ifdef __cplusplus
}
#endif



#endif