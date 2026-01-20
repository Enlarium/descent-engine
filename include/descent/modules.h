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

#ifndef DESCENT_MODULES_H
#define DESCENT_MODULES_H

// TODO: Module "keys" so that different threads can claim to implement modules
// Core owns the main allocator
// Logging owns the logging allocator, scripting the scripting allocator, and so on

typedef enum {
	MODULE_CORE = 0,
	MODULE_ALLOCATOR,
	MODULE_CLI,
	MODULE_THREADING,
	MODULE_LOGGING,  // Logging related to the logging system itself. Should be rather more robust in case of general failure
	MODULE_FILESYSTEM,
	MODULE_SCRIPTING,
	MODULE_RENDERING,
	MODULE_AUDIO,
	MODULE_PHYSICS,
	MODULE_NETWORKING,
	MODULE_USER,
	MODULE_COUNT
} DescentModule;

/*
MODULE_PLATFORM
MODULE_RESOURCES
MODULE_SERIALIZATION
MODULE_AI
MODULE_INPUT
*/

static inline int log_module_valid(int module) {
	return (module >= MODULE_CORE && module < MODULE_COUNT);
}

#endif
