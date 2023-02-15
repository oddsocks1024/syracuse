#include <limits.h>
#include "master-cfg-file.h"

//NAME_MAX

// FIXME These need to be able to be passed into the build
#define GLOBAL_CMOS_DIR "/usr/share/syracuse/cmos/"
#define GLOBAL_CFG_FILENAME ".syracuse.cfg"
#define MACHINE_CFG_DIRNAME ".syracuse/"
#define ARCBASEDIR "/usr/share/syracuse/"
#define PODULELIBDIR "/usr/lib64/syracuse/"
#define PODULEROMDIR "/usr/share/syracuse/roms/podules/"
#define ARCLOG "/tmp/syracuse-log.txt"
#define CPUREGDUMP "/tmp/modules.dmp"
#define HOSTFSDIR "/tmp/"
#define LOGDIR MACHINE_CFG_DIRNAME "logs/"
#define DOES_NOT_EXIST -1
#define DEBUG_LOG 1
#define PODULE_LONG_NAME_LEN 255
#define PODULE_SHORT_NAME_LEN 16

#define AKA05LOG LOGDIR "aka05.log"
#define AKA10LOG "/tmp/aka10-log.txt"
#define AKA12LOG "/tmp/aka12-log.txt"
#define AKA16LOG "/tmp/aka16-log.txt"
#define AKA31LOG LOGDIR "aka31.log"
#define LARKLOG "/tmp/lark-log.txt"
#define MIDIMAXLOG "/tmp/midimax-log.txt"
#define OAKSCSILOG "/tmp/oakscsi-log.txt"
#define ULTIMATECDROMLOG "/tmp/ultimatecdrom-log.txt"

enum {
    FDC_WD1770,
    FDC_82C711,
    FDC_WD1793_A500
};

extern int fdctype;

enum {
    ROM_ARTHUR_030,
    ROM_ARTHUR_120,
    ROM_RISCOS_200,
    ROM_RISCOS_201,
    ROM_RISCOS_300,
    ROM_RISCOS_310,
    ROM_RISCOS_311,
    ROM_RISCOS_319,
    ROM_ARTHUR_120_A500,
    ROM_RISCOS_200_A500,
    ROM_RISCOS_310_A500,
    ROM_MAX
};

extern int romset_available_mask;

char *config_get_romset_name(int romset);
char *config_get_cmos_name(int romset, int fdctype);

enum {
    MONITOR_STANDARD,
    MONITOR_MULTISYNC,
    MONITOR_VGA,
    MONITOR_MONO,
    MONITOR_LCD
};

extern int monitor_type;

enum {
    MACHINE_TYPE_NORMAL,
    MACHINE_TYPE_A4
};

#define CFG_MACHINE 0
#define CFG_GLOBAL  1

extern int machine_type;
extern float config_get_float(int is_global, const char *head, const char *name, float def);
extern int config_get_int(int is_global, const char *head, const char *name, int def);
extern const char *config_get_string(int is_global, const char *head, const char *name, const char *def);
extern void config_set_float(int is_global, const char *head, const char *name, float val);
extern void config_set_int(int is_global, const char *head, const char *name, int val);
extern void config_set_string(int is_global, const char *head, const char *name, char *val);
extern int config_free_section(int is_global, const char *name);
extern void add_config_callback(void(*loadconfig)(), void(*saveconfig)(), void(*onloaded)());
extern char *get_filename(char *s);
extern void get_config_dir_loc(char *s);
extern void append_filename(char *dest, const char *s1, const char *s2, int size);
extern void append_slash(char *s, int size);
extern void put_backslash(char *s);
extern char *get_extension(char *s);
extern void config_load(int is_global, char *fn);
extern void config_save(int is_global, char *fn);
extern void config_dump(int is_global);
extern void loadconfig();
extern void saveconfig();
extern char machine_config_name[256];
extern char machine_config_file[256];
extern char hd_fn[2][512];
extern int hd_spt[2], hd_hpc[2], hd_cyl[2];
extern char machine[7];
extern uint32_t unique_id;
extern char joystick_if[16];
extern char _5th_column_fn[512];
extern int support_rom_enabled;
int machine_is_a500(void);
