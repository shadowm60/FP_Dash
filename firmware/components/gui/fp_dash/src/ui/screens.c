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



typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main_calib,
    tick_screen_main_calib_1,
    tick_screen_main_calib_2,
    tick_screen_main_calib_3,
    tick_screen_main_calib_4,
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
}
