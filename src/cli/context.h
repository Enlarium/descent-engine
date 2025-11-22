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

#ifndef DESCENT_SOURCE_CLI_CONTEXT_H
#define DESCENT_SOURCE_CLI_CONTEXT_H

#include <descent/cli.h>

typedef struct {
	CLI_Parameter *shorts[69];
	CLI_Parameter *positionals[DESCENT_CLI_MAX_POSITIONALS];
	CLI_Parameter *catchall;
	CLI_Parameter *parameters;

	void *settings;

	const char **arguments;

	unsigned int argument_count;
	unsigned int argument_index;
	unsigned int parameter_count;
	unsigned int positional_index;
	int parse_named;

	unsigned int first_subcommand;
	unsigned int first_long_option;
} CLI_ParseContext;

static inline int cli_short_to_index(char c) {
	if (c == '!') return 0;
	if (c >= '#' && c <= '&') return 1 + (c - '#');
	if (c >= '0' && c <= '9') return 5 + (c - '0');
	if (c == '?' || c == '@') return 15 + (c - '?');
	if (c >= 'A' && c <= 'Z') return 17 + (c - 'A');
	if (c >= 'a' && c <= 'z') return 43 + (c - 'a');
	return -1; // Not valid, should not happen if prechecked
}

#endif
