SUBDIRS = img
DIST_SUBDIRS = img

bin_PROGRAMS = jody

jody_SOURCES = background.c \
	background.h \
	gamedefs.h main.c \
	particle.c \
	particle.h \
	player.h \
	resources.c \
	resources.h

jody_LDADD = $(SDL_LIBS)

## Both these ways work to pass the bindir to main.c
## DEFS = -DSCRIPTDIR=\"$(bindir)\" @DEFS@
AM_CPPFLAGS = -DSCRIPTDIR=\"$(bindir)\"

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

