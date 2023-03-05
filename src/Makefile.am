# Makefile.am for Syracuse

AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = syracuse
noinst_SCRIPTS =
CLEANFILES =

amrefresh:

# Special wxWidgets files
WINDRES = $(shell $(WX_CONFIG_PATH) --rescomp)
wx.res: wx.rc
	$(WINDRES) -i wx.rc --input-format=rc -o wx.res -O coff

wx-resources.cpp : syracuse.xrc
	-wxrc -c syracuse.xrc -o wx-resources.cc

# Syracuse
syracuse_SOURCES = master-cfg-file.c 82c711.c 82c711_fdc.c arm.c bmu.c cmos.c colourcard.c config.c cp15.c ddnoise.c \
 debugger.c debugger_swis.c disc.c disc_adf.c disc_apd.c disc_fdi.c disc_hfe.c disc_jfd.c disc_mfm_common.c disc_scp.c ds2401.c \
 eterna.c fdi2raw.c fpa.c g16.c g332.c hostfs.c ide.c ide_a3in.c ide_config.c ide_idea.c ide_riscdev.c \
 ide_zidefs.c ide_zidefs_a3k.c input_sdl2.c ioc.c ioeb.c joystick.c keyboard.c lc.c main.c mem.c memc.c \
 podules.c printer.c riscdev_hdfc.c romload.c sound.c soundopenal.c st506.c st506_akd52.c timer.c vidc.c \
 video_sdl2.c wd1770.c wx-app.cc wx-config.cc wx-config_sel.cc wx-hd_conf.cc wx-console.cc wx-hd_new.cc \
 wx-joystick-config.cc wx-main.cc wx-podule-config.cc wx-resources.cc wx-sdl2.c wx-sdl2-joystick.c

syracuse_CFLAGS = $(subst -fpermissive,,$(shell $(WX_CONFIG_PATH) --cxxflags) $(shell sdl2-config --cflags))
syracuse_CXXFLAGS = $(shell $(WX_CONFIG_PATH) --cxxflags) $(shell sdl2-config --cflags)
syracuse_LDADD = @LIBS@

if OS_LINUX
syracuse_SOURCES += hostfs-unix.c podules-linux.c
syracuse_LDADD += -ldl -lX11
endif


