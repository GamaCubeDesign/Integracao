#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <wiringPi.h>

#define GPIO_PIN 23     // GPIO BCM 23
#define TEMP_THRESHOLD 10.0

// --- Ler temperatura ---
float read_temperature() {
    DIR *dir;
    struct dirent *ent;
    char path[128];

    dir = opendir("/sys/bus/w1/devices/");
    if (!dir) {
        perror("Erro abrindo /sys/bus/w1/devices");
        return -1000;
    }

    char device[128] = {0};

    while ((ent = readdir(dir)) != NULL) {
        if (strncmp(ent->d_name, "28-", 3) == 0) {
            snprintf(device, sizeof(device),
                "/sys/bus/w1/devices/%s/w1_slave", ent->d_name);
            break;
        }
    }
    closedir(dir);

    if (device[0] == 0) {
        printf("Sensor não encontrado!\n");
        return -1000;
    }

    FILE *fp = fopen(device, "r");
    if (!fp) {
        perror("Erro lendo sensor");
        return -1000;
    }

    char line1[128], line2[128];
    fgets(line1, sizeof(line1), fp);
    fgets(line2, sizeof(line2), fp);
    fclose(fp);

    if (!strstr(line1, "YES")) {
        printf("Leitura inválida (CRC)\n");
        return -1000;
    }

    char *tptr = strstr(line2, "t=");
    if (!tptr) return -1000;

    int raw = atoi(tptr + 2);
    return raw / 1000.0;
}

int main() {
    printf("Iniciando controle de temperatura...\n");

    wiringPiSetupGpio();  // usa numeração BCM
    pinMode(GPIO_PIN, OUTPUT);
    digitalWrite(GPIO_PIN, LOW);

    while (1) {
        float temp = read_temperature();

        if (temp > -100) {
            printf("Temperatura: %.2f°C\n", temp);

            if (temp < TEMP_THRESHOLD) {
                printf(" -> Abaixo de 10°C: LIGANDO MOSFET\n");
                digitalWrite(GPIO_PIN, HIGH);
            } else {
                printf(" -> Acima de 10°C: DESLIGANDO MOSFET\n");
                digitalWrite(GPIO_PIN, LOW);
            }
        } else {
            printf("Erro lendo temperatura.\n");
        }

        sleep(5);  // espera 1s
    }

    return 0;
}
