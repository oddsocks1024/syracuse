# Makefile.am for AKA12
AUTOMAKE_OPTIONS = subdir-objects

if BUILD_PODULES
lib_LTLIBRARIES = libaka12.la
noinst_SCRIPTS =
CLEANFILES =

if OS_LINUX
CLEANFILES += $(noinst_SCRIPTS)
noinst_SCRIPTS += ../aka12.so
../aka12.so: libaka12.la
	cp .libs/libaka12.so ../aka12.so
endif

amrefresh:
libaka12_la_SOURCES = ../../common/misc/6522.c ../../common/uart/scc2691.c aka12.c ../../../src/master-cfg-file.c

if OS_LINUX
libaka12_la_SOURCES += ../../common/midi/midi_alsa.c
endif

libaka12_la_CFLAGS = -I../../../src -I../../common/midi -I../../common/misc -I../../common/uart
libaka12_la_LIBADD = @LIBS@
endif
