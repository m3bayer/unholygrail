#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define PHASE_PIN 15
#define ENABLE_PIN 14

#define FREQ 75000 // 75 kHz

int dutyCycle = 0;

int main() {
    stdio_init_all();
    sleep_ms(1000); // Allow time for USB connection

    // Initialize PHASE pin as digital output
    gpio_init(PHASE_PIN);
    gpio_set_dir(PHASE_PIN, GPIO_OUT);

    // Set ENABLE pin as PWM output
    gpio_set_function(ENABLE_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(ENABLE_PIN);

    // PWM frequency
    float clk_div = 2.0f;
    pwm_set_clkdiv(slice_num, clk_div);

    // PWM wrap
    uint16_t wrap = 1000;
    pwm_set_wrap(slice_num, wrap);

    // Start PWM
    pwm_set_enabled(slice_num, true);
    pwm_set_gpio_level(ENABLE_PIN, 0);

    printf("Motor control ready. Press '+' or '-' to change speed.\n");

    while (true) {
        int input = getchar_timeout_us(0);

        if (input != PICO_ERROR_TIMEOUT) {
            char c = (char)input;

            if (c == '+' && dutyCycle < 100) {
                dutyCycle++;
                printf("Duty Cycle: %d%%\n", dutyCycle);
            } 
            else if (c == '-' && dutyCycle > -100) {
                dutyCycle--;
                printf("Duty Cycle: %d%%\n", dutyCycle);
            }

            // Set dir
            gpio_put(PHASE_PIN, dutyCycle >= 0 ? 0 : 1);

            // Set PWM level
            pwm_set_gpio_level(ENABLE_PIN, (wrap * abs(dutyCycle)) / 100);
        }

        sleep_ms(10);
    }
}