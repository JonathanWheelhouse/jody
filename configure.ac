# Process this file with autoconf to produce a configure script.
AC_INIT(jody, 0.1.0, jonathan dot wheelhouse at gmail.com)
AC_CONFIG_SRCDIR(src/main.c)
AM_INIT_AUTOMAKE

AC_PROG_CC

SDL_VERSION=1.2.0
AM_PATH_SDL(${SDL_VERSION}, ,AC_MSG_ERROR([*** SDL version $(SDL_VERSION) not found!]))

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([atexit memset])

AC_CONFIG_FILES(Makefile src/Makefile src/img/Makefile)
AC_OUTPUT
