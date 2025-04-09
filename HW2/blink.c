#include "pico/stdlib.h"
#include <stdio.h>


#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif


#define LED_PIN 16
#define BUTTON_PIN 17

int main() {
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    bool led_on = false;
    bool prev_button_state = true;
    int press_count = 0;

    while (true) {
        bool button_state = gpio_get(BUTTON_PIN);


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