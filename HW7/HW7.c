#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "ssd1306.h"
#include "font.h"
#include <string.h>

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 12
#define I2C_SCL 13

int pico_led_init(void);
void pico_set_led(bool led_on);
void drawChar(int Xo, int Yo, char character);
void drawStr(int Xo, int Yo, char str[]);

int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c


    //display init
    ssd1306_setup();

    //LED init
    pico_led_init();
    pico_set_led(true);

    //adc init
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);
    
    char voltStr[50];
    char fpsStr[50];
    uint64_t currentTime = to_us_since_boot(get_absolute_time());
    uint64_t lastTime = currentTime;
    int frameCounter = 0;
    float fps = 0.0f;

    while (true){
        sleep_ms(1);

        uint16_t adcRaw = adc_read();
        float volt = adcRaw * 3.3f / 4096.0f;;
        sprintf(voltStr, "Voltage: %.2f V", volt);

        currentTime = to_us_since_boot(get_absolute_time());
        float delta_ms = (currentTime - lastTime) / 1000.0f;
        fps = 1000.0f / delta_ms;
        lastTime = currentTime;
        sprintf(fpsStr, "FPS: %.2f", fps);

        ssd1306_clear();
        drawStr(20, 8, voltStr);
        drawStr(65, 25, fpsStr);

        ssd1306_update();

    }



    //testing writing strings
    /*
    char message[50];
    strcpy(message, "Loremipsumdolorumipsocapsumetubrutus");
    ssd1306_clear();
    drawStr(0, 0, message);
    ssd1306_update();
    */
    
    // testing drawChar
    /*
    ssd1306_clear();
    drawChar(30, 12, 'M');
    ssd1306_update();
    */

    //blink led and pizel on screen every second
    /*
    bool pixel_on = false;

    while (true) {
        sleep_ms(500);

        //clear screen
        ssd1306_clear();


        ssd1306_drawPixel(0, 0, pixel_on);
        ssd1306_update();

        gpio_put(PICO_DEFAULT_LED_PIN, pixel_on);
        pixel_on = !pixel_on;
    }
        */

    
}

int pico_led_init(void) {
    #if defined(PICO_DEFAULT_LED_PIN)
        gpio_init(PICO_DEFAULT_LED_PIN);
        gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
        return PICO_OK;
    #elif defined(CYW43_WL_GPIO_LED_PIN)
        return cyw43_arch_init();
    #endif
}

void pico_set_led(bool led_on) {
    #if defined(PICO_DEFAULT_LED_PIN)
        gpio_put(PICO_DEFAULT_LED_PIN, led_on);
    #elif defined(CYW43_WL_GPIO_LED_PIN)
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
    #endif
}

void drawChar(int Xo, int Yo, char character) {

    char colBits;

    for (int i=0; i<5; i++){
        // space is at 0x20, so shift the index
        colBits = ASCII[(int)character - 0x20][i];

        for (int j=0; j<8; j++){
            //check if bit is on
            bool pixelOn = (colBits >> j) & 0x01;
            ssd1306_drawPixel(Xo + i, Yo + j, pixelOn);
        }
    }
}

void drawStr(int Xo, int Yo, char str[]){

    int y = Yo;
    int x = Xo;
    for (int i=0; str[i]!='\0'; i++){
        if ((x+5) > 128){
            x = Xo;
            y += 8;
        }
        drawChar(x, y, str[i]);
        x += 6;
    }

}