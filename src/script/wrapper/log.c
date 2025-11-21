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

#include "../wrapper.h"

#include <stddef.h>

#include <lauxlib.h>
#include <lua.h>

#include <descent/modules.h>
#include <descent/log.h>

static int lua_log_trace(lua_State *L) {
	int n = lua_gettop(L);  // number of args
	const char *msg;

	if (n >= 2) {
		// Call Lua's string.format(fmt, ...)
		lua_getglobal(L, "string");
		lua_getfield(L, -1, "format");
		lua_remove(L, -2);
		lua_insert(L, 1);
		lua_call(L, n, 1);
		msg = luaL_checkstring(L, -1);
	} else {
		msg = luaL_checkstring(L, 1);
	}

	LOG_TRACE(MODULE_USER, "%s", msg);
	return 0;
}

static int lua_log_info(lua_State *L) {
	const char *msg = luaL_checkstring(L, 1);
	LOG_INFO(MODULE_USER, "%s", msg);
	return 0;
}

static int lua_log_debug(lua_State *L) {
	const char *msg = luaL_checkstring(L, 1);
	LOG_DEBUG(MODULE_USER, "%s", msg);
	return 0;
}

static int lua_log_warn(lua_State *L) {
	const char *msg = luaL_checkstring(L, 1);
	LOG_WARN(MODULE_USER, "%s", msg);
	return 0;
}

static int lua_log_error(lua_State *L) {
	const char *msg = luaL_checkstring(L, 1);
	LOG_ERROR(MODULE_USER, "%s", msg);
	return 0;
}

static int lua_log_fatal(lua_State *L) {
	const char *msg = luaL_checkstring(L, 1);
	LOG_FATAL(MODULE_USER, "%s", msg);
	return 0;
}

static const luaL_Reg logging_funcs[] = {
	{"trace", lua_log_trace},
	{"info",  lua_log_info},
	{"debug", lua_log_debug},
	{"warn",  lua_log_warn},
	{"error", lua_log_error},
	{"fatal", lua_log_fatal},
	{NULL, NULL}
};

int luaopen_logging(lua_State *L) {
	luaL_newlib(L, logging_funcs);
	return 1;
}
