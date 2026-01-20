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

#ifndef DESCENT_SYSALLOC_H
#define DESCENT_SYSALLOC_H

#include <stddef.h>

#include <descent/rcode.h>

#ifndef DESCENT_MAX_ALLOC
#define DESCENT_MAX_ALLOC 0x10000000000ULL
#endif

enum {
	// Reading, writing, and executing are forbidden
	SYSALLOC_ACCESS_NONE = 0,
	// Reading permitted
	SYSALLOC_ACCESS_READ = 1,
	// Writing permitted
	SYSALLOC_ACCESS_WRITE = 2,
	// Execution permitted
	SYSALLOC_ACCESS_EXEC = 4,
	// Reading and writing permitted
	SYSALLOC_ACCESS_READ_WRITE = SYSALLOC_ACCESS_READ | SYSALLOC_ACCESS_WRITE,
	// Reading and executing permitted
	SYSALLOC_ACCESS_READ_EXEC = SYSALLOC_ACCESS_READ | SYSALLOC_ACCESS_EXEC,
	// Writing and executing permitted
	SYSALLOC_ACCESS_WRITE_EXEC = SYSALLOC_ACCESS_WRITE | SYSALLOC_ACCESS_EXEC,
	// Reading, writing, and executing permitted
	SYSALLOC_ACCESS_READ_WRITE_EXEC = SYSALLOC_ACCESS_READ_WRITE | SYSALLOC_ACCESS_EXEC,
};

typedef struct {
	void  *base;
	size_t size;
} Sysalloc;

size_t sysalloc_granularity(void);

rcode sysalloc(Sysalloc *s, int access);

rcode sysalloc_reserve(Sysalloc *s);

rcode sysalloc_commit(Sysalloc *s, size_t offset, size_t size, int access);

rcode sysalloc_decommit(Sysalloc *s, size_t offset, size_t size);

rcode sysfree(Sysalloc *s);

#endif