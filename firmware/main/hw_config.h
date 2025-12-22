#ifndef _HW_CONFIG_H_
#define _HW_CONFIG_H_

#define BOARD_2_4       1
#define BOARD_5         2
#define BOARD_7         3

#define TARGET      BOARD_5




#if (TARGET == BOARD_2_4)


/***********  SCREEN Params ********************/
#define LCD_H_RES                  240
#define LCD_V_RES                  320
#define LCD_PIXEL_CLOCK_HZ         (20 * 1000 * 1000)

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

/*********** SCREEN GPIO  *****************/
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

/*********** BACKLIGHT ************************/
#define LCD_BK_LIGHT_ON_LEVEL      1
#define LCD_BK_LIGHT_OFF_LEVEL     !LCD_BK_LIGHT_ON_LEVEL
#define LCD_PIN_NUM_BK_LIGHT       27

/*********** TOUCH GPIO AND CONFIG*********/
#define LCD_PIN_NUM_TOUCH_CS       15
#define PIN_NUM_TOUCH_CS           33

#elif (TARGET == BOARD_5)
/*********** SCREEN Params ********************/
#define LCD_H_RES                  800
#define LCD_V_RES                  480
#define LCD_PIXEL_CLOCK_HZ         (16 * 1000 * 1000)

#define LCD_BIT_PER_PIXEL           (16)
#define LCD_RGB_BIT_PER_PIXEL       (16)
#define LCD_RGB_DATA_WIDTH          (16)
#define LCD_RGB_BOUNCE_BUFFER_SIZE  (LCD_H_RES * CONFIG_LCD_RGB_BOUNCE_BUFFER_HEIGHT)

#define LVGL_DRAW_BUF_LINES    20 // number of display lines in each draw buffer
#define LVGL_TICK_PERIOD_MS    2
#define LVGL_TASK_MAX_DELAY_MS 500
#define LVGL_TASK_MIN_DELAY_MS 1000 / CONFIG_FREERTOS_HZ
#define LVGL_TASK_STACK_SIZE   (4 * 1024)
#define LVGL_TASK_PRIORITY     2
#define LVGL_TASK_STACK_SIZE   (4 * 1024)
#define LVGL_TASK_PRIORITY     2
#define CONFIG_LCD_MIRROR      1

/*********** SCREEN GPIO  *********************/
#define LCD_IO_RGB_VSYNC        (GPIO_NUM_3)
#define LCD_IO_RGB_HSYNC        (GPIO_NUM_46)
#define LCD_IO_RGB_DE           (GPIO_NUM_5)
#define LCD_IO_RGB_PCLK         (GPIO_NUM_7)
#define LCD_IO_RGB_DATA0        (GPIO_NUM_14)
#define LCD_IO_RGB_DATA1        (GPIO_NUM_38)
#define LCD_IO_RGB_DATA2        (GPIO_NUM_18)
#define LCD_IO_RGB_DATA3        (GPIO_NUM_17)
#define LCD_IO_RGB_DATA4        (GPIO_NUM_10)
#define LCD_IO_RGB_DATA5        (GPIO_NUM_39)
#define LCD_IO_RGB_DATA6        (GPIO_NUM_0)
#define LCD_IO_RGB_DATA7        (GPIO_NUM_45)
#define LCD_IO_RGB_DATA8        (GPIO_NUM_48)
#define LCD_IO_RGB_DATA9        (GPIO_NUM_47)
#define LCD_IO_RGB_DATA10       (GPIO_NUM_21)
#define LCD_IO_RGB_DATA11       (GPIO_NUM_1)
#define LCD_IO_RGB_DATA12       (GPIO_NUM_2)
#define LCD_IO_RGB_DATA13       (GPIO_NUM_42)
#define LCD_IO_RGB_DATA14       (GPIO_NUM_41)
#define LCD_IO_RGB_DATA15       (GPIO_NUM_40)
#define LCD_IO_RST              (-1)

/*********** BACKLIGHT ************************/
#define LCD_BK_LIGHT_ON_LEVEL   (1)
#define LCD_BK_LIGHT_OFF_LEVEL  !LCD_BK_LIGHT_ON_LEVEL
#define LCD_PIN_NUM_BK_LIGHT    (-1)    

/*********** TOUCH GPIO AND CONFIG ************/
#define I2C_MASTER_SCL_IO           9       
#define I2C_MASTER_SDA_IO           8       
#define I2C_MASTER_NUM              0       
#define I2C_MASTER_FREQ_HZ          400000   
#define I2C_MASTER_TX_BUF_DISABLE   0        
#define I2C_MASTER_RX_BUF_DISABLE   0        
#define I2C_MASTER_TIMEOUT_MS       1000
#define LCD_PIN_NUM_TOUCH_RST       (-1) 
#define LCD_PIN_NUM_TOUCH_INT       (-1) 
#define CONFIG_LCD_TOUCH_CONTROLLER_GT911

#elif (TARGET == BOARD_7)
/*********** SCREEN Params ********************/
/*********** SCREEN GPIO  *********************/
/*********** BACKLIGHT ************************/
/*********** TOUCH GPIO AND CONFIG ************/

#else
#error "No valid board selected"
#endif


#endif