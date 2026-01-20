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

// TODO: Allow user to define their own modules and module strings

#include <descent/log.h>

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include <descent/utilities/platform.h>
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
#include <unistd.h>
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <descent/modules.h>
#include <descent/thread/atomic.h>
#include <descent/thread/rwlock.h>
#include <descent/thread/thread.h>
#include <descent/utilities/intrin/bits.h>
#include <descent/rcode.h>

#include "tables.h"

#define LOG_MODULE_SINK_COUNT 2
#define LOG_MESSAGE_SIZE 256
#define LOG_QUEUE_SIZE 256
#define LOG_BACKOFF 256

/* TODO:
Synchronous logging
Flush logs
*/

// Declarations

typedef struct {
	const char **log_module_strings;
	const char **log_level_strings;
	FILE *output;
	uint8_t format;
	uint8_t filter;
} LogSink;

typedef struct {
	char message[LOG_MESSAGE_SIZE];
	time_t timestamp;
	int module;
	int level;
	atomic_32 complete;
} LogMessage;

// Global variables

static LogSink log_sinks[MODULE_COUNT][LOG_MODULE_SINK_COUNT] = {0};
static RWLock log_sink_lock;
static RWLock submit_lock;

static atomic_32 queue_head = 0;
static atomic_32 queue_tail = 0;
static LogMessage queue[LOG_QUEUE_SIZE] = {0};

// Validity Helpers

static inline int log_sink_handle_valid(LogSinkHandle h) {
	return h.module < MODULE_COUNT && h.sink < LOG_MODULE_SINK_COUNT;
}

static inline int log_format_valid(int format) {
	return (
		format == LOG_FORMAT_MINIMAL ||
		format == LOG_FORMAT_MODULE ||
		format == LOG_FORMAT_TIMESTAMP ||
		format == LOG_FORMAT_FULL
	);
}

static inline int log_levels_valid(int levels) {
	const int valid_mask = LOG_LEVEL_TRACE | LOG_LEVEL_INFO | LOG_LEVEL_DEBUG | LOG_LEVEL_WARN  | LOG_LEVEL_ERROR | LOG_LEVEL_FATAL;
	return (levels & ~valid_mask) == 0;
}

static inline int log_present_valid(int present) {
	return (
		present == LOG_PRESENT_PLAIN ||
		present == LOG_PRESENT_STYLED ||
		present == LOG_PRESENT_AUTO
	);
}

// Mutating Helpers

static inline int log_sink_supports_color(LogSinkHandle h) {
	assert(log_sink_handle_valid(h));

	rwlock_read_lock(&log_sink_lock);
	
	FILE *sink = log_sinks[h.module][h.sink].output;
	int result = 0;

	if (sink) {
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
		result = isatty(fileno(sink));
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
		HANDLE handle = NULL;
		if (sink == stdout) handle = GetStdHandle(STD_OUTPUT_HANDLE);
		else if (sink == stderr) handle = GetStdHandle(STD_ERROR_HANDLE);
		else {
			rwlock_read_unlock(&log_sink_lock);
			return result;
		}

		DWORD mode;
		if (GetConsoleMode(handle, &mode)) {
			result = (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
		}
#endif
	}

	rwlock_read_unlock(&log_sink_lock);

	return result;
}

static inline FILE *log_sink_exchange(LogSinkHandle h, FILE *f) {
	assert(log_sink_handle_valid(h));
	
	rwlock_write_lock(&log_sink_lock);

	FILE *old_output = log_sinks[h.module][h.sink].output;
	log_sinks[h.module][h.sink].output = f;

	rwlock_write_unlock(&log_sink_lock);

	return old_output;
}

static inline void log_sink_set(LogSinkHandle h, FILE *f) {
	assert(log_sink_handle_valid(h));

	FILE *old_output = log_sink_exchange(h, f);

	if (old_output && old_output != stdin && old_output != stdout && old_output != stderr) {
		fflush(old_output);
		fclose(old_output);
	}
}

// API implementations

int log_sink_init(LogSinkHandle h, int format, int levels, int present) {
	if (!log_sink_handle_valid(h)) return LOG_ERROR_INVALID_HANDLE;
	if (!log_format_valid(format)) return LOG_ERROR_INVALID_FORMAT;
	if (!log_levels_valid(levels)) return LOG_ERROR_INVALID_LEVEL;
	if (!log_present_valid(present)) return LOG_ERROR_INVALID_PRESENT;

	const char **module_strings;
	const char **level_strings;

	
	if (present == LOG_PRESENT_AUTO) {
		int color_support = 0;
#if defined(DESCENT_PLATFORM_TYPE_POSIX)
		color_support = isatty(fileno(stderr));
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
		DWORD mode;
		if (GetConsoleMode(GetStdHandle(STD_ERROR_HANDLE), &mode)) {
			color_support = (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
		}
#endif

		present = color_support ? LOG_PRESENT_STYLED : present;
	}
	
	if (present == LOG_PRESENT_STYLED) {
		module_strings = log_module_strings_styled;
		level_strings = log_level_strings_styled;
	} else {
		module_strings = log_module_strings_plain;
		level_strings = log_level_strings_plain;
	}

	rwlock_write_lock(&log_sink_lock);
	
	FILE *old_output = log_sinks[h.module][h.sink].output;

	log_sinks[h.module][h.sink].output = stderr;
	log_sinks[h.module][h.sink].format = format;
	log_sinks[h.module][h.sink].log_module_strings = module_strings;
	log_sinks[h.module][h.sink].log_level_strings = level_strings;
	log_sinks[h.module][h.sink].filter = levels;
	
	rwlock_write_unlock(&log_sink_lock);

	if (old_output && old_output != stdin && old_output != stdout && old_output != stderr) {
		fflush(old_output);
		fclose(old_output);
	}

	return 0;
}

int log_sink_file(LogSinkHandle h, const char *filepath, int mode) {
	if (!log_sink_handle_valid(h)) return LOG_ERROR_INVALID_HANDLE;
	if (!filepath) return DESCENT_ERROR_NULL;
	
	FILE *output = fopen(filepath, (mode == LOG_SINK_WRITE) ? "w" : "a");
	if (!output) return DESCENT_ERROR_MEMORY;

	log_sink_set(h, output);

	return 0;
}

int log_sink_stdout(LogSinkHandle h) {
	if (!log_sink_handle_valid(h)) return LOG_ERROR_INVALID_HANDLE;

	log_sink_set(h, stdout);

	return 0;
}

int log_sink_stderr(LogSinkHandle h) {
	if (!log_sink_handle_valid(h)) return LOG_ERROR_INVALID_HANDLE;

	log_sink_set(h, stderr);

	return 0;
}

int log_sink_format(LogSinkHandle h, int format) {
	if (!log_sink_handle_valid(h)) return LOG_ERROR_INVALID_HANDLE;
	if (!log_format_valid(format)) return LOG_ERROR_INVALID_FORMAT;

	rwlock_write_lock(&log_sink_lock);
	
	log_sinks[h.module][h.sink].format = format;

	rwlock_write_unlock(&log_sink_lock);
	
	return 0;
}

int log_sink_present(LogSinkHandle h, int present) {
	if (!log_sink_handle_valid(h)) return LOG_ERROR_INVALID_HANDLE;
	if (!log_present_valid(present)) return LOG_ERROR_INVALID_PRESENT;

	const char **module_strings;
	const char **level_strings;

	if (log_sink_supports_color(h) && (present == LOG_PRESENT_AUTO)) present = LOG_PRESENT_STYLED;
	
	if (present == LOG_PRESENT_STYLED) {
		module_strings = log_module_strings_styled;
		level_strings = log_level_strings_styled;
	} else {
		module_strings = log_module_strings_plain;
		level_strings = log_level_strings_plain;
	}

	rwlock_write_lock(&log_sink_lock);
	
	log_sinks[h.module][h.sink].log_module_strings = module_strings;
	log_sinks[h.module][h.sink].log_level_strings = level_strings;

	rwlock_write_unlock(&log_sink_lock);
	
	return 0;
}

int log_sink_filter(LogSinkHandle h, int levels) {
	if (!log_sink_handle_valid(h)) return LOG_ERROR_INVALID_HANDLE;
	if (!log_levels_valid(levels)) return LOG_ERROR_INVALID_LEVEL;

	rwlock_write_lock(&log_sink_lock);
	
	log_sinks[h.module][h.sink].filter = levels;

	rwlock_write_unlock(&log_sink_lock);
	
	return 0;
}

int log_sink_add_levels(LogSinkHandle h, int levels) {
	if (!log_sink_handle_valid(h)) return LOG_ERROR_INVALID_HANDLE;
	if (!log_levels_valid(levels)) return LOG_ERROR_INVALID_LEVEL;

	rwlock_write_lock(&log_sink_lock);
	
	log_sinks[h.module][h.sink].filter |= levels;

	rwlock_write_unlock(&log_sink_lock);
	
	return 0;
}

int log_sink_drop_levels(LogSinkHandle h, int levels) {
	if (!log_sink_handle_valid(h)) return LOG_ERROR_INVALID_HANDLE;
	if (!log_levels_valid(levels)) return LOG_ERROR_INVALID_LEVEL;

	rwlock_write_lock(&log_sink_lock);
	
	log_sinks[h.module][h.sink].filter &= ~levels;

	rwlock_write_unlock(&log_sink_lock);
	
	return 0;
}

int log_message(DescentModule m, LogLevel l, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int result = log_submit(m, l, fmt, args);
	va_end(args);
	return result;
}

int log_submit(DescentModule m, LogLevel l, const char *fmt, va_list args) {
	if (!log_module_valid(m)) return DESCENT_ERROR_MODULE;
	if (!log_levels_valid(l)) return LOG_ERROR_INVALID_LEVEL;
	if (!fmt) return DESCENT_ERROR_NULL;

	rwlock_read_lock(&submit_lock);

	// Claim a message index using a CAS loop
	// this loop may be overcontentious.
	uint32_t index;
	do {
		uint32_t tail = atomic_load_32(&queue_tail);
		uint32_t head = atomic_load_32(&queue_head);

		if ((tail - head) >= LOG_QUEUE_SIZE) {
			// Become a writer to free queue space, then retry
			// This is necessary if the caller has not set up a dedicated writer
			log_write();
			continue;
		}

		index = tail;
	} while (!atomic_compare_exchange_32(&queue_tail, &index, index + 1));

	LogMessage *msg = &queue[index % LOG_QUEUE_SIZE];

	// this loop may be overcontentious.
	while (atomic_load_32(&msg->complete) == 1) thread_yield();

	msg->timestamp = time(NULL);
	msg->module = m;
	msg->level = l;
	int result = vsnprintf(msg->message, LOG_MESSAGE_SIZE, fmt, args);

	if (result < 0) {
		const char err_msg[] = "Could not format message";
		memcpy(msg->message, err_msg, sizeof(err_msg));
		result = LOG_ERROR_FORMAT_MESSAGE;
	} else if (result >= LOG_MESSAGE_SIZE) {
		result = DESCENT_WARN_TRUNCATION;
	} else {
		result = 0;
	}

	atomic_store_32(&msg->complete, 1);
	
	rwlock_read_unlock(&submit_lock);
	
	return result;
}

void log_write(void) {
	// Claim a message index using a CAS loop
	uint32_t index;
	do {
		uint32_t tail = atomic_load_32(&queue_tail);
		uint32_t head = atomic_load_32(&queue_head);

		if (tail == head) return;

		index = head;
	} while (!atomic_compare_exchange_32(&queue_head, &index, index + 1));

	// Load the message
	LogMessage *msg = &queue[index % LOG_QUEUE_SIZE];

	// Wait to write the message until it is complete
	while (atomic_load_32(&msg->complete) == 0) thread_yield();

	// Load parameters from the log message
	int module = msg->module;
	int level = msg->level;
	time_t timestamp = msg->timestamp;
	const char *message = msg->message;

	// The level is known to be valid - nonzero and consisting only of defined bits.
	// If (somehow) a message is logged at multiple levels, select the highest level to log at
	int level_index = 31 - clz_32(level);

	rwlock_read_lock(&log_sink_lock);

	// Write the message to each sink for the module
	for (int i = 0; i < LOG_MODULE_SINK_COUNT; ++i) {
		const char **log_module_strings = log_sinks[module][i].log_module_strings;
		const char **log_level_strings = log_sinks[module][i].log_level_strings;
		FILE *output = log_sinks[module][i].output;
		uint32_t format = log_sinks[module][i].format;
		uint32_t filter = log_sinks[module][i].filter;

		// Do not log on invalid sinks
		if (!log_module_strings || !log_level_strings || !output) continue;

		// Do not log filtered messages
		if (!(filter & level)) continue;

		char time_string[24];
		struct tm time;

#if defined(DESCENT_PLATFORM_TYPE_POSIX)
			localtime_r(&timestamp, &time);
#elif defined(DESCENT_PLATFORM_TYPE_WINDOWS)
			localtime_s(&time, &timestamp);
#endif

		strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", &time);

		switch (format) {
			case LOG_FORMAT_MINIMAL:
				fprintf(output, "[%s] %s\n", log_level_strings[level_index], message);
				break;
			case LOG_FORMAT_MODULE:
				fprintf(output, "[%s] [%s] %s\n", log_module_strings[module], log_level_strings[level_index], message);
				break;
			case LOG_FORMAT_TIMESTAMP:
				fprintf(output, "[%s] [%s] %s\n", time_string, log_level_strings[level_index], message);
				break;
			case LOG_FORMAT_FULL:
				fprintf(output, "[%s] [%s] [%s] %s\n", time_string, log_module_strings[module], log_level_strings[level_index], message);
				break;
		}
	}

	rwlock_read_unlock(&log_sink_lock);

	atomic_store_32(&msg->complete, 0);
}

void log_close(void) {
	rwlock_write_lock(&submit_lock);

	while (atomic_load_32(&queue_head) != atomic_load_32(&queue_tail)) {
		log_write();
	}

	for (int i = 0; i < MODULE_COUNT; ++i) {
		for (int j = 0; j < LOG_MODULE_SINK_COUNT; ++j) {
			FILE *old_output = log_sinks[i][j].output;
			log_sinks[i][j].output = NULL;

			if (old_output && old_output != stdin && old_output != stdout && old_output != stderr) {
				fflush(old_output);
				fclose(old_output);
			}
		}
	}

  rwlock_write_unlock(&submit_lock);
}
