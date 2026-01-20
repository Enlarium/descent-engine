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

#ifndef DESCENT_CLI_H
#define DESCENT_CLI_H

#ifndef DESCENT_CLI_MAX_ARGUMENTS
#define DESCENT_CLI_MAX_ARGUMENTS 256
#endif

#ifndef DESCENT_CLI_MAX_POSITIONALS
#define DESCENT_CLI_MAX_POSITIONALS 16
#endif

#include <stdbool.h>

#include <descent/rcode.h>

/**
 * @brief Function type for CLI actions.
 *
 * A CLI_Action is called when a corresponding command-line argument
 * is matched. It receives the count and values of arguments, as well
 * as a pointer to user-defined settings.
 *
 * @param argument_count Number of arguments passed to this action.
 * @param arguments Array of argument strings.
 * @param settings User-defined settings pointer passed to the parser.
 * @return 0 on success, non-zero error code on failure.
 */
typedef rcode (*CLI_Action)(unsigned int argument_count, const char **arguments, void *settings);


/**
 * @brief Represents a CLI parameter.
 *
 * Can be a subcommand, option, positional argument, or catch-all.
 */
typedef struct CLI_Parameter {
	CLI_Action action;                /**< Action function for the parameter. NULL for subcommands. */
	struct CLI_Parameter *parameters; /**< Array of subcommand parameters. NULL for options and positionals. */
	const char *name_long;            /**< Long name of the option or subcommand. NULL if unused. */
	union {
		unsigned int parameter_count;   /**< Number of subcommand parameters. */
		unsigned int argument_count;    /**< Number of option arguments expected. */
		unsigned int position;          /**< Position index for positional arguments. 0 for catch-all. */
	};
	char name_short;                  /**< Short option name. '\0' if unused. */	
} CLI_Parameter;

/**
 * @brief Check if a character is a valid short name.
 * 
 * Valid short names are numbers, letters, and the symbols
 * `!`, `#`, `$`, `%`, `&`, `?`, and `@`.
 * 
 * @param c The character to check.
 * @return Non-zero if it is a valid, 0 otherwise.
 */
bool cli_is_valid_short_name(char c);

/**
 * @brief Check if a CLI_Parameter is a subcommand.
 *
 * @param p Parameter to check.
 * @return Non-zero if it is a subcommand, 0 otherwise.
 */
bool cli_is_subcommand(const CLI_Parameter *p);

/**
 * @brief Check if a CLI_Parameter is a long option.
 *
 * @param p Parameter to check.
 * @return Non-zero if it is a long option, 0 otherwise.
 */
bool cli_is_long_option(const CLI_Parameter *p);

/**
 * @brief Check if a CLI_Parameter is a short option.
 *
 * @param p Parameter to check.
 * @return Non-zero if it is a short option, 0 otherwise.
 */
bool cli_is_short_option(const CLI_Parameter *p);

/**
 * @brief Check if a CLI_Parameter is any type of option.
 *
 * @param p Parameter to check.
 * @return Non-zero if it is a long or short option, 0 otherwise.
 */
bool cli_is_option(const CLI_Parameter *p);

/**
 * @brief Check if a CLI_Parameter is a positional argument.
 *
 * @param p Parameter to check.
 * @return Non-zero if it is a positional argument, 0 otherwise.
 */
bool cli_is_positional(const CLI_Parameter *p);

/**
 * @brief Check if a CLI_Parameter is a catch-all argument.
 *
 * @param p Parameter to check.
 * @return Non-zero if it is a catch-all argument, 0 otherwise.
 */
bool cli_is_catchall(const CLI_Parameter *p);

/**
 * @brief Create a subcommand parameter.
 *
 * @param name Name of the subcommand. Must be non-NULL.
 * @param parc Number of subcommand parameters. Must be non-zero.
 * @param parv Array of subcommand parameters. Must be non-NULL.
 * @return Initialized CLI_Parameter representing a subcommand.
 */
CLI_Parameter cli_create_subcommand(const char *name, unsigned int parc, CLI_Parameter *parv);

/**
 * @brief Create an option parameter.
 *
 * @param name_long Long option string. Can be NULL if unused.
 * @param name_short Short option character. Can be '\0' if unused.
 * @param argc Number of arguments the option expects.
 * @param a Function to call when option is matched. Must be non-NULL.
 * @return Initialized CLI_Parameter representing an option.
 * @note Either `name_long` or `name_short` must be set.
 */
CLI_Parameter cli_create_option(const char *name_long, char name_short, unsigned int argc, CLI_Action a);

/**
 * @brief Create a positional argument parameter.
 *
 * @param position Position index of the argument (starting at 1). Must be non-zero.
 * @param a Function to call when argument is matched. Must be non-NULL.
 * @return Initialized CLI_Parameter representing a positional argument.
 */
CLI_Parameter cli_create_positional(unsigned int position, CLI_Action a);

/**
 * @brief Create a catch-all parameter.
 *
 * Matches any argument not consumed by other parameters.
 *
 * @param a Function to call for unmatched arguments. Must be non-NULL.
 * @return Initialized CLI_Parameter representing a catch-all argument.
 */
CLI_Parameter cli_create_catchall(CLI_Action a);

/**
 * @brief Parse command-line arguments.
 *
 * Processes options, subcommands, positionals, and catch-all parameters.
 *
 * @param argc Number of arguments in the argv array.
 * @param argv Array of argument strings.
 * @param parc Number of parameters in the parameters array.
 * @param parv Array of parameter definitions.
 * @param settings User-defined pointer passed to option actions.
 * @return 0 on success, non-zero error code on failure.
 */
rcode cli_parse(unsigned int argc, const char **argv, unsigned int parc, CLI_Parameter *parv, void *settings);

/**
 * @brief Get the last flagged argument that caused an error.
 * @return Pointer to the last argument string that failed, or NULL if none.
 * @note Thread safe (per-thread). Reset every time cli_parse() is called.
 */
const char *cli_flagged_argument(void);

/**
 * @brief Get the last flagged short option character that caused an error.
 * @return Character of the last short option that failed, or '\0' if none.
 * @note Thread safe (per-thread). Reset every time cli_parse() is called.
 */
char cli_flagged_short(void);

#endif
