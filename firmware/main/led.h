#ifndef _LED_H_
#define _LED_H_

#include <stdio.h>
#include <stdlib.h>
#include "esp_timer.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define LED1_PIN	17
#define LED2_PIN	4
#define LED3_PIN	16

void init_led_pins( void );
void set_led( uint8_t led );
void clr_led( uint8_t led );
void toggle_led( uint8_t led );

#endif