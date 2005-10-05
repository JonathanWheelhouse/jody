SUBDIRS = img
DIST_SUBDIRS = img

bin_PROGRAMS = jody

jody_SOURCES = \
	main.c \
	gamedefs.h \
	resources.c \
	resources.h \
	util.h \
	util.c

LDADD = $(SDL_LIBS) -lSDL_image

## Both these ways work to pass the bindir to main.c
## DEFS = -DSCRIPTDIR=\"$(bindir)\" @DEFS@
AM_CPPFLAGS = -DSCRIPTDIR=\"$(bindir)\" $(shell sdl-config --cflags)

AM_CFLAGS = -pedantic -Wall
noinst_HEADERS = img_dir.h
EXTRA_DIST = img_dir.h.in
## We borrow guile's convention and use @-...-@ as the substitution
## brackets here, instead of the usual @...@.  This prevents autoconf
## from substituting the values directly into the left-hand sides of
## the sed substitutions.
img_dir.h: img_dir.h.in ${top_builddir}/config.status
	rm -f $@.tmp
	sed < $< > $@.tmp \
	    -e 's:@-IMG_DIR-@:${IMG_DIR}:g'
	mv $@.tmp $@

BUILT_SOURCES = img_dir.h

