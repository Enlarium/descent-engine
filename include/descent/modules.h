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

#ifndef DESCENT_MODULES_H
#define DESCENT_MODULES_H

typedef enum {
	MODULE_BASE = 0,
	MODULE_LOGGING,
	MODULE_RENDERING,
	MODULE_SCRIPTING,
	MODULE_THREADING,
	MODULE_ALLOCATOR,
	MODULE_COUNT
} DescentModule;

static inline int log_module_valid(int module) {
	return (module >= MODULE_BASE && module < MODULE_COUNT);
}

#endif