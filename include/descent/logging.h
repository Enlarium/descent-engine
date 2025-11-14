// Copyright 2025 XavierHarkonnen9 and Enlarium
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//	 http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef DESCENT_LOGGING_H
#define DESCENT_LOGGING_H

#include <stdint.h>
#include <stdarg.h>

#include <descent/modules.h>

typedef enum {
	LOG_SUCCESS,
	LOG_WARNING_TRUNCATED_MESSAGE,
	LOG_ERROR_FORMAT_MESSAGE,
	LOG_ERROR_INVALID_HANDLE,
	LOG_ERROR_INVALID_FORMAT,
	LOG_ERROR_INVALID_LEVEL,
	LOG_ERROR_INVALID_MODULE,
	LOG_ERROR_INVALID_PRESENT,
	LOG_ERROR_INVALID_CONSOLE,
	LOG_ERROR_NULL_PATH,
	LOG_ERROR_FILE_ERROR,
	LOG_ERROR_NULL_POINTER,
	LOG_ERROR_ALLOCATION_FAILURE
} LogError;

typedef enum {
	LOG_LEVEL_TRACE   = 0x01,
	LOG_LEVEL_INFO    = 0x02,
	LOG_LEVEL_DEBUG   = 0x04,
	LOG_LEVEL_WARN    = 0x08,
	LOG_LEVEL_ERROR   = 0x10,
	LOG_LEVEL_FATAL   = 0x20,
	LOG_LEVEL_DEFAULT = LOG_LEVEL_WARN | LOG_LEVEL_ERROR | LOG_LEVEL_FATAL,
	LOG_LEVEL_ALL     = LOG_LEVEL_TRACE | LOG_LEVEL_INFO | LOG_LEVEL_DEBUG | LOG_LEVEL_DEFAULT
} LogLevel;

typedef enum {
	LOG_FORMAT_MINIMAL,
	LOG_FORMAT_MODULE,
	LOG_FORMAT_TIMESTAMP,
	LOG_FORMAT_FULL
} LogFormat;

typedef enum {
	LOG_PRESENT_PLAIN,
	LOG_PRESENT_STYLED,
	LOG_PRESENT_AUTO
} LogPresent;

typedef enum {
	LOG_SINK_WRITE,
	LOG_SINK_APPEND
} LogSinkMode;

typedef struct {
	uint8_t module;
	uint8_t sink;
} LogSinkHandle;

// Defaults to stderr
int log_sink_init(LogSinkHandle h, int format, int levels, int present);

int log_sink_file(LogSinkHandle h, const char *filepath, int mode);

int log_sink_stdout(LogSinkHandle h);

int log_sink_stderr(LogSinkHandle h);

int log_sink_format(LogSinkHandle h, int format);

int log_sink_present(LogSinkHandle h, int present);

int log_sink_filter(LogSinkHandle h, int levels);

int log_sink_add_levels(LogSinkHandle h, int levels);

int log_sink_drop_levels(LogSinkHandle h, int levels);

int log_message(DescentModule m, LogLevel l, const char *fmt, ...);

int log_submit(DescentModule m, LogLevel l, const char *fmt, va_list args);

void log_write(void);

void log_close(void);

static inline int log_sink_init_base(uint8_t sink, int format, int levels, int present) {
	return log_sink_init((LogSinkHandle) {.module = MODULE_BASE, .sink = sink}, format, levels, present);
}

static inline int log_sink_init_logging(uint8_t sink, int format, int levels, int present) {
	return log_sink_init((LogSinkHandle) {.module = MODULE_LOGGING, .sink = sink}, format, levels, present);
}

static inline int log_sink_init_rendering(uint8_t sink, int format, int levels, int present) {
	return log_sink_init((LogSinkHandle) {.module = MODULE_RENDERING, .sink = sink}, format, levels, present);
}

static inline int log_sink_init_scripting(uint8_t sink, int format, int levels, int present) {
	return log_sink_init((LogSinkHandle) {.module = MODULE_SCRIPTING, .sink = sink}, format, levels, present);
}

static inline int log_sink_init_threading(uint8_t sink, int format, int levels, int present) {
	return log_sink_init((LogSinkHandle) {.module = MODULE_THREADING, .sink = sink}, format, levels, present);
}

static inline int log_sink_init_allocator(uint8_t sink, int format, int levels, int present) {
	return log_sink_init((LogSinkHandle) {.module = MODULE_ALLOCATOR, .sink = sink}, format, levels, present);
}

#ifndef DESCENT_LOG_DISABLE_TRACE
#define TRACE_RENDERING(...) log_message(MODULE_RENDERING, LOG_LEVEL_TRACE, __VA_ARGS__)
#define TRACE_SCRIPTING(...) log_message(MODULE_SCRIPTING, LOG_LEVEL_TRACE, __VA_ARGS__)
#define TRACE_THREADING(...) log_message(MODULE_THREADING, LOG_LEVEL_TRACE, __VA_ARGS__)
#define TRACE_ALLOCATOR(...) log_message(MODULE_ALLOCATOR, LOG_LEVEL_TRACE, __VA_ARGS__)
#else
#define TRACE_RENDERING(...) ((void)0)
#define TRACE_SCRIPTING(...) ((void)0)
#define TRACE_THREADING(...) ((void)0)
#define TRACE_ALLOCATOR(...) ((void)0)
#endif

#ifndef DESCENT_LOG_DISABLE_INFO
#define INFO_RENDERING(...) log_message(MODULE_RENDERING, LOG_LEVEL_INFO, __VA_ARGS__)
#define INFO_SCRIPTING(...) log_message(MODULE_SCRIPTING, LOG_LEVEL_INFO, __VA_ARGS__)
#define INFO_THREADING(...) log_message(MODULE_THREADING, LOG_LEVEL_INFO, __VA_ARGS__)
#define INFO_ALLOCATOR(...) log_message(MODULE_ALLOCATOR, LOG_LEVEL_INFO, __VA_ARGS__)
#else
#define INFO_RENDERING(...) ((void)0)
#define INFO_SCRIPTING(...) ((void)0)
#define INFO_THREADING(...) ((void)0)
#define INFO_ALLOCATOR(...) ((void)0)
#endif

#ifndef DESCENT_LOG_DISABLE_DEBUG
#define DEBUG_RENDERING(...) log_message(MODULE_RENDERING, LOG_LEVEL_DEBUG, __VA_ARGS__)
#define DEBUG_SCRIPTING(...) log_message(MODULE_SCRIPTING, LOG_LEVEL_DEBUG, __VA_ARGS__)
#define DEBUG_THREADING(...) log_message(MODULE_THREADING, LOG_LEVEL_DEBUG, __VA_ARGS__)
#define DEBUG_ALLOCATOR(...) log_message(MODULE_ALLOCATOR, LOG_LEVEL_DEBUG, __VA_ARGS__)
#else
#define DEBUG_RENDERING(...) ((void)0)
#define DEBUG_SCRIPTING(...) ((void)0)
#define DEBUG_THREADING(...) ((void)0)
#define DEBUG_ALLOCATOR(...) ((void)0)
#endif

#ifndef DESCENT_LOG_DISABLE_WARN
#define WARN_RENDERING(...) log_message(MODULE_RENDERING, LOG_LEVEL_WARN, __VA_ARGS__)
#define WARN_SCRIPTING(...) log_message(MODULE_SCRIPTING, LOG_LEVEL_WARN, __VA_ARGS__)
#define WARN_THREADING(...) log_message(MODULE_THREADING, LOG_LEVEL_WARN, __VA_ARGS__)
#define WARN_ALLOCATOR(...) log_message(MODULE_ALLOCATOR, LOG_LEVEL_WARN, __VA_ARGS__)
#else
#define WARN_RENDERING(...) ((void)0)
#define WARN_SCRIPTING(...) ((void)0)
#define WARN_THREADING(...) ((void)0)
#define WARN_ALLOCATOR(...) ((void)0)
#endif

#ifndef DESCENT_LOG_DISABLE_ERROR
#define ERROR_RENDERING(...) log_message(MODULE_RENDERING, LOG_LEVEL_ERROR, __VA_ARGS__)
#define ERROR_SCRIPTING(...) log_message(MODULE_SCRIPTING, LOG_LEVEL_ERROR, __VA_ARGS__)
#define ERROR_THREADING(...) log_message(MODULE_THREADING, LOG_LEVEL_ERROR, __VA_ARGS__)
#define ERROR_ALLOCATOR(...) log_message(MODULE_ALLOCATOR, LOG_LEVEL_ERROR, __VA_ARGS__)
#else
#define ERROR_RENDERING(...) ((void)0)
#define ERROR_SCRIPTING(...) ((void)0)
#define ERROR_THREADING(...) ((void)0)
#define ERROR_ALLOCATOR(...) ((void)0)
#endif

#ifndef DESCENT_LOG_DISABLE_FATAL
#define FATAL_RENDERING(...) log_message(MODULE_RENDERING, LOG_LEVEL_FATAL, __VA_ARGS__)
#define FATAL_SCRIPTING(...) log_message(MODULE_SCRIPTING, LOG_LEVEL_FATAL, __VA_ARGS__)
#define FATAL_THREADING(...) log_message(MODULE_THREADING, LOG_LEVEL_FATAL, __VA_ARGS__)
#define FATAL_ALLOCATOR(...) log_message(MODULE_ALLOCATOR, LOG_LEVEL_FATAL, __VA_ARGS__)
#else
#define FATAL_RENDERING(...) ((void)0)
#define FATAL_SCRIPTING(...) ((void)0)
#define FATAL_THREADING(...) ((void)0)
#define FATAL_ALLOCATOR(...) ((void)0)
#endif

#endif