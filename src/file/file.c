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

#include <descent/file.h>

#include <stdint.h>
#include <string.h>

#include <descent/rcode.h>
#include <descent/alloc/sysalloc.h>
#include <descent/thread/rwlock.h>
#include <descent/thread/thread.h>

#include "path.h"
#include "fobj.h"
#include "file/handle.h"

#include <stdio.h>

#include <tomlc17.h>

#define FILE_ALIGNMENT 64

enum {
	FILE_FLAG_READ       = 1 << 0, // If set, the file is in READ mode, else in WRITE mode
	FILE_FLAG_EOF        = 1 << 1,  // End-of-file reached
	FILE_FLAG_ERROR      = 1 << 2,  // Sticky error flag
};

typedef struct File {
	uintptr_t handle;      // Platform-specific file handle
	void     *buffer;      // Pointer to file buffer
	size_t    buffer_size; // Size of file buffer in bytes
	size_t    position;    // Absolute file offset of the start of the buffer within the file
	size_t    cursor;      // Index inside the buffer pointing to the next byte to read or write
	size_t    end;         // How much valid data is in the buffer.
	size_t    size;        // Total size of the file if known
	uint32_t  mode;        // Bitmask describing how the file was opened
	uint32_t  flags;       // r/w, eof, sticky error
	RWLock    lock;
} File;

static const size_t FILE_ALIGNED_SIZE = (sizeof(File) + FILE_ALIGNMENT - 1) & ~(FILE_ALIGNMENT - 1);

static inline int file_goto(File *f, size_t position) {
	if (!f) return DESCENT_ERROR_NULL;

	int result = 0;

	// Flush writes if seeking past EOF
	if (position >= f->size) {
		result = file_flush(f);
		if (result) return result;

		f->flags |= FILE_FLAG_EOF;
	}

	// Fast path: inside buffer
	if ((position >= f->position) && (position < f->position + f->end)) {
		f->cursor = position - f->position;
		f->flags &= ~FILE_FLAG_EOF;
	}

	// Slow path: outside buffer
	else {
		result = file_flush(f);
		if (result) return result;

		result = file_seek_handle(f->handle, position);
		if (result) return result;

		// Do not fill buffer; lazy-load on read

		f->position = position;
	}

	return 0;
}

static inline int file_write_start(File *f) {
	if (rwlock_write_lock(&f->lock)) return DESCENT_ERROR_STATE;

	if (!(f->mode & FILE_MODE_WRITE)) return DESCENT_ERROR_FORBIDDEN;

	if (f->flags & FILE_FLAG_READ) {
		file_goto(f, f->position + f->cursor);
		f->flags &= ~FILE_FLAG_READ;
	}

	return 0;
}

static inline int file_write_end(File *f) {
	return rwlock_write_unlock(&f->lock) ? DESCENT_ERROR_STATE : 0;
}

static inline int file_read_start(File *f) {
	if (rwlock_read_lock(&f->lock)) return DESCENT_ERROR_STATE;

	if (!(f->mode & FILE_MODE_READ)) return DESCENT_ERROR_FORBIDDEN;

	if (!(f->flags & FILE_FLAG_READ)) {
		int result = file_flush(f);
		if (result) return result;

		file_goto(f, f->position + f->cursor);
		f->flags |= FILE_FLAG_READ;
	}

	return 0;
}

static inline int file_read_end(File *f) {
	return rwlock_read_unlock(&f->lock) ? DESCENT_ERROR_STATE : 0;
}

static inline int file_fill_buffer(File *f) {
	f->position += f->cursor;
	f->cursor = 0;
	f->end = 0;

	return file_read_handle(f->handle, f->buffer_size, f->buffer, &f->end);
}

int file_open(File **f, const char *dpath, int mode) {
	return file_open_ex(f, dpath, mode, NULL);
}

int file_open_ex(File **f, const char *dpath, int mode, FileOpenOptions* o) {
	if (!f || !dpath) return DESCENT_ERROR_NULL;
	if(!mode) return DESCENT_ERROR_INVALID;

	Sysalloc  alloc  = {0};
	Path      path   = {0};
	FobjInfo  info   = {0};
	uintptr_t handle = 0;
	File     *header = NULL;
	void     *buffer = NULL;
	int       result = 0;

	// Default configuration
	FileOpenOptions options = {
		.buffer_size = 0x8000, // 32 KB
	};
	
	// Override configuration if available
	if (o) {
		memcpy(&options, o, sizeof(FileOpenOptions));

		// Validate configuration
		if (options.buffer_size < 0x1000) options.buffer_size = 0x1000; // 4 KB
	}

	result = path_resolve_dpath(&path, dpath);
	if (result) return result;

	result = file_open_handle(&handle, &path, mode);
	if (result) return result;

	// Sysalloc rounds up to page sizes
	result = sysalloc(&alloc, &options.buffer_size);
	if (result) {
		file_close_handle(handle);
		return result;
	}

	// Get file size
	result = fobj_stat_path(&info, &path);
	if (result) {
		file_close(f);
		return result;
	}

	// Use buffer to contain header
	header = (File *)alloc;
	buffer = void_pointer_add(alloc, FILE_ALIGNED_SIZE);
	options.buffer_size -= FILE_ALIGNED_SIZE;

	header->handle      = handle;
	header->buffer      = buffer;
	header->buffer_size = options.buffer_size;
	header->position    = 0;
	header->cursor      = 0;
	header->end         = 0;
	header->size        = info.size;
	header->mode        = mode;
	header->flags       = 0;

	*f = header;

	return 0;
}

int file_close(File **f) {
	int result = file_flush(*f);
	if (result) return result;
	
	result = file_close_handle((*f)->handle);
	if (result) return result;

	void *alloc = (void *)*f;
	size_t size = (*f)->buffer_size + FILE_ALIGNED_SIZE;

	return sysfree(&alloc, &size);
}

int file_flush(File *f) {
	if (!f) return DESCENT_ERROR_NULL;

	size_t written = 0;

	int result = file_write_all_handle(f->handle, f->cursor, f->buffer, &written);
	if (result) return result;

	f->position += written;
	f->size += written;
	f->cursor = 0;
	f->end = 0;

	return 0;
}

int file_seek(File *f, int whence, int64_t offset) {
	if (!f) return DESCENT_ERROR_NULL;

	int64_t position = -1;

	switch (whence) {
		case FILE_SEEK_SET:
			position = offset;
			break;
		case FILE_SEEK_CURRENT:
			position = (int64_t) f->position + (int64_t) f->cursor + offset;
			break;
		case FILE_SEEK_END:
			position = (int64_t) f->size + offset;
			break;
	}

	if (position < 0) return DESCENT_ERROR_INVALID;

	return file_goto(f, position);
}

int file_write(File *f, size_t size, const void *input, size_t *written) {
	if (!f || !input) return DESCENT_ERROR_NULL;
	if (written) *written = 0;

	int result = file_write_start(f);
	if (result) {
		file_write_end(f);
		return result;
	}

	size_t remaining = size;
	while (remaining > 0) {
		size_t space = f->buffer_size - f->cursor;

		// Flush when buffer is full
		if (space == 0) {
			int result = file_flush(f);
			if (result) {
				file_write_end(f);
				return result;
			}

			space = f->buffer_size;
		}

		size_t to_copy = remaining < space ? remaining : space;

		memcpy(void_pointer_add(f->buffer, f->cursor), input, to_copy);
		
		f->cursor += to_copy;
		remaining -= to_copy;

		input = void_pointer_add_const(input, to_copy);
		if (written) *written += to_copy;
	}

	return file_write_end(f);
}

int file_read(File *f, size_t size, void *output, size_t *read) {
	if (!f || !output) return DESCENT_ERROR_NULL;
	if (read) *read = 0;

	int result = file_read_start(f);
	if (result) {
		file_read_end(f);
		return result;
	}

	size_t remaining = size;

	while (remaining > 0) {
		size_t available = f->end - f->cursor;

		// Refill when buffer is empty
		if (available == 0) {
			int result = file_fill_buffer(f);
			if (result) {
				file_read_end(f);
				return result;
			}
			
			// EOF
			if (f->end == 0) break;

			available = f->end;
		}

		size_t to_copy = remaining < available ? remaining : available;
		memcpy(output, void_pointer_add(f->buffer, f->cursor), to_copy);

		f->cursor += to_copy;
		f->position += to_copy;
		remaining -= to_copy;

		output = void_pointer_add(output, to_copy);
		if (read) *read += to_copy;

	}

	return file_read_end(f);
}

//int file_exists(const char *dpath);
//
//int file_write_char(FileHandle *of, char c);
//int file_write_string(FileHandle *of, size_t size, const char *s);
//int file_write_format(FileHandle *of, const char *format, ...);
//
//int file_read(FileHandle *of, size_t size, void *buffer, size_t *read);
//int file_read_char(FileHandle *of, char *c);
//int file_read_string(FileHandle *of, size_t size, char *s);
//int file_read_format(FileHandle *of, const char *format, ...);
