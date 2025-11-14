#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main_calib;
    lv_obj_t *main_calib_1;
    lv_obj_t *main_calib_2;
    lv_obj_t *main_calib_3;
    lv_obj_t *main_calib_4;
    lv_obj_t *startup_screen;
    lv_obj_t *read_dtcs_screen;
    lv_obj_t *clear_dtcs_screen;
    lv_obj_t *live_data_screen;
    lv_obj_t *freez_frame_screen;
    lv_obj_t *settings_screen;
    lv_obj_t *system_screen;
    lv_obj_t *obj0;
    lv_obj_t *p_top_left;
    lv_obj_t *p_top_right;
    lv_obj_t *p_bottom_right;
    lv_obj_t *p_bottom_left;
    lv_obj_t *p_center;
    lv_obj_t *obj1;
    lv_obj_t *p_top_left_1;
    lv_obj_t *p_top_right_1;
    lv_obj_t *p_bottom_right_1;
    lv_obj_t *p_bottom_left_1;
    lv_obj_t *p_center_1;
    lv_obj_t *obj2;
    lv_obj_t *p_top_left_2;
    lv_obj_t *p_top_right_2;
    lv_obj_t *p_bottom_right_2;
    lv_obj_t *p_bottom_left_2;
    lv_obj_t *p_center_2;
    lv_obj_t *obj3;
    lv_obj_t *p_top_left_3;
    lv_obj_t *p_top_right_3;
    lv_obj_t *p_bottom_right_3;
    lv_obj_t *p_bottom_left_3;
    lv_obj_t *p_center_3;
    lv_obj_t *obj4;
    lv_obj_t *p_top_left_4;
    lv_obj_t *p_top_right_4;
    lv_obj_t *p_bottom_right_4;
    lv_obj_t *p_bottom_left_4;
    lv_obj_t *p_center_4;
    lv_obj_t *obj5;
    lv_obj_t *com_button;
    lv_obj_t *com_button_text;
    lv_obj_t *obj6;
    lv_obj_t *status_button;
    lv_obj_t *status_button_text;
    lv_obj_t *read_dtcs_button;
    lv_obj_t *clear_dtcs_button;
    lv_obj_t *live_data_button;
    lv_obj_t *settings_button;
    lv_obj_t *freez_frame_data;
    lv_obj_t *system_button;
    lv_obj_t *read_dtcs_start;
    lv_obj_t *clear_dtcs_start;
    lv_obj_t *dtc_text_area;
    lv_obj_t *clear_dtcs_start_1;
    lv_obj_t *clear_dtc_text_area;
    lv_obj_t *obj7;
    lv_obj_t *obj8;
    lv_obj_t *obj9;
    lv_obj_t *obj10;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN_CALIB = 1,
    SCREEN_ID_MAIN_CALIB_1 = 2,
    SCREEN_ID_MAIN_CALIB_2 = 3,
    SCREEN_ID_MAIN_CALIB_3 = 4,
    SCREEN_ID_MAIN_CALIB_4 = 5,
    SCREEN_ID_STARTUP_SCREEN = 6,
    SCREEN_ID_READ_DTCS_SCREEN = 7,
    SCREEN_ID_CLEAR_DTCS_SCREEN = 8,
    SCREEN_ID_LIVE_DATA_SCREEN = 9,
    SCREEN_ID_FREEZ_FRAME_SCREEN = 10,
    SCREEN_ID_SETTINGS_SCREEN = 11,
    SCREEN_ID_SYSTEM_SCREEN = 12,
};

void create_screen_main_calib();
void tick_screen_main_calib();

void create_screen_main_calib_1();
void tick_screen_main_calib_1();

void create_screen_main_calib_2();
void tick_screen_main_calib_2();

void create_screen_main_calib_3();
void tick_screen_main_calib_3();

void create_screen_main_calib_4();
void tick_screen_main_calib_4();

void create_screen_startup_screen();
void tick_screen_startup_screen();

void create_screen_read_dtcs_screen();
void tick_screen_read_dtcs_screen();

void create_screen_clear_dtcs_screen();
void tick_screen_clear_dtcs_screen();

void create_screen_live_data_screen();
void tick_screen_live_data_screen();

void create_screen_freez_frame_screen();
void tick_screen_freez_frame_screen();

void create_screen_settings_screen();
void tick_screen_settings_screen();

void create_screen_system_screen();
void tick_screen_system_screen();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/