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

#ifndef DESCENT_FILE_H
#define DESCENT_FILE_H

#include <stddef.h>
#include <stdint.h>

enum {
	FILE_MODE_READ         = 1 << 0, // The file can be read from. Ignored if FILE_MODE_APPEND is set.
	FILE_MODE_WRITE        = 1 << 1, // The file can be written to. Implied if FILE_MODE_APPEND is set.
	FILE_MODE_APPEND       = 1 << 2, // All writes will start from the end of the file.
	FILE_MODE_TRUNCATE     = 1 << 3, // The file will be truncated on opening. Ignored if FILE_MODE_APPEND is set.
	FILE_MODE_CREATE       = 1 << 4, // The file will be created if it does not exist.
	FILE_MODE_EXCLUSIVE    = 1 << 5, // Creating the file will exist if it exists. Ignored if FILE_MODE_CREATE is not set.
	//FILE_ACCESS_SEQUENTIAL = 1 << 28,
	//FILE_ACCESS_RANDOM     = 1 << 29,
	//FILE_ACCESS_TEMPORARY  = 1 << 30,

	FILE_R   = FILE_MODE_READ,
	FILE_W   = FILE_MODE_WRITE  | FILE_MODE_TRUNCATE | FILE_MODE_CREATE,
	FILE_RW  = FILE_MODE_READ   | FILE_MODE_WRITE    | FILE_MODE_CREATE,
	FILE_WR  = FILE_MODE_READ   | FILE_MODE_WRITE    | FILE_MODE_TRUNCATE | FILE_MODE_CREATE,
	FILE_A   = FILE_MODE_APPEND | FILE_MODE_CREATE,
};

enum {
	FOBJ_NONE  = 0,
	FOBJ_FILE  = 1,
	FOBJ_DIR   = 2,
	FOBJ_OTHER = 3,
};

enum {
	FILE_SEEK_SET,
	FILE_SEEK_CURRENT,
	FILE_SEEK_END,
};

typedef struct {
	uint64_t size;  // File size in bytes (0 for dirs/symlinks)
	uint64_t atime; // Last access time (unix timestamp)
	uint64_t mtime; // Last modification time (unix timestamp)
	uint64_t ctime; // Metadata change / creation time
	uint32_t type;
	uint32_t mode;
} FobjInfo;

typedef struct File File;

typedef struct {
	size_t buffer_size;
	int    lockless;
} FileOpenOptions;

int file_init_virtual_roots(const char *program_name);

// File Object Operations

int fobj_stat(FobjInfo *info, const char *dpath);
int fobj_move(const char *old, const char *new, int overwrite);
int fobj_copy(const char *old, const char *new, int overwrite);
int fobj_move_into(const char *source, const char *destination, int overwrite);
int fobj_copy_into(const char *source, const char *destination, int overwrite);
int fobj_delete(const char *dpath);

// File

int file_open(File **f, const char *dpath, int mode);

int file_open_ex(File **f, const char *dpath, int mode, FileOpenOptions* o);

int file_write(File *f, size_t size, const void *input, size_t *written);

int file_flush(File *f);

int file_read(File *f, size_t size, void *output, size_t *read);

int file_close(File **f);

int file_seek(File *f, int whence, int64_t offset);

// Seek
// Tell
// Flush
// Temp
// Reopen

// formatted printing
// formatted scanning

// formatting:
/*
%[flags][width][.precision][length]specifier
specifiers:
*/

int file_exists(const char *dpath);

int file_delete(const char *dpath);

// Directory

int dir_create(const char *dpath);

int dir_ensure(const char *dpath);

int dir_create_recursive(const char *dpath);

int dir_remove(const char *dpath);

int dir_delete(const char *dpath);

int dir_remove_recursive(const char *dpath);

int dir_exists(const char *dpath);

#endif