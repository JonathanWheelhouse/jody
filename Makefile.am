AUTOMAKE_OPTIONS = 0.1 gnu dist-bzip2
SUBDIRS = src

cscope.files:
	find . -name '*.[ch]' > cscope.files

cscope.out: cscope.files
	cscope -b
