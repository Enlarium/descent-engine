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

#ifndef DESCENT_SOURCE_CLI_FIND_H
#define DESCENT_SOURCE_CLI_FIND_H

#include <descent/cli.h>

#include "context.h"

static inline CLI_Parameter *cli_find_subcommand(CLI_ParseContext *c, const char *name) {
	// printf("Finding subcommand: %s\n", name);

	unsigned int left  = c->first_subcommand;
	unsigned int right = c->first_long_option - 1;

	// unsigned int i = 0;
	while (left <= right) {
		// printf("  binary search step %u\n", i++);
		unsigned int mid = left + (right - left) / 2;
		CLI_Parameter *par = &c->parameters[mid];

		int cmp = strcmp(name, par->name_long);
		if (cmp > 0) left  = mid + 1;
		if (cmp < 0) right = mid - 1;
		else {
			// printf("Found subcommand: %s\n", par->name_long);
			return par;
		}
	}

	return NULL;
}

static inline CLI_Parameter *cli_find_long(CLI_ParseContext *c, const char *name) {
	// printf("Finding long option: %s\n", name);

	unsigned int left  = c->first_long_option;
	unsigned int right = c->parameter_count - 1;

	// unsigned int i = 0;
	while (left <= right) {
		// printf("  binary search step %u\n", i++);
		unsigned int mid = left + (right - left) / 2;
		CLI_Parameter *par = &c->parameters[mid];

		int cmp = strcmp(name, par->name_long);
		if (cmp > 0) left  = mid + 1;
		if (cmp < 0) right = mid - 1;
		else {
			// printf("Found long option: %s\n", par->name_long);
			return par;
		}
	}

	return NULL;
}

static inline CLI_Parameter *cli_find_short(CLI_ParseContext *c, char n) {
	// printf("Finding short option: %c\n", n);

	if (cli_is_valid_short_name(n)) return c->shorts[cli_short_to_index(n)];
	return NULL;
}

static inline CLI_Parameter *cli_find_positional(CLI_ParseContext *c) {
	// printf("Finding positional: %u\n", c->positional_index);

	unsigned int index = c->positional_index - 1;

	if (index <= DESCENT_CLI_MAX_POSITIONALS && c->positionals[index]) {
		return c->positionals[index];
	}
	
	return NULL;
}

static inline CLI_Parameter *cli_find_catchall(CLI_ParseContext *c) {
	// printf("Finding catchall\n");
	return c->catchall;
}

#endif
