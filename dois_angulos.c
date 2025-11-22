#include "common_aocs.h"
#include <stdio.h>
#include <stdint.h>

int main() {
    int fd = i2c_open();

    uint16_t a, b;

    printf("Digite o valor de A (0–65535): ");
    scanf("%hu", &a);

    printf("Digite o valor de B (0–65535): ");
    scanf("%hu", &b);

    // Montar mensagem I2C
    // Comando 2 + dois uint16 (5 bytes)
    uint8_t buf[5];
    buf[0] = '2';        // comando

    buf[1] = a & 0xFF;
    buf[2] = a >> 8;

    buf[3] = b & 0xFF;
    buf[4] = b >> 8;

    // Envia comando + parâmetros
    int w = write(fd, buf, 5);
    if (w != 5) {
        printf("Erro no envio I2C! Enviou %d bytes.\n", w);
    }

    close(fd);
    return 0;
}
