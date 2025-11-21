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

#include <descent/script.h>

#include <stddef.h>

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include <descent/log.h>
#include <descent/modules.h>

#include "wrapper.h"

void transfer_field(lua_State *state, int base, int box, const char *field) {
	lua_getfield(state, base, field);
	lua_setfield(state, box, field);
}

lua_State *create_lua_sandbox(lua_Alloc allocator, void *ud) {
	lua_State *sandbox = lua_newstate(allocator, ud);

	// Abort if state could not be allocated
	if (!sandbox) return NULL;
	
	// Create a new sandbox environment
	lua_newtable(sandbox);
	int box_env = lua_gettop(sandbox);

	// Load a temporary base library
	luaL_requiref(sandbox, "_G", luaopen_base, 0);
	int base_env = lua_gettop(sandbox);

	// Copy safe functions from the temporary base library
	transfer_field(sandbox, base_env, box_env, "pairs");
	transfer_field(sandbox, base_env, box_env, "ipairs");
	transfer_field(sandbox, base_env, box_env, "type");
	transfer_field(sandbox, base_env, box_env, "tonumber");
	transfer_field(sandbox, base_env, box_env, "tostring");
	transfer_field(sandbox, base_env, box_env, "assert");
	transfer_field(sandbox, base_env, box_env, "error");
	// Are there any additional safe functions?

	// Pop the temporary base library
	lua_pop(sandbox, 1);

	// Add allowed libraries into the sandbox
	luaL_requiref(sandbox, "math", luaopen_math, 0);
	lua_setfield(sandbox, box_env, "math");
	luaL_requiref(sandbox, "string", luaopen_string, 0);
	lua_setfield(sandbox, box_env, "string");
	// Are there any additional safe libraries?

	// Add custom libraries into the sandbox
	luaL_requiref(sandbox, "logging", luaopen_logging, 0);
	lua_setfield(sandbox, box_env, "logging");

	return sandbox;
}

int run_lua_script(const char *script_path, lua_Alloc allocator, void *ud) {
	lua_State *sandbox = create_lua_sandbox(allocator, ud);

	if (!sandbox) return SCRIPT_ERROR_ALLOCATION_FAILURE;

	int box_env = lua_gettop(sandbox);

	if (luaL_loadfile(sandbox, script_path) != LUA_OK) {
		const char *err = lua_tostring(sandbox, -1);
		lua_close(sandbox);
		LOG_ERROR(MODULE_SCRIPTING, err);
		return SCRIPT_ERROR_UNKNOWN;
	}

	lua_pushvalue(sandbox, box_env); // push sandbox table
	lua_setupvalue(sandbox, -2, 1);  // set _ENV (first upvalue) of chunk

	if (lua_pcall(sandbox, 0, LUA_MULTRET, 0) != LUA_OK) {
		const char *err = lua_tostring(sandbox, -1);
		lua_close(sandbox);
		LOG_ERROR(MODULE_SCRIPTING, err);
		return SCRIPT_ERROR_UNKNOWN;
	}

	lua_close(sandbox);

	return 0;
}
