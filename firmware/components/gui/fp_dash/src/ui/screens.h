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
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN_CALIB = 1,
    SCREEN_ID_MAIN_CALIB_1 = 2,
    SCREEN_ID_MAIN_CALIB_2 = 3,
    SCREEN_ID_MAIN_CALIB_3 = 4,
    SCREEN_ID_MAIN_CALIB_4 = 5,
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

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/