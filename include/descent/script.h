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

#ifndef DESCENT_SCRIPT_H
#define DESCENT_SCRIPT_H

#include <lua.h>

typedef enum {
	SCRIPT_ERROR_UNKNOWN = -1,
	SCRIPT_SUCCESS = 0,
	SCRIPT_ERROR_ALLOCATION_FAILURE,
} ScriptError;

lua_State *create_lua_sandbox(lua_Alloc allocator, void *ud);

int run_lua_script(const char *script_path, lua_Alloc allocator, void *ud);

#endif