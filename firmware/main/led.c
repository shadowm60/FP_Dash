#include "led.h"

static gpio_config_t led1_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << LED1_PIN
    };

static gpio_config_t led2_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << LED2_PIN
    };

static gpio_config_t led3_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << LED3_PIN
    };


static const char *TAG = "FP_Dash_LED";
    
void init_led_pins( void ) {

    ESP_LOGI(TAG, "Init LED Pins");


    ESP_ERROR_CHECK(gpio_config(&led1_gpio_config));
    ESP_ERROR_CHECK(gpio_config(&led2_gpio_config));
    ESP_ERROR_CHECK(gpio_config(&led3_gpio_config));


}

void set_led( uint8_t led ) {
   gpio_set_level(led, 1);
}

void clr_led( uint8_t led ) {
   gpio_set_level(led, 0);
}

void toggle_led( uint8_t led ) {
    int clvl = gpio_get_level(led);

    if ( clvl == 0 ) {
        gpio_set_level(led, 1);
    } else {
        gpio_set_level(led, 0);
    }
}
