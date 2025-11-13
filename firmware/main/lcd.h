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
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_ili9341.h"    
#include "lvgl.h"
#include "esp_lcd_touch_xpt2046.h"

#include "ui.h"


/*********** LCD data ********************/
#define LCD_H_RES                  240
#define LCD_V_RES                  320
#define LCD_PIXEL_CLOCK_HZ         (20 * 1000 * 1000)
#define LCD_BK_LIGHT_ON_LEVEL      1
#define LCD_BK_LIGHT_OFF_LEVEL     !LCD_BK_LIGHT_ON_LEVEL
#define LCD_CMD_BITS               8
#define LCD_PARAM_BITS             8

#define LVGL_DRAW_BUF_LINES    20 // number of display lines in each draw buffer
#define LVGL_TICK_PERIOD_MS    2
#define LVGL_TASK_MAX_DELAY_MS 500
#define LVGL_TASK_MIN_DELAY_MS 1000 / CONFIG_FREERTOS_HZ
#define LVGL_TASK_STACK_SIZE   (4 * 1024)
#define LVGL_TASK_PRIORITY     2
#define LVGL_TASK_STACK_SIZE   (4 * 1024)
#define LVGL_TASK_PRIORITY     2
#define CONFIG_LCD_MIRROR      1

/*********** GPIO initi  *****************/
#define BL_PIN_NUM_BK_LIGHT        27

//HSPI -> SPI2_HOST
//BSPI -> SPI3_HOST
#define LCD_HOST                   SPI2_HOST
#define LCD_PIN_NUM_SCLK           14
#define LCD_PIN_NUM_MOSI           13
#define LCD_PIN_NUM_MISO           12

#define LCD_PIN_NUM_LCD_DC         2
#define LCD_PIN_NUM_LCD_RST        -1 //not connected
#define LCD_PIN_NUM_LCD_CS         15
#define LCD_PIN_NUM_BK_LIGHT       27
#define LCD_PIN_NUM_TOUCH_CS       15
#define PIN_NUM_TOUCH_CS           33

void setup_LCD_Panel( void );

#endif