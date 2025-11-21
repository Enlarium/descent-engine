#ifndef DESCENT_EXAMPLES_CLI_H
#define DESCENT_EXAMPLES_CLI_H

int option_help(unsigned int argc, const char **argv, void *settings);

int option_version(unsigned int argc, const char *argv[], void *settings);

int option_catchall(unsigned int argc, const char *argv[], void *settings);

#endif