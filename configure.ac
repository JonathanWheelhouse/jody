dnl Process this file with autoconf to produce a configure script.
AC_INIT(jody, 1.0.0, wheelhouse@pacific.net.au)
AC_CONFIG_SRCDIR(src/main.c)
AM_INIT_AUTOMAKE
dnl AM_CONFIG_HEADER(config.h)

AC_PROG_CC
AC_PROG_MAKE_SET

SDL_VERSION=1.2.0
AM_PATH_SDL(${SDL_VERSION}, ,AC_MSG_ERROR([*** SDL version $(SDL_VERSION) not found!]))

IMG_DIR='${datadir}/jody/img'
AC_SUBST(IMG_DIR)

AC_CONFIG_FILES(Makefile src/Makefile src/img/Makefile)
AC_OUTPUT
