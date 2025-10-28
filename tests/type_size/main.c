#include <stdio.h>
#include <descent/utilities/platform.h>
#include <descent/utilities/opaque.h>
#include "../../src/thread/implementation.h"

static void print_abi(void) {
#if defined(DESCENT_PLATFORM_ABI_GLIBC_32)
	puts("ABI: DESCENT_PLATFORM_ABI_GLIBC_32");
#elif defined(DESCENT_PLATFORM_ABI_GLIBC_64)
	puts("ABI: DESCENT_PLATFORM_ABI_GLIBC_64");
#elif defined(DESCENT_PLATFORM_ABI_MUSL_32)
	puts("ABI: DESCENT_PLATFORM_ABI_MUSL_32");
#elif defined(DESCENT_PLATFORM_ABI_MUSL_64)
	puts("ABI: DESCENT_PLATFORM_ABI_MUSL_64");
#elif defined(DESCENT_PLATFORM_ABI_FREEBSD_32)
	puts("ABI: DESCENT_PLATFORM_ABI_FREEBSD_32");
#elif defined(DESCENT_PLATFORM_ABI_FREEBSD_64)
	puts("ABI: DESCENT_PLATFORM_ABI_FREEBSD_64");
#elif defined(DESCENT_PLATFORM_ABI_MACOS_32)
	puts("ABI: DESCENT_PLATFORM_ABI_MACOS_32");
#elif defined(DESCENT_PLATFORM_ABI_MACOS_64)
	puts("ABI: DESCENT_PLATFORM_ABI_MACOS_64");
#elif defined(DESCENT_PLATFORM_ABI_CYGWIN_32)
	puts("ABI: DESCENT_PLATFORM_ABI_CYGWIN_32");
#elif defined(DESCENT_PLATFORM_ABI_CYGWIN_64)
	puts("ABI: DESCENT_PLATFORM_ABI_CYGWIN_64");
#elif defined(DESCENT_PLATFORM_ABI_WINDOWS_32)
	puts("ABI: DESCENT_PLATFORM_ABI_WINDOWS_32");
#elif defined(DESCENT_PLATFORM_ABI_WINDOWS_64)
	puts("ABI: DESCENT_PLATFORM_ABI_WINDOWS_64");
#else
	puts("ABI: UNKNOWN");
#endif
	fflush(stdout);
}

int main(void) {
	print_abi();

	printf("Barrier:         o:%02u i:%02zu d:%02zu\n", DESCENT_OPAQUE_SIZE_BARRIER,         sizeof(BarrierImplementation),        DESCENT_OPAQUE_SIZE_BARRIER         - sizeof(BarrierImplementation));
	printf("Call-Once:       o:%02u i:%02zu d:%02zu\n", DESCENT_OPAQUE_SIZE_CALL_ONCE,       sizeof(CallOnceImplementation),       DESCENT_OPAQUE_SIZE_CALL_ONCE       - sizeof(CallOnceImplementation));
	printf("Condition:       o:%02u i:%02zu d:%02zu\n", DESCENT_OPAQUE_SIZE_CONDITION,       sizeof(ConditionImplementation),      DESCENT_OPAQUE_SIZE_CONDITION       - sizeof(ConditionImplementation));
	printf("Mutex:           o:%02u i:%02zu d:%02zu\n", DESCENT_OPAQUE_SIZE_MUTEX,           sizeof(MutexImplementation),          DESCENT_OPAQUE_SIZE_MUTEX           - sizeof(MutexImplementation));
	printf("Recursive_Mutex: o:%02u i:%02zu d:%02zu\n", DESCENT_OPAQUE_SIZE_RECURSIVE_MUTEX, sizeof(RecursiveMutexImplementation), DESCENT_OPAQUE_SIZE_RECURSIVE_MUTEX - sizeof(RecursiveMutexImplementation));
	printf("Rwlock:          o:%02u i:%02zu d:%02zu\n", DESCENT_OPAQUE_SIZE_RWLOCK,          sizeof(RWLockImplementation),         DESCENT_OPAQUE_SIZE_RWLOCK          - sizeof(RWLockImplementation));
	printf("Semaphore:       o:%02u i:%02zu d:%02zu\n", DESCENT_OPAQUE_SIZE_SEMAPHORE,       sizeof(SemaphoreImplementation),      DESCENT_OPAQUE_SIZE_SEMAPHORE       - sizeof(SemaphoreImplementation));
	fflush(stdout);

	if (DESCENT_OPAQUE_SIZE_BARRIER != sizeof(BarrierImplementation)) return 1;
	if (DESCENT_OPAQUE_SIZE_CALL_ONCE != sizeof(CallOnceImplementation)) return 1;
	if (DESCENT_OPAQUE_SIZE_CONDITION != sizeof(ConditionImplementation)) return 1;
	if (DESCENT_OPAQUE_SIZE_MUTEX != sizeof(MutexImplementation)) return 1;
	if (DESCENT_OPAQUE_SIZE_RECURSIVE_MUTEX != sizeof(RecursiveMutexImplementation)) return 1;
	if (DESCENT_OPAQUE_SIZE_RWLOCK != sizeof(RWLockImplementation)) return 1;
	if (DESCENT_OPAQUE_SIZE_SEMAPHORE != sizeof(SemaphoreImplementation)) return 1;

	return 0;
}
