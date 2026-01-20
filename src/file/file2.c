#include <stddef.h>
#include <stdint.h>

#include <descent/alloc/sysalloc.h>
#include <descent/file.h>
#include <descent/thread.h>
#include <descent/thread/rwlock.h>
#include <descent/rcode.h>
#include <descent/utilities/macros.h>

#include <descent/utilities/platform.h>

#include "path.h"

#include <tomlc17.h>

#define FILE_METADATA_ALIGN 64
#define FILE_METADATA_SUBALIGN 8

// Ideas:
// Unbuffered: This file reads and writes directly to the OS. Good for single-use files.
// Double Buffered: This file uses double-buffered, asynchronous IO.
// Lockless: User is responsible for managing concurrency on this file.
// Scatter/Gather: 
// Async (not yet needed): This file reads and writes directly between disk and buffer.
// Direct (not yet needed): This file reads and writes directly between disk and buffer.
// Compressed (maybe should be separate stage, ala serialization)

// Tell and fsync

enum {
	FILE_TYPE_BINARY,
	FILE_TYPE_TOML,
	FILE_TYPE_JSON,
};

enum {
	FILE_FLAG_READ  = 1 << 0, // If set, the file is in READ mode, else in WRITE mode
	FILE_FLAG_EOF   = 1 << 1,  // End-of-file reached
	FILE_FLAG_ERROR = 1 << 2,  // Sticky error flag
};

typedef struct {
	uintptr_t handle;      // Platform-specific file handle
	size_t    position;    // Absolute file offset of the start of the buffer within the file
	size_t    cursor;      // Index inside the buffer pointing to the next byte to read or write
	size_t    end;         // How much valid data is in the buffer.
	size_t    size;        // Total size of the file.
	uint32_t  mode;        // Bitmask describing how the file was opened
	uint32_t  flags;       // r/w, eof, sticky error
	uint16_t  RESERVED_1;
	uint16_t  offset_bufs; // Offset to file buffer data. 0 means none.
	uint16_t  offset_lock; // Offset to file lock data. 0 means none.
	uint16_t  offset_type; // Offset to file type data. 0 means none.
} FileHeader;

typedef struct {
	void  *buff_0; // Mandatory
	void  *buff_1; // Optional for double buffering
	size_t size_0; // Mandatory
	size_t size_1; // Optional for double buffering
} FileBuffers;

typedef struct {
	RWLock lock;
} FileLock;

typedef struct {
	uint32_t type; // Type of the file
} FileType;

typedef struct {
	FileType t;
}	FileTypeTOML;

typedef struct {
	FileType t;
}	FileTypeJSON;

/* static inline */ FileLock *file2_lock(FileHeader *h) {
	return h->offset_lock ? POINTER_OFFSET(FileLock, h, h->offset_lock) : NULL;
}

/* static inline */ size_t file2_type_size(uint32_t type) {
	switch (type) {
	case FILE_TYPE_TOML:
		return ALIGNED_SIZE(FileTypeTOML, FILE_METADATA_SUBALIGN);
	case FILE_TYPE_JSON:
		return ALIGNED_SIZE(FileTypeJSON, FILE_METADATA_SUBALIGN);
	default:
		return 0;
	}
}

/* static inline */ size_t file2_meta_size(File *f) {
	const FileHeader *h = (const FileHeader *)f;

	size_t size = ALIGNED_SIZE(FileHeader, FILE_METADATA_SUBALIGN);
	size += h->offset_bufs ? ALIGNED_SIZE(FileBuffers, FILE_METADATA_SUBALIGN) : 0;
	size += h->offset_lock ? ALIGNED_SIZE(FileLock, FILE_METADATA_SUBALIGN) : 0;

	if (h->offset_type) {
		const FileType *t = POINTER_OFFSET_CONST(FileType, h, h->offset_type);
		size += file2_type_size(t->type);
	}

	return size;
}

/* static inline */ int file2_goto(File *f, size_t position) {
	FileHeader *h = (FileHeader *)f;
	int result = 0;

	// Flush writes if seeking past EOF
	if (position >= h->size) {
		result = file2_flush(f);
		if (result) return result;

		h->flags |= FILE_FLAG_EOF;
	}

	// Fast path: remain inside buffer
	if ((position >= h->position) && (position < h->position + h->end)) {
		h->cursor = position - h->position;
		h->flags &= ~FILE_FLAG_EOF;
	}

	// Slow path: move outside buffer
	else {
		result = file2_flush(f);
		if (result) return result;

		result = file2_seek_handle(h->handle, position);
		if (result) return result;

		// Do not fill buffer; lazy-load on read

		h->position = position;
	}

	return 0;
}

/* static inline */ int file2_write_start(File *f) {
	FileHeader *h = (FileHeader *)f;
	FileLock *l = file2_lock(h);

	if (l && rwlock_write_lock(&l->lock)) {
		return DESCENT_ERROR_STATE;
	}

	if (!(h->mode & FILE_MODE_WRITE)) {
		if (l) rwlock_write_unlock(&l->lock);
		return DESCENT_ERROR_FORBIDDEN;
	}

	if (h->flags & FILE_FLAG_READ) {
		file2_goto(f, h->position + h->cursor);
		h->flags &= ~FILE_FLAG_READ;
	}

	return 0;
}

/* static inline */ int file2_write_end(File *f) {
	FileHeader *h = (FileHeader *)f;
	FileLock *l = file2_lock(h);

	if (l && rwlock_write_unlock(&l->lock)) return DESCENT_ERROR_STATE;

	return 0;
}

/* static inline */ int file2_read_start(File *f) {
	FileHeader *h = (FileHeader *)f;
	FileLock *l = file2_lock(h);

	if (l && rwlock_read_lock(&l->lock)) {
		return DESCENT_ERROR_STATE;
	}

	if (!(h->mode & FILE_MODE_READ)) {
		if (l) rwlock_read_unlock(&l->lock);
		return DESCENT_ERROR_FORBIDDEN;
	}

	if (!(h->flags & FILE_FLAG_READ)) {
		int result = file2_flush(f);
		if (result) {
			if (l) rwlock_read_unlock(&l->lock);
			return result;
		}

		file2_goto(f, h->position + h->cursor);
		h->flags |= FILE_FLAG_READ;
	}

	return 0;
}

/* static inline */ int file2_read_end(File *f) {
	FileHeader *h = (FileHeader *)f;
	FileLock *l = file2_lock(h);

	if (l && rwlock_read_unlock(&l->lock)) return DESCENT_ERROR_STATE;

	return 0;
}

/* static */ int file2_open_type(File **f, uint32_t type, const char *dpath, int mode, FileOpenOptions* o) {
	if (!f || !dpath) return DESCENT_ERROR_NULL;
	if(!mode) return DESCENT_ERROR_INVALID;

	Sysalloc    alloc       = {0};
	Path        path        = {0};
	FobjInfo    info        = {0};
	uintptr_t   handle      = 0;
	FileHeader *header      = NULL;
	void       *buffer      = NULL;
	uint16_t    offset_bufs = 0;
	uint16_t    offset_lock = 0;
	uint16_t    offset_type = 0;
	int         result      = 0;
	size_t      meta_size   = ALIGNED_SIZE(FileHeader, FILE_METADATA_SUBALIGN);

	// Default configuration
	FileOpenOptions options = {
		.buffer_size = 0x8000, // 32 KB
		.lockless = 0,
	};

	// Override configuration if available
	if (o) memcpy(&options, o, sizeof(FileOpenOptions));

	// Calculate metadata size and offsets
	if (options.buffer_size) {
		offset_bufs = meta_size;
		meta_size += ALIGNED_SIZE(FileBuffers, FILE_METADATA_SUBALIGN);
	}
	if (!options.lockless) {
		offset_lock = meta_size;
		meta_size += ALIGNED_SIZE(FileLock, FILE_METADATA_SUBALIGN);
	}
	if (file2_type_size(type)) {
		offset_type = meta_size;
		meta_size += file2_type_size(type);
	}

	// Align metadata size
	meta_size = ALIGNED_SIZE(meta_size, FILE_METADATA_ALIGN);

	// Calculate minimum allocation size
	alloc.size = meta_size + options.buffer_size;

	// Resolve path
	result = path_resolve_dpath(&path, dpath);
	if (result) return result;

	// Get native handle
	result = file2_open_handle(&handle, &path, mode);
	if (result) return result;

	// Allocate memory with sysalloc
	// Sysalloc rounds up to page size/allocation granularity
	result = sysalloc(&alloc, SYSALLOC_ACCESS_READ_WRITE);
	if (result) {
		file2_close_handle(handle);
		return result;
	}

	// Get file size
	result = fobj_stat_path(&info, &path);
	if (result) {
		file2_close(f);
		return result;
	}

	// Use buffer to contain header
	header = (FileHeader *)alloc.base;
	header->handle      = handle;
	header->position    = 0;
	header->cursor      = 0;
	header->end         = 0;
	header->size        = info.size;
	header->mode        = mode;
	header->flags       = 0;
	header->offset_bufs = offset_bufs;
	header->offset_lock = offset_lock;
	header->offset_type = offset_type;

	if (offset_bufs) {
		FileBuffers *buffers = POINTER_OFFSET(FileBuffers, header, offset_bufs);
		buffers->buff_0 = POINTER_OFFSET(void, alloc.base, meta_size);
		buffers->size_0 = alloc.size - meta_size;
	}

	if (offset_lock) {
		FileLock *lock = POINTER_OFFSET(FileLock, header, offset_lock);
		result = rwlock_init(&lock->lock);
		if (result) {
			file2_close(f);
			return result;
		}
	}

	if (offset_type) {
		FileType *t = POINTER_OFFSET(FileType, header, offset_type);
		t->type = type;
	}

	*f = (File *) alloc.base;

	return 0;
}

int file2_open(File **f, const char *dpath, int mode) {
	return file2_open_ex(f, dpath, mode, NULL);
}

int file2_open_ex(File **f, const char *dpath, int mode, FileOpenOptions* o) {
	return file2_open_type(f, FILE_TYPE_BINARY, dpath, mode, o);
}

/*
Base
- Output in decimal, hex, octal, quaternary, or binary
- Capitalization
Prefix
- Whether the base has a marker (0d, 0x, 0o, 0q, 0b)
- Capitalization
Sign
- boolean
- Show + for positive numbers or leave blank
Width:
- unsigned integer
- Minimum number of characters to occupy
Alignment:
- boolean
- Left or right justify in the field
Pad:
- character (non-printable for none)
- Fill character for padding
Separator
- character (non-printable for none)
- Separator between thousands

Floating Point

Precision:
- unsigned integer, 0 for auto
- Total number of meaningful digits
- Minimum for fixed-point, exact for exponential
Decimal:
- unsigned integer
- Number of digits after the decimal
Style:
- boolean
- Fixed-point or scientific
*/