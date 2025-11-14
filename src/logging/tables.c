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

#include "tables.h"

#include <descent/modules.h>

#define LOG_COLOR_CLEAR "\033[0m"

#define LOG_COLOR_MODULE_BASE      ""
#define LOG_COLOR_MODULE_LOGGING   "\033[1m\033[90m"
#define LOG_COLOR_MODULE_RENDERING "\033[1m\033[31m"
#define LOG_COLOR_MODULE_SCRIPTING "\033[1m\033[34m"
#define LOG_COLOR_MODULE_THREADING "\033[1m\033[32m"
#define LOG_COLOR_MODULE_ALLOCATOR "\033[1m\033[35m"

#define LOG_COLOR_LEVEL_TRACE "\033[90m"
#define LOG_COLOR_LEVEL_INFO  "\033[32m"
#define LOG_COLOR_LEVEL_DEBUG "\033[34m"
#define LOG_COLOR_LEVEL_WARN  "\033[33m"
#define LOG_COLOR_LEVEL_ERROR "\033[91m"
#define LOG_COLOR_LEVEL_FATAL "\033[31m"

const char *log_module_strings_plain[] = {
	"DESCENT",
	"LOGGING",
	"RENDERING",
	"SCRIPTING",
	"THREADING",
	"ALLOCATOR",
};

const char *log_module_strings_styled[] = {
	LOG_COLOR_MODULE_BASE      "DESCENT"   LOG_COLOR_CLEAR,
	LOG_COLOR_MODULE_LOGGING   "LOGGING"   LOG_COLOR_CLEAR,
	LOG_COLOR_MODULE_RENDERING "RENDERING" LOG_COLOR_CLEAR,
	LOG_COLOR_MODULE_SCRIPTING "SCRIPTING" LOG_COLOR_CLEAR,
	LOG_COLOR_MODULE_THREADING "THREADING" LOG_COLOR_CLEAR,
	LOG_COLOR_MODULE_ALLOCATOR "ALLOCATOR" LOG_COLOR_CLEAR,
};

const char *log_level_strings_plain[] = {
	"TRACE",
	"INFO",
	"DEBUG",
	"WARN",
	"ERROR",
	"FATAL",
};

const char *log_level_strings_styled[] = {
	LOG_COLOR_LEVEL_TRACE "TRACE" LOG_COLOR_CLEAR,
	LOG_COLOR_LEVEL_INFO  "INFO"  LOG_COLOR_CLEAR,
	LOG_COLOR_LEVEL_DEBUG "DEBUG" LOG_COLOR_CLEAR,
	LOG_COLOR_LEVEL_WARN  "WARN"  LOG_COLOR_CLEAR,
	LOG_COLOR_LEVEL_ERROR "ERROR" LOG_COLOR_CLEAR,
	LOG_COLOR_LEVEL_FATAL "FATAL" LOG_COLOR_CLEAR,
};
