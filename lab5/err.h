#ifndef ERR_H
#define ERR_H

#include <stdio.h>
#include <stdlib.h>

// print error message
void err_msg(const char *err);

// announce error and exit
void err_exit(const char *err);

// announce usage and exit
void usage_err(const char *err);

// handle fatal error
void fatal_err(const char *err);

#endif