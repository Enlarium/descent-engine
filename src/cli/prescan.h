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

#ifndef DESCENT_SOURCE_CLI_PRESCAN_H
#define DESCENT_SOURCE_CLI_PRESCAN_H

#include <descent/cli.h>
#include <descent/utilities/codes.h>

#include "context.h"

static inline int cli_comparator(const void *a, const void *b) {
	const CLI_Parameter *par_a = (const CLI_Parameter *)a;
	const CLI_Parameter *par_b = (const CLI_Parameter *)b;

	const char *long_a = par_a->name_long;
	const char *long_b = par_b->name_long;

	// Subcommands have non-null p->parameters
	int is_subcommand_a = cli_is_subcommand(par_a);
	int is_subcommand_b = cli_is_subcommand(par_b);

	// If neither has a long name, sorting doesn't matter
	if (!long_a && !long_b) return 0;

	// Sort parameters without long names before those with long names
	if (!long_a) return -1;
	if (!long_b) return 1;

	// Sort subcommands before long options
	if (is_subcommand_a != is_subcommand_b) return is_subcommand_b - is_subcommand_a;

	// Sort parameters with long names lexicographically
	return strcmp(long_a, long_b);
}

static inline int cli_prescan(CLI_ParseContext *c) {
	puts("Unsorted Parameters:");
	for (unsigned int i = 0; i < c->parameter_count; ++i) {
		if (cli_is_subcommand(&c->parameters[i]))        printf("  (%02u) subcommand %s\n", i, c->parameters[i].name_long);
		else if (cli_is_long_option(&c->parameters[i]))  printf("  (%02u) option     %s\n", i, c->parameters[i].name_long);
		else if (cli_is_short_option(&c->parameters[i])) printf("  (%02u) option     %c\n", i, c->parameters[i].name_short);
		else if (cli_is_positional(&c->parameters[i]))   printf("  (%02u) positional %u\n", i, c->parameters[i].position);
		else if (cli_is_catchall(&c->parameters[i]))     printf("  (%02u) catchall\n", i);
	}

	// Sort the parameter array lexicographically
	qsort(c->parameters, c->parameter_count, sizeof(c->parameters[0]), cli_comparator);

	puts("Sorted Parameters:");
	for (unsigned int i = 0; i < c->parameter_count; ++i) {
		if (cli_is_subcommand(&c->parameters[i]))        printf("  (%02u) subcommand %s\n", i, c->parameters[i].name_long);
		else if (cli_is_long_option(&c->parameters[i]))  printf("  (%02u) option     %s\n", i, c->parameters[i].name_long);
		else if (cli_is_short_option(&c->parameters[i])) printf("  (%02u) option     %c\n", i, c->parameters[i].name_short);
		else if (cli_is_positional(&c->parameters[i]))   printf("  (%02u) positional %u\n", i, c->parameters[i].position);
		else if (cli_is_catchall(&c->parameters[i]))     printf("  (%02u) catchall\n", i);
	}

	// Default to no subcommands or long options
	c->first_subcommand = c->parameter_count;
	c->first_long_option = c->parameter_count;

	// Initialize last parameter to NULL
	CLI_Parameter *par_last = NULL;

	// Scan through all parameters to set up bookkeeping
	for (unsigned int i = 0; i < c->parameter_count; ++i) {
		CLI_Parameter *par = &c->parameters[i];

		if (par->name_long) {
			// Record the first subcommand
			if (c->first_subcommand == c->parameter_count && cli_is_subcommand(par)) {
				printf("Found first subcommand at index %u\n", i);
				c->first_subcommand = i;
			}

			// Record the first long option
			else if (c->first_long_option == c->parameter_count && cli_is_long_option(par)) {
				printf("Found first long option at index %u\n", i);
				c->first_long_option = i;
			}
		}

		// Catch duplicates
		if (cli_is_subcommand(par)) {
			if (par_last && cli_is_subcommand(par_last) && !strcmp(par_last->name_long, par->name_long)) {
				return CLI_ERROR_DUPLICATE_PARAMETER;
			}
		}

		else if (cli_is_option(par)) {
			if (cli_is_long_option(par)) {
				if (par_last && cli_is_long_option(par_last) && !strcmp(par_last->name_long, par->name_long)) {
					return CLI_ERROR_DUPLICATE_PARAMETER;
				}
			}

			if (cli_is_short_option(par)) {
				int index = cli_short_to_index(par->name_short);
				if (!c->shorts[index]) c->shorts[index] = par;
				else return CLI_ERROR_DUPLICATE_PARAMETER;
			}
		}
		
		else if (cli_is_positional(par)) {
				int index = par->position - 1;
				if (!c->positionals[index]) c->positionals[index] = par;
				else return CLI_ERROR_DUPLICATE_PARAMETER;
		}
		
		else if (cli_is_catchall(par)) {
			if (!c->catchall) c->catchall = par;
			else return CLI_ERROR_DUPLICATE_PARAMETER;
		}
		
		else return CLI_ERROR_INVALID_PARAMETER;

		// Update last parameter
		par_last = par;
	}

	return 0;
}

#endif
