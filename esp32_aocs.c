//gcc raspi_i2c_master.c -o i2c_master

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <string.h>

#define I2C_BUS  "/dev/i2c-1"
#define ESP32_ADDR  0x28   // i2c esp

int main() {
    int fd = open(I2C_BUS, O_RDWR);
    if (fd < 0) {
        perror("Erro ao abrir I2C");
        return 1;
    }

    if (ioctl(fd, I2C_SLAVE, ESP32_ADDR) < 0) {
        perror("Erro ao conectar no dispositivo I2C");
        return 1;
    }

    char msg[] = "PING";
    if (write(fd, msg, strlen(msg)) < 0) {
        perror("Erro ao enviar");
    } else {
        printf("Enviado: %s\n", msg);
    }

    usleep(50000); // ESP32 precisa de tempo para responder

    char buffer[32] = {0};
    int len = read(fd, buffer, sizeof(buffer));
    if (len < 0) {
        perror("Erro ao ler");
    } else {
        printf("Recebido (%d bytes): %s\n", len, buffer);
    }

    close(fd);
    return 0;
}
