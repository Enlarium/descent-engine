#ifndef DESCENT_UTILITIES_VERSION_H
#define DESCENT_UTILITIES_VERSION_H

#include <stdint.h>

typedef struct {
	uint16_t major;
	uint16_t minor;
	uint16_t patch;
	uint16_t variant;
} Version;

// Checks whether a is newer (positive), the same (zero) or older (negative)than b
static inline int version_compare(Version a, Version b) {
    if (a.major != b.major) return a.major - b.major;
    if (a.minor != b.minor) return a.minor - b.minor;
    if (a.patch != b.patch) return a.patch - b.patch;
    return a.variant - b.variant;
}

static inline uint32_t version_to_vk_api(Version v) {
	if (v.variant >= 8 || v.major >= 128 || v.minor >= 1024 || v.major >= 4096) return 0;
	return ((((uint32_t)(v.variant)) << 29U) | (((uint32_t)(v.major)) << 22U) | (((uint32_t)(v.minor)) << 12U) | ((uint32_t)(v.patch)));
}

static inline Version version_from_vk_api(uint32_t v) {
	Version result;
	result.variant = (uint16_t)((v >> 29U) & 0x7U);
	result.major   = (uint16_t)((v >> 22U) & 0x7FU);
	result.minor   = (uint16_t)((v >> 12U) & 0x3FFU);
	result.patch   = (uint16_t)(v & 0xFFFU);
	return result;
}

#endif