#include "cli.h"

#include <stdlib.h>
#include <stdio.h>

#include <descent/utilities/codes.h>

int option_help(unsigned int argc, const char **argv, void *settings) {
	(void) argc;
	(void) argv;
	(void) settings;

	puts("Descent Engine Example v0.0.0");
	puts("Help unavailable at this time");

	exit(0);

	return 0;
}

int option_version(unsigned int argc, const char **argv, void *settings) {
	(void) argc;
	(void) argv;
	(void) settings;

	puts("Descent Engine Example 0.0.0");

	exit(0);

	return 0;
}

int option_catchall(unsigned int argc, const char *argv[], void *settings) {
	(void) argc;
	(void) settings;

	printf("Unrecognized argument: %s\n", argv[0]);

	return 0;
}
