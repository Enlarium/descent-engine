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

#include <descent/cli.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <descent/thread/tls.h>
#include <descent/utilities/codes.h>

// TODO:
// - fast lookups for long names

typedef struct {
	const CLI_Parameter *shorts[69];
	const CLI_Parameter *positionals[DESCENT_CLI_MAX_POSITIONALS];
	const CLI_Parameter *catchall;
	const CLI_Parameter *parameters;
	void *settings;
	const char **arguments;
	unsigned int argument_count;
	unsigned int argument_index;
	unsigned int parameter_count;
	unsigned int positional_index;
	int parse_named;
} CLI_ParseContext;

// Track flagged argument for error reporting.
static TLS const char *flagged_argument;
static TLS char        flagged_short;

static inline int cli_short_to_index(char c) {
	if (c == '!') return 0;
	if (c >= '#' && c <= '&') return 1 + (c - '#');
	if (c >= '0' && c <= '9') return 5 + (c - '0');
	if (c == '?' || c == '@') return 15 + (c - '?');
	if (c >= 'A' && c <= 'Z') return 17 + (c - 'A');
	if (c >= 'a' && c <= 'z') return 43 + (c - 'a');
	return -1; // not valid, should not happen if prechecked
}

static inline int cli_prescan(CLI_ParseContext *c) {
	// Pre-scan all parameters to validate and find catchall
	for (unsigned int i = 0; i < c->parameter_count; ++i) {
		CLI_Parameter p = c->parameters[i];
		const CLI_Parameter *pp = &c->parameters[i];

		if (cli_is_subcommand(p)) {
			// Hashing would permit faster lookups
			for (unsigned int j = 0; j < i; ++j) {
				CLI_Parameter op = c->parameters[j];
				if (cli_is_subcommand(op) && op.name_long && p.name_long && !strcmp(op.name_long, p.name_long)) {
					return CLI_ERROR_DUPLICATE_PARAMETER;
				}
			}
		} else if (cli_is_option(p)) {
			if (cli_is_long_option(p)) {
				// Hashing would permit faster lookups
				for (unsigned int j = 0; j < i; ++j) {
					CLI_Parameter op = c->parameters[j];
					if (cli_is_long_option(op) && op.name_long && p.name_long && !strcmp(op.name_long, p.name_long)) {
						return CLI_ERROR_DUPLICATE_PARAMETER;
					}
				}
			}
			if (cli_is_short_option(p)) {
				int index = cli_short_to_index(p.name_short);
				if (!c->shorts[index]) c->shorts[index] = pp;
				else return CLI_ERROR_DUPLICATE_PARAMETER;
			}
		} else if (cli_is_positional(p)) {
				int index = p.position - 1;
				if (!c->positionals[index]) c->positionals[index] = pp;
				else return CLI_ERROR_DUPLICATE_PARAMETER;
		} else if (cli_is_catchall(p)) {
			if (!c->catchall) c->catchall = pp;
			else return CLI_ERROR_DUPLICATE_PARAMETER;
		} else {
			return CLI_ERROR_INVALID_PARAMETER;
		}
	}

	return 0;
}

static inline int handle_long_option(CLI_ParseContext *c) {
	const char *argument = c->arguments[c->argument_index];

	// Check if the option matches a registered option in the option array
	for (unsigned int i = 0; i < c->parameter_count; ++i) {
			CLI_Parameter p = c->parameters[i];

		if ((p.name_long) && (!strcmp((argument + 2), p.name_long))) {
			const char **option_arguments = NULL;

			// If the option takes arguments:
			if (p.argument_count) {
				// Grab a reference to the next argument as the start of option arguments.
				option_arguments = &c->arguments[c->argument_index + 1];

				// Find the last argument which belongs to this cluster
				unsigned int new_argument_index = c->argument_index + p.argument_count;

				// Check that consumption doesn't exceed available arguments
				if (new_argument_index >= c->argument_count) return DESCENT_ERROR_OVERFLOW;

				// Consume the option arguments from the arguments array
				c->argument_index = new_argument_index;
			}

			// Call the option's action
			return p.action(p.argument_count, option_arguments, c->settings);
		}
	}

	// No match
	flagged_argument = argument;
	return CLI_ERROR_NO_HANDLER;
}

static inline int handle_short_option(CLI_ParseContext *c) {
	const char *argument = c->arguments[c->argument_index];

	// Iterate through all the short options in a series
	for (int short_index = 1; argument[short_index]; ++short_index) {		
		char argument_short = argument[short_index];
		
		// Check if the option matches a registered option in the option array
		int index = cli_short_to_index(argument_short);
		
		if (index < 0 || !c->shorts[index]) {
			flagged_short = argument_short;
			flagged_argument = argument;
			return CLI_ERROR_NO_HANDLER;
		}

		CLI_Parameter p = *c->shorts[index];

		const char **option_arguments = NULL;

		// If the option takes arguments:
		if (p.argument_count) {
			// Grab a reference to the next argument as the start of option arguments.
			option_arguments = &c->arguments[c->argument_index + 1];

			// Find the last argument which belongs to this cluster
			unsigned int new_argument_index = c->argument_index + p.argument_count;

			// Check that consumption doesn't exceed available arguments
			if (new_argument_index >= c->argument_count) return DESCENT_ERROR_OVERFLOW;

			// Consume the option arguments from the arguments array
			c->argument_index = new_argument_index;
		}

		// Call the option's action
		int result = p.action(p.argument_count, option_arguments, c->settings);
		if (result) return result;
	}

	return 0;
}

static inline int handle_option(CLI_ParseContext *c) {
	const char *argument = c->arguments[c->argument_index];

	if (argument[1] == '-' && argument[2] != '\0') {
		return handle_long_option(c);
	}

	return handle_short_option(c);
}

static inline int handle_argument(CLI_ParseContext *c) {
	const char *argument = c->arguments[c->argument_index];

	if (c->parse_named) {
		for (unsigned int i = 0; i < c->parameter_count; ++i) {
			CLI_Parameter p = c->parameters[i];

			if (cli_is_subcommand(p) && !strcmp(p.name_long, argument)) {
				unsigned int sub_argc = c->argument_count - c->argument_index;
				const char **sub_argv = &c->arguments[c->argument_index];

				// Subcommand consumes all remaining arguments
				c->argument_index = c->argument_count;

				return cli_parse(sub_argc, sub_argv, p.parameter_count, p.parameters, c->settings);
			}
		}
	}
	
	for (unsigned int i = 0; i < c->parameter_count; ++i) {
		CLI_Parameter p = c->parameters[i];

		if (cli_is_positional(p) && p.position == c->positional_index) {
      ++c->positional_index;
			return c->parameters[i].action(1, &argument, c->settings);
		}
	}

	if (c->catchall) return c->catchall->action(1, &argument, c->settings);

	flagged_argument = argument;
	return CLI_ERROR_NO_HANDLER;
}

static inline int cli_parse_parameters(CLI_ParseContext *c) {
	for (c->argument_index = 1; c->argument_index < c->argument_count; ++c->argument_index) {
		const char *argument = c->arguments[c->argument_index];
		if (!argument) return DESCENT_ERROR_NULL_POINTER;

		if (c->parse_named) {
			// Stop handling options after "--"
			if ((argument[0] == '-') && (argument[1] == '-') && (argument[2] == '\0')) {
				c->parse_named = 0;
				continue;
			} else if (argument[0] == '-' && argument[1] != '\0') {
				int result = handle_option(c);
				if (result) return result;
				continue;
			}
		}

		int result = handle_argument(c);
		if (result) return result;
	}

	return 0;
}

CLI_Parameter cli_create_subcommand(const char *name, unsigned int parc, CLI_Parameter *parv) {
	assert(name != NULL);
	assert(parc != 0);
	assert(parv != NULL);

	return (CLI_Parameter) {
		.action = NULL,
		.parameters = parv,
		.name_long = name,
		.parameter_count = parc,
		.name_short = '\0'
	};
}

CLI_Parameter cli_create_option(const char *name_long, char name_short, unsigned int argument_count, CLI_Action a) {
	assert(name_short != '\0' || cli_is_valid_short_name(name_short) || name_long != NULL);
	assert(a != NULL);

	return (CLI_Parameter) {
		.action = a,
		.parameters = NULL,
		.name_long = name_long,
		.argument_count = argument_count,
		.name_short = name_short
	};
}

CLI_Parameter cli_create_positional(unsigned int position, CLI_Action a) {
	assert(position != 0);
	assert(position <= DESCENT_CLI_MAX_POSITIONALS);
	assert(a != NULL);
	
	return (CLI_Parameter) {
		.action = a,
		.name_long = NULL,
		.position = position,
		.name_short = '\0'
	};
}

CLI_Parameter cli_create_catchall(CLI_Action a) {
	assert(a != NULL);

	return (CLI_Parameter) {
		.action = a,
		.name_long = NULL,
		.position = 0,
		.name_short = '\0'
	};
}

int cli_parse(const int argc, const char **argv, const int parc, const CLI_Parameter *parv, void *settings) {
	if (!argv || !parv) return DESCENT_ERROR_NULL_POINTER;
	if (argc < 0 || parc < 0) return DESCENT_ERROR_INVALID_PARAMETER;
	if (argc > DESCENT_CLI_MAX_ARGUMENTS) return DESCENT_ERROR_OVERFLOW;

	// Clear error detection
	flagged_argument = NULL;

	CLI_ParseContext c = {
		.shorts = {0},
		.positionals = {0},
		.catchall = NULL,
		.parameters = parv,
		.settings = settings,
		.arguments = argv,
		.argument_count = argc,
		.argument_index = 1,
		.parameter_count = parc,
		.positional_index = 1,
		.parse_named = 1
	};

	int result = cli_prescan(&c);
	if (result) return result;

	return cli_parse_parameters(&c);
}

const char *cli_flagged_argument(void) {
	return flagged_argument;
}

char cli_flagged_short(void) {
	return flagged_short;
}
