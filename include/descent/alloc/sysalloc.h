#ifndef DESCENT_SYSALLOC_H
#define DESCENT_SYSALLOC_H

#include <stddef.h>

typedef struct {
	void *alloc;
	size_t size;
} Alloc;

size_t block_size();

Alloc sysalloc(size_t size);

int sysfree(Alloc *a);

#endif