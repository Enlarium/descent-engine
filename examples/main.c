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

#include <descent/thread/thread.h>
#include <descent/core.h>
//#include <descent/log.h>
//#include <descent/cli.h>
#include <descent/rcode.h>
//#include <descent/script.h>
//#include <descent/file.h>
//
//#include "cli.h"
//
//#include "../src/file/path.h"

int render_function(void *arg) {
	(void) arg;
	puts("Hello from Render");
	return 0;
}

int audio_function(void *arg) {
	(void) arg;
	puts("Hello from Audio");
	return 0;
}

int network_function(void *arg) {
	(void) arg;
	puts("Hello from Network");
	return 0;
}

int worker_function(void *arg) {
	(void) arg;
	puts("Hello from Worker");
	return 0;
}

int main(void) {

	descent_init();

	printf("spawn render:    %s\n", rcode_string(thread_spawn_unique(0, render_function, NULL, "Render")));
	printf("spawn audio:     %s\n", rcode_string(thread_spawn_unique(1, audio_function, NULL, "Audio")));
	printf("spawn network:   %s\n", rcode_string(thread_spawn_unique(2, network_function, NULL, "Network")));
	printf("spawn worker:    %s\n", rcode_string(thread_spawn_worker(12, worker_function, NULL)));
	printf("collect render:  %s\n", rcode_string(thread_collect_unique(0)));
	printf("collect audio:   %s\n", rcode_string(thread_collect_unique(1)));
	printf("collect network: %s\n", rcode_string(thread_collect_unique(2)));
	printf("collect worker:  %s\n", rcode_string(thread_collect_worker()));

//	int result = file_init_virtual_roots("descent");
//	if (result) {
//		printf("Virtual root initialization failed with code %s (%s)\n", code_string(result), code_message(result));
//		return result;
//	}
//
//	printf("Program root: %s\n", path_get_program_root()->string);
//	printf("Config root:  %s\n", path_get_config_root()->string);
//	printf("Cache root:   %s\n", path_get_cache_root()->string);
//	printf("Temp root:    %s\n", path_get_temp_root()->string);
//
//	const char *dir_path = "//CONFIG/for_you/";
//	const char *file_path = "//CONFIG/for_you/message.txt";
//	const char *msg = "\tSed ut perspiciatis, unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem aperiam eaque ipsa, quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt, explicabo. Nemo enim ipsam voluptatem, quia voluptas sit, aspernatur aut odit aut fugit, sed quia consequuntur magni dolores eos, qui ratione voluptatem sequi nesciunt, neque porro quisquam est, qui dolorem ipsum, quia dolor sit amet consectetur adipisci[ng] velit, sed quia non numquam [do] eius modi tempora inci[di]dunt, ut labore et dolore magnam aliquam quaerat voluptatem. Ut enim ad minima veniam, quis nostrum[d] exercitationem ullam corporis suscipit laboriosam, nisi ut aliquid ex ea commodi consequatur? Quis autem vel eum i[r]ure reprehenderit, qui in ea voluptate velit esse, quam nihil molestiae consequatur, vel illum, qui dolorem eum fugiat, quo voluptas nulla pariatur?\n\tAt vero eos et accusamus et iusto odio dignissimos ducimus, qui blanditiis praesentium voluptatum deleniti atque corrupti, quos dolores et quas molestias excepturi sint, obcaecati cupiditate non provident, similique sunt in culpa, qui officia deserunt mollitia animi, id est laborum et dolorum fuga. Et harum quidem reru[d]um facilis est e[r]t expedita distinctio. Nam libero tempore, cum soluta nobis est eligendi optio, cumque nihil impedit, quo minus id, quod maxime placeat facere possimus, omnis voluptas assumenda est, omnis dolor repellend[a]us. Temporibus autem quibusdam et aut officiis debitis aut rerum necessitatibus saepe eveniet, ut et voluptates repudiandae sint et molestiae non recusandae. Itaque earum rerum hic tenetur a sapiente delectus, ut aut reiciendis voluptatibus maiores alias consequatur aut perferendis doloribus asperiores repellat.\n";
//	char buf[10240] = {0};
//
//	size_t msg_len = strlen(msg);
//	size_t msg_n = 3;
//
//	printf("Sub-Message Length: %zu\n", msg_len);
//	printf("Total Message Length: %zu\n", msg_len * msg_n);
//
//	result = dir_create_recursive(dir_path);
//	if (result) {
//		printf("Failed to recursively create directory %s, failed with code %s (%s)\n", dir_path, code_string(result), code_message(result));
//	}
//
//	FileOpenOptions foo = {
//		.buffer_size = 0x1000
//	};
//
//	File* f;
//	result = file_open_ex(&f, "//CONFIG/for_you/message.txt", FILE_WR, &foo);
//	if (result) {
//		printf("Failed to open file %s, failed with code %s (%s)\n", file_path, code_string(result), code_message(result));
//		return result;
//	}
//
//	for (size_t i = 0; i < msg_n; ++i) {
//		result = file_write(f, msg_len, (void *)msg, NULL);
//		if (result) {
//			printf("Failed to write to file %s, failed with code %s (%s)\n", file_path, code_string(result), code_message(result));
//			return result;
//		}
//	}
//
//	result = file_seek(f, FILE_SEEK_SET, 0);
//	if (result) {
//		printf("Failed to seek in file %s, failed with code %s (%s)\n", file_path, code_string(result), code_message(result));
//		return result;
//	}
//
//	size_t bytes_read = 0;
//
//	result = file_read(f, sizeof(buf), (void *)buf, &bytes_read);
//	if (result) {
//		printf("Failed to read from file %s, failed with code %s (%s)\n", file_path, code_string(result), code_message(result));
//		return result;
//	}
//
//	printf("Read %zu bytes\n", bytes_read);
//	buf[msg_len * msg_n] = '\0';
//
//	result = file_close(&f);
//	if (result) {
//		printf("Failed to close file with code %s (%s)\n", code_string(result), code_message(result));
//		return result;
//	}
//
//	result = dir_remove_recursive(dir_path);
//	if (result) {
//		printf("Failed to recursively remove directory %s, failed with code %s (%s)\n", dir_path, code_string(result), code_message(result));
//	}
//
//	printf("Message from file: %s\n", buf);

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

	for (int i = 0; i < MODULE_COUNT; ++i) log_message(i, LOG_LEVEL_TRACE, "HELLO");

	atomic_store_32(&do_logging, 0);
	e = thread_join(t, NULL);
	if (e) printf("Could not join thread (%s)\n", code_string(e));
	log_close();
	return 0;
}


*/