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

#include <stdio.h>
#include <string.h>

#include <descent/rcode.h>
#include <descent/cli.h>

#include "actions.h"

int check_parse(int argc, const char **argv, int parc, CLI_Parameter *parv, void *settings, int expected_result) {
	int result = cli_parse(argc, argv, parc, parv, settings);
	if(result != expected_result) {
		printf("Return code: %s (%d)\n", code_string(result), result);

		const char *argument = cli_flagged_argument();
		char short_name = cli_flagged_short();

		if (argument) printf("Flagged argument: %s\n", argument);
		if(short_name) printf("Flagged short: %c\n", short_name);
		
		return -1;
	}

	return 0;
}

int main(void) {
	Settings settings = {
		.subcommand = "subcommand",
		.fields_1 = "f1",
		.fields_2 = {
			"f2_1",
			"f2_2"
		},
		.fields_3 = {
			"f3_1",
			"f3_2",
			"f3_3"
		},
		.fields_4 = {
			"f4_1",
			"f4_2",
			"f4_3",
			"f4_4"
		},
		.positional_1 = "p1",
		.positional_2 = "p2",
		.positional_3 = "p3",
		.positional_4 = "p4",
		.catchall = {
			"c1",
			"c2",
			"c3",
			"c4",
			"-1",
			"-2",
			"--fields_2",
			NULL,
			NULL, NULL, NULL, NULL,
			NULL, NULL, NULL, NULL
		},
		.catchall_count = 0,
		.flag_1 = 0,
		.flag_2 = 0,
		.flag_3 = 0,
		.flag_4 = 0
	};

	CLI_Parameter sub_parameters[] = {
		cli_create_option("subcommand", 's', 1, option_subcommand),
		cli_create_catchall(option_catchall)
	};
	int sub_parameter_count = sizeof(sub_parameters)/sizeof(sub_parameters[0]);

	CLI_Parameter parameters[] = {
		cli_create_subcommand("subcommand", sub_parameter_count, sub_parameters),
		cli_create_option("flag_1", 'a', 0, option_flag_1),
		cli_create_option("flag_2", 'b', 0, option_flag_2),
		cli_create_option("flag_3", 'c', 0, option_flag_3),
		cli_create_option("flag_4", 'd', 0, option_flag_4),
		cli_create_option("fields_1", '1', 1, option_fields_1),
		cli_create_option("fields_2", '2', 2, option_fields_2),
		cli_create_option("fields_3", '3', 3, option_fields_3),
		cli_create_option("fields_4", '4', 4, option_fields_4),
		cli_create_positional(1, option_positional_1),
		cli_create_positional(2, option_positional_2),
		cli_create_positional(3, option_positional_3),
		cli_create_positional(4, option_positional_4),
		cli_create_catchall(option_catchall)
	};
	int parameter_count = sizeof(parameters)/sizeof(parameters[0]);

	const char *argv[] = {
		"descent-exec",
		"-a",
		"-1",
		settings.fields_1,
		"-bcd23",
		settings.fields_2[0],
		settings.fields_2[1],
		settings.fields_3[0],
		settings.fields_3[1],
		settings.fields_3[2],
		settings.positional_1,
		settings.positional_2,
		"--fields_4",
		settings.fields_4[0],
		settings.fields_4[1],
		settings.fields_4[2],
		settings.fields_4[3],
		settings.positional_3,
		settings.positional_4,
		settings.catchall[0],
		settings.catchall[1],
		settings.catchall[2],
		settings.catchall[3],
		"subcommand",
		"--subcommand",
		settings.subcommand,
		"--",
		settings.catchall[4],
		settings.catchall[5],
		settings.catchall[6]
	};
	int argc = sizeof(argv)/sizeof(argv[0]);

	for (int i = 0; i < argc; ++i) printf("%s ", argv[i]);
	puts("");

	if(check_parse(argc, argv, parameter_count, parameters, &settings, 0)) return -1;
	if (!settings.flag_1 || !settings.flag_2 || !settings.flag_3 || !settings.flag_4) {
    printf("One or more flags were not set!\n");
    return -1;
	}	

	return 0;
}