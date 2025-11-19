// main.c

#include "controle_termico.h"
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>

volatile sig_atomic_t stop_flag = 0;

// ids dos sensores(ver na raspberry)
const char *sensor_ids[MAX_SENSORS] = {
    "28-000000000001",  // bateria 1
    "28-000000000002",  // bateria 2
    "28-000000000003"   // externo
};

// Indicar quais sensores são da bateria
int battery_indexes[2] = {0, 1};
int battery_count = 2;

void handle_sigint(int sig){
    stop_flag = 1;
}

int main(){
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigint);

    gpio_init(MOSFET_GPIO_PIN);
    gpio_set(MOSFET_GPIO_PIN, 0);

    double temps[MAX_SENSORS];
    int heater_state = 0;

    printf("Sistema iniciado. Pressione Ctrl+C para sair.\n");

    while(!stop_flag){
        for(int i=0;i<MAX_SENSORS;i++){
            temps[i] = read_ds18b20(sensor_ids[i]);
            if(isnan(temps[i])) printf("Sensor %s: erro\n", sensor_ids[i]);
            else printf("Sensor %s: %.3f C\n", sensor_ids[i], temps[i]);
        }

        double vbus = read_ina219_voltage(I2C_DEVICE, INA_I2C_ADDR);
        if(isnan(vbus)) printf("INA219: erro leitura\n");
        else printf("INA219 tensão: %.3f V\n", vbus);

        int new_state = heater_should_activate(temps, MAX_SENSORS, heater_state);

        if(new_state != heater_state){
            heater_state = new_state;
            gpio_set(MOSFET_GPIO_PIN, heater_state);
            printf("AQUECEDOR -> %s\n", heater_state ? "LIGADO" : "DESLIGADO");
        }

        write_json("/home/gama/controle_termico/health.json", sensor_ids, temps, MAX_SENSORS, vbus);

        sleep(SAMPLE_INTERVAL);
    }

    gpio_set(MOSFET_GPIO_PIN, 0);
    gpio_cleanup(MOSFET_GPIO_PIN);
    printf("Encerrado.\n");
    return 0;
}
