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

/////////////// actions not part of calib
void action_read_dtcs_start_action(lv_event_t * e) {
    ESP_LOGI(TAG, "start read dtcs!");
}

void action_clear_dtcs_clear_start(lv_event_t * e) {
    ESP_LOGI(TAG, "start clear dtcs!");
}

void action_read_dtcs_screen_selected(lv_event_t * e) {
    ESP_LOGI(TAG, "read dtc screen selected");
    loadScreen(SCREEN_ID_READ_DTCS_SCREEN);
}

void action_clear_dtcs_screen_selected(lv_event_t * e) {
    ESP_LOGI(TAG, "clear dtc screen selected");
    loadScreen(SCREEN_ID_CLEAR_DTCS_SCREEN);
}

void action_live_data_screen_selected(lv_event_t * e) {
    ESP_LOGI(TAG, "live data screen selected");
    loadScreen(SCREEN_ID_LIVE_DATA_SCREEN);
}

void action_freez_frame_screen_selected(lv_event_t * e) {
    ESP_LOGI(TAG, "freez frame screen selected");
    loadScreen(SCREEN_ID_FREEZ_FRAME_SCREEN);
}

void action_settings_screen_selected(lv_event_t * e) {
    ESP_LOGI(TAG, "settings screen selected");
    loadScreen(SCREEN_ID_SETTINGS_SCREEN);
}

void action_system_screen_selected(lv_event_t * e) {
    ESP_LOGI(TAG, "system screen selected");
    loadScreen(SCREEN_ID_SYSTEM_SCREEN);
}

void action_gui_handler_gesture(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    lv_indev_t * indev = lv_event_get_indev(e);
    void * user_data = lv_event_get_user_data(e);
    int id = (int)(intptr_t)user_data;

    ESP_LOGI(TAG, "Event code: %d", code);
    ESP_LOGI(TAG, "Target object: %p", target);
    ESP_LOGI(TAG, "Input device: %p", indev);
    ESP_LOGI(TAG, "User data: %p", user_data);

    // If gesture-specific, you might want to log gesture direction
    if (code == LV_EVENT_GESTURE) {
        lv_dir_t dir = lv_indev_get_gesture_dir(indev);
        ESP_LOGI(TAG, "Gesture direction: %d", dir);

        switch (id) {
            case SCREEN_ID_STARTUP_SCREEN:
                if (dir == LV_DIR_LEFT) {

                } else if (dir == LV_DIR_RIGHT) {

                } else if (dir == LV_DIR_BOTTOM) {

                } else if (dir == LV_DIR_TOP) {

                } else {
                    ESP_LOGI(TAG, "Gesture direction: unhandled");
                }
            break;
            case SCREEN_ID_READ_DTCS_SCREEN:
                if (dir == LV_DIR_LEFT) {
                    loadScreen(SCREEN_ID_STARTUP_SCREEN);
                } else if (dir == LV_DIR_RIGHT) {
                    loadScreen(SCREEN_ID_CLEAR_DTCS_SCREEN);
                } else if (dir == LV_DIR_BOTTOM) {
                    loadScreen(SCREEN_ID_STARTUP_SCREEN);
                } else if (dir == LV_DIR_TOP) {
                    loadScreen(SCREEN_ID_LIVE_DATA_SCREEN);
                } else {
                    ESP_LOGI(TAG, "Gesture direction: unhandled");
                }            
            break;
            case SCREEN_ID_CLEAR_DTCS_SCREEN:
                if (dir == LV_DIR_LEFT) {
                    loadScreen(SCREEN_ID_READ_DTCS_SCREEN);
                } else if (dir == LV_DIR_RIGHT) {
                    loadScreen(SCREEN_ID_LIVE_DATA_SCREEN);
                } else if (dir == LV_DIR_BOTTOM) {
                    loadScreen(SCREEN_ID_STARTUP_SCREEN);
                } else if (dir == LV_DIR_TOP) {
                    loadScreen(SCREEN_ID_LIVE_DATA_SCREEN);
                } else {
                    ESP_LOGI(TAG, "Gesture direction: unhandled");
                }            
            break;
            case SCREEN_ID_LIVE_DATA_SCREEN:
                if (dir == LV_DIR_LEFT) {
                    loadScreen(SCREEN_ID_CLEAR_DTCS_SCREEN);
                } else if (dir == LV_DIR_RIGHT) {
                    loadScreen(SCREEN_ID_FREEZ_FRAME_SCREEN);
                } else if (dir == LV_DIR_BOTTOM) {
                    loadScreen(SCREEN_ID_STARTUP_SCREEN);
                } else if (dir == LV_DIR_TOP) {
                    loadScreen(SCREEN_ID_LIVE_DATA_SCREEN);
                } else {
                    ESP_LOGI(TAG, "Gesture direction: unhandled");
                }             
            break;
            case SCREEN_ID_FREEZ_FRAME_SCREEN:
                if (dir == LV_DIR_LEFT) {
                    loadScreen(SCREEN_ID_LIVE_DATA_SCREEN);
                } else if (dir == LV_DIR_RIGHT) {
                    loadScreen(SCREEN_ID_SETTINGS_SCREEN);
                } else if (dir == LV_DIR_BOTTOM) {
                    loadScreen(SCREEN_ID_STARTUP_SCREEN);
                } else if (dir == LV_DIR_TOP) {
                    loadScreen(SCREEN_ID_LIVE_DATA_SCREEN);
                } else {
                    ESP_LOGI(TAG, "Gesture direction: unhandled");
                }             
            break;
            case SCREEN_ID_SETTINGS_SCREEN:
                if (dir == LV_DIR_LEFT) {
                    loadScreen(SCREEN_ID_FREEZ_FRAME_SCREEN);
                } else if (dir == LV_DIR_RIGHT) {
                    loadScreen(SCREEN_ID_SYSTEM_SCREEN);
                } else if (dir == LV_DIR_BOTTOM) {
                    loadScreen(SCREEN_ID_STARTUP_SCREEN);
                } else if (dir == LV_DIR_TOP) {
                    loadScreen(SCREEN_ID_LIVE_DATA_SCREEN);
                } else {
                    ESP_LOGI(TAG, "Gesture direction: unhandled");
                }             
            break;
            case SCREEN_ID_SYSTEM_SCREEN:
                if (dir == LV_DIR_LEFT) {
                    loadScreen(SCREEN_ID_SETTINGS_SCREEN);
                } else if (dir == LV_DIR_RIGHT) {
                    loadScreen(SCREEN_ID_SYSTEM_SCREEN);
                } else if (dir == LV_DIR_BOTTOM) {
                    loadScreen(SCREEN_ID_STARTUP_SCREEN);
                } else if (dir == LV_DIR_TOP) {
                    loadScreen(SCREEN_ID_LIVE_DATA_SCREEN);
                } else {
                    ESP_LOGI(TAG, "Gesture direction: unhandled");
                }             
            break;

        }
    }
}
