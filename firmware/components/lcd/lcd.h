#ifndef _LCD_H_
#define _LCD_H_

#include <stdio.h>
#include <unistd.h>
#include <sys/lock.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/i2c.h"
#include "esp_lcd_panel_ops.h"
#include "hw_config.h"

#if (TARGET == BOARD_2_4)
    #include "esp_lcd_panel_io.h"
    #include "esp_lcd_panel_vendor.h"
    #include "esp_lcd_ili9341.h"    
    #include "esp_lcd_touch_xpt2046.h"
#elif (TARGET == BOARD_5)
    #include "esp_lcd_panel_rgb.h"
    #include "esp_lcd_touch_gt911.h"
#elif (TARGET == BOARD_7)
#endif


#include "lvgl.h"


#include "ui.h"


void setup_LCD_Panel( void );

#endif