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

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_LINUX)
#define _GNU_SOURCE
#endif

#include <descent/alloc/sysalloc.h>

#include <stddef.h>
#include <stdint.h>

#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <errno.h>
#include <sys/mman.h>
#include <sys/mman.h>
#include <unistd.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <descent/rcode.h>
#include <descent/utilities/macros.h>
#include <descent/thread/call_once.h>

#if defined(DESCENT_PLATFORM_TYPE_POSIX) 
#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif
#endif

static size_t granularity = 0;
static struct CallOnce granularity_call_once = {0};

static inline rcode sysalloc_access_to_native(int *access) {
	if (!access) return DESCENT_ERROR_NULL;

	int request = *access;
	int require = (request & ~SYSALLOC_ACCESS_READ_WRITE_EXEC) ? SYSALLOC_ACCESS_NONE : request;

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

	int native = PROT_NONE;

	if (require & SYSALLOC_ACCESS_READ)  native |= PROT_READ;
	if (require & SYSALLOC_ACCESS_WRITE) native |= PROT_WRITE;
	if (require & SYSALLOC_ACCESS_EXEC)  native |= PROT_EXEC;

	*access = native;

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

	switch (require) {
		case SYSALLOC_ACCESS_READ_WRITE_EXEC:
			*access = PAGE_EXECUTE_READWRITE;
			break;
		case SYSALLOC_ACCESS_READ_WRITE:
			*access = PAGE_READWRITE;
			break;
		case SYSALLOC_ACCESS_READ_EXEC:
			*access = PAGE_EXECUTE_READ;
			break;
		case SYSALLOC_ACCESS_WRITE_EXEC:
			*access = PAGE_EXECUTE_READWRITE; // Windows does not support write+exec without read
			break;
		case SYSALLOC_ACCESS_READ:
			*access = PAGE_READONLY;
			break;
		case SYSALLOC_ACCESS_WRITE:
			*access = PAGE_READWRITE; // Windows does not support write-only
			break;
		case SYSALLOC_ACCESS_EXEC:
			*access = PAGE_EXECUTE;
			break;
		default:
			*access = PAGE_NOACCESS;
			break;
	}

#endif

	return (request & ~SYSALLOC_ACCESS_READ_WRITE_EXEC) ? ALLOCATOR_ERROR_ALLOC : 0;
}

static inline rcode sysalloc_round_size(size_t *size) {
	if (!size) return DESCENT_ERROR_NULL;

	size_t requested_size = *size;
	*size = 0;

	if (!requested_size)
		return ALLOCATOR_ERROR_ALLOC;
	if (requested_size > DESCENT_MAX_ALLOC)
		return DESCENT_ERROR_OVERFLOW;
	
	// Use full-granularity allocations
	size_t size_granularity = sysalloc_granularity();
	if (!size_granularity)
		return DESCENT_ERROR_OS;

	// Round up the size to fill out memory pages
	*size = (requested_size + size_granularity - 1) & ~(size_granularity - 1);
	return 0;
}

static inline rcode sysalloc_internal(void **map, size_t *size, int access, int commit) {
	if (!map || !size) return DESCENT_ERROR_NULL;

	if (!commit) access = SYSALLOC_ACCESS_NONE;

	rcode result = sysalloc_round_size(size);
	if (result) return result;

	result = sysalloc_access_to_native(&access);
	if (result) return result;

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

	*map = mmap(NULL, *size, access, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (*map == MAP_FAILED) {
		switch (errno) {
			case ENOMEM:
			case EAGAIN:
			case EOVERFLOW:
				return DESCENT_ERROR_MEMORY;
			case EINVAL:
				return ALLOCATOR_ERROR_ALLOC;
			default:
				return DESCENT_ERROR_OS;
		}
	}

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

	DWORD type = MEM_RESERVE;
	if (commit) type |= MEM_COMMIT;

	*map = VirtualAlloc(NULL, *size, type, access);
	if (!*map) {
		switch (GetLastError()) {
			case ERROR_NOT_ENOUGH_MEMORY:
				return DESCENT_ERROR_MEMORY;
			case ERROR_INVALID_PARAMETER:
				return ALLOCATOR_ERROR_ALLOC;
			default:
				return DESCENT_ERROR_OS;
		}
	}

#endif

	return 0;
}

// Allocation granularity is always a power of two
static void init_sysalloc_granularity(void) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
	long page_size = sysconf(_SC_PAGESIZE);
	// Failure is exceptionally unlikely, but we will signal it with 0-size
	if (page_size < 0) page_size = 0;
	granularity = (size_t) page_size;
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
	SYSTEM_INFO si;
	GetNativeSystemInfo(&si); // This call will never fail
	granularity = (size_t) si.dwAllocationGranularity;
#endif
}

// Allocation granularity cannot change during runtime
size_t sysalloc_granularity(void) {
	call_once(&granularity_call_once, init_sysalloc_granularity);
	return granularity;
}

rcode sysalloc(Sysalloc *s, int access) {
	if (!s) return DESCENT_ERROR_NULL;

	void *map = s->base;
	size_t map_size = s->size;

	// Clear inputs in case of error
	s->base = NULL;
	s->size = 0;

	rcode result = sysalloc_internal(&map, &map_size, access, 1);
	if (result) return result;

	s->base = map;
	s->size = map_size;

	return 0;
}

rcode sysalloc_reserve(Sysalloc *s) {
	if (!s) return DESCENT_ERROR_NULL;

	void *map = s->base;
	size_t map_size = s->size;

	// Clear inputs in case of error
	s->base = NULL;
	s->size = 0;

	rcode result = sysalloc_internal(&map, &map_size, SYSALLOC_ACCESS_NONE, 0);
	if (result) return result;

	s->base = map;
	s->size = map_size;

	return 0;
}

rcode sysalloc_commit(Sysalloc *s, size_t offset, size_t size, int access) {
	if (!s) return DESCENT_ERROR_NULL;
	if (!s->base || !s->size) return ALLOCATOR_ERROR_ALLOC;
	if (!size) return DESCENT_ERROR_INVALID;

	size_t size_granularity = sysalloc_granularity();

	// Check that the allocation's base and size are valid
	if ((uintptr_t)s->base % size_granularity || s->size % size_granularity) {
		return ALLOCATOR_ERROR_ALLOC;
	}

	// Check that the requested offset and size are valid
	if (offset % size_granularity || size % size_granularity) {
		return DESCENT_ERROR_INVALID;
	}
	
	// Check that offset and size fit within allocation
	if (offset > SIZE_MAX - size || offset + size > s->size) {
		return ALLOCATOR_ERROR_ALLOC;
	}

	rcode result = sysalloc_access_to_native(&access);
	if (result) return result;

	void *region = POINTER_OFFSET(void, s->base, offset);

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

	if (mprotect(region, size, access)) {
		switch (errno) {
			case EACCES:
				return DESCENT_ERROR_FORBIDDEN;
			case EINVAL:
				return ALLOCATOR_ERROR_ALLOC;
			case ENOMEM:
				return DESCENT_ERROR_MEMORY;
			default:
				return DESCENT_ERROR_OS;
		}
	}

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

	if (!VirtualAlloc(region, size, MEM_COMMIT, access)) {
		switch (GetLastError()) {
			case ERROR_NOT_ENOUGH_MEMORY:
				return DESCENT_ERROR_MEMORY;
			case ERROR_INVALID_PARAMETER:
				return ALLOCATOR_ERROR_ALLOC;
			default:
				return DESCENT_ERROR_OS;
		}
	}

#endif

	return 0;
}

rcode sysalloc_decommit(Sysalloc *s, size_t offset, size_t size) {
	if (!s) return DESCENT_ERROR_NULL;
	if (!s->base || !s->size) return ALLOCATOR_ERROR_ALLOC;
	if (!size) return DESCENT_ERROR_INVALID;

	size_t size_granularity = sysalloc_granularity();

	// Check that the allocation's base and size are valid
	if ((uintptr_t)s->base % size_granularity || s->size % size_granularity) {
		return ALLOCATOR_ERROR_ALLOC;
	}

	// Check that the requested offset and size are valid
	if (offset % size_granularity || size % size_granularity) {
		return DESCENT_ERROR_INVALID;
	}
	
	// Check that offset and size fit within allocation
	if (offset > SIZE_MAX - size || offset + size > s->size) {
		return ALLOCATOR_ERROR_ALLOC;
	}

	void *region = POINTER_OFFSET(void, s->base, offset);

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

	if (mprotect(region, size, PROT_NONE)) {
		switch (errno) {
			case EACCES:
				return DESCENT_ERROR_FORBIDDEN;
			case EINVAL:
				return ALLOCATOR_ERROR_ALLOC;
			case ENOMEM:
				return DESCENT_ERROR_MEMORY;
			default:
				return DESCENT_ERROR_OS;
		}
	}

	if (madvise(region, size, MADV_DONTNEED)) {
		switch (errno) {
			case EINVAL:
				return ALLOCATOR_ERROR_ALLOC;
			case ENOMEM:
				return DESCENT_ERROR_MEMORY;
			default:
				return DESCENT_ERROR_OS;
		}
	}

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

	if (!VirtualFree(region, size, MEM_DECOMMIT)) {
		switch (GetLastError()) {
			case ERROR_NOT_ENOUGH_MEMORY:
				return DESCENT_ERROR_MEMORY;
			case ERROR_INVALID_PARAMETER:
				return ALLOCATOR_ERROR_ALLOC;
			default:
				return DESCENT_ERROR_OS;
		}
	}

#endif

	return 0;
}

rcode sysfree(Sysalloc *s) {
	if (!s) return DESCENT_ERROR_NULL;
	if (!s->base || !s->size) return ALLOCATOR_ERROR_FREE;

	// Check that base and size correspond to granularity

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

	if (munmap(s->base, s->size)) {
		switch (errno) {
			case EINVAL:
			case EFAULT:
				return ALLOCATOR_ERROR_FREE;
			default:
				return DESCENT_ERROR_OS;
		}
	}

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

	if (!VirtualFree(s->base, 0, MEM_RELEASE)) {
		switch (GetLastError()) {
			case ERROR_INVALID_PARAMETER:
				return ALLOCATOR_ERROR_FREE;
			default:
				return DESCENT_ERROR_OS;
		}
	}

#endif

	s->base = NULL;
	s->size = 0;

	return 0;
}

// Align suballocations to 64 bytes

// The master allocator owns all dynamic memory for the program
// It is initialized once and cannot be resized. (maybe have a growable dynamic section)
// Module suballocators

// typedef struct {
// 
// } RenderLayout;

// typedef struct {
// 	size_t
// } AllocationLayout;

// void *program_space;
// void *


// TODO: Swapping to disk
