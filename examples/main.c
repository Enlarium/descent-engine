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

#include <stdio.h>
#include <stdlib.h>

#include <descent/thread.h>
#include <descent/log.h>
#include <descent/cli.h>
#include <descent/utilities/codes.h>
#include <descent/script.h>

#include "cli.h"

int main(int argc, const char *argv[]) {
	int verbose = 0;
	const char *output_file = NULL;

	CLI_Parameter parameters[] = {
		cli_create_option("verbose", 'v', 0, option_version),
		cli_create_option("help", 'h', 0, option_help),
		cli_create_catchall(option_catchall)
	};

	int result = cli_parse(argc, argv, sizeof(parameters)/sizeof(parameters[0]), parameters, NULL);

	if (result != 0) {
		const char *arg = cli_flagged_argument();
		char s = cli_flagged_short();
		fprintf(stderr, "Error: %s\n", code_string(result));
		if (arg) {
			fprintf(stderr, "Error parsing argument: %s\n", arg);
		} else if (s) {
			fprintf(stderr, "Error parsing short option: -%c\n", s);
		} else {
			fprintf(stderr, "Unknown parsing error\n");
		}
		return 1;
	}

	if (verbose) {
		printf("Verbose flag is set\n");
	}

	if (output_file) {
		printf("Final output file: %p\n", (void *)output_file);
		printf("Final output file: %s\n", output_file);
	}

	return 0;
}
/*

static void *lua_malloc_wrapper(void *ud, void *ptr, size_t osize, size_t nsize) {
	(void)ud;    // unused
	(void)osize; // Lua may pass old size, we don't need it for malloc/realloc

	if (nsize == 0) {
		free(ptr);
		return NULL;
	} else {
		return realloc(ptr, nsize);
	}
}

atomic_32 do_logging = 1;

int tf(void *arg) {
	(void) arg;
	thread_set_name(thread_name("Logging"));
	LOG_TRACE(MODULE_THREADING, "Thread name set to %s", thread_get_name().name);
	while(do_logging) log_write();
	return 0;
}

static const char *filenames[MODULE_COUNT] = {
	"./log/core",
	"./log/logging",
	"./log/threading",
	"./log/allocator",
	"./log/filesystem",
	"./log/scripting",
	"./log/rendering",
	"./log/audio",
	"./log/physics",
	"./log/networking",
	"./log/user"
};

int main(void) {

	for (int i = 0; i < MODULE_COUNT; ++i) log_sink_init((LogSinkHandle) {.module = i, .sink = 0}, LOG_FORMAT_FULL, LOG_LEVEL_ALL, LOG_PRESENT_AUTO);

	for (int i = 0; i < MODULE_COUNT; ++i) log_sink_init((LogSinkHandle) {.module = i, .sink = 1}, LOG_FORMAT_TIMESTAMP, LOG_LEVEL_ALL, LOG_PRESENT_PLAIN);
	for (int i = 0; i < MODULE_COUNT; ++i) {
		log_sink_file((LogSinkHandle) {.module = i, .sink = 1}, filenames[i] , LOG_SINK_WRITE);
	}
	
	Thread t;
	int e;
	e = thread_create(&t, tf, NULL, NULL);
	if (e) printf("Could not create new thread (%s)\n", code_string(e));

	run_lua_script("../../game_logic.lua", lua_malloc_wrapper, NULL);

	for (int i = 0; i < MODULE_COUNT; ++i) log_message(i, LOG_LEVEL_TRACE, "ELLO");

	atomic_store_32(&do_logging, 0);
	e = thread_join(t, NULL);
	if (e) printf("Could not join thread (%s)\n", code_string(e));
	log_close();
	return 0;
}


*/