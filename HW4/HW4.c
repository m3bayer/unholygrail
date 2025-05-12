#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <math.h>
#include <stdio.h>


//default pins for the SPI0 peripheral
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

#define UPDATES_PER_CYCLE 100
#define SINE_FREQ_HZ 2
#define TRIANGLE_FREQ_HZ 1
#define VREF 3.3f
#define MAX_DAC_VAL 1023

//CS pin control
void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop");
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop");
}

void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop");
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop");
}

void spi_setup() {
    spi_init(SPI_PORT, 1000 * 1000); // 1 MHz SPI
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1); 
}

void dac_write(uint8_t channel, uint16_t value) {
    uint16_t command = 0;
    command |= (channel & 0x01) << 15;     // Channel A or B on chip
    command |= 0b111 << 12; 
    command |= (value & 0x3FF) << 2; 

    uint8_t buf[2];
    buf[0] = (command >> 8) & 0xFF;
    buf[1] = command & 0xFF;

    cs_select(PIN_CS);
    spi_write_blocking(SPI_PORT, buf, 2);
    cs_deselect(PIN_CS);
}

int main() {
    stdio_init_all();
    spi_setup();

    while (true) {
        /*
        // 2Hz Sine wave
        for (int i = 0; i < UPDATES_PER_CYCLE; i++) {
            float theta = 2 * M_PI * i / UPDATES_PER_CYCLE;
            float voltage = (sinf(theta) + 1.0f) / 2.0f * VREF;
            uint16_t value = (uint16_t)((voltage / VREF) * MAX_DAC_VAL);
            dac_write(0, value); // channel A
            sleep_ms(1000 / (SINE_FREQ_HZ * UPDATES_PER_CYCLE));
        }
        */
        
        
        // 1Hz Triangle wave
        for (int i = 0; i < UPDATES_PER_CYCLE / 2; i++) {
            uint16_t value = i * (MAX_DAC_VAL / (UPDATES_PER_CYCLE / 2));
            dac_write(0, value); // channel A
            sleep_ms(1000 / (TRIANGLE_FREQ_HZ * UPDATES_PER_CYCLE));
        }

        for (int i = UPDATES_PER_CYCLE / 2; i >= 0; i--) {
            uint16_t value = i * (MAX_DAC_VAL / (UPDATES_PER_CYCLE / 2));
            dac_write(0, value); // channel A
            sleep_ms(1000 / (TRIANGLE_FREQ_HZ * UPDATES_PER_CYCLE));
        }
        
    }
}