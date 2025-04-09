/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include <stdio.h>

// Pico W devices use a GPIO on the WIFI chip for the LED,
// so when building for Pico W, CYW43_WL_GPIO_LED_PIN will be defined
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif


#define LED_PIN 16      // Change if you're using an external LED
#define BUTTON_PIN 17   // Change to your actual button GPIO

int main() {
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN); // Use internal pull-up (active-low button)

    bool led_on = false;
    bool prev_button_state = true;
    int press_count = 0;

    while (true) {
        bool button_state = gpio_get(BUTTON_PIN);

        // Check if button was just pressed
        if (!button_state && prev_button_state) {
            led_on = !led_on;
            gpio_put(LED_PIN, led_on);
            sleep_ms(25); // Debounce delay

            press_count++;
            printf("Button pressed %d time%s\n", press_count, press_count == 1 ? "" : "s");
        }

        prev_button_state = button_state;
        sleep_ms(10);
    }
}