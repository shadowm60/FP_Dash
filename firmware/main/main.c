#include <stdio.h>
#include <unistd.h>
#include <sys/lock.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lcd.h"
#include "can.h"
#include "led.h"



static const char *TAG = "FP_Dash_main";
static const char *TAG_DBG = "StackOverflowHook";

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    // Log the task name and handle
    ESP_LOGI(TAG_DBG, "Stack overflow detected!");
    ESP_LOGI(TAG_DBG, "Task Name: %s", pcTaskName ? pcTaskName : "Unknown");
    ESP_LOGI(TAG_DBG, "Task Handle: %p", (void *)xTask);

    // Optional: Get remaining stack space (if still valid)
    UBaseType_t highWaterMark = uxTaskGetStackHighWaterMark(xTask);
    ESP_LOGI(TAG_DBG, "High Water Mark (words): %u", highWaterMark);

    // Optional: Print current free heap for context
    ESP_LOGI(TAG_DBG, "Free Heap: %u bytes", (unsigned int)xPortGetFreeHeapSize());

    // Suggest a safe recovery action
    ESP_LOGI(TAG_DBG, "System will halt or reset to prevent undefined behavior.");

    // Halt or reset (recommended for safety)
    abort(); // or esp_restart();
}


void app_main( void ) {

    ESP_LOGI(TAG, "Setup LCD");
    setup_LCD_Panel();

    init_led_pins();

    init_can();
    init_can_tasks();


}