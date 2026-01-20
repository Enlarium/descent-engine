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
#include <stdbool.h>
#include <stddef.h>

#include <descent/thread/tls.h>
#include <descent/rcode.h>

#include "context.h"
#include "prescan.h"
#include "find.h"

// Track flagged argument for error reporting
static TLS const char *flagged_argument;
static TLS char        flagged_short;

static inline void cli_clear_flagged(void) {
	flagged_argument = NULL;
	flagged_short = '\0';
}

const char *cli_flagged_argument(void) {
	return flagged_argument;
}

char cli_flagged_short(void) {
	return flagged_short;
}

static inline rcode call_action(CLI_Action a, unsigned int argc, const char **argv, void *settings) {
	rcode result = a(argc, argv, settings);

	if (!result) cli_clear_flagged();
	return result;
}

static inline rcode call_option(CLI_ParseContext *c, CLI_Parameter *par) {
	const char **option_arguments = NULL;

	// If the option takes arguments:
	if (par->argument_count) {
		// Grab a reference to the next argument as the start of option arguments
		option_arguments = &c->arguments[c->argument_index + 1];

		// Find the last argument which belongs to this cluster
		unsigned int new_argument_index = c->argument_index + par->argument_count;

		// Check that consumption doesn't exceed available arguments
		if (new_argument_index >= c->argument_count) return DESCENT_ERROR_OVERFLOW;

		// Consume the option arguments from the arguments array
		c->argument_index = new_argument_index;
	}

	// Call the option's action
	return call_action(par->action, par->argument_count, option_arguments, c->settings);
}

static inline rcode call_subcommand(CLI_ParseContext *c, CLI_Parameter *par) {
	unsigned int sub_argc = c->argument_count - c->argument_index;
	const char **sub_argv = &c->arguments[c->argument_index];

	// Subcommand consumes all remaining arguments
	c->argument_index = c->argument_count;

	// Call the subcommand
	rcode result = cli_parse(sub_argc, sub_argv, par->parameter_count, par->parameters, c->settings);

	if (!result) cli_clear_flagged();
	return result;
}

static inline rcode call_positional(CLI_ParseContext *c, CLI_Parameter *par) {
	const char *argument = c->arguments[c->argument_index];

	++c->positional_index;

	// Call the positional's action
	return call_action(par->action, 1, &argument, c->settings);
}

static inline rcode call_catchall(CLI_ParseContext *c, CLI_Parameter *par) {
	const char *argument = c->arguments[c->argument_index];

	// Call the catchall's action
	return call_action(par->action, 1, &argument, c->settings);
}

static inline rcode handle_long_option(CLI_ParseContext *c) {
	const char *argument = c->arguments[c->argument_index];

	const char *argument_long = argument + 2;

	// Check if the option matches a registered long option
	CLI_Parameter *par = cli_find_long(c, argument_long);
	if (par) return call_option(c, par);

	// No match
	return CLI_ERROR_NO_HANDLER;
}

static inline rcode handle_short_option(CLI_ParseContext *c) {
	const char *argument = c->arguments[c->argument_index];

	// Iterate through all the short options in a series
	for (int short_index = 1; argument[short_index]; ++short_index) {		
		char argument_short = argument[short_index];

		// This will remain set until unset by a successful call.
		flagged_short = argument_short;

		// Check if the option matches a registered short option
		CLI_Parameter *par = cli_find_short(c, argument_short);
		if (par) {
			rcode result = call_option(c, par);
			if (result) return result;
		}

		// No match
		else return CLI_ERROR_NO_HANDLER;
	}

	return 0;
}

static inline rcode handle_option(CLI_ParseContext *c) {
	const char *argument = c->arguments[c->argument_index];

	if (argument[1] == '-' && argument[2] != '\0') {
		return handle_long_option(c);
	}

	return handle_short_option(c);
}

static inline rcode handle_argument(CLI_ParseContext *c) {
	const char *argument = c->arguments[c->argument_index];

	CLI_Parameter *par = NULL;

	// This will remain set until unset by a successful call.
	flagged_argument = argument;

	// Check if the option matches a registered subcommand
	if (c->parse_named) {
		par = cli_find_subcommand(c, argument);
		if (par) return call_subcommand(c, par);
	}

	// Check if the option matches a registered option in the positional array
	par = cli_find_positional(c);
	if (par) return call_positional(c, par);

	// Call the catchall if available
	par = cli_find_catchall(c);
	if (par) return call_catchall(c, par);

	return CLI_ERROR_NO_HANDLER;
}

static inline rcode cli_parse_parameters(CLI_ParseContext *c) {
	for (c->argument_index = 1; c->argument_index < c->argument_count; ++c->argument_index) {
		const char *argument = c->arguments[c->argument_index];
		if (!argument) return DESCENT_ERROR_NULL;

		if (c->parse_named) {
			// Stop handling options after "--"
			if ((argument[0] == '-') && (argument[1] == '-') && (argument[2] == '\0')) {
				c->parse_named = 0;
				continue;
			}
			
			else if (argument[0] == '-' && argument[1] != '\0') {
				rcode result = handle_option(c);
				if (result) return result;
				continue;
			}
		}

		rcode result = handle_argument(c);
		if (result) return result;
	}

	return 0;
}

rcode cli_parse(unsigned int argc, const char **argv, unsigned int parc, CLI_Parameter *parv, void *settings) {
	if (!argv || !parv) return DESCENT_ERROR_NULL;
	if (argc > DESCENT_CLI_MAX_ARGUMENTS) return DESCENT_ERROR_OVERFLOW;

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
		.parse_named = 1,
		.first_subcommand = parc,
		.first_long_option = parc
	};

	rcode result = cli_prescan(&c);
	if (result) return result;

	return cli_parse_parameters(&c);
}

bool cli_is_valid_short_name(char c) {
	if (c == '!') return 1;
	if (c >= '#' && c <= '&') return 1;
	if (c >= '0' && c <= '9') return 1;
	if (c >= '?' && c <= 'Z') return 1;
	if (c >= 'a' && c <= 'z') return 1;

	return 0;
}

bool cli_is_subcommand(const CLI_Parameter *p) {
	return (
		p &&
		p->action == NULL &&
		p->name_long != NULL &&
		p->name_short == '\0' &&
		p->parameter_count != 0 &&
		p->parameters != NULL
	);
}

bool cli_is_long_option(const CLI_Parameter *p) {
	return (
		p &&
		p->action != NULL &&
		p->name_long != NULL &&
		(p->name_short == '\0' || cli_is_valid_short_name(p->name_short)) &&
		p->parameters == NULL
	);
}

bool cli_is_short_option(const CLI_Parameter *p) {
	return (
		p &&
		p->action != NULL &&
		cli_is_valid_short_name(p->name_short) &&
		p->parameters == NULL
	);
}

bool cli_is_option(const CLI_Parameter *p) {
	return cli_is_long_option(p) || cli_is_short_option(p);
}

bool cli_is_positional(const CLI_Parameter *p) {
	return (
		p &&
		p->action != NULL &&
		p->name_long == NULL &&
		p->name_short == '\0' &&
		p->position != 0 &&
		p->position <= DESCENT_CLI_MAX_POSITIONALS &&
		p->parameters == NULL
	);
}

bool cli_is_catchall(const CLI_Parameter *p) {
	return (
		p &&
		p->action != NULL &&
		p->name_long == NULL &&
		p->name_short == '\0' &&
		p->position == 0 &&
		p->parameters == NULL
	);
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
	assert(cli_is_valid_short_name(name_short) || name_long != NULL);
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
