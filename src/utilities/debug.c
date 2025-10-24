// Copyright 2025 XavierHarkonnen9 and Enlarium
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <descent/utilities/debug.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <descent/thread/mutex.h>

static Mutex log_lock = MUTEX_INITIALIZER;
static FILE *log_file = NULL;

int debug_log_open(const char* path) {

	FILE *file;
	
	if (!path || strncmp(path, "stderr", 7) == 0) {
		file = stderr;
	} else if (strncmp(path, "stdout", 7) == 0) {
		file = stdout;
	} else {
		file = fopen(path, "a");
	}
		
	mutex_lock(&log_lock);
	log_file = file;
	mutex_unlock(&log_lock);
	
	return !file;
}

void debug_log_close(void) {
	mutex_lock(&log_lock);
	if (log_file) {
		if (log_file != stdin && log_file != stderr && log_file != stdout) fclose(log_file);
		log_file = NULL;
	}
	mutex_unlock(&log_lock);
}

void debug_log(const char* context, const char* fmt, ...) {
	if (!log_file) return;

	time_t t = time(NULL);
	struct tm tm;

	localtime_r(&t, &tm);

	char timestr[32];
	strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", &tm);

	mutex_lock(&log_lock);

	fprintf(log_file, "[%s] [%s] ", timestr, context);

	va_list args;
	va_start(args, fmt);
	vfprintf(log_file, fmt, args);
	va_end(args);

	fputc('\n', log_file);
	fflush(log_file);

	mutex_unlock(&log_lock);
}
