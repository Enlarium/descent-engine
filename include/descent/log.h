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

#ifndef DESCENT_LOG_H
#define DESCENT_LOG_H

#include <stdint.h>
#include <stdarg.h>

#include <descent/modules.h>

/**
 * @enum LogLevel
 * @brief Logging levels.
 */
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

/**
 * @enum LogFormat
 * @brief Logging formats.
 */
typedef enum {
	LOG_FORMAT_MINIMAL,
	LOG_FORMAT_MODULE,
	LOG_FORMAT_TIMESTAMP,
	LOG_FORMAT_FULL
} LogFormat;

/**
 * @enum LogPresent
 * @brief Logging presentation modes.
 */
typedef enum {
	LOG_PRESENT_PLAIN,
	LOG_PRESENT_STYLED,
	LOG_PRESENT_AUTO
} LogPresent;

/**
 * @enum LogSinkMode
 * @brief Logging sink modes.
 */
typedef enum {
	LOG_SINK_WRITE, /**< Overwrite existing contents of the sink */
	LOG_SINK_APPEND /**< Append to the sink */
} LogSinkMode;

/**
 * @struct LogSinkHandle
 * @brief Handle to a specific sink
 */
typedef struct {
	uint8_t module;
	uint8_t sink;
} LogSinkHandle;

/**
 * @brief Creates a log sink handle
 * 
 * @param m the module
 * @param sink 
 * @return The corresponding log sink handle 
 */
static inline LogSinkHandle log_sink_handle(DescentModule m, int sink) {
	return (LogSinkHandle) {.module = m, .sink = sink};
}

/**
 * @brief Initializes the specified sink to stderr.
 * 
 * @param h A handle to the sink to initialize.
 * @param format The format the sink will use.
 * @param levels The levels the sink will log.
 * @param present The presentation mode the sink will use.
 * @return 0 on success, DescentCode on failure.
 */
int log_sink_init(LogSinkHandle h, int format, int levels, int present);

// Sets the sink's output to a disk file
int log_sink_file(LogSinkHandle h, const char *filepath, int mode);

// Sets the sink's output to stdout
int log_sink_stdout(LogSinkHandle h);

// Sets the sink's output to stderr
int log_sink_stderr(LogSinkHandle h);

// Sets the sink's format
int log_sink_format(LogSinkHandle h, int format);

// Sets the sink's presentation mode
int log_sink_present(LogSinkHandle h, int present);

// Sets the sink's filter
int log_sink_filter(LogSinkHandle h, int levels);

// Adds levels to the sink's filter (will accept)
int log_sink_add_levels(LogSinkHandle h, int levels);

// Drops levels from the sink's filter (will no longer accept)
int log_sink_drop_levels(LogSinkHandle h, int levels);

// Puts message onto queue. Limited to 256 characters, will truncate if too long.
// Truncation is not failure, but will return a warning code.
int log_message(DescentModule m, LogLevel l, const char *fmt, ...);

// Puts message onto queue. Limited to 256 characters, will truncate if too long.
// Truncation is not failure, but will return a warning code.
int log_submit(DescentModule m, LogLevel l, const char *fmt, va_list args);

// Writes a message from the queue
void log_write(void);

// Flushes the queue and closes all sinks
void log_close(void);

// Initializer for the core module
static inline int log_sink_init_core(uint8_t sink, int format, int levels, int present) {
	return log_sink_init(log_sink_handle(MODULE_CORE, sink), format, levels, present);
}

static inline int log_sink_init_logging(uint8_t sink, int format, int levels, int present) {
	return log_sink_init(log_sink_handle(MODULE_LOGGING, sink), format, levels, present);
}

static inline int log_sink_init_threading(uint8_t sink, int format, int levels, int present) {
	return log_sink_init(log_sink_handle(MODULE_THREADING, sink), format, levels, present);
}

static inline int log_sink_init_allocator(uint8_t sink, int format, int levels, int present) {
	return log_sink_init(log_sink_handle(MODULE_ALLOCATOR, sink), format, levels, present);
}

static inline int log_sink_init_filesystem(uint8_t sink, int format, int levels, int present) {
	return log_sink_init(log_sink_handle(MODULE_FILESYSTEM, sink), format, levels, present);
}

static inline int log_sink_init_scripting(uint8_t sink, int format, int levels, int present) {
	return log_sink_init(log_sink_handle(MODULE_SCRIPTING, sink), format, levels, present);
}

static inline int log_sink_init_rendering(uint8_t sink, int format, int levels, int present) {
	return log_sink_init(log_sink_handle(MODULE_RENDERING, sink), format, levels, present);
}

static inline int log_sink_init_audio(uint8_t sink, int format, int levels, int present) {
	return log_sink_init(log_sink_handle(MODULE_AUDIO, sink), format, levels, present);
}

static inline int log_sink_init_physics(uint8_t sink, int format, int levels, int present) {
	return log_sink_init(log_sink_handle(MODULE_PHYSICS, sink), format, levels, present);
}

static inline int log_sink_init_networking(uint8_t sink, int format, int levels, int present) {
	return log_sink_init(log_sink_handle(MODULE_NETWORKING, sink), format, levels, present);
}

#ifndef DESCENT_LOG_DISABLE_TRACE
#define LOG_TRACE(module, ...) log_message(module, LOG_LEVEL_TRACE, __VA_ARGS__)

#ifndef DESCENT_LOG_DISABLE_CORE_TRACE
#define CORE_TRACE(...) log_message(MODULE_CORE, LOG_LEVEL_TRACE, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_LOGGING_TRACE
#define LOGGING_TRACE(...) log_message(MODULE_LOGGING, LOG_LEVEL_TRACE, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_THREADING_TRACE
#define THREADING_TRACE(...) log_message(MODULE_THREADING, LOG_LEVEL_TRACE, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_ALLOCATOR_TRACE
#define ALLOCATOR_TRACE(...) log_message(MODULE_ALLOCATOR, LOG_LEVEL_TRACE, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_FILESYSTEM_TRACE
#define FILESYSTEM_TRACE(...) log_message(MODULE_FILESYSTEM, LOG_LEVEL_TRACE, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_SCRIPTING_TRACE
#define SCRIPTING_TRACE(...) log_message(MODULE_SCRIPTING, LOG_LEVEL_TRACE, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_RENDERING_TRACE
#define RENDERING_TRACE(...) log_message(MODULE_RENDERING, LOG_LEVEL_TRACE, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_AUDIO_TRACE
#define AUDIO_TRACE(...) log_message(MODULE_AUDIO, LOG_LEVEL_TRACE, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_PHYSICS_TRACE
#define PHYSICS_TRACE(...) log_message(MODULE_PHYSICS, LOG_LEVEL_TRACE, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_NETWORKING_TRACE
#define NETWORKING_TRACE(...) log_message(MODULE_NETWORKING, LOG_LEVEL_TRACE, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_USER_TRACE
#define USER_TRACE(...) log_message(MODULE_USER, LOG_LEVEL_TRACE, __VA_ARGS__)
#endif
#endif

#ifndef DESCENT_LOG_DISABLE_INFO
#define LOG_INFO(module, ...) log_message(module, LOG_LEVEL_INFO, __VA_ARGS__)

#ifndef DESCENT_LOG_DISABLE_CORE_INFO
#define CORE_INFO(...) log_message(MODULE_CORE, LOG_LEVEL_INFO, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_LOGGING_INFO
#define LOGGING_INFO(...) log_message(MODULE_LOGGING, LOG_LEVEL_INFO, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_THREADING_INFO
#define THREADING_INFO(...) log_message(MODULE_THREADING, LOG_LEVEL_INFO, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_ALLOCATOR_INFO
#define ALLOCATOR_INFO(...) log_message(MODULE_ALLOCATOR, LOG_LEVEL_INFO, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_FILESYSTEM_INFO
#define FILESYSTEM_INFO(...) log_message(MODULE_FILESYSTEM, LOG_LEVEL_INFO, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_SCRIPTING_INFO
#define SCRIPTING_INFO(...) log_message(MODULE_SCRIPTING, LOG_LEVEL_INFO, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_RENDERING_INFO
#define RENDERING_INFO(...) log_message(MODULE_RENDERING, LOG_LEVEL_INFO, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_AUDIO_INFO
#define AUDIO_INFO(...) log_message(MODULE_AUDIO, LOG_LEVEL_INFO, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_PHYSICS_INFO
#define PHYSICS_INFO(...) log_message(MODULE_PHYSICS, LOG_LEVEL_INFO, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_NETWORKING_INFO
#define NETWORKING_INFO(...) log_message(MODULE_NETWORKING, LOG_LEVEL_INFO, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_USER_INFO
#define USER_INFO(...) log_message(MODULE_USER, LOG_LEVEL_INFO, __VA_ARGS__)
#endif
#endif

#ifndef DESCENT_LOG_DISABLE_DEBUG
#define LOG_DEBUG(module, ...) log_message(module, LOG_LEVEL_DEBUG, __VA_ARGS__)

#ifndef DESCENT_LOG_DISABLE_CORE_DEBUG
#define CORE_DEBUG(...) log_message(MODULE_CORE, LOG_LEVEL_DEBUG, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_LOGGING_DEBUG
#define LOGGING_DEBUG(...) log_message(MODULE_LOGGING, LOG_LEVEL_DEBUG, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_THREADING_DEBUG
#define THREADING_DEBUG(...) log_message(MODULE_THREADING, LOG_LEVEL_DEBUG, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_ALLOCATOR_DEBUG
#define ALLOCATOR_DEBUG(...) log_message(MODULE_ALLOCATOR, LOG_LEVEL_DEBUG, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_FILESYSTEM_DEBUG
#define FILESYSTEM_DEBUG(...) log_message(MODULE_FILESYSTEM, LOG_LEVEL_DEBUG, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_SCRIPTING_DEBUG
#define SCRIPTING_DEBUG(...) log_message(MODULE_SCRIPTING, LOG_LEVEL_DEBUG, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_RENDERING_DEBUG
#define RENDERING_DEBUG(...) log_message(MODULE_RENDERING, LOG_LEVEL_DEBUG, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_AUDIO_DEBUG
#define AUDIO_DEBUG(...) log_message(MODULE_AUDIO, LOG_LEVEL_DEBUG, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_PHYSICS_DEBUG
#define PHYSICS_DEBUG(...) log_message(MODULE_PHYSICS, LOG_LEVEL_DEBUG, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_NETWORKING_DEBUG
#define NETWORKING_DEBUG(...) log_message(MODULE_NETWORKING, LOG_LEVEL_DEBUG, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_USER_DEBUG
#define USER_DEBUG(...) log_message(MODULE_USER, LOG_LEVEL_DEBUG, __VA_ARGS__)
#endif
#endif

#ifndef DESCENT_LOG_DISABLE_WARN
#define LOG_WARN(module, ...) log_message(module, LOG_LEVEL_WARN, __VA_ARGS__)

#ifndef DESCENT_LOG_DISABLE_CORE_WARN
#define CORE_WARN(...) log_message(MODULE_CORE, LOG_LEVEL_WARN, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_LOGGING_WARN
#define LOGGING_WARN(...) log_message(MODULE_LOGGING, LOG_LEVEL_WARN, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_THREADING_WARN
#define THREADING_WARN(...) log_message(MODULE_THREADING, LOG_LEVEL_WARN, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_ALLOCATOR_WARN
#define ALLOCATOR_WARN(...) log_message(MODULE_ALLOCATOR, LOG_LEVEL_WARN, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_FILESYSTEM_WARN
#define FILESYSTEM_WARN(...) log_message(MODULE_FILESYSTEM, LOG_LEVEL_WARN, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_SCRIPTING_WARN
#define SCRIPTING_WARN(...) log_message(MODULE_SCRIPTING, LOG_LEVEL_WARN, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_RENDERING_WARN
#define RENDERING_WARN(...) log_message(MODULE_RENDERING, LOG_LEVEL_WARN, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_AUDIO_WARN
#define AUDIO_WARN(...) log_message(MODULE_AUDIO, LOG_LEVEL_WARN, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_PHYSICS_WARN
#define PHYSICS_WARN(...) log_message(MODULE_PHYSICS, LOG_LEVEL_WARN, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_NETWORKING_WARN
#define NETWORKING_WARN(...) log_message(MODULE_NETWORKING, LOG_LEVEL_WARN, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_USER_WARN
#define USER_WARN(...) log_message(MODULE_USER, LOG_LEVEL_WARN, __VA_ARGS__)
#endif
#endif

#ifndef DESCENT_LOG_DISABLE_ERROR
#define LOG_ERROR(module, ...) log_message(module, LOG_LEVEL_ERROR, __VA_ARGS__)

#ifndef DESCENT_LOG_DISABLE_CORE_ERROR
#define CORE_ERROR(...) log_message(MODULE_CORE, LOG_LEVEL_ERROR, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_LOGGING_ERROR
#define LOGGING_ERROR(...) log_message(MODULE_LOGGING, LOG_LEVEL_ERROR, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_THREADING_ERROR
#define THREADING_ERROR(...) log_message(MODULE_THREADING, LOG_LEVEL_ERROR, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_ALLOCATOR_ERROR
#define ALLOCATOR_ERROR(...) log_message(MODULE_ALLOCATOR, LOG_LEVEL_ERROR, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_FILESYSTEM_ERROR
#define FILESYSTEM_ERROR(...) log_message(MODULE_FILESYSTEM, LOG_LEVEL_ERROR, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_SCRIPTING_ERROR
#define SCRIPTING_ERROR(...) log_message(MODULE_SCRIPTING, LOG_LEVEL_ERROR, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_RENDERING_ERROR
#define RENDERING_ERROR(...) log_message(MODULE_RENDERING, LOG_LEVEL_ERROR, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_AUDIO_ERROR
#define AUDIO_ERROR(...) log_message(MODULE_AUDIO, LOG_LEVEL_ERROR, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_PHYSICS_ERROR
#define PHYSICS_ERROR(...) log_message(MODULE_PHYSICS, LOG_LEVEL_ERROR, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_NETWORKING_ERROR
#define NETWORKING_ERROR(...) log_message(MODULE_NETWORKING, LOG_LEVEL_ERROR, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_USER_ERROR
#define USER_ERROR(...) log_message(MODULE_USER, LOG_LEVEL_ERROR, __VA_ARGS__)
#endif
#endif

#ifndef DESCENT_LOG_DISABLE_FATAL
#define LOG_FATAL(module, ...) log_message(module, LOG_LEVEL_FATAL, __VA_ARGS__)

#ifndef DESCENT_LOG_DISABLE_CORE_FATAL
#define CORE_FATAL(...) log_message(MODULE_CORE, LOG_LEVEL_FATAL, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_LOGGING_FATAL
#define LOGGING_FATAL(...) log_message(MODULE_LOGGING, LOG_LEVEL_FATAL, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_THREADING_FATAL
#define THREADING_FATAL(...) log_message(MODULE_THREADING, LOG_LEVEL_FATAL, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_ALLOCATOR_FATAL
#define ALLOCATOR_FATAL(...) log_message(MODULE_ALLOCATOR, LOG_LEVEL_FATAL, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_FILESYSTEM_FATAL
#define FILESYSTEM_FATAL(...) log_message(MODULE_FILESYSTEM, LOG_LEVEL_FATAL, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_SCRIPTING_FATAL
#define SCRIPTING_FATAL(...) log_message(MODULE_SCRIPTING, LOG_LEVEL_FATAL, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_RENDERING_FATAL
#define RENDERING_FATAL(...) log_message(MODULE_RENDERING, LOG_LEVEL_FATAL, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_AUDIO_FATAL
#define AUDIO_FATAL(...) log_message(MODULE_AUDIO, LOG_LEVEL_FATAL, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_PHYSICS_FATAL
#define PHYSICS_FATAL(...) log_message(MODULE_PHYSICS, LOG_LEVEL_FATAL, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_NETWORKING_FATAL
#define NETWORKING_FATAL(...) log_message(MODULE_NETWORKING, LOG_LEVEL_FATAL, __VA_ARGS__)
#endif
#ifndef DESCENT_LOG_DISABLE_USER_FATAL
#define USER_FATAL(...) log_message(MODULE_USER, LOG_LEVEL_FATAL, __VA_ARGS__)
#endif
#endif

#ifndef LOG_TRACE
#define LOG_TRACE(module, ...) ((void) 0)
#endif
#ifndef CORE_TRACE
#define CORE_TRACE(...) ((void) 0)
#endif
#ifndef LOGGING_TRACE
#define LOGGING_TRACE(...) ((void) 0)
#endif
#ifndef THREADING_TRACE
#define THREADING_TRACE(...) ((void) 0)
#endif
#ifndef ALLOCATOR_TRACE
#define ALLOCATOR_TRACE(...) ((void) 0)
#endif
#ifndef FILESYSTEM_TRACE
#define FILESYSTEM_TRACE(...) ((void) 0)
#endif
#ifndef SCRIPTING_TRACE
#define SCRIPTING_TRACE(...) ((void) 0)
#endif
#ifndef RENDERING_TRACE
#define RENDERING_TRACE(...) ((void) 0)
#endif
#ifndef AUDIO_TRACE
#define AUDIO_TRACE(...) ((void) 0)
#endif
#ifndef PHYSICS_TRACE
#define PHYSICS_TRACE(...) ((void) 0)
#endif
#ifndef NETWORKING_TRACE
#define NETWORKING_TRACE(...) ((void) 0)
#endif
#ifndef USER_TRACE
#define USER_TRACE(...) ((void) 0)
#endif

#ifndef LOG_INFO
#define LOG_INFO(module, ...) ((void)0)
#endif
#ifndef CORE_INFO
#define CORE_INFO(...) ((void) 0)
#endif
#ifndef LOGGING_INFO
#define LOGGING_INFO(...) ((void) 0)
#endif
#ifndef THREADING_INFO
#define THREADING_INFO(...) ((void) 0)
#endif
#ifndef ALLOCATOR_INFO
#define ALLOCATOR_INFO(...) ((void) 0)
#endif
#ifndef FILESYSTEM_INFO
#define FILESYSTEM_INFO(...) ((void) 0)
#endif
#ifndef SCRIPTING_INFO
#define SCRIPTING_INFO(...) ((void) 0)
#endif
#ifndef RENDERING_INFO
#define RENDERING_INFO(...) ((void) 0)
#endif
#ifndef AUDIO_INFO
#define AUDIO_INFO(...) ((void) 0)
#endif
#ifndef PHYSICS_INFO
#define PHYSICS_INFO(...) ((void) 0)
#endif
#ifndef NETWORKING_INFO
#define NETWORKING_INFO(...) ((void) 0)
#endif
#ifndef USER_INFO
#define USER_INFO(...) ((void) 0)
#endif

#ifndef LOG_DEBUG
#define LOG_DEBUG(module, ...) ((void)0)
#endif
#ifndef CORE_DEBUG
#define CORE_DEBUG(...) ((void) 0)
#endif
#ifndef LOGGING_DEBUG
#define LOGGING_DEBUG(...) ((void) 0)
#endif
#ifndef THREADING_DEBUG
#define THREADING_DEBUG(...) ((void) 0)
#endif
#ifndef ALLOCATOR_DEBUG
#define ALLOCATOR_DEBUG(...) ((void) 0)
#endif
#ifndef FILESYSTEM_DEBUG
#define FILESYSTEM_DEBUG(...) ((void) 0)
#endif
#ifndef SCRIPTING_DEBUG
#define SCRIPTING_DEBUG(...) ((void) 0)
#endif
#ifndef RENDERING_DEBUG
#define RENDERING_DEBUG(...) ((void) 0)
#endif
#ifndef AUDIO_DEBUG
#define AUDIO_DEBUG(...) ((void) 0)
#endif
#ifndef PHYSICS_DEBUG
#define PHYSICS_DEBUG(...) ((void) 0)
#endif
#ifndef NETWORKING_DEBUG
#define NETWORKING_DEBUG(...) ((void) 0)
#endif
#ifndef USER_DEBUG
#define USER_DEBUG(...) ((void) 0)
#endif

#ifndef LOG_WARN
#define LOG_WARN(module, ...) ((void)0)
#endif
#ifndef CORE_WARN
#define CORE_WARN(...) ((void) 0)
#endif
#ifndef LOGGING_WARN
#define LOGGING_WARN(...) ((void) 0)
#endif
#ifndef THREADING_WARN
#define THREADING_WARN(...) ((void) 0)
#endif
#ifndef ALLOCATOR_WARN
#define ALLOCATOR_WARN(...) ((void) 0)
#endif
#ifndef FILESYSTEM_WARN
#define FILESYSTEM_WARN(...) ((void) 0)
#endif
#ifndef SCRIPTING_WARN
#define SCRIPTING_WARN(...) ((void) 0)
#endif
#ifndef RENDERING_WARN
#define RENDERING_WARN(...) ((void) 0)
#endif
#ifndef AUDIO_WARN
#define AUDIO_WARN(...) ((void) 0)
#endif
#ifndef PHYSICS_WARN
#define PHYSICS_WARN(...) ((void) 0)
#endif
#ifndef NETWORKING_WARN
#define NETWORKING_WARN(...) ((void) 0)
#endif
#ifndef USER_WARN
#define USER_WARN(...) ((void) 0)
#endif

#ifndef LOG_ERROR
#define LOG_ERROR(module, ...) ((void)0)
#endif
#ifndef CORE_ERROR
#define CORE_ERROR(...) ((void) 0)
#endif
#ifndef LOGGING_ERROR
#define LOGGING_ERROR(...) ((void) 0)
#endif
#ifndef THREADING_ERROR
#define THREADING_ERROR(...) ((void) 0)
#endif
#ifndef ALLOCATOR_ERROR
#define ALLOCATOR_ERROR(...) ((void) 0)
#endif
#ifndef FILESYSTEM_ERROR
#define FILESYSTEM_ERROR(...) ((void) 0)
#endif
#ifndef SCRIPTING_ERROR
#define SCRIPTING_ERROR(...) ((void) 0)
#endif
#ifndef RENDERING_ERROR
#define RENDERING_ERROR(...) ((void) 0)
#endif
#ifndef AUDIO_ERROR
#define AUDIO_ERROR(...) ((void) 0)
#endif
#ifndef PHYSICS_ERROR
#define PHYSICS_ERROR(...) ((void) 0)
#endif
#ifndef NETWORKING_ERROR
#define NETWORKING_ERROR(...) ((void) 0)
#endif
#ifndef USER_ERROR
#define USER_ERROR(...) ((void) 0)
#endif

#ifndef LOG_FATAL
#define LOG_FATAL(module, ...) ((void)0)
#endif
#ifndef CORE_FATAL
#define CORE_FATAL(...) ((void) 0)
#endif
#ifndef LOGGING_FATAL
#define LOGGING_FATAL(...) ((void) 0)
#endif
#ifndef THREADING_FATAL
#define THREADING_FATAL(...) ((void) 0)
#endif
#ifndef ALLOCATOR_FATAL
#define ALLOCATOR_FATAL(...) ((void) 0)
#endif
#ifndef FILESYSTEM_FATAL
#define FILESYSTEM_FATAL(...) ((void) 0)
#endif
#ifndef SCRIPTING_FATAL
#define SCRIPTING_FATAL(...) ((void) 0)
#endif
#ifndef RENDERING_FATAL
#define RENDERING_FATAL(...) ((void) 0)
#endif
#ifndef AUDIO_FATAL
#define AUDIO_FATAL(...) ((void) 0)
#endif
#ifndef PHYSICS_FATAL
#define PHYSICS_FATAL(...) ((void) 0)
#endif
#ifndef NETWORKING_FATAL
#define NETWORKING_FATAL(...) ((void) 0)
#endif
#ifndef USER_FATAL
#define USER_FATAL(...) ((void) 0)
#endif

#endif