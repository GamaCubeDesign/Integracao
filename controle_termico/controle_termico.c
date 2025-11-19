// controle_termico.c

#include "controle_termico.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <time.h>

//sfys helpers

static int write_file(const char *path, const char *value){
    int fd = open(path, O_WRONLY);
    if(fd < 0) return -1;
    write(fd, value, strlen(value));
    close(fd);
    return 0;
}

int gpio_init(int gpio){
    char path[128];

    // export
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", gpio);
    write_file("/sys/class/gpio/export", buf);
    usleep(100000);

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", gpio);
    return write_file(path, "out");
}

void gpio_set(int gpio, int value){
    char path[128];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", gpio);

    int fd = open(path, O_WRONLY);
    if(fd < 0) return;

    char v = value ? '1' : '0';
    write(fd, &v, 1);
    close(fd);
}

void gpio_cleanup(int gpio){
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", gpio);
    write_file("/sys/class/gpio/unexport", buf);
}

double read_ds18b20(const char *id){
    char path[256];
    snprintf(path, sizeof(path), "/sys/bus/w1/devices/%s/w1_slave", id);

    FILE *f = fopen(path, "r");
    if(!f) return NAN;

    char line[256];
    if(!fgets(line, sizeof(line), f)) { fclose(f); return NAN; }
    if(!fgets(line, sizeof(line), f)) { fclose(f); return NAN; }
    fclose(f);

    char *tpos = strstr(line, "t=");
    if(!tpos) return NAN;

    long t_milli = strtol(tpos+2, NULL, 10);
    return t_milli / 1000.0;
}


double read_ina219_voltage(const char *dev, int addr){
    int fd = open(dev, O_RDWR);
    if(fd < 0) return NAN;

    if(ioctl(fd, I2C_SLAVE, addr) < 0){
        close(fd); return NAN;
    }

    uint8_t reg = 0x02;
    if(write(fd, &reg, 1) != 1){
        close(fd); return NAN;
    }

    uint8_t buf[2];
    if(read(fd, buf, 2) != 2){
        close(fd); return NAN;
    }

    close(fd);

    int16_t raw = (buf[0] << 8) | buf[1];
    int16_t shifted = raw >> 3;
    return shifted * 0.004; // 4 mV per LSB
}


int heater_should_activate(double temps[], int nt, int heater_state){
    int below = 0;
    int above = 1;

    for(int i=0;i<battery_count;i++){
        int idx = battery_indexes[i];
        if(idx < 0 || idx >= nt) continue;

        double t = temps[idx];
        if(isnan(t)) continue;

        if(t < HEATER_ON_THRESHOLD)
            below = 1;

        if(t <= HEATER_OFF_THRESHOLD)
            above = 0;
    }

    if(!heater_state && below)  // desligado e ficou frio
        return 1;

    if(heater_state && above)   // ligado e ficou quente
        return 0;

    return heater_state; // manter estado
}


void write_json(const char *file, const char *ids[], double temps[], int ntemps, double voltage){
    FILE *f = fopen(file, "w");
    if(!f) return;

    time_t now = time(NULL);
    struct tm t;
    gmtime_r(&now, &t);

    char ts[64];
    strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%SZ", &t);

    fprintf(f, "{\n");
    fprintf(f, "  \"timestamp\": \"%s\",\n", ts);

    if(isnan(voltage))
        fprintf(f, "  \"ina219_voltage_v\": null,\n");
    else
        fprintf(f, "  \"ina219_voltage_v\": %.3f,\n", voltage);

    fprintf(f, "  \"temperatures_c\": [\n");
    for(int i=0;i<ntemps;i++){
        fprintf(f, "    {\"id\": \"%s\", \"temp_c\": ", ids[i]);
        if(isnan(temps[i])) fprintf(f, "null}");
        else fprintf(f, "%.3f}", temps[i]);
        fprintf(f, (i==ntemps-1) ? "\n" : ",\n");
    }
    fprintf(f, "  ]\n");
    fprintf(f, "}\n");

    fclose(f);
}
