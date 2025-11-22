// controle_termico.h
// gcc -o controle_termico controle_termico.c -lm

#ifndef CONTROLE_TERMICO_H
#define CONTROLE_TERMICO_H

#include <stdint.h>

#define MAX_SENSORS 3

// IDs dos DS18B20 (preencher na main)
extern const char *sensor_ids[MAX_SENSORS];

// Índices dos sensores de bateria (preencher na main)
extern int battery_indexes[2];
extern int battery_count;

// GPIO MOSFET
#define MOSFET_GPIO_PIN 17

// leituras (segundos)
#define SAMPLE_INTERVAL 30


#define HEATER_ON_THRESHOLD   10.0   // liga abaixo de 10 
#define HEATER_OFF_THRESHOLD  12.0   // desliga acima de 12

// INA219
#define INA_I2C_ADDR 0x40
#define I2C_DEVICE   "/dev/i2c-1"

double read_ds18b20(const char *id);
double read_ina219_voltage(const char *dev, int addr);

int gpio_init(int gpio);
void gpio_set(int gpio, int value);
void gpio_cleanup(int gpio);

int heater_should_activate(double temps[], int nt, int heater_state);
void write_json(const char *file, const char *ids[], double temps[], int ntemps, double voltage);

#endif
