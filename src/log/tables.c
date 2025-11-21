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

#include "tables.h"

#include <descent/modules.h>

#define LOG_COLOR_CLEAR "\033[0m"

#define LOG_COLOR_MODULE_CORE        "\033[1m\033[38;2;54;44;125m"
#define LOG_COLOR_MODULE_LOGGING     "\033[1m\033[38;2;22;157;122m"
#define LOG_COLOR_MODULE_THREADING   "\033[1m\033[38;2;12;90;40m"
#define LOG_COLOR_MODULE_ALLOCATOR   "\033[1m\033[38;2;97;21;120m"
#define LOG_COLOR_MODULE_FILESYSTEM  "\033[1m\033[38;2;182;186;0m"
#define LOG_COLOR_MODULE_SCRIPTING   "\033[1m\033[38;2;24;24;184m"
#define LOG_COLOR_MODULE_RENDERING   "\033[1m\033[38;2;154;7;7m"
#define LOG_COLOR_MODULE_AUDIO       "\033[1m\033[38;2;24;127;218m"
#define LOG_COLOR_MODULE_PHYSICS     "\033[1m\033[38;2;220;114;9m"
#define LOG_COLOR_MODULE_NETWORKING  "\033[1m\033[38;2;190;10;180m"
#define LOG_COLOR_MODULE_USER        "\033[1m"

#define LOG_COLOR_LEVEL_TRACE "\033[90m"
#define LOG_COLOR_LEVEL_INFO  "\033[32m"
#define LOG_COLOR_LEVEL_DEBUG "\033[34m"
#define LOG_COLOR_LEVEL_WARN  "\033[33m"
#define LOG_COLOR_LEVEL_ERROR "\033[91m"
#define LOG_COLOR_LEVEL_FATAL "\033[31m"

const char *log_module_strings_plain[MODULE_COUNT] = {
	"DESCENT",
	"LOGGING",
	"THREADING",
	"ALLOCATOR",
	"FILESYSTEM",
	"SCRIPTING",
	"RENDERING",
	"AUDIO",
	"PHYSICS",
	"NETWORKING",
	"USER",
};

const char *log_module_strings_styled[MODULE_COUNT] = {
	LOG_COLOR_MODULE_CORE        "DESCENT"     LOG_COLOR_CLEAR,
	LOG_COLOR_MODULE_LOGGING     "LOGGING"     LOG_COLOR_CLEAR,
	LOG_COLOR_MODULE_THREADING   "THREADING"   LOG_COLOR_CLEAR,
	LOG_COLOR_MODULE_ALLOCATOR   "ALLOCATOR"   LOG_COLOR_CLEAR,
	LOG_COLOR_MODULE_FILESYSTEM  "FILESYSTEM"  LOG_COLOR_CLEAR,
	LOG_COLOR_MODULE_SCRIPTING   "SCRIPTING"   LOG_COLOR_CLEAR,
	LOG_COLOR_MODULE_RENDERING   "RENDERING"   LOG_COLOR_CLEAR,
	LOG_COLOR_MODULE_AUDIO       "AUDIO"       LOG_COLOR_CLEAR,
	LOG_COLOR_MODULE_PHYSICS     "PHYSICS"     LOG_COLOR_CLEAR,
	LOG_COLOR_MODULE_NETWORKING  "NETWORKING"  LOG_COLOR_CLEAR,
	LOG_COLOR_MODULE_USER        "USER"        LOG_COLOR_CLEAR,
};

const char *log_level_strings_plain[7] = {
	"TRACE",
	"INFO",
	"DEBUG",
	"WARN",
	"ERROR",
	"FATAL",
};

const char *log_level_strings_styled[7] = {
	LOG_COLOR_LEVEL_TRACE "TRACE" LOG_COLOR_CLEAR,
	LOG_COLOR_LEVEL_INFO  "INFO"  LOG_COLOR_CLEAR,
	LOG_COLOR_LEVEL_DEBUG "DEBUG" LOG_COLOR_CLEAR,
	LOG_COLOR_LEVEL_WARN  "WARN"  LOG_COLOR_CLEAR,
	LOG_COLOR_LEVEL_ERROR "ERROR" LOG_COLOR_CLEAR,
	LOG_COLOR_LEVEL_FATAL "FATAL" LOG_COLOR_CLEAR,
};
