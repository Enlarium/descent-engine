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