# Makefile.am for AKA10
AUTOMAKE_OPTIONS = subdir-objects

if BUILD_PODULES
lib_LTLIBRARIES = libaka10.la
noinst_SCRIPTS =
CLEANFILES =

if OS_LINUX
CLEANFILES += $(noinst_SCRIPTS)
noinst_SCRIPTS += ../aka10.so
../aka10.so: libaka10.la
	cp .libs/libaka10.so ../aka10.so
endif

amrefresh:
libaka10_la_SOURCES = ../../common/uart/6850.c aka10.c ../../../src/master-cfg-file.c

if OS_LINUX
libaka10_la_SOURCES += ../../common/midi/midi_alsa.c
endif

libaka10_la_CFLAGS = -I../../../src -I../../common/midi -I../../common/uart
libaka10_la_LIBADD = @LIBS@

endif
