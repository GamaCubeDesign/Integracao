#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <wiringPi.h>

#define GPIO_PIN 23     // GPIO BCM 23

int main() {
    printf("Iniciando controle de temperatura...\n");

    wiringPiSetupGpio();  // usa numeração BCM
    pinMode(GPIO_PIN, OUTPUT);
    digitalWrite(GPIO_PIN, LOW);

    while (1) {
        digitalWrite(GPIO_PIN, HIGH);
        sleep(20);  // espera 5s
    }

    return 0;
}
