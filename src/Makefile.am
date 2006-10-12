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

LDADD = $(SDL_LIBS) -lSDL_image

AM_CPPFLAGS = $(shell sdl-config --cflags) -DIMG_DIR=\"${pkgdatadir}/img/\"

AM_CFLAGS = -pedantic -Wall -std=gnu99


