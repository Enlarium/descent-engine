#include <descent/alloc/sysalloc.h>

#include <stddef.h>
#include <stdint.h>

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <stdio.h>
#include <strings.h>
#include <sys/mman.h>
#include <unistd.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#include <windows.h>
#endif

#define DESCENT_MAX_ALLOC  0x10000000000ULL
#define SUBALLOC_BLOCK_SIZE 64

#if defined(DESCENT_PLATFORM_TYPE_POSIX) 
#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif
#endif

// TODO: Swapping to disk

size_t block_size() {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)

	// This function call can be assumed to never fail, and page size is always a power of two
	return (size_t) sysconf(_SC_PAGESIZE);

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

	// This function call will never fail, and page size is always a power of two
	SYSTEM_INFO si;
	GetNativeSystemInfo(&si);

	// Use allocation granularity instead of page size
	return (size_t) si.dwAllocationGranularity;

#endif
}

Alloc sysalloc(size_t size) {
	Alloc result = {};
	
	if (size > DESCENT_MAX_ALLOC) return result;

	// Request full-granularity allocations
	size_t min_size = block_size();

	// Round up the size to fill out memory pages
	result.size = (size + min_size - 1) & ~(min_size - 1);

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

	int prot  = PROT_READ | PROT_WRITE;
	int flags = MAP_PRIVATE | MAP_ANONYMOUS;

	result.alloc = mmap(NULL, result.size, prot, flags, -1, 0);

	if (result.alloc == MAP_FAILED) {
		result.alloc = NULL;
		result.size = 0;
	}

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

	int prot = PAGE_READWRITE;
	int flags = MEM_COMMIT | MEM_RESERVE;

	result.alloc = VirtualAlloc(NULL, result.size, flags, prot);

	if (!result.alloc) {
		result.alloc = NULL;
		result.size = 0;
	}

#endif

	return result;
}

int sysfree(Alloc *a) {
	// return negative one if a is null
	int result = -!a;

	if (a && a->alloc) {

#if defined(DESCENT_PLATFORM_TYPE_POSIX)

		result = munmap(a->alloc, a->size);

#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)

		result =  VirtualFree(a->alloc, 0, MEM_RELEASE) ? 0 : -1;

#endif

		a->alloc = NULL;
		a->size = 0;
	}

	return result;
}

// Align suballocations to 64 bytes
