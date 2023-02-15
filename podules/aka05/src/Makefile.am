# Makefile.am for AKA05
AUTOMAKE_OPTIONS = subdir-objects

if BUILD_PODULES
lib_LTLIBRARIES = libaka05.la
noinst_SCRIPTS =
CLEANFILES =

if OS_LINUX
CLEANFILES += $(noinst_SCRIPTS)
noinst_SCRIPTS += ../aka05.so
../aka05.so: libaka05.la
	cp .libs/libaka05.so ../aka05.so
endif

amrefresh:
libaka05_la_SOURCES = aka05.c ../../../src/master-cfg-file.c
libaka05_la_CFLAGS = -I../../../src
libaka05_la_LIBADD = @LIBS@

endif
