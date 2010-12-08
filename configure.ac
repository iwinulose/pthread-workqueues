#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([pthread-workqueues], [0.0])
AC_CONFIG_SRCDIR([dequeue.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
LT_INIT

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

CFLAGS="$CFLAGS -pthread -Wall -Wextra -pedantic -std=c99 -D_REENTRANT -g -O0"
LDFLAGS="$LDFLAGS -pthread"

# Checks for libraries.
AC_CHECK_LIB([dl], [dlopen])

# Checks for header files.
AC_CHECK_HEADERS([limits.h stddef.h stdint.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
