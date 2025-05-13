#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>

//default pins for the SPI0 peripheral and external RAM
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_SCK  18
#define PIN_MOSI 19
#define PIN_CS_DAC 17
#define PIN_CS_RAM 15

#define VREF 3.3f
#define MAX_DAC_VAL 1023
#define SAMPLES 1000


//Float for bitshifting
union FloatInt {
    float f;
    uint32_t i;
};

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
    spi_init(SPI_PORT, 1000 * 1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    gpio_init(PIN_CS_DAC);
    gpio_set_dir(PIN_CS_DAC, GPIO_OUT);
    gpio_put(PIN_CS_DAC, 1);

    gpio_init(PIN_CS_RAM);
    gpio_set_dir(PIN_CS_RAM, GPIO_OUT);
    gpio_put(PIN_CS_RAM, 1);
}

void dac_write(uint8_t channel, uint16_t value) {
    uint16_t command = 0;
    command |= (channel & 0x01) << 15;
    command |= 0b111 << 12;
    command |= (value & 0x3FF) << 2;

    uint8_t buf[2];
    buf[0] = (command >> 8) & 0xFF;
    buf[1] = command & 0xFF;

    cs_select(PIN_CS_DAC);
    spi_write_blocking(SPI_PORT, buf, 2);
    cs_deselect(PIN_CS_DAC);
}

void sram_init() {
    cs_select(PIN_CS_RAM);
    uint8_t config[3] = {0x01, 0x40, 0x00};
    spi_write_blocking(SPI_PORT, config, 2);
    cs_deselect(PIN_CS_RAM);
}

void sram_write_float(uint16_t address, float val) {
    union FloatInt num;
    num.f = val;
    
    uint8_t buf[7];
    buf[0] = 0x02; // Write instruction
    buf[1] = (address >> 8) & 0xFF;
    buf[2] = address & 0xFF;
    buf[3] = (num.i >> 24) & 0xFF;
    buf[4] = (num.i >> 16) & 0xFF;
    buf[5] = (num.i >> 8) & 0xFF;
    buf[6] = num.i & 0xFF;

    cs_select(PIN_CS_RAM);
    spi_write_blocking(SPI_PORT, buf, 7);
    cs_deselect(PIN_CS_RAM);
}

float sram_read_float(uint16_t address) {
    uint8_t cmd[3] = {0x03, (address >> 8) & 0xFF, address & 0xFF};
    uint8_t recv[4];

    cs_select(PIN_CS_RAM);
    spi_write_blocking(SPI_PORT, cmd, 3);
    spi_read_blocking(SPI_PORT, 0, recv, 4);
    cs_deselect(PIN_CS_RAM);

    union FloatInt num;
    num.i = ((uint32_t)recv[0] << 24) | ((uint32_t)recv[1] << 16) | ((uint32_t)recv[2] << 8) | recv[3];
    return num.f;
}

int main() {
    stdio_init_all();
    spi_setup();
    sram_init();

    // Write 1000 float sin values to SRAM
    for (int i = 0; i < SAMPLES; i++) {
        float theta = 2.0f * M_PI * i / SAMPLES;
        float voltage = (sinf(theta) + 1.0f) / 2.0f * VREF;
        sram_write_float(i * 4, voltage);
    }

    while (true) {
        for (int i = 0; i < SAMPLES; i++) {
            float voltage = sram_read_float(i * 4);
            uint16_t value = (uint16_t)((voltage / VREF) * MAX_DAC_VAL);
            dac_write(0, value);
            sleep_ms(1); // 1Hz sine wave
        }
    }
}
