#ifndef DESCENT_UTILITIES_BUILTIN_BITS_H
#define DESCENT_UTILITIES_BUILTIN_BITS_H

#include <stdbool.h>
#include <stdint.h>

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_COMPILER_MSVC)
#include <intrin.h>
#endif

#include <descent/utilities/builtin/api.h>

#ifdef __cplusplus
namespace descent::builtin {
#endif

// Cyclic Shifts

DESCENT_BUILTIN_API uint64_t
rotl_64(uint64_t v, unsigned int count) {
	DESCENT_BUILTIN_CONST unsigned int MASK = 63;
	count &= MASK;
#if DESCENT_HAS_BUILTIN(__builtin_rotateleft64)
	return __builtin_rotateleft64(v, count);
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC) && DESCENT_PLATFORM_ARCHITECTURE_SIZE == 64
	return _rotl64(v, count);
#else
	return (v << count) | (v >> (-count & MASK));
#endif
}

DESCENT_BUILTIN_API uint32_t
rotl_32(uint32_t v, unsigned int count) {
	DESCENT_BUILTIN_CONST unsigned int MASK = 31;
	count &= MASK;
#if DESCENT_HAS_BUILTIN(__builtin_rotateleft32)
	return __builtin_rotateleft32(v, count);
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC)
	return _rotl32(v, count);
#else
	return (v << count) | (v >> (-count & MASK));
#endif
}

DESCENT_BUILTIN_API uint16_t
rotl_16(uint16_t v, unsigned int count) {
	DESCENT_BUILTIN_CONST unsigned int MASK = 15;
	count &= MASK;
#if DESCENT_HAS_BUILTIN(__builtin_rotateleft16)
	return __builtin_rotateleft16(v, (unsigned short) count);
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC)
	return _rotl16(v, count);
#else
	return (v << count) | (v >> (-count & MASK));
#endif
}

DESCENT_BUILTIN_API uint8_t
rotl_8(uint8_t v, unsigned int count) {
	DESCENT_BUILTIN_CONST unsigned int MASK = 7;
	count &= MASK;
#if DESCENT_HAS_BUILTIN(__builtin_rotateleft8)
	return __builtin_rotateleft8(v, (unsigned char) count);
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC)
	return _rotl8(v, count);
#else
	return (v << count) | (v >> (-count & MASK));
#endif
}

DESCENT_BUILTIN_API uint64_t
rotr_64(uint64_t v, unsigned int count) {
#if DESCENT_HAS_BUILTIN(__builtin_rotateright64)
	return __builtin_rotateright64(v, count);
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC) && DESCENT_PLATFORM_ARCHITECTURE_SIZE == 64
	return _rotr64(v, count);
#else
	DESCENT_BUILTIN_CONST unsigned int MASK = 63;
	count &= MASK;
	return (v >> count) | (v << (-count & MASK));
#endif
}

DESCENT_BUILTIN_API uint32_t
rotr_32(uint32_t v, unsigned int count) {
#if DESCENT_HAS_BUILTIN(__builtin_rotateright32)
	return __builtin_rotateright32(v, count);
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC)
	return _rotr32(v, count);
#else
	DESCENT_BUILTIN_CONST unsigned int MASK = 31;
	count &= MASK;
	return (v >> count) | (v << (-count & MASK));
#endif
}

DESCENT_BUILTIN_API uint16_t
rotr_16(uint16_t v, unsigned int count) {
#if DESCENT_HAS_BUILTIN(__builtin_rotateright16)
	return __builtin_rotateright16(v, (unsigned short) count);
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC)
	return _rotr16(v, count);
#else
	DESCENT_BUILTIN_CONST unsigned int MASK = 15;
	count &= MASK;
	return (v >> count) | (v << (-count & MASK));
#endif
}

DESCENT_BUILTIN_API uint8_t
rotr_8(uint8_t v, unsigned int count) {
#if DESCENT_HAS_BUILTIN(__builtin_rotateright8)
	return __builtin_rotateright8(v, (unsigned char) count);
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC)
	return _rotr8(v, count);
#else
	DESCENT_BUILTIN_CONST unsigned int MASK = 7;
	count &= MASK;
	return (v >> count) | (v << (-count & MASK));
#endif
}

// Bit Reversal

DESCENT_BUILTIN_API uint64_t
bitreverse_64(uint64_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_bitreverse64)
	return __builtin_bitreverse64(v);
#else
	v = ((v & 0xAAAAAAAAAAAAAAAAull) >> 1)  | ((v & 0x5555555555555555ull) << 1);
	v = ((v & 0xCCCCCCCCCCCCCCCCull) >> 2)  | ((v & 0x3333333333333333ull) << 2);
	v = ((v & 0xF0F0F0F0F0F0F0F0ull) >> 4)  | ((v & 0x0F0F0F0F0F0F0F0Full) << 4);
	v = ((v & 0xFF00FF00FF00FF00ull) >> 8)  | ((v & 0x00FF00FF00FF00FFull) << 8);
	v = ((v & 0xFFFF0000FFFF0000ull) >> 16) | ((v & 0x0000FFFF0000FFFFull) << 16);
	return (v >> 32) | (v << 32);
#endif
}

DESCENT_BUILTIN_API uint32_t
bitreverse_32(uint32_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_bitreverse32)
	return __builtin_bitreverse32(v);
#else
	v = (((v & 0xAAAAAAAAu) >> 1) | ((v & 0x55555555u) << 1));
	v = (((v & 0xCCCCCCCCu) >> 2) | ((v & 0x33333333u) << 2));
	v = (((v & 0xF0F0F0F0u) >> 4) | ((v & 0x0F0F0F0Fu) << 4));
	v = (((v & 0xFF00FF00u) >> 8) | ((v & 0x00FF00FFu) << 8));
	return((v >> 16) | (v << 16));
#endif
}

DESCENT_BUILTIN_API uint16_t
bitreverse_16(uint16_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_bitreverse16)
	return __builtin_bitreverse16(v);
#else
	v = (((v & 0xAAAAu) >> 1) | ((v & 0x5555u) << 1));
	v = (((v & 0xCCCCu) >> 2) | ((v & 0x3333u) << 2));
	v = (((v & 0xF0F0u) >> 4) | ((v & 0x0F0Fu) << 4));
	return((v >> 8) | (v << 8));
#endif
}

DESCENT_BUILTIN_API uint8_t
bitreverse_8(uint8_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_bitreverse8)
	return __builtin_bitreverse8(v);
#else
	v = (((v & 0xAAu) >> 1) | ((v & 0x55u) << 1));
	v = (((v & 0xCCu) >> 2) | ((v & 0x33u) << 2));
	return((v >> 4) | (v << 4));
#endif
}

// Byte Swapping

DESCENT_BUILTIN_API uint64_t
byteswap_64(uint64_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_bswap64)
	return __builtin_bswap64(v);
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC)
	return _byteswap_uint64(v);
#else
	return
		((v & 0x00000000000000FFull) << 56) |
		((v & 0x000000000000FF00ull) << 40) |
		((v & 0x0000000000FF0000ull) << 24) |
		((v & 0x00000000FF000000ull) << 8 ) |
		((v & 0x000000FF00000000ull) >> 8 ) |
		((v & 0x0000FF0000000000ull) >> 24) |
		((v & 0x00FF000000000000ull) >> 40) |
		((v & 0xFF00000000000000ull) >> 56);
#endif
}

DESCENT_BUILTIN_API uint32_t
byteswap_32(uint32_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_bswap32)
	return __builtin_bswap32(v);
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC)
	return _byteswap_ulong(v);
#else
	return
		((v & 0x000000FFu) << 24) |
		((v & 0x0000FF00u) << 8 ) |
		((v & 0x00FF0000u) >> 8 ) |
		((v & 0xFF000000u) >> 24);
#endif
}

DESCENT_BUILTIN_API uint16_t
byteswap_16(uint16_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_bswap16)
	return __builtin_bswap16(v);
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC)
	return _byteswap_ushort(v);
#else
	return
		((v & 0x00FFU) << 8 ) |
		((v & 0xFF00U) >> 8 );
#endif
}

// Population Count

DESCENT_BUILTIN_API int
popcount_64(uint64_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_popcountll)
	return __builtin_popcountll(v);
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC) && DESCENT_PLATFORM_ARCHITECTURE_SIZE == 64
	return (int) (__popcnt((uint32_t) v) + __popcnt((uint32_t) (v >> 32)));
#else
	v = v - ((v >> 1) & 0x5555555555555555ULL);
	v = (v & 0x3333333333333333ULL) + ((v >> 2) & 0x3333333333333333ULL);
	v = (v + (v >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
	v = v + (v >> 8);
	v = v + (v >> 16);
	v = v + (v >> 32);
	return v & 0x7F;
#endif
}

DESCENT_BUILTIN_API int
popcount_32(uint32_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_popcount)
	return __builtin_popcount(v);
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC)
	return __popcnt(v);
#else
	v -= ((v >> 1) & 0x55555555);
	v = (((v >> 2) & 0x33333333) + (v & 0x33333333));
	v = (((v >> 4) + v) & 0x0F0F0F0F);
	v += (v >> 8);
	v += (v >> 16);
	return(v & 0x0000003f);
#endif
}

DESCENT_BUILTIN_API int
popcount_16(uint16_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_popcount)
	return __builtin_popcount((unsigned int) v);
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC)
	return __popcnt16(v);
#else
	v = v - ((v >> 1) & 0x5555);
	v = (v & 0x3333) + ((v >> 2) & 0x3333);
	v = (v + (v >> 4)) & 0x0F0F;
	v = v + (v >> 8);
	return v & 0x1F;
#endif
}

DESCENT_BUILTIN_API int
popcount_8(uint8_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_popcount)
	return __builtin_popcount((unsigned int) v);
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC)
	return __popcnt16((uint16_t) v);
#else
	v = v - ((v >> 1) & 0x55);
	v = (v & 0x33) + ((v >> 2) & 0x33);
	v = (v + (v >> 4)) & 0x0F;
	return v;
#endif
}

// Parity

DESCENT_BUILTIN_API int
parity_64(uint64_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_parityll)
	return __builtin_parityll(v);
#else
	return popcount_64(v) & 1;
#endif
}

DESCENT_BUILTIN_API int
parity_32(uint32_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_parity)
	return __builtin_parity(v);
#else
	return popcount_32(v) & 1;
#endif
}

DESCENT_BUILTIN_API int
parity_16(uint16_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_parity)
	return __builtin_parity((unsigned int) v);
#else
	return popcount_16(v) & 1;
#endif
}

DESCENT_BUILTIN_API int
parity_8(uint8_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_parity)
	return __builtin_parity((unsigned int) v);
#else
	return popcount_8(v) & 1;
#endif
}

// Count Leading Zeroes

DESCENT_BUILTIN_API int
clz_64(uint64_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_clzll)
	return v ? __builtin_clzll(v) : 64;
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC) && DESCENT_PLATFORM_ARCHITECTURE_SIZE == 64
	unsigned long index;
	return _BitScanReverse64(&index, v) ? 63 - index : 64;
#else
	v |= (v >> 1);
	v |= (v >> 2);
	v |= (v >> 4);
	v |= (v >> 8);
	v |= (v >> 16);
	v |= (v >> 32);
	return(64 - popcount_64(v));
#endif
}

DESCENT_BUILTIN_API int
clz_32(uint32_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_clz)
	return v ? __builtin_clz(v) : 32;
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC)
	unsigned long index;
	return _BitScanReverse(&index, v) ? 31 - index : 32;
#else
	v |= (v >> 1);
	v |= (v >> 2);
	v |= (v >> 4);
	v |= (v >> 8);
	v |= (v >> 16);
	return(32 - popcount_32(v));
#endif
}

DESCENT_BUILTIN_API int
clz_16(uint16_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_clz) || defined(DESCENT_PLATFORM_COMPILER_MSVC)
	return clz_32((uint32_t) v) - 16;
#else
	v |= (v >> 1);
	v |= (v >> 2);
	v |= (v >> 4);
	v |= (v >> 8);
	return(16 - popcount_16(v));
#endif
}

DESCENT_BUILTIN_API int
clz_8(uint8_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_clz) || defined(DESCENT_PLATFORM_COMPILER_MSVC)
	return clz_32((uint32_t) v) - 24;
#else
	v |= (v >> 1);
	v |= (v >> 2);
	v |= (v >> 4);
	return(8 - popcount_8(v));
#endif
}

// Count Trailing Zeroes

DESCENT_BUILTIN_API int
ctz_64(uint64_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_ctzll)
	return v ? __builtin_ctzll(v) : 64;
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC) && DESCENT_PLATFORM_ARCHITECTURE_SIZE == 64
	unsigned long index;
	return _BitScanForward64(&index, v) ? (int) index : 64;
#else
	DESCENT_BUILTIN_CONST uint8_t TABLE[64] = {
		0,  1,  2,  53, 3,  7,  54, 27,
		4,  38, 41, 8,  34, 55, 48, 28,
		62, 5,  39, 46, 44, 42, 22, 9,
		24, 35, 59, 56, 49, 18, 29, 11,
		63, 52, 6,  26, 37, 40, 33, 47,
		61, 45, 43, 21, 23, 58, 17, 10,
		51, 25, 36, 32, 60, 20, 57, 16,
		50, 31, 19, 15, 30, 14, 13, 12
	};

	if (v == 0) return 64;

	uint64_t isolated = v & -v;
	uint64_t index = (isolated * 0x03F79D71B4CB0A89ull) >> 58;

	return (int) TABLE[index];
#endif
}

DESCENT_BUILTIN_API int
ctz_32(uint32_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_ctz)
	return v ? __builtin_ctz(v) : 32;
#elif defined(DESCENT_PLATFORM_COMPILER_MSVC)
	unsigned long index;
	return _BitScanForward(&index, v) ? (int) index : 32;
#else
	DESCENT_BUILTIN_CONST uint8_t TABLE[32] = {
		0,  1,  28, 2,  29, 14, 24, 3,
		30, 22, 20, 15, 25, 17, 4,  8,
		31, 27, 13, 23, 21, 19, 16, 7,
		26, 12, 18, 6,  11, 5,  10, 9
	};

	if (v == 0) return 32;

	uint32_t isolated = v & -v;
	uint32_t index = (isolated * 0x077CB531U) >> 27;
	return (int) TABLE[index];
#endif
}

DESCENT_BUILTIN_API int
ctz_16(uint16_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_ctz) || defined(DESCENT_PLATFORM_COMPILER_MSVC)
	return v ? ctz_32((uint32_t)v) : 16;
#else
	DESCENT_BUILTIN_CONST uint8_t TABLE[16] = {
		0, 1,  2, 12, 3, 6, 13, 9,
		4, 11, 7, 10, 5, 8, 14, 15
	};

	if (v == 0) return 16;

	uint16_t isolated = v & -v;
	uint16_t index = (isolated * 0x0FB9U) >> 12;
	return (int) TABLE[index];
#endif
}

DESCENT_BUILTIN_API int
ctz_8(uint8_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_ctz) || defined(DESCENT_PLATFORM_COMPILER_MSVC)
	return v ? ctz_32((uint32_t)v) : 8;
#else
	DESCENT_BUILTIN_CONST uint8_t TABLE[8] = {
		0, 1, 2, 6, 3, 5, 7, 4
	};

	if (v == 0) return 8;

	uint8_t isolated = v & -v;
	uint8_t index = (isolated * 0x1DU) >> 5;
	return (int) TABLE[index];
#endif
}

// Find First Set

DESCENT_BUILTIN_API int
ffs_64(uint64_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_ffsll)
	return __builtin_ffsll((long long) v);
#else
	return v ? ctz_64(v) + 1 : 0;
#endif
}

DESCENT_BUILTIN_API int
ffs_32(uint32_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_ffs)
	return __builtin_ffs((int) v);
#else
	return v ? ctz_32(v) + 1 : 0;
#endif
}

DESCENT_BUILTIN_API int
ffs_16(uint16_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_ffs)
	return __builtin_ffs((int) v);
#else
	return v ? ctz_16(v) + 1 : 0;
#endif
}

DESCENT_BUILTIN_API int
ffs_8(uint8_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_ffs)
	return __builtin_ffs((int) v);
#else
	return v ? ctz_8(v) + 1 : 0;
#endif
}

// Count Leading Redundant Sign Bits

DESCENT_BUILTIN_API int
clrsb_64(int64_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_clrsbll)
	return __builtin_clrsbll(v);
#else
	int64_t sign = v >> 63;
	return clz_64((uint64_t) (v ^ sign)) - 1;
#endif
}

DESCENT_BUILTIN_API int
clrsb_32(int32_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_clrsb)
	return __builtin_clrsb(v);
#else
	int32_t sign = v >> 31;
	return clz_32((uint32_t) (v ^ sign)) - 1;
#endif
}

DESCENT_BUILTIN_API int
clrsb_16(int16_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_clrsb)
	return __builtin_clrsb((int32_t) v) - 16;
#else
	int32_t sign = v >> 15;
	return clz_16((uint16_t) (v ^ sign)) - 1;
#endif
}

DESCENT_BUILTIN_API int
clrsb_8(int8_t v) {
#if DESCENT_HAS_BUILTIN(__builtin_clrsb)
	return __builtin_clrsb(v) - 24;
#else
	int8_t sign = v >> 7;
	return clz_8((uint8_t) (v ^ sign)) - 1;
#endif
}

// Keep If

DESCENT_BUILTIN_API uint64_t
keep_if_64(uint64_t v, bool flag) {
	uint64_t mask = (uint64_t) (-!!((int64_t) flag));
	return v & mask;
}

DESCENT_BUILTIN_API uint32_t
keep_if_32(uint32_t v, bool flag) {
	uint32_t mask = (uint32_t) (-!!((int32_t) flag));
	return v & mask;
}

DESCENT_BUILTIN_API uint16_t
keep_if_16(uint16_t v, bool flag) {
	uint16_t mask = (uint16_t) (-!!((int16_t) flag));
	return v & mask;
}

DESCENT_BUILTIN_API uint8_t
keep_if_8(uint8_t v, bool flag) {
	uint8_t mask = (uint8_t) (-!!((int8_t) flag));
	return v & mask;
}

// Test Bit

DESCENT_BUILTIN_API bool
testbit_64(uint64_t v, unsigned int index) {
	DESCENT_BUILTIN_CONST unsigned int MASK = 63;
	index &= MASK;
	return ((v >> index) & 1);
}

DESCENT_BUILTIN_API bool
testbit_32(uint32_t v, unsigned int index) {
	DESCENT_BUILTIN_CONST unsigned int MASK = 31;
	index &= MASK;
	return ((v >> index) & 1);
}

DESCENT_BUILTIN_API bool
testbit_16(uint16_t v, unsigned int index) {
	DESCENT_BUILTIN_CONST unsigned int MASK = 15;
	index &= MASK;
	return ((v >> index) & 1);
}

DESCENT_BUILTIN_API bool
testbit_8(uint8_t v, unsigned int index) {
	DESCENT_BUILTIN_CONST unsigned int MASK = 7;
	index &= MASK;
	return ((v >> index) & 1);
}

// Set Bit

DESCENT_BUILTIN_API uint64_t
bit_set_64(uint64_t v, unsigned int index) {
	DESCENT_BUILTIN_CONST unsigned int MASK = 63;
	index &= MASK;
	return v | ((uint64_t) (1 << index));
}

DESCENT_BUILTIN_API uint32_t
bit_set_32(uint32_t v, unsigned int index) {
	DESCENT_BUILTIN_CONST unsigned int MASK = 31;
	index &= MASK;
	return v | ((uint32_t) (1 << index));
}

DESCENT_BUILTIN_API uint16_t
bit_set_16(uint16_t v, unsigned int index) {
	DESCENT_BUILTIN_CONST unsigned int MASK = 15;
	index &= MASK;
	return v | ((uint16_t) (1 << index));
}

DESCENT_BUILTIN_API uint8_t
bit_set_8(uint8_t v, unsigned int index) {
	DESCENT_BUILTIN_CONST unsigned int MASK = 7;
	index &= MASK;
	return v | ((uint8_t) (1 << index));
}

// Clear Bit

DESCENT_BUILTIN_API uint64_t
bit_clear_64(uint64_t v, unsigned int index) {
	DESCENT_BUILTIN_CONST unsigned int MASK = 63;
	index &= MASK;
	return v & ~((uint64_t) (1 << index));
}

DESCENT_BUILTIN_API uint32_t
bit_clear_32(uint32_t v, unsigned int index) {
	DESCENT_BUILTIN_CONST unsigned int MASK = 31;
	index &= MASK;
	return v & ~((uint32_t) (1 << index));
}

DESCENT_BUILTIN_API uint16_t
bit_clear_16(uint16_t v, unsigned int index) {
	DESCENT_BUILTIN_CONST unsigned int MASK = 15;
	index &= MASK;
	return v & ~((uint16_t) (1 << index));
}

DESCENT_BUILTIN_API uint8_t
bit_clear_8(uint8_t v, unsigned int index) {
	DESCENT_BUILTIN_CONST unsigned int MASK = 7;
	index &= MASK;
	return v ^ ((uint8_t) (1 << index));
}

// Toggle Bit

DESCENT_BUILTIN_API uint64_t
bit_toggle_64(uint64_t v, unsigned int index) {
	DESCENT_BUILTIN_CONST unsigned int MASK = 63;
	index &= MASK;
	return v ^ ((uint64_t) (1 << index));
}

DESCENT_BUILTIN_API uint32_t
bit_toggle_32(uint32_t v, unsigned int index) {
	DESCENT_BUILTIN_CONST unsigned int MASK = 31;
	index &= MASK;
	return v ^ ((uint32_t) (1 << index));
}

DESCENT_BUILTIN_API uint16_t
bit_toggle_16(uint16_t v, unsigned int index) {
	DESCENT_BUILTIN_CONST unsigned int MASK = 15;
	index &= MASK;
	return v ^ ((uint16_t) (1 << index));
}

DESCENT_BUILTIN_API uint8_t
bit_toggle_8(uint8_t v, unsigned int index) {
	DESCENT_BUILTIN_CONST unsigned int MASK = 7;
	index &= MASK;
	return v ^ ((uint8_t) (1 << index));
}

// Assign Bit

DESCENT_BUILTIN_API uint64_t
bit_assign_64(uint64_t v, bool flag, unsigned int index) {
	DESCENT_BUILTIN_CONST unsigned int MASK = 63;
	index &= MASK;
	return (v & ~((uint64_t) (1 << index))) | ((uint64_t) (!!flag << index));
}

DESCENT_BUILTIN_API uint32_t
bit_assign_32(uint32_t v, bool flag, unsigned int index) {
	DESCENT_BUILTIN_CONST unsigned int MASK = 31;
	index &= MASK;
	return (v & ~((uint32_t) (1 << index))) | ((uint32_t) (!!flag << index));
}

DESCENT_BUILTIN_API uint16_t
bit_assign_16(uint16_t v, bool flag, unsigned int index) {
	DESCENT_BUILTIN_CONST unsigned int MASK = 15;
	index &= MASK;
	return (v & ~((uint16_t) (1 << index))) | ((uint16_t) (!!flag << index));
}

DESCENT_BUILTIN_API uint8_t
bit_assign_8(uint8_t v, bool flag, unsigned int index) {
	DESCENT_BUILTIN_CONST unsigned int MASK = 7;
	index &= MASK;
	return (v & ~((uint8_t) (1 << index))) | ((uint8_t) (!!flag << index));
}

// Set Bits

DESCENT_BUILTIN_API uint64_t
bits_set_64(uint64_t v, uint64_t mask) {
	return v | mask;
}

DESCENT_BUILTIN_API uint32_t
bits_set_32(uint32_t v, uint32_t mask) {
	return v | mask;
}

DESCENT_BUILTIN_API uint16_t
bits_set_16(uint16_t v, uint16_t mask) {
	return v | mask;
}

DESCENT_BUILTIN_API uint8_t
bits_set_8(uint8_t v, uint8_t mask) {
	return v | mask;
}

// Clear Bits

DESCENT_BUILTIN_API uint64_t
bits_clear_64(uint64_t v, uint64_t mask) {
	return v & ~mask;
}

DESCENT_BUILTIN_API uint32_t
bits_clear_32(uint32_t v, uint32_t mask) {
	return v & ~mask;
}

DESCENT_BUILTIN_API uint16_t
bits_clear_16(uint16_t v, uint16_t mask) {
	return v & ~mask;
}

DESCENT_BUILTIN_API uint8_t
bits_clear_8(uint8_t v, uint8_t mask) {
	return v & ~mask;
}

// Toggle Bits

DESCENT_BUILTIN_API uint64_t
bits_toggle_64(uint64_t v, uint64_t mask) {
	return v ^ mask;
}

DESCENT_BUILTIN_API uint32_t
bits_toggle_32(uint32_t v, uint32_t mask) {
	return v ^ mask;
}

DESCENT_BUILTIN_API uint16_t
bits_toggle_16(uint16_t v, uint16_t mask) {
	return v ^ mask;
}

DESCENT_BUILTIN_API uint8_t
bits_toggle_8(uint8_t v, uint8_t mask) {
	return v ^ mask;
}

// Assign Bits

DESCENT_BUILTIN_API uint64_t
bits_assign_64(uint64_t v, bool flag, uint64_t mask) {
	return (v & ~mask) | (-(!!flag) & mask);
}

DESCENT_BUILTIN_API uint32_t
bits_assign_32(uint32_t v, bool flag, uint32_t mask) {
	return (v & ~mask) | (-(!!flag) & mask);
}

DESCENT_BUILTIN_API uint16_t
bits_assign_16(uint16_t v, bool flag, uint16_t mask) {
	return (v & ~mask) | (-(!!flag) & mask);
}

DESCENT_BUILTIN_API uint8_t
bits_assign_8(uint8_t v, bool flag, uint8_t mask) {
	return (v & ~mask) | (-(!!flag) & mask);
}

#ifdef __cplusplus
}
#endif

#endif
