## Makefile.am -- Process this file with automake to produce Makefile.in

# Include libtool macros
ACLOCAL_AMFLAGS = -I m4

# Additional C compiler flags
AM_CFLAGS = --target=aarch64-linux-gnu -march=armv8.3-a -Wall -Wextra

# Additional linker flags
AM_LDFLAGS =

# Shared libraries
lib_LTLIBRARIES  =

# Test executables
check_PROGRAMS =

# Include module-specific Makefiles
include src/Makemodule.am
include tests/Makemodule.am

# Tests to run
TESTS = $(check_PROGRAMS)

# Preload PACStack wrappers for each test
AM_TESTS_ENVIRONMENT = LD_PRELOAD=$(top_builddir)/.libs/libpacstack.so;
