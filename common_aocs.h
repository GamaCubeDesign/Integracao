#ifndef COMMON_AOCS_H
#define COMMON_AOCS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define I2C_ADDR 0x12

int i2c_open() {
    int fd = open("/dev/i2c-1", O_RDWR);
    if (fd < 0) {
        perror("Erro ao abrir /dev/i2c-1");
        exit(1);
    }

    if (ioctl(fd, I2C_SLAVE, I2C_ADDR) < 0) {
        perror("Erro ao configurar I2C_SLAVE");
        exit(1);
    }
    return fd;
}

#endif
