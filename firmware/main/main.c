#include <stdio.h>
#include <unistd.h>
#include <sys/lock.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lcd.h"
#include "can.h"



static const char *TAG = "FP_Dash_main";

void app_main( void ) {

    ESP_LOGI(TAG, "Setup LCD");
    setup_LCD_Panel();

    init_can();
    init_can_tasks();


}