/*
    I2C + CMOS RAM emulation
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "arc.h"
#include "bmu.h"
#include "cmos.h"
#include "config.h"
#include "timer.h"

char cmosdir[PATH_MAX];
int cmos_changed = 0;
int i2c_clock = 1, i2c_data = 1;

#define TRANSMITTER_CMOS 1
#define TRANSMITTER_ARM -1
#define I2C_IDLE             0
#define I2C_RECEIVE          1
#define I2C_TRANSMIT         2
#define I2C_ACKNOWLEDGE      3
#define I2C_TRANSACKNOWLEDGE 4

enum {
    CMOS_IDLE,
    CMOS_RECEIVEADDR,
    CMOS_RECEIVEDATA,
    CMOS_SENDDATA,
    CMOS_NOT_SELECTED
};


static struct {
    int state;
    int last_data;
    int pos;
    int transmit;
    uint8_t byte;
} i2c;


static struct {
    uint8_t device_addr;
    int state;
    int addr;
    int rw;
    uint8_t ram[256];
    uint8_t rtc_ram[8];
    emu_timer_t timer;
} cmos;


static struct {
    int msec;
    int sec;
    int min;
    int hour;
    int day;
    int mon;
    int year;
} systemtime;

static const int rtc_days_in_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static void cmos_get_time();

/*
 * Attempts to load the machine specific CMOS. If it can't find it, it loads the global default
 * CMOS. Then this written out to the machine specific CMOS
*/
void cmos_load() {
    char fn[PATH_MAX];
    char cmos_name[PATH_MAX];
    FILE *cmosf;

    LOG_CMOS("Read cmos %i\n", romset);
    snprintf(cmos_name, sizeof(cmos_name), "%s.%s.cmos.bin", machine_config_name, config_get_cmos_name(romset, fdctype));
    append_filename(fn, cmosdir, cmos_name, 511);
    cmosf = fopen(fn, "rb");

    if (cmosf) {
        ignore_result(fread(cmos.ram, 256, 1, cmosf));
        fclose(cmosf);
        LOG_CMOS("Read CMOS contents from %s\n", fn);
    }
    else {
        LOG_CMOS("%s doesn't exist; resetting CMOS\n", fn);
        snprintf(cmos_name, sizeof(cmos_name), "%s/cmos.bin", config_get_cmos_name(romset, fdctype));
        append_filename(fn, GLOBAL_CMOS_DIR, cmos_name, 511);
        cmosf = fopen(fn, "rb");

        if (cmosf) {
            ignore_result(fread(cmos.ram, 256, 1, cmosf));
            fclose(cmosf);
        }
        else {
            memset(cmos.ram, 0, 256);
        }
    }

    cmos_get_time();
}


void cmos_save() {
    char fn[PATH_MAX];
    char cmos_name[PATH_MAX];
    FILE *cmosf;

    LOG_CMOS("Writing CMOS %i\n", romset);
    snprintf(cmos_name, sizeof(cmos_name), "%s.%s.cmos.bin", machine_config_name, config_get_cmos_name(romset, fdctype));
    append_filename(fn, cmosdir, cmos_name, 511);

    LOG_CMOS("Writing %s\n", fn);
    cmosf = fopen(fn, "wb");
    fwrite(cmos.ram, 256, 1, cmosf);
    fclose(cmosf);
}


static void cmos_stop() {
    LOG_CMOS("cmos_stop()\n");
    cmos.state = CMOS_IDLE;
    i2c.transmit = TRANSMITTER_ARM;
}


static void cmos_next_byte() {
    LOG_CMOS("cmos_next_byte(%d)\n", cmos.addr);

    if (!machine_is_a500() || cmos.device_addr == 0xa0) {
        i2c.byte = cmos.ram[(cmos.addr++) & 0xFF];
    }
    else if (machine_is_a500() && cmos.device_addr == 0xd0) {

        if (!(cmos.addr & 0x70)) {
            i2c.byte = cmos.rtc_ram[cmos.addr & 0x07];
            cmos.addr = (cmos.addr & 0x70) | ((cmos.addr + 1) & 7);
        }
        else {
            i2c.byte = 0;
        }
    }
}


static void cmos_get_time() {
    int c, d;

    LOG_CMOS("cmos_get_time()\n");

    if (machine_is_a500()) {
        d = systemtime.hour % 10;
        c = systemtime.hour / 10;
        cmos.rtc_ram[0] = d | (c << 4);
        d = systemtime.min % 10;
        c = systemtime.min / 10;
        cmos.rtc_ram[1] = d | (c << 4);
        d = systemtime.day % 10;
        c = systemtime.day / 10;
        cmos.rtc_ram[2] = d | (c << 4);
        d = systemtime.mon % 10;
        c = systemtime.mon / 10;
        cmos.rtc_ram[3] = d | (c << 4);
    }
    else {
        c = systemtime.msec / 10;
        d = c % 10;
        c /= 10;
        cmos.ram[1] = d | (c << 4);
        d = systemtime.sec % 10;
        c = systemtime.sec / 10;
        cmos.ram[2] = d | (c << 4);
        d = systemtime.min % 10;
        c = systemtime.min / 10;
        cmos.ram[3] = d | (c << 4);
        d = systemtime.hour % 10;
        c = systemtime.hour / 10;
        cmos.ram[4] = d | (c << 4);
        d = systemtime.day % 10;
        c = systemtime.day / 10;
        cmos.ram[5] = d | (c << 4);
        d = systemtime.mon % 10;
        c = systemtime.mon / 10;
        cmos.ram[6] = d | (c << 4);
    }
}


static void cmos_tick(void *p) {
    timer_advance_u64(&cmos.timer, TIMER_USEC * 10000); /*10ms*/
    systemtime.msec += 10;

    if (systemtime.msec >= 1000) {
        systemtime.msec = 0;
        systemtime.sec++;

        if (systemtime.sec >= 60) {
            systemtime.sec = 0;
            systemtime.min++;

            if (systemtime.min >= 60) {
                systemtime.min = 0;
                systemtime.hour++;

                if (systemtime.hour >= 24) {
                    systemtime.hour = 0;
                    systemtime.day++;

                    if (systemtime.day >= rtc_days_in_month[systemtime.mon]) {
                        systemtime.day = 0;
                        systemtime.mon++;

                        if (systemtime.mon >= 12) {
                            systemtime.mon = 0;
                            systemtime.year++;
                        }
                    }
                }
            }
        }
    }
}


void cmos_init() {
    struct tm *cur_time_tm;
    time_t cur_time;
    time(&cur_time);
    cur_time_tm = localtime(&cur_time);
    systemtime.msec = 0;
    systemtime.sec = cur_time_tm->tm_sec;
    systemtime.min = cur_time_tm->tm_min;
    systemtime.hour = cur_time_tm->tm_hour;
    systemtime.day = cur_time_tm->tm_mday;
    systemtime.mon = cur_time_tm->tm_mon + 1;
    systemtime.year = cur_time_tm->tm_year + 1900;
    timer_add(&cmos.timer, cmos_tick, NULL, 1);
}


void cmos_write(uint8_t byte) {
    LOG_CMOS("cmos_write()\n");

    switch (cmos.state) {
        case CMOS_IDLE:
            cmos.rw = byte & 1;
            /*
             * A500 has PCF8570 EEPROM at 0xa0, PCF8573 RTC at 0xd0
              Production machines have PCF8583 at 0xa0
            */
            cmos.device_addr = byte & 0xfe;

            if (cmos.device_addr == 0xa0 || (cmos.device_addr == 0xa2 && machine_type == MACHINE_TYPE_A4) || (cmos.device_addr == 0xd0 && machine_is_a500())) {

                if (cmos.rw) {
                        cmos.state = CMOS_SENDDATA;
                        i2c.transmit = TRANSMITTER_CMOS;

                    if (cmos.device_addr == 0xa0) {

                        if (!machine_is_a500() && (cmos.addr < 0x10)) {
                            cmos_get_time();
                        }

                        i2c.byte = cmos.ram[(cmos.addr++) & 0xFF];
                    }
                    else if (cmos.device_addr == 0xa2 && machine_type == MACHINE_TYPE_A4) {
                        i2c.byte = bmu_read(cmos.addr);
                        cmos.addr++;
                    }
                    else if (cmos.device_addr == 0xd0 && machine_is_a500()) {
                        cmos_get_time();

                        if (!(cmos.addr & 0x70)) {
                            i2c.byte = cmos.rtc_ram[cmos.addr & 0x07];
                            cmos.addr = (cmos.addr & 0x70) | ((cmos.addr + 1) & 7);
                        }
                        else {
                            i2c.byte = 0;
                        }
                    }
                }
                else {
                    cmos.state = CMOS_RECEIVEADDR;
                    i2c.transmit = TRANSMITTER_ARM;
                }
            }
            else {
                cmos.state = CMOS_NOT_SELECTED;
                i2c.byte = 0xff;
            }
            return;

        case CMOS_RECEIVEADDR:
            cmos.addr = byte;

            if (cmos.rw) {
                cmos.state = CMOS_SENDDATA;
            }
            else {
                cmos.state = CMOS_RECEIVEDATA;
            }

            break;

        case CMOS_RECEIVEDATA:

            if (cmos.device_addr == 0xa0) {

                if (!cmos_changed) {
                    cmos_changed = CMOS_CHANGE_DELAY;
                }

                cmos.ram[(cmos.addr++) & 0xFF] = byte;
            }
            else if (cmos.device_addr == 0xa2 && machine_type == MACHINE_TYPE_A4) {
                bmu_write(cmos.addr, byte);
                cmos.addr++;
            }
            else if (cmos.device_addr == 0xd0 && machine_is_a500()) {

                if (!(cmos.addr & 0x70)) {
                    cmos.rtc_ram[cmos.addr & 0x07] = byte;
                    cmos.addr = (cmos.addr & 0x70) | ((cmos.addr + 1) & 7);
                }
            }

            break;

        case CMOS_SENDDATA:
#ifndef RELEASE_BUILD
            fatal("Send data %02X\n", cmos.addr);
#endif
            break;
    }
}


void i2c_change(int new_clock, int new_data) {

    switch (i2c.state) {
        case I2C_IDLE:
            if (i2c_clock && new_clock) {

                if (i2c.last_data && !new_data)  { /*Start bit*/
                    i2c.state = I2C_RECEIVE;
                    i2c.pos = 0;
                }
            }

            break;

        case I2C_RECEIVE:
            if (!i2c_clock && new_clock) {
                i2c.byte <<= 1;

                if (new_data) {
                    i2c.byte |= 1;
                }
                else {
                    i2c.byte &= 0xFE;
                }

                i2c.pos++;

                if (i2c.pos == 8) {
                    cmos_write(i2c.byte);
                    i2c.state = I2C_ACKNOWLEDGE;
                }
            }
            else if (i2c_clock && new_clock && new_data && !i2c.last_data)  { /*Stop bit*/
                i2c.state = I2C_IDLE;
                cmos_stop();
            }
            else if (i2c_clock && new_clock && !new_data && i2c.last_data) { /*Start bit*/
                i2c.pos = 0;
                cmos.state = CMOS_IDLE;
            }

            break;

        case I2C_ACKNOWLEDGE:
            if (!i2c_clock && new_clock) {
                new_data = 0;
                i2c.pos = 0;

                if (i2c.transmit == TRANSMITTER_ARM) {
                    i2c.state = I2C_RECEIVE;
                }
                else {
                    i2c.state = I2C_TRANSMIT;
                }
            }
            break;

        case I2C_TRANSACKNOWLEDGE:

            if (!i2c_clock && new_clock) {
                if (new_data)  { /*It's not acknowledged - must be end of transfer*/
                    i2c.state = I2C_IDLE;
                    cmos_stop();
                }
                else { /*Next byte to transfer*/
                    i2c.state = I2C_TRANSMIT;
                    cmos_next_byte();
                    i2c.pos = 0;
                }
            }

            break;

        case I2C_TRANSMIT:

            if (!i2c_clock && new_clock) {
                i2c_data = new_data = i2c.byte & 128;
                i2c.byte <<= 1;
                i2c.pos++;

                if (i2c.pos == 8) {
                    i2c.state = I2C_TRANSACKNOWLEDGE;
                }

                i2c_clock = new_clock;
                return;
            }
            break;
    }

    if (!i2c_clock && new_clock) {
        i2c_data = new_data;
    }

    i2c.last_data = new_data;
    i2c_clock = new_clock;
}
