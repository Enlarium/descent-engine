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

#ifndef DESCENT_TESTS_ACTIONS_H
#define DESCENT_TESTS_ACTIONS_H

typedef struct {
	const char *subcommand;
	const char *fields_1;
	const char *fields_2[2];
	const char *fields_3[3];
	const char *fields_4[4];
	const char *positional_1;
	const char *positional_2;
	const char *positional_3;
	const char *positional_4;
	const char *catchall[16];
	unsigned int catchall_count;
	int flag_1;
	int flag_2;
	int flag_3;
	int flag_4;
} Settings;

int option_subcommand(unsigned int argc, const char **argv, void *settings);

int option_fields_1(unsigned int argc, const char **argv, void *settings);
int option_fields_2(unsigned int argc, const char **argv, void *settings);
int option_fields_3(unsigned int argc, const char **argv, void *settings);
int option_fields_4(unsigned int argc, const char **argv, void *settings);

int option_positional_1(unsigned int argc, const char **argv, void *settings);
int option_positional_2(unsigned int argc, const char **argv, void *settings);
int option_positional_3(unsigned int argc, const char **argv, void *settings);
int option_positional_4(unsigned int argc, const char **argv, void *settings);

int option_flag_1(unsigned int argc, const char **argv, void *settings);
int option_flag_2(unsigned int argc, const char **argv, void *settings);
int option_flag_3(unsigned int argc, const char **argv, void *settings);
int option_flag_4(unsigned int argc, const char **argv, void *settings);

int option_catchall(unsigned int argc, const char **argv, void *settings);

#endif