#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

// I2C settings
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9
#define HEART_LED 25

// config registers
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C
// sensor data registers:
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H   0x41
#define TEMP_OUT_L   0x42
#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48
#define WHO_AM_I     0x75

#define MPU6050_ADDR 0x68

void heartbeat() {
    gpio_put(HEART_LED, 1);
    sleep_ms(50);
    gpio_put(HEART_LED, 0);
    sleep_ms(50);
}

void mpu6050_init() {
    uint8_t cmds[][2] = {
        {PWR_MGMT_1, 0x00},
        {ACCEL_CONFIG, 0x00},
        {GYRO_CONFIG, 0x18},
    };
    for (int i = 0; i < 3; i++) {
        i2c_write_blocking(I2C_PORT, MPU6050_ADDR, cmds[i], 2, false);
    }
}

void read_accel(int16_t *ax, int16_t *ay) {
    uint8_t reg = ACCEL_XOUT_H;
    uint8_t buf[4];
    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, MPU6050_ADDR, buf, 4, false);

    *ax = (buf[0] << 8) | buf[1];
    *ay = (buf[2] << 8) | buf[3];
}

void ssd1306_drawLine(int x0, int y0, int x1, int y1, int color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        ssd1306_drawPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void draw_vector(float ax, float ay) {
    if (ax > 2) ax = 2;
    if (ax < -2) ax = -2;
    if (ay > 2) ay = 2;
    if (ay < -2) ay = -2;

    int x0 = 64, y0 = 16;
    int x1 = x0 - (int)(ax * 30);
    int y1 = y0 + (int)(ay * 20);

    ssd1306_clear();
    ssd1306_drawLine(x0, y0, x1, y1, 1);
    ssd1306_drawPixel(x0, y0, 1); // center
    ssd1306_update();
}

int main() {
    stdio_init_all();
    
    gpio_init(HEART_LED);
    gpio_set_dir(HEART_LED, GPIO_OUT);

    i2c_init(I2C_PORT, 400000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Check MPU6050 connection
    uint8_t who = 0;
    uint8_t reg = WHO_AM_I;
    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, MPU6050_ADDR, &who, 1, false);
    if (who != 0x68) while (1); // freeze if not found

    mpu6050_init();
    ssd1306_setup();

    while (true) {
        heartbeat();

        int16_t ax_raw, ay_raw;
        read_accel(&ax_raw, &ay_raw);

        float ax = ax_raw * 0.000061f;
        float ay = ay_raw * 0.000061f;

        draw_vector(ax, ay);
    }
}

