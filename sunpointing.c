#include "common_aocs.h"

int main() {
    int fd = i2c_open();

    uint16_t cmd = '3';
    write(fd, &cmd, 1);

    printf("Comando 3 enviado.\n");

    close(fd);
    return 0;
}
