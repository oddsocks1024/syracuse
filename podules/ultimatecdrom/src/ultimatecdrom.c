/*
    HCCS Ultimate CD-ROM

    IOC Address map :
    0000-1fff : ROM (128k banked)
    3000 : ROM page register
    3200-323f : CD registers
    3200 - Command/Data/Status
    3220 - Flags
    3300 - ???
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include "podule_api.h"
#include "cdrom.h"
#include "mitsumi.h"
#include "sound_out.h"
#include "ultimatecdrom.h"
#include "config.h"
#include "master-cfg-file.h"
#include "arc.h"

#define BOOL int
#define APIENTRY
#define ULTIMATECDROMLOG LOGDIR "ultimate_cdrom.log"

static const podule_callbacks_t *podule_callbacks;
char podule_path[PATH_MAX];
static FILE *cdlogf;

void cdlog(const char *format, ...) {
    char buf[1024];
    char logfile[PATH_MAX];
    get_config_dir_loc(logfile);
    strncat(logfile, ULTIMATECDROMLOG, sizeof(logfile) - strlen(logfile));

    if (!cdlogf)
        cdlogf = fopen(logfile,"wt");

    va_list ap;
    va_start(ap, format);
    vsprintf(buf, format, ap);
    va_end(ap);
    fputs(buf, cdlogf);
    fflush(cdlogf);
}

void cdfatal(const char *format, ...) {
    cdlog(format);
    exit(-1);
}

typedef struct cdrom_t {
    uint8_t rom[0x20000];
    int rom_page;
    int audio_poll_count;
    mitsumi_t mitsumi;
    void *sound_out;
} cdrom_t;

static uint8_t cdrom_read_b(struct podule_t *podule, podule_io_type type, uint32_t addr) {
    cdrom_t *cdrom = podule->p;

    if (type != PODULE_IO_TYPE_IOC)
        return 0xff; /*Only IOC accesses supported*/

    switch (addr&0x3000) {
        case 0x0000: case 0x1000:
            return cdrom->rom[((cdrom->rom_page * 2048) + ((addr & 0x1fff) >> 2)) & 0x1ffff];

        case 0x3000:
            if ((addr & 0x0f00) == 0x0200)
                return mitsumi_read_b(&cdrom->mitsumi, addr);
            break;
    }

    return 0xff;
}

static void cdrom_write_b(struct podule_t *podule, podule_io_type type, uint32_t addr, uint8_t val) {
    cdrom_t *cdrom = podule->p;

    if (type != PODULE_IO_TYPE_IOC)
        return; /*Only IOC accesses supported*/

    switch (addr & 0x3f00) {
        case 0x3000:
            cdrom->rom_page = val;
            return;
        case 0x3200:
            mitsumi_write_b(&cdrom->mitsumi, addr, val);
            break;
    }
}

static int cdrom_run(struct podule_t *podule, int timeslice_us) {
    cdrom_t *cdrom = podule->p;
    mitsumi_poll(&cdrom->mitsumi);
    cdrom->audio_poll_count++;

    if (cdrom->audio_poll_count >= 100) {
        int16_t audio_buffer[(44100*2)/10];
        cdrom->audio_poll_count = 0;
        memset(audio_buffer, 0, sizeof(audio_buffer));
        ioctl_audio_callback(audio_buffer, (44100*2)/10);
        sound_out_buffer(cdrom->sound_out, audio_buffer, 44100/10);
    }

    return 1000; /*1ms*/
}

static int cdrom_init(struct podule_t *podule) {
    FILE *f;
    char rom_fn[PATH_MAX];
    const char *drive_path;

    cdrom_t *cdrom = malloc(sizeof(cdrom_t));
    memset(cdrom, 0, sizeof(cdrom_t));
    sprintf(rom_fn, "%s%sultimatecdrom.rom", PODULEROMDIR, "ultimatecdrom/");
    cdlog("Loading the Ultimate CD-ROM ROM %s\n", rom_fn);
    f = fopen(rom_fn, "rb");

    if (!f) {
        cdlog("Failed to open %s\n", rom_fn);
        return -1;
    }

    ignore_result(fread(cdrom->rom, 0x20000, 1, f));
    fclose(f);
    drive_path = podule_callbacks->config_get_string(podule, "drive_path", "");
    mitsumi_reset(&cdrom->mitsumi, drive_path);
    cdrom->sound_out = sound_out_init(cdrom, 44100, 4410, cdlog, podule_callbacks, podule);
    podule->p = cdrom;
    return 0;
}

static void cdrom_close(struct podule_t *podule) {
    cdrom_t *cdrom = podule->p;
    sound_out_close(cdrom->sound_out);
    free(cdrom);
}

static podule_config_t cdrom_config = {
    .items =
    {
        {
            .name = "drive_path",
            .description = "Host CD-ROM drive",
            .type = CONFIG_SELECTION_STRING,
            .selection = NULL,
            .default_string = ""
        },
        {
            .type = -1
        }
    }
};

static const podule_header_t cdrom_podule_header = {
    .version = PODULE_API_VERSION,
    .flags = PODULE_FLAGS_UNIQUE,
    .short_name = "ultimatecdrom",
    .name = "HCCS Ultimate CD-ROM",
    .functions =
    {
        .init = cdrom_init,
        .close = cdrom_close,
        .read_b = cdrom_read_b,
        .write_b = cdrom_write_b,
        .run = cdrom_run
    },
    .config = &cdrom_config
};

const podule_header_t *podule_probe(const podule_callbacks_t *callbacks, char *path) {
    podule_callbacks = callbacks;
    strcpy(podule_path, path);
    cdrom_config.items[0].selection = cdrom_devices_config();
    return &cdrom_podule_header;
}
