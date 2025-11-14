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

#include <stdio.h>

#include <descent/thread.h>
#include <descent/logging.h>

atomic_32 do_logging = 1;

int tf(void *arg) {
	(void) arg;
	while(do_logging) log_write();
	return 0;
}

int main(void) {
	log_sink_init_base(0, LOG_FORMAT_FULL, LOG_LEVEL_ALL, LOG_PRESENT_AUTO);
	log_sink_init_logging(0, LOG_FORMAT_FULL, LOG_LEVEL_ALL, LOG_PRESENT_AUTO);
	log_sink_init_scripting(0, LOG_FORMAT_FULL, LOG_LEVEL_ALL, LOG_PRESENT_AUTO);
	log_sink_init_threading(0, LOG_FORMAT_FULL, LOG_LEVEL_ALL, LOG_PRESENT_AUTO);
	log_sink_init_threading(1, LOG_FORMAT_FULL, LOG_LEVEL_ALL, LOG_PRESENT_PLAIN);
	log_sink_file((LogSinkHandle) {.module = MODULE_THREADING, .sink = 1}, "./log", LOG_SINK_WRITE);
	log_sink_init_rendering(0, LOG_FORMAT_FULL, LOG_LEVEL_ALL, LOG_PRESENT_AUTO);
	log_sink_init_allocator(0, LOG_FORMAT_FULL, LOG_LEVEL_ALL, LOG_PRESENT_AUTO);

	Thread t;
	int e;
	
	e = thread_create(&t, tf, NULL, NULL);
	if (e) printf("Could not create new thread (%s)\n", thread_error_message(e));

	thread_sleep(1000000000);

	for (int i = 0; i < 100; ++i) {
		log_message(i % MODULE_COUNT, LOG_LEVEL_TRACE << ((i / MODULE_COUNT) % 7), "Hello World!");
	}

	thread_sleep(1000000000);
	atomic_store_32(&do_logging, 0);

	e = thread_join(t, NULL);
	if (e) printf("Could not join thread (%s)\n", thread_error_message(e));

	log_close();

	return 0;
}

/*
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <descent/thread/rwlock.h>

typedef enum {
	SCRIPT_ERROR_UNKNOWN = -1,
	SCRIPT_SUCCESS = 0,
	SCRIPT_ERROR_ALLOCATION_FAILURE,
} ScriptError;

// Limit cycles

// TODO: use custom script allocator instead
static lua_State *create_lua_sandbox(void) {
	// Create a new Lua state with default allocator
	lua_State *sandbox = luaL_newstate();

	// Abort if state could not be allocated
	if (!sandbox) return NULL;
	
	// Create a new sandbox environment
	lua_newtable(sandbox);
	int box_env = lua_gettop(sandbox);

	// Load a temporary base library
	luaL_requiref(sandbox, "_G", luaopen_base, 0);
	int base_env = lua_gettop(sandbox);

	// What libraries and functions should I allow?
	// I want to forbid loading arbitrary external modules
	// Printing and other OS interactions need to be managed through my own wrappers

	// Copy safe functions from the temporary base library
	lua_getfield(sandbox, base_env, "print");
	lua_setfield(sandbox, box_env, "print");
	lua_getfield(sandbox, base_env, "pairs");
	lua_setfield(sandbox, box_env, "pairs");
	lua_getfield(sandbox, base_env, "type");
	lua_setfield(sandbox, box_env, "type");

	// Pop the temporary base library
	lua_pop(sandbox, 1);

	// Add allowed libraries into the sandbox
	luaL_requiref(sandbox, "math", luaopen_math, 0);
	lua_setfield(sandbox, box_env, "math");
	luaL_requiref(sandbox, "string", luaopen_string, 0);
	lua_setfield(sandbox, box_env, "string");
	
	return sandbox;
}

int run_lua_script(const char *script_path) {
	lua_State *sandbox = create_lua_sandbox();

	if (!sandbox) return SCRIPT_ERROR_ALLOCATION_FAILURE;

	int box_env = lua_gettop(sandbox);

	if (luaL_loadfile(sandbox, script_path) != LUA_OK) {
		const char *err = lua_tostring(sandbox, -1);
		lua_close(sandbox);
		puts(err); // TODO
		return SCRIPT_ERROR_UNKNOWN;
	}

	lua_pushvalue(sandbox, box_env);      // push sandbox table
	lua_setupvalue(sandbox, -2, 1);       // set _ENV (first upvalue) of chunk

	if (lua_pcall(sandbox, 0, LUA_MULTRET, 0) != LUA_OK) {
		const char *err = lua_tostring(sandbox, -1);
		lua_close(sandbox);
		puts(err); // TODO
		return SCRIPT_ERROR_UNKNOWN;
	}

	lua_close(sandbox);

	return 0;
}

int main(void) {
	run_lua_script("../../game_logic.lua");
}
*/