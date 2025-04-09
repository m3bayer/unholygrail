#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define LED_PIN 16
#define BUTTON_PIN 17

void wait_for_usb_connection() {
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
}

void read_adc_samples(int count) {
    for (int i = 0; i < count; i++) {
        uint16_t result = adc_read();
        float voltage = result * 3.3f / 4095.0f;
        printf("Sample %d: %.2f V\r\n", i + 1, voltage);
        sleep_ms(10); // 100 Hz = every 10ms
    }
}

int main()
{
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    gpio_put(LED_PIN, 0);

    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0

    wait_for_usb_connection();

    gpio_put(LED_PIN, 1);

    while (gpio_get(BUTTON_PIN)) {
        sleep_ms(10);
    }

    gpio_put(LED_PIN, 0);

    while (1) {
        printf("Enter number of samples (1-100): ");
        int sample_count = 0;
        scanf("%d", &sample_count);
        read_adc_samples(sample_count);
    }
}
