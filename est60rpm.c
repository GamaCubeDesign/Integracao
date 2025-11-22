#include "common_aocs.h"

int main() {
    int fd = i2c_open();

    uint16_t cmd = '4';
    write(fd, &cmd, 1);

    printf("Comando 4 enviado.\n");

    close(fd);
    return 0;
}
