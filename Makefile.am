AUTOMAKE_OPTIONS = 0.1 gnu dist-bzip2
SUBDIRS = src

cscope.files:
	find . -name '*.[ch]' > cscope.files

cscope.out: cscope.files
	cscope -b

etags:
	# make sure etags.files is up to date.
	${MAKE} -f Makefile.TAGS etags.files
	# now use the contents of etags.files to re-make TAGS if needed.
	${MAKE} -f Makefile.TAGS etags

.PHONY: etags
