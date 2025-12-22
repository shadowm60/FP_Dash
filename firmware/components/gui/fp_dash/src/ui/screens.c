#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;
uint32_t active_theme_index = 0;

static void event_handler_unchecked_cb_system_screen_fixed_brightness(lv_event_t *e) {
    lv_obj_t *ta = lv_event_get_target(e);
    if (!lv_obj_has_state(ta, LV_STATE_CHECKED)) {
        action_fixed_brightess(e);
    }
}

static void event_handler_checked_cb_system_screen_fixed_brightness(lv_event_t *e) {
    lv_obj_t *ta = lv_event_get_target(e);
    if (lv_obj_has_state(ta, LV_STATE_CHECKED)) {
        action_fixed_brightess(e);
    }
}

static void event_handler_checked_cb_system_screen_use_sd(lv_event_t *e) {
    lv_obj_t *ta = lv_event_get_target(e);
    if (lv_obj_has_state(ta, LV_STATE_CHECKED)) {
        action_use_sd_logging(e);
    }
}

static void event_handler_unchecked_cb_system_screen_use_sd(lv_event_t *e) {
    lv_obj_t *ta = lv_event_get_target(e);
    if (!lv_obj_has_state(ta, LV_STATE_CHECKED)) {
        action_use_sd_logging(e);
    }
}

static void event_handler_checked_cb_system_screen_log_engine_param(lv_event_t *e) {
    lv_obj_t *ta = lv_event_get_target(e);
    if (lv_obj_has_state(ta, LV_STATE_CHECKED)) {
        action_log_eng_params(e);
    }
}

static void event_handler_unchecked_cb_system_screen_log_engine_param(lv_event_t *e) {
    lv_obj_t *ta = lv_event_get_target(e);
    if (!lv_obj_has_state(ta, LV_STATE_CHECKED)) {
        action_log_eng_params(e);
    }
}

static void event_handler_checked_cb_system_screen_ts_is_calibrated(lv_event_t *e) {
    lv_obj_t *ta = lv_event_get_target(e);
    if (lv_obj_has_state(ta, LV_STATE_CHECKED)) {
        action_ts_is_calibrated(e);
    }
}

static void event_handler_unchecked_cb_system_screen_ts_is_calibrated(lv_event_t *e) {
    lv_obj_t *ta = lv_event_get_target(e);
    if (!lv_obj_has_state(ta, LV_STATE_CHECKED)) {
        action_ts_is_calibrated(e);
    }
}

static void event_handler_checked_cb_system_screen_use_status_led(lv_event_t *e) {
    lv_obj_t *ta = lv_event_get_target(e);
    if (lv_obj_has_state(ta, LV_STATE_CHECKED)) {
        action_use_led_status(e);
    }
}

static void event_handler_unchecked_cb_system_screen_use_status_led(lv_event_t *e) {
    lv_obj_t *ta = lv_event_get_target(e);
    if (!lv_obj_has_state(ta, LV_STATE_CHECKED)) {
        action_use_led_status(e);
    }
}

void create_screen_main_calib() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main_calib = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_add_event_cb(obj, action_calib_handler, LV_EVENT_PRESSED, (void *)0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj0 = obj;
            lv_obj_set_pos(obj, 64, 77);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Calibrate Touch");
        }
        {
            // p_top_left
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_top_left = obj;
            lv_obj_set_pos(obj, 15, 15);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_event_cb(obj, action_calib_handle_top_left, LV_EVENT_PRESSED, (void *)0);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // p_top_right
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_top_right = obj;
            lv_obj_set_pos(obj, 195, 15);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // p_bottom_right
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_bottom_right = obj;
            lv_obj_set_pos(obj, 195, 275);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // p_bottom_left
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_bottom_left = obj;
            lv_obj_set_pos(obj, 15, 275);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // p_center
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_center = obj;
            lv_obj_set_pos(obj, 105, 145);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_main_calib();
}

void tick_screen_main_calib() {
}

void create_screen_main_calib_1() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main_calib_1 = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_add_event_cb(obj, action_calib_handler, LV_EVENT_PRESSED, (void *)0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj1 = obj;
            lv_obj_set_pos(obj, 64, 77);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Calibrate Touch");
        }
        {
            // p_top_left_1
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_top_left_1 = obj;
            lv_obj_set_pos(obj, 15, 15);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_event_cb(obj, action_calib_handle_top_left, LV_EVENT_PRESSED, (void *)0);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // p_top_right_1
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_top_right_1 = obj;
            lv_obj_set_pos(obj, 195, 15);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_event_cb(obj, action_calib_handle_top_right, LV_EVENT_PRESSED, (void *)0);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // p_bottom_right_1
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_bottom_right_1 = obj;
            lv_obj_set_pos(obj, 195, 275);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // p_bottom_left_1
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_bottom_left_1 = obj;
            lv_obj_set_pos(obj, 15, 275);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // p_center_1
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_center_1 = obj;
            lv_obj_set_pos(obj, 105, 145);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_main_calib_1();
}

void tick_screen_main_calib_1() {
}

void create_screen_main_calib_2() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main_calib_2 = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_add_event_cb(obj, action_calib_handler, LV_EVENT_PRESSED, (void *)0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj2 = obj;
            lv_obj_set_pos(obj, 64, 77);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Calibrate Touch");
        }
        {
            // p_top_left_2
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_top_left_2 = obj;
            lv_obj_set_pos(obj, 15, 15);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_event_cb(obj, action_calib_handle_top_left, LV_EVENT_PRESSED, (void *)0);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // p_top_right_2
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_top_right_2 = obj;
            lv_obj_set_pos(obj, 195, 15);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_event_cb(obj, action_calib_handle_top_right, LV_EVENT_PRESSED, (void *)0);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // p_bottom_right_2
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_bottom_right_2 = obj;
            lv_obj_set_pos(obj, 195, 275);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // p_bottom_left_2
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_bottom_left_2 = obj;
            lv_obj_set_pos(obj, 15, 275);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_event_cb(obj, action_calib_handler_bottom_left, LV_EVENT_PRESSED, (void *)0);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // p_center_2
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_center_2 = obj;
            lv_obj_set_pos(obj, 105, 145);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_main_calib_2();
}

void tick_screen_main_calib_2() {
}

void create_screen_main_calib_3() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main_calib_3 = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_add_event_cb(obj, action_calib_handler, LV_EVENT_PRESSED, (void *)0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj3 = obj;
            lv_obj_set_pos(obj, 64, 77);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Calibrate Touch");
        }
        {
            // p_top_left_3
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_top_left_3 = obj;
            lv_obj_set_pos(obj, 15, 15);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_event_cb(obj, action_calib_handle_top_left, LV_EVENT_PRESSED, (void *)0);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // p_top_right_3
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_top_right_3 = obj;
            lv_obj_set_pos(obj, 195, 15);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_event_cb(obj, action_calib_handle_top_right, LV_EVENT_PRESSED, (void *)0);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // p_bottom_right_3
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_bottom_right_3 = obj;
            lv_obj_set_pos(obj, 195, 275);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_event_cb(obj, action_calib_handle_bottom_right, LV_EVENT_PRESSED, (void *)0);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // p_bottom_left_3
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_bottom_left_3 = obj;
            lv_obj_set_pos(obj, 15, 275);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_event_cb(obj, action_calib_handler_bottom_left, LV_EVENT_PRESSED, (void *)0);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // p_center_3
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_center_3 = obj;
            lv_obj_set_pos(obj, 105, 145);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_main_calib_3();
}

void tick_screen_main_calib_3() {
}

void create_screen_main_calib_4() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main_calib_4 = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_add_event_cb(obj, action_calib_handler, LV_EVENT_PRESSED, (void *)0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj4 = obj;
            lv_obj_set_pos(obj, 64, 77);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Calibrate Touch");
        }
        {
            // p_top_left_4
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_top_left_4 = obj;
            lv_obj_set_pos(obj, 15, 15);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_event_cb(obj, action_calib_handle_top_left, LV_EVENT_PRESSED, (void *)0);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // p_top_right_4
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_top_right_4 = obj;
            lv_obj_set_pos(obj, 195, 15);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_event_cb(obj, action_calib_handle_top_right, LV_EVENT_PRESSED, (void *)0);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // p_bottom_right_4
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_bottom_right_4 = obj;
            lv_obj_set_pos(obj, 195, 275);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_event_cb(obj, action_calib_handle_bottom_right, LV_EVENT_PRESSED, (void *)0);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // p_bottom_left_4
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_bottom_left_4 = obj;
            lv_obj_set_pos(obj, 15, 275);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_event_cb(obj, action_calib_handler_bottom_left, LV_EVENT_PRESSED, (void *)0);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // p_center_4
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.p_center_4 = obj;
            lv_obj_set_pos(obj, 105, 145);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            static lv_point_precise_t line_points[] = {
                { 0, 15 },
                { 30, 15 },
                { 15, 15 },
                { 15, 0 },
                { 15, 30 }
            };
            lv_line_set_points(obj, line_points, 5);
            lv_obj_add_event_cb(obj, action_calib_handler_center, LV_EVENT_PRESSED, (void *)0);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_main_calib_4();
}

void tick_screen_main_calib_4() {
}

void create_screen_startup_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.startup_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj5 = obj;
            lv_obj_set_pos(obj, 11, 17);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Communication:");
        }
        {
            // com_button
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.com_button = obj;
            lv_obj_set_pos(obj, 141, 13);
            lv_obj_set_size(obj, 89, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // com_button_text
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.com_button_text = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "UDS");
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj6 = obj;
            lv_obj_set_pos(obj, 11, 52);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Status: ");
        }
        {
            // status_button
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.status_button = obj;
            lv_obj_set_pos(obj, 75, 48);
            lv_obj_set_size(obj, 155, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // status_button_text
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.status_button_text = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Not Connected");
                }
            }
        }
        {
            // read_dtcs_button
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.read_dtcs_button = obj;
            lv_obj_set_pos(obj, 11, 92);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_add_event_cb(obj, action_read_dtcs_screen_selected, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Read DTCs");
                }
            }
        }
        {
            // clear_dtcs_button
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.clear_dtcs_button = obj;
            lv_obj_set_pos(obj, 130, 92);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_add_event_cb(obj, action_clear_dtcs_screen_selected, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Clear DTCs");
                }
            }
        }
        {
            // live_data_button
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.live_data_button = obj;
            lv_obj_set_pos(obj, 11, 160);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_add_event_cb(obj, action_live_data_screen_selected, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Live Data");
                }
            }
        }
        {
            // settings_button
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.settings_button = obj;
            lv_obj_set_pos(obj, 11, 226);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_add_event_cb(obj, action_settings_screen_selected, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Settings");
                }
            }
        }
        {
            // freez_frame_data
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.freez_frame_data = obj;
            lv_obj_set_pos(obj, 130, 160);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_add_event_cb(obj, action_freez_frame_screen_selected, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Freez Frame");
                }
            }
        }
        {
            // system_button
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.system_button = obj;
            lv_obj_set_pos(obj, 130, 226);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_add_event_cb(obj, action_system_screen_selected, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "System");
                }
            }
        }
    }
    
    tick_screen_startup_screen();
}

void tick_screen_startup_screen() {
}

void create_screen_read_dtcs_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.read_dtcs_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_add_event_cb(obj, action_gui_handler_gesture, LV_EVENT_GESTURE, (void *)7);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // read_dtcs_start
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.read_dtcs_start = obj;
            lv_obj_set_pos(obj, 11, 10);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_add_event_cb(obj, action_read_dtcs_start_action, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Read");
                }
            }
        }
        {
            // clear_dtcs_start
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.clear_dtcs_start = obj;
            lv_obj_set_pos(obj, 130, 10);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_add_event_cb(obj, action_clear_dtcs_clear_start, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Clear");
                }
            }
        }
        {
            // dtc_text_area
            lv_obj_t *obj = lv_textarea_create(parent_obj);
            objects.dtc_text_area = obj;
            lv_obj_set_pos(obj, 12, 74);
            lv_obj_set_size(obj, 218, 210);
            lv_textarea_set_max_length(obj, 128);
            lv_textarea_set_text(obj, "P0048 - test");
            lv_textarea_set_one_line(obj, false);
            lv_textarea_set_password_mode(obj, false);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        }
    }
    
    tick_screen_read_dtcs_screen();
}

void tick_screen_read_dtcs_screen() {
}

void create_screen_clear_dtcs_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.clear_dtcs_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_add_event_cb(obj, action_gui_handler_gesture, LV_EVENT_GESTURE, (void *)8);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // clear_dtcs_start_1
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.clear_dtcs_start_1 = obj;
            lv_obj_set_pos(obj, 12, 10);
            lv_obj_set_size(obj, 218, 50);
            lv_obj_add_event_cb(obj, action_clear_dtcs_clear_start, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Clear");
                }
            }
        }
        {
            // clear_dtc_text_area
            lv_obj_t *obj = lv_textarea_create(parent_obj);
            objects.clear_dtc_text_area = obj;
            lv_obj_set_pos(obj, 12, 74);
            lv_obj_set_size(obj, 218, 210);
            lv_textarea_set_max_length(obj, 128);
            lv_textarea_set_text(obj, "Clearing codes ...");
            lv_textarea_set_one_line(obj, false);
            lv_textarea_set_password_mode(obj, false);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        }
    }
    
    tick_screen_clear_dtcs_screen();
}

void tick_screen_clear_dtcs_screen() {
}

void create_screen_live_data_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.live_data_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_add_event_cb(obj, action_gui_handler_gesture, LV_EVENT_GESTURE, (void *)9);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj7 = obj;
            lv_obj_set_pos(obj, 71, 11);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "live_data_screen");
        }
    }
    
    tick_screen_live_data_screen();
}

void tick_screen_live_data_screen() {
}

void create_screen_freez_frame_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.freez_frame_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_add_event_cb(obj, action_gui_handler_gesture, LV_EVENT_GESTURE, (void *)10);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj8 = obj;
            lv_obj_set_pos(obj, 48, 13);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "freez_frame_screen");
        }
    }
    
    tick_screen_freez_frame_screen();
}

void tick_screen_freez_frame_screen() {
}

void create_screen_settings_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.settings_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_add_event_cb(obj, action_gui_handler_gesture, LV_EVENT_GESTURE, (void *)11);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj9 = obj;
            lv_obj_set_pos(obj, 14, 21);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Data Source:");
        }
        {
            // data_source_select
            lv_obj_t *obj = lv_dropdown_create(parent_obj);
            objects.data_source_select = obj;
            lv_obj_set_pos(obj, 120, 11);
            lv_obj_set_size(obj, 109, LV_SIZE_CONTENT);
            lv_dropdown_set_options(obj, "OBD2\nRusEFI\nHaltech");
            lv_dropdown_set_selected(obj, 0);
            lv_obj_add_event_cb(obj, action_dropdown_value_changed, LV_EVENT_VALUE_CHANGED, (void *)0);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj10 = obj;
            lv_obj_set_pos(obj, 14, 64);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Base ID:");
        }
        {
            lv_obj_t *obj = lv_spinbox_create(parent_obj);
            lv_obj_set_pos(obj, 120, 60);
            lv_obj_set_size(obj, 109, 30);
            lv_spinbox_set_digit_format(obj, 5, 0);
            lv_spinbox_set_range(obj, 0, 99999);
            lv_spinbox_set_rollover(obj, false);
            lv_spinbox_set_step(obj, 1);
            lv_spinbox_set_value(obj, 512);
        }
    }
    
    tick_screen_settings_screen();
}

void tick_screen_settings_screen() {
}

void create_screen_system_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.system_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_add_event_cb(obj, action_gui_handler_gesture, LV_EVENT_GESTURE, (void *)12);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // fixed_brightness
            lv_obj_t *obj = lv_checkbox_create(parent_obj);
            objects.fixed_brightness = obj;
            lv_obj_set_pos(obj, 13, 11);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_checkbox_set_text(obj, "Fixed Brightness");
            lv_obj_add_event_cb(obj, event_handler_unchecked_cb_system_screen_fixed_brightness, LV_EVENT_VALUE_CHANGED, (void *)0);
            lv_obj_add_event_cb(obj, event_handler_checked_cb_system_screen_fixed_brightness, LV_EVENT_VALUE_CHANGED, (void *)1);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // brightness_slide
            lv_obj_t *obj = lv_slider_create(parent_obj);
            objects.brightness_slide = obj;
            lv_obj_set_pos(obj, 13, 46);
            lv_obj_set_size(obj, 213, 10);
            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
            lv_obj_add_event_cb(obj, action_brightness_slider, LV_EVENT_RELEASED, (void *)0);
        }
        {
            // use_sd
            lv_obj_t *obj = lv_checkbox_create(parent_obj);
            objects.use_sd = obj;
            lv_obj_set_pos(obj, 13, 72);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_checkbox_set_text(obj, "Use SD_Logging");
            lv_obj_add_event_cb(obj, event_handler_checked_cb_system_screen_use_sd, LV_EVENT_VALUE_CHANGED, (void *)1);
            lv_obj_add_event_cb(obj, event_handler_unchecked_cb_system_screen_use_sd, LV_EVENT_VALUE_CHANGED, (void *)0);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // log_engine_param
            lv_obj_t *obj = lv_checkbox_create(parent_obj);
            objects.log_engine_param = obj;
            lv_obj_set_pos(obj, 13, 110);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_checkbox_set_text(obj, "Log Engine Parameters");
            lv_obj_add_event_cb(obj, event_handler_checked_cb_system_screen_log_engine_param, LV_EVENT_VALUE_CHANGED, (void *)1);
            lv_obj_add_event_cb(obj, event_handler_unchecked_cb_system_screen_log_engine_param, LV_EVENT_VALUE_CHANGED, (void *)0);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // ts_is_calibrated
            lv_obj_t *obj = lv_checkbox_create(parent_obj);
            objects.ts_is_calibrated = obj;
            lv_obj_set_pos(obj, 13, 285);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_checkbox_set_text(obj, "TS is calibrated");
            lv_obj_add_event_cb(obj, event_handler_checked_cb_system_screen_ts_is_calibrated, LV_EVENT_VALUE_CHANGED, (void *)1);
            lv_obj_add_event_cb(obj, event_handler_unchecked_cb_system_screen_ts_is_calibrated, LV_EVENT_VALUE_CHANGED, (void *)0);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // use_status_led
            lv_obj_t *obj = lv_checkbox_create(parent_obj);
            objects.use_status_led = obj;
            lv_obj_set_pos(obj, 13, 150);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_checkbox_set_text(obj, "Use LED status");
            lv_obj_add_event_cb(obj, event_handler_checked_cb_system_screen_use_status_led, LV_EVENT_VALUE_CHANGED, (void *)1);
            lv_obj_add_event_cb(obj, event_handler_unchecked_cb_system_screen_use_status_led, LV_EVENT_VALUE_CHANGED, (void *)0);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_system_screen();
}

void tick_screen_system_screen() {
}



typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main_calib,
    tick_screen_main_calib_1,
    tick_screen_main_calib_2,
    tick_screen_main_calib_3,
    tick_screen_main_calib_4,
    tick_screen_startup_screen,
    tick_screen_read_dtcs_screen,
    tick_screen_clear_dtcs_screen,
    tick_screen_live_data_screen,
    tick_screen_freez_frame_screen,
    tick_screen_settings_screen,
    tick_screen_system_screen,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main_calib();
    create_screen_main_calib_1();
    create_screen_main_calib_2();
    create_screen_main_calib_3();
    create_screen_main_calib_4();
    create_screen_startup_screen();
    create_screen_read_dtcs_screen();
    create_screen_clear_dtcs_screen();
    create_screen_live_data_screen();
    create_screen_freez_frame_screen();
    create_screen_settings_screen();
    create_screen_system_screen();
}
