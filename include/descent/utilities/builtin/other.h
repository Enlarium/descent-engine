#ifndef DESCENT_UTILITIES_BUILTIN_OTHER_H
#define DESCENT_UTILITIES_BUILTIN_OTHER_H

#include <descent/utilities/builtin/api.h>

#ifndef unreachable
#if DESCENT_HAS_BUILTIN(__builtin_unreachable)
#define unreachable() __builtin_unreachable()
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC)
#define unreachable() __assume(0)
#else
#define unreachable() (void) 0
#endif
#endif

#endif
