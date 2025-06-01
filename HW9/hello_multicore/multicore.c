/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"

#define FLAG_VALUE 123
#define LED_PIN 15

int input;
float voltage;

void core1_entry() {
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    


    while (1){
        uint32_t g = multicore_fifo_pop_blocking();

        if (g == 0){ //read adc on 0
            uint16_t adcVoltage = adc_read();
            voltage = (float) (adcVoltage * 3.3f / 4096);
        }
        else if (g == 1){ //turn on LED on 1
            gpio_put(LED_PIN, 1);
        }
        else if (g == 2){ //turn off LED on 2
            gpio_put(LED_PIN, 0);
        }

        multicore_fifo_push_blocking(FLAG_VALUE);
    }

}

int main() {
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    
    printf("Hello, multicore?\n");

    /// \tag::setup_multicore[]

    multicore_launch_core1(core1_entry);

    // Wait for it to start up


    printf("HELP...");

    while (1){
        printf("Enter your command (0, 1, 2): \n\r");
        scanf("%d", &input);
        printf("Input Recieved: %d\n\r", input);

        multicore_fifo_push_blocking(input);

        if (input == 0){
            printf("voltage: %f\n\n", voltage);
        }
        else if (input == 1){
            printf("LED on\n\n");
        }
        else if (input == 2){
            printf("LED off\n\n");
        }
        else{
            printf("Error 404 command not found\n\n");
        }
        uint32_t g = multicore_fifo_pop_blocking();
    }

}

    /// \end::setup_multicore[]
