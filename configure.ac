#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_INIT([pacstack-wrappers], [0.1],
        [https://github.com/pointer-authentication/pacstack-wrappers/issues],
        [pacstack-wrappers],
        [https://github.com/pointer-authentication/pacstack-wrappers])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([src/pacstack_longjmp.c])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC

# Use libtool.
AC_ENABLE_SHARED(yes)
AC_ENABLE_STATIC(no)
LT_INIT

# Created configuration files.
AC_CONFIG_FILES([Makefile])

AC_OUTPUT
