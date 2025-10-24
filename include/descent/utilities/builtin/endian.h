#ifndef DESCENT_UTILITIES_BUILTIN_ENDIAN_H
#define DESCENT_UTILITIES_BUILTIN_ENDIAN_H

#include <stdint.h>

#include <descent/utilities/builtin/api.h>
#include <descent/utilities/builtin/bits.h>

#ifdef __cplusplus
namespace descent::builtin {
#endif

// Determines whether the platform is little-endian
DESCENT_BUILTIN_API int
is_little_endian() {
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)
	return __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__;
#elif defined(_WIN32) || defined(__LITTLE_ENDIAN__) || defined(__ARMEL__) || defined(_MIPSEL)
	return 1;
#elif defined(__BIG_ENDIAN__) || defined(__ARMEB__) || defined(_MIPSEB)
	return 0;
#else
	DESCENT_BUILTIN_CONST union {
		uint32_t i;
		uint8_t c[4];
	} TEST = { 0x01020304 };

	return TEST.c[0] == 0x04;
#endif
}

DESCENT_BUILTIN_API uint64_t
htole_64(uint64_t v) { return is_little_endian() ? v : byteswap_64(v); }

DESCENT_BUILTIN_API uint64_t
letoh_64(uint64_t v) { return is_little_endian() ? v : byteswap_64(v); }

DESCENT_BUILTIN_API uint64_t
htobe_64(uint64_t v) { return is_little_endian() ? byteswap_64(v) : v; }

DESCENT_BUILTIN_API uint64_t
betoh_64(uint64_t v) { return is_little_endian() ? byteswap_64(v) : v; }

DESCENT_BUILTIN_API uint32_t
htole_32(uint32_t v) { return is_little_endian() ? v : byteswap_32(v); }

DESCENT_BUILTIN_API uint32_t
letoh_32(uint32_t v) { return is_little_endian() ? v : byteswap_32(v); }

DESCENT_BUILTIN_API uint32_t
htobe_32(uint32_t v) { return is_little_endian() ? byteswap_32(v) : v; }

DESCENT_BUILTIN_API uint32_t
betoh_32(uint32_t v) { return is_little_endian() ? byteswap_32(v) : v; }

DESCENT_BUILTIN_API uint16_t
htole_16(uint16_t v) { return is_little_endian() ? v : byteswap_16(v); }

DESCENT_BUILTIN_API uint16_t
letoh_16(uint16_t v) { return is_little_endian() ? v : byteswap_16(v); }

DESCENT_BUILTIN_API uint16_t
htobe_16(uint16_t v) { return is_little_endian() ? byteswap_16(v) : v; }

DESCENT_BUILTIN_API uint16_t
betoh_16(uint16_t v) { return is_little_endian() ? byteswap_16(v) : v; }

DESCENT_BUILTIN_API uint8_t
htole_8(uint8_t v) { return v; }

DESCENT_BUILTIN_API uint8_t
letoh_8(uint8_t v) { return v; }

DESCENT_BUILTIN_API uint8_t
htobe_8(uint8_t v) { return v; }

DESCENT_BUILTIN_API uint8_t
betoh_8(uint8_t v) { return v; }

#ifdef __cplusplus
}
#endif

#endif
