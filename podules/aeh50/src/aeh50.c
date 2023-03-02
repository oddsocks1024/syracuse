/*
    Acorn Ethernet II podule (AEH50)

    IOC address map :
    0000-1fff - ROM (read)
    2000-2fff - IRQ status (read)
                bit 0 - IRQ active
    3000-3fff - Page register (write)
                bits 0-8 - ROM page (not used with default ROM)
                bit 14 - Remote DMA port access direction (1=read, 0=write)
                bit 15 - DP8390 reset (active low)

    MEMC address map :
    2000-2fff - Remote DMA transfer port
                Used with both byte and word accesses
    3000-3fff - DP8390 registers
                Arc A2-A5 connected to A0-A3
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <pthread.h>
#include "podule_api.h"
#include "aeh50.h"
#include "net.h"
#include "ne2000.h"
#include "config.h"
#include "master-cfg-file.h"
#include "arc.h"

#define BOOL int
#define APIENTRY
#define AEH50LOG LOGDIR "aeh50.log"

static const podule_callbacks_t *podule_callbacks;
static char podule_path[PATH_MAX];

typedef struct aeh50_t {
    uint8_t rom[0x4000];
    uint16_t page_reg;
    int irq_status;
    void *ne2000;
    podule_t *podule;
    net_t *net;
} aeh50_t;

static FILE *aeh50_logf;
static int timestamp = 0;

void aeh50_log(const char *format, ...) {
    char buf[1024];
    char logfile[PATH_MAX];
    get_config_dir_loc(logfile);
    strncat(logfile, AEH50LOG, sizeof(logfile) - strlen(logfile));
    va_list ap;

    if (!aeh50_logf)
        aeh50_logf = fopen(logfile, "wt");

    va_start(ap, format);
    vsprintf(buf, format, ap);
    va_end(ap);
    fprintf(aeh50_logf, "[%08i] : ", timestamp);
    fputs(buf, aeh50_logf);
    fflush(aeh50_logf);
}

void aeh50_fatal(const char *format, ...) {
    aeh50_log(format);
    exit(-1);
}

static uint8_t aeh50_read_b(struct podule_t *podule, podule_io_type type, uint32_t addr) {
    aeh50_t *aeh50 = podule->p;

    if (type == PODULE_IO_TYPE_IOC) {
        switch ((addr & 0x3000)) {
            case 0x0000:
            case 0x1000:
                return aeh50->rom[(addr >> 2) & 0x7ff];

            case 0x2000:
                return aeh50->irq_status;

            case 0x3000:
                return 0xff;
        }
    } else {
        //        aeh50_log("aeh50_read_b MEMC %08x\n", addr);

        switch ((addr & 0x3000)) {
            case 0x2000:
                return ne2000_dma_read_w(0, aeh50->ne2000);

            case 0x3000:
                return ne2000_read((addr >> 2) & 0xf, aeh50->ne2000);
        }

        return 0xff;
    }
}

static uint16_t aeh50_read_w(struct podule_t *podule, podule_io_type type, uint32_t addr) {
    aeh50_t *aeh50 = podule->p;

    if (type == PODULE_IO_TYPE_IOC)
        return aeh50_read_b(podule, type, addr) | 0xff00;
    else {
        //        aeh50_log("aeh50_read_w MEMC %08x\n", addr);

        switch ((addr & 0x3000)) {
            case 0x2000:
                return ne2000_dma_read_w(0, aeh50->ne2000);

            case 0x3000:
                return ne2000_read((addr >> 2) & 0xf, aeh50->ne2000);
        }

        return 0xffff;
    }
}

static void aeh50_write_b(struct podule_t *podule, podule_io_type type, uint32_t addr, uint8_t val) {
    aeh50_t *aeh50 = podule->p;

    if (type == PODULE_IO_TYPE_IOC) {
        switch (addr & 0x3000) {
            case 0x3000:
                aeh50->page_reg = val | (val << 8);
                //                aeh50_log("page_reg = %04x\n", val | (val << 8));
                break;
        }
    } else {
        //        aeh50_log("aeh50_write_b MEMC %08x %02x\n", addr, val);

        switch ((addr & 0x3000)) {
            case 0x2000:
                ne2000_dma_write_w(0, val | (val << 8), aeh50->ne2000);
                break;

            case 0x3000:
                ne2000_write((addr >> 2) & 0xf, val, aeh50->ne2000);
                break;
        }
    }
}

static void aeh50_write_w(struct podule_t *podule, podule_io_type type, uint32_t addr, uint16_t val) {
    aeh50_t *aeh50 = podule->p;

    if (type == PODULE_IO_TYPE_IOC) {
        switch (addr & 0x3000) {
            case 0x3000:
                if (!(aeh50->page_reg & 0x8000) && (val & 0x8000))
                    ne2000_do_reset(aeh50->ne2000);
                aeh50->page_reg = val;
                //                aeh50_log("page_reg = %04x\n", val);
                break;
        }
    } else {
        //        aeh50_log("aeh50_write_w MEMC %08x %04x\n", addr, val);

        switch ((addr & 0x3000)) {
            case 0x2000:
                ne2000_dma_write_w(0, val, aeh50->ne2000);
                break;

            case 0x3000:
                ne2000_write((addr >> 2) & 0xf, val, aeh50->ne2000);
                break;
        }
    }
}

static void aeh50_reset(struct podule_t *podule) {
    aeh50_t *aeh50 = podule->p;
    aeh50_log("Reset podule AEH50\n");
    aeh50->page_reg = 0;
}

void aeh50_set_irq(void *p, int irq) {
    aeh50_t *aeh50 = p;
    // aeh50_log("aeh50_irq: aeh50=%p irq=%i\n", aeh50, irq);
    podule_callbacks->set_irq(aeh50->podule, irq);
    aeh50->irq_status = irq ? 1 : 0;
}

static int aeh50_init(struct podule_t *podule) {
    FILE *f;
    char rom_fn[PATH_MAX];
    aeh50_t *aeh50 = malloc(sizeof(aeh50_t));
    memset(aeh50, 0, sizeof(aeh50_t));

    sprintf(rom_fn, "%s%sEthernetII_ID_ROM.ROM", PODULEROMDIR, "aeh50/");
    aeh50_log("Loading AEH50 ROM %s\n", rom_fn);

    f = fopen(rom_fn, "rb");
    if (!f) {
        aeh50_log("Failed to open %s\n", rom_fn);
        return -1;
    }

    ignore_result(fread(aeh50->rom, 0x4000, 1, f));
    fclose(f);
    aeh50->podule = podule;
    podule->p = aeh50;
    const char *network_device = podule_callbacks->config_get_string(podule, "network_device", NETWORK_DEVICE_DEFAULT);
    aeh50->net = net_init(network_device, &aeh50->rom[0x100]);
    aeh50->ne2000 = ne2000_init(aeh50_set_irq, aeh50, aeh50->net);
    aeh50_log("aeh50_init: podule=%p\n", podule);
    return 0;
}

static void aeh50_close(struct podule_t *podule) {
    aeh50_t *aeh50 = podule->p;
    aeh50->net->close((struct net_t *)aeh50->net);
    ne2000_close(&aeh50->ne2000);
    free(aeh50);
}

static int aeh50_run(struct podule_t *podule, int timeslice_us) {
    aeh50_t *aeh50 = podule->p;
    timestamp++;
    ne2000_poll(aeh50->ne2000);
    return 1000;
}

static podule_config_t aeh50_podule_config = {.items = {{
                                                            .name = "network_device",
                                                            .description = "Network",
                                                            .type = CONFIG_SELECTION_STRING,
                                                            .selection = 0,
                                                            .default_string = NETWORK_DEVICE_DEFAULT,
                                                        },
                                                        {.type = -1}}};

static const podule_header_t aeh50_podule_header = {
    .version = PODULE_API_VERSION,
    .flags = PODULE_FLAGS_UNIQUE,
    .short_name = "aeh50",
    .name = "Acorn Ethernet II podule (AEH50)",
    .functions = {.init = aeh50_init, .close = aeh50_close, .reset = aeh50_reset, .read_b = aeh50_read_b, .read_w = aeh50_read_w, .write_b = aeh50_write_b, .write_w = aeh50_write_w, .run = aeh50_run},
    .config = &aeh50_podule_config};

const podule_header_t *podule_probe(const podule_callbacks_t *callbacks, char *path) {
    aeh50_log("podule_probe %p path=%s\n", &aeh50_podule_header, path);

    podule_callbacks = callbacks;
    strcpy(podule_path, path);

    aeh50_podule_config.items[0].selection = net_get_networks();

    return &aeh50_podule_header;
}

