#include "actions.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <descent/cli.h>
#include <descent/utilities/codes.h>

static int check_inputs_flag(unsigned int argc, const char **argv, void *settings, const char *function_name) {
	if (argc) {
		printf("%s received an nonzero number in parameter `argc` (expected 0, got %u)\n", function_name, argc);
		return CLI_ERROR_ARGUMENT_COUNT;
	}
	if (argv) {
		printf("%s received a non-null pointer for parameter `argv`\n", function_name);
		return CLI_ERROR_ARGUMENT_COUNT;
	}
	if (!settings) {
		printf("%s received a null pointer for parameter `settings`\n", function_name);
		return DESCENT_ERROR_NULL_POINTER;
	}

	return 0;
}

static int check_inputs_nonflag(unsigned int argc, const char **argv, void *settings, unsigned int expected_argc, const char *function_name) {
	if (argc != expected_argc) {
		printf("%s received an incorrect number in parameter `argc` (expected %u, got %u)\n", function_name, expected_argc, argc);
		return CLI_ERROR_ARGUMENT_COUNT;
	}
	if (!argv) {
		printf("%s received a null pointer for parameter `argv`\n", function_name);
		return DESCENT_ERROR_NULL_POINTER;
	}
	if (!settings) {
		printf("%s received a null pointer for parameter `settings`\n", function_name);
		return DESCENT_ERROR_NULL_POINTER;
	}

	return 0;
}

static int check_field(const char *expected, const char **argv, const char *function_name) {
	if (!expected) {
		printf("%s received a null pointer for parameter `expected`\n", function_name);
		return DESCENT_ERROR_NULL_POINTER;
	}
	if (!argv) {
		printf("%s received a null pointer for parameter `argv`\n", function_name);
		return DESCENT_ERROR_NULL_POINTER;
	}
	if (!argv[0]) {
		printf("%s received a null pointer for parameter element `argv[0]`\n", function_name);
		return DESCENT_ERROR_NULL_POINTER;
	}

	if (strcmp(expected, argv[0])) {
		printf("%s failed test for %s == \33[0;31m%s\33[0m\n", function_name, expected, argv[0]);
		return CLI_ERROR_INCORRECT_ARGUMENT;
	}

	return 0;
}

static int check_fields(unsigned int count, const char **expected, const char **argv, const char *function_name) {
	if (!expected) {
		printf("%s received a null pointer for parameter `expected`\n", function_name);
		return DESCENT_ERROR_NULL_POINTER;
	}
	if (!argv) {
		printf("%s received a null pointer for parameter `argv`\n", function_name);
		return DESCENT_ERROR_NULL_POINTER;
	}

	for (unsigned int i = 0; i < count; ++i) {
		if (!expected[i]) {
			printf("%s received a null pointer for parameter element `expected[%u]`\n", function_name, i);
			return DESCENT_ERROR_NULL_POINTER;
		}
		if (!argv[i]) {
			printf("%s received a null pointer for parameter element `argv[%u]`\n", function_name, i);
			return DESCENT_ERROR_NULL_POINTER;
		}

		if (strcmp(expected[i], argv[i])) {
			printf("%s failed test for %s == \33[0;31m%s\33[0m\n", function_name, expected[i], argv[i]);
			return CLI_ERROR_INCORRECT_ARGUMENT;
		}
	}

	return 0;
}

int option_subcommand(unsigned int argc, const char **argv, void *settings) {
	int result = check_inputs_nonflag(argc, argv, settings, 1, __func__);
	if (result) return result;

	Settings *s = settings;
	return check_field(s->subcommand, argv, __func__);
}

int option_fields_1(unsigned int argc, const char **argv, void *settings) {
	int result = check_inputs_nonflag(argc, argv, settings, 1, __func__);
	if (result) return result;

	Settings *s = settings;
	return check_field(s->fields_1, argv, __func__);
}

int option_fields_2(unsigned int argc, const char **argv, void *settings) {
	int result = check_inputs_nonflag(argc, argv, settings, 2, __func__);
	if (result) return result;

	Settings *s = settings;
	return check_fields(2, s->fields_2, argv, __func__);
}

int option_fields_3(unsigned int argc, const char **argv, void *settings) {
	int result = check_inputs_nonflag(argc, argv, settings, 3, __func__);
	if (result) return result;

	Settings *s = settings;
	return check_fields(3, s->fields_3, argv, __func__);
}

int option_fields_4(unsigned int argc, const char **argv, void *settings) {
	int result = check_inputs_nonflag(argc, argv, settings, 4, __func__);
	if (result) return result;

	Settings *s = settings;
	return check_fields(4, s->fields_4, argv, __func__);
}


int option_positional_1(unsigned int argc, const char **argv, void *settings) {
	int result = check_inputs_nonflag(argc, argv, settings, 1, __func__);
	if (result) return result;

	Settings *s = settings;
	return check_field(s->positional_1, argv, __func__);
}

int option_positional_2(unsigned int argc, const char **argv, void *settings) {
	int result = check_inputs_nonflag(argc, argv, settings, 1, __func__);
	if (result) return result;

	Settings *s = settings;
	return check_field(s->positional_2, argv, __func__);
}

int option_positional_3(unsigned int argc, const char **argv, void *settings) {
	int result = check_inputs_nonflag(argc, argv, settings, 1, __func__);
	if (result) return result;

	Settings *s = settings;
	return check_field(s->positional_3, argv, __func__);
}

int option_positional_4(unsigned int argc, const char **argv, void *settings) {
	int result = check_inputs_nonflag(argc, argv, settings, 1, __func__);
	if (result) return result;

	Settings *s = settings;
	return check_field(s->positional_4, argv, __func__);
}


int option_flag_1(unsigned int argc, const char **argv, void *settings) {
	int result = check_inputs_flag(argc, argv, settings, __func__);
	if (result) return result;

	Settings *s = settings;
	s->flag_1 = 1;

	return 0;
}

int option_flag_2(unsigned int argc, const char **argv, void *settings) {
	int result = check_inputs_flag(argc, argv, settings, __func__);
	if (result) return result;

	Settings *s = settings;
	s->flag_2 = 1;

	return 0;
}

int option_flag_3(unsigned int argc, const char **argv, void *settings) {
	int result = check_inputs_flag(argc, argv, settings, __func__);
	if (result) return result;

	Settings *s = settings;
	s->flag_3 = 1;

	return 0;
}

int option_flag_4(unsigned int argc, const char **argv, void *settings) {
	int result = check_inputs_flag(argc, argv, settings, __func__);
	if (result) return result;

	Settings *s = settings;
	s->flag_4 = 1;

	return 0;
}

int option_catchall(unsigned int argc, const char **argv, void *settings) {
	int result = check_inputs_nonflag(argc, argv, settings, 1, __func__);
	if (result) return result;

	Settings *s = settings;
	return check_field(s->catchall[s->catchall_count++], argv, __func__);
}
