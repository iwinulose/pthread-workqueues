//Charles Duyk
//charles.duyk@gmail.com

#ifndef STDINCLUDES_H
#define NO_PRAGMAS
#ifndef NO_PRAGMAS
#pragma mark Standard includes
#endif
#define STDINCLUDES_H


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef NO_PRAGMAS
#pragma mark -
#pragma mark Standard Defines
#endif

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#endif /* STDINCLUDES_H */
