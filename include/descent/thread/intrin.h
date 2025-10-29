#ifndef DESCENT_THREAD_INTRIN_H
#define DESCENT_THREAD_INTRIN_H

/**
 * @brief Provide a CPU pause hint. Does not yield execution.
 */
static inline void thread_pause(void) {
#if defined(DESCENT_PLATFORM_COMPILER_TYPE_GCC) || defined(DESCENT_PLATFORM_COMPILER_TYPE_CLANG)
#if defined(DESCENT_PLATFORM_ARCHITECTURE_FAMILY_X86)
	__builtin_ia32_pause();
#elif defined(DESCENT_PLATFORM_ARCHITECTURE_FAMILY_ARM)
	__asm__ __volatile__("yield" ::: "memory");
#endif
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC)
#if defined(DESCENT_PLATFORM_ARCHITECTURE_FAMILY_X86)
	_mm_pause();
#elif defined(DESCENT_PLATFORM_ARCHITECTURE_FAMILY_ARM)
	__yield();
#endif
#else 
#error "what"
#endif
}

#endif
