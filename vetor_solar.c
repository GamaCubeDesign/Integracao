#include "common_aocs.h"

int main() {
    int fd = i2c_open();

    // Enviar comando "1"
    uint16_t cmd = '1';
    write(fd, &cmd, 1);

    // Receber inteiro
    int result;
    read(fd, &result, sizeof(int));

    printf("Retorno do vetor solar: %d\n", result);

    close(fd);
    return 0;
}
