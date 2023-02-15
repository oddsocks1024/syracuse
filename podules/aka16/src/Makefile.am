# Makefile.am for AKA16
AUTOMAKE_OPTIONS = subdir-objects

if BUILD_PODULES
lib_LTLIBRARIES = libaka16.la
noinst_SCRIPTS =
CLEANFILES =

if OS_LINUX
CLEANFILES += $(noinst_SCRIPTS)
noinst_SCRIPTS += ../aka16.so
../aka16.so: libaka16.la
	cp .libs/libaka16.so ../aka16.so
endif

amrefresh:
libaka16_la_SOURCES = ../../common/uart/scc2691.c aka16.c ../../../src/master-cfg-file.c

if OS_LINUX
libaka16_la_SOURCES += ../../common/midi/midi_alsa.c
endif

libaka16_la_CFLAGS = -I../../../src -I../../common/midi -I../../common/uart
libaka16_la_LIBADD = @LIBS@
endif
