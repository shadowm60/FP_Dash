#include "esp_err.h"
#include "esp_log.h"

#include "lvgl.h"
#include "screens.h"
#include "ui.h"

static const char *TAG = "GUI Handler";
static uint16_t last_x, last_y;
static uint8_t calib_state = SCREEN_ID_MAIN_CALIB_1;

void gui_handler_store_last_point(uint16_t x, uint16_t y ) {
    last_x = x;
    last_y = y;
}


// e == LV_EVENT_PRESSED
void action_calib_handler(lv_event_t * e) {
    ESP_LOGI(TAG, "action_calib_handler x: %d, y: %d", last_x, last_y);

    loadScreen(calib_state);
    calib_state++;
    if (calib_state > SCREEN_ID_MAIN_CALIB_4) {
        ESP_LOGI(TAG, "last screen");
    }
}

//These might not work if we do not have a calibrated touch
void action_calib_handle_top_left(lv_event_t * e) {
    //loadScreen(SCREEN_ID_MAIN_CALIB_1);
}

void action_calib_handle_top_right(lv_event_t * e) {
    //loadScreen(SCREEN_ID_MAIN_CALIB_2);
}

void action_calib_handler_bottom_left(lv_event_t * e) {
    //loadScreen(SCREEN_ID_MAIN_CALIB_3);
}

void action_calib_handle_bottom_right(lv_event_t * e) {
    //loadScreen(SCREEN_ID_MAIN_CALIB_4);
}

void action_calib_handler_center(lv_event_t * e) {
    ESP_LOGI(TAG, "last screen");
}