SUBDIRS = img
DIST_SUBDIRS = img

bin_PROGRAMS = jody

jody_SOURCES = \
	main.c \
	gamedefs.h \
	csprite.h \
	csprite.c \
	resources.h \
	resources.c \
	cursor.h \
	cursor.c \
	util.h \
	util.c

nodist_jody_SOURCES = datadir.h
BUILT_SOURCES = datadir.h

LDADD = $(SDL_LIBS) -lSDL_image

AM_CPPFLAGS = $(shell sdl-config --cflags)

AM_CFLAGS = -pedantic -Wall -std=gnu99

DISTCLEANFILES = datadir.h

datadir.h: Makefile
	echo '#define IMG_DIR "$(pkgdatadir)/img/"' >$@

