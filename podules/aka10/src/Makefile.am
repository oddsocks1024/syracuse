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
libaka10_la_SOURCES = ../../common/uart/6850.c aka10.c ./../common/joystick/joystick_sdl2.o ../../common/adc/d7002c.o ../../../src/master-cfg-file.o

if OS_LINUX
libaka10_la_SOURCES += ../../common/midi/midi_alsa.c
endif

libaka10_la_CFLAGS = -I../../../src -I../../common/adc -I../../common/joystick -I../../common/midi -I../../common/uart $(shell sdl2-config --cflags)
libaka10_la_LIBADD = @LIBS@

endif
